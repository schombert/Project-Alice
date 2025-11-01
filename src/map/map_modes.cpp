#include "map_modes.hpp"

#include "color.hpp"
#include "demographics.hpp"
#include "system_state.hpp"
#include "dcon_generated.hpp"
#include "province.hpp"
#include "nations.hpp"
#include "economy_stats.hpp"
#include "economy_production.hpp"

#include <unordered_map>

#include "modes/political.hpp"
#include "modes/supply.hpp"
#include "modes/region.hpp"
#include "modes/nationality.hpp"
#include "modes/population.hpp"
#include "modes/sphere.hpp"
#include "modes/diplomatic.hpp"
#include "modes/rank.hpp"
#include "modes/relation.hpp"
#include "modes/recruitment.hpp"
#include "modes/civilization_level.hpp"
#include "modes/migration.hpp"
#include "modes/infrastructure.hpp"
#include "modes/revolt.hpp"
#include "modes/party_loyalty.hpp"
#include "modes/admin.hpp"
#include "modes/naval.hpp"
#include "modes/national_focus.hpp"
#include "modes/crisis.hpp"
#include "modes/colonial.hpp"
#include "modes/rgo_output.hpp"
#include "modes/religion.hpp"

//
// EXTRA MAP MODES
//
std::vector<uint32_t> ideology_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	if(state.map_state.get_selected_province()) {
		auto fat_id = state.world.province_get_dominant_ideology(state.map_state.get_selected_province());
		if(bool(fat_id)) {
			uint32_t full_color = fat_id.get_color();
			uint32_t empty_color = 0xDDDDDD;
			// Make the other end of the gradient dark if the color is bright and vice versa.
			if((full_color & 0xFF) + (full_color >> 8 & 0xFF) + (full_color >> 16 & 0xFF) > 140 * 3) {
				empty_color = 0x222222;
			}
			auto const pkey = pop_demographics::to_key(state, fat_id.id);
			state.world.for_each_province([&](dcon::province_id prov_id) {
				auto i = province::to_map_id(prov_id);
				float total = 0.f;
				float value = 0.f;
				for(const auto pl : state.world.province_get_pop_location_as_province(prov_id)) {
					value += pop_demographics::get_demo(state, pl.get_pop(), pkey) * pl.get_pop().get_size();
					total += pl.get_pop().get_size();
				}
				auto ratio = value / total;
				auto color = ogl::color_gradient(ratio, full_color, empty_color);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			});
		}
	} else {
		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto id = province::to_map_id(prov_id);
			float total_pops = state.world.province_get_demographics(prov_id, demographics::total);
			dcon::ideology_id primary_id;
			dcon::ideology_id secondary_id;
			float primary_percent = 0.f;
			float secondary_percent = 0.f;
			state.world.for_each_ideology([&](dcon::ideology_id id) {
				auto demo_key = demographics::to_key(state, id);
				auto volume = state.world.province_get_demographics(prov_id, demo_key);
				float percent = volume / total_pops;
				if(percent > primary_percent) {
					secondary_id = primary_id;
					secondary_percent = primary_percent;
					primary_id = id;
					primary_percent = percent;
				} else if(percent > secondary_percent) {
					secondary_id = id;
					secondary_percent = percent;
				}
			});
			uint32_t primary_color = dcon::fatten(state.world, primary_id).get_color();
			uint32_t secondary_color = 0xFFAAAAAA; // This color won't be reached
			if(bool(secondary_id)) {
				secondary_color = dcon::fatten(state.world, secondary_id).get_color();
			}
			if(secondary_percent >= primary_percent * 0.75f) {
				prov_color[id] = primary_color;
				prov_color[id + texture_size] = secondary_color;
			} else {
				prov_color[id] = primary_color;
				prov_color[id + texture_size] = primary_color;
			}
		});
	}
	return prov_color;
}

