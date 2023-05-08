#include "military.hpp"
#include "dcon_generated.hpp"
#include "prng.hpp"

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

bool can_use_cb_against(sys::state& state, dcon::nation_id from, dcon::nation_id target) {
	auto other_cbs = state.world.nation_get_available_cbs(from);
	for(auto& cb : other_cbs) {
		if(cb.target == target && cb_conditions_satisfied(state, from, target, cb.cb_type))
			return true;
	}
	for(auto cb : state.world.in_cb_type) {
		if((cb.get_type_bits() & military::cb_flag::always) != 0) {
			if(cb_conditions_satisfied(state, from, target, cb))
				return true;
		}
	}
	return false;
}

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids) {
	// TODO: implement function
	return false;
}

template<typename T>
auto province_is_under_siege(sys::state const& state, T ids) {
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
	auto mobilization_size = std::max(0.0f, fp.get_nation_from_province_ownership().get_modifier_values(sys::national_mod_offsets::mobilization_size));

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

bool cb_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb) {
	auto can_use = state.world.cb_type_get_can_use(cb);
	if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(actor))) {
		return false;
	}

	auto allowed_countries = state.world.cb_type_get_allowed_countries(cb);
	auto allowed_states = state.world.cb_type_get_allowed_states(cb);

	if(!allowed_countries && allowed_states) {
		bool any_allowed = false;
		for(auto si : state.world.nation_get_state_ownership(target)) {
			if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {
				any_allowed = true;
				break;
			}
		}
		if(!any_allowed)
			return false;
	}

	auto allowed_substates = state.world.cb_type_get_allowed_substate_regions(cb);
	if(allowed_substates) {
		bool any_allowed = [&]() {
			for(auto v : state.world.nation_get_overlord_as_ruler(target)) {
				if(v.get_subject().get_is_substate()) {
					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_substates, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {
							return true;
						}
					}
				}
			}
			return false;
		}();
		if(!any_allowed)
			return false;
	}

	if(allowed_countries) {
		bool any_allowed = [&]() {
			for(auto n : state.world.in_nation) {
				if(n != target && n != actor) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(n.id))) {
						if(allowed_states) { // check whether any state within the target is valid for free / liberate
							for(auto si : state.world.nation_get_state_ownership(target)) {
								if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(n.id))) {
									return true;
								}
							}
						} else { // no allowed states trigger -> nation is automatically a valid target
							return true;
						}
					}
				}
			}
			return false;
		}();
		if(!any_allowed)
			return false;
	}

	return true;
}

void update_cbs(sys::state& state) {
	for(auto n : state.world.in_nation) {
		auto current_cbs = n.get_available_cbs();
		for(uint32_t i = current_cbs.size(); i-- > 0;) {
			if(current_cbs[i].expiration && current_cbs[i].expiration <= state.current_date) {
				current_cbs.remove_at(i);
			}
		}

		// check for cancellation
		if(n.get_constructing_cb_type()) {
			/*
			CBs that become invalid (the nations involved no longer satisfy the conditions or enter into a war with each other) are canceled (and the player should be notified in this event).
			*/
			auto target = n.get_constructing_cb_target();
			if(military::are_at_war(state, n, target)
				|| state.world.nation_get_owned_province_count(target) == 0
				|| !cb_conditions_satisfied(state, n, target, n.get_constructing_cb_type())) {

				// TODO: notify player

				n.set_constructing_cb_is_discovered(false);
				n.set_constructing_cb_progress(0.0f);
				n.set_constructing_cb_target(dcon::nation_id{});
				n.set_constructing_cb_type(dcon::cb_type_id{});
			}
		}

		if(n.get_constructing_cb_type() && !nations::is_involved_in_crisis(state, n)) {
			/*
			CB fabrication by a nation is paused while that nation is in a crisis (nor do events related to CB fabrication happen). CB fabrication is advanced by points equal to:
			define:CB_GENERATION_BASE_SPEED x cb-type-construction-speed x (national-cb-construction-speed-modifiers + technology-cb-construction-speed-modifier + 1).
			*/

			n.get_constructing_cb_progress() += state.defines.cb_generation_base_speed * n.get_constructing_cb_type().get_construction_speed() * (n.get_modifier_values(sys::national_mod_offsets::cb_generation_speed_modifier) + 1.0f);

			/*
			Each day, a fabricating CB has a define:CB_DETECTION_CHANCE_BASE out of 1000 chance to be detected. If discovered, the fabricating country gains the infamy for that war goal x the fraction of fabrication remaining. If discovered relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states with a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED
			*/
			if(!n.get_constructing_cb_is_discovered()) {
				auto val = rng::get_random(state, uint32_t((n.id.index() << 3) + 5)) % 1000;
				if(val <= uint32_t(state.defines.cb_detection_chance_base)) {
					execute_cb_discovery(state, n);
					n.set_constructing_cb_is_discovered(true);
				}
			}


			// TODO: cb fabrication events

			/*
			When fabrication progress reaches 100, the CB will remain valid for define:CREATED_CB_VALID_TIME months (so x30 days for us). Note that pending CBs have their target nation fixed, but all other parameters are flexible.
			*/
			if(n.get_constructing_cb_progress() >= 100.0f) {
				add_cb(state, n, n.get_constructing_cb_type(), n.get_constructing_cb_target());
				n.set_constructing_cb_is_discovered(false);
				n.set_constructing_cb_progress(0.0f);
				n.set_constructing_cb_target(dcon::nation_id{});
				n.set_constructing_cb_type(dcon::cb_type_id{});
			}
		}
	}
}

