#pragma once
#include <algorithm>

struct party_info {
	dcon::ideology_id ideology;
	float loyalty;
	uint32_t color;
};

std::vector<party_info> get_sorted_parties_info(sys::state &state, dcon::province_id prov_id) {
	std::vector<party_info> result;

	state.world.for_each_ideology([&](dcon::ideology_id ideology) {
		auto loyalty = state.world.province_get_party_loyalty(prov_id, ideology);
		if (loyalty > 0) {
			result.push_back({ideology, loyalty, state.world.ideology_get_color(ideology)});
		}
	});

	std::sort(result.begin(), result.end(), [&](party_info a, party_info b) { return a.loyalty > b.loyalty; });

	return result;
}

std::vector<uint32_t> party_loyalty_map_from(sys::state &state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		if (province::has_an_owner(state, prov_id)) {
			auto parties_info = get_sorted_parties_info(state, prov_id);

			auto i = province::to_map_id(prov_id);

			if (parties_info.size() == 0) {

				prov_color[i] = 0xFFFFFF;
				prov_color[i + texture_size] = 0xFFFFFF;

			} else if (parties_info.size() == 1) {

				uint32_t color = parties_info[0].color;
				prov_color[i] = color;
				prov_color[i + texture_size] = color;

			} else {
				party_info a = parties_info[0];
				party_info b = parties_info[1];

				prov_color[i] = a.color;

				if (b.loyalty >= a.loyalty * 0.75) {
					prov_color[i + texture_size] = b.color;
				} else {
					prov_color[i + texture_size] = a.color;
				}
			}
		}
	});

	return prov_color;
}
