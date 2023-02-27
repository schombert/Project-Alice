#include "map_modes.hpp"

#include "demographics.hpp"
#include "system_state.hpp"
#include "dcon_generated.hpp"
#include "province.hpp"
#include <unordered_map>

namespace map_mode {

void set_political(sys::state& state) {
	std::vector<uint32_t> prov_color(state.world.province_size() + 1);

	state.world.for_each_province([&](dcon::province_id prov_id){
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_nation_from_province_ownership();
		uint32_t color;
		if(bool(id))
			color = id.get_color();
		else // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		auto i = province::to_map_id(prov_id);
		prov_color[i] = color;
	});

	state.map_display.set_province_color(prov_color, mode::political);
}

// borrowed from http://www.burtleburtle.net/bob/hash/doobs.html
inline constexpr uint32_t scramble(uint32_t color) {
	uint32_t m1 = 0x1337CAFE;
	uint32_t m2 = 0xDEADBEEF;
	m1 -= m2; m1 -= color; m1 ^= (color>>13);
	m2 -= color; m2 -= m1; m2 ^= (m1<<8);
	color -= m1; color -= m2; color ^= (m2>>13);
	m1 -= m2; m1 -= color; m1 ^= (color>>12);
	m2 -= color; m2 -= m1; m2 ^= (m1<<16);
	color -= m1; color -= m2; color ^= (m2>>5);
	m1 -= m2; m1 -= color; m1 ^= (color>>3);
	m2 -= color; m2 -= m1; m2 ^= (m1<<10);
	color -= m1; color -= m2; color ^= (m2>>15);
	return color;
}

void set_region(sys::state& state) {
	std::vector<uint32_t> prov_color(state.world.province_size() + 1);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_abstract_state_membership();
		uint32_t color = scramble(id.get_state().id.index());
		auto i = province::to_map_id(prov_id);
		prov_color[i] = color;
	});

	state.map_display.set_province_color(prov_color, mode::region);
}

uint32_t color_gradient(float percent, uint32_t top_color, uint32_t bot_color) {
	uint32_t color = 0;
	for(uint32_t i = 0; i <= 16; i += 8) {
		auto diff = int32_t(top_color >> i & 0xFF) - int32_t(bot_color >> i & 0xFF);
		color |= uint32_t(int32_t(bot_color >> i & 0xFF) + diff * percent) << i;
	}
	return color;
}

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

	std::vector<uint32_t> prov_color(state.world.province_size() + 1);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto cid = fat_id.get_continent().id.index();
		auto i = province::to_map_id(prov_id);
		float gradient_index = prov_population[i] / continent_max_pop[cid];
		prov_color[i] = color_gradient(gradient_index, 210, 100 << 8);
	});

	return prov_color;
}

std::vector<uint32_t> get_national_population_color(sys::state& state) {
	auto fat_selected_id = dcon::fatten(state.world, province::from_map_id(state.map_display.get_selected_province()));
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

	std::vector<uint32_t> prov_color(state.world.province_size() + 1);
	for(size_t i = 0; i < prov_population.size(); i++) {
		if(prov_population[i] > -1.f) {
			float gradient_index = prov_population[i] / max_population;
			prov_color[i] = color_gradient(gradient_index, 210, 100 << 8);
		} else {
			prov_color[i] = 0xFFAAAAAA;
		}
	}

	return prov_color;
}

void set_population(sys::state& state) {
	std::vector<uint32_t> prov_color;
	if(state.map_display.get_selected_province()) {
		prov_color = get_national_population_color(state);
	} else {
		prov_color = get_global_population_color(state);
	}

	state.map_display.set_province_color(prov_color, mode::population);
}

std::vector<uint32_t> get_nationality_global_color(sys::state& state) {
	std::vector<uint32_t> prov_color(state.world.province_size() + 1);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto culture_id = fat_id.get_dominant_culture();
		
		auto i = province::to_map_id(prov_id);
		if(bool(culture_id)) {
			prov_color[i] = culture_id.get_color();
		} else {
			prov_color[i] = 0xFFAAAAAA;
		}
	});
	
	return prov_color;
}

std::vector<uint32_t> get_nationality_diaspora_color(sys::state& state) {
	auto fat_selected_id = dcon::fatten(state.world, province::from_map_id(state.map_display.get_selected_province()));
	auto culture_id = fat_selected_id.get_dominant_culture();
	auto culture_key = demographics::to_key(state, culture_id.id);

	std::vector<uint32_t> prov_color(state.world.province_size() + 1);
	if(bool(culture_id)) {
		uint32_t full_color = culture_id.get_color();
		uint32_t empty_color = 0xDDDDDD;
		// Make the other end of the gradient dark if the color is bright and vice versa.
		// This should make it easier to see cultures that would otherwise be problematic.
		if((full_color & 0xFF) > 128 || (full_color >> 8 & 0xFF) > 128 || (full_color >> 16 & 0xFF) > 128) {
			empty_color = 0x222222;
		}

		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto i = province::to_map_id(prov_id);
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto total_pop = state.world.province_get_demographics(prov_id, demographics::total);
			auto culture_pop = state.world.province_get_demographics(prov_id, culture_key);
			auto ratio = culture_pop / total_pop;

			prov_color[i] = color_gradient(ratio, full_color, empty_color);
		});
	}
	return prov_color;
}

void set_nationality(sys::state& state) {
	std::vector<uint32_t> prov_color;
	if(state.map_display.get_selected_province()) {
		prov_color = get_nationality_diaspora_color(state);
	} else {
		prov_color = get_nationality_global_color(state);
	}
	
	state.map_display.set_province_color(prov_color, mode::nationality);
}

void set_map_mode(sys::state& state, mode mode) {
	switch (mode)
	{
	case mode::terrain:
		state.map_display.set_terrain_map_mode();
		break;
	case mode::political:
		set_political(state);
		break;
	case mode::region:
		set_region(state);
		break;
	case mode::population:
		set_population(state);
		break;
	case mode::nationality:
		set_nationality(state);
		break;
	default:
		break;
	}
}

void update_map_mode(sys::state& state) {
	if(state.map_display.active_map_mode == mode::terrain || state.map_display.active_map_mode == mode::region) {
		return;
	}
	set_map_mode(state, state.map_display.active_map_mode);
}
}
