#include "events.hpp"
#include "system_state.hpp"

namespace event {

void take_option(sys::state& state, pending_human_n_event const& e, uint8_t opt) {
	for(auto i = state.pending_n_event.size(); i-- > 0;) {
		if(state.pending_n_event[i].date == e.date && state.pending_n_event[i].e == e.e &&
				state.pending_n_event[i].from_slot == e.from_slot && state.pending_n_event[i].n == e.n &&
				state.pending_n_event[i].primary_slot == e.primary_slot && state.pending_n_event[i].r_hi == e.r_hi &&
				state.pending_n_event[i].r_lo == e.r_lo) {

			if(opt > state.world.national_event_get_options(e.e).size() || !(state.world.national_event_get_options(e.e)[opt].name))
				return; // invalid option

			state.pending_n_event[i] = state.pending_n_event.back();
			state.pending_n_event.pop_back();

			effect::execute(state, state.world.national_event_get_options(e.e)[opt].effect, e.primary_slot, trigger::to_generic(e.n),
					e.from_slot, e.r_lo, e.r_hi);

			return;
		}
	}
}

void take_option(sys::state& state, pending_human_f_n_event const& e, uint8_t opt) {
	for(auto i = state.pending_f_n_event.size(); i-- > 0;) {
		if(state.pending_f_n_event[i].date == e.date && state.pending_f_n_event[i].e == e.e && state.pending_f_n_event[i].n == e.n &&
				state.pending_f_n_event[i].r_hi == e.r_hi && state.pending_f_n_event[i].r_lo == e.r_lo) {

			if(opt > state.world.free_national_event_get_options(e.e).size() ||
					!(state.world.free_national_event_get_options(e.e)[opt].name))
				return; // invalid option

			state.pending_f_n_event[i] = state.pending_f_n_event.back();
			state.pending_f_n_event.pop_back();

			effect::execute(state, state.world.free_national_event_get_options(e.e)[opt].effect, trigger::to_generic(e.n),
					trigger::to_generic(e.n), 0, e.r_lo, e.r_hi);

			return;
		}
	}
}

void take_option(sys::state& state, pending_human_p_event const& e, uint8_t opt) {
	for(auto i = state.pending_p_event.size(); i-- > 0;) {
		if(state.pending_p_event[i].date == e.date && state.pending_p_event[i].e == e.e &&
				state.pending_p_event[i].from_slot == e.from_slot && state.pending_p_event[i].p == e.p &&
				state.pending_p_event[i].r_hi == e.r_hi && state.pending_p_event[i].r_lo == e.r_lo) {

			if(opt > state.world.provincial_event_get_options(e.e).size() || !(state.world.provincial_event_get_options(e.e)[opt].name))
				return; // invalid option

			state.pending_p_event[i] = state.pending_p_event.back();
			state.pending_p_event.pop_back();

			effect::execute(state, state.world.provincial_event_get_options(e.e)[opt].effect, trigger::to_generic(e.p),
					trigger::to_generic(e.p), e.from_slot, e.r_lo, e.r_hi);

			return;
		}
	}
}
void take_option(sys::state& state, pending_human_f_p_event const& e, uint8_t opt) {
	for(auto i = state.pending_f_p_event.size(); i-- > 0;) {
		if(state.pending_f_p_event[i].date == e.date && state.pending_f_p_event[i].e == e.e && state.pending_f_p_event[i].p == e.p &&
				state.pending_f_p_event[i].r_hi == e.r_hi && state.pending_f_p_event[i].r_lo == e.r_lo) {

			if(opt > state.world.free_provincial_event_get_options(e.e).size() ||
					!(state.world.free_provincial_event_get_options(e.e)[opt].name))
				return; // invalid option

			state.pending_f_p_event[i] = state.pending_f_p_event.back();
			state.pending_f_p_event.pop_back();

			effect::execute(state, state.world.free_provincial_event_get_options(e.e)[opt].effect, trigger::to_generic(e.p),
					trigger::to_generic(e.p), -1, e.r_lo, e.r_hi);

			return;
		}
	}
}

void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_lo, uint32_t r_hi,
		int32_t primary_slot, slot_type pt, int32_t from_slot, slot_type ft) {
	if(ft == slot_type::province)
		assert(dcon::fatten(state.world, state.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot)))
							 .is_valid());

	if(auto immediate = state.world.national_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, primary_slot, trigger::to_generic(n), from_slot, r_lo, r_hi);
	}
	if(state.world.nation_get_is_player_controlled(n)) {
		pending_human_n_event new_event{r_lo, r_hi + 1, primary_slot, from_slot, e, n, state.current_date, pt, ft};
		state.pending_n_event.push_back(new_event);
		if(n == state.local_player_nation)
			state.new_n_event.push(new_event);
	} else {
		auto& opt = state.world.national_event_get_options(e);
		float total = 0.0f;
		float odds[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance && opt[i].effect) {
				odds[i] =
						trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, primary_slot, trigger::to_generic(n), from_slot);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ n.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance && opt[i].effect) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						effect::execute(state, opt[i].effect, primary_slot, trigger::to_generic(n), from_slot, r_lo, r_hi + 1);
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, primary_slot, trigger::to_generic(n), from_slot, r_lo, r_hi + 1);
		}

		// TODO: notify
	}
}
void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo,
		int32_t from_slot, slot_type ft) {
	trigger_national_event(state, e, n, r_hi, r_lo, trigger::to_generic(n), slot_type::nation, from_slot, ft);
}
void trigger_national_event(sys::state& state, dcon::free_national_event_id e, dcon::nation_id n, uint32_t r_lo, uint32_t r_hi) {
	if(state.world.free_national_event_get_only_once(e) && state.world.free_national_event_get_has_been_triggered(e))
		return;

	state.world.free_national_event_set_has_been_triggered(e, true);
	if(auto immediate = state.world.free_national_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi);
	}
	if(state.world.nation_get_is_player_controlled(n)) {
		pending_human_f_n_event new_event{r_lo, r_hi + 1, e, n, state.current_date};
		state.pending_f_n_event.push_back(new_event);
		if(n == state.local_player_nation)
			state.new_f_n_event.push(new_event);
	} else {
		auto& opt = state.world.free_national_event_get_options(e);
		float total = 0.0f;
		float odds[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance && opt[i].effect) {
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(n), trigger::to_generic(n), 0);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ n.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance && opt[i].effect) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						effect::execute(state, opt[i].effect, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi + 1);
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi + 1);
		}

		// TODO: notify
	}
}
void trigger_provincial_event(sys::state& state, dcon::provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo,
		int32_t from_slot, slot_type ft) {
	if(ft == slot_type::province)
		assert(dcon::fatten(state.world, state.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot))).is_valid());

	auto owner = state.world.province_get_nation_from_province_ownership(p);
	if(state.world.nation_get_is_player_controlled(owner)) {
		pending_human_p_event new_event{r_lo, r_hi, from_slot, e, p, state.current_date, ft};
		state.pending_p_event.push_back(new_event);
		if(owner == state.local_player_nation)
			state.new_p_event.push(new_event);
	} else {
		auto& opt = state.world.provincial_event_get_options(e);
		float total = 0.0f;
		float odds[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance && opt[i].effect) {
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(p),
						trigger::to_generic(p), from_slot);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ p.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance && opt[i].effect) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						effect::execute(state, opt[i].effect, trigger::to_generic(p), trigger::to_generic(p), from_slot, r_lo, r_hi);
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(p), trigger::to_generic(p), from_slot, r_lo, r_hi);
		}

		// TODO: notify
	}
}
void trigger_provincial_event(sys::state& state, dcon::free_provincial_event_id e, dcon::province_id p, uint32_t r_hi,
		uint32_t r_lo) {
	state.world.free_provincial_event_set_has_been_triggered(e, true);

	auto owner = state.world.province_get_nation_from_province_ownership(p);
	if(state.world.nation_get_is_player_controlled(owner)) {
		pending_human_f_p_event new_event{r_lo, r_hi, e, p, state.current_date};
		state.pending_f_p_event.push_back(new_event);
		if(owner == state.local_player_nation)
			state.new_f_p_event.push(new_event);
	} else {
		auto& opt = state.world.free_provincial_event_get_options(e);
		float total = 0.0f;
		float odds[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance && opt[i].effect) {
				odds[i] =
						trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(p), trigger::to_generic(p), 0);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ p.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance && opt[i].effect) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						effect::execute(state, opt[i].effect, trigger::to_generic(p), trigger::to_generic(p), 0, r_lo, r_hi);
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(p), trigger::to_generic(p), 0, r_lo, r_hi);
		}

		// TODO: notify
	}
}

