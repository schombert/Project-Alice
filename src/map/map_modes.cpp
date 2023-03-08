#include "map_modes.hpp"

#include "demographics.hpp"
#include "system_state.hpp"
#include "dcon_generated.hpp"
#include "province.hpp"
#include "nations.hpp"
#include <unordered_map>

namespace map_mode {

void set_political(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_nation_from_province_ownership();
		uint32_t color;
		if(bool(id))
			color = id.get_color();
		else // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		auto i = province::to_map_id(prov_id);

		prov_color[i] = color;
		prov_color[i + texture_size] = color;

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
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = fat_id.get_abstract_state_membership();
		uint32_t color = scramble(id.get_state().id.index());
		auto i = province::to_map_id(prov_id);
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
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

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto cid = fat_id.get_continent().id.index();
		auto i = province::to_map_id(prov_id);
		float gradient_index = prov_population[i] / continent_max_pop[cid];

		auto color = color_gradient(gradient_index, 210, 100 << 8);
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
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

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	for(size_t i = 0; i < prov_population.size(); i++) {
		uint32_t color = 0xFFAAAAAA;
		if(prov_population[i] > -1.f) {
			float gradient_index = prov_population[i] / max_population;
			color = color_gradient(gradient_index, 210, 100 << 8);
		}
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
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
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto id = province::to_map_id(prov_id);
		auto total_pops = state.world.province_get_demographics(prov_id, demographics::total);

		dcon::culture_id primary_culture, secondary_culture;
		float primary_culture_percent = 0.f, secondary_culture_percent = 0.f;

		state.world.for_each_culture([&](dcon::culture_id culture_id) {
			auto demo_key = demographics::to_key(state, culture_id);
			auto volume = state.world.province_get_demographics(prov_id, demo_key);
			float percent = volume / total_pops;

			if(percent > primary_culture_percent) {
				secondary_culture = primary_culture;
				secondary_culture_percent = primary_culture_percent;
				primary_culture = culture_id;
				primary_culture_percent = percent;
			} else if(percent > secondary_culture_percent) {
				secondary_culture = culture_id;
				secondary_culture_percent = percent;
			}
		});

		dcon::culture_fat_id fat_primary_culture = dcon::fatten(state.world, primary_culture);

		uint32_t primary_culture_color = fat_primary_culture.get_color();
		uint32_t secondary_culture_color = 0xFFAAAAAA; // This color won't be reached

		if(bool(secondary_culture)) {
			dcon::culture_fat_id fat_secondary_culture = dcon::fatten(state.world, secondary_culture);
			secondary_culture_color = fat_secondary_culture.get_color();
		}

		if(secondary_culture_percent >= .35) {
			prov_color[id] = primary_culture_color;
			prov_color[id + texture_size] = secondary_culture_color;
		} else {
			prov_color[id] = primary_culture_color;
			prov_color[id + texture_size] = primary_culture_color;
		}

	});

	
	return prov_color;
}

