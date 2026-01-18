#include "ai_campaign.hpp"
#include "ai_campaign_values.hpp"
#include "ai_types.hpp"
#include "prng.hpp"
#include "system_state.hpp"
#include "commands.hpp"

namespace ai {

void update_ai_campaign_strategy(sys::state& state) {
	auto v = state.current_date.value;
	auto d = v % 100;

	if(d == 0) {
		for(auto nation : state.world.in_nation) {
			if(state.world.nation_get_is_player_controlled(nation)) {
				continue;
			}

			// If AI has more than half provinces bordering sea and little neighbours - it follows naval strategy. In vanilla - see UK and Portugal.
			auto total_provinces_owned = 0.f;
			auto core_provinces_owned = 0.f;
			auto core_provinces_sea_access = 0.f;
			auto neighbours = 0.f;
			for(auto p : nation.get_province_ownership()) {
				total_provinces_owned++;
				if(p.get_province().get_is_colonial())
					continue;
				core_provinces_owned++;
				if(p.get_province().get_is_coast())
					core_provinces_sea_access++;
			}
			for(auto g : state.world.nation_get_nation_adjacency(nation)) {
				neighbours++;
			}

			if(core_provinces_sea_access / core_provinces_owned > 0.5f && total_provinces_owned / neighbours >= 3.f) {
				auto tagname = text::produce_simple_string(state, nation.get_identity_from_identity_holder().get_name());
				nation.set_ai_strategy(ai_strategies::naval);
			}
			// If AI is a GP or has a lot of neighbours - it follows military strategy
			else if(nation.get_is_great_power() || neighbours >= 4.f) {
				nation.set_ai_strategy(ai_strategies::militant);
			} else {
				nation.set_ai_strategy(ai_strategies::industrious);
			}
		}
	}
}

/* Update AI threats and rivals */
void update_ai_general_status(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(state.world.nation_get_owned_province_count(n) == 0) {
			state.world.nation_set_ai_is_threatened(n, false);
			state.world.nation_set_ai_rival(n, dcon::nation_id{});
			continue;
		}

		auto ll = state.world.nation_get_last_war_loss(n);
		float safety_factor = 1.2f;
		if(ll && state.current_date < ll + 365 * 4) {
			safety_factor = 1.8f;
		}
		auto in_sphere_of = state.world.nation_get_in_sphere_of(n);

		float greatest_neighbor = 0.0f;
		for(auto b : state.world.nation_get_nation_adjacency_as_connected_nations(n)) {
			auto other = b.get_connected_nations(0) != n ? b.get_connected_nations(0) : b.get_connected_nations(1);
			if(!nations::are_allied(state, n, other) && (!in_sphere_of || in_sphere_of != other.get_in_sphere_of())) {
				greatest_neighbor = std::max(greatest_neighbor, estimate_strength(state, other));
			}
		}

		float self_str = float(state.world.nation_get_military_score(n));
		for(auto subj : n.get_overlord_as_ruler())
			self_str += 0.75f * float(subj.get_subject().get_military_score());
		float defensive_str = estimate_defensive_strength(state, n);

		bool threatened = defensive_str < safety_factor * greatest_neighbor;
		state.world.nation_set_ai_is_threatened(n, threatened);

		if(!n.get_ai_rival()) {
			float min_str = 0.0f;
			dcon::nation_id potential;
			for(auto adj : n.get_nation_adjacency()) {
				auto other = adj.get_connected_nations(0) != n ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
				auto ol = other.get_overlord_as_subject().get_ruler();
				if(!ol && other.get_in_sphere_of() != n && (!threatened || !nations::are_allied(state, n, other))) {
					auto other_str = estimate_strength(state, other);
					if(self_str * 0.5f < other_str && other_str <= self_str * 1.5f && min_str > self_str) {
						min_str = other_str;
						potential = other;
					}
				}
			}

			if(potential) {
				if(!n.get_is_player_controlled() && nations::are_allied(state, n, potential)) {
					assert(command::can_cancel_alliance(state, n, potential));
					command::execute_cancel_alliance(state, n, potential);
				}
				n.set_ai_rival(potential);
			}
		} else {
			auto rival_str = estimate_strength(state, n.get_ai_rival());
			auto ol = n.get_ai_rival().get_overlord_as_subject().get_ruler();
			if(ol || n.get_ai_rival().get_in_sphere_of() == n || rival_str * 2 < self_str || self_str * 2 < rival_str) {
				n.set_ai_rival(dcon::nation_id{});
			}
		}
	}
}


