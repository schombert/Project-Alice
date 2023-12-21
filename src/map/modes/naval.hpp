#pragma once

#include <vector>

std::vector<uint32_t> naval_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();

		if(nation == state.local_player_nation) {
			uint32_t color = 0x222222;
			uint32_t stripe_color = 0x222222;

			if(province::has_naval_base_being_built(state, prov_id)) {
				color = 0x00FF00;
				stripe_color = 0x005500;
			} else if(province::can_build_naval_base(state, prov_id, state.local_player_nation)) {
				if(state.world.province_get_building_level(prov_id, economy::province_building_type::naval_base) != 0) {
					color = 0x00FF00;
					stripe_color = 0x00FF00;
				} else {
					color = sys::pack_color(50, 150, 200);
					stripe_color = sys::pack_color(50, 150, 200);
				}
			} else if(state.world.province_get_building_level(prov_id, economy::province_building_type::naval_base) != 0) {
				color = 0x005500;
				stripe_color = 0x005500;
			} else { // no naval base, not build target

			}

			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = stripe_color;
		}
	});

	return prov_color;
}