void add_cb(sys::state& state, dcon::nation_id n, dcon::cb_type_id cb, dcon::nation_id target) {
	auto current_cbs = state.world.nation_get_available_cbs(n);
	current_cbs.push_back(military::available_cb{ target, state.current_date + int32_t(state.defines.created_cb_valid_time) * 30 , cb});
	// TODO: notify
}

float cb_infamy(sys::state const& state, dcon::cb_type_id t) {
	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(t);

	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		total += state.defines.infamy_clear_union_sphere;
	}
	if((bits & cb_flag::po_gunboat) != 0) {
		total += state.defines.infamy_gunboat;
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += state.defines.infamy_annex;
	}
	if((bits & cb_flag::po_demand_state) != 0) {
		total += state.defines.infamy_demand_state;
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += state.defines.infamy_add_to_sphere;
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += state.defines.infamy_disarmament;
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += state.defines.infamy_reparations;
	}
	if((bits & cb_flag::po_transfer_provinces) != 0) {
		total += state.defines.infamy_transfer_provinces;
	}
	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += state.defines.infamy_prestige;
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += state.defines.infamy_make_puppet;
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += state.defines.infamy_release_puppet;
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += state.defines.infamy_status_quo;
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += state.defines.infamy_install_communist_gov_type;
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += state.defines.infamy_uninstall_communist_gov_type;
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += state.defines.infamy_remove_cores;
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += state.defines.infamy_colony;
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += state.defines.infamy_destroy_forts;
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total += state.defines.infamy_destroy_naval_bases;
	}

	return total * state.world.cb_type_get_badboy_factor(t);
}

void execute_cb_discovery(sys::state& state, dcon::nation_id n) {
	/*
	 If discovered, the fabricating country gains the infamy for that war goal x the fraction of fabrication remaining. If discovered relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states with a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED
	*/
	auto infamy = cb_infamy(state, state.world.nation_get_constructing_cb_type(n));
	state.world.nation_get_infamy(n) += ((100.0f - state.world.nation_get_constructing_cb_progress(n)) / 100.0f) * infamy;

	auto target = state.world.nation_get_constructing_cb_target(n);

	nations::adjust_relationship(state, n, target, state.defines.on_cb_detected_relation_change);

	for(auto si : state.world.nation_get_state_ownership(target)) {
		if(si.get_state().get_flashpoint_tag()) {
			si.get_state().get_flashpoint_tension() += state.defines.tension_on_cb_discovered;
		}
	}

	// TODO: notify
}

bool leader_is_in_combat(sys::state& state, dcon::leader_id l) {
	// TODO: implement
	return false;
}

dcon::leader_id make_new_leader(sys::state& state, dcon::nation_id n, bool is_general) {
	auto l = fatten(state.world, state.world.create_leader());
	l.set_is_admiral(!is_general);

	uint32_t seed_base = (uint32_t(n.index()) << 6) ^ uint32_t(l.id.index());

	auto num_personalities = state.military_definitions.first_background_trait.index() - 1;
	auto num_backgrounds = (state.world.leader_trait_size() - num_personalities) - 2;

	auto trait_pair = rng::get_random_pair(state, seed_base);

	l.set_personality(dcon::leader_trait_id{dcon::leader_trait_id::value_base_t( 1 + trait_pair.high % num_personalities ) });
	l.set_background(dcon::leader_trait_id{ dcon::leader_trait_id::value_base_t(state.military_definitions.first_background_trait.index() + 1 + trait_pair.low % num_backgrounds) });

	auto names_pair = rng::get_random_pair(state, seed_base + 1);

	auto names = state.world.culture_get_last_names(state.world.nation_get_primary_culture(n));
	if(names.size() > 0)
		l.set_name(names.at(names_pair.high % names.size()));

	l.set_since(state.current_date);

	state.world.try_create_leader_loyalty(n, l);

	state.world.nation_get_leadership_points(n) -= state.defines.leader_recruit_cost;

	return l;
}

void kill_leader(sys::state& state, dcon::leader_id l) {
	// TODO: notify?
	/*
	the player only gets leader death messages if the leader is currently assigned to an army or navy (assuming the message setting for it is turned on).
	*/

	state.world.delete_leader(l);
}