std::vector<uint32_t> issue_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	if(state.map_state.get_selected_province()) {
		auto fat_id = state.world.province_get_dominant_issue_option(state.map_state.get_selected_province());
		if(bool(fat_id)) {
			uint32_t full_color = ogl::get_ui_color(state, fat_id.id);
			uint32_t empty_color = 0xDDDDDD;
			// Make the other end of the gradient dark if the color is bright and vice versa.
			if((full_color & 0xFF) + (full_color >> 8 & 0xFF) + (full_color >> 16 & 0xFF) > 140 * 3) {
				empty_color = 0x222222;
			}
			auto const pkey = pop_demographics::to_key(state, fat_id.id);
			state.world.for_each_province([&](dcon::province_id prov_id) {
				auto i = province::to_map_id(prov_id);
				float total = 0.f;
				float value = 0.f;
				for(const auto pl : state.world.province_get_pop_location_as_province(prov_id)) {
					value += pop_demographics::get_demo(state, pl.get_pop(), pkey) * pl.get_pop().get_size();
					total += pl.get_pop().get_size();
				}
				auto ratio = value / total;
				auto color = ogl::color_gradient(ratio, full_color, empty_color);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			});
		}
	} else {
		state.world.for_each_province([&](dcon::province_id prov_id) {
			auto id = province::to_map_id(prov_id);
			float total_pops = state.world.province_get_demographics(prov_id, demographics::total);
			dcon::issue_option_id primary_id;
			dcon::issue_option_id secondary_id;
			float primary_percent = 0.f;
			float secondary_percent = 0.f;
			state.world.for_each_issue_option([&](dcon::issue_option_id id) {
				auto demo_key = demographics::to_key(state, id);
				auto volume = state.world.province_get_demographics(prov_id, demo_key);
				float percent = volume / total_pops;
				if(percent > primary_percent) {
					secondary_id = primary_id;
					secondary_percent = primary_percent;
					primary_id = id;
					primary_percent = percent;
				} else if(percent > secondary_percent) {
					secondary_id = id;
					secondary_percent = percent;
				}
			});
			uint32_t primary_color = ogl::get_ui_color(state, primary_id);
			uint32_t secondary_color = 0xFFAAAAAA; // This color won't be reached
			if(bool(secondary_id)) {
				secondary_color = ogl::get_ui_color(state, secondary_id);
			}
			if(secondary_percent >= primary_percent * 0.75f) {
				prov_color[id] = primary_color;
				prov_color[id + texture_size] = secondary_color;
			} else {
				prov_color[id] = primary_color;
				prov_color[id + texture_size] = primary_color;
			}
		});
	}
	return prov_color;
}

std::vector<uint32_t> fort_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	int32_t max_lvl = state.economy_definitions.building_definitions[int32_t(economy::province_building_type::fort)].max_level;
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		int32_t current_lvl = state.world.province_get_building_level(prov_id, uint8_t(economy::province_building_type::fort));
		int32_t max_local_lvl = state.world.nation_get_max_building_level(state.local_player_nation, uint8_t(economy::province_building_type::fort));
		uint32_t color = 0x222222;
		uint32_t stripe_color = 0x222222;

		if(current_lvl > 0) {
			color = ogl::color_gradient(
				float(current_lvl) / float(max_lvl),
				sys::pack_color(14, 240, 44), // green
				sys::pack_color(41, 5, 245) // blue
			);
		}
		if(province::can_build_fort(state, prov_id, state.local_player_nation)) {
			stripe_color = sys::pack_color(232, 228, 111); // yellow
		} else if(nation == state.local_player_nation && province::has_fort_being_built(state, prov_id)) {
			stripe_color = sys::pack_color(247, 15, 15); // yellow
		} else {
			stripe_color = color;
		}
		auto i = province::to_map_id(prov_id);
		prov_color[i] = color;
		prov_color[i + texture_size] = stripe_color;
	});
	return prov_color;
}

