#include "events.hpp"

namespace event {

void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_lo, uint32_t r_hi) {
	if(auto immediate = state.world.national_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi);
	}
	if(state.world.nation_get_is_player_controlled(n)) {
		state.pending_n_event.push_back(pending_human_n_event{e, n, r_lo, r_hi + 1, state.current_date });
		// TODO: notify ui
	} else {
		auto& opt = state.world.national_event_get_options(e);
		float total = 0.0f;
		float odds[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
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
void trigger_national_event(sys::state& state, dcon::free_national_event_id e, dcon::nation_id n, uint32_t r_lo, uint32_t r_hi) {
	state.world.free_national_event_set_has_been_triggered(e, true);
	if(auto immediate = state.world.free_national_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi);
	}
	if(state.world.nation_get_is_player_controlled(n)) {
		state.pending_f_n_event.push_back(pending_human_f_n_event{ e, n, r_lo, r_hi + 1, state.current_date });
		// TODO: notify ui
	} else {
		auto& opt = state.world.free_national_event_get_options(e);
		float total = 0.0f;
		float odds[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
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
void trigger_provincial_event(sys::state& state, dcon::provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo) {
	auto owner = state.world.province_get_nation_from_province_ownership(p);
	if(state.world.nation_get_is_player_controlled(owner)) {
		state.pending_p_event.push_back(pending_human_p_event{ e, p, r_lo, r_hi, state.current_date });
		// TODO: notify ui
	} else {
		auto& opt = state.world.provincial_event_get_options(e);
		float total = 0.0f;
		float odds[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance && opt[i].effect) {
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(p), trigger::to_generic(owner), 0);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ p.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance && opt[i].effect) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						effect::execute(state, opt[i].effect, trigger::to_generic(p), trigger::to_generic(owner), 0, r_lo, r_hi);
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(p), trigger::to_generic(owner), 0, r_lo, r_hi);
		}

		// TODO: notify
	}
}
void trigger_provincial_event(sys::state& state, dcon::free_provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo) {
	state.world.free_provincial_event_set_has_been_triggered(e, true);

	auto owner = state.world.province_get_nation_from_province_ownership(p);
	if(state.world.nation_get_is_player_controlled(owner)) {
		state.pending_f_p_event.push_back(pending_human_f_p_event{ e, p, r_lo, r_hi, state.current_date });
		// TODO: notify ui
	} else {
		auto& opt = state.world.free_provincial_event_get_options(e);
		float total = 0.0f;
		float odds[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance && opt[i].effect) {
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(p), trigger::to_generic(owner), 0);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ p.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance && opt[i].effect) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						effect::execute(state, opt[i].effect, trigger::to_generic(p), trigger::to_generic(owner), 0, r_lo, r_hi);
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(p), trigger::to_generic(owner), 0, r_lo, r_hi);
		}

		// TODO: notify
	}
}

