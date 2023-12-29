#pragma once

std::vector<uint32_t> get_religion_global_color(sys::state& state) {
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto id = province::to_map_id(prov_id);
		float total_pops = state.world.province_get_demographics(prov_id, demographics::total);

		dcon::religion_id primary_religion, secondary_religion;
		float primary_religion_percent = 0.f, secondary_religion_percent = 0.f;

		state.world.for_each_religion([&](dcon::religion_id religion_id) {
			auto demo_key = demographics::to_key(state, religion_id);
			auto volume = state.world.province_get_demographics(prov_id, demo_key);
			float percent = volume / total_pops;

			if(percent > primary_religion_percent) {
				secondary_religion = primary_religion;
				secondary_religion_percent = primary_religion_percent;
				primary_religion = religion_id;
				primary_religion_percent = percent;
			} else if(percent > secondary_religion_percent) {
				secondary_religion = religion_id;
				secondary_religion_percent = percent;
			}
		});

		dcon::religion_fat_id fat_primary_religion = dcon::fatten(state.world, primary_religion);

		uint32_t primary_religion_color = fat_primary_religion.get_color();
		uint32_t secondary_religion_color = 0xFFAAAAAA; // This color won't be reached

		if(bool(secondary_religion)) {
			dcon::religion_fat_id fat_secondary_religion = dcon::fatten(state.world, secondary_religion);
			secondary_religion_color = fat_secondary_religion.get_color();
		}

		if(secondary_religion_percent >= .35) {
			prov_color[id] = primary_religion_color;
			prov_color[id + texture_size] = secondary_religion_color;
		} else {
			prov_color[id] = primary_religion_color;
			prov_color[id + texture_size] = primary_religion_color;
		}
	});

	return prov_color;
}

std::vector<uint32_t> get_religion_diaspora_color(sys::state& state) {
	auto fat_selected_id = dcon::fatten(state.world, state.map_state.get_selected_province());
	auto religion_id = fat_selected_id.get_dominant_religion();
	auto religion_key = demographics::to_key(state, religion_id.id);

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	if(bool(religion_id)) {
		uint32_t full_color = religion_id.get_color();
		uint32_t empty_color = 0xDDDDDD;
		// Make the other end of the gradient dark if the color is bright and vice versa.
		// This should make it easier to see religions that would otherwise be problematic.
		if((full_color & 0xFF) + (full_color >> 8 & 0xFF) + (full_color >> 16 & 0xFF) > 140 * 3) {
			empty_color = 0x222222;
		}

		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto i = province::to_map_id(prov_id);
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto total_pop = state.world.province_get_demographics(prov_id, demographics::total);
			auto religion_pop = state.world.province_get_demographics(prov_id, religion_key);
			auto ratio = religion_pop / total_pop;

			auto color = ogl::color_gradient(ratio, full_color, empty_color);

			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		});
	}
	return prov_color;
}

std::vector<uint32_t> religion_map_from(sys::state& state) {
	std::vector<uint32_t> prov_color;
	if(state.map_state.get_selected_province()) {
		prov_color = get_religion_diaspora_color(state);
	} else {
		prov_color = get_religion_global_color(state);
	}

	return prov_color;
}