struct event_nation_pair {
	dcon::nation_id n;
	dcon::free_national_event_id e;

	bool operator==(event_nation_pair const& other) const noexcept {
		return other.n == n && other.e == e;
	}
	bool operator<(event_nation_pair const& other) const noexcept {
		return other.n != n ? (n.value < other.n.value) : (e.value < other.e.value);
	}
};
struct event_prov_pair {
	dcon::province_id p;
	dcon::free_provincial_event_id e;

	bool operator==(event_prov_pair const& other) const noexcept {
		return other.p == p && other.e == e;
	}
	bool operator<(event_prov_pair const& other) const noexcept {
		return other.p != p ? (p.value < other.p.value) : (e.value < other.e.value);
	}
};

void update_events(sys::state& state) {
	uint32_t n_block_size = state.world.free_national_event_size() / 32;
	uint32_t p_block_size = state.world.free_provincial_event_size() / 32;

	uint32_t block_index = (state.current_date.value & 31);

	concurrency::combinable<std::vector<event_nation_pair>> events_triggered;

	auto n_block_end = block_index == 31 ? state.world.free_national_event_size() : n_block_size * (block_index + 1);
	concurrency::parallel_for(n_block_size * block_index, n_block_end, [&](uint32_t i) {
		dcon::free_national_event_id id{dcon::national_event_id::value_base_t(i)};
		auto mod = state.world.free_national_event_get_mtth(id);
		auto t = state.world.free_national_event_get_trigger(id);

		if(state.world.free_national_event_get_only_once(id) == false || state.world.free_national_event_get_has_been_triggered(id) == false) {
			ve::execute_serial_fast<dcon::nation_id>(state.world.nation_size(), [&](auto ids) {
				/*
				For national events: the base factor (scaled to days) is multiplied with all modifiers that hold. If the value is
				non positive, we take the probability of the event occurring as 0.000001. If the value is less than 0.001, the
				event is guaranteed to happen. Otherwise, the probability is the multiplicative inverse of the value.
				*/
				auto some_exist = t
					? (state.world.nation_get_owned_province_count(ids) != 0) && trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(ids), 0)
					: (state.world.nation_get_owned_province_count(ids) != 0);
				if(ve::compress_mask(some_exist).v != 0) {
					auto chances = mod ?
						trigger::evaluate_multiplicative_modifier(state, mod, trigger::to_generic(ids), trigger::to_generic(ids), 0) : ve::fp_vector{ 1.0f };
					auto adj_chance = 1.0f - ve::select(chances <= 1.0f, 1.0f, 1.0f / (chances));
					auto adj_chance_2 = adj_chance * adj_chance;
					auto adj_chance_4 = adj_chance_2 * adj_chance_2;
					auto adj_chance_8 = adj_chance_4 * adj_chance_4;
					auto adj_chance_16 = adj_chance_8 * adj_chance_8;

					ve::apply(
							[&](dcon::nation_id n, float c, bool condition) {
								auto owned_range = state.world.nation_get_province_ownership(n);
								if(condition && owned_range.begin() != owned_range.end()) {
									if(float(rng::get_random(state, uint32_t((i << 1) ^ n.index())) & 0xFFFFFF) / float(0xFFFFFF + 1) >= c) {
										events_triggered.local().push_back(event_nation_pair{n, id});
									}
								}
							},
							ids, adj_chance_16, some_exist);
				}
			});
		}
	});

	auto total_vector = events_triggered.combine([](auto& a, auto& b) {
		std::vector<event_nation_pair> result(a.begin(), a.end());
		result.insert(result.end(), b.begin(), b.end());
		return result;
	});
	std::sort(total_vector.begin(), total_vector.end());
	for(auto& v : total_vector) {
		event::trigger_national_event(state, v.e, v.n, uint32_t((state.current_date.value) ^ (v.e.value << 3)), uint32_t(v.n.value));
	}

	concurrency::combinable<std::vector<event_prov_pair>> p_events_triggered;

	auto p_block_end = block_index == 31 ? state.world.free_provincial_event_size() : p_block_size * (block_index + 1);
	concurrency::parallel_for(p_block_size * block_index, p_block_end, [&](uint32_t i) {
		dcon::free_provincial_event_id id{dcon::free_provincial_event_id::value_base_t(i)};
		auto mod = state.world.free_provincial_event_get_mtth(id);
		auto t = state.world.free_provincial_event_get_trigger(id);

		
			ve::execute_serial_fast<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](ve::contiguous_tags<dcon::province_id> ids) {
						/*
						The probabilities for province events are calculated in the same way, except that they are twice as likely to
						happen.
						*/
						auto owners = state.world.province_get_nation_from_province_ownership(ids);
						auto some_exist = t ? (owners != dcon::nation_id{}) &&
																			trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(owners), 0)
																: (owners != dcon::nation_id{});
						if(ve::compress_mask(some_exist).v != 0) {
							auto chances = mod
								? trigger::evaluate_multiplicative_modifier(state, mod, trigger::to_generic(ids), trigger::to_generic(owners), 0)
								: ve::fp_vector{2.0f};
							auto adj_chance = 1.0f - ve::select(chances <= 2.0f, 1.0f, 2.0f / chances);
							auto adj_chance_2 = adj_chance * adj_chance;
							auto adj_chance_4 = adj_chance_2 * adj_chance_2;
							auto adj_chance_8 = adj_chance_4 * adj_chance_4;
							auto adj_chance_16 = adj_chance_8 * adj_chance_8;

							ve::apply(
									[&](dcon::province_id p, dcon::nation_id o, float c, bool condition) {
										if(condition) {
											if(float(rng::get_random(state, uint32_t((i << 1) ^ p.index())) & 0xFFFFFF) / float(0xFFFFFF + 1) >= c) {
												p_events_triggered.local().push_back(event_prov_pair{p, id});
											}
										}
									},
									ids, owners, adj_chance_16, some_exist);
						}
					});
		
	});

	auto total_p_vector = p_events_triggered.combine([](auto& a, auto& b) {
		std::vector<event_prov_pair> result(a.begin(), a.end());
		result.insert(result.end(), b.begin(), b.end());
		return result;
	});
	std::sort(total_p_vector.begin(), total_p_vector.end());
	for(auto& v : total_p_vector) {
		trigger_provincial_event(state, v.e, v.p, uint32_t((state.current_date.value) ^ (v.e.value << 3)), uint32_t(v.p.value));
	}

	for(auto i = state.pending_n_event.size(); i-- > 0;) {
		if(state.pending_n_event[i].date + expiration_in_days <= state.current_date) { // expired
			auto& opt = state.world.national_event_get_options(state.pending_n_event[i].e);

			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, state.pending_n_event[i].primary_slot,
						trigger::to_generic(state.pending_n_event[i].n), state.pending_n_event[i].from_slot, state.pending_n_event[i].r_lo,
						state.pending_n_event[i].r_hi);
			}

			// TODO: notify

			state.pending_n_event[i] = state.pending_n_event.back();
			state.pending_n_event.pop_back();
		}
	}

	for(auto i = state.pending_f_n_event.size(); i-- > 0;) {
		if(state.pending_f_n_event[i].date + expiration_in_days <= state.current_date) { // expired
			auto& opt = state.world.free_national_event_get_options(state.pending_f_n_event[i].e);

			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, trigger::to_generic(state.pending_f_n_event[i].n),
						trigger::to_generic(state.pending_f_n_event[i].n), 0, state.pending_f_n_event[i].r_lo,
						state.pending_f_n_event[i].r_hi);
			}

			// TODO: notify

			state.pending_f_n_event[i] = state.pending_f_n_event.back();
			state.pending_f_n_event.pop_back();
		}
	}

	for(auto i = state.pending_p_event.size(); i-- > 0;) {
		if(state.pending_p_event[i].date + expiration_in_days <= state.current_date) { // expired
			auto& opt = state.world.provincial_event_get_options(state.pending_p_event[i].e);

			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, trigger::to_generic(state.pending_p_event[i].p),
						trigger::to_generic(state.pending_p_event[i].p), state.pending_p_event[i].from_slot, state.pending_p_event[i].r_lo,
						state.pending_p_event[i].r_hi);
			}

			// TODO: notify

			state.pending_p_event[i] = state.pending_p_event.back();
			state.pending_p_event.pop_back();
		}
	}

	for(auto i = state.pending_f_p_event.size(); i-- > 0;) {
		if(state.pending_f_p_event[i].date + expiration_in_days <= state.current_date) { // expired
			auto& opt = state.world.free_provincial_event_get_options(state.pending_f_p_event[i].e);

			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, trigger::to_generic(state.pending_f_p_event[i].p),
						trigger::to_generic(state.pending_f_p_event[i].p), 0, state.pending_f_p_event[i].r_lo,
						state.pending_f_p_event[i].r_hi);
			}

			// TODO: notify

			state.pending_f_p_event[i] = state.pending_f_p_event.back();
			state.pending_f_p_event.pop_back();
		}
	}
}

