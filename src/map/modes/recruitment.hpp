#pragma once

std::vector<uint32_t> recruitment_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();

		if(nation == state.local_player_nation) {
			auto max_regiments = military::regiments_max_possible_from_province(state, prov_id);
			auto created_regiments = military::regiments_created_from_province(state, prov_id);

			uint32_t color;
			if(max_regiments == 0) {
				// grey
				color = sys::pack_color(155, 156, 149);
			} else if(created_regiments < max_regiments) {
				// yellow
				color = sys::pack_color(212, 214, 62);
			} else {
				// green
				color = sys::pack_color(53, 196, 53);
			}
			auto i = province::to_map_id(prov_id);

			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}

	});

	return prov_color;
}
