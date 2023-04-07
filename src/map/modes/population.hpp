#pragma once

std::vector<uint32_t> get_global_population_color(sys::state& state) {
	std::vector<float> prov_population(state.world.province_size() + 1);
	std::unordered_map<int32_t, float> continent_max_pop = {};

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		float population = state.world.province_get_demographics(prov_id, demographics::total);
		auto cid = fat_id.get_continent().id.index();
		continent_max_pop[cid] = std::max(continent_max_pop[cid], population);
		auto i = province::to_map_id(prov_id);
		prov_population[i] = population;
	});

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto cid = fat_id.get_continent().id.index();
		auto i = province::to_map_id(prov_id);
		float gradient_index = prov_population[i] / continent_max_pop[cid];

		auto color = ogl::color_gradient(gradient_index, 210, 100 << 8);
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
	});

	return prov_color;
}

std::vector<uint32_t> get_national_population_color(sys::state& state) {
	auto fat_selected_id = dcon::fatten(state.world, state.map_display.get_selected_province());
	auto nat_id = fat_selected_id.get_nation_from_province_ownership();
	if(!bool(nat_id)) {
		return get_global_population_color(state);
	}
	float max_population = 0.f;
	std::vector<float> prov_population(state.world.province_size() + 1);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);
		if(fat_id.get_nation_from_province_ownership().id == nat_id.id) {
			float population = state.world.province_get_demographics(prov_id, demographics::total);
			max_population = std::max(max_population, population);
			prov_population[i] = population;
		} else {
			prov_population[i] = -1.f;
		}
	});

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	for(size_t i = 0; i < prov_population.size(); i++) {
		uint32_t color = 0xFFAAAAAA;
		if(prov_population[i] > -1.f) {
			float gradient_index = prov_population[i] / max_population;
			color = ogl::color_gradient(gradient_index, 210, 100 << 8);
		}
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
	}

	return prov_color;
}

std::vector<uint32_t> population_map_from(sys::state& state) {
	std::vector<uint32_t> prov_color;
	if(state.map_display.get_selected_province()) {
		prov_color = get_national_population_color(state);
	} else {
		prov_color = get_global_population_color(state);
	}

	return prov_color;
}
