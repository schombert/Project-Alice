#include "military.hpp"
#include "dcon_generated.hpp"

namespace military {

void reset_unit_stats(sys::state& state) {
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		state.world.for_each_nation([&](dcon::nation_id nid) {
			state.world.nation_get_unit_stats(nid, uid) = state.military_definitions.unit_base_definitions[uid];
		});
	}
}

void apply_base_unit_stat_modifiers(sys::state& state) {
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto base_id = state.military_definitions.unit_base_definitions[uid].is_land ? state.military_definitions.base_army_unit : state.military_definitions.base_naval_unit;
		state.world.for_each_nation([&](dcon::nation_id nid) {
			state.world.nation_get_unit_stats(nid, uid) += state.world.nation_get_unit_stats(nid, base_id);
		});
	}
}

void restore_unsaved_values(sys::state& state) {
	state.world.for_each_nation([&](dcon::nation_id n) {
		auto w = state.world.nation_get_war_participant(n);
		if(w.begin() != w.end()) {
			state.world.nation_set_is_at_war(n, true);
		}
	});
	update_all_recruitable_regiments(state);
	regenerate_total_regiment_counts(state);
	update_naval_supply_points(state);
}

bool can_use_cb_against(sys::state const& state, dcon::nation_id from, dcon::nation_id target) {
	// TODO: implement function
	// return true if nation from has any cb to use against target
	return false;
}

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids) {
	// TODO: implement function
	return false;
}

template<typename T>
auto battle_is_ongoing_in_province(sys::state const& state, T ids) {
	// TODO: implement function
	return false;
}

float recruited_pop_fraction(sys::state const& state, dcon::nation_id n) {
	// TODO: implement function
	return 0.0f;
}

bool state_has_naval_base(sys::state const& state, dcon::state_instance_id si) {
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	auto def = state.world.state_instance_get_definition(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(def)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			if(p.get_province().get_naval_base_level() > 0)
				return true;
		}
	}
	return false;
}

bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	for(auto wa : state.world.nation_get_war_participant(a)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() == b && o.get_is_attacker() != is_attacker)
				return true;
		}
	}
	return false;
}

int32_t supply_limit_in_province(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	/*
	(province-supply-limit-modifier + 1) x (2.5 if it is owned an controlled or 2 if it is just controlled, you are allied to the controller, have military access with the controller, a rebel controls it, it is one of your core provinces, or you are sieging it) x (technology-supply-limit-modifier + 1)
	*/
	float modifier = 2.0f;
	auto prov_controller = state.world.province_get_nation_from_province_control(p);
	auto self_controlled = prov_controller == n;
	if(state.world.province_get_nation_from_province_ownership(p) == n && self_controlled) {
		modifier = 2.5f;
	} else if(self_controlled || bool(state.world.province_get_rebel_faction_from_province_rebel_control(p))) { // TODO: check for sieging
		modifier = 2.0f;
	} /*else if(auto dip_rel = state.world.get_diplomatic_relation_by_diplomatic_pair(prov_controller, n);
		state.world.diplomatic_relation_get_are_allied(dip_rel)
		) {
		modifier = 2.0f;
	} else if(auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(prov_controller, n);
		state.world.unilateral_relationship_get_military_access(uni_rel)
		) {
		modifier = 2.0f;
	} else if(bool(state.world.get_core_by_prov_tag_key(p, state.world.nation_get_identity_from_identity_holder(n)))) {
		modifier = 2.0f;
	}*/
	auto base_supply_lim = (state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::supply_limit) + 1.0f);
	auto national_supply_lim = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::supply_limit) + 1.0f);
	return int32_t( base_supply_lim * modifier * national_supply_lim );
}
int32_t regiments_created_from_province(sys::state& state, dcon::province_id p) {
	int32_t total = 0;
	for(auto pop : state.world.province_get_pop_location(p)) {
		if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
			auto regs = pop.get_pop().get_regiment_source();
			total += int32_t(regs.end() - regs.begin());
		}
	}
	return total;
}
int32_t mobilized_regiments_created_from_province(sys::state& state, dcon::province_id p) {
	/*
	Mobilized regiments come only from non-colonial provinces.
	*/
	if(fatten(state.world, p).get_is_colonial())
		return 0;

	int32_t total = 0;
	for(auto pop : state.world.province_get_pop_location(p)) {
		if(pop.get_pop().get_poptype() != state.culture_definitions.soldiers && pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
			auto regs = pop.get_pop().get_regiment_source();
			total += int32_t(regs.end() - regs.begin());
		}
	}
	return total;
}
int32_t regiments_max_possible_from_province(sys::state& state, dcon::province_id p) {
	/*
	- A soldier pop must be at least define:POP_MIN_SIZE_FOR_REGIMENT to support any regiments
	- If it is at least that large, then it can support one regiment per define:POP_SIZE_PER_REGIMENT x define:POP_MIN_SIZE_FOR_REGIMENT_COLONY_MULTIPLIER (if it is located in a colonial province) x define:POP_MIN_SIZE_FOR_REGIMENT_NONCORE_MULTIPLIER (if it is non-colonial but uncored)
	*/
	int32_t total = 0;
	if(state.world.province_get_is_colonial(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= state.defines.pop_min_size_for_regiment) {
					total += int32_t(pop.get_pop().get_size() / divisor);
				}
			}
		}
	} else if(!state.world.province_get_is_owner_core(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= state.defines.pop_min_size_for_regiment) {
					total += int32_t(pop.get_pop().get_size() / divisor);
				}
			}
		}
	} else {
		float divisor = state.defines.pop_size_per_regiment;
		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= state.defines.pop_min_size_for_regiment) {
					total += int32_t(pop.get_pop().get_size() / divisor);
				}
			}
		}
	}
	return total;
}