struct internal_n_epair {
	dcon::national_event_id e;
	int32_t chance;
};

void fire_fixed_event(sys::state& state, std::vector<nations::fixed_event> const& v, int32_t primary_slot, slot_type pt,
		dcon::nation_id this_slot, int32_t from_slot, slot_type ft) {
	static std::vector<internal_n_epair> valid_list;
	valid_list.clear();

	int32_t total_chances = 0;

	for(auto& fe : v) {
		if(!fe.condition || trigger::evaluate(state, fe.condition, primary_slot, trigger::to_generic(this_slot), from_slot)) {
			total_chances += fe.chance;
			valid_list.push_back(internal_n_epair{fe.id, fe.chance});
		}
	}

	auto possible_events = valid_list.size();
	if(possible_events > 0) {

		int32_t random_value =
				int32_t(rng::get_random(state, uint32_t(primary_slot + (state.world.nation_get_owned_province_count(this_slot) << 3))) %
								total_chances);

		for(auto& fe : valid_list) {
			random_value -= fe.chance;
			if(random_value < 0) {
				trigger_national_event(state, fe.e, this_slot, state.current_date.value, uint32_t(primary_slot), from_slot, ft);
				return;
			}
		}
	}
}

struct internal_p_epair {
	dcon::provincial_event_id e;
	int32_t chance;
};

void fire_fixed_event(sys::state& state, std::vector<nations::fixed_province_event> const& v, dcon::province_id prov,
		int32_t from_slot, slot_type ft) {
	static std::vector<internal_p_epair> valid_list;
	valid_list.clear();

	int32_t total_chances = 0;

	for(auto& fe : v) {
		if(!fe.condition || trigger::evaluate(state, fe.condition, trigger::to_generic(prov), trigger::to_generic(prov), from_slot)) {
			total_chances += fe.chance;
			valid_list.push_back(internal_p_epair{fe.id, fe.chance});
		}
	}

	auto possible_events = valid_list.size();
	if(possible_events > 0) {

		int32_t random_value = int32_t(rng::get_random(state, uint32_t(prov.index())) % total_chances);

		for(auto& fe : valid_list) {
			random_value -= fe.chance;
			if(random_value < 0) {
				trigger_provincial_event(state, fe.e, prov, state.current_date.value, uint32_t(prov.index()), from_slot, ft);
				return;
			}
		}
	}
}

} // namespace event