std::vector<uint32_t> factory_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);

	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	// get state with most factories
	float max_total = 0;
	state.world.for_each_province([&](dcon::province_id pid) {
		float total = 0.f;
		if(sel_nation) {
			if (state.world.province_get_nation_from_province_ownership(pid) == sel_nation)
				total = economy::province_factory_level(state, pid);
		} else {
			total = economy::province_factory_level(state, pid);
		}

		if(total > max_total)
			max_total = total;
		if(max_total == 0)
			max_total = 1;
	});
	state.world.for_each_province([&](dcon::province_id pid) {
		float total = 0;

		if(
			(sel_nation && state.world.province_get_nation_from_province_ownership(pid) != sel_nation)
			|| !(state.world.province_get_nation_from_province_ownership(pid))
		) {
			return;
		}

		total = economy::province_factory_level(state, pid);
		float value = std::log(float(total + 1)) / std::log(float(max_total + 1));
		uint32_t color = ogl::color_gradient_viridis(value);
		auto i = province::to_map_id(pid);
		prov_color[i] = color;
		prov_color[i + texture_size] = color;
	});
	return prov_color;
}

std::vector<uint32_t> con_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto scale = 1.f / 10.f;
			auto value = scale * (state.world.province_get_demographics(prov_id, demographics::consciousness) / state.world.province_get_demographics(prov_id, demographics::total));
			uint32_t color = ogl::color_gradient_magma(value);
			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}

std::vector<uint32_t> literacy_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto value = (state.world.province_get_demographics(prov_id, demographics::literacy) / state.world.province_get_demographics(prov_id, demographics::total));
			uint32_t color = ogl::color_gradient_viridis(value);
			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> growth_map_from(sys::state& state) {
	std::vector<float> prov_population_change(state.world.province_size() + 1);
	std::unordered_map<int32_t, float> continent_max_growth = {};
	std::unordered_map<int32_t, float> continent_min_growth = {};
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			float population_change = float(demographics::get_monthly_pop_increase(state, prov_id));
			auto cid = fat_id.get_continent().id.index();
			continent_max_growth[cid] = std::max(continent_max_growth[cid], population_change);
			continent_min_growth[cid] = std::min(continent_min_growth[cid], population_change);
			auto i = province::to_map_id(prov_id);
			prov_population_change[i] = population_change;
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			auto color = sys::pack_color(232, 228, 111); // yellow
			if(prov_population_change[i] > 0.f) {
				float gradient_index = (continent_max_growth[cid] == 0.f ? 0.f : (prov_population_change[i] / continent_max_growth[cid]));
				color = ogl::color_gradient(gradient_index,
					sys::pack_color(46, 247, 15), // green
					sys::pack_color(232, 228, 111) // yellow
				);
			} else if(prov_population_change[i] < 0.f) {
				float gradient_index = (continent_min_growth[cid] == 0.f ? 0.f : (prov_population_change[i] / continent_min_growth[cid]));
				color = ogl::color_gradient(gradient_index,
					sys::pack_color(247, 15, 15), // red
					sys::pack_color(232, 228, 111) // yellow
				);
			}
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> income_map_from(sys::state& state) {
	std::vector<float> prov_population(state.world.province_size() + 1);
	std::unordered_map<int32_t, float> continent_max_pop = {};
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			float population = 0.f;
			for(const auto pl : state.world.province_get_pop_location_as_province(prov_id))
				population += pl.get_pop().get_savings();
			auto cid = fat_id.get_continent().id.index();
			continent_max_pop[cid] = std::max(continent_max_pop[cid], population);
			auto i = province::to_map_id(prov_id);
			prov_population[i] = population;
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			float gradient_index = 1.f - (prov_population[i] / continent_max_pop[cid]);
			auto color = ogl::color_gradient(gradient_index, 210, 100 << 8);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> employment_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto value = state.world.province_get_demographics(prov_id, demographics::employed) / (1.f + state.world.province_get_demographics(prov_id, demographics::total));
			uint32_t color = ogl::color_gradient_viridis(value);
			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}

std::vector<uint32_t> militancy_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;

	std::vector<uint32_t> prov_color(texture_size * 2);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto fat_id = dcon::fatten(state.world, prov_id);
		auto nation = fat_id.get_nation_from_province_ownership();
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			float revolt_risk = province::revolt_risk(state, prov_id) / 10;
			uint32_t color = ogl::color_gradient_magma(revolt_risk);
			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}