int32_t mobilized_regiments_possible_from_province(sys::state& state, dcon::province_id p) {
	/*
	Mobilized regiments come only from unoccupied, non-colonial provinces. 
	*/
	auto fp = fatten(state.world, p);
	if(fp.get_is_colonial() || fp.get_nation_from_province_control() != fp.get_nation_from_province_ownership())
		return 0;

	int32_t total = 0;
	// Mobilization size = national-modifier-to-mobilization-size + technology-modifier-to-mobilization-size
	auto mobilization_size = fp.get_nation_from_province_ownership().get_modifier_values(sys::national_mod_offsets::mobilization_size);

	for(auto pop : state.world.province_get_pop_location(p)) {
		/*
		In those provinces, mobilized regiments come from non-soldier, non-slave, poor-strata pops with a culture that is either the primary culture of the nation or an accepted culture.
		*/

		if(pop.get_pop().get_poptype() != state.culture_definitions.soldiers && pop.get_pop().get_poptype() != state.culture_definitions.slaves && pop.get_pop().get_is_primary_or_accepted_culture() && pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {

			/*
			The number of regiments these pops can provide is determined by pop-size x mobilization-size / define:POP_SIZE_PER_REGIMENT.
			*/
			total += int32_t(pop.get_pop().get_size() * mobilization_size / state.defines.pop_size_per_regiment);
		}
	}
	return total;
}

void update_recruitable_regiments(sys::state& state, dcon::nation_id n) {
	state.world.nation_set_recruitable_regiments(n, uint16_t(0));
	for(auto p : state.world.nation_get_province_ownership(n)) {
		state.world.nation_get_recruitable_regiments(n) += uint16_t(regiments_max_possible_from_province(state, p.get_province()));
	}
}
void update_all_recruitable_regiments(sys::state& state) {
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_recruitable_regiments(ids, ve::int_vector(0));
	});
	state.world.for_each_province([&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(owner) {
			state.world.nation_get_recruitable_regiments(owner) += uint16_t(regiments_max_possible_from_province(state, p));
		}
	});
}
void regenerate_total_regiment_counts(sys::state& state) {
	state.world.execute_serial_over_nation([&](auto ids) {
		state.world.nation_set_active_regiments(ids, ve::int_vector(0));
	});
	state.world.for_each_army([&](dcon::army_id a) {
		auto owner = state.world.army_get_controller_from_army_control(a);
		if(owner) {
			auto regs_range = state.world.army_get_army_membership(a);
			auto num_regs = regs_range.end() -regs_range.begin();
			state.world.nation_get_active_regiments(owner) += uint16_t(num_regs);
		}
	});
}

