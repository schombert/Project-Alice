#pragma once

std::vector<uint32_t> political_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_nation_from_province_ownership();
		uint32_t color = 0;
		if(bool(id)) {
			color = id.get_color();
			if(auto ovr = state.world.nation_get_overlord_as_subject(id); ovr) {
				switch(state.user_settings.vassal_color) {
				case sys::map_vassal_color_mode::inherit: //colour is interpolated from the owner and saved
					break;
				case sys::map_vassal_color_mode::same:
					color = state.world.nation_get_color(state.world.overlord_get_ruler(ovr));
					break;
				case sys::map_vassal_color_mode::none:
					color = state.world.national_identity_get_color(state.world.nation_get_identity_from_identity_holder(id));
					break;
				}
			}
		} else { // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		}
		auto i = province::to_map_id(prov_id);

		uint32_t color_b = id ? sys::pack_color(127, 127, 127) : sys::pack_color(255, 255, 255);
		if(auto occupier = fat_id.get_nation_from_province_control(); occupier) {
			color_b = occupier.get_color();
			if(auto ovr = state.world.nation_get_overlord_as_subject(occupier);  ovr) {
				switch(state.user_settings.vassal_color) {
				case sys::map_vassal_color_mode::inherit: //colour is interpolated from the owner and saved
					break;
				case sys::map_vassal_color_mode::same:
					color_b = state.world.nation_get_color(state.world.overlord_get_ruler(ovr));
					break;
				case sys::map_vassal_color_mode::none:
					color_b = state.world.national_identity_get_color(state.world.nation_get_identity_from_identity_holder(occupier));
					break;
				}
			}
		}

		prov_color[i] = color;
		prov_color[i + texture_size] = color_b;
	});

	return prov_color;
}