//
// Even newer mapmodes!
//
std::vector<uint32_t> life_needs_map_from(sys::state& state) {
	std::vector<float> prov_satisfaction(state.world.province_size() + 1);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			float population = 0.f;
			for(const auto pl : state.world.province_get_pop_location_as_province(prov_id))
				population += pop_demographics::get_life_needs(state, pl.get_pop()) * pl.get_pop().get_size();
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			prov_satisfaction[i] = population / state.world.province_get_demographics(prov_id, demographics::total);
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			auto color = ogl::color_gradient_viridis(prov_satisfaction[i]);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> everyday_needs_map_from(sys::state& state) {
	std::vector<float> prov_satisfaction(state.world.province_size() + 1);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			float population = 0.f;
			for(const auto pl : state.world.province_get_pop_location_as_province(prov_id))
				population += pop_demographics::get_everyday_needs(state, pl.get_pop()) * pl.get_pop().get_size();
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			prov_satisfaction[i] = population / state.world.province_get_demographics(prov_id, demographics::total);
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			float gradient_index = prov_satisfaction[i];
			auto color = ogl::color_gradient_viridis(gradient_index);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> luxury_needs_map_from(sys::state& state) {
	std::vector<float> prov_satisfaction(state.world.province_size() + 1);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			float population = 0.f;
			for(const auto pl : state.world.province_get_pop_location_as_province(prov_id))
				population += pop_demographics::get_luxury_needs(state, pl.get_pop()) * pl.get_pop().get_size();
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			prov_satisfaction[i] = population / state.world.province_get_demographics(prov_id, demographics::total);
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			auto color = ogl::color_gradient_viridis(prov_satisfaction[i]);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> life_rating_map_from(sys::state& state) {
	std::vector<float> prov_population(state.world.province_size() + 1);
	std::unordered_map<int32_t, float> continent_max_pop = {};
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			continent_max_pop[cid] = std::max(continent_max_pop[cid], float(fat_id.get_life_rating()));
			auto i = province::to_map_id(prov_id);
			prov_population[i] = float(fat_id.get_life_rating());
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			float gradient_index = prov_population[i] / continent_max_pop[cid];
			auto color = ogl::color_gradient_viridis(gradient_index);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> officers_map_from(sys::state& state) {
	std::vector<float> prov_population(state.world.province_size() + 1);
	std::unordered_map<int32_t, float> continent_max_pop = {};
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			float total_officers = fat_id.get_demographics(demographics::to_key(state, state.culture_definitions.officers));
			continent_max_pop[cid] = std::max(continent_max_pop[cid], total_officers);
			auto i = province::to_map_id(prov_id);
			prov_population[i] = total_officers;
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			float gradient_index = 1.f - (prov_population[i] / continent_max_pop[cid]);
			auto color = ogl::color_gradient(gradient_index, 210, 100 << 8);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> ctc_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto total_pw = state.world.province_get_demographics(prov_id, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
			auto total_sw = state.world.province_get_demographics(prov_id, demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
			auto total = total_pw + total_sw;
			auto value = (total_pw == 0.f || total_sw == 0.f) ? 0.f : total_pw / (total_pw + total_sw);
			value = 1.f - (state.economy_definitions.craftsmen_fraction - value);
			uint32_t color = ogl::color_gradient(value,
				sys::pack_color(46, 247, 15), // green
				sys::pack_color(247, 15, 15) // red
			);
			auto i = province::to_map_id(prov_id);
			prov_color[i] = color;
			prov_color[i + texture_size] = color;
		}
	});
	return prov_color;
}
std::vector<uint32_t> crime_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		dcon::crime_id cmp_crime;
		if(state.map_state.get_selected_province()) {
			cmp_crime = state.world.province_get_crime(state.map_state.get_selected_province());
		}
		auto i = province::to_map_id(prov_id);
		if(auto crime = state.world.province_get_crime(prov_id); crime && (!cmp_crime || crime == cmp_crime)) {
			prov_color[i] = ogl::get_ui_color(state, crime);
			prov_color[i + texture_size] = ogl::get_ui_color(state, crime);
		} else {
			prov_color[i] = 0;
			prov_color[i + texture_size] = 0;
		}
	});
	return prov_color;
}
std::vector<uint32_t> rally_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		auto i = province::to_map_id(prov_id);
		prov_color[i] = state.world.province_get_land_rally_point(prov_id) ? sys::pack_color(46, 247, 15) : 0;
		prov_color[i + texture_size] = state.world.province_get_naval_rally_point(prov_id) ? sys::pack_color(46, 15, 247) : 0;
	});
	return prov_color;
}
std::vector<uint32_t> mobilization_map_from(sys::state& state) {
	std::vector<float> prov_population(state.world.province_size() + 1);
	std::unordered_map<int32_t, float> continent_max_pop = {};
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			float total_regs = float(military::regiments_max_possible_from_province(state, prov_id));
			continent_max_pop[cid] = std::max(continent_max_pop[cid], total_regs);
			auto i = province::to_map_id(prov_id);
			prov_population[i] = total_regs;
		}
	});
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((sel_nation && nation == sel_nation) || !sel_nation) {
			auto fat_id = dcon::fatten(state.world, prov_id);
			auto cid = fat_id.get_continent().id.index();
			auto i = province::to_map_id(prov_id);
			if(prov_population[i] == 0.f) {
				auto color = sys::pack_color(0, 0, 0);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			} else {
				float gradient_index = 1.f - (prov_population[i] / continent_max_pop[cid]);
				auto color = ogl::color_gradient(gradient_index, 210, 100 << 8);
				prov_color[i] = color;
				prov_color[i + texture_size] = color;
			}
		}
	});
	return prov_color;
}
std::vector<uint32_t> workforce_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	auto sel_nation = state.world.province_get_nation_from_province_ownership(state.map_state.get_selected_province());

	float max_gdp = 0.f;

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((nation == sel_nation) || !sel_nation) {
			auto gdp = economy::gdp::breakdown_province(state, prov_id);
			if(gdp.total > max_gdp) {
				max_gdp = gdp.total;
			}
		}
	});

	state.world.for_each_province([&](dcon::province_id prov_id) {
		auto id = province::to_map_id(prov_id);
		auto nation = state.world.province_get_nation_from_province_ownership(prov_id);
		if((nation == sel_nation) || !sel_nation) {
			auto gdp = economy::gdp::breakdown_province(state, prov_id);
			auto eps = 0.001f;
			auto scale = std::log(gdp.total + 1.f) / (std::log(max_gdp + 1.f) + eps);

			auto primary_color = sys::pack_color(
				std::max(0.f, gdp.primary / (gdp.total + eps) * scale),
				std::max(0.f, gdp.secondary_factory / (gdp.total + eps) * scale),
				std::max(0.f, gdp.secondary_artisan / (gdp.total + eps) * scale)
			);
			auto secondary_color = primary_color;

			prov_color[id] = primary_color;
			prov_color[id + texture_size] = primary_color;
		} else {
			prov_color[id] = 0x000000;
			prov_color[id + texture_size] = 0x000000;
		}
	});

	return prov_color;
}
std::vector<uint32_t> players_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size() + 1;
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2);
	for(const auto n : state.world.in_nation) {
		if(n.get_is_player_controlled()) {
			for(const auto po : state.world.nation_get_province_ownership_as_nation(n)) {
				auto id = province::to_map_id(po.get_province());
				prov_color[id] = n.get_color();
			}
			for(const auto po : state.world.nation_get_province_control_as_nation(n)) {
				auto id = province::to_map_id(po.get_province());
				prov_color[id + texture_size] = n.get_color();
			}
		}
	}
	return prov_color;
}

