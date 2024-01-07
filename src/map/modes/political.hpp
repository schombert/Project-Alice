#pragma once
#include "prng.hpp"

uint32_t derive_color_from_ol_color(sys::state& state, uint32_t ol_color, dcon::nation_id n) {
	auto base = sys::rgb_to_hsv(ol_color);
	auto roff = rng::get_random_pair(state, uint32_t(n.index()), uint32_t(n.index()));
	base.h = fmod(base.h + (float(roff.low & 0x1F) - 15.5f), 360.0f);
	base.s = std::clamp(base.s + (float((roff.low >> 8) & 0xFF) / 255.0f) * 0.2f - 0.1f, 0.0f, 1.0f);
	base.v = std::clamp(base.v + (float((roff.high >> 4) & 0xFF) / 255.0f) * 0.2f - 0.1f, 0.0f, 1.0f);
	return sys::hsv_to_rgb(base);
}

std::vector<uint32_t> political_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	std::vector<uint32_t> nation_color(state.world.nation_size() + 1);
	state.world.for_each_nation([&](dcon::nation_id n) {
		nation_color[n.value] = state.world.nation_get_color(n);
		auto olr = state.world.nation_get_overlord_as_subject(n);
		if(auto ol = state.world.overlord_get_ruler(olr); ol) {
			auto ol_color = state.world.nation_get_color(ol);
			switch(state.user_settings.vassal_color) {
			case sys::map_vassal_color_mode::inherit:
				nation_color[n.value] = derive_color_from_ol_color(state, ol_color, n);
				break;
			case sys::map_vassal_color_mode::same:
				nation_color[n.value] = ol_color;
				break;
			case sys::map_vassal_color_mode::none:
				break;
			}
		}
	});

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_nation_from_province_ownership();
		uint32_t color = 0;
		if(bool(id)) {
			color = nation_color[id.id.value];
		} else { // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		}
		auto i = province::to_map_id(prov_id);

		auto occupier = fat_id.get_nation_from_province_control();
		uint32_t color_b = occupier ? nation_color[occupier.id.value] :
			(id ? sys::pack_color(127, 127, 127) : sys::pack_color(255, 255, 255));

		prov_color[i] = color;
		prov_color[i + texture_size] = color_b;
	});

	return prov_color;
}
