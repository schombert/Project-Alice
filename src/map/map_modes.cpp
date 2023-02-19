#include "map_modes.hpp"

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
			color = id.get_identity_from_identity_holder().get_color();
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

std::vector<uint32_t> get_global_population_color(sys::state& state) {
	std::vector<float> prov_population(state.world.province_size() + 1);
	std::unordered_map<int32_t, float> continent_max_pop = {};

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		float population = 0;
		for(auto pop_location : fat_id.get_pop_location_as_province()) {
			population += pop_location.get_pop().get_size();
		}
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
		uint32_t color = uint32_t(100.f * (1.f - gradient_index)) << 8;
		color |= uint32_t(210.f * gradient_index);
		prov_color[i] = color;
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
			float population = 0;
			for(auto pop_location : fat_id.get_pop_location_as_province()) {
				population += pop_location.get_pop().get_size();
			}
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
			uint32_t color = uint32_t(135.f * (1.f - gradient_index)) << 8;
			color |= uint32_t(210.f * gradient_index);
			prov_color[i] = color;
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
	default:
		break;
	}
}
}