#include "gui_element_types.hpp"

std::vector<uint32_t> select_states_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2, 0);

	assert(state.state_selection.has_value());
	if(state.state_selection) {
		for(const auto s : state.state_selection->selectable_states) {
			uint32_t color = ogl::color_from_hash(s.index());

			for(const auto m : state.world.state_definition_get_abstract_state_membership_as_state(s)) {
				auto p = m.get_province();

				auto i = province::to_map_id(p.id);

				prov_color[i] = color;
				prov_color[i + texture_size] = ~color;
			}
		}
	}
	return prov_color;
}

std::vector<uint32_t> select_national_identity_map_from(sys::state& state) {
	uint32_t province_size = state.world.province_size();
	uint32_t texture_size = province_size + 256 - province_size % 256;
	std::vector<uint32_t> prov_color(texture_size * 2, 0);

	assert(state.national_identity_selection.has_value());
	if(state.national_identity_selection) {
		for(const auto ni : state.national_identity_selection->selectable_identities) {
			uint32_t color = ogl::color_from_hash(ni.index());

			for(const auto c : state.world.national_identity_get_core(ni)) {
				auto p = c.get_province();
				auto i = province::to_map_id(p.id);

				// Filtering outside cores
				if(state.national_identity_selection->province_owner_filter && p.get_nation_from_province_ownership() != state.national_identity_selection->province_owner_filter) {
					continue;
				}

				// Identity coming first in a stack is higher priority
				if(prov_color[i] == 0) {
					prov_color[i] = color;
					prov_color[i + texture_size] = ~color;
				}
			}
			
		}
	}
	return prov_color;
}