struct leader_counts {
	int32_t admirals = 0;
	int32_t generals = 0;
};

leader_counts count_leaders(sys::state& state, dcon::nation_id n) {
	leader_counts result{};
	for(auto l : state.world.nation_get_leader_loyalty(n)) {
		if(l.get_leader().get_is_admiral())
			++result.admirals;
		else
			++result.generals;
	}
	return result;
}
int32_t count_armies(sys::state& state, dcon::nation_id n) {
	auto x = state.world.nation_get_army_control(n);
	return int32_t(x.end() - x.begin());
}
int32_t count_navies(sys::state& state, dcon::nation_id n) {
	auto x = state.world.nation_get_navy_control(n);
	return int32_t(x.end() - x.begin());
}

void monthly_leaders_update(sys::state& state) {
	/*
	- A nation gets ((number-of-officers / total-population) / officer-optimum)^1 x officer-leadership-amount + national-modifier-to-leadership x (national-modifier-to-leadership-modifier + 1) leadership points per month.
	*/

	state.world.execute_serial_over_nation([&, optimum_officers = state.world.pop_type_get_research_optimum(state.culture_definitions.officers)](auto ids) {
		auto ofrac = state.world.nation_get_demographics(ids, demographics::to_key(state, state.culture_definitions.officers)) / ve::max(state.world.nation_get_demographics(ids, demographics::total), 1.0f);
		auto omod = ve::min(1.0f, ofrac / optimum_officers) * float(state.culture_definitions.officer_leadership_points);
		auto nmod = (state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::leadership_modifier) + 1.0f) * state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::leadership);

		state.world.nation_set_leadership_points(ids, state.world.nation_get_leadership_points(ids) + omod + nmod);
	});


	for(auto n : state.world.in_nation) {
		if(n.get_leadership_points() > state.defines.leader_recruit_cost * 3.0f) {
			// automatically make new leader
			auto new_l = [&]() {
				auto existing_leaders = count_leaders(state, n);
				auto army_count = count_armies(state, n);
				auto navy_count = count_navies(state, n);
				if(existing_leaders.generals < army_count) {
					return make_new_leader(state, n, true);
				} else if(existing_leaders.admirals < navy_count) {
					return make_new_leader(state, n, false);
				} else {
					auto too_many_generals = (existing_leaders.admirals > 0 && navy_count > 0) ? float(existing_leaders.generals) / float(existing_leaders.admirals) > float(army_count) / float(navy_count) : false;
					return make_new_leader(state, n, !too_many_generals);
				}
			}();
			if(state.world.leader_get_is_admiral(new_l)) {
				for(auto v : state.world.nation_get_navy_control(n)) {
					if(!v.get_navy().get_admiral_from_navy_leadership()) {
						state.world.try_create_navy_leadership(v.get_navy(), new_l);
						break;
					}
				}
			} else {
				for(auto a : state.world.nation_get_army_control(n)) {
					if(!a.get_army().get_general_from_army_leadership()) {
						state.world.try_create_army_leadership(a.get_army(), new_l);
						break;
					}
				}
			}
			
		}
	}
}
void daily_leaders_update(sys::state& state) {
	/*
	Leaders who are both less than 26 years old and not in combat have no chance of death. Otherwise, we take the age of the leader and divide by define:LEADER_AGE_DEATH_FACTOR. Then we multiply that result by 2 if the leader is currently in combat. That is then the leader's current chance of death out of ... my notes say 11,000 here.
	*/

	for(uint32_t i = state.world.leader_size(); i-->0; ) {
		dcon::leader_id l{dcon::leader_id::value_base_t(i) };
		auto age_in_days = state.world.leader_get_since(l).to_raw_value() * 365;
		if(age_in_days > 365 * 26) { // assume leaders are created at age 20; no death chance prior to 46
			float age_in_years = float(age_in_days) / 365.0f;
			float death_chance = (age_in_years * (leader_is_in_combat(state, l) ? 2.0f : 1.0f) / state.defines.leader_age_death_factor) / 11000.0f;

			/*
			float live_chance = 1.0f - death_chance;
			float live_chance_2 = live_chance * live_chance;
			float live_chance_4 = live_chance_2 * live_chance_2;
			float live_chance_8 = live_chance_4 * live_chance_4;
			float live_chance_16 = live_chance_8 * live_chance_8;
			float live_chance_32 = live_chance_16 * live_chance_16;

			float monthly_chance = 1.0f - (live_chance_32 / live_chance_2);
			*/

			auto int_chance = uint32_t(death_chance * float(0xFFFFFFFF));
			auto rvalue = uint32_t(rng::get_random(state, uint32_t(l.index())) & 0xFFFFFFFF);

			if(rvalue < int_chance)
				kill_leader(state, l);
		}
	}
}

bool has_truce_with(sys::state const& state, dcon::nation_id attacker, dcon::nation_id target) {
	// TODO
	return false;
}

}
