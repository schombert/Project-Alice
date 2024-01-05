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
			switch(state.user_settings.vassal_color) {
			case sys::map_vassal_color_mode::inherit:
				//colour is interpolated from the owner and saved
				break;
			case sys::map_vassal_color_mode::same:
			{
				auto ovr = state.world.nation_get_overlord_as_subject(id);
				if(ovr) {
					auto ruler = state.world.overlord_get_ruler(ovr);
					color = state.world.nation_get_color(id);
				}
				break;
			}
			case sys::map_vassal_color_mode::none:
			{
				auto ovr = state.world.nation_get_overlord_as_subject(id);
				if(ovr) {
					color = state.world.national_identity_get_color(state.world.nation_get_identity_from_identity_holder(id));
				}
				break;
			}
			}
		} else // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		auto i = province::to_map_id(prov_id);

		auto occupier = fat_id.get_nation_from_province_control();
		uint32_t color_b = occupier ? occupier.get_color() : (id ? sys::pack_color(127, 127, 127) : sys::pack_color(255, 255, 255));

		prov_color[i] = color;
		prov_color[i + texture_size] = color_b;
	});

	return prov_color;
}