void regenerate_land_unit_average(sys::state& state) {
	/*
	We also need to know the average land unit score, which we define here as (attack + defense + national land attack modifier + national land defense modifier) x discipline
	*/
	auto const max = state.military_definitions.unit_base_definitions.size();
	state.world.for_each_nation([&](dcon::nation_id n) {
		float total = 0;
		float count = 0;

		auto lo_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_attack_modifier);
		auto ld_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_defense_modifier);

		for(uint32_t i = 2; i < max; ++i) {
			dcon::unit_type_id u{ dcon::unit_type_id::value_base_t(i) };
			if(state.world.nation_get_active_unit(n, u) && state.military_definitions.unit_base_definitions[u].is_land) {
				auto& reg_stats = state.world.nation_get_unit_stats(n, u);
				total += ((reg_stats.defence_or_hull + ld_mod) + (reg_stats.attack_or_gun_power + lo_mod)) * state.military_definitions.unit_base_definitions[u].discipline;
				++count;
			}
		}
		state.world.nation_set_averge_land_unit_score(n, total / count);
	});

}
void regenerate_ship_scores(sys::state& state) {
	/*
	To that we add for each capital ship: (hull points + national-naval-defense-modifier) x (gun power + national-naval-attack-modifier) / 250
	*/
	state.world.for_each_nation([&](dcon::nation_id n) {
		float total = 0;
		auto no_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::naval_attack_modifier);
		auto nd_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::naval_defense_modifier);

		for(auto nv : state.world.nation_get_navy_control(n)) {
			for(auto shp : nv.get_navy().get_navy_membership()) {
				if(state.military_definitions.unit_base_definitions[shp.get_ship().get_type()].capital) {
					auto& ship_stats = state.world.nation_get_unit_stats(n, shp.get_ship().get_type());
					total += (ship_stats.defence_or_hull + nd_mod) * (ship_stats.attack_or_gun_power + no_mod);
				}
			}
		}
		state.world.nation_set_capital_ship_score(n, total / 250.0f);
	});
}

int32_t naval_supply_points(sys::state& state, dcon::nation_id n) {
	return int32_t(state.world.nation_get_naval_supply_points(n));
}
int32_t naval_supply_points_used(sys::state& state, dcon::nation_id n) {
	return int32_t(state.world.nation_get_used_naval_supply_points(n));
}

void update_naval_supply_points(sys::state& state) {
	/*
	- naval supply score: you get define:NAVAL_BASE_SUPPLY_SCORE_BASE x (2 to the power of (its-level - 1)) for each naval base or define:NAVAL_BASE_SUPPLY_SCORE_EMPTY for each state without one, multiplied by define:NAVAL_BASE_NON_CORE_SUPPLY_SCORE if it is neither a core nor connected to the capital.
	*/
	state.world.for_each_nation([&](dcon::nation_id n) {
		auto cap_region = state.world.province_get_connected_region_id(state.world.nation_get_capital(n));
		float total = 0;
		for(auto si : state.world.nation_get_state_ownership(n)) {
			auto d = state.world.state_instance_get_definition(si.get_state());
			bool saw_coastal = false;
			int32_t nb_level = 0;
			for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == n) {
					if(p.get_province().get_is_coast()) {
						saw_coastal = true;
					}
					nb_level = std::max(nb_level, int32_t(p.get_province().get_naval_base_level()));
				}
			}
			bool is_core_or_connected = si.get_state().get_capital().get_is_owner_core() || si.get_state().get_capital().get_connected_region_id() == cap_region;
			float core_factor = is_core_or_connected ? 1.0f : state.defines.naval_base_non_core_supply_score;
			if(nb_level > 0) {
				total += state.defines.naval_base_supply_score_base * float(1 << (nb_level - 1)) * core_factor;
			} else if(saw_coastal) {
				total += state.defines.naval_base_supply_score_empty * core_factor;
			}
		}
		state.world.nation_set_naval_supply_points(n, uint16_t(total));
	});

	/*
	- ships consume naval base supply at their supply_consumption_score. Going over the naval supply score comes with various penalties (described elsewhere).
	*/
	state.world.for_each_nation([&](dcon::nation_id n) {
		float total = 0;
		for(auto nv : state.world.nation_get_navy_control(n)) {
			for(auto shp : nv.get_navy().get_navy_membership()) {
				total += state.world.nation_get_unit_stats(n, shp.get_ship().get_type()).supply_consumption;
			}
		}
		state.world.nation_set_used_naval_supply_points(n, uint16_t(total));
	});
}

float mobilization_size(sys::state const& state, dcon::nation_id n) {
	// Mobilization size = national-modifier-to-mobilization-size + technology-modifier-to-mobilization-size
	return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_size);
}
float mobilization_impact(sys::state const& state, dcon::nation_id n) {
	// Mobilization impact = 1 - mobilization-size x (national-mobilization-economy-impact-modifier + technology-mobilization-impact-modifier), to a minimum of zero.
	return std::clamp(
		1.0f - mobilization_size(state, n) * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_impact),
		0.0f, 1.0f);
}

}
