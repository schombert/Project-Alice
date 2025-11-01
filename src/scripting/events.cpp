#include "events.hpp"
#include "effects.hpp"
#include "gui_event.hpp"
#include "prng.hpp"
#include "system_state.hpp"
#include "triggers.hpp"

namespace event {

bool is_valid_option(sys::event_option const& opt) {
	return opt.effect || opt.name;
}

void take_option(sys::state& state, pending_human_n_event const& e, uint8_t opt) {
	for(auto i = state.pending_n_event.size(); i-- > 0;) {
		if(state.pending_n_event[i] == e) {

			if(opt > state.world.national_event_get_options(e.e).size() || !is_valid_option(state.world.national_event_get_options(e.e)[opt]))
				return; // invalid option

			state.pending_n_event[i] = state.pending_n_event.back();
			state.pending_n_event.pop_back();

			if(state.world.national_event_get_options(e.e)[opt].effect) {
				effect::execute(state, state.world.national_event_get_options(e.e)[opt].effect, e.primary_slot, trigger::to_generic(e.n),
						e.from_slot, e.r_lo, e.r_hi);
			}
			return;
		}
	}
}

void take_option(sys::state& state, pending_human_f_n_event const& e, uint8_t opt) {
	for(auto i = state.pending_f_n_event.size(); i-- > 0;) {
		if(state.pending_f_n_event[i] == e) {

			if(opt > state.world.free_national_event_get_options(e.e).size() || !is_valid_option(state.world.free_national_event_get_options(e.e)[opt]))
				return; // invalid option

			state.pending_f_n_event[i] = state.pending_f_n_event.back();
			state.pending_f_n_event.pop_back();

			if(state.world.free_national_event_get_options(e.e)[opt].effect) {
				effect::execute(state, state.world.free_national_event_get_options(e.e)[opt].effect, trigger::to_generic(e.n),
						trigger::to_generic(e.n), 0, e.r_lo, e.r_hi);
			}
			return;
		}
	}
}

void take_option(sys::state& state, pending_human_p_event const& e, uint8_t opt) {
	for(auto i = state.pending_p_event.size(); i-- > 0;) {
		if(state.pending_p_event[i] == e) {

			if(opt > state.world.provincial_event_get_options(e.e).size() || !is_valid_option(state.world.provincial_event_get_options(e.e)[opt]))
				return; // invalid option

			state.pending_p_event[i] = state.pending_p_event.back();
			state.pending_p_event.pop_back();

			if(state.world.provincial_event_get_options(e.e)[opt].effect) {
				effect::execute(state, state.world.provincial_event_get_options(e.e)[opt].effect, trigger::to_generic(e.p),
						trigger::to_generic(e.p), e.from_slot, e.r_lo, e.r_hi);
			}

			return;
		}
	}
}
void take_option(sys::state& state, pending_human_f_p_event const& e, uint8_t opt) {
	for(auto i = state.pending_f_p_event.size(); i-- > 0;) {
		if(state.pending_f_p_event[i] == e) {

			if(opt > state.world.free_provincial_event_get_options(e.e).size() || !is_valid_option(state.world.free_provincial_event_get_options(e.e)[opt]))
				return; // invalid option

			state.pending_f_p_event[i] = state.pending_f_p_event.back();
			state.pending_f_p_event.pop_back();

			if(state.world.free_provincial_event_get_options(e.e)[opt].effect) {
				effect::execute(state, state.world.free_provincial_event_get_options(e.e)[opt].effect, trigger::to_generic(e.p),
						trigger::to_generic(e.p), -1, e.r_lo, e.r_hi);
			}
			return;
		}
	}
}

template<typename T>
bool event_has_options(sys::state& state, T id) {
	auto fat_id = dcon::fatten(state.world, id);
	bool b = false;
	for(const auto& opt : fat_id.get_options()) {
		if(!is_valid_option(opt))
			break;
		b = true;
	}
	return b;
}

void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_lo, uint32_t r_hi, int32_t primary_slot, slot_type pt, int32_t from_slot, slot_type ft) {
	if(!state.world.national_event_get_name(e) && !state.world.national_event_get_immediate_effect(e) && !event_has_options(state, e))
		return; // event without data
	if(ft == slot_type::province)
		assert(dcon::fatten(state.world, state.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot))).is_valid());


	if(state.world.national_event_get_is_major(e)) {
		notification::post(state, notification::message{
			[ev = pending_human_n_event{r_lo, r_hi + 1, primary_slot, from_slot, state.current_date, e, n, pt, ft}](sys::state& state, text::layout_base& contents) {
				text::substitution_map m;
				ui::populate_event_submap(state, m, ev);

				{
					auto box = text::open_layout_box(contents);
					text::add_to_layout_box(state, contents, box, state.world.national_event_get_name(ev.e), m);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					text::add_line_break_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, state.world.national_event_get_description(ev.e), m);
					text::close_layout_box(contents, box);
				}
			},
			"msg_m_event_title",
			n, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::major_event,
			dcon::province_id{ }
		});
	} else if(n == state.local_player_nation) {
		notification::post(state, notification::message{
			[ev = pending_human_n_event{r_lo, r_hi + 1, primary_slot, from_slot, state.current_date, e, n, pt, ft}](sys::state& state, text::layout_base& contents) {
				text::substitution_map m;
				ui::populate_event_submap(state, m, ev);

				{
					auto box = text::open_layout_box(contents);
					text::add_to_layout_box(state, contents, box, state.world.national_event_get_name(ev.e), m);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					text::add_line_break_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, state.world.national_event_get_description(ev.e), m);
					text::close_layout_box(contents, box);
				}
			},
			"msg_n_event_title",
			n, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::national_event,
			dcon::province_id{ }
		});
	}

	if(auto immediate = state.world.national_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, primary_slot, trigger::to_generic(n), from_slot, r_lo, r_hi);
	}
	if(state.world.nation_get_is_player_controlled(n)) {
		pending_human_n_event new_event{r_lo, r_hi + 1, primary_slot, from_slot, state.current_date, e, n, pt, ft};
		state.pending_n_event.push_back(new_event);
		if(n == state.local_player_nation)
			state.new_n_event.push(new_event);
	} else {
		auto& opt = state.world.national_event_get_options(e);
		float total = 0.0f;
		float odds[sys::max_event_options] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f , 0.0f };
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance) { //opt[i].effect may not be defined, but it may still be present
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, primary_slot, trigger::to_generic(n), from_slot);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ n.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						if(opt[i].effect) {
							effect::execute(state, opt[i].effect, primary_slot, trigger::to_generic(n), from_slot, r_lo, r_hi + 1);
						}
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, primary_slot, trigger::to_generic(n), from_slot, r_lo, r_hi + 1);
		}
	}
}
void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo, int32_t from_slot, slot_type ft) {
	trigger_national_event(state, e, n, r_hi, r_lo, trigger::to_generic(n), slot_type::nation, from_slot, ft);
}
void trigger_national_event(sys::state& state, dcon::free_national_event_id e, dcon::nation_id n, uint32_t r_lo, uint32_t r_hi) {
	if(state.world.free_national_event_get_only_once(e) && state.world.free_national_event_get_has_been_triggered(e))
		return;
	if(!state.world.free_national_event_get_name(e) && !state.world.free_national_event_get_immediate_effect(e) && !event_has_options(state, e))
		return; // event without data

	state.world.free_national_event_set_has_been_triggered(e, true);
	if(state.world.free_national_event_get_is_major(e)) {
		notification::post(state, notification::message{
			[ev = pending_human_f_n_event{r_lo, r_hi + 1, state.current_date, e, n}](sys::state& state, text::layout_base& contents) {
				text::substitution_map m;
				ui::populate_event_submap(state, m, ev);

				{
					auto box = text::open_layout_box(contents);
					text::add_to_layout_box(state, contents, box, state.world.free_national_event_get_name(ev.e), m);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					text::add_line_break_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, state.world.free_national_event_get_description(ev.e), m);
					text::close_layout_box(contents, box);
				}
			},
			"msg_m_event_title",
			n, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::major_event,
			dcon::province_id{ }
		});
	} else if(n == state.local_player_nation) {
		notification::post(state, notification::message{
			[ev = pending_human_f_n_event{r_lo, r_hi + 1, state.current_date, e, n}](sys::state& state, text::layout_base& contents) {
				text::substitution_map m;
				ui::populate_event_submap(state, m, ev);

				{
					auto box = text::open_layout_box(contents);
					text::add_to_layout_box(state, contents, box, state.world.free_national_event_get_name(ev.e), m);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					text::add_line_break_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, state.world.free_national_event_get_description(ev.e), m);
					text::close_layout_box(contents, box);
				}
			},
			"msg_n_event_title",
			n, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::national_event,
			dcon::province_id{ }
		});
	}
	if(auto immediate = state.world.free_national_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi);
	}
	if(state.world.nation_get_is_player_controlled(n)) {
		pending_human_f_n_event new_event{r_lo, r_hi + 1, state.current_date, e, n};
		state.pending_f_n_event.push_back(new_event);
		if(n == state.local_player_nation)
			state.new_f_n_event.push(new_event);
	} else {
		auto& opt = state.world.free_national_event_get_options(e);
		float total = 0.0f;
		float odds[sys::max_event_options] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance) { //effect may not be present but chance may
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(n), trigger::to_generic(n), 0);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t((e.index() << 3) ^ n.index())) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						if(opt[i].effect) {
							effect::execute(state, opt[i].effect, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi + 1);
						}
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(n), trigger::to_generic(n), 0, r_lo, r_hi + 1);
		}
	}
}
void trigger_provincial_event(sys::state& state, dcon::provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo, int32_t from_slot, slot_type ft) {
	assert(e && "Potential invalid write incoming");
	if(!state.world.provincial_event_get_name(e) && !state.world.provincial_event_get_immediate_effect(e) && !event_has_options(state, e))
		return; // event without data
	if(ft == slot_type::province)
		assert(dcon::fatten(state.world, state.world.province_get_nation_from_province_ownership(trigger::to_prov(from_slot))).is_valid());

	if(auto immediate = state.world.provincial_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, trigger::to_generic(p), trigger::to_generic(p), from_slot, r_lo, r_hi);
	}
	auto owner = state.world.province_get_nation_from_province_ownership(p);
	if(owner == state.local_player_nation) {
		notification::post(state, notification::message{
			[ev = pending_human_p_event{r_lo, r_hi, from_slot, state.current_date, e, p, ft}](sys::state& state, text::layout_base& contents) {
				text::substitution_map m;
				ui::populate_event_submap(state, m, ev);

				{
					auto box = text::open_layout_box(contents);
					text::add_to_layout_box(state, contents, box, state.world.provincial_event_get_name(ev.e), m);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					text::add_line_break_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, state.world.provincial_event_get_description(ev.e), m);
					text::close_layout_box(contents, box);
				}
			},
			"msg_p_event_title",
			owner, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::province_event,
			p
		});
	}
	if(state.world.nation_get_is_player_controlled(owner)) {
		pending_human_p_event new_event{r_lo, r_hi, from_slot, state.current_date, e, p, ft};
		state.pending_p_event.push_back(new_event);
		if(owner == state.local_player_nation)
			state.new_p_event.push(new_event);
	} else {
		auto& opt = state.world.provincial_event_get_options(e);
		float total = 0.0f;
		float odds[sys::max_event_options] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance) { //effect may not be present but chance may
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(p), trigger::to_generic(p), from_slot);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ p.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						if(opt[i].effect) {
							effect::execute(state, opt[i].effect, trigger::to_generic(p), trigger::to_generic(p), from_slot, r_lo, r_hi);
						}
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(p), trigger::to_generic(p), from_slot, r_lo, r_hi);
		}
	}
}
void trigger_provincial_event(sys::state& state, dcon::free_provincial_event_id e, dcon::province_id p, uint32_t r_hi,
		uint32_t r_lo) {
	assert(e);
	if(state.world.free_provincial_event_get_only_once(e) && state.world.free_provincial_event_get_has_been_triggered(e))
		return;
	if(!state.world.free_provincial_event_get_name(e) && !state.world.free_provincial_event_get_immediate_effect(e) && !event_has_options(state, e))
		return; // event without data

	state.world.free_provincial_event_set_has_been_triggered(e, true);
	if(auto immediate = state.world.free_provincial_event_get_immediate_effect(e); immediate) {
		effect::execute(state, immediate, trigger::to_generic(p), trigger::to_generic(p), 0, r_lo, r_hi);
	}
	auto owner = state.world.province_get_nation_from_province_ownership(p);
	if(owner == state.local_player_nation) {
		notification::post(state, notification::message{
			[ev = pending_human_f_p_event{r_lo, r_hi, state.current_date, e, p}](sys::state& state, text::layout_base& contents) {
				text::substitution_map m;
				ui::populate_event_submap(state, m, ev);

				{
					auto box = text::open_layout_box(contents);
					text::add_to_layout_box(state, contents, box, state.world.free_provincial_event_get_name(ev.e), m);
					text::close_layout_box(contents, box);
				}
				{
					auto box = text::open_layout_box(contents);
					text::add_line_break_to_layout_box(state, contents, box);
					text::add_to_layout_box(state, contents, box, state.world.free_provincial_event_get_description(ev.e), m);
					text::close_layout_box(contents, box);
				}
			},
			"msg_p_event_title",
			owner, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::province_event,
			p
		});
	}
	if(state.world.nation_get_is_player_controlled(owner)) {
		pending_human_f_p_event new_event{r_lo, r_hi, state.current_date, e, p};
		state.pending_f_p_event.push_back(new_event);
		if(owner == state.local_player_nation)
			state.new_f_p_event.push(new_event);
	} else {
		auto& opt = state.world.free_provincial_event_get_options(e);
		float total = 0.0f;
		float odds[sys::max_event_options] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for(uint32_t i = 0; i < opt.size(); ++i) {
			if(opt[i].ai_chance) { //effect may not be present but chance may
				odds[i] = trigger::evaluate_multiplicative_modifier(state, opt[i].ai_chance, trigger::to_generic(p), trigger::to_generic(p), 0);
				total += odds[i];
			}
		}

		if(total > 0.0f) {
			auto rvalue = float(rng::get_random(state, uint32_t(e.index() ^ p.index() << 5)) & 0xFFFF) / float(0xFFFF + 1);
			for(uint32_t i = 0; i < opt.size(); ++i) {
				if(opt[i].ai_chance) {
					rvalue -= odds[i] / total;
					if(rvalue < 0.0f) {
						if(opt[i].effect) {
							effect::execute(state, opt[i].effect, trigger::to_generic(p), trigger::to_generic(p), 0, r_lo, r_hi);
						}
						return;
					}
				}
			}
		}

		if(opt[0].effect) {
			effect::execute(state, opt[0].effect, trigger::to_generic(p), trigger::to_generic(p), 0, r_lo, r_hi);
		}
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

bool would_be_duplicate_instance(sys::state& state, dcon::national_event_id e, dcon::nation_id n, sys::date date) {
	if(state.world.national_event_get_allow_multiple_instances(e))
		return false;
	for(int32_t i = 0; i < int32_t(state.future_n_event.size()); i++) {
		auto const& nev = state.future_n_event[i];
		if(nev.e == e
		&& nev.n == n
		&& nev.date == date) {
			return true;
		}
	}
	return false;
}

void update_future_events(sys::state& state) {
	for(uint32_t i = 0; i < uint32_t(state.defines.alice_max_event_iterations); i++) {
		bool fired_n = false;
		uint32_t n_n_events = uint32_t(state.future_n_event.size());
		for(int32_t j = int32_t(n_n_events - 1); j >= 0; j--) {
			auto const& e = state.future_n_event[j];
			if(e.date <= state.current_date) {
				trigger_national_event(state, e.e, e.n, e.r_lo, e.r_hi, e.primary_slot, e.pt, e.from_slot, e.ft);
				fired_n = true;
			}
		}
		if(fired_n) {
			for(int32_t j = int32_t(n_n_events - 1); j >= 0; j--) {
				auto const& e = state.future_n_event[j];
				if(e.date <= state.current_date) {
					std::swap(state.future_n_event[j], state.future_n_event[state.future_n_event.size() - 1]);
					state.future_n_event.pop_back();
				}
			}
		}
		//
		bool fired_p = false;
		uint32_t n_p_events = uint32_t(state.future_p_event.size());
		for(int32_t j = int32_t(n_p_events - 1); j >= 0; j--) {
			auto const& e = state.future_p_event[j];
			if(e.date <= state.current_date) {
				trigger_provincial_event(state, e.e, e.p, e.r_lo, e.r_hi, e.from_slot, e.ft);
				fired_p = true;
			}
		}
		if(fired_p) {
			for(int32_t j = int32_t(n_p_events - 1); j >= 0; j--) {
				auto const& e = state.future_p_event[j];
				if(e.date <= state.current_date) {
					std::swap(state.future_p_event[j], state.future_p_event[state.future_p_event.size() - 1]);
					state.future_p_event.pop_back();
				}
			}
		}
		if(!fired_p && !fired_n)
			break;
	}
}

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
					? (nations::exists_or_is_utility_tag(state, ids)) && trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(ids), 0)
					: (nations::exists_or_is_utility_tag(state, ids));
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
								if(condition && nations::exists_or_is_utility_tag(state, n)) {
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
		if(trigger::evaluate(state, state.world.free_national_event_get_trigger(v.e), trigger::to_generic(v.n), trigger::to_generic(v.n), 0)) {
			event::trigger_national_event(state, v.e, v.n, uint32_t((state.current_date.value) ^ (v.e.value << 3)), uint32_t(v.n.value));
		}
	}

	concurrency::combinable<std::vector<event_prov_pair>> p_events_triggered;

	auto p_block_end = block_index == 31 ? state.world.free_provincial_event_size() : p_block_size * (block_index + 1);
	concurrency::parallel_for(p_block_size * block_index, p_block_end, [&](uint32_t i) {
		dcon::free_provincial_event_id id{dcon::free_provincial_event_id::value_base_t(i)};
		auto mod = state.world.free_provincial_event_get_mtth(id);
		auto t = state.world.free_provincial_event_get_trigger(id);

		if(state.world.free_provincial_event_get_only_once(id) == false || state.world.free_provincial_event_get_has_been_triggered(id) == false) {
			ve::execute_serial_fast<dcon::province_id>(uint32_t(state.province_definitions.first_sea_province.index()),
					[&](ve::contiguous_tags<dcon::province_id> ids) {
						/*
						The probabilities for province events are calculated in the same way, except that they are twice as likely to
						happen.
						*/
						auto owners = state.world.province_get_nation_from_province_ownership(ids);
						auto some_exist = t ? (owners != dcon::nation_id{}) &&
							trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(ids), 0)
							: (owners != dcon::nation_id{});
						if(ve::compress_mask(some_exist).v != 0) {
							auto chances = mod
								? trigger::evaluate_multiplicative_modifier(state, mod, trigger::to_generic(ids), trigger::to_generic(ids), 0)
								: ve::fp_vector{ 2.0f };
							auto adj_chance = 1.0f - ve::select(chances <= 2.0f, 1.0f, 2.0f / chances);
							auto adj_chance_2 = adj_chance * adj_chance;
							auto adj_chance_4 = adj_chance_2 * adj_chance_2;
							auto adj_chance_8 = adj_chance_4 * adj_chance_4;
							auto adj_chance_16 = adj_chance_8 * adj_chance_8;

							ve::apply(
									[&](dcon::province_id p, dcon::nation_id o, float c, bool condition) {
										if(condition) {
											if(float(rng::get_random(state, uint32_t((i << 1) ^ p.index())) & 0xFFFFFF) / float(0xFFFFFF + 1) >= c) {
												p_events_triggered.local().push_back(event_prov_pair{ p, id });
											}
										}
									},
									ids, owners, adj_chance_16, some_exist);
						}
					});
		}
	});

	auto total_p_vector = p_events_triggered.combine([](auto& a, auto& b) {
		std::vector<event_prov_pair> result(a.begin(), a.end());
		result.insert(result.end(), b.begin(), b.end());
		return result;
	});
	std::sort(total_p_vector.begin(), total_p_vector.end());
	for(auto& v : total_p_vector) {
		if(trigger::evaluate(state, state.world.free_provincial_event_get_trigger(v.e), trigger::to_generic(v.p), trigger::to_generic(v.p), 0)) {
			trigger_provincial_event(state, v.e, v.p, uint32_t((state.current_date.value) ^ (v.e.value << 3)), uint32_t(v.p.value));
		}
	}

	for(auto i = state.pending_n_event.size(); i-- > 0;) {
		if(state.pending_n_event[i].date + (int32_t)state.defines.alice_event_taken_auto_days <= state.current_date) { // expired
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
		if(state.pending_f_n_event[i].date + (int32_t)state.defines.alice_event_taken_auto_days <= state.current_date) { // expired
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
		if(state.pending_p_event[i].date + (int32_t) state.defines.alice_event_taken_auto_days <= state.current_date) { // expired
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
		if(state.pending_f_p_event[i].date + (int32_t) state.defines.alice_event_taken_auto_days <= state.current_date) { // expired
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

	update_future_events(state);
}

struct internal_n_epair {
	dcon::national_event_id e;
	int32_t chance;
};

void fire_fixed_event(sys::state& state, std::vector<nations::fixed_event> const& v, int32_t primary_slot, slot_type pt, dcon::nation_id this_slot, int32_t from_slot, slot_type ft) {
	static std::vector<internal_n_epair> valid_list;
	valid_list.clear();
	int32_t total_chances = 0;
	for(auto& fe : v) {
		if(!fe.condition || trigger::evaluate(state, fe.condition, primary_slot, trigger::to_generic(this_slot), from_slot)) {
			total_chances += fe.chance;
			valid_list.push_back(internal_n_epair{fe.id, fe.chance});
		}
	}
	if(auto possible_events = valid_list.size(); possible_events > 0) {
		int32_t random_value = int32_t(rng::get_random(state, uint32_t(primary_slot + (state.world.nation_get_owned_province_count(this_slot) << 3))) % total_chances);
		for(auto& fe : valid_list) {
			random_value -= fe.chance;
			if(random_value < 0) {
				trigger_national_event(state, fe.e, this_slot, state.current_date.value, uint32_t(primary_slot), primary_slot, pt, from_slot, ft);
				return;
			}
		}
	}
}

void fire_fixed_event(sys::state& state, std::vector<nations::fixed_election_event> const& v, int32_t primary_slot, slot_type pt, dcon::nation_id this_slot, int32_t from_slot, slot_type ft) {
	static std::vector<internal_n_epair> valid_list;
	valid_list.clear();
	int32_t total_chances = 0;
	for(auto& fe : v) {
		if(!fe.condition || trigger::evaluate(state, fe.condition, primary_slot, trigger::to_generic(this_slot), from_slot)) {
			total_chances += fe.chance;
			valid_list.push_back(internal_n_epair{ fe.id, fe.chance });
		}
	}
	if(auto possible_events = valid_list.size(); possible_events > 0) {
		int32_t random_value = int32_t(rng::get_random(state, uint32_t(primary_slot + (state.world.nation_get_owned_province_count(this_slot) << 3))) % total_chances);
		for(auto& fe : valid_list) {
			random_value -= fe.chance;
			if(random_value < 0) {
				trigger_national_event(state, fe.e, this_slot, state.current_date.value, uint32_t(primary_slot), primary_slot, pt, from_slot, ft);
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

dcon::issue_id get_election_event_issue(sys::state& state, dcon::national_event_id e) {
	for(auto const& fe : state.national_definitions.on_election_tick) {
		if(fe.id == e) {
			return fe.issue_group;
		}
	}
	return dcon::issue_id{};
}

} // namespace event
