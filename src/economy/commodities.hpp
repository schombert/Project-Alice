#pragma once

namespace economy {

bool inline valid_need(sys::state& state, dcon::nation_id n, dcon::commodity_id c) {
	return state.world.commodity_get_is_available_from_start(c)
		|| state.world.nation_get_unlocked_commodities(n, c);
}
template<typename T>
ve::mask_vector inline valid_need(sys::state& state, T n, dcon::commodity_id c) {
	ve::mask_vector available_at_start = state.world.commodity_get_is_available_from_start(c);
	ve::mask_vector active_mask = state.world.nation_get_unlocked_commodities(n, c);
	return available_at_start || active_mask;
}

}
