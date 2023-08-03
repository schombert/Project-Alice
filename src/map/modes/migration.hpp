#pragma once

std::vector<uint32_t> migration_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		// The province should have an owner
		if(state.world.province_get_nation_from_province_ownership(prov_id)) {

			auto immigrant_attraction = state.world.province_get_modifier_values(prov_id, sys::provincial_mod_offsets::immigrant_attract);
			float interpolation = (immigrant_attraction + 1) / 2;

			uint32_t color = ogl::color_gradient(interpolation, sys::pack_color(46, 247, 15), // red
				sys::pack_color(247, 15, 15)												  // green
			);
			auto i = province::to_map_id(prov_id);

			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