namespace map_mode {

void set_map_mode(sys::state& state, mode mode) {
	if(mode == map_mode::mode::handled_from_outside) {
		return;
	}

	std::vector<uint32_t> prov_color;
	switch(mode) {
		case map_mode::mode::migration:
		case map_mode::mode::population:
		case map_mode::mode::relation:
		case map_mode::mode::revolt:
		case map_mode::mode::supply:
		case map_mode::mode::admin:
		case map_mode::mode::crisis:
		//New mapmodes
		case map_mode::mode::literacy:
		case map_mode::mode::conciousness:
		case map_mode::mode::growth:
		case map_mode::mode::income:
		case map_mode::mode::employment:
		case map_mode::mode::militancy:
		case map_mode::mode::life_needs:
		case map_mode::mode::everyday_needs:
		case map_mode::mode::luxury_needs:
		case map_mode::mode::mobilization:
		case map_mode::mode::officers:
		case map_mode::mode::life_rating:
		case map_mode::mode::clerk_to_craftsmen_ratio:
			if(state.ui_state.map_gradient_legend)
				state.ui_state.map_gradient_legend->set_visible(state, true);
			break;
		default:
			if(state.ui_state.map_gradient_legend)
				state.ui_state.map_gradient_legend->set_visible(state, false);
			break;
	}
	if(mode == mode::civilization_level) {
		if(state.ui_state.map_civ_level_legend)
			state.ui_state.map_civ_level_legend->set_visible(state, true);
	} else {
		if(state.ui_state.map_civ_level_legend)
			state.ui_state.map_civ_level_legend->set_visible(state, false);
	}
	if(mode == mode::colonial) {
		if(state.ui_state.map_col_legend)
			state.ui_state.map_col_legend->set_visible(state, true);
	} else {
		if(state.ui_state.map_col_legend)
			state.ui_state.map_col_legend->set_visible(state, false);
	}
	if(mode == mode::diplomatic) {
		if(state.ui_state.map_dip_legend)
			state.ui_state.map_dip_legend->set_visible(state, true);
	} else {
		if(state.ui_state.map_dip_legend)
			state.ui_state.map_dip_legend->set_visible(state, false);
	}
	if(mode == mode::infrastructure || mode == mode::fort) {
		if(state.ui_state.map_rr_legend)
			state.ui_state.map_rr_legend->set_visible(state, true);
	} else {
		if(state.ui_state.map_rr_legend)
			state.ui_state.map_rr_legend->set_visible(state, false);
	}
	if(mode == mode::naval) {
		if(state.ui_state.map_nav_legend)
			state.ui_state.map_nav_legend->set_visible(state, true);
	} else {
		if(state.ui_state.map_nav_legend)
			state.ui_state.map_nav_legend->set_visible(state, false);
	}
	if(mode == mode::rank) {
		if(state.ui_state.map_rank_legend)
			state.ui_state.map_rank_legend->set_visible(state, true);
	} else {
		if(state.ui_state.map_rank_legend)
			state.ui_state.map_rank_legend->set_visible(state, false);
	}
	if(mode == mode::recruitment) {
		if(state.ui_state.map_rec_legend)
			state.ui_state.map_rec_legend->set_visible(state, true);
	} else {
		if(state.ui_state.map_rec_legend)
			state.ui_state.map_rec_legend->set_visible(state, false);
	}

	switch(mode) {
	case mode::state_select:
		prov_color = select_states_map_from(state);
		break;
	case mode::nation_identity_select:
		prov_color = select_national_identity_map_from(state);
		break;
	case mode::terrain:
		state.map_state.set_terrain_map_mode();
		return;
	case mode::political:
		prov_color = political_map_from(state);
		break;
	case mode::region:
		prov_color = region_map_from(state);
		break;
	case mode::population:
		prov_color = population_map_from(state);
		break;
	case mode::nationality:
		prov_color = nationality_map_from(state);
		break;
	case mode::sphere:
		prov_color = sphere_map_from(state);
		break;
	case mode::diplomatic:
		prov_color = diplomatic_map_from(state);
		break;
	case mode::rank:
		prov_color = rank_map_from(state);
		break;
	case mode::recruitment:
		prov_color = recruitment_map_from(state);
		break;
	case mode::supply:
		prov_color = supply_map_from(state);
		break;
	case mode::relation:
		prov_color = relation_map_from(state);
		break;
	case mode::civilization_level:
		prov_color = civilization_level_map_from(state);
		break;
	case mode::migration:
		prov_color = migration_map_from(state);
		break;
	case mode::infrastructure:
		prov_color = infrastructure_map_from(state);
		break;
	case mode::revolt:
		prov_color = revolt_map_from(state);
		break;
	case mode::party_loyalty:
		prov_color = party_loyalty_map_from(state);
		break;
	case mode::admin:
		prov_color = admin_map_from(state);
		break;
	case mode::naval:
		prov_color = naval_map_from(state);
		break;
	case mode::national_focus:
		prov_color = national_focus_map_from(state);
		break;
	case mode::crisis:
		prov_color = crisis_map_from(state);
		break;
	case mode::colonial:
		prov_color = colonial_map_from(state);
		break;
	case mode::rgo_output:
		prov_color = rgo_output_map_from(state);
		break;
	case mode::religion:
		prov_color = religion_map_from(state);
		break;
	case mode::issues:
		prov_color = issue_map_from(state);
		break;
	case mode::ideology:
		prov_color = ideology_map_from(state);
		break;
	case mode::fort:
		prov_color = fort_map_from(state);
		break;
	case mode::income:
		prov_color = income_map_from(state);
		break;
	case mode::conciousness:
		prov_color = con_map_from(state);
		break;
	case mode::militancy:
		prov_color = militancy_map_from(state);
		break;
	case mode::literacy:
		prov_color = literacy_map_from(state);
		break;
	case mode::employment:
		prov_color = employment_map_from(state);
		break;
	case mode::factories:
		prov_color = factory_map_from(state);
		break;
	case mode::growth:
		prov_color = growth_map_from(state);
		break;
	//even newer mapmodes
	case mode::players:
		prov_color = players_map_from(state);
		break;
	case mode::life_needs:
		prov_color = life_needs_map_from(state);
		break;
	case mode::everyday_needs:
		prov_color = everyday_needs_map_from(state);
		break;
	case mode::luxury_needs:
		prov_color = luxury_needs_map_from(state);
		break;
	case mode::life_rating:
		prov_color = life_rating_map_from(state);
		break;
	case mode::clerk_to_craftsmen_ratio:
		prov_color = ctc_map_from(state);
		break;
	case mode::crime:
		prov_color = crime_map_from(state);
		break;
	case mode::rally:
		prov_color = rally_map_from(state);
		break;
	case mode::officers:
		prov_color = officers_map_from(state);
		break;
	case mode::mobilization:
		prov_color = mobilization_map_from(state);
		break;
	case mode::workforce:
		prov_color = workforce_map_from(state);
		break;
	default:
		return;
	}

	state.map_state.set_province_color(prov_color, mode);
}

void update_map_mode(sys::state& state) {
	if(state.map_state.active_map_mode == mode::terrain || state.map_state.active_map_mode == mode::region) {
		return;
	}
	if(state.current_scene.id == game_scene::scene_id::in_game_economy_viewer) {
		economy_viewer::update(state);
		return;
	}
	set_map_mode(state, state.map_state.active_map_mode);
}
} // namespace map_mode