std::vector<uint32_t> get_nationality_diaspora_color(sys::state& state) {
	auto fat_selected_id = dcon::fatten(state.world, province::from_map_id(state.map_display.get_selected_province()));
	auto culture_id = fat_selected_id.get_dominant_culture();
	auto culture_key = demographics::to_key(state, culture_id.id);

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

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

			auto color = color_gradient(ratio, full_color, empty_color);

			prov_color[i] = color;
			prov_color[i + texture_size] = color;
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

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto i = province::to_map_id(prov_id);

		auto owner = fat_id.get_nation_from_province_ownership();

		uint32_t color = 0x222222;

		if(bool(owner)) {
			// Currently there's no simple way to check if a nation is a great power.
			bool is_great_power = false;
			owner.for_each_gp_relationship_as_great_power([&](dcon::gp_relationship_id rel_id) {
				if(!is_great_power) is_great_power = true;
			});

			if(is_great_power) {
				color = owner.get_color();
			} else {
				auto master = owner.get_in_sphere_of();
				if(bool(master)) {
					color = master.get_color();
				}
			}
		}
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
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
	/**
	 * Color logic
	 *	- GP -> Green
	 *  - In Sphere -> Yellow
	 *	- Is influenced -> Blue
	 *  - Is influenced by more than 1 -> Add Red Stripes
	 *
	 *  The concept of influenced is weird, if a tag has cordial relations with a gp but the influence is 0, then its not influenced
	 *  Only the influence value should be taken into account
	 *
	 * If country is in sphere or is sphere master
	 *	Paint only the tags influenced by the master
	 * Else
	 *	Paint the tags that influence the country + the country
	**/

	// Todo: Get better colors?
	uint32_t gp_color = 0x00FF00;        // Green
	uint32_t inf_color = 0xFF0000;       // Blue
	uint32_t sphere_color = 0x00FFFF;    // Yellow
	uint32_t other_inf_color = 0x0000FF; // Red

	// Province color vector init
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);

	auto fat_selected_id = dcon::fatten(state.world, province::from_map_id(state.map_display.get_selected_province()));
	auto selected_nation = fat_selected_id.get_nation_from_province_ownership();

	// Get sphere master if exists
	auto master = selected_nation.get_in_sphere_of();
	if(!bool(master) && is_great_power(selected_nation)) {
		master = selected_nation;
	}

	// Paint only sphere countries
	if(bool(master)) {
		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto i = province::to_map_id(prov_id);

			uint32_t color = 0x222222;
			uint32_t stripe_color = 0x222222;

			auto owner = fat_id.get_nation_from_province_ownership();

			if(bool(owner)) {
				if(owner.id == master.id) {
					color = gp_color;
				} else {
					auto gp_rel_iter = owner.get_gp_relationship_as_influence_target();
					
					auto master_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(owner, master);
					if(bool(master_rel_id)) {
						auto master_rel_status = state.world.gp_relationship_get_status(master_rel_id);
						auto master_rel_inf = state.world.gp_relationship_get_influence(master_rel_id);

						if(master_rel_status == nations::influence::level_in_sphere || master_rel_inf != 0) {
							owner.for_each_gp_relationship_as_influence_target([&](dcon::gp_relationship_id rel_id) {
								// Has more than one influencer
								if(rel_id != master_rel_id && state.world.gp_relationship_get_influence(rel_id) != 0 && state.world.gp_relationship_get_influence(rel_id) != 0) {
									stripe_color = other_inf_color;
								}
							});

							if(master_rel_status == nations::influence::level_in_sphere) {
								color = sphere_color;
							} else if(master_rel_inf != 0) {
								color = inf_color;
							}
						}
					}
				}
			}

			// If no stripe has been set, use the prov color
			if(stripe_color == 0x222222) {
				stripe_color = color;
			}

			prov_color[i] = color;
			prov_color[i + texture_size] = stripe_color;
		});
	} else { // Paint selected country and influencers
		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto i = province::to_map_id(prov_id);

			uint32_t color = 0x222222;
			uint32_t stripe_color = 0x222222;

			auto owner = fat_id.get_nation_from_province_ownership();
			if(bool(owner)) {
				bool is_gp = false;
				uint8_t rel_count = 0;

				selected_nation.for_each_gp_relationship_as_influence_target([&](dcon::gp_relationship_fat_id relation_id) {
					if(!is_gp && relation_id.get_influence() > 0) {
						if(owner.id == selected_nation.id) {
							rel_count++;
						}
						if(relation_id.get_great_power() == owner.id) {
							is_gp = true;
						}
					}
				});

				if(is_gp) {
					color = gp_color;
				} else if(rel_count >= 1) {
					color = inf_color;
					if(rel_count > 1) {
						stripe_color = other_inf_color;
					}
				}
			}

			// If no stripe has been set, use the prov color
			if(stripe_color == 0x222222) {
				stripe_color = color;
			}

			prov_color[i] = color;
			prov_color[i + texture_size] = stripe_color;
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
