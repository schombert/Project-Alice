#pragma once
#include "system_state.hpp"

namespace military {

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids) {
	return state.world.province_get_is_blockaded(ids);
}

template<typename T>
auto province_is_under_siege(sys::state const& state, T ids) {
	return state.world.province_get_siege_progress(ids) > 0.0f;
}

template<typename T>
auto battle_is_ongoing_in_province(sys::state const& state, T ids) {
	ve::apply(
			[&](dcon::province_id p) {
				auto battles = state.world.province_get_land_battle_location(p);
				return battles.begin() != battles.end();
			},
			ids);
	return false;
}

// Calculates whether province can support more regiments
// Considers existing regiments and construction as well
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, F&& filter) {

	float divisor = 0;

	if(state.world.province_get_is_colonial(p)) {
		divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
	}
	else if(!state.world.province_get_is_owner_core(p)) {
		divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
	}
	else {
		divisor = state.defines.pop_size_per_regiment;
	}

	for(auto pop : state.world.province_get_pop_location(p)) {
		if(filter(state, pop.get_pop())) {
			if(can_pop_form_regiment(state, pop.get_pop(), divisor)) {
				return pop.get_pop().id;

			}
		}
	}
	return dcon::pop_id{};
	
}



// Finds a pop which can support more regiments anywhere in the target nation
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier_anywhere(sys::state& state, dcon::nation_id nation, F&& filter) {
	if(!nation) {
		// can't find a pop for the invalid/rebel tag
		return dcon::pop_id{ };
	}

	for(auto p : state.world.nation_get_province_ownership(nation)) {
		auto prov = p.get_province();
		if(state.world.province_get_nation_from_province_control(prov) == nation) {
			auto pop = find_available_soldier(state, prov, filter);
			if(bool(pop)) {
				return pop;
			}
		}
	}
	return dcon::pop_id{};

}



// Calculates whether province can support more regiments when parsing OOBs
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier_parsing(sys::state& state, dcon::province_id province_id, F&& filter) {
	float divisor = state.defines.pop_size_per_regiment;

	for(auto pop : state.world.province_get_pop_location(province_id)) {
		if(filter(state, pop.get_pop())) {
			if(can_pop_form_regiment(state, pop.get_pop(), divisor)) {
				return pop.get_pop().id;

			}
		}
	}
	return dcon::pop_id{ };
};

} // namespace military
