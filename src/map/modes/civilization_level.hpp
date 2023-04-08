#pragma once

std::vector<uint32_t> civilization_level_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		auto status = nations::get_status(state, nation);

		uint32_t color;

		// if it is uncolonized
		if(!nation) {

			color = sys::pack_color(250, 5, 5);// red

		} else if(state.world.nation_get_is_civilized(nation)) {

			color = sys::pack_color(53, 196, 53);// green

		} else {

			float civ_level = state.world.nation_get_modifier_values(nation, sys::national_mod_offsets::civilization_progress_modifier);
			// gray <-> yellow
			color = ogl::color_gradient(civ_level * (1 + (1 - civ_level)), sys::pack_color(250, 250, 5), sys::pack_color(64, 64, 64));

		}
		auto i = province::to_map_id(prov_id);

		prov_color[i] = color;
		if(!state.world.province_get_is_colonial(prov_id)) {
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
