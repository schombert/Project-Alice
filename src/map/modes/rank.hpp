#pragma once

std::vector<uint32_t> rank_map_from(sys::state &state) {
	// These colors are arbitrary
	// 1 to 8 -> green #30f233
	// 9 to 16 -> blue #242fff
	// under 16 but civilized -> yellow #eefc26
	// under 16 but uncivilized -> red #ff2626

	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	auto num_nations = state.world.nation_size();
	auto unciv_rank = num_nations;
	for (uint32_t i = 0; i < num_nations; ++i) {
		if (!state.world.nation_get_is_civilized(state.nations_by_rank[i])) {
			unciv_rank = i;
			break;
		}
	}

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);

		auto nation_id = fat_id.get_nation_from_province_ownership();
		auto status = nations::get_status(state, nation_id);

		float darkness = 0.0f;
		if (status == nations::status::great_power)
			darkness = 1.0f - 0.7f * (state.world.nation_get_rank(nation_id)) / state.defines.great_nations_count;
		else if (status == nations::status::secondary_power)
			darkness = 1.0f - 0.7f * (state.world.nation_get_rank(nation_id) - state.defines.great_nations_count) / (state.defines.colonial_rank - state.defines.great_nations_count);
		else if (status == nations::status::civilized)
			darkness = 1.0f - 0.7f * (state.world.nation_get_rank(nation_id) - state.defines.colonial_rank) / std::max(1.0f, (float(unciv_rank) - state.defines.colonial_rank));
		else
			darkness = 1.0f - 0.7f * (state.world.nation_get_rank(nation_id) - unciv_rank) / std::max(1.0f, (float(num_nations) - float(unciv_rank)));

		uint32_t color;
		if (bool(nation_id)) {
			switch (status) {
			case nations::status::great_power:
				color = sys::pack_color(
				    int32_t(48 * darkness),
				    int32_t(242 * darkness),
				    int32_t(51 * darkness));
				break;

			case nations::status::secondary_power:
				color = sys::pack_color(
				    int32_t(36 * darkness),
				    int32_t(47 * darkness),
				    int32_t(255 * darkness));
				break;

			case nations::status::civilized:
				color = sys::pack_color(
				    int32_t(238 * darkness),
				    int32_t(252 * darkness),
				    int32_t(38 * darkness));
				break;

				// primitive, uncivilized and westernized
			default:
				color = sys::pack_color(
				    int32_t(250 * darkness),
				    int32_t(5 * darkness),
				    int32_t(5 * darkness));
				break;
			}
		} else { // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		}

		auto i = province::to_map_id(prov_id);

		prov_color[i] = color;
		prov_color[i + texture_size] = color;
	});
	return prov_color;
}
