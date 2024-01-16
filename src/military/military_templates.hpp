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
} // namespace military