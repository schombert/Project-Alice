#pragma once

std::vector<uint32_t> relation_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	auto selected_province = state.map_state.get_selected_province();
	auto fat_id = dcon::fatten(state.world, selected_province);
	auto selected_nation = fat_id.get_nation_from_province_ownership();

	if(!selected_nation) {
		selected_nation = state.local_player_nation;
	}

	auto relations = selected_nation.get_diplomatic_relation_as_related_nations();

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto other_nation = state.world.province_get_nation_from_province_ownership(prov_id);

		// if the province has no owners
		if(!other_nation) {
			return;
		}

		uint32_t color;

		if(other_nation == selected_nation.id) {
			// the selected nation should be blue
			color = sys::pack_color(66, 106, 227);
		} else {
			auto diplo_relation = state.world.get_diplomatic_relation_by_diplomatic_pair(other_nation, selected_nation);
			auto relation_value = state.world.diplomatic_relation_get_value(diplo_relation);

			float interpolation = (200 + relation_value) / 400.f;

			color = ogl::color_gradient(interpolation, sys::pack_color(46, 247, 15), // green
					sys::pack_color(247, 15, 15)	// red
			);
		}

		auto i = province::to_map_id(prov_id);

		prov_color[i] = color;
		prov_color[i + texture_size] = color;
	});

	return prov_color;
}
