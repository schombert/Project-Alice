#pragma once

std::vector<uint32_t> admin_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();

		if(nation == state.local_player_nation) {
			auto admin_efficiency = province::state_admin_efficiency(state, fat_id.get_state_membership());

			uint32_t color = ogl::color_gradient(
			    admin_efficiency,
			    sys::pack_color(46, 247, 15), // red
			    sys::pack_color(247, 15, 15)  // green
			);

			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
