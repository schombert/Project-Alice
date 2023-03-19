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
		auto id = province::to_map_id(prov_id);
		float total_pops = state.world.province_get_demographics(prov_id, demographics::total);

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
	 *	Paint only the tags influenced by the master, including spherearlings
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

std::vector<uint32_t> get_selected_diplomatic_color(sys::state& state) {
	/**
	 * Color:
	 *	- Yellorange -> Casus belli TODO: How do I get the casus belli?
	 *  - Skyblue -> Ally -> X
	 *  - Green stripes -> Cores X
	 *  - White stripes -> Not cores X
	 *  - Yellow stripes -> Nation cultural union X
	 *  - Red -> War X
	 *  - Green -> Selected X
	 *  - Light Green -> Sphereling X
	 *  - Dark green -> Puppet or puppet master X
	 */

	// This could be stored in an other place
	uint32_t causus_belli_color = 0x00AAFF;
	uint32_t ally_color = 0xFFAA00;
	uint32_t selected_color = 0x00FF00; // Also unclaimed cores stripes color
	uint32_t sphere_color = 0x55AA55;
	uint32_t puppet_color = 0x009900;
	uint32_t non_cores_color = 0xFFFFFF;
	uint32_t war_color = 0x0000FF;
	uint32_t cultural_union_color = 0x00FFFF;

	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);

	auto fat_selected_id = dcon::fatten(state.world, province::from_map_id(state.map_display.get_selected_province()));
	auto selected_nation = fat_selected_id.get_nation_from_province_ownership();
	
	std::vector<dcon::nation_id> enemies, allies, sphere;

	if(bool(selected_nation)) {

		// Get all enemies
		for(auto wa : state.world.nation_get_war_participant(selected_nation)) {
			bool is_attacker = wa.get_is_attacker();
			for(auto o : wa.get_war().get_war_participant()) {
				if(o.get_is_attacker() != is_attacker) {
					enemies.push_back(o.get_nation().id);
				}
			}
		}

		// Get all allies
		selected_nation.for_each_diplomatic_relation([&](dcon::diplomatic_relation_fat_id relation_id) {
			if(relation_id.get_are_allied()) {
				dcon::nation_id ally_id = relation_id.get_related_nations(0).id != selected_nation.id ? relation_id.get_related_nations(0).id : relation_id.get_related_nations(1).id;
				allies.push_back(ally_id);
			}
		});

		auto selected_primary_culture = selected_nation.get_primary_culture();

		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto i = province::to_map_id(prov_id);
			uint32_t color = 0x222222;
			uint32_t stripe_color = 0x222222;

			auto fat_owner = fat_id.get_province_control().get_nation();  // hehe

			if(bool(fat_owner)) {

				// Selected nation
				if(fat_owner.id == selected_nation.id) {
					color = selected_color;
					// By default asume its not a core
					stripe_color = non_cores_color;
				} else {
					// Cultural Union
					auto cultural_union_identity = selected_primary_culture.get_culture_group_membership().get_group().get_identity_from_cultural_union_of();
					fat_id.for_each_core([&](dcon::core_fat_id core_id) {
						if(core_id.get_identity().id == cultural_union_identity.id) {
							stripe_color = cultural_union_color;
						}
					});

					// Sphere
					auto master_rel_id = state.world.get_gp_relationship_by_gp_influence_pair(fat_owner, selected_nation);
					if(bool(master_rel_id) && state.world.gp_relationship_get_status(master_rel_id) == nations::influence::level_in_sphere) {
						color = sphere_color;
					}

					// Puppets
					auto province_overlord_id = fat_id.get_province_ownership().get_nation().get_overlord_as_subject();
					if(bool(province_overlord_id) && province_overlord_id.get_ruler().id == selected_nation.id) {
						color = puppet_color;
					} else {
						auto selected_overlord_id = selected_nation.get_overlord_as_subject();
						if(bool(selected_overlord_id) && selected_overlord_id.get_ruler().id == fat_owner.id) {
							color = puppet_color;
						}
					}

					// War
					if(std::find(enemies.begin(), enemies.end(), fat_owner.id) != enemies.end()) {
						color = war_color;
					}

					// Allies
					if(std::find(allies.begin(), allies.end(), fat_owner.id) != allies.end()) {
						color = ally_color;
					}
				}

				// Core
				fat_id.for_each_core([&](dcon::core_fat_id core_id) {
					if(core_id.get_identity().get_nation_from_identity_holder().id == selected_nation.id) {
						stripe_color = selected_color;
					}
				});
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

void set_diplomatic(sys::state& state) {
	std::vector<uint32_t> prov_color;

	if(state.map_display.get_selected_province()) {
		prov_color = get_selected_diplomatic_color(state);
	} else {
		prov_color = get_selected_diplomatic_color(state);
	}

	state.map_display.set_province_color(prov_color, mode::diplomatic);
}

void set_rank(sys::state& state) {
	// These colors are arbitrary
	// 1 to 8 -> green from #30f233 to #1f991f
	// 9 to 16 -> blue from #242fff to #151c99
	// under 16 but civilized -> yellow from #eefc26 to #54590d
	// under 16 but uncivilized -> red from #ff2626 to #590d0d

	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation_id = fat_id.get_nation_from_province_ownership();
		auto status = nations::get_status(state, nation_id);


		//uint16_t nation_rank = state.world.nation_get_rank(nation_id);


		uint32_t color;
		if(bool(nation_id))
			switch(status) {
				case nations::status::great_power:
					color = sys::pack_color(48, 242, 51);
					break;

				case nations::status::secondary_power:
					color = sys::pack_color(36, 47, 255);
					break;

				case nations::status::civilized:
					color = sys::pack_color(238, 252, 38);
					break;

					// primitive, uncivilized and westernized
				default:
					color = sys::pack_color(250, 5, 5);
					break;
			}
		else // If no owner use default color
			color = 255 << 16 | 255 << 8 | 255;
		auto i = province::to_map_id(prov_id);

		prov_color[i] = color;
		prov_color[i + texture_size] = color;
		
	});
	state.map_display.set_province_color(prov_color, mode::rank);
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
	case mode::diplomatic:
		set_diplomatic(state);
		break;
	case mode::rank:
		set_rank(state);
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
