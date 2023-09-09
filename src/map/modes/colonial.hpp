#pragma once
std::vector<uint32_t> colonial_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);

		if(!(fat_id.get_nation_from_province_ownership())) {
			if(province::is_colonizing(state, state.local_player_nation, fat_id.get_state_from_abstract_state_membership())) {
				if(province::can_invest_in_colony(state, state.local_player_nation, fat_id.get_state_from_abstract_state_membership())) {
					prov_color[i] = sys::pack_color(46, 247, 15);
					prov_color[i + texture_size] = sys::pack_color(46, 247, 15);
				} else {
					prov_color[i] = sys::pack_color(250, 250, 5);
					prov_color[i + texture_size] = sys::pack_color(250, 250, 5);
				}
			} else if(province::can_start_colony(state, state.local_player_nation, fat_id.get_state_from_abstract_state_membership())) {
				prov_color[i] = sys::pack_color(46, 247, 15);
				prov_color[i + texture_size] = sys::pack_color(46, 247, 15);
			} else {
				prov_color[i] = sys::pack_color(247, 15, 15);
				prov_color[i + texture_size] = sys::pack_color(247, 15, 15);
			}
		}
	});
	return prov_color;
}