// MP compliant
void update_ai_research(sys::state& state) {
	auto ymd_date = state.current_date.to_ymd(state.start_date);
	auto year = uint32_t(ymd_date.year);
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t id) {
		dcon::nation_id n{ dcon::nation_id::value_base_t(id) };

		if(state.world.nation_get_is_player_controlled(n)
			|| state.world.nation_get_current_research(n)
			|| !state.world.nation_get_is_civilized(n)
			|| state.world.nation_get_owned_province_count(n) == 0) {

			//skip -- does not need new research
			return;
		}

		struct potential_techs {
			dcon::technology_id id;
			float weight = 0.0f;
		};

		std::vector<potential_techs> potential;

		for(auto tid : state.world.in_technology) {
			if(state.world.nation_get_active_technologies(n, tid))
				continue; // Already researched

			if(state.current_date.to_ymd(state.start_date).year >= state.world.technology_get_year(tid)) {
				// Research technologies costing LP (military doctrines) only if can research it immediately
				if(culture::effective_technology_lp_cost(state, year, n, tid) > state.world.nation_get_leadership_points(n)) {
					continue;
				}
				// Technologies costing RP:
				// Find previous technology before this one
				dcon::technology_id prev_tech = dcon::technology_id(dcon::technology_id::value_base_t(tid.id.index() - 1));
				// Previous technology is from the same folder so we have to check that we have researched it beforehand
				if(tid.id.index() != 0 && state.world.technology_get_folder_index(prev_tech) == state.world.technology_get_folder_index(tid)) {
					// Only allow if all previously researched techs are researched
					if(state.world.nation_get_active_technologies(n, prev_tech))
						potential.push_back(potential_techs{ tid, 0.0f });
				} else { // first tech in folder
					potential.push_back(potential_techs{ tid, 0.0f });
				}
			}
		}

		for(auto& pt : potential) { // weight techs
			auto base = state.world.technology_get_ai_weight(pt.id);
			if(state.world.nation_get_ai_is_threatened(n) && state.culture_definitions.tech_folders[state.world.technology_get_folder_index(pt.id)].category == culture::tech_category::army) {
				base *= 2.0f;
			}

			if(state.world.nation_get_ai_strategy(n) == ai_strategies::militant && state.culture_definitions.tech_folders[state.world.technology_get_folder_index(pt.id)].category == culture::tech_category::army) {
				base *= 2.0f;
			}
			// AI countries with naval strategy pay higher attention to naval tech
			if(state.world.nation_get_ai_strategy(n) == ai_strategies::naval && state.culture_definitions.tech_folders[state.world.technology_get_folder_index(pt.id)].category == culture::tech_category::navy) {
				base *= 2.0f;
			}
			if(state.world.nation_get_ai_strategy(n) == ai_strategies::industrious && state.culture_definitions.tech_folders[state.world.technology_get_folder_index(pt.id)].category == culture::tech_category::industry) {
				base *= 2.0f;
			}
			if(state.world.nation_get_ai_strategy(n) == ai_strategies::industrious && state.culture_definitions.tech_folders[state.world.technology_get_folder_index(pt.id)].category == culture::tech_category::commerce) {
				base *= 2.0f;
			}
			if(state.world.nation_get_ai_strategy(n) == ai_strategies::technological && state.culture_definitions.tech_folders[state.world.technology_get_folder_index(pt.id)].category == culture::tech_category::culture) {
				base *= 2.0f;
			}

			auto cost = (float) std::pow(std::max(1.0f, culture::effective_technology_rp_cost(state, year, n, pt.id)), 2);
			pt.weight = (rng::get_random(state, id * pt.id.value) % 100) * base / cost;
		}
		auto rval = rng::get_random(state, id);
		std::sort(potential.begin(), potential.end(), [&](potential_techs& a, potential_techs& b) {
			if(a.weight != b.weight)
				return a.weight > b.weight;
			else // sort semi randomly
				return (a.id.index() ^ rval) > (b.id.index() ^ rval);
		});

		if(!potential.empty()) {
			state.world.nation_set_current_research(n, potential[0].id);
		}
	});
}

void initialize_ai_tech_weights(sys::state& state) {
	for(auto t : state.world.in_technology) {
		float base = 1000.0f;
		if(state.culture_definitions.tech_folders[t.get_folder_index()].category == culture::tech_category::army)
			base *= 3.0f;
		if(t.get_increase_building(economy::province_building_type::fort))
			base *= 0.5f;

		if(t.get_increase_building(economy::province_building_type::naval_base))
			base *= 1.5f;
		else if(state.culture_definitions.tech_folders[t.get_folder_index()].category == culture::tech_category::navy)
			base *= 0.9f;

		for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
			dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
			if(t.get_activate_unit(uid))
				base *= 3.0f;
		}

		for(auto ft : state.world.in_factory_type) {
			if(t.get_activate_building(ft))
				base *= 2.5f;
		}

		auto mod = t.get_modifier();
		auto& vals = mod.get_national_values();
		for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
			if(vals.offsets[i] == sys::national_mod_offsets::research_points) {
				base *= 6.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::research_points_modifier) {
				base *= 6.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::education_efficiency) {
				base *= 3.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::education_efficiency_modifier) {
				base *= 3.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::pop_growth) {
				base *= 5.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::max_national_focus) {
				base *= 4.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::colonial_life_rating) {
				base *= 4.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::rgo_output) {
				base *= 1.2f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::factory_output) {
				base *= 1.5f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::factory_throughput) {
				base *= 3.0f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::factory_input) {
				base *= 1.5f;
			} else if(vals.offsets[i] == sys::national_mod_offsets::tax_efficiency) {
				base *= 2.5f;
			}
		}

		t.set_ai_weight(base);
	}
}


}
