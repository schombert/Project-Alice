#include "ai_focuses.hpp"
#include "demographics.hpp"
#include "triggers.hpp"


namespace ai {
void identify_focuses(sys::state& state) {
	for(auto f : state.world.in_national_focus) {
		if(f.get_promotion_amount() > 0) {
			if(f.get_promotion_type() == state.culture_definitions.clergy)
				state.national_definitions.clergy_focus = f;
			if(f.get_promotion_type() == state.culture_definitions.soldiers)
				state.national_definitions.soldier_focus = f;
			if(f.get_promotion_type() == state.culture_definitions.aristocrat)
				state.national_definitions.aristocrat_focus = f;
			if(f.get_promotion_type() == state.culture_definitions.capitalists)
				state.national_definitions.capitalist_focus = f;
			if(f.get_promotion_type() == state.culture_definitions.primary_factory_worker)
				state.national_definitions.primary_factory_worker_focus = f;
			if(f.get_promotion_type() == state.culture_definitions.secondary_factory_worker)
				state.national_definitions.secondary_factory_worker_focus = f;
		}
	}
}

void update_focuses(sys::state& state) {
	for(auto si : state.world.in_state_instance) {
		if(!si.get_nation_from_state_ownership().get_is_player_controlled())
			si.set_owner_focus(dcon::national_focus_id{});
	}

	for(auto n : state.world.in_nation) {
		if(n.get_is_player_controlled())
			continue;
		if(n.get_owned_province_count() == 0)
			continue;

		n.set_state_from_flashpoint_focus(dcon::state_instance_id{});

		auto num_focuses_total = nations::max_national_focuses(state, n);
		if(num_focuses_total <= 0)
			return;

		auto base_opt = state.world.pop_type_get_research_optimum(state.culture_definitions.clergy);
		auto clergy_frac = n.get_demographics(demographics::to_key(state, state.culture_definitions.clergy)) / n.get_demographics(demographics::total);
		bool max_clergy = clergy_frac >= base_opt;

		static std::vector<dcon::state_instance_id> ordered_states;
		ordered_states.clear();
		for(auto si : n.get_state_ownership()) {
			ordered_states.push_back(si.get_state().id);
		}
		std::sort(ordered_states.begin(), ordered_states.end(), [&](auto a, auto b) {
			auto apop = state.world.state_instance_get_demographics(a, demographics::total);
			auto bpop = state.world.state_instance_get_demographics(b, demographics::total);
			if(apop != bpop)
				return apop > bpop;
			else
				return a.index() < b.index();
		});
		bool threatened = n.get_ai_is_threatened() || n.get_is_at_war();
		for(uint32_t i = 0; num_focuses_total > 0 && i < ordered_states.size(); ++i) {
			auto prov = state.world.state_instance_get_capital(ordered_states[i]);
			if(max_clergy) {
				if(threatened) {
					auto nf = state.national_definitions.soldier_focus;
					auto k = state.world.national_focus_get_limit(nf);
					if(!k || trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(n), -1)) {
						assert(command::can_set_national_focus(state, n, ordered_states[i], nf));
						state.world.state_instance_set_owner_focus(ordered_states[i], state.national_definitions.soldier_focus);
						--num_focuses_total;
					}
				} else {
					auto total = state.world.state_instance_get_demographics(ordered_states[i], demographics::total);
					auto cfrac = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_key(state, state.culture_definitions.clergy)) / total;
					if(cfrac < state.defines.max_clergy_for_literacy * 0.8f && !state.world.province_get_is_colonial(state.world.state_instance_get_capital(ordered_states[i]))) {
						auto nf = state.national_definitions.clergy_focus;
						auto k = state.world.national_focus_get_limit(nf);
						if(!k || trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(n), -1)) {
							assert(command::can_set_national_focus(state, n, ordered_states[i], nf));
							state.world.state_instance_set_owner_focus(ordered_states[i], state.national_definitions.clergy_focus);
							--num_focuses_total;
						}
					}
				}
			} else {
				// If we haven't maxxed out clergy on this state, then our number 1 priority is to maximize clergy
				auto cfrac = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_key(state, state.culture_definitions.clergy)) / state.world.state_instance_get_demographics(ordered_states[i], demographics::total);
				if(cfrac < base_opt * 1.2f && !state.world.province_get_is_colonial(state.world.state_instance_get_capital(ordered_states[i]))) {
					auto nf = state.national_definitions.clergy_focus;
					auto k = state.world.national_focus_get_limit(nf);
					if(!k || trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(n), -1)) {
						assert(command::can_set_national_focus(state, n, ordered_states[i], nf));
						state.world.state_instance_set_owner_focus(ordered_states[i], state.national_definitions.clergy_focus);
						--num_focuses_total;
					}
				}
			}
		}

		for(uint32_t i = 0; num_focuses_total > 0 && i < ordered_states.size(); ++i) {
			auto prov = state.world.state_instance_get_capital(ordered_states[i]);

			if(state.world.province_get_is_colonial(state.world.state_instance_get_capital(ordered_states[i]))) {
				continue;
			}

			auto total = state.world.state_instance_get_demographics(ordered_states[i], demographics::total);
			auto pw_num = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_key(state, state.culture_definitions.primary_factory_worker));
			auto pw_employed = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
			auto sw_num = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
			auto sw_employed = state.world.state_instance_get_demographics(ordered_states[i], demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));
			auto sw_frac = sw_num / std::max(pw_num + sw_num, 1.0f);
			auto ideal_swfrac = (1.f - state.economy_definitions.craftsmen_fraction);
			// Due to floating point comparison where 2.9999 != 3, we will round the number
			// so that the ratio is NOT exact, but rather an aproximate
			if((pw_employed >= pw_num || pw_num < 1.0f)) {
				auto nf = state.national_definitions.primary_factory_worker_focus;
				auto k = state.world.national_focus_get_limit(nf);
				if(!k || trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(n), -1)) {
					// Keep balance between ratio of factory workers
					// we will only promote secondary workers if none are unemployed
					assert(command::can_set_national_focus(state, n, ordered_states[i], nf));
					state.world.state_instance_set_owner_focus(ordered_states[i], nf);
					--num_focuses_total;
				}
			} else if(pw_num > 1.0f && pw_employed > 1.0f && int8_t(sw_frac * 100.f) != int8_t(ideal_swfrac * 100.f)) {
				auto nf = state.national_definitions.secondary_factory_worker_focus;
				auto k = state.world.national_focus_get_limit(nf);
				if(!k || trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(n), -1)) {
					// Keep balance between ratio of factory workers
					// we will only promote primary workers if none are unemployed
					assert(command::can_set_national_focus(state, n, ordered_states[i], nf));
					state.world.state_instance_set_owner_focus(ordered_states[i], nf);
					--num_focuses_total;
				}
			} else {
				/* If we are a civilized nation, and we allow pops to operate on the economy
				   i.e Laissez faire, we WILL promote capitalists, since they will help to
				   build new factories for us */
				auto rules = n.get_combined_issue_rules();
				if(n.get_is_civilized() && (rules & (issue_rule::pop_build_factory | issue_rule::pop_build_factory_invest | issue_rule::pop_expand_factory | issue_rule::pop_expand_factory_invest | issue_rule::pop_open_factory | issue_rule::pop_open_factory_invest)) != 0) {
					auto nf = state.national_definitions.capitalist_focus;
					auto k = state.world.national_focus_get_limit(nf);
					if(!k || trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(n), -1)) {
						assert(command::can_set_national_focus(state, n, ordered_states[i], nf));
						state.world.state_instance_set_owner_focus(ordered_states[i], nf);
						--num_focuses_total;
					}
				} else {
					auto nf = state.national_definitions.aristocrat_focus;
					auto k = state.world.national_focus_get_limit(nf);
					if(!k || trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(n), -1)) {
						assert(command::can_set_national_focus(state, n, ordered_states[i], nf));
						state.world.state_instance_set_owner_focus(ordered_states[i], nf);
						--num_focuses_total;
					}
				}
			}
		}
	}
}

}