void update_events(sys::state& state) {
	uint32_t n_block_size = state.world.free_national_event_size() / 32;
	uint32_t p_block_size = state.world.free_provincial_event_size() / 32;

	uint32_t block_index = (state.current_date.value & 31);

	auto n_block_end = block_index == 31 ? state.world.free_national_event_size() : n_block_size * (block_index + 1);
	for(uint32_t i = n_block_size * block_index; i < n_block_end; ++i) {
		dcon::free_national_event_id id{dcon::national_event_id::value_base_t(i) };
		auto mod = state.world.free_national_event_get_mtth(id);
		auto t = state.world.free_national_event_get_trigger(id);

		if(mod && (state.world.free_national_event_get_only_once(id) == false || state.world.free_national_event_get_has_been_triggered(id) == false)) {
			ve::execute_serial_fast<dcon::nation_id>(state.world.nation_size(), [&](auto ids) {
				/*
				For national events: the base factor (scaled to days) is multiplied with all modifiers that hold. If the value is non positive, we take the probability of the event occurring as 0.000001. If the value is less than 0.001, the event is guaranteed to happen. Otherwise, the probability is the multiplicative inverse of the value.
				*/
				auto some_exist = t ? (state.world.nation_get_owned_province_count(ids) != 0) && trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(ids), 0) : (state.world.nation_get_owned_province_count(ids) != 0);
				if(ve::compress_mask(some_exist).v != 0) {
					auto chances = trigger::evaluate_multiplicative_modifier(state, mod, trigger::to_generic(ids), trigger::to_generic(ids), 0);
					auto adj_chance = 1.0f - ve::select(chances <= 1.0f, 1.0f, 1.0f / (chances));
					auto adj_chance_2 = adj_chance * adj_chance;
					auto adj_chance_4 = adj_chance_2 * adj_chance_2;
					auto adj_chance_8 = adj_chance_4 * adj_chance_4;
					auto adj_chance_16 = adj_chance_8 * adj_chance_8;

					ve::apply([&](dcon::nation_id n, float c) {
						auto owned_range = state.world.nation_get_province_ownership(n);
						if((state.world.free_national_event_get_only_once(id) == false || state.world.free_national_event_get_has_been_triggered(id) == false)
							&& owned_range.begin() != owned_range.end()) {

							if(float(rng::get_random(state, uint32_t((i << 1) ^ n.index())) & 0xFFFF) / float(0xFFFF + 1) >= c) {
								trigger_national_event(state, id, n, uint32_t((state.current_date.value) ^ (i << 3)), uint32_t(n.index()));
							}
						}
						
					}, ids, adj_chance_16);
				}
			});
		}
	}

	auto p_block_end = block_index == 31 ? state.world.free_provincial_event_size() : p_block_size * (block_index + 1);
	for(uint32_t i = p_block_size * block_index; i < p_block_end; ++i) {
		dcon::free_provincial_event_id id{ dcon::free_provincial_event_id::value_base_t(i) };
		auto mod = state.world.free_provincial_event_get_mtth(id);
		auto t = state.world.free_provincial_event_get_trigger(id);

		if(mod) {
			ve::execute_serial_fast<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()), [&](auto ids){
				/*
				The probabilities for province events are calculated in the same way, except that they are twice as likely to happen.
				*/
				auto owners = state.world.province_get_nation_from_province_ownership(ids);
				auto some_exist = t ? (owners != dcon::nation_id{}) && trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(owners), 0) : (owners != dcon::nation_id{});
				if(ve::compress_mask(some_exist).v != 0) {
					auto chances = trigger::evaluate_multiplicative_modifier(state, mod, trigger::to_generic(ids), trigger::to_generic(owners), 0);
					auto adj_chance = 1.0f - ve::select(chances <= 2.0f, 1.0f, 2.0f / chances);
					auto adj_chance_2 = adj_chance * adj_chance;
					auto adj_chance_4 = adj_chance_2 * adj_chance_2;
					auto adj_chance_8 = adj_chance_4 * adj_chance_4;
					auto adj_chance_16 = adj_chance_8 * adj_chance_8;

					ve::apply([&](dcon::province_id p, dcon::nation_id o, float c) {
						auto owned_range = state.world.nation_get_province_ownership(o);
						if(o) {
							if(float(rng::get_random(state, uint32_t((i << 1) ^ p.index())) & 0xFFFF) / float(0xFFFF + 1) >= c) {
								trigger_provincial_event(state, id, p, uint32_t((state.current_date.value) ^ (i << 3)), uint32_t(p.index()));
							}
						}
					}, ids, owners, adj_chance_16);
				}
			});
		}
	}

	for(auto i = state.pending_n_event.size(); i-- > 0;) {
		if(state.pending_n_event[i].date + 30 <= state.current_date) { // expired
			auto& opt = state.world.national_event_get_options(state.pending_n_event[i].e);
			
			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, trigger::to_generic(state.pending_n_event[i].n), trigger::to_generic(state.pending_n_event[i].n), 0, state.pending_n_event[i].r_lo, state.pending_n_event[i].r_hi);
			}

			// TODO: notify

			state.pending_n_event[i] = state.pending_n_event.back();
			state.pending_n_event.pop_back();
		}
	}

	for(auto i = state.pending_f_n_event.size(); i-- > 0;) {
		if(state.pending_f_n_event[i].date + 30 <= state.current_date) { // expired
			auto& opt = state.world.free_national_event_get_options(state.pending_f_n_event[i].e);

			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, trigger::to_generic(state.pending_f_n_event[i].n), trigger::to_generic(state.pending_f_n_event[i].n), 0, state.pending_f_n_event[i].r_lo, state.pending_f_n_event[i].r_hi);
			}

			// TODO: notify

			state.pending_f_n_event[i] = state.pending_f_n_event.back();
			state.pending_f_n_event.pop_back();
		}
	}

	for(auto i = state.pending_p_event.size(); i-- > 0;) {
		if(state.pending_p_event[i].date + 30 <= state.current_date) { // expired
			auto& opt = state.world.provincial_event_get_options(state.pending_p_event[i].e);

			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, trigger::to_generic(state.pending_p_event[i].p), trigger::to_generic(state.world.province_get_nation_from_province_ownership(state.pending_p_event[i].p)), 0, state.pending_p_event[i].r_lo, state.pending_p_event[i].r_hi);
			}

			// TODO: notify

			state.pending_p_event[i] = state.pending_p_event.back();
			state.pending_p_event.pop_back();
		}
	}

	for(auto i = state.pending_f_p_event.size(); i-- > 0;) {
		if(state.pending_f_p_event[i].date + 30 <= state.current_date) { // expired
			auto& opt = state.world.free_provincial_event_get_options(state.pending_f_p_event[i].e);

			if(opt[0].effect) {
				effect::execute(state, opt[0].effect, trigger::to_generic(state.pending_f_p_event[i].p), trigger::to_generic(state.world.province_get_nation_from_province_ownership(state.pending_f_p_event[i].p)), 0, state.pending_f_p_event[i].r_lo, state.pending_f_p_event[i].r_hi);
			}

			// TODO: notify

			state.pending_f_p_event[i] = state.pending_f_p_event.back();
			state.pending_f_p_event.pop_back();
		}
	}
}

}
