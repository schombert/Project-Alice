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
		auto ol = state.world.overlord_get_ruler(olr);
		auto ol_temp = ol;

		while(ol) {
			olr = state.world.nation_get_overlord_as_subject(ol);
			ol_temp = ol;
			ol = state.world.overlord_get_ruler(olr);
		} ol = ol_temp;

		if(ol) {
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
		auto i = province::to_map_id(prov_id);
		if(prov_id.index() >= state.province_definitions.first_sea_province.index()) {
			prov_color[i] = 0;
			prov_color[i + texture_size] = 0;

			dcon::nation_id first_n{};
			dcon::nation_id second_n{};
			for(const auto adj : fat_id.get_province_adjacency_as_connected_provinces()) {
				auto p2 = adj.get_connected_provinces(adj.get_connected_provinces(0) == prov_id ? 1 : 0);
				if(p2.get_is_coast()) {
					auto n = p2.get_province_control_as_province().get_nation();
					if(!n || second_n == n || first_n == n)
						continue;
					if(!bool(second_n) || n.get_rank() > state.world.nation_get_rank(second_n)) {
						if(!bool(first_n) || n.get_rank() > state.world.nation_get_rank(first_n)) {
							second_n = first_n;
							first_n = n;
						} else {
							second_n = n;
						}
					}
				}
			}

			if(first_n) {
				prov_color[i] = nation_color[first_n.value];
				prov_color[i] |= 0xff000000;
				if(second_n) {
					prov_color[i + texture_size] = nation_color[second_n.value];
					prov_color[i + texture_size] |= 0xff000000;
				}
			}
		} else {
			auto id = fat_id.get_nation_from_province_ownership();
			uint32_t color = 0;
			if(bool(id)) {
				color = nation_color[id.id.value];
			} else { // If no owner use default color
				color = 255 << 16 | 255 << 8 | 255;
			}
			auto occupier = fat_id.get_nation_from_province_control();
			auto rebel_faction = fat_id.get_rebel_faction_from_province_rebel_control().id;
			uint32_t color_b;
			uint32_t rebel_color = sys::pack_color(127, 127, 127);
			if(rebel_faction) {
				dcon::rebel_type_fat_id rtype = state.world.rebel_faction_get_type(rebel_faction);
				dcon::ideology_fat_id ideology = rtype.get_ideology();
				if(ideology)
					rebel_color = ideology.get_color();
			}
			color_b = occupier ? nation_color[occupier.id.value] :
				(id ? rebel_color : sys::pack_color(255, 255, 255));

			prov_color[i] = color;
			prov_color[i + texture_size] = color_b;
		}
	});

	return prov_color;
}
