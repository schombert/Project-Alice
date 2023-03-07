#include "map_modes.hpp"

#include "demographics.hpp"
#include "system_state.hpp"
#include "dcon_generated.hpp"
#include "province.hpp"
#include "nations.hpp"
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

std::vector<uint32_t> get_global_sphere_color(sys::state& state) {
	std::vector<uint32_t> prov_color(state.world.province_size() + 1);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);

		auto owner = fat_id.get_nation_from_province_ownership();

		if(bool(owner)) {
			// Currently there's no simple way to check if a nation is a great power.
			bool is_great_power = false;
			owner.for_each_gp_relationship_as_great_power([&](dcon::gp_relationship_id rel_id) {
				if(!is_great_power) is_great_power = true;
			});

			if(is_great_power) {
				prov_color[i] = owner.get_color();
			} else {
				auto master = owner.get_in_sphere_of();
				if(bool(master)) {
					prov_color[i] = master.get_color();
				} else {
					prov_color[i] = 0xFFAAAAAA;
				}
			}
		} else {
			prov_color[i] = 0x222222;
		}
	});

	return prov_color;
}

bool is_great_power(dcon::nation_fat_id nation) {
	bool is_great_power = false;
	nation.for_each_gp_relationship_as_great_power([&](dcon::gp_relationship_id rel_id) {
		if(!is_great_power) is_great_power = true;
	});
	return is_great_power;
}

std::vector<uint32_t> get_selected_sphere_color(sys::state& state) {
	// Not sure if these are the real colors
	uint32_t gp_color = 0x3CB43C;
	uint32_t influence_color = 0xAA5064;
	uint32_t sphere_color = 0x50A5A5;
	uint32_t other_influence_color = 0xDC6E6E;

	std::vector<uint32_t> prov_color(state.world.province_size() + 1);

	auto fat_selected_id = dcon::fatten(state.world, province::from_map_id(state.map_display.get_selected_province()));
	auto selected_nation = fat_selected_id.get_nation_from_province_ownership();

	if(is_great_power(selected_nation)) {
		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto i = province::to_map_id(prov_id);

			auto owner = fat_id.get_nation_from_province_ownership();
			if(owner.id == selected_nation.id) {
				// Province is owned by the GP
				prov_color[i] = gp_color;
			} else if(bool(owner)) {
				bool is_in_sphere = false;
				bool is_in_influence = false;

				auto rel_id = state.world.get_gp_relationship_by_gp_influence_pair(owner, selected_nation);
				auto relationship_status = state.world.gp_relationship_get_status(rel_id);
				if(relationship_status == nations::influence::level_in_sphere) {
					is_in_sphere = true;
				} else if(relationship_status != nations::influence::level_neutral && state.world.gp_relationship_get_influence(rel_id) != 0) {
					is_in_influence = true;
				}

				if(is_in_sphere) {
					prov_color[i] = sphere_color;
				} else if(is_in_influence) {
					prov_color[i] = influence_color;
				} else {
					prov_color[i] = 0xFFAAAAAA;
				}
			} else {
				prov_color[i] = 0x222222;
			}
		});
	}

	return prov_color;
}

void set_sphere(sys::state& state) {
	std::vector<uint32_t> prov_color;

	if(state.map_display.get_selected_province()) {
		prov_color = get_selected_sphere_color(state);
	} else {
		prov_color = get_global_sphere_color(state);
	}

	state.map_display.set_province_color(prov_color, mode::sphere);
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
	case mode::sphere:
		set_sphere(state);
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
