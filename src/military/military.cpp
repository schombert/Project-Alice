#include "military.hpp"
#include "military_templates.hpp"
#include "dcon_generated.hpp"
#include "prng.hpp"
#include "effects.hpp"
#include "events.hpp"
#include "ai.hpp"
#include "ai_war.hpp"
#include "demographics.hpp"
#include "politics.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "triggers.hpp"
#include "container_types.hpp"
#include "math_fns.hpp"

namespace military {


// this function should be used
// only for provinces owned by a war participant
// US5AC4. What % of the province score should be counted towards occupation. [0.0f;1.0f].
float share_province_score_for_war_occupation(sys::state& state, dcon::war_id w, dcon::province_id p) {
	auto controller = state.world.province_get_nation_from_province_control(p);
	auto owner = state.world.province_get_nation_from_province_ownership(p);
	// province must be occupied
	if(owner == controller) {
		return 0.f;
	}
	// rebels do not count
	if(!controller) {
		return 0.f;
	}
	// US5AC5. count 50% of occupation score for wars declared after targetted war
	auto date = state.world.war_get_start_date(w);
	for(auto candidate_war : state.world.nation_get_war_participant(owner)) {
		if(candidate_war.get_war() == w)
			continue;
		auto is_attacker = candidate_war.get_is_attacker();
		for(auto o : candidate_war.get_war().get_war_participant()) {
			if(o.get_nation() == controller) {
				auto& candidate_date = candidate_war.get_war().get_start_date();
				if(candidate_date < date) {
					return 0.f;
				}
				else {
					return 1.f;
				}
			}
		}
	}
	return 1.f;
}

template auto province_is_blockaded<ve::tagged_vector<dcon::province_id>>(sys::state const&, ve::tagged_vector<dcon::province_id>);
template auto province_is_under_siege<ve::tagged_vector<dcon::province_id>>(sys::state const&, ve::tagged_vector<dcon::province_id>);
template auto battle_is_ongoing_in_province<ve::tagged_vector<dcon::province_id>>(sys::state const&, ve::tagged_vector<dcon::province_id>);

constexpr inline float org_dam_mul = 0.18f;
constexpr inline float str_dam_mul = 0.14f;

int32_t total_regiments(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_active_regiments(n);
}
int32_t total_ships(sys::state& state, dcon::nation_id n) {
	int32_t total = 0;
	for(auto v : state.world.nation_get_navy_control(n)) {
		auto srange = v.get_navy().get_navy_membership();
		total += int32_t(srange.end() - srange.begin());
	}
	return total;
}

bool is_infantry_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given) {
	if(!best) {
		return true;
	}
	auto curbeststats = state.world.nation_get_unit_stats(n, best);
	auto& curbestdef = (curbeststats.discipline_or_evasion > 0.0f) ? curbeststats : state.military_definitions.unit_base_definitions[best];

	auto stats = state.world.nation_get_unit_stats(n, given);
	auto& def = (stats.discipline_or_evasion > 0.0f) ? stats : state.military_definitions.unit_base_definitions[given];

	if(def.defence_or_hull > curbestdef.defence_or_hull) {
		return true;
	} else if(def.defence_or_hull == curbestdef.defence_or_hull && def.default_organisation > curbestdef.default_organisation) {
		return true;
	} else if(def.defence_or_hull == curbestdef.defence_or_hull && def.default_organisation == curbestdef.default_organisation && def.attack_or_gun_power > curbestdef.attack_or_gun_power) {
		return true;
	}
	return false;
}

bool is_artillery_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given) {
	if(!best) {
		return true;
	}
	auto curbeststats = state.world.nation_get_unit_stats(n, best);
	auto& curbestdef = (curbeststats.discipline_or_evasion > 0.0f) ? curbeststats : state.military_definitions.unit_base_definitions[best];

	auto stats = state.world.nation_get_unit_stats(n, given);
	auto& def = (stats.discipline_or_evasion > 0.0f) ? stats : state.military_definitions.unit_base_definitions[given];

	if(def.support > curbestdef.support) {
		return true;
	} else if(def.support == curbestdef.support && def.discipline_or_evasion > curbestdef.discipline_or_evasion) {
		return true;
	} else if(def.support == curbestdef.support && def.discipline_or_evasion == curbestdef.discipline_or_evasion && def.siege_or_torpedo_attack > curbestdef.siege_or_torpedo_attack) {
		return true;
	}
	return false;
}

bool is_cavalry_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given) {
	if(!best) {
		return true;
	}
	auto curbeststats = state.world.nation_get_unit_stats(n, best);
	auto& basedef = state.military_definitions.unit_base_definitions[best];
	auto& curbestdef = (curbeststats.discipline_or_evasion > 0.0f) ? curbeststats : basedef;

	auto stats = state.world.nation_get_unit_stats(n, given);
	auto& newbasedef = state.military_definitions.unit_base_definitions[given];
	auto& def = (stats.discipline_or_evasion > 0.0f) ? stats : newbasedef;

	if(def.reconnaissance_or_fire_range > curbestdef.reconnaissance_or_fire_range) {
		return true;
	} else if(def.reconnaissance_or_fire_range == curbestdef.reconnaissance_or_fire_range && def.attack_or_gun_power > curbestdef.attack_or_gun_power) {
		return true;
	} else if(def.reconnaissance_or_fire_range == curbestdef.reconnaissance_or_fire_range && def.attack_or_gun_power == curbestdef.attack_or_gun_power && basedef.maneuver > newbasedef.maneuver) {
		return true;
	}
	return false;
}

bool is_transport_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given) {
	if(!best) {
		return true;
	}
	auto curbeststats = state.world.nation_get_unit_stats(n, best);
	auto& curbestdef = (curbeststats.discipline_or_evasion > 0.0f) ? curbeststats : state.military_definitions.unit_base_definitions[best];

	auto stats = state.world.nation_get_unit_stats(n, given);
	auto& def = (stats.discipline_or_evasion > 0.0f) ? stats : state.military_definitions.unit_base_definitions[given];

	if(def.maximum_speed > curbestdef.maximum_speed) {
		return true;
	} else if(def.maximum_speed == curbestdef.maximum_speed && def.defence_or_hull > curbestdef.defence_or_hull) {
		return true;
	} else if(def.maximum_speed == curbestdef.maximum_speed && def.defence_or_hull == curbestdef.defence_or_hull && def.attack_or_gun_power > curbestdef.attack_or_gun_power) {
		return true;
	}
	return false;
}

bool is_light_ship_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given) {
	if(!best) {
		return true;
	}
	auto curbeststats = state.world.nation_get_unit_stats(n, best);
	auto& curbestdef = (curbeststats.discipline_or_evasion > 0.0f) ? curbeststats : state.military_definitions.unit_base_definitions[best];

	auto stats = state.world.nation_get_unit_stats(n, given);
	auto& def = (stats.discipline_or_evasion > 0.0f) ? stats : state.military_definitions.unit_base_definitions[given];

	if(def.maximum_speed > curbestdef.maximum_speed) {
		return true;
	} else if(def.maximum_speed == curbestdef.maximum_speed && def.attack_or_gun_power > curbestdef.attack_or_gun_power) {
		return true;
	} else if(def.maximum_speed == curbestdef.maximum_speed && def.attack_or_gun_power == curbestdef.attack_or_gun_power && def.defence_or_hull > curbestdef.defence_or_hull) {
		return true;
	}
	return false;
}

bool is_big_ship_better(sys::state& state, dcon::nation_id n, dcon::unit_type_id best, dcon::unit_type_id given) {
	if(!best) {
		return true;
	}
	auto curbeststats = state.world.nation_get_unit_stats(n, best);
	auto& curbestdef = (curbeststats.discipline_or_evasion > 0.0f) ? curbeststats : state.military_definitions.unit_base_definitions[best];

	auto stats = state.world.nation_get_unit_stats(n, given);
	auto& def = (stats.discipline_or_evasion > 0.0f) ? stats : state.military_definitions.unit_base_definitions[given];

	if(def.attack_or_gun_power > curbestdef.attack_or_gun_power) {
		return true;
	} else if(def.attack_or_gun_power == curbestdef.attack_or_gun_power && def.defence_or_hull > curbestdef.defence_or_hull) {
		return true;
	} else if(def.attack_or_gun_power == curbestdef.attack_or_gun_power && def.defence_or_hull == curbestdef.defence_or_hull && def.siege_or_torpedo_attack > curbestdef.siege_or_torpedo_attack) {
		return true;
	}
	return false;
}

// Evaluate if the nation can reasonably build this unit type
bool will_have_shortages_building_unit(sys::state& state, dcon::nation_id n, dcon::unit_type_id type) {
	auto& def = state.military_definitions.unit_base_definitions[type];

	bool lacking_input = false;

	auto m = state.world.nation_get_capital(n).get_state_membership().get_market_from_local_market();

	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(def.build_cost.commodity_type[i]) {
			if(m.get_demand_satisfaction(def.build_cost.commodity_type[i]) < 0.1f && m.get_demand(def.build_cost.commodity_type[i]) > 0.01f)
				lacking_input = true;
		} else {
			break;
		}
	}

	return lacking_input;
}

dcon::unit_type_id get_best_infantry(sys::state& state, dcon::nation_id n, bool primary_culture, bool evaluate_shortages) {
	dcon::unit_type_id curbest = state.military_definitions.irregular;

	auto ndef = state.world.nation_get_identity_from_identity_holder(n);

	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto& def = state.military_definitions.unit_base_definitions[uid];

		if(def.type != military::unit_type::infantry) {
			continue;
		}

		auto available = state.world.nation_get_active_unit(n, uid) || def.active;

		if(!available) {
			continue;
		}

		if(def.primary_culture && !primary_culture) {
			continue;
		}

		if(evaluate_shortages && will_have_shortages_building_unit(state, n, uid)) {
			continue;
		}

		if(is_infantry_better(state, n, curbest, uid)) {
			curbest = uid;
		}
	}
	return curbest;
}

dcon::unit_type_id get_best_artillery(sys::state& state, dcon::nation_id n, bool primary_culture, bool evaluate_shortages) {
	dcon::unit_type_id curbest;

	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto def = state.military_definitions.unit_base_definitions[uid];

		if(def.type != military::unit_type::support) {
			continue;
		}

		auto available = state.world.nation_get_active_unit(n, uid) || def.active;

		if(!available) {
			continue;
		}

		if(def.primary_culture && !primary_culture) {
			continue;
		}

		if(evaluate_shortages && will_have_shortages_building_unit(state, n, uid)) {
			continue;
		}

		if(is_artillery_better(state, n, curbest, uid)) {
			curbest = uid;
		}
	}
	return curbest;
}

dcon::unit_type_id get_best_cavalry(sys::state& state, dcon::nation_id n, bool primary_culture, bool evaluate_shortages) {
	dcon::unit_type_id curbest;
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };

		auto def = state.military_definitions.unit_base_definitions[uid];

		if(def.type != military::unit_type::cavalry) {
			continue;
		}

		auto available = state.world.nation_get_active_unit(n, uid) || def.active;

		if(!available) {
			continue;
		}

		if(def.primary_culture && !primary_culture) {
			continue;
		}

		if(evaluate_shortages && will_have_shortages_building_unit(state, n, uid)) {
			continue;
		}

		if(is_cavalry_better(state, n, curbest, uid)) {
			curbest = uid;
		}
	}
	return curbest;
}

dcon::unit_type_id get_best_transport(sys::state& state, dcon::nation_id n, bool primary_culture, bool evaluate_shortages) {
	dcon::unit_type_id curbest;

	auto ndef = state.world.nation_get_identity_from_identity_holder(n);

	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto& def = state.military_definitions.unit_base_definitions[uid];

		auto available = state.world.nation_get_active_unit(n, uid) || def.active;
		if(!available || def.type != military::unit_type::transport || (def.primary_culture && !primary_culture)) {
			continue;
		}

		if(evaluate_shortages && will_have_shortages_building_unit(state, n, uid)) {
			continue;
		}

		if(is_transport_better(state, n, curbest, uid)) {
			curbest = uid;
		}
	}
	return curbest;
}

dcon::unit_type_id get_best_light_ship(sys::state& state, dcon::nation_id n, bool primary_culture, bool evaluate_shortages) {
	dcon::unit_type_id curbest;

	auto ndef = state.world.nation_get_identity_from_identity_holder(n);

	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto& def = state.military_definitions.unit_base_definitions[uid];

		auto available = state.world.nation_get_active_unit(n, uid) || def.active;
		if(!available || def.type != military::unit_type::light_ship || (def.primary_culture && !primary_culture)) {
			continue;
		}

		if(evaluate_shortages && will_have_shortages_building_unit(state, n, uid)) {
			continue;
		}

		if(is_light_ship_better(state, n, curbest, uid)) {
			curbest = uid;
		}
	}
	return curbest;
}

dcon::unit_type_id get_best_big_ship(sys::state& state, dcon::nation_id n, bool primary_culture, bool evaluate_shortages) {
	dcon::unit_type_id curbest;

	auto ndef = state.world.nation_get_identity_from_identity_holder(n);

	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto& def = state.military_definitions.unit_base_definitions[uid];

		auto available = state.world.nation_get_active_unit(n, uid) || def.active;
		if(!available || def.type != military::unit_type::big_ship || (def.primary_culture && !primary_culture)) {
			continue;
		}

		if(evaluate_shortages && will_have_shortages_building_unit(state, n, uid)) {
			continue;
		}

		if(is_big_ship_better(state, n, curbest, uid)) {
			curbest = uid;
		}
	}
	return curbest;
}

void reset_unit_stats(sys::state& state) {
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		state.world.for_each_nation([&](dcon::nation_id nid) {
			state.world.nation_set_unit_stats(nid, uid, state.military_definitions.unit_base_definitions[uid]);
		});
	}
}

void apply_base_unit_stat_modifiers(sys::state& state) {
	assert(state.military_definitions.base_army_unit.index() < 2);
	assert(state.military_definitions.base_naval_unit.index() < 2);
	assert(state.military_definitions.base_army_unit.index() != -1);
	assert(state.military_definitions.base_naval_unit.index() != -1);
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{ dcon::unit_type_id::value_base_t(i) };
		auto base_id = state.military_definitions.unit_base_definitions[uid].is_land
			? state.military_definitions.base_army_unit
			: state.military_definitions.base_naval_unit;
		state.world.for_each_nation([&](dcon::nation_id nid) {
			auto& base_stats = state.world.nation_get_unit_stats(nid, base_id);
			auto& current_stats = state.world.nation_get_unit_stats(nid, uid);
			current_stats += base_stats;
			state.world.nation_set_unit_stats(nid, uid, current_stats); // set the value in dcon even after the += assignment to trap invalid stores and avoid having to create a new struct
			assert(current_stats.maximum_speed > 0.f);
		});
	}
}

void invalidate_unowned_wargoals(sys::state& state) {
	for(auto wg : state.world.in_wargoal) {
		if(wg.get_war_from_wargoals_attached()) {
			auto s = wg.get_associated_state();
			if(s) {
				bool found_state = false;
				for(auto si : wg.get_target_nation().get_state_ownership()) {
					if(si.get_state().get_definition() == s) {
						found_state = true;
						break;
					}
				}
				if(!found_state) {
					state.world.delete_wargoal(wg);
				}
			}
		}
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

bool can_add_always_cb_to_war(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb, dcon::war_id w) {

	auto can_use = state.world.cb_type_get_can_use(cb);
	if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(target))) {
		return false;
	}

	auto allowed_countries = state.world.cb_type_get_allowed_countries(cb);
	auto allowed_states = state.world.cb_type_get_allowed_states(cb);

	bool for_all_state = (state.world.cb_type_get_type_bits(cb) & military::cb_flag::all_allowed_states) != 0;

	if(!allowed_countries && allowed_states) {
		if(for_all_state) {
			for(auto wg : state.world.war_get_wargoals_attached(w)) {
				if(wg.get_wargoal().get_added_by() == actor
					&& wg.get_wargoal().get_type() == cb
					&& wg.get_wargoal().get_target_nation() == target) {

					return false;
				}
			}
		}

		bool any_allowed = false;
		for(auto si : state.world.nation_get_state_ownership(target)) {
			if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
				trigger::to_generic(actor))) {

				any_allowed = true;

				for(auto wg : state.world.war_get_wargoals_attached(w)) {
					if(wg.get_wargoal().get_added_by() == actor
						&& wg.get_wargoal().get_type() == cb
						&& wg.get_wargoal().get_associated_state() == si.get_state().get_definition()
						&& wg.get_wargoal().get_target_nation() == target) {

						any_allowed = false;
						break;
					}
				}
				if(any_allowed)
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
				if(n != actor) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(n.id))) {
						if(allowed_states) { // check whether any state within the target is valid for free / liberate

							bool found_dup = false;

							if(for_all_state) {
								for(auto wg : state.world.war_get_wargoals_attached(w)) {
									if(wg.get_wargoal().get_added_by() == actor
										&& wg.get_wargoal().get_type() == cb
										&& (wg.get_wargoal().get_associated_tag() == n.get_identity_from_identity_holder()
											|| wg.get_wargoal().get_secondary_nation() == n)
										&& wg.get_wargoal().get_target_nation() == target) {

										found_dup = true;
									}
								}
							}

							if(!found_dup) {
								for(auto si : state.world.nation_get_state_ownership(target)) {
									if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(n.id))) {


										for(auto wg : state.world.war_get_wargoals_attached(w)) {
											if(wg.get_wargoal().get_added_by() == actor
												&& wg.get_wargoal().get_type() == cb
												&& (wg.get_wargoal().get_associated_tag() == n.get_identity_from_identity_holder()
													|| wg.get_wargoal().get_secondary_nation() == n)
												&& wg.get_wargoal().get_associated_state() == si.get_state().get_definition()
												&& wg.get_wargoal().get_target_nation() == target) {

												found_dup = true;
												break;
											}
										}
									}
								}
							}

							if(!found_dup)
								return true;

						} else { // no allowed states trigger -> nation is automatically a valid target
							bool found_dup = false;

							for(auto wg : state.world.war_get_wargoals_attached(w)) {
								if(wg.get_wargoal().get_added_by() == actor
									&& wg.get_wargoal().get_type() == cb
									&& (wg.get_wargoal().get_associated_tag() == n.get_identity_from_identity_holder()
										|| wg.get_wargoal().get_secondary_nation() == n)
									&& wg.get_wargoal().get_target_nation() == target) {

									found_dup = true;
								}
							}

							if(!found_dup) {
								return true;
							}
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

bool cb_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb) {
	auto can_use = state.world.cb_type_get_can_use(cb);
	if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(target))) {
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
						if(trigger::evaluate(state, allowed_substates, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
							trigger::to_generic(actor))) {
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
				if(n != actor) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(n.id))) {
						if(allowed_states) { // check whether any state within the target is valid for free / liberate
							for(auto si : state.world.nation_get_state_ownership(target)) {
								if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
									trigger::to_generic(n.id))) {
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

bool cb_instance_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb,
		dcon::state_definition_id st, dcon::national_identity_id tag, dcon::nation_id secondary) {

	auto can_use = state.world.cb_type_get_can_use(cb);
	if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(target))) {
		return false;
	}

	auto allowed_countries = state.world.cb_type_get_allowed_countries(cb);
	auto allowed_states = state.world.cb_type_get_allowed_states(cb);

	if(!allowed_countries && allowed_states) {
		bool any_allowed = false;
		if(cb_requires_selection_of_a_state(state, cb)) {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(si.get_state().get_definition() == st &&
						trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
							trigger::to_generic(actor))) {
					any_allowed = true;
					break;
				}
			}
		} else {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
					trigger::to_generic(actor))) {
					any_allowed = true;
					break;
				}
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
					if(cb_requires_selection_of_a_state(state, cb)) {
						for(auto si : state.world.nation_get_state_ownership(v.get_subject())) {
							if(si.get_state().get_definition() == st &&
									trigger::evaluate(state, allowed_substates, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
										trigger::to_generic(actor))) {
								return true;
							}
						}
					} else {
						for(auto si : state.world.nation_get_state_ownership(v.get_subject())) {
							if(trigger::evaluate(state, allowed_substates, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
								trigger::to_generic(actor))) {
								return true;
							}
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
		auto secondary_nation = secondary ? secondary : state.world.national_identity_get_nation_from_identity_holder(tag);

		if(secondary_nation != actor && trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(secondary_nation))) {
			bool validity = false;
			if(allowed_states) { // check whether any state within the target is valid for free / liberate
				if((state.world.cb_type_get_type_bits(cb) & cb_flag::all_allowed_states) != 0) {
					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
							trigger::to_generic(secondary_nation))) {
							validity = true;
							break;
						}
					}
				} else {
					for(auto si : state.world.nation_get_state_ownership(target)) {
						if(si.get_state().get_definition() == st &&
								trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
									trigger::to_generic(secondary_nation))) {
							validity = true;
							break;
						}
					}
				}
			} else { // no allowed states trigger -> nation is automatically a valid target
				validity = true;
			}
			if(!validity)
				return false;
		} else {
			return false;
		}
	}

	return true;
}


bool province_is_blockaded(sys::state const& state, dcon::province_id ids) {
	return state.world.province_get_is_blockaded(ids);
}
// returns true if the specified province is blockaded by an enemy navy (does not nessecarily mean the province in question will receive blockade penalties, just that there is an enemy navy
bool province_is_blockaded_by_enemy(sys::state& state, dcon::province_id prov, dcon::nation_id thisnation) {
	assert(state.world.province_get_is_coast(prov)); // should not be called on land provinces ever
	auto sea_prov = state.world.province_get_port_to(prov);
	for(auto navy : state.world.province_get_navy_location(sea_prov)) {
		if(military::are_at_war(state, thisnation, navy.get_navy().get_controller_from_navy_control())) {
			return true;
		}
	}

	return false;
}

bool compute_blockade_status(sys::state& state, dcon::province_id p) {
	auto controller = state.world.province_get_nation_from_province_control(p);
	auto owner = state.world.province_get_nation_from_province_ownership(p);
	if(!owner)
		return false;

	auto port_to = state.world.province_get_port_to(p);
	if(!port_to)
		return false;

	for(auto n : state.world.province_get_navy_location(port_to)) {
		if(n.get_navy().get_is_retreating() == false && !n.get_navy().get_battle_from_navy_battle_participation()) {
			if(military::are_at_war(state, controller, n.get_navy().get_controller_from_navy_control()))
				return true;
		}
	}
	return false;
}

void update_blockade_status(sys::state& state) {
	province::for_each_land_province(state, [&](dcon::province_id p) {
		state.world.province_set_is_blockaded(p, compute_blockade_status(state, p));
	});
}

bool province_is_under_siege(sys::state const& state, dcon::province_id ids) {
	return state.world.province_get_siege_progress(ids) > 0.0f;
}

float recruited_pop_fraction(sys::state const& state, dcon::nation_id n) {
	auto current = float(state.world.nation_get_active_regiments(n));
	auto maximum = float(state.world.nation_get_recruitable_regiments(n));
	return maximum > 0.0f ? current / maximum : 1.0f;
}

bool state_has_naval_base(sys::state const& state, dcon::state_instance_id si) {
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	auto def = state.world.state_instance_get_definition(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(def)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			if(p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)) > 0)
				return true;
		}
	}
	return false;
}

uint32_t state_naval_base_level(sys::state const& state, dcon::state_instance_id si) {
	uint32_t level = 0;
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	auto def = state.world.state_instance_get_definition(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(def)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			level += p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base));
		}
	}
	return level;
}

uint32_t state_railroad_level(sys::state const& state, dcon::state_instance_id si) {
	uint32_t level = 0;
	auto owner = state.world.state_instance_get_nation_from_state_ownership(si);
	auto def = state.world.state_instance_get_definition(si);
	for(auto p : state.world.state_definition_get_abstract_state_membership(def)) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			level += p.get_province().get_building_level(uint8_t(economy::province_building_type::railroad));
		}
	}
	return level;
}
// wrapper for are_at_war which also returns true if one of the tags is rebel, and the other is not
bool are_enemies(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	if((!a && a) || (b && !a)) {
		return true;
	} else {
		return are_at_war(state, a, b);
	}
}

bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	if(!state.world.nation_get_is_at_war(a) || !state.world.nation_get_is_at_war(b))
		return false;
	for(auto wa : state.world.nation_get_war_participant(a)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() == b)
				return o.get_is_attacker() != is_attacker;
		}
	}
	return false;
}

bool are_allied_in_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	for(auto wa : state.world.nation_get_war_participant(a)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() == b)
				return o.get_is_attacker() == is_attacker;
		}
	}
	return false;
}

bool are_in_common_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	for(auto wa : state.world.nation_get_war_participant(a)) {
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() == b)
				return true;
		}
	}
	return false;
}

void remove_from_common_allied_wars(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	// Since removing primary participant effectively stops the war without peace treaty or truces, we shouldn't do it.
	// If A is war leader, remove B. If B is war leader, remove A

	std::vector<dcon::war_id> removeA;
	std::vector<dcon::war_id> removeB;

	// For every war of A, if B is participant on the same side and not primary, remove B
	for(auto wa : state.world.nation_get_war_participant(a)) {
		for(auto o : wa.get_war().get_war_participant()) {
			auto is_primary = (o.get_is_attacker()) ? o.get_war().get_primary_attacker() == o.get_nation() : o.get_war().get_primary_defender() == o.get_nation();
			if(o.get_nation() == b && o.get_is_attacker() == wa.get_is_attacker() && !is_primary) {
				removeB.push_back(wa.get_war());
			}
		}
	}
	// For every war of B, if A is participant on the same side and not primary, remove A
	for(auto wa : state.world.nation_get_war_participant(b)) {
		for(auto o : wa.get_war().get_war_participant()) {
			auto is_primary = (o.get_is_attacker()) ? o.get_war().get_primary_attacker() == o.get_nation() : o.get_war().get_primary_defender() == o.get_nation();
			if(o.get_nation() == a && o.get_is_attacker() == wa.get_is_attacker() && !is_primary) {
				removeA.push_back(wa.get_war());
			}
		}
	}

	for(const auto w : removeA) {
		military::remove_from_war(state, w, a, false);
	}
	for(const auto w : removeB) {
		military::remove_from_war(state, w, b, false);
	}

}

struct participation {
	dcon::war_id w;
	war_role role = war_role::none;
};

participation internal_find_war_between(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	for(auto wa : state.world.nation_get_war_participant(a)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() == b) {
				if(o.get_is_attacker() != is_attacker)
					return participation{ wa.get_war().id, is_attacker ? war_role::attacker : war_role::defender };
				else
					return participation{};
			}
		}
	}
	return participation{};
}

dcon::war_id find_war_between(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	return internal_find_war_between(state, a, b).w;
}

bool joining_war_does_not_violate_constraints(sys::state const& state, dcon::nation_id a, dcon::war_id w, bool as_attacker) {
	auto target_war_participants = state.world.war_get_war_participant(w);
	for(auto wa : state.world.nation_get_war_participant(a)) {
		for(auto other_participants : wa.get_war().get_war_participant()) {
			if(other_participants.get_is_attacker() ==
					wa.get_is_attacker()) { // case: ally on same side -- must not be on opposite site
				for(auto tp : target_war_participants) {
					if(tp.get_nation() == other_participants.get_nation() && tp.get_is_attacker() != as_attacker)
						return false;
				}
			} else { // case opponent -- must not be in new war at all
				for(auto tp : target_war_participants) {
					if(tp.get_nation() == other_participants.get_nation())
						return false;
				}
			}
		}
	}
	return true;
}

bool is_civil_war(sys::state const& state, dcon::war_id w) {
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if((wg.get_wargoal().get_type().get_type_bits() & cb_flag::is_civil_war) != 0)
			return true;
	}
	return false;
}

bool is_defender_wargoal(sys::state const& state, dcon::war_id w, dcon::wargoal_id wg) {
	auto from = state.world.wargoal_get_added_by(wg);
	for(auto p : state.world.war_get_war_participant(w)) {
		if(p.get_nation() == from)
			return !p.get_is_attacker();
	}
	return false;
}

bool defenders_have_non_status_quo_wargoal(sys::state const& state, dcon::war_id w) {
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if((wg.get_wargoal().get_type().get_type_bits() & cb_flag::po_status_quo) == 0 && is_defender_wargoal(state, w, wg.get_wargoal()))
			return true;
	}
	return false;
}

bool defenders_have_status_quo_wargoal(sys::state const& state, dcon::war_id w) {
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if((wg.get_wargoal().get_type().get_type_bits() & cb_flag::po_status_quo) != 0 && is_defender_wargoal(state, w, wg.get_wargoal()))
			return true;
	}
	return false;
}
bool attackers_have_status_quo_wargoal(sys::state const& state, dcon::war_id w) {
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if((wg.get_wargoal().get_type().get_type_bits() & cb_flag::po_status_quo) != 0 && !is_defender_wargoal(state, w, wg.get_wargoal()))
			return true;
	}
	return false;
}

bool joining_as_attacker_would_break_truce(sys::state& state, dcon::nation_id a, dcon::war_id w) {
	for(auto p : state.world.war_get_war_participant(w)) {
		if(p.get_is_attacker() == false) {
			if(has_truce_with(state, a, p.get_nation())) {
				return true;
			}
		}
	}
	return false;
}

template<regiment_dmg_source damage_source>
float get_war_exhaustion_from_land_losses(sys::state& state, float strength_losses, dcon::nation_id controller) {
	switch(damage_source) {
		case regiment_dmg_source::combat:
			return state.defines.combatloss_war_exhaustion * strength_losses / std::max(1.0f, float(state.world.nation_get_recruitable_regiments(controller)));
		case regiment_dmg_source::attrition:
			return state.defines.alice_attrition_war_exhaustion * strength_losses / std::max(1.0f, float(state.world.nation_get_recruitable_regiments(controller)));
		default:
			assert(false);
			std::abort();
	}
}

int32_t supply_limit_in_province(sys::state& state, dcon::nation_id n, dcon::province_id p) {
	/*
	(province-supply-limit-modifier + 1) x (2.5 if it is owned an controlled or 2 if it is just controlled, you are allied to the
	controller, have military access with the controller, a rebel controls it, it is one of your core provinces, or you are
	sieging it) x (technology-supply-limit-modifier + 1)
	*/
	float modifier = 1.0f;
	auto prov_controller = state.world.province_get_nation_from_province_control(p);
	auto self_controlled = prov_controller == n;
	if(state.world.province_get_nation_from_province_ownership(p) == n && self_controlled) {
		modifier = 2.5f;
	} else if(self_controlled ||
						bool(state.world.province_get_rebel_faction_from_province_rebel_control(p))) { // TODO: check for sieging
		modifier = 2.0f;
	} else if(auto dip_rel = state.world.get_diplomatic_relation_by_diplomatic_pair(prov_controller, n);
						state.world.diplomatic_relation_get_are_allied(dip_rel)) {
		modifier = 2.0f;
	} else if(province::has_safe_access_to_province(state, n, p)) {
		modifier = 2.0f;
	} else if(bool(state.world.get_core_by_prov_tag_key(p, state.world.nation_get_identity_from_identity_holder(n)))) {
		modifier = 2.0f;
	} else if(state.world.province_get_siege_progress(p) > 0.0f) {
		modifier = 2.0f;
	}
	auto base_supply_lim = (state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::supply_limit) + 1.0f);
	auto national_supply_lim = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::supply_limit) + 1.0f);
	return std::max(int32_t(base_supply_lim * modifier * national_supply_lim), 0);
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
		if(pop.get_pop().get_poptype() != state.culture_definitions.soldiers &&
				pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {
			auto regs = pop.get_pop().get_regiment_source();
			total += int32_t(regs.end() - regs.begin());
		}
	}
	return total;
}

int32_t regiments_possible_from_pop(sys::state& state, dcon::pop_id p) {
	auto type = state.world.pop_get_poptype(p);
	if(type == state.culture_definitions.soldiers) {
		auto location = state.world.pop_get_province_from_pop_location(p);
		if(state.world.province_get_is_colonial(location)) {
			float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
			float minimum = state.defines.pop_min_size_for_regiment;

			if(state.world.pop_get_size(p) >= minimum) {
				return int32_t((state.world.pop_get_size(p) / divisor) + 1);
			}
			return 0;
		} else if(!state.world.province_get_is_owner_core(location)) {
			float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
			float minimum = state.defines.pop_min_size_for_regiment;

			if(state.world.pop_get_size(p) >= minimum) {
				return int32_t((state.world.pop_get_size(p) / divisor) + 1);
			}
			return 0;
		} else {
			float divisor = state.defines.pop_size_per_regiment;
			float minimum = state.defines.pop_min_size_for_regiment;

			if(state.world.pop_get_size(p) >= minimum) {
				return int32_t((state.world.pop_get_size(p) / divisor) + 1);
			}
			return 0;
		}
	} else { // mobilized
		return int32_t(state.world.pop_get_size(p) * mobilization_size(state, nations::owner_of_pop(state, p)) / state.defines.pop_size_per_regiment);
	}
}

int32_t regiments_max_possible_from_province(sys::state& state, dcon::province_id p) {
	/*
	- A soldier pop must be at least define:POP_MIN_SIZE_FOR_REGIMENT to support any regiments
	- If it is at least that large, then it can support one regiment per define:POP_SIZE_PER_REGIMENT x
	define:POP_MIN_SIZE_FOR_REGIMENT_COLONY_MULTIPLIER (if it is located in a colonial province) x
	define:POP_MIN_SIZE_FOR_REGIMENT_NONCORE_MULTIPLIER (if it is non-colonial but uncored)
	*/
	int32_t total = 0;
	if(state.world.province_get_is_colonial(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum) {
					total += int32_t((pop.get_pop().get_size() / divisor) + 1);
				}
			}
		}
	} else if(!state.world.province_get_is_owner_core(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum) {
					total += int32_t((pop.get_pop().get_size() / divisor) + 1);
				}
			}
		}
	} else {
		float divisor = state.defines.pop_size_per_regiment;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum) {
					total += int32_t((pop.get_pop().get_size() / divisor) + 1);
				}
			}
		}
	}
	return total;
}
int32_t main_culture_regiments_created_from_province(sys::state& state, dcon::province_id p) {
	int32_t total = 0;
	for(auto pop : state.world.province_get_pop_location(p)) {
		if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers && pop.get_pop().get_is_primary_or_accepted_culture()) {
			auto regs = pop.get_pop().get_regiment_source();
			total += int32_t(regs.end() - regs.begin());
		}
	}
	return total;
}
int32_t main_culture_regiments_max_possible_from_province(sys::state& state, dcon::province_id p) {
	/*
	- A soldier pop must be at least define:POP_MIN_SIZE_FOR_REGIMENT to support any regiments
	- If it is at least that large, then it can support one regiment per define:POP_SIZE_PER_REGIMENT x
	define:POP_MIN_SIZE_FOR_REGIMENT_COLONY_MULTIPLIER (if it is located in a colonial province) x
	define:POP_MIN_SIZE_FOR_REGIMENT_NONCORE_MULTIPLIER (if it is non-colonial but uncored)
	*/
	int32_t total = 0;
	if(state.world.province_get_is_colonial(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers &&
					pop.get_pop().get_is_primary_or_accepted_culture()) {
				if(pop.get_pop().get_size() >= minimum) {
					total += int32_t((pop.get_pop().get_size() / divisor) + 1);
				}
			}
		}
	} else if(!state.world.province_get_is_owner_core(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers &&
					pop.get_pop().get_is_primary_or_accepted_culture()) {
				if(pop.get_pop().get_size() >= minimum) {
					total += int32_t(pop.get_pop().get_size() / divisor);
				}
			}
		}
	} else {
		float divisor = state.defines.pop_size_per_regiment;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers &&
					pop.get_pop().get_is_primary_or_accepted_culture()) {
				if(pop.get_pop().get_size() >= minimum) {
					total += int32_t(pop.get_pop().get_size() / divisor);
				}
			}
		}
	}
	return total;
}
int32_t regiments_under_construction_in_province(sys::state& state, dcon::province_id p) {
	int32_t total = 0;
	for(auto pop : state.world.province_get_pop_location(p)) {
		if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
			auto regs = pop.get_pop().get_province_land_construction();
			total += int32_t(regs.end() - regs.begin());
		}
	}
	return total;
}
int32_t main_culture_regiments_under_construction_in_province(sys::state& state, dcon::province_id p) {
	int32_t total = 0;
	for(auto pop : state.world.province_get_pop_location(p)) {
		if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers && pop.get_pop().get_is_primary_or_accepted_culture()) {
			auto regs = pop.get_pop().get_province_land_construction();
			total += int32_t(regs.end() - regs.begin());
		}
	}
	return total;
}


bool can_pop_form_regiment(sys::state& state, dcon::pop_id pop, float divisor) {
	if(state.world.pop_get_size(pop) >= state.defines.pop_min_size_for_regiment) {

		auto amount = int32_t((state.world.pop_get_size(pop) / divisor) + 1);
		auto regs = state.world.pop_get_regiment_source(pop);
		auto building = state.world.pop_get_province_land_construction(pop);

		if(amount > ((regs.end() - regs.begin()) + (building.end() - building.begin()))) {
			return true;
		}
	}
	return false;
}



dcon::pop_id find_available_soldier_anywhere(sys::state& state, dcon::nation_id nation, dcon::unit_type_id type) {

	const auto& unit_stats = state.military_definitions.unit_base_definitions[type];
	if(unit_stats.primary_culture) {
		return find_available_soldier_anywhere(state, nation, [&](sys::state& state, dcon::pop_id pop) {
			return state.world.pop_get_poptype(pop) == state.culture_definitions.soldiers && state.world.pop_get_is_primary_or_accepted_culture(pop);
		});
	}
	else {
		return find_available_soldier_anywhere(state, nation, [&](sys::state& state, dcon::pop_id pop) {
			return state.world.pop_get_poptype(pop) == state.culture_definitions.soldiers;
		});
	}
}


// Calculates whether province can support more regiments
// Considers existing regiments and construction as well
dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, dcon::culture_id pop_culture) {

	return find_available_soldier(state, p, [&](sys::state& state, dcon::pop_id pop) {
		return state.world.pop_get_poptype(pop) == state.culture_definitions.soldiers && state.world.pop_get_culture(pop) == pop_culture;
	});

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
	auto mobilization_size =
		std::max(0.0f, fp.get_nation_from_province_ownership().get_modifier_values(sys::national_mod_offsets::mobilization_size));

	for(auto pop : state.world.province_get_pop_location(p)) {
		/*
		In those provinces, mobilized regiments come from non-soldier, non-slave, poor-strata pops with a culture that is either
		the primary culture of the nation or an accepted culture.
		*/
		if(pop_eligible_for_mobilization(state, pop.get_pop())) {
			/*
			The number of regiments these pops can provide is determined by pop-size x mobilization-size /
			define:POP_SIZE_PER_REGIMENT.
			*/
			total += int32_t(pop.get_pop().get_size() * mobilization_size / state.defines.pop_size_per_regiment);
		}
	}
	return total;
}

int32_t mobilized_regiments_pop_limit(sys::state& state, dcon::nation_id n) {
	int32_t total = 0;
	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(p.get_province().get_is_colonial() == false)
			total += mobilized_regiments_possible_from_province(state, p.get_province());
	}
	return total;
}

void update_recruitable_regiments(sys::state& state, dcon::nation_id n) {
	state.world.nation_set_recruitable_regiments(n, uint16_t(0));
	auto& cur_regiments = state.world.nation_get_recruitable_regiments(n);
	for(auto p : state.world.nation_get_province_ownership(n)) {
		state.world.nation_set_recruitable_regiments(n, uint16_t(cur_regiments + uint16_t(regiments_max_possible_from_province(state, p.get_province()))));
	}
}
void update_all_recruitable_regiments(sys::state& state) {
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_recruitable_regiments(ids, ve::int_vector(0)); });
	state.world.for_each_province([&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(owner) {
			auto& cur_regiments = state.world.nation_get_recruitable_regiments(owner);
			state.world.nation_set_recruitable_regiments(owner, uint16_t(cur_regiments + uint16_t(regiments_max_possible_from_province(state, p))));
		}
	});
}
void regenerate_total_regiment_counts(sys::state& state) {
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_active_regiments(ids, ve::int_vector(0)); });
	state.world.for_each_army([&](dcon::army_id a) {
		auto owner = state.world.army_get_controller_from_army_control(a);
		if(owner) {
			auto regs_range = state.world.army_get_army_membership(a);
			auto num_regs = regs_range.end() - regs_range.begin();
			auto& cur_regiments = state.world.nation_get_active_regiments(owner);
			state.world.nation_set_active_regiments(owner, uint16_t(cur_regiments + uint16_t(num_regs)));
		}
	});
}

void regenerate_land_unit_average(sys::state& state) {
	/*
	We also need to know the average land unit score, which we define here as (attack + defense + national land attack modifier +
	national land defense modifier) x discipline
	*/
	auto const max = state.military_definitions.unit_base_definitions.size();
	state.world.for_each_nation([&](dcon::nation_id n) {
		float total = 0;
		float count = 0;

		auto lo_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_attack_modifier);
		auto ld_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_defense_modifier);

		for(uint32_t i = 2; i < max; ++i) {
			dcon::unit_type_id u{ dcon::unit_type_id::value_base_t(i) };
			if((state.world.nation_get_active_unit(n, u) || state.military_definitions.unit_base_definitions[u].active) && state.military_definitions.unit_base_definitions[u].is_land) {
				auto& reg_stats = state.world.nation_get_unit_stats(n, u);
				total += ((reg_stats.defence_or_hull + ld_mod) + (reg_stats.attack_or_gun_power + lo_mod)) *
					state.military_definitions.unit_base_definitions[u].discipline_or_evasion;
				++count;
			}
		}
		state.world.nation_set_averge_land_unit_score(n, total / count);
	});
}
void regenerate_ship_scores(sys::state& state) {
	/*
	To that we add for each capital ship: (hull points + national-naval-defense-modifier) x (gun power +
	national-naval-attack-modifier) / 250
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

uint32_t naval_supply_from_naval_base(sys::state& state, dcon::province_id prov, dcon::nation_id nation) {
	uint32_t supply = uint32_t(state.defines.naval_base_supply_score_base * (std::pow(2, (dcon::fatten(state.world, prov).get_building_level(uint8_t(economy::province_building_type::naval_base)) - 1))));
	if(dcon::fatten(state.world, prov).get_building_level(uint8_t(economy::province_building_type::naval_base)) != 0) {
		return supply;
	} else {
		for(auto c : dcon::fatten(state.world, prov).get_core()) {
			if(c.get_identity().get_nation_from_identity_holder().id == nation && province::has_access_to_province(state, nation, prov)) {
				return uint32_t(state.defines.naval_base_supply_score_empty);
			}
		}
		if(!province::has_access_to_province(state, nation, prov)) {
			return uint32_t(state.defines.naval_base_supply_score_empty * state.defines.naval_base_non_core_supply_score);
		} else {
			return uint32_t(state.defines.naval_base_supply_score_empty);
		}
	}
}

naval_range_display_data closest_naval_range_port_with_distance(sys::state& state, dcon::province_id prov, dcon::nation_id nation) {
	naval_range_display_data closest{ .closest_port = dcon::province_id{ }, .distance = 9999999.0f, .timestamp = state.current_date };
	if(state.world.nation_get_province_control(nation).begin() != state.world.nation_get_province_control(nation).end()) {
		for(auto p : state.world.nation_get_province_control(nation)) {
			if(auto nb_level = p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)); nb_level > 0) {
				auto dist = province::naval_range_distance(state, p.get_province(), prov, nation);
				if(dist.is_reachable && dist.distance < closest.distance) {
					closest.closest_port = p.get_province();
					closest.distance = dist.distance;
				}
			}
		}
	}
	return closest;
	
}

void update_naval_supply_points(sys::state& state) {
	/*
	- naval supply score: you get define:NAVAL_BASE_SUPPLY_SCORE_BASE x (2 to the power of (its-level - 1)) for each naval base or
	define:NAVAL_BASE_SUPPLY_SCORE_EMPTY for each state without one, multiplied by define:NAVAL_BASE_NON_CORE_SUPPLY_SCORE if it
	is neither a core nor connected to the capital.
	*/
	state.world.for_each_nation([&](dcon::nation_id n) {
		auto cap_region = state.world.province_get_connected_region_id(state.world.nation_get_capital(n));
		float total = 0;
		for(auto si : state.world.nation_get_state_ownership(n)) {
			auto d = state.world.state_instance_get_definition(si.get_state());
			bool saw_coastal = false;
			bool nb_was_core = false;
			bool coast_was_core = false;
			int32_t nb_level = 0;
			for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == n) {
					if(p.get_province().get_is_coast()) {
						saw_coastal = true;
						coast_was_core = coast_was_core || p.get_province().get_is_owner_core();
					}
					nb_level = std::max(nb_level, int32_t(p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base))));
					if(nb_level > 0)
						nb_was_core = p.get_province().get_is_owner_core();
				}
			}
			bool connected = si.get_state().get_capital().get_connected_region_id() == cap_region;

			if(saw_coastal) {
				if(nb_level > 0) {
					if(nb_was_core || connected)
						total += state.defines.naval_base_supply_score_base * float(1 << (nb_level - 1));
					else
						total += state.defines.naval_base_supply_score_base * float(1 << (nb_level - 1)) * state.defines.naval_base_non_core_supply_score;
				} else {
					if(coast_was_core || connected)
						total += state.defines.naval_base_supply_score_empty;
					else
						total += 1.0f;
				}
			}
		}
		state.world.nation_set_naval_supply_points(n, uint16_t(total));
	});

	/*
	- ships consume naval base supply at their supply_consumption_score. Going over the naval supply score comes with various
	penalties (described elsewhere).
	*/
	state.world.for_each_nation([&](dcon::nation_id n) {
		float total = 0;
		for(auto nv : state.world.nation_get_navy_control(n)) {
			for(auto shp : nv.get_navy().get_navy_membership()) {
				total += state.military_definitions.unit_base_definitions[shp.get_ship().get_type()].supply_consumption_score;
			}
		}
		state.world.nation_set_used_naval_supply_points(n, uint16_t(total));
	});
}

float mobilization_size(sys::state const& state, dcon::nation_id n) {
	// Mobilization size = national-modifier-to-mobilization-size + technology-modifier-to-mobilization-size
	return state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_size);
}

ve::fp_vector ve_mobilization_size(sys::state const& state, ve::tagged_vector<dcon::nation_id> nations) {
	// Mobilization size = national-modifier-to-mobilization-size + technology-modifier-to-mobilization-size
	return state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::mobilization_size);
}

float mobilization_impact(sys::state const& state, dcon::nation_id n) {
	// Mobilization impact = 1 - mobilization-size x (national-mobilization-economy-impact-modifier +
	// technology-mobilization-impact-modifier), to a minimum of zero.
	return std::clamp(
		1.0f - mobilization_size(state, n)
		* state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_impact),
		0.0f,
		1.0f
	);
}

ve::fp_vector ve_mobilization_impact(sys::state const& state, ve::tagged_vector<dcon::nation_id> nations) {
	auto size = ve_mobilization_size(state, nations);
	auto impact = (1.f - size) * state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::mobilization_impact);
	return ve::min(ve::max(impact, 0.f), 1.f);
}

// calculates the "combat score" of a ship, mainly to be displayed on the battle progressbar to make more advanced ships count for more
float get_ship_combat_score(sys::state& state, dcon::ship_id ship) {
	auto owner = state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(ship));
	auto& stats = state.world.nation_get_unit_stats(owner, state.world.ship_get_type(ship));

	return state.world.ship_get_strength(ship) * stats.defence_or_hull * stats.attack_or_gun_power * (1 + stats.discipline_or_evasion);
}

void update_cbs(sys::state& state) {
	for(auto n : state.world.in_nation) {
		if(state.current_date == n.get_reparations_until()) {
			for(auto urel : n.get_unilateral_relationship_as_source()) {
				urel.set_reparations(false);
			}
		}
		auto current_cbs = n.get_available_cbs();
		for(uint32_t i = current_cbs.size(); i-- > 0;) {
			if(current_cbs[i].expiration && current_cbs[i].expiration <= state.current_date) {
				current_cbs.remove_at(i);
			}
		}

		// check for cancellation
		if(n.get_constructing_cb_type()) {
			/*
			CBs that become invalid (the nations involved no longer satisfy the conditions or enter into a war with each other)
			are canceled (and the player should be notified in this event).
			*/
			auto target = n.get_constructing_cb_target();
			if(military::are_at_war(state, n, target) || state.world.nation_get_owned_province_count(target) == 0 ||
					!cb_conditions_satisfied(state, n, target, n.get_constructing_cb_type())) {
				if(n == state.local_player_nation) {
					notification::post(state, notification::message{
						[](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_fab_canceled_1");
						},
						"msg_fab_canceled_title",
						n, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::cb_fab_cancelled,
						dcon::province_id{ }
					});
				}

				n.set_constructing_cb_is_discovered(false);
				n.set_constructing_cb_progress(0.0f);
				n.set_constructing_cb_target(dcon::nation_id{});
				n.set_constructing_cb_type(dcon::cb_type_id{});
				n.set_constructing_cb_target_state(dcon::state_definition_id{});
			}
		}

		if(n.get_constructing_cb_type() && !nations::is_involved_in_crisis(state, n)) {
			/*
			CB fabrication by a nation is paused while that nation is in a crisis (nor do events related to CB fabrication
			happen). CB fabrication is advanced by points equal to: define:CB_GENERATION_BASE_SPEED x cb-type-construction-speed x
			(national-cb-construction-speed-modifiers + technology-cb-construction-speed-modifier + 1).
			*/

			auto eff_speed = state.defines.cb_generation_base_speed * n.get_constructing_cb_type().get_construction_speed() * (n.get_modifier_values(sys::national_mod_offsets::cb_generation_speed_modifier) + 1.0f);

			n.set_constructing_cb_progress(n.get_constructing_cb_progress() + std::max(eff_speed, 0.0f));

			/*
			Each day, a fabricating CB has a define:CB_DETECTION_CHANCE_BASE out of 1000 chance to be detected. If discovered, the
			fabricating country gains the infamy for that war goal x the fraction of fabrication remaining. If discovered
			relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states with
			a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED
			*/
			if(!n.get_constructing_cb_is_discovered() && eff_speed > 0.0f) {
				auto val = rng::get_random(state, uint32_t((n.id.index() << 3) + 5)) % 1000;
				if(val <= uint32_t(state.defines.cb_detection_chance_base)) {
					execute_cb_discovery(state, n);
					n.set_constructing_cb_is_discovered(true);
				}
			}

			/*
			When fabrication progress reaches 100, the CB will remain valid for define:CREATED_CB_VALID_TIME months (so x30 days
			for us). Note that pending CBs have their target nation fixed, but all other parameters are flexible.
			*/
			if(n.get_constructing_cb_progress() >= 100.0f) {
				add_cb(state, n, n.get_constructing_cb_type(), n.get_constructing_cb_target(), n.get_constructing_cb_target_state());

				if(n == state.local_player_nation) {
					notification::post(state, notification::message{
						[t = n.get_constructing_cb_target(), c = n.get_constructing_cb_type()](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_fab_finished_1", text::variable_type::x, state.world.cb_type_get_name(c), text::variable_type::y, t);
						},
						"msg_fab_finished_title",
						n, dcon::nation_id{}, dcon::nation_id{},
						sys::message_base_type::cb_fab_finished,
						dcon::province_id{ }
					});
				}

				n.set_constructing_cb_is_discovered(false);
				n.set_constructing_cb_progress(0.0f);
				n.set_constructing_cb_target(dcon::nation_id{});
				n.set_constructing_cb_type(dcon::cb_type_id{});
				n.set_constructing_cb_target_state(dcon::state_definition_id{});
			}
		}
	}
}

void add_cb(sys::state& state, dcon::nation_id n, dcon::cb_type_id cb, dcon::nation_id target, dcon::state_definition_id target_state = dcon::state_definition_id{}) {
	assert(state.world.nation_is_valid(n));
	auto current_cbs = state.world.nation_get_available_cbs(n);
	current_cbs.push_back(military::available_cb{ state.current_date + int32_t(state.defines.created_cb_valid_time) * 30, target, cb, target_state });
}

float cb_infamy_country_modifier(sys::state& state, dcon::nation_id target) {
	float total_population = 0.f;
	auto total_countries = 0;

	auto target_civilized = state.world.nation_get_is_civilized(target);

	for(auto n : state.world.in_nation) {
		/* Use correct reference group for average country population calculation.
		Civs are compared vs civs and uncivs vs uncivs as they have just too big a difference in averages.*/
		auto is_civilized = state.world.nation_get_is_civilized(n);

		if(target_civilized != is_civilized) {
			continue;
		}
		auto n_pop = state.world.nation_get_demographics(n, demographics::total);
		total_population += n_pop;
		total_countries += (n_pop > 0) ? 1 : 0;
	}

	float country_population = state.world.nation_get_demographics(target, demographics::total);

	return std::clamp(math::sqrt(country_population / (total_population / total_countries)), 0.5f, 2.0f);
}

/* New logic for conquest infamy: take `demand state` infamy and multiply it by the proportion of the target nation population to the states average
Basically conquest costs the same as conquering every state of the country */
float cb_infamy_conquest_modifier(sys::state& state, dcon::nation_id target) {
	if(target == dcon::nation_id{}) {
		assert(false && "This shouldn't happen");
		return 10.0f; // Default cap for infamy price
	}

	float country_population = state.world.nation_get_demographics(target, demographics::total);

	float total_population = 0.f;
	auto total_states = 0;

	auto target_civilized = state.world.nation_get_is_civilized(target);

	for(auto s : state.world.in_state_instance) {
		/* Use correct reference group for average country population calculation.
		Civs are compared vs civs and uncivs vs uncivs as they have just too big a difference in averages.*/
		auto n = state.world.state_instance_get_nation_from_state_ownership(s);
		auto is_civilized = state.world.nation_get_is_civilized(n);

		if(target_civilized != is_civilized) {
			continue;
		}
		auto s_pop = state.world.state_instance_get_demographics(s, demographics::total);
		total_population += s_pop;
		total_states += (s_pop > 0) ? 1 : 0;
	}

	return std::clamp(math::sqrt(country_population / (total_population / total_states)), 0.5f, 10.0f);
}

float cb_infamy_state_modifier(sys::state& state, dcon::nation_id target, dcon::state_definition_id cb_state) {
	if(cb_state == dcon::state_definition_id{}) {
		// assert(false && "This shouldn't happen");
		return 2.0f; // Default cap for infamy price
	}

	float total_population = 0.f;
	auto total_states = 0;
	float state_population = 0;

	auto target_civilized = state.world.nation_get_is_civilized(target);

	for(auto s : state.world.in_state_instance) {
		/* Use correct reference group for average state population calculation.
		Civs are compared vs civs and uncivs vs uncivs as they have just too big a difference in averages.*/
		auto n = state.world.state_instance_get_nation_from_state_ownership(s);
		auto is_civilized = state.world.nation_get_is_civilized(n);

		if(target_civilized != is_civilized) {
			continue;
		}

		auto s_pop = s.get_demographics(demographics::total);
		total_population += s_pop;
		total_states += (s_pop > 0) ? 1 : 0;

		if(s.get_definition() == cb_state && s.get_nation_from_state_ownership() == target) {
			state_population = s_pop;
		}
	}

	return std::clamp(math::sqrt(state_population / (total_population / total_states)), 0.5f, 2.0f);
}

float cb_infamy(sys::state& state, dcon::cb_type_id t, dcon::nation_id target, dcon::state_definition_id cb_state) {
	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(t);

	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += state.defines.infamy_prestige;
	}
	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		total += state.defines.infamy_clear_union_sphere * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_gunboat) != 0) {
		total += state.defines.infamy_gunboat * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += state.defines.infamy_demand_state * cb_infamy_conquest_modifier(state, target);
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += state.defines.infamy_make_puppet * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_make_substate) != 0) {
		total += state.defines.infamy_make_substate * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += state.defines.infamy_release_puppet * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += state.defines.infamy_status_quo;
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += state.defines.infamy_install_communist_gov_type * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += state.defines.infamy_uninstall_communist_gov_type * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += state.defines.infamy_remove_cores * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += state.defines.infamy_colony * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += state.defines.infamy_destroy_forts * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total += state.defines.infamy_destroy_naval_bases * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += state.defines.infamy_add_to_sphere * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += state.defines.infamy_disarmament * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += state.defines.infamy_reparations * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_unequal_treaty) != 0) {
		total += state.defines.infamy_unequal_treaty * cb_infamy_country_modifier(state, target);
	}

	auto infamy_state_mod = 1.0f;
	if(military::cb_requires_selection_of_a_state(state, t)) {
		infamy_state_mod = cb_infamy_state_modifier(state, target, cb_state);
	}

	if((bits & cb_flag::po_demand_state) != 0) {
		total += state.defines.infamy_demand_state * infamy_state_mod;
	}
	if((bits & cb_flag::po_transfer_provinces) != 0) {
		total += state.defines.infamy_transfer_provinces * infamy_state_mod;
	}

	return total * state.world.cb_type_get_badboy_factor(t);
}

float truce_break_cb_prestige_cost(sys::state& state, dcon::cb_type_id t) {
	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(t);

	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		total += state.defines.breaktruce_prestige_clear_union_sphere;
	}
	if((bits & cb_flag::po_gunboat) != 0) {
		total += state.defines.breaktruce_prestige_gunboat;
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += state.defines.breaktruce_prestige_annex;
	}
	if((bits & cb_flag::po_demand_state) != 0) {
		total += state.defines.breaktruce_prestige_demand_state;
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += state.defines.breaktruce_prestige_add_to_sphere;
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += state.defines.breaktruce_prestige_disarmament;
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += state.defines.breaktruce_prestige_reparations;
	}
	if((bits & cb_flag::po_transfer_provinces) != 0) {
		total += state.defines.breaktruce_prestige_transfer_provinces;
	}
	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += state.defines.breaktruce_prestige_prestige;
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += state.defines.breaktruce_prestige_make_puppet;
	}
	if((bits & cb_flag::po_make_substate) != 0) {
		total += state.defines.breaktruce_prestige_make_substate;
	}
	if((bits & cb_flag::po_unequal_treaty) != 0) {
		total += state.defines.breaktruce_prestige_unequal_treaty;
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += state.defines.breaktruce_prestige_release_puppet;
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += state.defines.breaktruce_prestige_status_quo;
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += state.defines.breaktruce_prestige_install_communist_gov_type;
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += state.defines.breaktruce_prestige_uninstall_communist_gov_type;
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += state.defines.breaktruce_prestige_remove_cores;
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += state.defines.breaktruce_prestige_colony;
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += state.defines.breaktruce_prestige_destroy_forts;
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total += state.defines.breaktruce_prestige_destroy_naval_bases;
	}

	return total * state.world.cb_type_get_break_truce_prestige_factor(t);
}
float truce_break_cb_militancy(sys::state& state, dcon::cb_type_id t) {
	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(t);

	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		total += state.defines.breaktruce_militancy_clear_union_sphere;
	}
	if((bits & cb_flag::po_gunboat) != 0) {
		total += state.defines.breaktruce_militancy_gunboat;
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += state.defines.breaktruce_militancy_annex;
	}
	if((bits & cb_flag::po_demand_state) != 0) {
		total += state.defines.breaktruce_militancy_demand_state;
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += state.defines.breaktruce_militancy_add_to_sphere;
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += state.defines.breaktruce_militancy_disarmament;
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += state.defines.breaktruce_militancy_reparations;
	}
	if((bits & cb_flag::po_transfer_provinces) != 0) {
		total += state.defines.breaktruce_militancy_transfer_provinces;
	}
	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += state.defines.breaktruce_militancy_prestige;
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += state.defines.breaktruce_militancy_make_puppet;
	}
	if((bits & cb_flag::po_make_substate) != 0) {
		total += state.defines.breaktruce_militancy_make_substate;
	}
	if((bits & cb_flag::po_unequal_treaty) != 0) {
		total += state.defines.breaktruce_militancy_unequal_treaty;
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += state.defines.breaktruce_militancy_release_puppet;
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += state.defines.breaktruce_militancy_status_quo;
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += state.defines.breaktruce_militancy_install_communist_gov_type;
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += state.defines.breaktruce_militancy_uninstall_communist_gov_type;
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += state.defines.breaktruce_militancy_remove_cores;
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += state.defines.breaktruce_militancy_colony;
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += state.defines.breaktruce_militancy_destroy_forts;
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total += state.defines.breaktruce_militancy_destroy_naval_bases;
	}

	return total * state.world.cb_type_get_break_truce_militancy_factor(t);
}
float truce_break_cb_infamy(sys::state& state, dcon::cb_type_id t, dcon::nation_id target, dcon::state_definition_id cb_state) {
	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(t);

	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		total += state.defines.breaktruce_infamy_clear_union_sphere * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_gunboat) != 0) {
		total += state.defines.breaktruce_infamy_gunboat * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += state.defines.breaktruce_infamy_annex * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_demand_state) != 0) {
		total += state.defines.breaktruce_infamy_demand_state * cb_infamy_state_modifier(state, target, cb_state);
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += state.defines.breaktruce_infamy_add_to_sphere * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += state.defines.breaktruce_infamy_disarmament * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += state.defines.breaktruce_infamy_reparations * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_transfer_provinces) != 0) {
		total += state.defines.breaktruce_infamy_transfer_provinces * cb_infamy_state_modifier(state, target, cb_state);
	}
	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += state.defines.breaktruce_infamy_prestige;
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += state.defines.breaktruce_infamy_make_puppet * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_make_substate) != 0) {
		total += state.defines.breaktruce_infamy_make_substate * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_unequal_treaty) != 0) {
		total += state.defines.breaktruce_infamy_unequal_treaty * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += state.defines.breaktruce_infamy_release_puppet * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += state.defines.breaktruce_infamy_status_quo;
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += state.defines.breaktruce_infamy_install_communist_gov_type * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += state.defines.breaktruce_infamy_uninstall_communist_gov_type * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += state.defines.breaktruce_infamy_remove_cores * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += state.defines.breaktruce_infamy_colony * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += state.defines.breaktruce_infamy_destroy_forts * cb_infamy_country_modifier(state, target);
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total += state.defines.breaktruce_infamy_destroy_naval_bases * cb_infamy_country_modifier(state, target);
	}

	return total * state.world.cb_type_get_break_truce_infamy_factor(t);
}

// Calculate victory points that a province P is worth in the nation N. Used for warscore, occupation rate.
// This logic is duplicated in province_victory_points_text UI component
int32_t province_point_cost(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	/*
	All provinces have a base value of 1.
	For non colonial provinces: each level of naval base increases its value by 1.
	Every fort level increases its value by 1.
	If it is a state capital, its value increases by 1 for every factory in the state (factory level does not matter).
	This value is the doubled for non-overseas provinces where the owner has a core.
	It is then tripled for the nation's capital province.
	*/
	int32_t total = 1;
	if(!state.world.province_get_is_colonial(p)) {
		total += state.world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base));
	}
	auto fac_range = state.world.province_get_factory_location(p);
	total += int32_t(fac_range.end() - fac_range.begin());
	total += state.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort));

	auto owner_cap = state.world.nation_get_capital(n);
	auto overseas = (state.world.province_get_continent(p) != state.world.province_get_continent(owner_cap)) &&
		(state.world.province_get_connected_region_id(p) != state.world.province_get_connected_region_id(owner_cap));

	if(state.world.province_get_is_owner_core(p) && !overseas) {
		total *= 2;
	}
	if(state.world.nation_get_capital(n) == p) {
		total *= 3;
	}
	return total;
}

int32_t peace_cost(sys::state& state, dcon::war_id war, dcon::cb_type_id wargoal, dcon::nation_id from, dcon::nation_id target,
		dcon::nation_id secondary_nation, dcon::state_definition_id wargoal_state, dcon::national_identity_id wargoal_tag) {

	/*
	Each war goal has a value that determines how much it is worth in a peace offer (and peace offers are capped at 100 points of
	war goals). Great war obligatory war goals cost 0. Then we iterate over the po tags and sum up the peace cost for each. Some
	tags have a fixed cost in the defines, such as define:PEACE_COST_RELEASE_PUPPET. For anything that conquers provinces directly
	(ex: demand state), each province is worth its value relative to the total cost of provinces owned by the target (see below)
	x 2.8. For po_clear_union_sphere, the cost is defines:PEACE_COST_CLEAR_UNION_SPHERE x the number of nations that will be
	affected. If the war is a great war, this cost is multiplied by defines:GW_WARSCORE_COST_MOD. If it is a great war, world wars
	are enabled, and the war score is at least defines:GW_WARSCORE_2_THRESHOLD, the cost is multiplied by
	defines:GW_WARSCORE_COST_MOD_2 instead. The peace cost of a single war goal is capped at 100.0.
	*/

	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(wargoal);

	if((bits & cb_flag::great_war_obligatory) != 0) {
		return 0;
	}

	if((bits & cb_flag::po_gunboat) != 0) {
		total += state.defines.peace_cost_gunboat;
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += state.defines.peace_cost_annex;
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += state.defines.peace_cost_add_to_sphere;
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += state.defines.peace_cost_disarmament;
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += state.defines.peace_cost_reparations;
	}
	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += state.defines.peace_cost_prestige;
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += state.defines.peace_cost_make_puppet;
	}
	if((bits & cb_flag::po_make_substate) != 0) {
		total += state.defines.peace_cost_make_substate;
	}
	if((bits & cb_flag::po_unequal_treaty) != 0) {
		total += state.defines.peace_cost_unequal_treaty;
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += state.defines.peace_cost_release_puppet;
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += state.defines.peace_cost_status_quo;
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += state.defines.peace_cost_install_communist_gov_type;
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += state.defines.peace_cost_uninstall_communist_gov_type;
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += state.defines.peace_cost_remove_cores;
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += state.defines.peace_cost_colony;
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += state.defines.peace_cost_destroy_forts;
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total += state.defines.peace_cost_destroy_naval_bases;
	}

	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		auto from_group = state.world.nation_get_primary_culture(from).get_group_from_culture_group_membership();
		for(auto target_sphere : state.world.nation_get_gp_relationship_as_great_power(target)) {
			if((target_sphere.get_status() & nations::influence::level_mask) == nations::influence::level_in_sphere &&
					target_sphere.get_influence_target().get_primary_culture().get_group_from_culture_group_membership() == from_group) {

				total += state.defines.peace_cost_clear_union_sphere;
			}
		}
	}
	if((bits & (cb_flag::po_transfer_provinces | cb_flag::po_demand_state)) != 0) {
		int32_t sum_target_prov_values = 0;
		for(auto prv : state.world.nation_get_province_ownership(target)) {
			sum_target_prov_values += province_point_cost(state, prv.get_province(), target);
		}
		auto secondary = secondary_nation ? secondary_nation : state.world.national_identity_get_nation_from_identity_holder(wargoal_tag);
		bool is_lib = (bits & cb_flag::po_transfer_provinces) != 0;

		if(sum_target_prov_values > 0) {
			if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states && (bits & cb_flag::all_allowed_states) != 0) {
				for(auto si : state.world.nation_get_state_ownership(target)) {
					if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
						is_lib ? trigger::to_generic(secondary) : trigger::to_generic(from))) {

						province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id tprov) {
							total += 280.0f * float(province_point_cost(state, tprov, target)) / float(sum_target_prov_values);
						});
					}
				}
			} else {
				for(auto tprov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
					if(tprov.get_province().get_nation_from_province_ownership() == target)
						total += 280.0f * float(province_point_cost(state, tprov.get_province(), target)) / float(sum_target_prov_values);
				}
			}
		}
	}

	if(state.world.war_get_is_great(war)) {
		bool is_attacker = false;
		for(auto par : state.world.war_get_war_participant(war)) {
			if(par.get_nation() == from) {
				is_attacker = par.get_is_attacker();
				break;
			}
		}
		if(state.military_definitions.world_wars_enabled &&
				((is_attacker && primary_warscore(state, war) >= state.defines.gw_warscore_2_threshold) ||
					(!is_attacker && primary_warscore(state, war) <= -state.defines.gw_warscore_2_threshold))) {
			total *= state.defines.gw_warscore_cost_mod_2;
		} else {
			total *= state.defines.gw_warscore_cost_mod;
		}
	}

	return std::min(int32_t(total * state.world.cb_type_get_peace_cost_factor(wargoal)), 100);
}

int32_t cost_of_peace_offer(sys::state& state, dcon::peace_offer_id offer) {
	auto war = state.world.peace_offer_get_war_from_war_settlement(offer);
	int32_t total = 0;
	for(auto wg : state.world.peace_offer_get_peace_offer_item(offer)) {
		total += peace_cost(state, war, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(),
				wg.get_wargoal().get_target_nation(), wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(),
				wg.get_wargoal().get_associated_tag());
	}
	return total;
}
int32_t peace_offer_truce_months(sys::state& state, dcon::peace_offer_id offer) {
	int32_t max_months = 0;
	for(auto wg : state.world.peace_offer_get_peace_offer_item(offer)) {
		max_months = std::max(max_months, int32_t(wg.get_wargoal().get_type().get_truce_months()));
	}
	return max_months + int32_t(state.defines.base_truce_months);
}
int32_t attacker_peace_cost(sys::state& state, dcon::war_id war) {
	int32_t total = 0;
	for(auto wg : state.world.war_get_wargoals_attached(war)) {
		if(is_attacker(state, war, wg.get_wargoal().get_added_by())) {
			total += peace_cost(state, war, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(),
					wg.get_wargoal().get_target_nation(), wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(),
					wg.get_wargoal().get_associated_tag());
		}
	}
	return total;
}
int32_t defender_peace_cost(sys::state& state, dcon::war_id war) {
	int32_t total = 0;
	for(auto wg : state.world.war_get_wargoals_attached(war)) {
		if(!is_attacker(state, war, wg.get_wargoal().get_added_by())) {
			total += peace_cost(state, war, wg.get_wargoal().get_type(), wg.get_wargoal().get_added_by(),
					wg.get_wargoal().get_target_nation(), wg.get_wargoal().get_secondary_nation(), wg.get_wargoal().get_associated_state(),
					wg.get_wargoal().get_associated_tag());
		}
	}
	return total;
}

float successful_cb_prestige(sys::state& state, dcon::cb_type_id t, dcon::nation_id actor) {
	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(t);
	float actor_prestige = nations::prestige_score(state, actor);

	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		total += std::max(state.defines.prestige_clear_union_sphere * actor_prestige, state.defines.prestige_clear_union_sphere_base);
	}
	if((bits & cb_flag::po_gunboat) != 0) {
		total += std::max(state.defines.prestige_gunboat * actor_prestige, state.defines.prestige_gunboat_base);
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += std::max(state.defines.prestige_annex * actor_prestige, state.defines.prestige_annex_base);
	}
	if((bits & cb_flag::po_demand_state) != 0) {
		total += std::max(state.defines.prestige_demand_state * actor_prestige, state.defines.prestige_demand_state_base);
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += std::max(state.defines.prestige_add_to_sphere * actor_prestige, state.defines.prestige_add_to_sphere_base);
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += std::max(state.defines.prestige_disarmament * actor_prestige, state.defines.prestige_disarmament_base);
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += std::max(state.defines.prestige_reparations * actor_prestige, state.defines.prestige_reparations_base);
	}
	if((bits & cb_flag::po_transfer_provinces) != 0) {
		total += std::max(state.defines.prestige_transfer_provinces * actor_prestige, state.defines.prestige_transfer_provinces_base);
	}
	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += std::max(state.defines.prestige_prestige * actor_prestige, state.defines.prestige_prestige_base);
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += std::max(state.defines.prestige_make_puppet * actor_prestige, state.defines.prestige_make_puppet_base);
	}
	if((bits & cb_flag::po_make_substate) != 0) {
		total += std::max(state.defines.prestige_make_substate * actor_prestige, state.defines.prestige_make_substate);
	}
	if((bits & cb_flag::po_unequal_treaty) != 0) {
		total += std::max(state.defines.prestige_unequal_treaty * actor_prestige, state.defines.prestige_unequal_treaty);
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += std::max(state.defines.prestige_release_puppet * actor_prestige, state.defines.prestige_release_puppet_base);
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += std::max(state.defines.prestige_status_quo * actor_prestige, state.defines.prestige_status_quo_base);
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += std::max(state.defines.prestige_install_communist_gov_type * actor_prestige,
				state.defines.prestige_install_communist_gov_type_base);
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += std::max(state.defines.prestige_uninstall_communist_gov_type * actor_prestige,
				state.defines.prestige_uninstall_communist_gov_type_base);
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += std::max(state.defines.prestige_remove_cores * actor_prestige, state.defines.prestige_remove_cores_base);
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += std::max(state.defines.prestige_colony * actor_prestige, state.defines.prestige_colony_base);
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += std::max(state.defines.prestige_destroy_forts * actor_prestige, state.defines.prestige_destroy_forts_base);
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total +=
			std::max(state.defines.prestige_destroy_naval_bases * actor_prestige, state.defines.prestige_destroy_naval_bases_base);
	}

	return total * state.world.cb_type_get_prestige_factor(t);
}

float crisis_cb_addition_infamy_cost(sys::state& state, dcon::cb_type_id type, dcon::nation_id from, dcon::nation_id target, dcon::state_definition_id cb_state) {
	if((state.world.cb_type_get_type_bits(type) & (military::cb_flag::is_not_constructing_cb)) != 0) {
		// not a constructible CB
		return 0.0f;
	}

	// Always available CBs cost zero infamy
	if((state.world.cb_type_get_type_bits(type) & military::cb_flag::always) != 0 && state.defines.alice_always_available_cbs_zero_infamy != 0.f) {
		return 0.0f;
	}

	return cb_infamy(state, type, target, cb_state) * state.defines.crisis_wargoal_infamy_mult;
}
float cb_addition_infamy_cost(sys::state& state, dcon::war_id war, dcon::cb_type_id type, dcon::nation_id from,
		dcon::nation_id target, dcon::state_definition_id cb_state) {
	if((state.world.cb_type_get_type_bits(type) & (military::cb_flag::is_not_constructing_cb)) != 0) {
		// not a constructible CB
		return 0.0f;
	}

	// Always available CBs cost zero infamy
	if((state.world.cb_type_get_type_bits(type) & military::cb_flag::always) != 0 && state.defines.alice_always_available_cbs_zero_infamy != 0.f) {
		return 0.0f;
	}

	auto other_cbs = state.world.nation_get_available_cbs(from);
	for(auto& cb : other_cbs) {
		if(cb.target == target && cb.cb_type == type && cb_conditions_satisfied(state, from, target, cb.cb_type))
			return 0.0f;
	}

	if(state.world.war_get_is_great(war))
		return cb_infamy(state, type, target, cb_state) * state.defines.gw_justify_cb_badboy_impact;
	else
		return cb_infamy(state, type, target, cb_state);
}

float war_declaration_infamy_cost(sys::state& state, dcon::cb_type_id type, dcon::nation_id from,
		dcon::nation_id target, dcon::state_definition_id cb_state) {
	if((state.world.cb_type_get_type_bits(type) & (military::cb_flag::is_not_constructing_cb)) != 0) {
		// not a constructible CB
		return 0.0f;
	}

	// Always available CBs cost zero infamy
	if((state.world.cb_type_get_type_bits(type) & military::cb_flag::always) != 0 && state.defines.alice_always_available_cbs_zero_infamy != 0.f) {
		return 0.0f;
	}

	auto other_cbs = state.world.nation_get_available_cbs(from);
	for(auto& cb : other_cbs) {
		if(cb.target == target && cb.cb_type == type && cb_conditions_satisfied(state, from, target, cb.cb_type))
			return 0.0f;
	}

	return cb_infamy(state, type, target, cb_state);
}

bool cb_requires_selection_of_a_valid_nation(sys::state const& state, dcon::cb_type_id t) {
	auto allowed_nation = state.world.cb_type_get_allowed_countries(t);
	return bool(allowed_nation);
}
bool cb_requires_selection_of_a_liberatable_tag(sys::state const& state, dcon::cb_type_id t) {
	auto bits = state.world.cb_type_get_type_bits(t);
	return (bits & (cb_flag::po_transfer_provinces)) != 0;
}
bool cb_requires_selection_of_a_state(sys::state const& state, dcon::cb_type_id t) {
	auto bits = state.world.cb_type_get_type_bits(t);
	return (bits & (cb_flag::po_demand_state | cb_flag::po_transfer_provinces | cb_flag::po_destroy_naval_bases | cb_flag::po_destroy_forts)) != 0
		&& (bits & cb_flag::all_allowed_states) == 0;
}

void execute_cb_discovery(sys::state& state, dcon::nation_id n) {
	/*
	 If discovered, the fabricating country gains the infamy for that war goal x the fraction of fabrication remaining. If
	 discovered relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states
	 with a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED
	*/
	auto infamy = cb_infamy(state, state.world.nation_get_constructing_cb_type(n), state.world.nation_get_constructing_cb_target(n), state.world.nation_get_constructing_cb_target_state(n));
	auto adj_infamy = std::max(0.0f, ((100.0f - state.world.nation_get_constructing_cb_progress(n)) / 100.0f) * infamy);
	state.world.nation_set_infamy(n, state.world.nation_get_infamy(n) + adj_infamy);

	auto target = state.world.nation_get_constructing_cb_target(n);

	nations::adjust_relationship(state, n, target, state.defines.on_cb_detected_relation_change);

	for(auto si : state.world.nation_get_state_ownership(target)) {
		if(si.get_state().get_flashpoint_tag()) {
			si.get_state().set_flashpoint_tension(
					std::min(100.0f, si.get_state().get_flashpoint_tension() + state.defines.tension_on_cb_discovered));
		}
	}

	notification::post(state, notification::message{
		[n, target, adj_infamy](sys::state& state, text::layout_base& contents) {
			if(n == state.local_player_nation) {
				text::add_line(state, contents, "msg_fab_discovered_1", text::variable_type::x, text::fp_one_place{ adj_infamy });
			} else {
				text::add_line(state, contents, "msg_fab_discovered_2", text::variable_type::x, n, text::variable_type::y, target);
			}
		},
		"msg_fab_discovered_title",
		n, target, dcon::nation_id{},
		sys::message_base_type::cb_detected,
		dcon::province_id{ }
	});
}

bool leader_is_in_combat(sys::state& state, dcon::leader_id l) {
	auto army = state.world.leader_get_army_from_army_leadership(l);
	if(state.world.army_get_battle_from_army_battle_participation(army))
		return true;
	auto navy = state.world.leader_get_navy_from_navy_leadership(l);
	if(state.world.navy_get_battle_from_navy_battle_participation(navy))
		return true;
	return false;
}

dcon::leader_id make_new_leader(sys::state& state, dcon::nation_id n, bool is_general) {
	auto l = fatten(state.world, state.world.create_leader());
	l.set_is_admiral(!is_general);

	uint32_t seed_base = (uint32_t(n.index()) << 6) ^ uint32_t(l.id.index());

	auto num_personalities = uint32_t(state.military_definitions.first_background_trait.index() - 1);
	auto num_backgrounds = uint32_t((state.world.leader_trait_size() - num_personalities) - 2);

	auto trait_pair = rng::get_random_pair(state, seed_base);

	l.set_personality(dcon::leader_trait_id{ dcon::leader_trait_id::value_base_t(1 + rng::reduce(uint32_t(trait_pair.high), num_personalities)) });
	l.set_background(dcon::leader_trait_id{ dcon::leader_trait_id::value_base_t(state.military_definitions.first_background_trait.index() + 1 + rng::reduce(uint32_t(trait_pair.low), num_backgrounds)) });

	auto names_pair = rng::get_random_pair(state, seed_base + 1);

	auto names = state.world.culture_get_last_names(state.world.nation_get_primary_culture(n));
	if(names.size() > 0) {
		l.set_name(names.at(rng::reduce(uint32_t(names_pair.high), names.size())));
	}

	l.set_since(state.current_date);

	/* defines:LEADER_MAX_RANDOM_PRESTIGE */
	float r_factor = float(rng::reduce(uint32_t(rng::get_random(state, seed_base + 1)), 100)) / 100.f;
	l.set_prestige(r_factor * state.defines.leader_max_random_prestige);

	state.world.try_create_leader_loyalty(n, l);

	state.world.nation_get_leadership_points(n) -= state.defines.leader_recruit_cost;

	return l;
}

void kill_leader(sys::state& state, dcon::leader_id l) {
	/*
	the player only gets leader death messages if the leader is currently assigned to an army or navy (assuming the message
	setting for it is turned on).
	*/
	if(state.world.leader_get_nation_from_leader_loyalty(l) == state.local_player_nation) {
		if(state.world.leader_get_army_from_army_leadership(l) || state.world.leader_get_navy_from_navy_leadership(l)) {
			dcon::nation_id n = state.local_player_nation;

			auto is_admiral = state.world.leader_get_is_admiral(l);
			auto location = is_admiral ? state.world.navy_get_location_from_navy_location(state.world.leader_get_navy_from_navy_leadership(l)) : state.world.army_get_location_from_army_location(state.world.leader_get_army_from_army_leadership(l));
			auto name = state.world.leader_get_name(l);

			notification::post(state, notification::message{
				[is_admiral, location, name](sys::state& state, text::layout_base& contents) {
					if(is_admiral)
						text::add_line(state, contents, "msg_leader_died_2", text::variable_type::x, state.to_string_view(name), text::variable_type::y, location);
					else
						text::add_line(state, contents, "msg_leader_died_1", text::variable_type::x, state.to_string_view(name), text::variable_type::y, location);
				},
				"msg_leader_died_title",
				n, dcon::nation_id{}, dcon::nation_id{},
				sys::message_base_type::leader_dies,
				dcon::province_id{ }
			});
		}
	}

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

float calculate_monthly_leadership_points(sys::state& state, dcon::nation_id n) {
	if(state.world.nation_get_owned_province_count(n) == 0) {
		return 0.f;
	}
	// % of officers in the population
	auto optimum_officers = state.world.pop_type_get_research_optimum(state.culture_definitions.officers);
	auto ofrac = state.world.nation_get_demographics(n, demographics::to_key(state, state.culture_definitions.officers)) / state.world.nation_get_demographics(n, demographics::total);
	// How much leadership these officers generate PRE national modifiers
	// officer_leadership_points is `leadership = 3` from poptypes/officers.txt
	auto omod = std::min(1.0f, ofrac / optimum_officers) * float(state.culture_definitions.officer_leadership_points) / state.defines.alice_leadership_generation_divisor;
	// Calculate national modifiers
	auto nmod = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::leadership_modifier) + 1.0f) *
		state.world.nation_get_modifier_values(n, sys::national_mod_offsets::leadership);

	return state.world.nation_get_leadership_points(n) + omod + nmod;
}

void monthly_leaders_update(sys::state& state) {
	/*
	- A nation gets ((number-of-officers / total-population) / officer-optimum)^1 x officer-leadership-amount +
	national-modifier-to-leadership x (national-modifier-to-leadership-modifier + 1) leadership points per month.
	*/

	state.world.execute_serial_over_nation(
			[&, optimum_officers = state.world.pop_type_get_research_optimum(state.culture_definitions.officers)](auto ids) {
				// % of officers in the population
				auto ofrac = state.world.nation_get_demographics(ids, demographics::to_key(state, state.culture_definitions.officers)) /
					ve::max(state.world.nation_get_demographics(ids, demographics::total), 1.0f);
				// How much leadership these officers generate PRE national modifiers
				// officer_leadership_points is `leadership = 3` from poptypes/officers.txt
				auto omod = ve::min(1.0f, ofrac / optimum_officers) * float(state.culture_definitions.officer_leadership_points) / state.defines.alice_leadership_generation_divisor;
				// Calculate national modifiers
				auto nmod = (state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::leadership_modifier) + 1.0f) *
					state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::leadership);

				state.world.nation_set_leadership_points(ids, ve::select(state.world.nation_get_owned_province_count(ids) != 0, state.world.nation_get_leadership_points(ids) + omod + nmod, 0.0f));
			});

	for(auto n : state.world.in_nation) {
		if(n.get_leadership_points() > state.defines.leader_recruit_cost * 3.0f && state.defines.alice_auto_hire_generals > 0.f) {
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
					auto too_many_generals =
						(existing_leaders.admirals > 0 && navy_count > 0)
						? float(existing_leaders.generals + 1) / float(existing_leaders.admirals + 1) > float(army_count + 1) / float(navy_count + 1)
						: false;
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
	Leaders who are both less than 26 years old and not in combat have no chance of death. Otherwise, we take the age of the
	leader and divide by define:LEADER_AGE_DEATH_FACTOR. Then we multiply that result by 2 if the leader is currently in combat.
	That is then the leader's current chance of death out of ... my notes say 11,000 here.
	*/

	for(uint32_t i = state.world.leader_size(); i-- > 0;) {
		dcon::leader_id l{ dcon::leader_id::value_base_t(i) };
		if(!state.world.leader_is_valid(l))
			continue;

		auto age_in_days = state.current_date.to_raw_value() - state.world.leader_get_since(l).to_raw_value();
		if(age_in_days > 365 * 26) { // assume leaders are created at age 20; no death chance prior to 46
			float age_in_years = float(age_in_days) / 365.0f;
			float death_chance =
				(age_in_years * (leader_is_in_combat(state, l) ? 2.0f : 1.0f) / state.defines.leader_age_death_factor) / 11000.0f;

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

bool has_truce_with(sys::state& state, dcon::nation_id attacker, dcon::nation_id target) {
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(target, attacker);
	if(rel) {
		auto truce_ends = state.world.diplomatic_relation_get_truce_until(rel);
		if(truce_ends && state.current_date < truce_ends)
			return true;
	}
	return false;
}

sys::date truce_end_date(sys::state& state, dcon::nation_id attacker, dcon::nation_id target) {
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(target, attacker);
	if(rel) {
		auto truce_ends = state.world.diplomatic_relation_get_truce_until(rel);
		if(truce_ends)
			return truce_ends;
	}
	return sys::date{};
}

dcon::regiment_id create_new_regiment(sys::state& state, dcon::nation_id n, dcon::unit_type_id t) {
	auto reg = fatten(state.world, state.world.create_regiment());
	reg.set_type(t);
	// TODO make name
	auto exp = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_unit_start_experience) / 100.f;
	exp += state.world.nation_get_modifier_values(n, sys::national_mod_offsets::regular_experience_level) / 100.f;
	reg.set_experience(std::clamp(exp, 0.f, 1.f));
	reg.set_strength(1.f);
	reg.set_org(1.f);
	return reg.id;
}
dcon::ship_id create_new_ship(sys::state& state, dcon::nation_id n, dcon::unit_type_id t) {
	auto shp = fatten(state.world, state.world.create_ship());
	shp.set_type(t);
	// TODO make name
	auto exp = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::naval_unit_start_experience) / 100.f;
	exp += state.world.nation_get_modifier_values(n, sys::national_mod_offsets::regular_experience_level) / 100.f;
	shp.set_experience(std::clamp(exp, 0.f, 1.f));
	shp.set_strength(1.f);
	shp.set_org(1.f);
	return shp.id;
}

dcon::nation_id get_effective_unit_commander(sys::state& state, dcon::army_id unit) {
	auto army_controller = state.world.army_get_controller_from_army_control(unit);
	auto potential_overlord = state.world.nation_get_overlord_as_subject(army_controller);
	if(bool(potential_overlord) && state.world.nation_get_overlord_commanding_units(army_controller)) {
		return state.world.overlord_get_ruler(potential_overlord);
	}
	return army_controller;
}

dcon::nation_id get_effective_unit_commander(sys::state& state, dcon::navy_id unit) {
	auto navy_controller = state.world.navy_get_controller_from_navy_control(unit);
	auto potential_overlord = state.world.nation_get_overlord_as_subject(navy_controller);
	if(bool(potential_overlord) && state.world.nation_get_overlord_commanding_units(navy_controller)) {
		return state.world.overlord_get_ruler(potential_overlord);
	}
	return navy_controller;
}

void give_military_access(sys::state& state, dcon::nation_id accessing_nation, dcon::nation_id target) {
	auto ur = state.world.get_unilateral_relationship_by_unilateral_pair(target, accessing_nation);
	if(!ur) {
		ur = state.world.force_create_unilateral_relationship(target, accessing_nation);
	}
	state.world.unilateral_relationship_set_military_access(ur, true);
}
void remove_military_access(sys::state& state, dcon::nation_id accessing_nation, dcon::nation_id target) {
	auto ur = state.world.get_unilateral_relationship_by_unilateral_pair(target, accessing_nation);
	if(ur) {
		state.world.unilateral_relationship_set_military_access(ur, false);
	}
}

void end_wars_between(sys::state& state, dcon::nation_id a, dcon::nation_id b) {
	dcon::war_id w = find_war_between(state, a, b);
	while(w) {
		military::remove_from_war(state, w, b, false);
		// cleanup_war(state, w, war_result::draw);
		w = find_war_between(state, a, b);
	}
}

void populate_war_text_subsitutions(sys::state& state, dcon::war_id w, text::substitution_map& sub) {
	auto war = fatten(state.world, w);

	dcon::nation_id primary_attacker = state.world.war_get_original_attacker(war);
	dcon::nation_id primary_defender = state.world.war_get_original_target(war);

	text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));

	text::add_to_substitution_map(sub, text::variable_type::second, text::get_adjective(state, primary_defender));
	text::add_to_substitution_map(sub, text::variable_type::second_country, primary_defender);
	text::add_to_substitution_map(sub, text::variable_type::first, text::get_adjective(state, primary_attacker));
	text::add_to_substitution_map(sub, text::variable_type::third, war.get_over_tag());
	text::add_to_substitution_map(sub, text::variable_type::state, war.get_over_state());
	text::add_to_substitution_map(sub, text::variable_type::country_adj, state.world.national_identity_get_adjective(war.get_over_tag()));
}

void add_to_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_attacker, bool on_war_creation) {
	assert(n);
	if(state.world.nation_get_owned_province_count(n) == 0)
		return;

	state.trade_route_cached_values_out_of_date = true;

	auto participant = state.world.force_create_war_participant(w, n);
	state.world.war_participant_set_is_attacker(participant, as_attacker);
	state.world.nation_set_is_at_war(n, true);
	state.world.nation_set_disarmed_until(n, sys::date{});

	for(auto dep : state.world.nation_get_overlord_as_ruler(n)) {
		add_to_war(state, w, dep.get_subject(), as_attacker);
	}

	for(auto wp : state.world.war_get_war_participant(w)) {
		if(wp.get_is_attacker() == !as_attacker && nations::are_allied(state, n, wp.get_nation()))
			nations::break_alliance(state, n, wp.get_nation());
	}

	if(!as_attacker && state.world.nation_get_rank(state.world.war_get_primary_defender(w)) > state.world.nation_get_rank(n)) {
		state.world.war_set_primary_defender(w, n);
	}

	for(auto ul : state.world.nation_get_unilateral_relationship_as_source(n)) {
		if(ul.get_war_subsidies()) {
			auto role = get_role(state, w, ul.get_target());
			if(role != war_role::none) {
				if((as_attacker && role == war_role::defender) || (!as_attacker && role == war_role::attacker)) {
					ul.set_war_subsidies(false);

					notification::post(state, notification::message{
						[source = n, target = ul.get_target().id](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_wsub_end_1", text::variable_type::x, source, text::variable_type::y, target);
						},
						"msg_wsub_end_title",
						n, ul.get_target().id, dcon::nation_id{},
						sys::message_base_type::war_subsidies_end,
						dcon::province_id{ }
					});
				}
			}
		}
	}
	for(auto ul : state.world.nation_get_unilateral_relationship_as_target(n)) {
		if(ul.get_war_subsidies()) {
			auto role = get_role(state, w, ul.get_source());
			if(role != war_role::none) {
				if((as_attacker && role == war_role::defender) || (!as_attacker && role == war_role::attacker)) {
					ul.set_war_subsidies(false);

					notification::post(state, notification::message{
						[source = ul.get_source().id, target = n](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_wsub_end_1", text::variable_type::x, source, text::variable_type::y, target);
						},
						"msg_wsub_end_title",
						n, ul.get_target().id, dcon::nation_id{},
						sys::message_base_type::war_subsidies_end,
						dcon::province_id{ }
					});
				}
			}
		}
	}

	if(state.military_definitions.great_wars_enabled && !state.world.war_get_is_great(w)) {
		int32_t gp_attackers = 0;
		int32_t gp_defenders = 0;

		for(auto par : state.world.war_get_war_participant(w)) {
			if(nations::is_great_power(state, par.get_nation())) {
				if(par.get_is_attacker())
					++gp_attackers;
				else
					++gp_defenders;
			}
		}

		if(gp_attackers >= 2 && gp_defenders >= 2) {
			auto old_name = get_war_name(state, w);
			state.world.war_set_is_great(w, true);
			auto it = state.lookup_key(std::string_view{ "great_war_name" });
			if(it) {
				state.world.war_set_name(w, it);
			}

			notification::post(state, notification::message{
				[old_name, w](sys::state& state, text::layout_base& contents) {
					std::string new_war_name = get_war_name(state, w);
					text::add_line(state, contents, "msg_war_becomes_great_1", text::variable_type::x, std::string_view{old_name}, text::variable_type::y, std::string_view{new_war_name});
				},
				"msg_war_becomes_great_title",
				state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
				sys::message_base_type::war_becomes_great,
				dcon::province_id{ }
			});
		}
	}

	notification::post(state, notification::message{
		[w, n](sys::state& state, text::layout_base& contents) {
			std::string war_name = get_war_name(state, w);
			text::add_line(state, contents, "msg_war_join_1", text::variable_type::x, n, text::variable_type::val, std::string_view{war_name});
		},
		"msg_war_join_title",
		n, get_role(state, w, state.local_player_nation) != war_role::none ? state.local_player_nation : dcon::nation_id{}, dcon::nation_id{ },
		sys::message_base_type::join_war,
		dcon::province_id{ }
	});

	if(!on_war_creation && state.world.nation_get_is_player_controlled(n) == false) {
		ai::add_free_ai_cbs_to_war(state, n, w);
	}
	// update flag black status before we check for collisions with enemy armies
	state.military_definitions.pending_blackflag_update = true;
	military::update_blackflag_status(state);
	for(auto o : state.world.nation_get_army_control(n)) {
		if(o.get_army().get_is_retreating() || o.get_army().get_black_flag() || o.get_army().get_navy_from_army_transport() || o.get_army().get_battle_from_army_battle_participation())
			continue;
		auto ce = o.get_army().get_location_from_army_location();
		army_arrives_in_province(state, o.get_army(), ce, crossing_type::none);
	}
	for(auto o : state.world.nation_get_navy_control(n)) {
		if(o.get_navy().get_is_retreating() || o.get_navy().get_battle_from_navy_battle_participation())
			continue;

		auto loc = o.get_navy().get_location_from_navy_location();
		if(loc.id.index() >= state.province_definitions.first_sea_province.index())
			navy_arrives_in_province(state, o.get_navy(), loc);
	}
}


void give_back_units(sys::state& state, dcon::nation_id target) {
	state.world.nation_set_overlord_commanding_units(target, false);
}


bool is_attacker(sys::state& state, dcon::war_id w, dcon::nation_id n) {
	for(auto p : state.world.war_get_war_participant(w)) {
		if(p.get_nation() == n)
			return p.get_is_attacker();
	}
	return false;
}

war_role get_role(sys::state const& state, dcon::war_id w, dcon::nation_id n) {
	for(auto p : state.world.war_get_war_participant(w)) {
		if(p.get_nation() == n)
			return p.get_is_attacker() ? war_role::attacker : war_role::defender;
	}
	return war_role::none;
}

dcon::war_id create_war(sys::state& state, dcon::nation_id primary_attacker, dcon::nation_id primary_defender,
		dcon::cb_type_id primary_wargoal, dcon::state_definition_id primary_wargoal_state,
		dcon::national_identity_id primary_wargoal_tag, dcon::nation_id primary_wargoal_secondary) {
	assert(primary_attacker);
	assert(primary_defender);
	auto new_war = fatten(state.world, state.world.create_war());
	state.trade_route_cached_values_out_of_date = true;

	// release puppet if subject declares on overlord or vice versa
	{
		auto ol_rel = state.world.nation_get_overlord_as_subject(primary_defender);
		if(auto ol = state.world.overlord_get_ruler(ol_rel); ol && ol == primary_attacker)
			nations::release_vassal(state, ol_rel);
	}
	{
		auto ol_rel = state.world.nation_get_overlord_as_subject(primary_attacker);
		if(auto ol = state.world.overlord_get_ruler(ol_rel); ol && ol == primary_defender)
			nations::release_vassal(state, ol_rel);
	}

	auto real_target = primary_defender;
	auto target_ol_rel = state.world.nation_get_overlord_as_subject(primary_defender);
	if(auto ol = state.world.overlord_get_ruler(target_ol_rel); ol && ol != primary_attacker)
		real_target = ol;

	new_war.set_primary_attacker(primary_attacker);
	new_war.set_primary_defender(real_target);
	new_war.set_start_date(state.current_date);
	new_war.set_over_state(primary_wargoal_state);
	new_war.set_over_tag(primary_wargoal_tag);
	new_war.set_original_target(primary_defender);
	new_war.set_original_attacker(primary_attacker);
	if(primary_wargoal_secondary) {
		new_war.set_over_tag(state.world.nation_get_identity_from_identity_holder(primary_wargoal_secondary));
	}

	add_to_war(state, new_war, primary_attacker, true, true);
	add_to_war(state, new_war, real_target, false, true);

	if(primary_wargoal) {
		add_wargoal(state, new_war, primary_attacker, primary_defender, primary_wargoal, primary_wargoal_state, primary_wargoal_tag,
				primary_wargoal_secondary);
		new_war.set_name(state.world.cb_type_get_war_name(primary_wargoal));
	} else {
		auto it = state.lookup_key(std::string_view{ "agression_war_name" }); // misspelling is intentional; DO NOT CORRECT
		if(it) {
			new_war.set_name(it);
		}
	}

	if(!state.cheat_data.disable_ai) {
		if(state.world.nation_get_is_player_controlled(primary_attacker) == false)
			ai::add_free_ai_cbs_to_war(state, primary_attacker, new_war);
		if(state.world.nation_get_is_player_controlled(primary_defender) == false)
			ai::add_free_ai_cbs_to_war(state, primary_defender, new_war);
	}

	notification::post(state, notification::message{
		[primary_attacker, primary_defender, w = new_war.id](sys::state& state, text::layout_base& contents) {
			std::string resolved_war_name = get_war_name(state, w);
			text::add_line(state, contents, "msg_war_1", text::variable_type::x, primary_attacker, text::variable_type::y, primary_defender, text::variable_type::val, std::string_view{resolved_war_name});
		},
		"msg_war_title",
		primary_attacker, primary_defender, dcon::nation_id{},
		sys::message_base_type::war,
		dcon::province_id{ }
	});

	return new_war;
}

std::string get_war_name(sys::state& state, dcon::war_id war) {
	text::substitution_map sub;
	populate_war_text_subsitutions(state, war, sub);

	auto fat = fatten(state.world, war);
	auto attacker = state.world.nation_get_identity_from_identity_holder(fat.get_primary_attacker());
	auto defender = state.world.nation_get_identity_from_identity_holder(fat.get_primary_defender());
	auto attacker_tag = fatten(state.world, attacker).get_name();
	auto defender_tag = fatten(state.world, defender).get_name();
	auto war_name_sequence = fat.get_name();

	auto attacker_tag_key = attacker_tag;
	auto defender_tag_key = defender_tag;
	auto war_name_key = war_name_sequence;

	if(attacker_tag_key && defender_tag_key && war_name_key) {
		std::string war_name = std::string{ state.to_string_view(war_name_key) };
		auto attacker_name = state.to_string_view(attacker_tag_key);
		auto defender_name = state.to_string_view(defender_tag_key);
		auto combined_name = war_name + std::string("_") + std::string{ attacker_name } + std::string("_") + std::string{ defender_name };
		std::string_view name = std::string_view(combined_name);
		if(auto it = state.lookup_key(name); it)
			return text::resolve_string_substitution(state, it, sub);
	}

	return text::resolve_string_substitution(state, war_name_sequence, sub);
}

bool standard_war_joining_is_possible(sys::state& state, dcon::war_id wfor, dcon::nation_id n, bool as_attacker) {
	auto ol_relation = state.world.nation_get_overlord_as_subject(n);
	auto ol_nation = state.world.overlord_get_ruler(ol_relation);

	if(!as_attacker) {
		return joining_war_does_not_violate_constraints(state, n, wfor, false) &&
			state.world.nation_get_in_sphere_of(n) != state.world.war_get_primary_attacker(wfor) &&
			ol_nation != state.world.war_get_primary_attacker(wfor);
	} else {
		return joining_war_does_not_violate_constraints(state, n, wfor, true) &&
			state.world.nation_get_in_sphere_of(n) != state.world.war_get_primary_attacker(wfor) &&
			ol_nation != state.world.war_get_primary_attacker(wfor);
	}
}
void call_defender_allies(sys::state& state, dcon::war_id wfor) {
	if(is_civil_war(state, wfor))
		return;

	auto n = state.world.war_get_primary_defender(wfor);
	auto sphere = state.world.nation_get_in_sphere_of(n);
	bool called_in_sphere_early = false;
	for(auto drel : state.world.nation_get_diplomatic_relation(n)) {
		auto other_nation = drel.get_related_nations(0) != n ? drel.get_related_nations(0) : drel.get_related_nations(1);
		if(drel.get_are_allied() && standard_war_joining_is_possible(state, wfor, other_nation, false)) {

			diplomatic_message::message m;
			std::memset(&m, 0, sizeof(m));
			m.from = n;
			m.to = other_nation;
			m.type = diplomatic_message::type_t::call_ally_request;
			m.data.war = wfor;
			m.automatic_call = true;
			diplomatic_message::post(state, m);
			if(sphere == other_nation) {
				called_in_sphere_early = true;
			}
		}
	}
	if(state.world.nation_get_in_sphere_of(n) && !called_in_sphere_early) {
		if(joining_war_does_not_violate_constraints(state, state.world.nation_get_in_sphere_of(n), wfor, false)) {

			diplomatic_message::message m;
			std::memset(&m, 0, sizeof(m));
			m.from = n;
			m.to = state.world.nation_get_in_sphere_of(n);
			m.type = diplomatic_message::type_t::call_ally_request;
			m.data.war = wfor;
			m.automatic_call = true;
			diplomatic_message::post(state, m);
		}
	}
}
void call_attacker_allies(sys::state& state, dcon::war_id wfor) {
	if(is_civil_war(state, wfor))
		return;

	auto n = state.world.war_get_primary_attacker(wfor);
	for(auto drel : state.world.nation_get_diplomatic_relation(n)) {
		auto other_nation = drel.get_related_nations(0) != n ? drel.get_related_nations(0) : drel.get_related_nations(1);
		if(drel.get_are_allied() && !has_truce_with(state, other_nation, state.world.war_get_primary_defender(wfor)) &&
				standard_war_joining_is_possible(state, wfor, other_nation, true)) {

			diplomatic_message::message m;
			std::memset(&m, 0, sizeof(m));
			m.from = n;
			m.to = other_nation;
			m.type = diplomatic_message::type_t::call_ally_request;
			m.data.war = wfor;
			m.automatic_call = true;
			diplomatic_message::post(state, m);
		}
	}
}
void add_wargoal(sys::state& state, dcon::war_id wfor, dcon::nation_id added_by, dcon::nation_id target, dcon::cb_type_id type,
		dcon::state_definition_id sd, dcon::national_identity_id tag, dcon::nation_id secondary_nation) {
	auto for_attacker = is_attacker(state, wfor, added_by);

	if(sd) {
		std::vector<dcon::nation_id> targets;
		for(auto p : state.world.state_definition_get_abstract_state_membership(sd)) {
			auto owner = p.get_province().get_nation_from_province_ownership();
			if(std::find(targets.begin(), targets.end(), owner) == targets.end()) {
				for(auto par : state.world.war_get_war_participant(wfor)) {
					if(par.get_nation() == owner && par.get_is_attacker() != for_attacker) {
						auto new_wg = fatten(state.world, state.world.create_wargoal());
						new_wg.set_added_by(added_by);
						new_wg.set_associated_state(sd);
						new_wg.set_associated_tag(tag);
						new_wg.set_secondary_nation(secondary_nation);
						new_wg.set_target_nation(owner);
						new_wg.set_type(type);
						new_wg.set_war_from_wargoals_attached(wfor);
						targets.push_back(owner.id);
					}
				}
			}
		}

		bool is_colonial_claim = (state.world.cb_type_get_type_bits(type) & cb_flag::po_colony) != 0; //doesn't require an owned state
		if(targets.empty()) {
			if(!is_colonial_claim) {
				return; // wargoal not added
			} else {
				auto new_wg = fatten(state.world, state.world.create_wargoal());
				new_wg.set_added_by(added_by);
				new_wg.set_associated_state(sd);
				new_wg.set_associated_tag(tag);
				new_wg.set_secondary_nation(secondary_nation);
				new_wg.set_target_nation(target);
				new_wg.set_type(type);
				new_wg.set_war_from_wargoals_attached(wfor);
			}
		}
	} else {
		auto new_wg = fatten(state.world, state.world.create_wargoal());
		new_wg.set_added_by(added_by);
		new_wg.set_associated_state(sd);
		new_wg.set_associated_tag(tag);
		new_wg.set_secondary_nation(secondary_nation);
		new_wg.set_target_nation(target);
		new_wg.set_type(type);
		new_wg.set_war_from_wargoals_attached(wfor);
	}


	if(auto on_add = state.world.cb_type_get_on_add(type); on_add) {
		effect::execute(state, on_add, trigger::to_generic(added_by), trigger::to_generic(added_by), trigger::to_generic(target),
				uint32_t(state.current_date.value), uint32_t((added_by.index() << 7) ^ target.index() ^ (type.index() << 3)));
		demographics::regenerate_jingoism_support(state, added_by);
	}

	if((state.world.cb_type_get_type_bits(type) & cb_flag::always) == 0) {
		auto cb_set = state.world.nation_get_available_cbs(added_by);
		for(uint32_t i = cb_set.size(); i-- > 0;) {
			if(cb_set.at(i).cb_type == type && cb_set.at(i).target == target) {
				cb_set.remove_at(i);
				break;
			}
		}
	}
	if(added_by != state.local_player_nation && get_role(state, wfor, state.local_player_nation) != war_role::none) {
		notification::post(state, notification::message{
			[added_by, target, type, sd, tag, secondary_nation](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_wargoal_1", text::variable_type::x, added_by);

				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::recipient, target);
				text::add_to_substitution_map(sub, text::variable_type::from, added_by);
				if(secondary_nation)
					text::add_to_substitution_map(sub, text::variable_type::third, secondary_nation);
				else if(tag)
					text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, sd);

				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, state.world.cb_type_get_shortest_desc(type), sub);
				text::close_layout_box(contents, box);
			},
			"msg_wargoal_title",
			added_by, target, state.local_player_nation,
			sys::message_base_type::wargoal_added,
			dcon::province_id{ }
		});
	}
}

void remove_from_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_loss) {
	for(auto vas : state.world.nation_get_overlord_as_ruler(n)) {
		remove_from_war(state, w, vas.get_subject(), as_loss);
	}

	dcon::war_participant_id par;
	for(auto p : state.world.nation_get_war_participant(n)) {
		if(p.get_war() == w) {
			par = p.id;
			break;
		}
	}

	if(!par)
		return;

	/*
	When a losing peace offer is accepted, the ruling party in the losing nation has its party loyalty reduced by
	define:PARTY_LOYALTY_HIT_ON_WAR_LOSS percent in all provinces (this includes accepting a crisis resolution offer in which you
	lose).
	*/

	if(as_loss) {
		auto rp_ideology = state.world.nation_get_ruling_party(n).get_ideology();
		if(rp_ideology) {
			for(auto prv : state.world.nation_get_province_ownership(n)) {
				auto& cur_loyalty = prv.get_province().get_party_loyalty(rp_ideology);
				prv.get_province().set_party_loyalty(rp_ideology, cur_loyalty * (1.0f - state.defines.party_loyalty_hit_on_war_loss));
			}
		}
	}

	/*
	When a war goal is failed (the nation it targets leaves the war without that war goal being enacted): the nation that added it
	loses WAR_FAILED_GOAL_PRESTIGE_BASE^(WAR_FAILED_GOAL_PRESTIGE x current-prestige) x CB-penalty-factor prestige. Every pop in
	that nation gains CB-penalty-factor x define:WAR_FAILED_GOAL_MILITANCY militancy.
	*/

	float pop_militancy = 0.0f;

	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(wg.get_wargoal().get_added_by() == n) {
			float prestige_loss = std::min(state.defines.war_failed_goal_prestige_base, state.defines.war_failed_goal_prestige * nations::prestige_score(state, n)) * wg.get_wargoal().get_type().get_penalty_factor();
			nations::adjust_prestige(state, n, prestige_loss);

			pop_militancy += state.defines.war_failed_goal_militancy * wg.get_wargoal().get_type().get_penalty_factor();
		}
	}

	if(pop_militancy > 0) {
		for(auto prv : state.world.nation_get_province_ownership(n)) {
			for(auto pop : prv.get_province().get_pop_location()) {
				auto mil = pop_demographics::get_militancy(state, pop.get_pop());
				pop_demographics::set_militancy(state, pop.get_pop().id, std::min(mil + pop_militancy, 10.0f));
			}
		}
	}

	state.world.delete_war_participant(par);
	auto rem_wars = state.world.nation_get_war_participant(n);
	if(rem_wars.begin() == rem_wars.end()) {
		// give back units if said nation is a puppet and there are no remaining wars
		if(bool(state.world.nation_get_overlord_as_subject(n))) {
			military::give_back_units(state, n);
		}
		state.world.nation_set_is_at_war(n, false);
	}

	// Remove invalid occupations
	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(auto c = p.get_province().get_nation_from_province_control(); c && c != n) {
			if(!military::are_at_war(state, c, n)) {
				state.world.province_set_siege_progress(p.get_province(), 0.0f);
				province::set_province_controller(state, p.get_province(), n);
				military::eject_ships(state, p.get_province());
			}
		}
	}

	for(auto p : state.world.nation_get_province_control(n)) {
		if(auto c = p.get_province().get_nation_from_province_ownership(); c && c != n) {
			if(!military::are_at_war(state, c, n)) {
				state.world.province_set_siege_progress(p.get_province(), 0.0f);
				province::set_province_controller(state, p.get_province(), c);
				military::eject_ships(state, p.get_province());
			}
		}
	}

	if(as_loss) {
		state.world.nation_set_last_war_loss(n, state.current_date);
	}
}

void cleanup_war(sys::state& state, dcon::war_id w, war_result result) {
	auto par = state.world.war_get_war_participant(w);
	state.military_definitions.pending_blackflag_update = true;

	if(state.world.war_get_is_crisis_war(w)) {
		nations::cleanup_crisis(state);
	}

	while(par.begin() != par.end()) {
		if((*par.begin()).get_is_attacker()) {
			remove_from_war(state, w, (*par.begin()).get_nation(), result == war_result::defender_won);
		} else {
			remove_from_war(state, w, (*par.begin()).get_nation(), result == war_result::attacker_won);
		}
	}

	auto nbattles = state.world.war_get_naval_battle_in_war(w);
	while(nbattles.begin() != nbattles.end()) {
		end_battle(state, (*nbattles.begin()).get_battle().id, battle_result::indecisive);
	}

	auto lbattles = state.world.war_get_land_battle_in_war(w);
	while(lbattles.begin() != lbattles.end()) {
		end_battle(state, (*lbattles.begin()).get_battle().id, battle_result::indecisive);
	}

	state.world.delete_war(w);
}

void set_initial_leaders(sys::state& state) {
	for(auto a : state.world.in_army) {
		if(!bool(a.get_general_from_army_leadership())) {
			auto controller = a.get_controller_from_army_control();
			for(auto l : controller.get_leader_loyalty()) {
				if(l.get_leader().get_is_admiral() == false && !bool(l.get_leader().get_army_from_army_leadership())) {
					a.set_general_from_army_leadership(l.get_leader());
					break;
				}
			}
		}
	}
	for(auto a : state.world.in_navy) {
		if(!bool(a.get_admiral_from_navy_leadership())) {
			auto controller = a.get_controller_from_navy_control();
			for(auto l : controller.get_leader_loyalty()) {
				if(l.get_leader().get_is_admiral() == true && !bool(l.get_leader().get_navy_from_navy_leadership())) {
					a.set_admiral_from_navy_leadership(l.get_leader());
					break;
				}
			}
		}
	}
}

void take_from_sphere(sys::state& state, dcon::nation_id member, dcon::nation_id new_gp) {
	auto existing_sphere_leader = state.world.nation_get_in_sphere_of(member);
	if(existing_sphere_leader) {
		nations::remove_from_sphere(state, member, nations::influence::level_hostile);

	}

	if(!nations::is_great_power(state, new_gp))
		return;
	if(nations::is_great_power(state, member)) {
		// edge case which happens when losing nation became a great power during the war
		return;
	}
	if(state.world.nation_get_owned_province_count(member) == 0)
		return;

	auto nrel = state.world.get_gp_relationship_by_gp_influence_pair(member, new_gp);
	if(!nrel) {
		nrel = state.world.force_create_gp_relationship(member, new_gp);
	}

	state.world.gp_relationship_set_influence(nrel, state.defines.max_influence);
	nations::sphere_nation(state, member, new_gp);

	notification::post(state, notification::message{
		[member, existing_sphere_leader, new_gp](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_rem_sphere_2", text::variable_type::x, new_gp, text::variable_type::y, member, text::variable_type::val, existing_sphere_leader);
		},
		"msg_rem_sphere_title",
		new_gp, existing_sphere_leader, member,
		sys::message_base_type::rem_sphere,
		dcon::province_id{ }
	});
}

void implement_war_goal(sys::state& state, dcon::war_id war, dcon::cb_type_id wargoal, dcon::nation_id from,
		dcon::nation_id target, dcon::nation_id secondary_nation, dcon::state_definition_id wargoal_state,
		dcon::national_identity_id wargoal_t) {
	assert(from);
	assert(target);
	assert(wargoal);

	// variable for testing AI changes
	// state.pressed_wargoals++;

	auto bits = state.world.cb_type_get_type_bits(wargoal);
	bool for_attacker = is_attacker(state, war, from);

	// po_unequal_treaty: opens market for 5 years
	if((bits & cb_flag::po_unequal_treaty) != 0) {
		auto enddt = state.current_date + (int32_t)(365 * state.defines.alice_free_trade_agreement_years);

		// One way tariff removal
		auto rel_1 = state.world.get_unilateral_relationship_by_unilateral_pair(target, from);
		if(!rel_1) {
			rel_1 = state.world.force_create_unilateral_relationship(target, from);
		}
		state.world.unilateral_relationship_set_no_tariffs_until(rel_1, enddt);
		state.trade_route_cached_values_out_of_date = true;
	}

	// po_add_to_sphere: leaves its current sphere and has its opinion of that nation set to hostile. Is added to the nation that
	// added the war goal's sphere with max influence.
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		if(secondary_nation) {
			if(secondary_nation != target) {
				if(state.world.nation_get_owned_province_count(secondary_nation) != 0)
					take_from_sphere(state, secondary_nation, from);
			} else { // we see this in the independence cb
				if(state.world.nation_get_owned_province_count(from) != 0)
					take_from_sphere(state, from, dcon::nation_id{});
			}
		} else {
			take_from_sphere(state, target, from);
		}
	}

	// po_clear_union_sphere: every nation of the actors culture group in the target nation's sphere leaves (and has relation set
	// to friendly) and is added to the actor nation's sphere with max influence. All vassals of the target nation affected by
	// this are freed.
	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		auto from_cg = state.world.nation_get_primary_culture(from).get_group_from_culture_group_membership();
		for(auto gprl : state.world.nation_get_gp_relationship_as_great_power(target)) {
			if(gprl.get_influence_target().get_in_sphere_of() == target &&
					gprl.get_influence_target().get_primary_culture().get_group_from_culture_group_membership() == from_cg) {

				take_from_sphere(state, gprl.get_influence_target(), from);
			}
		}
	}

	// po_release_puppet: nation stops being a vassal
	if((bits & cb_flag::po_release_puppet) != 0) {
		assert(secondary_nation);
		if(secondary_nation != target) {
			auto ol = state.world.nation_get_overlord_as_subject(secondary_nation);
			if(ol) {
				nations::release_vassal(state, ol);
			}
		} else { // we see this in the independence cb
			auto ol = state.world.nation_get_overlord_as_subject(from);
			if(ol) {
				nations::release_vassal(state, ol);
			}
		}
	}

	// po_make_puppet: the target nation releases all of its vassals and then becomes a vassal of the acting nation.
	if((bits & cb_flag::po_make_puppet) != 0) {
		// Unless CB has po_save_subjects - release all subjects
		if((bits & cb_flag::po_save_subjects) == 0) {
			for(auto sub : state.world.nation_get_overlord_as_ruler(target)) {
				nations::release_vassal(state, sub);
			}
		} else {
			for(auto sub : state.world.nation_get_overlord_as_ruler(target)) {
				nations::make_vassal(state, sub.get_subject(), from);
			}
		}
		if(state.world.nation_get_owned_province_count(target) > 0) {
			nations::make_vassal(state, target, from);
			take_from_sphere(state, target, from);
		}
	}

	// po_make_substate: the target nation releases all of its vassals and then becomes a vassal of the acting nation.
	if((bits & cb_flag::po_make_substate) != 0) {
		// Unless CB has po_save_subjects - release all subjects
		if((bits & cb_flag::po_save_subjects) == 0) {
			for(auto sub : state.world.nation_get_overlord_as_ruler(target)) {
				nations::release_vassal(state, sub);
			}
		} else {
			for(auto sub : state.world.nation_get_overlord_as_ruler(target)) {
				nations::make_vassal(state, sub.get_subject(), from);
			}
		}

		if(state.world.nation_get_owned_province_count(target) > 0) {
			nations::make_substate(state, target, from);
			take_from_sphere(state, target, from);
		}
	}

	// po_destory_forts: reduces fort levels to zero in any targeted states
	if((bits & cb_flag::po_destroy_forts) != 0) {
		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					prov.get_province().set_building_level(uint8_t(economy::province_building_type::fort), 0);
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
					trigger::to_generic(from))) {
					province::for_each_province_in_state_instance(state, si.get_state(),
							[&](dcon::province_id prov) { state.world.province_set_building_level(prov, uint8_t(economy::province_building_type::fort), 0); });
				}
			}
		}
	}

	// po_destory_naval_bases: as above
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					prov.get_province().set_building_level(uint8_t(economy::province_building_type::naval_base), 0);
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
					trigger::to_generic(from))) {
					province::for_each_province_in_state_instance(state, si.get_state(),
							[&](dcon::province_id prov) { state.world.province_set_building_level(prov, uint8_t(economy::province_building_type::naval_base), 0); });
				}
			}
		}
	}

	// po_disarmament: a random define:DISARMAMENT_ARMY_HIT fraction of the nations units are destroyed. All current unit
	// constructions are canceled. The nation is disarmed. Disarmament lasts until define:REPARATIONS_YEARS or the nation is at
	// war again.
	if((bits & cb_flag::po_disarmament) != 0) {
		if(state.world.nation_get_owned_province_count(target) > 0) {
			state.world.nation_set_disarmed_until(target, state.current_date + int32_t(state.defines.reparations_years) * 365);
		}
		// Cancel all constructions
		for(const auto po : state.world.nation_get_province_ownership(target)) {
			auto lc = po.get_province().get_province_building_construction();
			while(lc.begin() != lc.end()) {
				state.world.delete_province_building_construction(*(lc.begin()));
			}
			auto nc = po.get_province().get_province_naval_construction();
			while(nc.begin() != nc.end()) {
				state.world.delete_province_naval_construction(*(nc.begin()));
			}
		}
		auto uc = state.world.nation_get_province_land_construction(target);
		while(uc.begin() != uc.end()) {
			state.world.delete_province_land_construction(*(uc.begin()));
		}
		// Destroy units (fraction is disarmament hit)
		if(state.defines.disarmament_army_hit > 0.f) {
			auto ar = state.world.nation_get_army_control(target);
			auto total = int32_t(ar.end() - ar.begin());
			auto rem = int32_t(float(total) * state.defines.disarmament_army_hit);
			auto it = ar.begin();
			while(it != ar.end() && rem > 0) {
				if((*it).get_army().get_army_battle_participation()) {
					++it;
				} else {
					military::cleanup_army(state, (*it).get_army());
					delete (&it);
					new (&it) dcon::internal::iterator_nation_foreach_army_control_as_controller(ar.begin());
					--rem;
				}
			}
		}
	}

	// po_reparations: the nation is set to pay reparations for define:REPARATIONS_YEARS
	if((bits & cb_flag::po_reparations) != 0) {
		if(state.world.nation_get_owned_province_count(target) > 0) {
			state.world.nation_set_reparations_until(target, state.current_date + int32_t(state.defines.reparations_years) * 365);
			auto urel = state.world.get_unilateral_relationship_by_unilateral_pair(target, from);
			if(!urel) {
				urel = state.world.force_create_unilateral_relationship(target, from);
			}
			state.world.unilateral_relationship_set_reparations(urel, true);
		}
	}

	// po_remove_prestige: the target loses (current-prestige x define:PRESTIGE_REDUCTION) + define:PRESTIGE_REDUCTION_BASE
	// prestige
	if((bits & cb_flag::po_remove_prestige) != 0) {
		if(state.world.nation_get_owned_province_count(target) > 0) {
			nations::adjust_prestige(state, target,
					-(state.defines.prestige_reduction * nations::prestige_score(state, target) + state.defines.prestige_reduction_base));
		}
	}

	// po_install_communist_gov: The target switches its government type and ruling ideology (if possible) to that of the nation
	// that added the war goal. Relations with the nation that added the war goal are set to 0. The nation leaves its current
	// sphere and enters the actor's sphere if it is a GP. If the war continues, the war leader on the opposite side gains the
	// appropriate `counter_wargoal_on_install_communist_gov` CB, if any and allowed by the conditions of that CB.
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		if(state.world.nation_get_owned_province_count(target) > 0) {
			politics::change_government_type(state, target, state.world.nation_get_government_type(from));
			politics::force_ruling_party_ideology(state, target,
					state.world.political_party_get_ideology(state.world.nation_get_ruling_party(from)));
			take_from_sphere(state, target, from);
		}
	}

	// po_uninstall_communist_gov_type: The target switches its government type to that of the nation that added the war goal. The
	// nation leaves its current sphere and enters the actor's sphere if it is a GP.
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		if(state.world.nation_get_owned_province_count(target) > 0) {
			politics::change_government_type(state, target, state.world.nation_get_government_type(from));
			take_from_sphere(state, target, from);
		}
	}

	// po_colony: colonization finishes, with the adder of the war goal getting the colony and all other colonizers being kicked
	// out
	if((bits & cb_flag::po_colony) != 0) {
		std::vector<dcon::colonization_id> to_del;
		for(auto c : state.world.state_definition_get_colonization(wargoal_state)) {
			if(c.get_colonizer() != from)
				to_del.push_back(c.id);
		}
		for(auto c : to_del) {
			state.world.delete_colonization(c);
		}
	}

	bool target_existed = state.world.nation_get_owned_province_count(target);

	// po_remove_cores: also cores are removed from any territory taken / target territory if it is already owned by sender
	// po_transfer_provinces: all the valid states are transferred to the nation specified in the war goal. Relations between that
	// country and the nation that added the war goal increase by define:LIBERATE_STATE_RELATION_INCREASE. If the nation is newly
	// created by this, the nation it was created from gets a truce of define:BASE_TRUCE_MONTHS months with it (and it is placed
	// in the liberator's sphere if that nation is a GP).

	if((bits & cb_flag::po_transfer_provinces) != 0) {
		auto target_tag = state.world.nation_get_identity_from_identity_holder(target);
		// No defined wargoal_t leads to defaulting to the from nation
		dcon::nation_id holder = from;
		if(wargoal_t) {
			holder = state.world.national_identity_get_nation_from_identity_holder(wargoal_t);
			if(!holder) {
				holder = state.world.create_nation();
				state.world.nation_set_identity_from_identity_holder(holder, wargoal_t);
			}
			if(auto lprovs = state.world.nation_get_province_ownership(holder); lprovs.begin() == lprovs.end()) {
				nations::create_nation_based_on_template(state, holder, from);
			}
		}
		// add to sphere if not existed
		if(!target_existed && state.world.nation_get_is_great_power(from)) {
			nations::sphere_nation(state, holder, from);
		}
		add_truce(state, holder, target, int32_t(state.defines.base_truce_months) * 30);

		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					province::conquer_province(state, prov.get_province(), holder);
					if((bits & cb_flag::po_remove_cores) != 0) {
						auto find_core = state.world.get_core_by_prov_tag_key(prov.get_province(), target_tag);
						if(find_core) {
							state.world.delete_core(find_core);
						}
					}
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			std::vector<dcon::state_instance_id> prior_states;
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from), trigger::to_generic(holder))) {
					prior_states.push_back(si.get_state());
				}
			}
			for(auto si : prior_states) {
				for(auto prov : state.world.state_definition_get_abstract_state_membership(state.world.state_instance_get_definition(si))) {
					if(prov.get_province().get_nation_from_province_ownership() == target) {
						province::conquer_province(state, prov.get_province(), holder);
						if((bits & cb_flag::po_remove_cores) != 0) {
							auto find_core = state.world.get_core_by_prov_tag_key(prov.get_province(), target_tag);
							if(find_core) {
								state.world.delete_core(find_core);
							}
						}
					}
				}
			}
		}
	}

	// po_demand_state: state is taken (this can turn into annex if it is the last state)
	if((bits & cb_flag::po_demand_state) != 0) {
		auto target_tag = state.world.nation_get_identity_from_identity_holder(target);

		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					province::conquer_province(state, prov.get_province(), from);
					if((bits & cb_flag::po_remove_cores) != 0) {
						auto find_core = state.world.get_core_by_prov_tag_key(prov.get_province(), target_tag);
						if(find_core) {
							state.world.delete_core(find_core);
						}
					}
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			std::vector<dcon::state_instance_id> prior_states;
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from), trigger::to_generic(from))) {
					prior_states.push_back(si.get_state());
				}
			}
			for(auto si : prior_states) {
				for(auto prov :
						state.world.state_definition_get_abstract_state_membership(state.world.state_instance_get_definition(si))) {

					if(prov.get_province().get_nation_from_province_ownership() == target) {
						province::conquer_province(state, prov.get_province(), from);
						if((bits & cb_flag::po_remove_cores) != 0) {
							auto find_core = state.world.get_core_by_prov_tag_key(prov.get_province(), target_tag);
							if(find_core) {
								state.world.delete_core(find_core);
							}
						}
					}
				}
			}
		}

		if(war && target_existed && state.world.nation_get_owned_province_count(target) == 0) {
			if(state.military_definitions.liberate) {
				auto counter_wg = fatten(state.world, state.world.create_wargoal());
				counter_wg.set_added_by(
						for_attacker ? state.world.war_get_primary_defender(war) : state.world.war_get_primary_attacker(war));
				counter_wg.set_target_nation(from);
				counter_wg.set_associated_tag(state.world.nation_get_identity_from_identity_holder(target));
				counter_wg.set_type(state.military_definitions.liberate);
				state.world.force_create_wargoals_attached(war, counter_wg);
			}
		}
	}

	// po_annex: nation is annexed, vassals and substates are released, diplomatic relations are dissolved.
	if((bits & cb_flag::po_annex) != 0 && target != from) {
		// Unless CB has po_save_subjects - release all subjects
		if((bits & cb_flag::po_save_subjects) == 0) {
			// Release vassals
			for(auto n : state.world.in_nation) {
				auto rel = state.world.nation_get_overlord_as_subject(n);
				auto overlord = state.world.overlord_get_ruler(rel);

				if(overlord == target) {
					nations::release_vassal(state, rel);
				}
			}
		} else {
			// Transfer vassals
			for(auto n : state.world.in_nation) {
				auto rel = state.world.nation_get_overlord_as_subject(n);
				auto overlord = state.world.overlord_get_ruler(rel);

				if(overlord == target) {
					nations::make_vassal(state, n, from);
				}
			}
		}

		auto target_tag = state.world.nation_get_identity_from_identity_holder(target);
		auto target_owns = state.world.nation_get_province_ownership(target);

		while(target_owns.begin() != target_owns.end()) {
			auto prov = (*target_owns.begin()).get_province();
			province::conquer_province(state, prov, from);
			if((bits & cb_flag::po_remove_cores) != 0) {
				auto find_core = state.world.get_core_by_prov_tag_key(prov, target_tag);
				if(find_core) {
					state.world.delete_core(find_core);
				}
			}
		}

		if(target_existed) {
			if(war && state.military_definitions.liberate) {
				auto counter_wg = fatten(state.world, state.world.create_wargoal());
				counter_wg.set_added_by(
						for_attacker ? state.world.war_get_primary_defender(war) : state.world.war_get_primary_attacker(war));
				counter_wg.set_target_nation(from);
				counter_wg.set_associated_tag(state.world.nation_get_identity_from_identity_holder(target));
				counter_wg.set_type(state.military_definitions.liberate);
				state.world.force_create_wargoals_attached(war, counter_wg);
			}
		}
	}

	// other/in general: the `on_po_accepted` member of the CB is run. Primary slot: target of the war goal. This slot: nation
	// that added the war goal.
	auto accepted_effect = state.world.cb_type_get_on_po_accepted(wargoal);
	if(accepted_effect) {
		effect::execute(state, accepted_effect, trigger::to_generic(target), trigger::to_generic(from), trigger::to_generic(from),
				uint32_t((state.current_date.value << 8) ^ target.index()), uint32_t(from.index() ^ (wargoal.index() << 3)));
	}

	// The nation that added the war goal gains prestige. This is done, by calculating the sum ,over all the po tags, of
	// base-prestige-for-that-tag v (nations-current-prestige x prestige-for-that-tag) and then multiplying the result by the CB's
	// prestige factor. The nation that was targeted by the war goal also loses that much prestige.
	float prestige_gain = successful_cb_prestige(state, wargoal, from) * (war ? 1.0f : state.defines.crisis_wargoal_prestige_mult);
	if(state.world.nation_get_owned_province_count(from) > 0)
		nations::adjust_prestige(state, from, prestige_gain);
	if(state.world.nation_get_owned_province_count(target) > 0)
		nations::adjust_prestige(state, target, -prestige_gain);
}

void merge_navies_impl(sys::state& state, dcon::navy_id a, dcon::navy_id b) {
	assert(state.world.navy_get_controller_from_navy_control(a) == state.world.navy_get_controller_from_navy_control(b));
	// take leader
	auto a_leader = state.world.navy_get_admiral_from_navy_leadership(a);
	auto b_leader = state.world.navy_get_admiral_from_navy_leadership(b);
	if(!a_leader && b_leader) {
		state.world.navy_set_admiral_from_navy_leadership(a, b_leader);
	}

	uint8_t highest_months_out_of_range = std::max(state.world.navy_get_months_outside_naval_range(b), state.world.navy_get_months_outside_naval_range(a));

	state.world.navy_set_months_outside_naval_range(a, highest_months_out_of_range);

	auto regs = state.world.navy_get_navy_membership(b);
	while(regs.begin() != regs.end()) {
		auto reg = (*regs.begin()).get_ship();
		reg.set_navy_from_navy_membership(a);
	}

	auto transported = state.world.navy_get_army_transport(b);
	while(transported.begin() != transported.end()) {
		auto arm = (*transported.begin()).get_army();
		arm.set_navy_from_army_transport(a);
	}
}

void run_gc(sys::state& state) {

	//
	// peace offers from dead nations
	//

	auto remove_pending_offer = [&](dcon::peace_offer_id id) {
		for(auto& m : state.pending_messages) {
			if(m.type == diplomatic_message::type::peace_offer && m.data.peace == id) {
				m.type = diplomatic_message::type::none;
				return;
			}
		}
		};
	for(auto po : state.world.in_peace_offer) {
		if(!po.get_nation_from_pending_peace_offer()) {
			remove_pending_offer(po);
			state.world.delete_peace_offer(po);
		} else if(!po.get_war_from_war_settlement() && !po.get_is_crisis_offer()) {
			remove_pending_offer(po);
			state.world.delete_peace_offer(po);
		} else if(state.current_crisis_state == sys::crisis_state::inactive && po.get_is_crisis_offer()) {
			remove_pending_offer(po);
			state.world.delete_peace_offer(po);
		}
	}

	//
	// war goals from nations that have left the war
	//

	static std::vector<dcon::wargoal_id> to_delete;
	to_delete.clear();
	for(auto w : state.world.in_war) {
		for(auto wg : w.get_wargoals_attached()) {
			if(get_role(state, w, wg.get_wargoal().get_added_by()) == war_role::none || get_role(state, w, wg.get_wargoal().get_target_nation()) == war_role::none)
				to_delete.push_back(wg.get_wargoal());
		}
	}
	for(auto g : to_delete)
		state.world.delete_wargoal(g);

	for(auto w : state.world.in_war) {
		if(get_role(state, w, w.get_primary_attacker()) != war_role::attacker || w.get_primary_attacker().get_overlord_as_subject().get_ruler()) {
			int32_t best_rank = 0;
			dcon::nation_id n;
			for(auto par : w.get_war_participant()) {
				if(par.get_is_attacker() && !(par.get_nation().get_overlord_as_subject().get_ruler())) {
					if(!n || par.get_nation().get_rank() < best_rank) {
						best_rank = par.get_nation().get_rank();
						n = par.get_nation();
					}
				}
			}
			if(!n) {
				cleanup_war(state, w, military::war_result::draw);
				continue;
			}
			w.set_primary_attacker(n);
		}
		if(get_role(state, w, w.get_primary_defender()) != war_role::defender || w.get_primary_defender().get_overlord_as_subject().get_ruler()) {
			int32_t best_rank = 0;
			dcon::nation_id n;
			for(auto par : w.get_war_participant()) {
				if(!par.get_is_attacker() && !(par.get_nation().get_overlord_as_subject().get_ruler())) {
					if(!n || par.get_nation().get_rank() < best_rank) {
						best_rank = par.get_nation().get_rank();
						n = par.get_nation();
					}
				}
			}
			if(!n) {
				cleanup_war(state, w, military::war_result::draw);
				continue;
			}
			w.set_primary_defender(n);
		}
		bool non_sq_war_goal = false;
		for(auto wg : w.get_wargoals_attached()) {
			// Has to truly be a status quo, not a pseudo status quo like the american cb on GFM
			if(wg.get_wargoal().get_type().get_type_bits() == cb_flag::po_status_quo) {
				// ignore status quo
			} else {
				non_sq_war_goal = true;
				break;
			}
		}
		if(!non_sq_war_goal)
			cleanup_war(state, w, military::war_result::draw);
	}


	//
	// wargoals that do not belong to a peace offer or war
	//
	for(auto wg : state.world.in_wargoal) {
		auto po = wg.get_peace_offer_from_peace_offer_item();
		auto wr = wg.get_war_from_wargoals_attached();
		if(!po && !wr)
			state.world.delete_wargoal(wg);
	}

	//
	// empty armies / navies or leaderless ones
	//
	for(uint32_t i = state.world.navy_size(); i-- > 0; ) {
		dcon::navy_id n{ dcon::navy_id::value_base_t(i) };
		if(state.world.navy_is_valid(n)) {
			auto rng = state.world.navy_get_navy_membership(n);
			if(!state.world.navy_get_battle_from_navy_battle_participation(n)) {
				if(rng.begin() == rng.end() || !state.world.navy_get_controller_from_navy_control(n)) {
					military::cleanup_navy(state, n);
				}
			}
		}
	}
	for(uint32_t i = state.world.army_size(); i-- > 0; ) {
		dcon::army_id n{ dcon::army_id::value_base_t(i) };
		if(state.world.army_is_valid(n)) {
			auto rng = state.world.army_get_army_membership(n);
			if(!state.world.army_get_battle_from_army_battle_participation(n)) {
				if(rng.begin() == rng.end() || (!state.world.army_get_controller_from_army_rebel_control(n) && !state.world.army_get_controller_from_army_control(n))) {
					military::cleanup_army(state, n);
				}
			}
		}
	}
}

void add_truce_between_sides(sys::state& state, dcon::war_id w, int32_t months) {
	auto wpar = state.world.war_get_war_participant(w);
	auto num_par = int32_t(wpar.end() - wpar.begin());
	auto end_truce = state.current_date + months * 31;

	for(int32_t i = 0; i < num_par; ++i) {
		auto this_par = *(wpar.begin() + i);
		auto this_nation = this_par.get_nation();

		auto attacker = this_par.get_is_attacker();

		for(int32_t j = i + 1; j < num_par; ++j) {
			auto other_par = *(wpar.begin() + j);
			auto other_nation = other_par.get_nation();

			if(!other_nation.get_overlord_as_subject().get_ruler() && attacker != other_par.get_is_attacker()) {
				auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(this_nation, other_nation);
				if(!rel) {
					rel = state.world.force_create_diplomatic_relation(this_nation, other_nation);
				}
				auto& current_truce = state.world.diplomatic_relation_get_truce_until(rel);
				if(!current_truce || current_truce < end_truce)
					state.world.diplomatic_relation_set_truce_until(rel, end_truce);
			}
		}
	}
}
void add_truce_from_nation(sys::state& state, dcon::war_id w, dcon::nation_id n, int32_t months) {
	auto end_truce = state.current_date + months * 31;
	bool attacker = military::is_attacker(state, w, n);

	for(auto par : state.world.war_get_war_participant(w)) {
		auto other_nation = par.get_nation();


		auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(n, other_nation);
		if(!rel) {
			rel = state.world.force_create_diplomatic_relation(n, other_nation);
		}
		auto& current_truce = state.world.diplomatic_relation_get_truce_until(rel);
		if(!current_truce || current_truce < end_truce)
			state.world.diplomatic_relation_set_truce_until(rel, end_truce);
	}
}

void add_truce(sys::state& state, dcon::nation_id a, dcon::nation_id b, int32_t days) {
	auto end_truce = state.current_date + days;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
	if(!rel) {
		rel = state.world.force_create_diplomatic_relation(a, b);
	}
	auto& current_truce = state.world.diplomatic_relation_get_truce_until(rel);
	if(!current_truce || current_truce < end_truce)
		state.world.diplomatic_relation_set_truce_until(rel, end_truce);
}

void implement_peace_offer(sys::state& state, dcon::peace_offer_id offer) {
	dcon::nation_id from = state.world.peace_offer_get_nation_from_pending_peace_offer(offer);
	dcon::nation_id target = state.world.peace_offer_get_target(offer);


	auto war = state.world.peace_offer_get_war_from_war_settlement(offer);


	if(war) {
		notification::post(state, notification::message{
			[from, target, pa = state.world.war_get_primary_attacker(war), pd = state.world.war_get_primary_defender(war), name = state.world.war_get_name(war), tag = state.world.war_get_over_tag(war), st = state.world.war_get_over_state(war)](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, text::get_adjective(state, pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, text::get_adjective(state, pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_peace_offer_accepted_1", text::variable_type::x, target, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_peace_offer_accepted_title",
			target, from, dcon::nation_id{},
			sys::message_base_type::peace_accepted,
			dcon::province_id{ }
		});
	}

	bool contains_sq = false;
	//implementation order matters
	for(auto wg_offered : state.world.peace_offer_get_peace_offer_item(offer)) {
		auto wg = wg_offered.get_wargoal();
		implement_war_goal(state, state.world.peace_offer_get_war_from_war_settlement(offer), wg.get_type(),
			wg.get_added_by(), wg.get_target_nation(), wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag());
		if((wg.get_type().get_type_bits() & military::cb_flag::po_status_quo) != 0)
			contains_sq = true;
	}

	if(war) {
		auto truce_months = military::peace_offer_truce_months(state, offer);
		// remove successful WG
		auto offer_range = state.world.peace_offer_get_peace_offer_item(offer);
		while(offer_range.begin() != offer_range.end()) {
			state.world.delete_wargoal((*offer_range.begin()).get_wargoal());
		}

		if((state.world.war_get_primary_attacker(war) == from && state.world.war_get_primary_defender(war) == target)
			|| (contains_sq && military::is_attacker(state, war, from))) {

			if(state.world.war_get_is_great(war)) {
				if(state.world.peace_offer_get_is_concession(offer) == false) {
					for(auto par : state.world.war_get_war_participant(war)) {
						if(par.get_is_attacker() == false) {
							implement_war_goal(state, war, state.military_definitions.standard_great_war, from, par.get_nation(),
									dcon::nation_id{}, dcon::state_definition_id{}, dcon::national_identity_id{});
						}
					}
				} else {
					for(auto par : state.world.war_get_war_participant(war)) {
						if(par.get_is_attacker() == true) {
							implement_war_goal(state, war, state.military_definitions.standard_great_war, target, par.get_nation(),
									dcon::nation_id{}, dcon::state_definition_id{}, dcon::national_identity_id{});
						}
					}
				}
			}
			add_truce_between_sides(state, war, truce_months);
			cleanup_war(state, war,
					state.world.peace_offer_get_is_concession(offer) ? war_result::defender_won : war_result::attacker_won);

		} else if((state.world.war_get_primary_attacker(war) == target && state.world.war_get_primary_defender(war) == from)
			|| (contains_sq && !military::is_attacker(state, war, from))) {

			if(state.world.war_get_is_great(war)) {
				if(state.world.peace_offer_get_is_concession(offer) == false) {
					for(auto par : state.world.war_get_war_participant(war)) {
						if(par.get_is_attacker() == true) {
							implement_war_goal(state, war, state.military_definitions.standard_great_war, from, par.get_nation(),
									dcon::nation_id{}, dcon::state_definition_id{}, dcon::national_identity_id{});
						}
					}
				} else {
					for(auto par : state.world.war_get_war_participant(war)) {
						if(par.get_is_attacker() == false) {
							implement_war_goal(state, war, state.military_definitions.standard_great_war, target, par.get_nation(),
									dcon::nation_id{}, dcon::state_definition_id{}, dcon::national_identity_id{});
						}
					}
				}
			}
			add_truce_between_sides(state, war, truce_months);
			cleanup_war(state, war,
					state.world.peace_offer_get_is_concession(offer) ? war_result::attacker_won : war_result::defender_won);

		} else if(state.world.war_get_primary_attacker(war) == from || state.world.war_get_primary_defender(war) == from) {

			if(state.world.war_get_is_great(war) && state.world.peace_offer_get_is_concession(offer) == false) {
				implement_war_goal(state, war, state.military_definitions.standard_great_war, from, target, dcon::nation_id{},
						dcon::state_definition_id{}, dcon::national_identity_id{});
			}

			if(state.world.nation_get_owned_province_count(state.world.war_get_primary_attacker(war)) == 0) {
				add_truce_between_sides(state, war, truce_months);
				cleanup_war(state, war, war_result::defender_won);
			} else if(state.world.nation_get_owned_province_count(state.world.war_get_primary_defender(war)) == 0) {
				add_truce_between_sides(state, war, truce_months);
				cleanup_war(state, war, war_result::attacker_won);
			} else {
				add_truce_from_nation(state, war, target, truce_months);
				remove_from_war(state, war, target, state.world.peace_offer_get_is_concession(offer) == false);
			}
		} else if(state.world.war_get_primary_attacker(war) == target || state.world.war_get_primary_defender(war) == target) {

			if(state.world.war_get_is_great(war) && state.world.peace_offer_get_is_concession(offer) == true) {
				implement_war_goal(state, war, state.military_definitions.standard_great_war, target, from, dcon::nation_id{},
						dcon::state_definition_id{}, dcon::national_identity_id{});
			}

			if(state.world.nation_get_owned_province_count(state.world.war_get_primary_attacker(war)) == 0) {
				add_truce_between_sides(state, war, truce_months);
				cleanup_war(state, war, war_result::defender_won);
			} else if(state.world.nation_get_owned_province_count(state.world.war_get_primary_defender(war)) == 0) {
				add_truce_between_sides(state, war, truce_months);
				cleanup_war(state, war, war_result::attacker_won);
			} else {
				add_truce_from_nation(state, war, from, truce_months);
				remove_from_war(state, war, from, state.world.peace_offer_get_is_concession(offer) == true);
			}
		} else {
			assert(false);
		}
	} else { // crisis offer
		bool crisis_attackers_won = (from == state.primary_crisis_attacker) == (state.world.peace_offer_get_is_concession(offer) == false);

		auto truce_months = military::peace_offer_truce_months(state, offer);

		for(auto wg : state.world.peace_offer_get_peace_offer_item(offer)) {
			add_truce(state, wg.get_wargoal().get_added_by(), wg.get_wargoal().get_target_nation(), truce_months * 31);
		}

		for(auto swg : state.crisis_attacker_wargoals) {
			bool was_part_of_offer = false;
			for(auto wg : state.world.peace_offer_get_peace_offer_item(offer)) {
				if(wg.get_wargoal().get_added_by() == swg.added_by)
					was_part_of_offer = true;
			}

			if(!was_part_of_offer) {
				float prestige_loss = std::min(state.defines.war_failed_goal_prestige_base,
																	state.defines.war_failed_goal_prestige * state.defines.crisis_wargoal_prestige_mult *
																			nations::prestige_score(state, swg.added_by)) *
					state.world.cb_type_get_penalty_factor(swg.cb);
				nations::adjust_prestige(state, swg.added_by, prestige_loss);

				auto pop_militancy = state.defines.war_failed_goal_militancy * state.defines.crisis_wargoal_militancy_mult * state.world.cb_type_get_penalty_factor(swg.cb);
				if(pop_militancy > 0) {
					for(auto prv : state.world.nation_get_province_ownership(swg.added_by)) {
						for(auto pop : prv.get_province().get_pop_location()) {
							auto mil = pop_demographics::get_militancy(state, pop.get_pop());
							pop_demographics::set_militancy(state, pop.get_pop().id, std::min(mil + pop_militancy, 10.0f));
						}
					}
				}

				if(crisis_attackers_won) {
					// Wargoal added by attacker. They won, but leader ignored wargoal
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_attacker,
							-state.defines.crisis_winner_relations_impact);
				} else {
					// Wargoal added by attacker. They lost
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_defender,
							-state.defines.crisis_winner_relations_impact);
				}
			} else {
				if(crisis_attackers_won) {
					// Wargoal added by attacker. They won and leader enforced WG
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_attacker,
							state.defines.crisis_winner_relations_impact);
				} else {
					// Wargoal added by attacker. They lost and defender leader enforced WG
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_defender,
							state.defines.crisis_winner_relations_impact);
				}
			}
		}

		for(auto swg : state.crisis_attacker_wargoals) {
			bool was_part_of_offer = false;
			for(auto wg : state.world.peace_offer_get_peace_offer_item(offer)) {
				if(wg.get_wargoal().get_added_by() == swg.added_by)
					was_part_of_offer = true;
			}

			if(!was_part_of_offer) {
				float prestige_loss = std::min(state.defines.war_failed_goal_prestige_base,
																	state.defines.war_failed_goal_prestige * state.defines.crisis_wargoal_prestige_mult *
																			nations::prestige_score(state, swg.added_by)) *
					state.world.cb_type_get_penalty_factor(swg.cb);
				nations::adjust_prestige(state, swg.added_by, prestige_loss);

				auto pop_militancy = state.defines.war_failed_goal_militancy * state.defines.crisis_wargoal_militancy_mult * state.world.cb_type_get_penalty_factor(swg.cb);
				if(pop_militancy > 0) {
					for(auto prv : state.world.nation_get_province_ownership(swg.added_by)) {
						for(auto pop : prv.get_province().get_pop_location()) {
							auto mil = pop_demographics::get_militancy(state, pop.get_pop());
							pop_demographics::set_militancy(state, pop.get_pop().id, std::min(mil + pop_militancy, 10.0f));
						}
					}
				}

				if(crisis_attackers_won) {
					// Wargoal added by defender. They lost, but leader ignored wargoal
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_defender,
							-state.defines.crisis_winner_relations_impact);
				} else {
					// Wargoal added by defender. They won and defender leader enforced WG
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_attacker,
							-state.defines.crisis_winner_relations_impact);
				}
			} else {
				if(crisis_attackers_won) {
					// Wargoal added by defender. They lost and attacker leader enforced WG
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_attacker,
							state.defines.crisis_winner_relations_impact);
				} else {
					// Wargoal added by defender. They won and defender leader enforced WG
					nations::adjust_relationship(state, swg.added_by, state.primary_crisis_defender,
							state.defines.crisis_winner_relations_impact);
				}
			}
		}

		for(auto& par : state.crisis_participants) {
			if(!par.id)
				break;

			if(par.merely_interested == false && par.id != state.primary_crisis_attacker && par.id != state.primary_crisis_defender) {
				if(crisis_attackers_won != par.supports_attacker) {
					if(par.supports_attacker) {
						nations::adjust_relationship(state, par.id, state.primary_crisis_attacker,
								-state.defines.crisis_winner_relations_impact);
					} else {
						nations::adjust_relationship(state, par.id, state.primary_crisis_defender,
								-state.defines.crisis_winner_relations_impact);
					}
				} else {
					if(par.supports_attacker) {
						nations::adjust_relationship(state, par.id, state.primary_crisis_attacker,
								state.defines.crisis_winner_relations_impact);
					} else {
						nations::adjust_relationship(state, par.id, state.primary_crisis_defender,
								state.defines.crisis_winner_relations_impact);
					}
				}
			}
		}

		if(crisis_attackers_won) {

			float p_factor = 0.05f * (state.defines.crisis_winner_prestige_factor_base +
											 state.defines.crisis_winner_prestige_factor_year * float(state.current_date.value) / float(365));

			nations::adjust_prestige(state, state.primary_crisis_defender,
					-p_factor * nations::prestige_score(state, state.primary_crisis_attacker));
			nations::adjust_prestige(state, state.primary_crisis_attacker,
					p_factor * nations::prestige_score(state, state.primary_crisis_attacker));

			auto rp_ideology = state.world.nation_get_ruling_party(state.primary_crisis_defender).get_ideology();
			if(rp_ideology) {
				for(auto prv : state.world.nation_get_province_ownership(state.primary_crisis_defender)) {
					auto& cur_loyalty = prv.get_province().get_party_loyalty(rp_ideology);
					prv.get_province().set_party_loyalty(rp_ideology, cur_loyalty * (1.0f - state.defines.party_loyalty_hit_on_war_loss));
				}
			}
		} else {

			float p_factor = 0.05f * (state.defines.crisis_winner_prestige_factor_base +
											 state.defines.crisis_winner_prestige_factor_year * float(state.current_date.value) / float(365));

			nations::adjust_prestige(state, state.primary_crisis_attacker,
					-p_factor * nations::prestige_score(state, state.primary_crisis_attacker));
			nations::adjust_prestige(state, state.primary_crisis_defender,
					p_factor * nations::prestige_score(state, state.primary_crisis_attacker));

			auto rp_ideology = state.world.nation_get_ruling_party(state.primary_crisis_attacker).get_ideology();
			if(rp_ideology) {
				for(auto prv : state.world.nation_get_province_ownership(state.primary_crisis_attacker)) {
					auto& cur_loyalty = prv.get_province().get_party_loyalty(rp_ideology);
					prv.get_province().set_party_loyalty(rp_ideology, cur_loyalty * (1.0f - state.defines.party_loyalty_hit_on_war_loss));
				}
			}
		}
	}

	state.world.peace_offer_set_war_from_war_settlement(offer, dcon::war_id{});
	state.world.peace_offer_set_is_crisis_offer(offer, false);
}

void reject_peace_offer(sys::state& state, dcon::peace_offer_id offer) {
	dcon::nation_id from = state.world.peace_offer_get_nation_from_pending_peace_offer(offer);
	dcon::nation_id target = state.world.peace_offer_get_target(offer);
	notification::message m;

	auto war = state.world.peace_offer_get_war_from_war_settlement(offer);

	if(war) {
		notification::post(state, notification::message{
			[from, target, pa = state.world.war_get_primary_attacker(war), pd = state.world.war_get_primary_defender(war), name = state.world.war_get_name(war), tag = state.world.war_get_over_tag(war), st = state.world.war_get_over_state(war)](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, text::get_adjective(state, pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, text::get_adjective(state, pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_peace_offer_rejected_1", text::variable_type::x, target, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_peace_offer_rejected_title",
			target, from, dcon::nation_id{},
			sys::message_base_type::peace_rejected,
			dcon::province_id{ }
		});
	}

	// TODO: detect
	/*
	If a "good" peace offer is refused, the refusing nation gains define:GOOD_PEACE_REFUSAL_WAREXH war exhaustion and all of its
	pops gain define:GOOD_PEACE_REFUSAL_MILITANCY. What counts as a good offer, well if the peace offer is considered "better"
	than expected. This seems to be a complicated thing to calculate involving: the direction the war is going in (sign of the
	latest war score change), the overall quantity of forces on each side (with navies counting for less), time since the war
	began, war exhaustion, war score, the peace cost of the offer, and whether the recipient will be annexed as a result.
	*/

	state.world.peace_offer_set_war_from_war_settlement(offer, dcon::war_id{});
	state.world.peace_offer_set_is_crisis_offer(offer, false);
}

void update_ticking_war_score(sys::state& state) {
	for(auto wg : state.world.in_wargoal) {
		auto war = wg.get_war_from_wargoals_attached();
		if(!war)
			continue;

		auto attacker_goal = military::is_attacker(state, war, wg.get_added_by());
		auto role = attacker_goal ? war_role::attacker : war_role::defender;

		/*
		#### Occupation score

		Percentage occupied is the share of victory points in the country under occupation.

		Increases by occupation-percentage x define:TWS_FULFILLED_SPEED (up to define:TWS_CB_LIMIT_DEFAULT) when the percentage
		occupied is >= define:TWS_FULFILLED_IDLE_SPACE or when the occupation percentage is > 0 and the current occupation score
		is negative. If there is no occupation, the score decreases by define:TWS_NOT_FULFILLED_SPEED. This can only take the
		score into negative after define:TWS_GRACE_PERIOD_DAYS, at which point it can go to -define:TWS_CB_LIMIT_DEFAULT.
		*/

		auto bits = wg.get_type().get_type_bits();
		static float score = 0;
		if((bits & (cb_flag::po_annex | cb_flag::po_transfer_provinces | cb_flag::po_demand_state)) != 0) {
			// Calculate occupations
			float total_points = 0.0f;
			float occupied = 0.0f;
			if(wg.get_associated_state()) {
				for(auto prv : wg.get_associated_state().get_abstract_state_membership()) {
					if(prv.get_province().get_nation_from_province_ownership() == wg.get_target_nation()) {
						score = (float) province_point_cost(state, prv.get_province(), wg.get_target_nation());
						total_points += score;
						occupied += share_province_score_for_war_occupation(state, war, prv.get_province()) * score;
					}
				}
			} else if((bits & cb_flag::po_annex) != 0) {
				for(auto prv : wg.get_target_nation().get_province_ownership()) {
					score = (float)province_point_cost(state, prv.get_province(), wg.get_target_nation());
					total_points += score;
					occupied += share_province_score_for_war_occupation(state, war, prv.get_province()) * score;
				}
			} else if(auto allowed_states = wg.get_type().get_allowed_states(); allowed_states) {
				auto from_slot = wg.get_secondary_nation().id ? wg.get_secondary_nation().id : wg.get_associated_tag().get_nation_from_identity_holder().id;
				bool is_lib = (bits & cb_flag::po_transfer_provinces) != 0;
				for(auto st : wg.get_target_nation().get_state_ownership()) {
					if(trigger::evaluate(state, allowed_states, trigger::to_generic(st.get_state().id), trigger::to_generic(wg.get_added_by().id), is_lib ? trigger::to_generic(from_slot) : trigger::to_generic(wg.get_added_by().id))) {
						province::for_each_province_in_state_instance(state, st.get_state(), [&](dcon::province_id prv) {
							score = (float)province_point_cost(state, prv, wg.get_target_nation());
							total_points += score;
							occupied += share_province_score_for_war_occupation(state, war, prv) * score;
						});
					}
				}
			}

			// Adjust warscore based on occupation rates
			if(total_points > 0.0f) {
				float fraction = occupied / total_points;
				if(fraction >= state.defines.tws_fulfilled_idle_space || (wg.get_ticking_war_score() < 0 && occupied > 0.0f)) {
					wg.set_ticking_war_score(wg.get_ticking_war_score() + state.defines.tws_fulfilled_speed * fraction);
				} else if(occupied == 0.0f) {
					// Ticking warscore may go into negative only after grace period ends
					// Ticking warscore may drop to zero before grace period ends
					if(wg.get_ticking_war_score() > 0.0f || war.get_start_date() + int32_t(state.defines.tws_grace_period_days) <= state.current_date) {
						wg.set_ticking_war_score(wg.get_ticking_war_score() - state.defines.tws_not_fulfilled_speed);
					}
					}
				}
			}

		// Ticking warscope for make_puppet war
		// If capital of the target nation is occupied - increase ticking warscore
		// If any province of the target nation is occupied - no changes
		// Otherwise, after grace period, decrease ticking warscore
		if((bits & cb_flag::po_make_puppet) != 0 || (bits & cb_flag::po_make_substate) != 0) {
			auto target = wg.get_target_nation().get_capital();
			bool any_occupied = false;
			for(auto prv : wg.get_target_nation().get_province_ownership()) {
				if(!prv.get_province().get_is_colonial() && get_role(state, war, prv.get_province().get_nation_from_province_control()) == role) {
					any_occupied = true;
				}
			}

			if(get_role(state, war, target.get_nation_from_province_control()) == role) {
				wg.set_ticking_war_score(wg.get_ticking_war_score() + state.defines.tws_fulfilled_speed);
			} else if(any_occupied) {
				// We hold some non-colonial province of the target, stay at zero
			}
			// Ticking warscore may go into negative only after grace period ends
			// Ticking warscore may drop to zero before grace period ends
			else if(wg.get_ticking_war_score() > 0.0f || war.get_start_date() + int32_t(state.defines.tws_grace_period_days) <= state.current_date) {
				wg.set_ticking_war_score(wg.get_ticking_war_score() - state.defines.tws_not_fulfilled_speed);
			}
		}

		if(wg.get_type().get_tws_battle_factor() > 0) {

			/*
			#### Battle score

			- zero if fewer than define:TWS_BATTLE_MIN_COUNT have been fought
			- only if the war goal has tws_battle_factor > 0
			- calculate relative losses for each side (something on the order of the difference in losses / 10,000 for land combat
			or the difference in losses / 10 for sea combat) with the points going to the winner, and then take the total of the
			relative loss scores for both sides and divide by the relative loss score for the defender.
			- subtract from tws_battle_factor and then divide by define:TWS_BATTLE_MAX_ASPECT (limited to -1 to +1). This then
			works is the occupied percentage described below.
			*/
			if(war.get_number_of_battles() >= uint16_t(state.defines.tws_battle_min_count)) {

				float ratio = 0.0f;
				if(attacker_goal) {
					ratio = war.get_defender_battle_score() > 0.0f ? war.get_attacker_battle_score() / war.get_defender_battle_score() : 10.0f;
				} else {
					ratio = war.get_attacker_battle_score() > 0.0f ? war.get_defender_battle_score() / war.get_attacker_battle_score() : 10.0f;
				}
				if(ratio >= wg.get_type().get_tws_battle_factor()) {
					auto effective_percentage = std::min(ratio / state.defines.tws_battle_max_aspect, 1.0f);
					wg.set_ticking_war_score(wg.get_ticking_war_score() + state.defines.tws_fulfilled_speed * effective_percentage);
				} else if(ratio <= 1.0f / wg.get_type().get_tws_battle_factor() && ratio > 0.0f) {
					auto effective_percentage = std::min(1.0f / (ratio * state.defines.tws_battle_max_aspect), 1.0f);
					wg.set_ticking_war_score(wg.get_ticking_war_score() - state.defines.tws_fulfilled_speed * effective_percentage);
				} else if(ratio == 0.0f) {
					wg.set_ticking_war_score(wg.get_ticking_war_score() - state.defines.tws_fulfilled_speed);
				}
			}
		}

		// Ticking warscore may not be bigger than 2x the wargoal enforcement cost
		auto max_score = peace_cost(state, war, wg.get_type(), wg.get_added_by(), wg.get_target_nation(), wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag()) * 2.f;
		wg.set_ticking_war_score(std::clamp(wg.get_ticking_war_score(), -float(max_score), float(max_score)));
	}
}

float primary_warscore_from_blockades(sys::state& state, dcon::war_id w) {
	auto pattacker = state.world.war_get_primary_attacker(w);
	auto pdefender = state.world.war_get_primary_defender(w);

	auto d_cpc = state.world.nation_get_central_ports(pdefender);
	int32_t d_blockaded_in_war = 0;
	for(auto p : state.world.nation_get_province_ownership(pdefender)) {
		if(military::province_is_blockaded(state, p.get_province()) && !province::is_overseas(state, p.get_province().id)) {
			for(auto v : state.world.province_get_navy_location(p.get_province().get_port_to())) {
				if(!v.get_navy().get_is_retreating() && !v.get_navy().get_battle_from_navy_battle_participation()) {
					if(military::get_role(state, w, v.get_navy().get_controller_from_navy_control()) == military::war_role::attacker) {
						++d_blockaded_in_war;
						break; // out of inner loop
					}
				}
			}
		}
	}
	auto def_b_frac = std::clamp(d_cpc > 0 ? float(d_blockaded_in_war) / float(d_cpc) : 0.0f, 0.0f, 1.0f);

	int32_t a_blockaded_in_war = 0;
	for(auto p : state.world.nation_get_province_ownership(pattacker)) {
		if(military::province_is_blockaded(state, p.get_province()) && !province::is_overseas(state, p.get_province().id)) {
			for(auto v : state.world.province_get_navy_location(p.get_province().get_port_to())) {
				if(!v.get_navy().get_is_retreating() && !v.get_navy().get_battle_from_navy_battle_participation()) {
					if(military::get_role(state, w, v.get_navy().get_controller_from_navy_control()) == military::war_role::defender) {
						++a_blockaded_in_war;
						break; // out of inner loop
					}
				}
			}
		}
	}
	auto a_cpc = state.world.nation_get_central_ports(pattacker);
	auto att_b_frac = std::clamp(a_cpc > 0 ? float(a_blockaded_in_war) / float(a_cpc) : 0.0f, 0.0f, 1.0f);

	return 25.0f * (def_b_frac - att_b_frac);
}

float primary_warscore(sys::state& state, dcon::war_id w) {
	// return immediately if one side has 100% warscore from occupation (full occupation). This supercedes all else
	auto occupation_score = primary_warscore_from_occupation(state, w);
	if(occupation_score >= 100.0f || occupation_score <= -100.0f) {
		return std::clamp(occupation_score, -100.0f, 100.0f);
	}
	return std::clamp(
		// US5AC4
		occupation_score
		// US5AC1
		+ primary_warscore_from_battles(state, w)
		// US5AC2
		+ primary_warscore_from_blockades(state, w)
		// US5AC3
		+ primary_warscore_from_war_goals(state, w), -100.0f, 100.0f);
}

float primary_warscore_from_occupation(sys::state& state, dcon::war_id w) {
	float total = 0.0f;

	auto pattacker = state.world.war_get_primary_attacker(w);
	auto pdefender = state.world.war_get_primary_defender(w);

	float sum_attacker_prov_values = 0;
	float sum_attacker_occupied_values = 0;
	for(auto prv : state.world.nation_get_province_ownership(pattacker)) {
		auto v = (float) province_point_cost(state, prv.get_province(), pattacker);
		sum_attacker_prov_values += v;
		sum_attacker_occupied_values += share_province_score_for_war_occupation(state, w, prv.get_province()) * v;
	}

	float sum_defender_prov_values = 0;
	float sum_defender_occupied_values = 0;
	for(auto prv : state.world.nation_get_province_ownership(pdefender)) {
		auto v = (float) province_point_cost(state, prv.get_province(), pdefender);
		sum_defender_prov_values += v;
		sum_defender_occupied_values += share_province_score_for_war_occupation(state, w, prv.get_province()) * v;
	}
	// if one side occupues 100% of victory points, then it is 100 warscore no matter what the other side may occupy
	if(sum_defender_prov_values > 0) {
		float defender_total = (sum_defender_occupied_values * 100.0f) / sum_defender_prov_values;
		if(defender_total >= 100.0f) {
			return defender_total;
		}
		else {
			total += defender_total;
		}
	}
		
	if(sum_attacker_prov_values > 0) {
		float attacker_total = (sum_attacker_occupied_values * 100.0f) / sum_attacker_prov_values;
		if(attacker_total >= 100.0f) {
			return -attacker_total;
		}
		else {
			total -= attacker_total;
		}
	}

	return total;
}
float primary_warscore_from_battles(sys::state& state, dcon::war_id w) {
	return std::clamp(state.world.war_get_attacker_battle_score(w) - state.world.war_get_defender_battle_score(w),
			-state.defines.max_warscore_from_battles, state.defines.max_warscore_from_battles);
}
float primary_warscore_from_war_goals(sys::state& state, dcon::war_id w) {
	float total = 0.0f;

	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(is_attacker(state, w, wg.get_wargoal().get_added_by())) {
			total += wg.get_wargoal().get_ticking_war_score();
		} else {
			total -= wg.get_wargoal().get_ticking_war_score();
		}
	}

	return total;
}

float directed_warscore(
	sys::state& state,
	dcon::war_id w,
	dcon::nation_id potential_beneficiary,
	dcon::nation_id target
) {
	float total = 0.0f;

	auto beneficiary_is_primary_attacker = state.world.war_get_primary_attacker(w) == potential_beneficiary;
	auto beneficiary_is_primary_defender = state.world.war_get_primary_defender(w) == potential_beneficiary;

	auto beneficiary_is_war_leader = beneficiary_is_primary_attacker || beneficiary_is_primary_defender;

	auto target_is_primary_attacker = state.world.war_get_primary_attacker(w) == target;
	auto target_is_primary_defender = state.world.war_get_primary_defender(w) == target;

	auto target_is_war_leader = target_is_primary_attacker || target_is_primary_defender;

	if(beneficiary_is_primary_attacker && target_is_primary_defender)
		return primary_warscore(state, w);
	if(target_is_primary_attacker && beneficiary_is_primary_defender)
		return -primary_warscore(state, w);

	float beneficiary_score_from_occupation = 0;
	float beneficiary_potential_score_from_occupation = 0;
	for(auto prv : state.world.nation_get_province_ownership(target)) {
		auto v = (float) province_point_cost(state, prv.get_province(), target);
		beneficiary_potential_score_from_occupation += v;

		if(beneficiary_is_primary_attacker || beneficiary_is_primary_defender) {
			beneficiary_score_from_occupation += share_province_score_for_war_occupation(state, w, prv.get_province()) * v;
		} else {
			if(prv.get_province().get_nation_from_province_control() == potential_beneficiary)
				beneficiary_score_from_occupation += v;
		}
	}

	float against_beneficiary_score_from_occupation = 0;
	float against_beneficiary_potential_score_from_occupation = 0;
	for(auto prv : state.world.nation_get_province_ownership(potential_beneficiary)) {
		auto v = (float) province_point_cost(state, prv.get_province(), potential_beneficiary);
		against_beneficiary_potential_score_from_occupation += v;
		against_beneficiary_score_from_occupation += share_province_score_for_war_occupation(state, w, prv.get_province()) * v;
	}

	// if one country has full occupied the other (100% occupation warscore), then stick return early as it supercedes all alse
	if(beneficiary_potential_score_from_occupation > 0) {
		float beneficiary_occupation_score = (beneficiary_score_from_occupation * 100.0f / beneficiary_potential_score_from_occupation);
		if(beneficiary_occupation_score >= 100.0f) {
			return beneficiary_occupation_score;
		}
		else {
			total += beneficiary_occupation_score;
		}
		
	}
		

	if(against_beneficiary_potential_score_from_occupation > 0) {
		float against_beneficiary_occupation_score = (against_beneficiary_score_from_occupation * 100.0f / against_beneficiary_potential_score_from_occupation);
		if(against_beneficiary_occupation_score >= 100.0f) {
			return -against_beneficiary_occupation_score;
		}
		else {
			total -= against_beneficiary_occupation_score;
		}
	}
		

	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		auto wargoal_is_added_by_beneficiary = wg.get_wargoal().get_added_by() == potential_beneficiary;
		auto wargoal_is_added_by_target = wg.get_wargoal().get_added_by() == target;
		auto wargoal_targets_beneficiary = wg.get_wargoal().get_target_nation() == potential_beneficiary;
		auto wargoal_targets_target = wg.get_wargoal().get_target_nation() == target;

		if(
			(wargoal_is_added_by_beneficiary || beneficiary_is_war_leader) && wargoal_targets_target
		) {
			total += wg.get_wargoal().get_ticking_war_score();
		} else if(
			wargoal_is_added_by_target && (wargoal_targets_beneficiary || beneficiary_is_war_leader)
		) {
			total -= wg.get_wargoal().get_ticking_war_score();
		} else if(
			wargoal_is_added_by_beneficiary	&& (wargoal_targets_target || target_is_war_leader)
		) {
			total += wg.get_wargoal().get_ticking_war_score();
		} else if(
			(wargoal_is_added_by_target || target_is_war_leader) && wargoal_targets_beneficiary
		) {
			total -= wg.get_wargoal().get_ticking_war_score();
		}
	}

	auto d_cpc = state.world.nation_get_central_ports(target);
	int32_t d_blockaded_in_war = 0;
	for(auto p : state.world.nation_get_province_ownership(target)) {
		if(military::province_is_blockaded(state, p.get_province()) && !province::is_overseas(state, p.get_province().id)) {
			for(auto v : state.world.province_get_navy_location(p.get_province().get_port_to())) {
				if(!v.get_navy().get_is_retreating() && !v.get_navy().get_battle_from_navy_battle_participation()) {

					if(beneficiary_is_primary_attacker) {
						if(get_role(state, w, v.get_navy().get_controller_from_navy_control()) == war_role::attacker) {
							++d_blockaded_in_war;
							break; // out of inner loop
						}
					} else if(beneficiary_is_primary_defender) {
						if(get_role(state, w, v.get_navy().get_controller_from_navy_control()) == war_role::defender) {
							++d_blockaded_in_war;
							break; // out of inner loop
						}
					} else {
						if(v.get_navy().get_controller_from_navy_control() == potential_beneficiary) {
							++d_blockaded_in_war;
							break; // out of inner loop
						}
					}
				}
			}
		}
	}
	auto def_b_frac = std::clamp(d_cpc > 0 ? float(d_blockaded_in_war) / float(d_cpc) : 0.0f, 0.0f, 1.0f);

	int32_t a_blockaded_in_war = 0;
	for(auto p : state.world.nation_get_province_ownership(potential_beneficiary)) {
		if(military::province_is_blockaded(state, p.get_province()) && !province::is_overseas(state, p.get_province().id)) {
			for(auto v : state.world.province_get_navy_location(p.get_province().get_port_to())) {
				if(!v.get_navy().get_is_retreating() && !v.get_navy().get_battle_from_navy_battle_participation()) {
					if(target_is_primary_attacker) {
						if(get_role(state, w, v.get_navy().get_controller_from_navy_control()) == war_role::attacker) {
							++a_blockaded_in_war;
							break; // out of inner loop
						}
					} else if(target_is_primary_defender) {
						if(get_role(state, w, v.get_navy().get_controller_from_navy_control()) == war_role::defender) {
							++a_blockaded_in_war;
							break; // out of inner loop
						}
					} else {
						if(v.get_navy().get_controller_from_navy_control() == target) {
							++a_blockaded_in_war;
							break; // out of inner loop
						}
					}
				}
			}
		}
	}
	auto a_cpc = state.world.nation_get_central_ports(potential_beneficiary);
	auto att_b_frac = std::clamp(a_cpc > 0 ? float(a_blockaded_in_war) / float(a_cpc) : 0.0f, 0.0f, 1.0f);

	total += 25.0f * (def_b_frac - att_b_frac);

	return std::clamp(total, 0.0f, 100.0f);
}


bool try_force_peace(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id in_war, dcon::peace_offer_id pending_offer) {
	auto directed_warscore = military::directed_warscore(state, in_war, source, target);
	// check gamerule to see if auto concession peaces can be rejected
	if(gamerule::check_gamerule(state, state.hardcoded_gamerules.auto_concession_peace, uint8_t(gamerule::auto_concession_peace_settings::cannot_reject))) {
		// A concession offer must be accepted when target concedes all wargoals
		if(directed_warscore < 0.0f && state.world.peace_offer_get_is_concession(pending_offer)) {
			if(military::cost_of_peace_offer(state, pending_offer) >= 100) {
				military::implement_peace_offer(state, pending_offer);
				return true;
			}

			auto containseverywargoal = true;
			for(auto poi : state.world.peace_offer_get_peace_offer_item(pending_offer)) {

				auto foundmatch = false;
				for(auto wg : state.world.war_get_wargoals_attached(in_war)) {
					if(wg.get_wargoal().id == poi.get_wargoal().id) {
						foundmatch = true;
						break;
					}
				}

				if(!foundmatch) {
					containseverywargoal = false;
					break;
				}
			}

			if(containseverywargoal) {
				military::implement_peace_offer(state, pending_offer);

				if(target == state.local_player_nation) {
					sound::play_interface_sound(state, sound::get_enemycapitulated_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				}
				return true;
			}
		}
	}

	
	// A peace offer must be accepted when war score reaches 100.
	if(directed_warscore >= 100.0f && (!state.world.nation_get_is_player_controlled(target) || !state.world.peace_offer_get_is_concession(pending_offer)) && military::cost_of_peace_offer(state, pending_offer) <= 100) {
		military::implement_peace_offer(state, pending_offer);

		if(target == state.local_player_nation) {
			sound::play_interface_sound(state, sound::get_wecapitulated_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		}
		return true;
	}
	else {
		return false;
	}
}



void upgrade_ship(sys::state& state, dcon::ship_id ship, dcon::unit_type_id new_type) {
	state.world.ship_set_type(ship, new_type);
	state.world.ship_set_strength(ship, 0.01f);
}
void upgrade_regiment(sys::state& state, dcon::regiment_id ship, dcon::unit_type_id new_type) {
	state.world.regiment_set_type(ship, new_type);
	state.world.regiment_set_strength(ship, 0.01f);
}

bool can_embark_onto_sea_tile(sys::state& state, dcon::nation_id from, dcon::province_id p, dcon::army_id a) {
	int32_t max_cap = 0;
	for(auto n : state.world.province_get_navy_location(p)) {
		if(n.get_navy().get_controller_from_navy_control() == from &&
				!bool(n.get_navy().get_battle_from_navy_battle_participation())) {
			max_cap = std::max(free_transport_capacity(state, n.get_navy()), max_cap);
		}
	}
	auto regs = state.world.army_get_army_membership(a);
	return int32_t(regs.end() - regs.begin()) <= max_cap;
}

dcon::navy_id find_embark_target(sys::state& state, dcon::nation_id from, dcon::province_id p, dcon::army_id a) {
	auto regs = state.world.army_get_army_membership(a);
	int32_t count = int32_t(regs.end() - regs.begin());

	int32_t max_cap = 0;
	for(auto n : state.world.province_get_navy_location(p)) {
		if(n.get_navy().get_controller_from_navy_control() == from) {
			if(free_transport_capacity(state, n.get_navy()) >= count)
				return n.get_navy();
		}
	}
	return dcon::navy_id{};
}

float effective_army_speed(sys::state& state, dcon::army_id a) {
	auto owner = state.world.army_get_controller_from_army_control(a);
	if(!owner)
		owner = state.world.rebel_faction_get_ruler_from_rebellion_within(state.world.army_get_controller_from_army_rebel_control(a));

	float min_speed = 10000.0f;
	for(auto reg : state.world.army_get_army_membership(a)) {
		auto reg_speed = state.world.nation_get_unit_stats(owner, reg.get_regiment().get_type()).maximum_speed;
		min_speed = std::min(min_speed, reg_speed);
	}

	/*
	 slowest ship or regiment x
	 (possibly-some-modifier-for-crossing-water) x (define:LAND_SPEED_MODIFIER or define:NAVAL_SPEED_MODIFIER) x (leader-speed-trait
	 + 1)
	 (Speed bonus from railroads & modifiers are handled by the movement_cost modifier, which is subtracted by the distance later)
	*/
	auto leader = state.world.army_get_general_from_army_leadership(a);
	auto bg = get_leader_background_wrapper(state, leader);
	auto per = get_leader_personality_wrapper(state, leader);
	auto leader_move = state.world.leader_trait_get_speed(bg) + state.world.leader_trait_get_speed(per);
	// US8AC1 Army can be commanded to "Strategic Redeployment" increasing its speed but limiting org to 10%
	// Yes, it applies the infrastructure bonus again
	auto special_order_mod = state.world.army_get_special_order(a) == military::special_army_order::strategic_redeployment ? (state.defines.strategic_redeployment_speed_modifier +
		state.defines.strategic_redeployment_infrastructure_factor * state.world.province_get_building_level(state.world.army_get_location_from_army_location(a), uint8_t(economy::province_building_type::railroad)) *
								state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure) : 1.f;
	return min_speed * (state.world.army_get_is_retreating(a) ? 2.0f : 1.0f) * (leader_move + 1.0f) * special_order_mod * state.defines.land_speed_modifier;
}
float effective_navy_speed(sys::state& state, dcon::navy_id n) {
	auto owner = state.world.navy_get_controller_from_navy_control(n);

	float min_speed = 10000.0f;
	for(auto reg : state.world.navy_get_navy_membership(n)) {
		auto reg_speed = state.world.nation_get_unit_stats(owner, reg.get_ship().get_type()).maximum_speed;
		min_speed = std::min(min_speed, reg_speed);
	}

	auto leader = state.world.navy_get_admiral_from_navy_leadership(n);
	auto bg = get_leader_background_wrapper(state, leader);
	auto per = get_leader_personality_wrapper(state, leader);
	auto leader_move = state.world.leader_trait_get_speed(bg) + state.world.leader_trait_get_speed(per);
	return min_speed * (leader_move + 1.0f);
}

float movement_time_from_to(sys::state& state, dcon::army_id a, dcon::province_id from, dcon::province_id to) {
	auto adj = state.world.get_province_adjacency_by_province_pair(from, to);
	float distance = province::distance_km(state, adj);
	auto controller = state.world.army_get_controller_from_army_control(a);
	// take the average of the modifiers in the two provinces. If the army is "over" a sea prov (naval invading or moving into transports), use 1.0f as the movement_cost.
	float prov_from_mod = from.index() < state.province_definitions.first_sea_province.index() ? province::get_province_modifier_without_hostile_buildings(state, controller, from, sys::provincial_mod_offsets::movement_cost) : 1.0f;
	float prov_to_mod = to.index() < state.province_definitions.first_sea_province.index() ? province::get_province_modifier_without_hostile_buildings(state, controller, to, sys::provincial_mod_offsets::movement_cost) : 1.0f;
	float avg_mods = (prov_from_mod + prov_to_mod) / 2.0f;
	float effective_distance = std::max(0.1f, distance * avg_mods);

	float effective_speed = effective_army_speed(state, a);

	float days = effective_speed > 0.0f ? effective_distance / (effective_speed * state.defines.alice_army_marching_hours_per_day) : 50;
	assert(days > 0.0f);
	return days;
}
float movement_time_from_to(sys::state& state, dcon::navy_id n, dcon::province_id from, dcon::province_id to) {
	auto adj = state.world.get_province_adjacency_by_province_pair(from, to);
	float distance = province::distance_km(state, adj);
	float effective_distance = distance;

	float effective_speed = effective_navy_speed(state, n);

	float days = effective_speed > 0.0f ? effective_distance / (effective_speed * state.defines.alice_navy_sailing_hours_per_day) : 50;
	assert(days > 0.0f);
	return days;
}

arrival_time_info_raw arrival_time_to_in_days(sys::state& state, dcon::army_id a, dcon::province_id to, dcon::province_id from) {
	auto float_days = movement_time_from_to(state, a, from, to);

	float travel_days = std::ceil(float_days);
	float extra_days = travel_days - float_days;

	return arrival_time_info_raw{ .travel_days = int32_t(travel_days), .unused_travel_days = extra_days };
}

arrival_time_info_raw arrival_time_to_in_days(sys::state& state, dcon::navy_id n, dcon::province_id to, dcon::province_id from) {
	auto float_days = movement_time_from_to(state, n, from, to);

	float travel_days = std::ceil(float_days);
	float extra_days = travel_days - float_days;

	return arrival_time_info_raw{ .travel_days = int32_t(travel_days), .unused_travel_days = extra_days };
}

arrival_time_info arrival_time_to(sys::state& state, dcon::army_id a, dcon::province_id p) {

	auto arrival_raw_data = arrival_time_to_in_days(state, a, p, state.world.army_get_location_from_army_location(a));

	return arrival_time_info{ .arrival_time = state.current_date + arrival_raw_data.travel_days, .unused_travel_days = arrival_raw_data.unused_travel_days };
}
arrival_time_info arrival_time_to(sys::state& state, dcon::navy_id n, dcon::province_id p) {
	auto arrival_raw_data = arrival_time_to_in_days(state, n, p, state.world.navy_get_location_from_navy_location(n));


	return arrival_time_info{ .arrival_time = state.current_date + arrival_raw_data.travel_days, .unused_travel_days = arrival_raw_data.unused_travel_days };
}

void add_army_to_battle(sys::state& state, dcon::army_id a, dcon::land_battle_id b, war_role r) {
	assert(state.world.army_is_valid(a));
	assert(state.world.land_battle_is_valid(b));
	bool battle_attacker = (r == war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);
	if(battle_attacker) {
		if(!state.world.land_battle_get_general_from_attacking_general(b)) {
			state.world.land_battle_set_general_from_attacking_general(b, state.world.army_get_general_from_army_leadership(a));
		}

		auto reserves = state.world.land_battle_get_reserves(b);
		for(auto reg : state.world.army_get_army_membership(a)) {

			auto type = state.military_definitions.unit_base_definitions[reg.get_regiment().get_type()].type;
			switch(type) {
			case unit_type::infantry:
			{
				reserves.push_back(
						reserve_regiment{ reg.get_regiment().id, reserve_regiment::is_attacking | reserve_regiment::type_infantry });
				auto& atk_infantry = state.world.land_battle_get_attacker_infantry(b);
				state.world.land_battle_set_attacker_infantry(b, atk_infantry + reg.get_regiment().get_strength());
				break;
			}	
			case unit_type::cavalry:
			{
				reserves.push_back(
						reserve_regiment{ reg.get_regiment().id, reserve_regiment::is_attacking | reserve_regiment::type_cavalry });
				auto& atk_cav = state.world.land_battle_get_attacker_cav(b);
				state.world.land_battle_set_attacker_cav(b, atk_cav + reg.get_regiment().get_strength());
				break;
			}				
			case unit_type::special:
			case unit_type::support:
			{
				reserves.push_back(
						reserve_regiment{ reg.get_regiment().id, reserve_regiment::is_attacking | reserve_regiment::type_support });
				auto& atk_sup = state.world.land_battle_get_attacker_support(b);
				state.world.land_battle_set_attacker_support(b, atk_sup + reg.get_regiment().get_strength());
				break;
			}				
			default:
				assert(false);
			}
		}
	} else {
		auto& def_bonus = state.world.land_battle_get_defender_bonus(b);
		auto prev_dig_in = def_bonus & defender_bonus_dig_in_mask;
		auto new_dig_in = uint8_t(std::min(prev_dig_in, state.world.army_get_dig_in(a) & defender_bonus_dig_in_mask));
		state.world.land_battle_set_defender_bonus(b, uint8_t(def_bonus & ~defender_bonus_dig_in_mask));
		state.world.land_battle_set_defender_bonus(b, uint8_t(def_bonus | new_dig_in));

		if(!state.world.land_battle_get_general_from_defending_general(b)) {
			state.world.land_battle_set_general_from_defending_general(b, state.world.army_get_general_from_army_leadership(a));
		}
		auto reserves = state.world.land_battle_get_reserves(b);
		for(auto reg : state.world.army_get_army_membership(a)) {

			auto type = state.military_definitions.unit_base_definitions[reg.get_regiment().get_type()].type;
			switch(type) {
			case unit_type::infantry:
			{
				reserves.push_back(reserve_regiment{ reg.get_regiment().id, reserve_regiment::type_infantry });

				auto& def_infantry = state.world.land_battle_get_defender_infantry(b);
				state.world.land_battle_set_defender_infantry(b, def_infantry + reg.get_regiment().get_strength());

				break;
			}			
			case unit_type::cavalry:
			{
				reserves.push_back(reserve_regiment{ reg.get_regiment().id, reserve_regiment::type_cavalry });

				auto& def_cav = state.world.land_battle_get_defender_cav(b);
				state.world.land_battle_set_defender_cav(b, def_cav + reg.get_regiment().get_strength());
				break;
			}
			case unit_type::special:
			case unit_type::support:
			{
				reserves.push_back(reserve_regiment{ reg.get_regiment().id, reserve_regiment::type_support });

				auto& def_sup = state.world.land_battle_get_defender_support(b);
				state.world.land_battle_set_defender_support(b, def_sup + reg.get_regiment().get_strength());
				break;
			}		
			default:
				assert(false);
			}
		}
	}

	state.world.army_set_battle_from_army_battle_participation(a, b);
	state.world.army_set_arrival_time(a, sys::date{}); // pause movement
	update_battle_leaders(state, b);
}
template <apply_attrition_on_arrival attrition_tick>
void army_arrives_in_province(sys::state& state, dcon::army_id a, dcon::province_id p, crossing_type crossing, dcon::land_battle_id from) {
	assert(state.world.army_is_valid(a));
	assert(!state.world.army_get_battle_from_army_battle_participation(a));

	state.world.army_set_location_from_army_location(a, p);
	auto regs = state.world.army_get_army_membership(a);
	if(!state.world.army_get_black_flag(a) && !state.world.army_get_is_retreating(a) && regs.begin() != regs.end()) {
		auto owner_nation = state.world.army_get_controller_from_army_control(a);

		// look for existing battle
		for(auto b : state.world.province_get_land_battle_location(p)) {
			if(b.get_battle() != from) {
				auto battle_war = b.get_battle().get_war_from_land_battle_in_war();
				if(battle_war) {
					auto owner_role = get_role(state, battle_war, owner_nation);
					if(owner_role != war_role::none) {
						add_army_to_battle(state, a, b.get_battle(), owner_role);
						return; // done -- only one battle per
					}
				} else { // rebels
					add_army_to_battle(state, a, b.get_battle(), bool(owner_nation) ? war_role::defender : war_role::attacker);
					return;
				}
			}
		}

		// start battle
		dcon::land_battle_id gather_to_battle;
		dcon::war_id battle_in_war;

		for(auto o : state.world.province_get_army_location(p)) {
			if(o.get_army() == a)
				continue;
			if(o.get_army().get_is_retreating() || o.get_army().get_black_flag() || o.get_army().get_navy_from_army_transport() || o.get_army().get_battle_from_army_battle_participation())
				continue;

			auto other_nation = o.get_army().get_controller_from_army_control();

			if(bool(owner_nation) != bool(other_nation)) { // battle vs. rebels
				auto new_battle = fatten(state.world, state.world.create_land_battle());
				new_battle.set_war_attacker_is_attacker(!bool(owner_nation));
				new_battle.set_start_date(state.current_date);
				new_battle.set_location_from_land_battle_location(p);
				new_battle.set_dice_rolls(make_dice_rolls(state, uint32_t(new_battle.id.value)));

				uint8_t flags = defender_bonus_dig_in_mask;
				if(crossing == crossing_type::river)
					flags |= defender_bonus_crossing_river;
				if(crossing == crossing_type::sea)
					flags |= defender_bonus_crossing_sea;
				new_battle.set_defender_bonus(flags);

				auto cw_a = state.defines.base_combat_width +
					state.world.nation_get_modifier_values(owner_nation, sys::national_mod_offsets::combat_width);
				auto cw_b = state.defines.base_combat_width +
					state.world.nation_get_modifier_values(other_nation, sys::national_mod_offsets::combat_width);
				new_battle.set_combat_width(uint8_t(
					std::clamp(int32_t(std::min(cw_a, cw_b) *
						(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::combat_width) + 1.0f)),
						2, 30)));

				add_army_to_battle(state, a, new_battle, !bool(owner_nation) ? war_role::attacker : war_role::defender);
				add_army_to_battle(state, o.get_army(), new_battle, bool(owner_nation) ? war_role::attacker : war_role::defender);

				gather_to_battle = new_battle.id;
				break;
			} else if(auto par = internal_find_war_between(state, owner_nation, other_nation); par.role != war_role::none) {
				auto new_battle = fatten(state.world, state.world.create_land_battle());
				new_battle.set_war_attacker_is_attacker(par.role == war_role::attacker);
				new_battle.set_start_date(state.current_date);
				new_battle.set_war_from_land_battle_in_war(par.w);
				new_battle.set_location_from_land_battle_location(p);
				new_battle.set_dice_rolls(make_dice_rolls(state, uint32_t(new_battle.id.value)));

				uint8_t flags = defender_bonus_dig_in_mask;
				if(crossing == crossing_type::river)
					flags |= defender_bonus_crossing_river;
				if(crossing == crossing_type::sea)
					flags |= defender_bonus_crossing_sea;
				new_battle.set_defender_bonus(flags);

				auto cw_a = state.defines.base_combat_width +
					state.world.nation_get_modifier_values(owner_nation, sys::national_mod_offsets::combat_width);
				auto cw_b = state.defines.base_combat_width +
					state.world.nation_get_modifier_values(other_nation, sys::national_mod_offsets::combat_width);
				new_battle.set_combat_width(uint8_t(
					std::clamp(int32_t(std::min(cw_a, cw_b) *
						(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::combat_width) + 1.0f)),
						2, 30)));

				add_army_to_battle(state, a, new_battle, par.role);
				add_army_to_battle(state, o.get_army(), new_battle, par.role == war_role::attacker ? war_role::defender : war_role::attacker);

				gather_to_battle = new_battle.id;
				battle_in_war = par.w;
				break;
			}

		}

		if(gather_to_battle) {
			for(auto o : state.world.province_get_army_location(p)) {
				if(o.get_army() == a)
					continue;
				if(o.get_army().get_is_retreating() || o.get_army().get_black_flag() || o.get_army().get_navy_from_army_transport() || o.get_army().get_battle_from_army_battle_participation())
					continue;

				auto other_nation = o.get_army().get_controller_from_army_control();
				if(battle_in_war) {
					if(auto role = get_role(state, battle_in_war, other_nation); role != war_role::none) {
						add_army_to_battle(state, o.get_army(), gather_to_battle, role);
					}
				} else { // battle vs. rebels
					add_army_to_battle(state, o.get_army(), gather_to_battle, !bool(other_nation) ? war_role::attacker : war_role::defender);
				}
			}

			if(battle_in_war) { // gather as part of war
				for(auto par : state.world.war_get_war_participant(battle_in_war)) {
					if(par.get_nation().get_is_player_controlled() == false)
						ai::gather_to_battle(state, par.get_nation(), p);
				}
			} else if(state.world.nation_get_is_player_controlled(owner_nation) == false) { // gather vs. rebels
				ai::gather_to_battle(state, owner_nation, p);
			}
		}
	}
	// apply an attrition tick if the flag is set. In vic2, armies take one attrition tick immediately upon arriving in a new province. The apply_attrition_to_army functon will check if it is eligible to take attrition
	if constexpr(attrition_tick == apply_attrition_on_arrival::yes) {
		apply_attrition_to_army(state, a);
		
	}
}

void add_navy_to_battle(sys::state& state, dcon::navy_id n, dcon::naval_battle_id b, war_role r) {
	assert(state.world.navy_is_valid(n));
	bool battle_attacker = (r == war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
	if(battle_attacker) {
		// try add admiral as leader
		if(!state.world.naval_battle_get_admiral_from_attacking_admiral(b)) {
			state.world.naval_battle_set_admiral_from_attacking_admiral(b, state.world.navy_get_admiral_from_navy_leadership(n));
		}
		// put ships in slots
		auto slots = state.world.naval_battle_get_slots(b);
		for(auto ship : state.world.navy_get_navy_membership(n)) {
			if(ship.get_ship().get_strength() <= 0.0f)
				continue;

			auto type = state.military_definitions.unit_base_definitions[ship.get_ship().get_type()].type;
			switch(type) {
			case unit_type::big_ship:
				slots.push_back(ship_in_battle{ ship.get_ship().id, -1,
						naval_battle_distance_to_center | ship_in_battle::mode_seeking | ship_in_battle::is_attacking | ship_in_battle::type_big , 0});
				state.world.naval_battle_get_attacker_big_ships(b)++;
				break;
			case unit_type::light_ship:
				slots.push_back(ship_in_battle{ ship.get_ship().id, -1,
						naval_battle_distance_to_center | ship_in_battle::mode_seeking | ship_in_battle::is_attacking | ship_in_battle::type_small, 0});
				state.world.naval_battle_get_attacker_small_ships(b)++;
				break;
			case unit_type::transport:
				slots.push_back(ship_in_battle{ ship.get_ship().id, -1,
						naval_battle_distance_to_center | ship_in_battle::mode_seeking | ship_in_battle::is_attacking | ship_in_battle::type_transport, 0});
				state.world.naval_battle_get_attacker_transport_ships(b)++;
				break;
			default:
				assert(false);
			}
		}
	} else {
		if(!state.world.naval_battle_get_admiral_from_defending_admiral(b)) {
			state.world.naval_battle_set_admiral_from_defending_admiral(b, state.world.navy_get_admiral_from_navy_leadership(n));
		}
		auto slots = state.world.naval_battle_get_slots(b);
		for(auto ship : state.world.navy_get_navy_membership(n)) {
			if(ship.get_ship().get_strength() <= 0.0f)
				continue;

			auto type = state.military_definitions.unit_base_definitions[ship.get_ship().get_type()].type;
			switch(type) {
			case unit_type::big_ship:
				slots.push_back(ship_in_battle{ ship.get_ship().id, -1, naval_battle_distance_to_center | ship_in_battle::mode_seeking | ship_in_battle::type_big, 0});
				state.world.naval_battle_get_defender_big_ships(b)++;
				break;
			case unit_type::light_ship:
				slots.push_back(ship_in_battle{ ship.get_ship().id, -1, naval_battle_distance_to_center | ship_in_battle::mode_seeking | ship_in_battle::type_small, 0 });
				state.world.naval_battle_get_defender_small_ships(b)++;
				break;
			case unit_type::transport:
				slots.push_back(
						ship_in_battle{ ship.get_ship().id, -1, naval_battle_distance_to_center | ship_in_battle::mode_seeking | ship_in_battle::type_transport, 0});
				state.world.naval_battle_get_defender_transport_ships(b)++;
				break;
			default:
				assert(false);
			}
		}
	}

	state.world.navy_set_battle_from_navy_battle_participation(n, b);

	update_battle_leaders(state, b);
}


std::vector<dcon::nation_id> get_one_side_war_participants(sys::state& state, dcon::war_id war, bool attackers) {
	std::vector<dcon::nation_id> result;
	if(attackers) {
		for(auto wp : state.world.war_get_war_participant(war)) {
			if(wp.get_is_attacker()) {
				result.push_back(wp.get_nation().id);
			}
		}
	}
	else {
		for(auto wp : state.world.war_get_war_participant(war)) {
			if(!wp.get_is_attacker()) {
				result.push_back(wp.get_nation().id);
			}
		}
	}
	return result;
}

template<battle_is_ending battle_state>
bool retreat(sys::state& state, dcon::navy_id n) {
	auto province_start = state.world.navy_get_location_from_navy_location(n);
	auto nation_controller = state.world.navy_get_controller_from_navy_control(n);

	if(!nation_controller)
		return false;
	auto retreat_path = command::can_retreat_from_naval_battle(state, nation_controller, n, true);
	if(retreat_path.size() > 0) {
		state.world.navy_set_is_retreating(n, true);
		auto existing_path = state.world.navy_get_path(n);
		existing_path.load_range(retreat_path.data(), retreat_path.data() + retreat_path.size());

		auto arrival_info = arrival_time_to(state, n, retreat_path.back());
		state.world.navy_set_arrival_time(n, arrival_info.arrival_time);
		state.world.navy_set_unused_travel_days(n, arrival_info.unused_travel_days);

		for(auto em : state.world.navy_get_army_transport(n)) {
			stop_army_movement(state, em.get_army());
		}
		// if the battle isn't about to end, start retreating the ships from the navy
		if constexpr(battle_state == battle_is_ending::no) {
			auto battle = state.world.navy_get_battle_from_navy_battle_participation(n);
			assert(bool(battle));
			for(auto shp : state.world.navy_get_navy_membership(n)) {
				for(auto& s : state.world.naval_battle_get_slots(battle)) {
					if(s.ship == shp.get_ship() && (s.flags & s.mode_mask) != s.mode_sunk && (s.flags & s.mode_mask) != s.mode_retreated) {
						military::single_ship_start_retreat(state, s, battle);
					}
				}
			}
		}
		state.world.navy_set_moving_to_merge(n, false);
		return true;
	} else {
		return false;
	}
}
template bool retreat<battle_is_ending::yes>(sys::state& state, dcon::navy_id n);
template bool retreat<battle_is_ending::no>(sys::state& state, dcon::navy_id n);

bool retreat(sys::state& state, dcon::army_id n) {
	auto province_start = state.world.army_get_location_from_army_location(n);
	auto nation_controller = state.world.army_get_controller_from_army_control(n);

	if(!nation_controller)
		return false; // rebels don't retreat

	auto retreat_path = province::make_land_retreat_path(state, nation_controller, province_start);
	if(retreat_path.size() > 0) {
		state.world.army_set_is_retreating(n, true);
		auto existing_path = state.world.army_get_path(n);
		existing_path.load_range(retreat_path.data(), retreat_path.data() + retreat_path.size());

		auto arrival_info = arrival_time_to(state, n, retreat_path.back());
		state.world.army_set_arrival_time(n, arrival_info.arrival_time);
		state.world.army_set_unused_travel_days(n, arrival_info.unused_travel_days);
		state.world.army_set_dig_in(n, 0);
		return true;
	} else {
		return false;
	}
}

dcon::nation_id get_naval_battle_lead_attacker(sys::state& state, dcon::naval_battle_id b) {
	auto by_admiral =
		state.world.leader_get_nation_from_leader_loyalty(state.world.naval_battle_get_admiral_from_attacking_admiral(b));
	if(by_admiral)
		return by_admiral;

	auto war = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
	bool war_attackers = state.world.naval_battle_get_war_attacker_is_attacker(b);

	for(auto nbp : state.world.naval_battle_get_navy_battle_participation(b)) {
		if(war_attackers && is_attacker(state, war, nbp.get_navy().get_controller_from_navy_control())) {
			return nbp.get_navy().get_controller_from_navy_control();
		} else if(!war_attackers && !is_attacker(state, war, nbp.get_navy().get_controller_from_navy_control())) {
			return nbp.get_navy().get_controller_from_navy_control();
		}
	}

	return dcon::nation_id{};
}

dcon::nation_id get_naval_battle_lead_defender(sys::state& state, dcon::naval_battle_id b) {
	auto by_admiral =
		state.world.leader_get_nation_from_leader_loyalty(state.world.naval_battle_get_admiral_from_defending_admiral(b));
	if(by_admiral)
		return by_admiral;

	auto war = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
	bool war_attackers = state.world.naval_battle_get_war_attacker_is_attacker(b);

	for(auto nbp : state.world.naval_battle_get_navy_battle_participation(b)) {
		if(!war_attackers && is_attacker(state, war, nbp.get_navy().get_controller_from_navy_control())) {
			return nbp.get_navy().get_controller_from_navy_control();
		} else if(war_attackers && !is_attacker(state, war, nbp.get_navy().get_controller_from_navy_control())) {
			return nbp.get_navy().get_controller_from_navy_control();
		}
	}

	return dcon::nation_id{};
}

dcon::nation_id get_land_battle_lead_attacker(sys::state& state, dcon::land_battle_id b) {
	auto by_general =
		state.world.leader_get_nation_from_leader_loyalty(state.world.land_battle_get_general_from_attacking_general(b));
	if(by_general)
		return by_general;

	auto war = state.world.land_battle_get_war_from_land_battle_in_war(b);
	bool war_attackers = state.world.land_battle_get_war_attacker_is_attacker(b);

	if(!war) {
		if(war_attackers)
			return dcon::nation_id{};

		for(auto nbp : state.world.land_battle_get_army_battle_participation(b)) {
			auto c = nbp.get_army().get_controller_from_army_control();
			if(c)
				return c;
		}
	}

	for(auto nbp : state.world.land_battle_get_army_battle_participation(b)) {
		if(war_attackers && is_attacker(state, war, nbp.get_army().get_controller_from_army_control())) {
			return nbp.get_army().get_controller_from_army_control();
		} else if(!war_attackers && !is_attacker(state, war, nbp.get_army().get_controller_from_army_control())) {
			return nbp.get_army().get_controller_from_army_control();
		}
	}

	return dcon::nation_id{};
}

dcon::nation_id get_land_battle_lead_defender(sys::state& state, dcon::land_battle_id b) {
	auto by_general =
		state.world.leader_get_nation_from_leader_loyalty(state.world.land_battle_get_general_from_defending_general(b));
	if(by_general)
		return by_general;

	auto war = state.world.land_battle_get_war_from_land_battle_in_war(b);
	bool war_attackers = state.world.land_battle_get_war_attacker_is_attacker(b);

	if(!war) {
		if(!war_attackers)
			return dcon::nation_id{};

		for(auto nbp : state.world.land_battle_get_army_battle_participation(b)) {
			auto c = nbp.get_army().get_controller_from_army_control();
			if(c)
				return c;
		}
	}

	for(auto nbp : state.world.land_battle_get_army_battle_participation(b)) {
		if(!war_attackers && is_attacker(state, war, nbp.get_army().get_controller_from_army_control())) {
			return nbp.get_army().get_controller_from_army_control();
		} else if(war_attackers && !is_attacker(state, war, nbp.get_army().get_controller_from_army_control())) {
			return nbp.get_army().get_controller_from_army_control();
		}
	}

	return dcon::nation_id{};
}

float get_leader_select_score(sys::state& state, dcon::leader_id l, bool is_attacking) {
	/*
	- Each side has a leader that is in charge of the combat, which is the leader with the greatest
	value as determined by the following formula: (organization x 5 + attack + defend + morale +
	speed + attrition + experience / 2 + reconnaissance / 5  + reliability / 5) x (prestige + 1)
	*/
	auto per = get_leader_personality_wrapper(state, l);
	auto bak = get_leader_background_wrapper(state, l);
	// atk and def are both set to 0 initally, and will be set to its actual amount depending if on the on_attacking input param
	// this makes it so for example if the leader is attacking, it will disregard all "defence" stats for the purposes of calculating the score
	float atk = 0.f;
	float def = 0.f;
	auto org = state.world.leader_trait_get_organisation(per) + state.world.leader_trait_get_organisation(bak);
	if(is_attacking) {
		atk = state.world.leader_trait_get_attack(per) + state.world.leader_trait_get_attack(bak);
	} else {
		def = state.world.leader_trait_get_defense(per) + state.world.leader_trait_get_defense(bak);
	}
	auto spd = state.world.leader_trait_get_speed(per) + state.world.leader_trait_get_speed(bak);
	auto mor = state.world.leader_trait_get_morale(per) + state.world.leader_trait_get_morale(bak);
	auto att = state.world.leader_trait_get_experience(per) + state.world.leader_trait_get_experience(bak);
	auto rel = state.world.leader_trait_get_reliability(per) + state.world.leader_trait_get_reliability(bak);
	auto exp = state.world.leader_trait_get_experience(per) + state.world.leader_trait_get_experience(bak);
	auto rec = state.world.leader_trait_get_reconnaissance(per) + state.world.leader_trait_get_reconnaissance(bak);
	auto lp = state.world.leader_get_prestige(l);
	return (org * 5.f + atk + def + mor + spd + att + exp / 2.f + rec / 5.f + rel / 5.f) * (lp + 1.f);
}

bool is_attacker_in_battle(sys::state& state, dcon::army_id a) {
	assert(state.world.army_get_battle_from_army_battle_participation(a)); // make sure the army is actually in a battle
	auto battle = state.world.army_get_battle_from_army_battle_participation(a);
	auto war = state.world.land_battle_get_war_from_land_battle_in_war(battle);
	auto lead_attacker = get_land_battle_lead_attacker(state, battle);
	auto lead_defender = get_land_battle_lead_defender(state, battle);
	auto thisnation = state.world.army_get_controller_from_army_control(a);
	bool war_attacker = state.world.land_battle_get_war_attacker_is_attacker(battle);
	// country vs country
	if(lead_attacker && lead_defender) {
		for(const auto par : state.world.nation_get_war_participant(thisnation)) {
			if(par.get_war() == war) {
				if((par.get_is_attacker() && war_attacker) || (!par.get_is_attacker() && !war_attacker)) {
					return true;
				} else if((!par.get_is_attacker() && war_attacker) || (par.get_is_attacker() && !war_attacker)) {
					return false;
				}
			}

		}
		return false;
	}
	// country vs rebels
	else {
		// if the "this" nation is the rebels, and they are attacking
		if(!thisnation) {
			return war_attacker;
		}
		// if the "this" nation is not rebels
		else {
			return !war_attacker;
		}
	}
}

bool is_attacker_in_battle(sys::state& state, dcon::navy_id a) {
	assert(state.world.navy_get_battle_from_navy_battle_participation(a)); // make sure the army is actually in a battle
	auto battle = state.world.navy_get_battle_from_navy_battle_participation(a);
	auto war = state.world.naval_battle_get_war_from_naval_battle_in_war(battle);
	auto thisnation = state.world.navy_get_controller_from_navy_control(a);
	// country vs country
	bool war_attacker = state.world.naval_battle_get_war_attacker_is_attacker(battle);
	for(const auto par : state.world.nation_get_war_participant(thisnation)) {
		if(par.get_war() == war) {
			if((par.get_is_attacker() && war_attacker) || (!par.get_is_attacker() && !war_attacker)) {
				return true;
			} else if((!par.get_is_attacker() && war_attacker) || (par.get_is_attacker() && !war_attacker)) {
				return false;
			}
		}
	}
	return false;

}

// this wrapper will also assign the proper background to a no_leader general if detected
dcon::leader_trait_id get_leader_background_wrapper(sys::state& state, dcon::leader_id id) {
	return (bool(id)) ? state.world.leader_get_background(id) : state.military_definitions.first_background_trait;
}
// the wrapper will assign the proper personality to a no_leader general if detected
dcon::leader_trait_id get_leader_personality_wrapper(sys::state& state, dcon::leader_id id) {
	return (bool(id)) ? state.world.leader_get_personality(id) : dcon::leader_trait_id(0);
}


void update_battle_leaders(sys::state& state, dcon::land_battle_id b) {
	/*auto la = get_land_battle_lead_attacker(state, b);*/
	dcon::leader_id a_lid;
	// starting a_score and d_score set to low number, so no-leader won't take over sometimes even if there is a leader available
	float a_score = -999.f;
	/*auto ld = get_land_battle_lead_defender(state, b);*/
	dcon::leader_id d_lid;
	float d_score = -999.f;
	for(const auto a : state.world.land_battle_get_army_battle_participation(b)) {
		auto l = a.get_army().get_general_from_army_leadership();
		// if its no leader, skip
		if(!l.is_valid()) {
			continue;
		}
		bool is_attacking = is_attacker_in_battle(state, a.get_army());
		auto score = get_leader_select_score(state, l, is_attacking);
		/*if(a.get_army().get_controller_from_army_control() == la) {*/
		if(is_attacking) {
			if(score > a_score) {
				a_lid = l;
				a_score = score;
			}
		} /*else if(a.get_army().get_controller_from_army_control() == ld) {*/
		else if(!is_attacking) {
			if(score > d_score) {
				d_lid = l;
				d_score = score;
			}
		}
	}
	auto aa = state.world.land_battle_get_attacking_general(b);
	state.world.attacking_general_set_general(aa, a_lid);
	auto ab = state.world.land_battle_get_defending_general(b);
	state.world.defending_general_set_general(ab, d_lid);
}
void update_battle_leaders(sys::state& state, dcon::naval_battle_id b) {
	/*auto la = get_naval_battle_lead_attacker(state, b);*/
	dcon::leader_id a_lid;
	// starting a_score and d_score set to low number, so no-leader won't take over sometimes even if there is a leader available
	float a_score = -999.f;
	/*auto ld = get_naval_battle_lead_defender(state, b);*/
	dcon::leader_id d_lid;
	float d_score = -999.f;
	for(const auto a : state.world.naval_battle_get_navy_battle_participation(b)) {
		auto l = a.get_navy().get_admiral_from_navy_leadership();
		// if its no leader, skip
		if(!l.is_valid()) {
			continue;
		}
		bool is_attacking = is_attacker_in_battle(state, a.get_navy());
		auto score = get_leader_select_score(state, l, is_attacking);
		if(is_attacking) {
			if(score > a_score) {
				a_lid = l;
				a_score = score;
			}
		} else if(!is_attacking) {
			if(score > d_score) {
				d_lid = l;
				d_score = score;
			}
		}
	}
	auto aa = state.world.naval_battle_get_attacking_admiral(b);
	state.world.attacking_admiral_set_admiral(aa, a_lid);
	auto ab = state.world.naval_battle_get_defending_admiral(b);
	state.world.defending_admiral_set_admiral(ab, d_lid);
}

void delete_regiment_safe_wrapper(sys::state& state, dcon::regiment_id reg) {
	if(state.world.regiment_is_valid(reg)) {
		auto army = state.world.regiment_get_army_from_army_membership(reg);
		auto battle = state.world.army_get_battle_from_army_battle_participation(army);
		if(battle) {
			auto& att_back = state.world.land_battle_get_attacker_back_line(battle);
			auto& att_front = state.world.land_battle_get_attacker_front_line(battle);
			auto& def_back = state.world.land_battle_get_defender_back_line(battle);
			auto& def_front = state.world.land_battle_get_defender_front_line(battle);
			auto reserves = state.world.land_battle_get_reserves(battle);
			bool found = false;
			for(uint32_t j = reserves.size(); j-- > 0;) {
				if(reserves[j].regiment == reg) {
					std::swap(reserves[j], reserves[reserves.size() - 1]);
					reserves.pop_back();
					found = true;
					break;
				}
			}
			if(!found) {
				for(uint32_t i = 0; i < state.world.land_battle_get_combat_width(battle); i++) {
					if(att_back[i] == reg) {
						att_back[i] = dcon::regiment_id{ };
						break;
					}
					if(att_front[i] == reg) {
						att_front[i] = dcon::regiment_id{ };
						break;
					}
					;
					if(def_back[i] == reg) {
						def_back[i] = dcon::regiment_id{ };
						break;
					}
					if(def_front[i] == reg) {
						def_front[i] = dcon::regiment_id{ };
						break;
					}
				}
			}
			
		}
		state.world.delete_regiment(reg);
	}
}

void cleanup_army(sys::state& state, dcon::army_id n) {
	assert(!state.world.army_get_battle_from_army_battle_participation(n));

	auto regs = state.world.army_get_army_membership(n);
	while(regs.begin() != regs.end()) {
		military::delete_regiment_safe_wrapper(state, (*regs.begin()).get_regiment().id);
	}

	auto b = state.world.army_get_battle_from_army_battle_participation(n);
	if(b) {
		state.world.army_set_is_retreating(n, true); // prevents army from re-entering battles

		bool should_end = true;
		auto controller = state.world.army_get_controller_from_army_control(n);
		if(bool(controller)) {
			// TODO: Do they have to be in common war or can they just be "hostile against"?
			bool has_other = false;
			bool has_rebels = false;
			for(auto bp : state.world.land_battle_get_army_battle_participation_as_battle(b)) {
				if(bp.get_army() != n) {
					has_other = true;
					if(are_allied_in_war(state, controller, bp.get_army().get_controller_from_army_control())) {
						should_end = false;
					} else if(bp.get_army().get_controller_from_army_rebel_control()) {
						has_rebels = true;
					}
				}
			}
			// continue fighting rebels
			if(has_other && has_rebels)
				should_end = false;
		} else if(state.world.army_get_controller_from_army_rebel_control(n)) {
			for(auto bp : state.world.land_battle_get_army_battle_participation_as_battle(b)) {
				if(bp.get_army() != n && bp.get_army().get_army_rebel_control()) {
					should_end = false;
				}
			}
		} else {

		}

		if(should_end) {
			bool as_attacker = state.world.land_battle_get_war_attacker_is_attacker(b);
			end_battle(state, b, as_attacker ? battle_result::defender_won : battle_result::attacker_won);
		}
	}

	state.world.delete_army(n);
}

void cleanup_navy(sys::state& state, dcon::navy_id n) {
	assert(!state.world.navy_get_battle_from_navy_battle_participation(n));

	auto shps = state.world.navy_get_navy_membership(n);
	while(shps.begin() != shps.end()) {
		state.world.delete_ship((*shps.begin()).get_ship());
	}
	auto em = state.world.navy_get_army_transport(n);
	while(em.begin() != em.end()) {
		cleanup_army(state, (*em.begin()).get_army());
	}

	auto controller = state.world.navy_get_controller_from_navy_control(n);
	auto b = state.world.navy_get_battle_from_navy_battle_participation(n);

	state.world.navy_set_is_retreating(n, true); // prevents navy from re-entering battles
	if(b && controller) {
		bool should_end = true;
		// TODO: Do they have to be in common war or can they just be "hostile against"?
		for(auto bp : state.world.naval_battle_get_navy_battle_participation_as_battle(b)) {
			if(bp.get_navy() != n && are_allied_in_war(state, controller, state.world.navy_get_controller_from_navy_control(bp.get_navy()))) {
				should_end = false;
			}
		}
		if(should_end) {
			bool as_attacker = state.world.naval_battle_get_war_attacker_is_attacker(b);
			end_battle(state, b, as_attacker ? battle_result::defender_won : battle_result::attacker_won);
		}
	}

	state.world.delete_navy(n);
}

void adjust_leader_prestige(sys::state& state, dcon::leader_id l, float value) {
	// dont adjust prestige to a invalid leader (no leader), as it will be an invalid write
	if(l) {
		auto v = state.world.leader_get_prestige(l);
		v = std::clamp(v + value, 0.f, 1.f); //from 0% to 100%
		state.world.leader_set_prestige(l, v);
	}
	
}

// Won and lost battles give leadership points to highlight the growing experience of the military high command
void adjust_leadership_from_battle(sys::state& state, dcon::nation_id n, float value) {
	state.world.nation_set_leadership_points(n, state.world.nation_get_leadership_points(n) + value);
}

void adjust_regiment_experience(sys::state& state, dcon::nation_id n, dcon::regiment_id l, float value) {
	auto v = state.world.regiment_get_experience(l);

	auto min_exp = std::clamp(state.world.nation_get_modifier_values(n, sys::national_mod_offsets::regular_experience_level) / 100.f, 0.f, 1.f);

	v = std::clamp(v + value, min_exp, 1.f); //from regular_experience_level to 100%

	state.world.regiment_set_experience(l, v);
}
void adjust_ship_experience(sys::state& state, dcon::nation_id n, dcon::ship_id r, float value) {
	auto v = state.world.ship_get_experience(r);

	auto min_exp = std::clamp(state.world.nation_get_modifier_values(n, sys::national_mod_offsets::regular_experience_level) / 100.f, 0.f, 1.f);

	v = std::clamp(v + value * state.defines.exp_gain_div, min_exp, 1.f);
	state.world.ship_set_experience(r, v); //from regular_experience_level to 100%
}


bool nation_participating_in_battle(sys::state& state, dcon::land_battle_id battle, dcon::nation_id nation) {
	assert(state.world.land_battle_is_valid(battle));
	std::vector<dcon::nation_id> participants{};
	for(auto army : state.world.land_battle_get_army_battle_participation(battle)) {
		auto army_controller = state.world.army_get_controller_from_army_control(army.get_army().id);
		if(nation == army_controller) {
			return true;
		}
	}
	return false;
}



bool nation_participating_in_battle(sys::state& state, dcon::naval_battle_id battle, dcon::nation_id nation) {
	assert(state.world.naval_battle_is_valid(battle));
	for(auto navy : state.world.naval_battle_get_navy_battle_participation(battle)) {
		auto navy_controller = state.world.navy_get_controller_from_navy_control(navy.get_navy().id);
		if(nation == navy_controller) {
			return true;
		}
	}
	return false;
}




void end_battle(sys::state& state, dcon::land_battle_id b, battle_result result) {
	auto war = state.world.land_battle_get_war_from_land_battle_in_war(b);
	auto location = state.world.land_battle_get_location_from_land_battle_location(b);

	assert(location);

	auto stackwipe = [&](dcon::army_id a) {
		// disband regiment with pop death when they are being stackwiped, and set the army to be ready for garbage collection
		while(state.world.army_get_army_membership(a).begin() != state.world.army_get_army_membership(a).end()) {
			disband_regiment_w_pop_death<regiment_dmg_source::combat>(state, (*state.world.army_get_army_membership(a).begin()).get_regiment());
		}
		state.world.army_set_controller_from_army_control(a, dcon::nation_id{});
		state.world.army_set_controller_from_army_rebel_control(a, dcon::rebel_faction_id{});
		state.world.army_set_is_retreating(a, true);
		};

	auto a_nation = get_land_battle_lead_attacker(state, b);
	auto d_nation = get_land_battle_lead_defender(state, b);

	for(auto n : state.world.land_battle_get_army_battle_participation(b)) {
		auto nation_owner = state.world.army_get_controller_from_army_control(n.get_army());

		auto role_in_war = bool(war)
			? get_role(state, war, n.get_army().get_controller_from_army_control())
			: (bool(nation_owner) ? war_role::defender : war_role::attacker);

		bool battle_attacker = (role_in_war == war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);


		if(battle_attacker && result == battle_result::defender_won) {
			if(!can_retreat_from_battle(state, b)) {
				stackwipe(n.get_army());
			} else {
				if(!retreat(state, n.get_army()))
					stackwipe(n.get_army());
			}
		} else if(!battle_attacker && result == battle_result::attacker_won) {
			if(!can_retreat_from_battle(state, b)) {
				stackwipe(n.get_army());
			} else {
				if(!retreat(state, n.get_army()))
					stackwipe(n.get_army());
			}
		} else {
			auto path = n.get_army().get_path();
			if(path.size() > 0) {
				// unused travel days is saved from when the army entered the battle
				auto arrival_info = arrival_time_to(state, n.get_army(), path.at(path.size() - 1));
				state.world.army_set_arrival_time(n.get_army(), arrival_info.arrival_time);
			}
		}
	}

	/*
	On finishing a battle:
	Each winning combatant get a random `on_battle_won` event, and each losing combatant gets a random `on_battle_lost` event

	War score is gained based on the difference in losses (in absolute terms) divided by 5 plus 0.1 to a minimum of 0.1
	*/

	for(auto& ref : state.lua_on_battle_end) {
		lua_rawgeti(state.lua_game_loop_environment, LUA_REGISTRYINDEX, ref);
		lua_pushnumber(state.lua_game_loop_environment, b.index());
		lua_pushboolean(state.lua_game_loop_environment, result != battle_result::indecisive);
		lua_pushboolean(state.lua_game_loop_environment, result == battle_result::attacker_won);
		auto pcall_result = lua_pcall(state.lua_game_loop_environment, 3, 0, 0);
		if(pcall_result) {
			state.lua_notification(lua_tostring(state.lua_game_loop_environment, -1));
			lua_settop(state.lua_game_loop_environment, 0);
		}
		assert(lua_gettop(state.lua_game_loop_environment) == 0);
	}

	if(result != battle_result::indecisive) {
		if(war)
			state.world.war_set_number_of_battles(war, uint16_t(state.world.war_get_number_of_battles(war) + 1));

		auto a_leader = state.world.land_battle_get_general_from_attacking_general(b);
		auto b_leader = state.world.land_battle_get_general_from_defending_general(b);

		if(result == battle_result::attacker_won) {
			auto total_def_loss = state.world.land_battle_get_defender_cav_lost(b) + state.world.land_battle_get_defender_infantry_lost(b) + state.world.land_battle_get_defender_support_lost(b);
			auto total_att_loss = state.world.land_battle_get_attacker_cav_lost(b) + state.world.land_battle_get_attacker_infantry_lost(b) + state.world.land_battle_get_attacker_support_lost(b);
			auto score = std::max(0.0f, 3.0f * (total_def_loss - total_att_loss) / 10.0f);

			if(war) {
				if(state.world.land_battle_get_war_attacker_is_attacker(b)) {
					state.world.war_set_attacker_battle_score(war, state.world.war_get_attacker_battle_score(war) + score);
				} else {
					state.world.war_set_defender_battle_score(war, state.world.war_get_defender_battle_score(war) + score);
				}
			}


			if(a_nation && d_nation) { // no prestige for beating up rebels
				nations::adjust_prestige(state, a_nation, score / 50.0f);
				nations::adjust_prestige(state, d_nation, score / -50.0f);

				adjust_leader_prestige(state, a_leader, score / 100.f);
				adjust_leader_prestige(state, b_leader, -score / 100.f);
			}

			// Both victories and losses give leadership points to showcase growing experience of the high command
			if(a_nation)
				adjust_leadership_from_battle(state, a_nation, score / state.defines.alice_battle_won_score_to_leadership);
			if(d_nation)
				adjust_leadership_from_battle(state, d_nation, score / state.defines.alice_battle_lost_score_to_leadership);

			// Report.
			if(nation_participating_in_battle(state, b, state.local_player_nation)) {
				land_battle_report rep;
				rep.attacker_infantry_losses = state.world.land_battle_get_attacker_infantry_lost(b);
				rep.attacker_infantry = state.world.land_battle_get_attacker_infantry(b);
				rep.attacker_cavalry_losses = state.world.land_battle_get_attacker_cav_lost(b);
				rep.attacker_cavalry = state.world.land_battle_get_attacker_cav(b);
				rep.attacker_support_losses = state.world.land_battle_get_attacker_support_lost(b);
				rep.attacker_support = state.world.land_battle_get_attacker_support(b);

				rep.defender_infantry_losses = state.world.land_battle_get_defender_infantry_lost(b);
				rep.defender_infantry = state.world.land_battle_get_defender_infantry(b);
				rep.defender_cavalry_losses = state.world.land_battle_get_defender_cav_lost(b);
				rep.defender_cavalry = state.world.land_battle_get_defender_cav(b);
				rep.defender_support_losses = state.world.land_battle_get_defender_support_lost(b);
				rep.defender_support = state.world.land_battle_get_defender_support(b);

				rep.attacker_won = (result == battle_result::attacker_won);

				rep.attacking_nation = get_land_battle_lead_attacker(state, b);
				rep.defending_nation = get_land_battle_lead_defender(state, b);
				rep.attacking_general = state.world.land_battle_get_general_from_attacking_general(b);
				rep.defending_general = state.world.land_battle_get_general_from_defending_general(b);

				rep.location = state.world.land_battle_get_location_from_land_battle_location(b);
				rep.player_on_winning_side = bool(war)
					? is_attacker(state, war, state.local_player_nation) == state.world.land_battle_get_war_attacker_is_attacker(b)
					: !state.world.land_battle_get_war_attacker_is_attacker(b);

				if(war) {
					if(rep.player_on_winning_side) {
						rep.warscore_effect = score;
						rep.prestige_effect = score / 50.0f;
					} else {
						rep.warscore_effect = -score;
						rep.prestige_effect = -score / 50.0f;
					}
				}

				auto discard = state.land_battle_reports.try_push(rep);
			}

		} else if(result == battle_result::defender_won) {
			auto total_def_loss = state.world.land_battle_get_defender_cav_lost(b) + state.world.land_battle_get_defender_infantry_lost(b) + state.world.land_battle_get_defender_support_lost(b);
			auto total_att_loss = state.world.land_battle_get_attacker_cav_lost(b) + state.world.land_battle_get_attacker_infantry_lost(b) + state.world.land_battle_get_attacker_support_lost(b);
			auto score = std::max(0.0f, 3.0f * (total_att_loss - total_def_loss) / 10.0f);

			if(war) {
				if(state.world.land_battle_get_war_attacker_is_attacker(b)) {
					state.world.war_set_defender_battle_score(war, state.world.war_get_defender_battle_score(war) + score);
				} else {
					state.world.war_set_attacker_battle_score(war, state.world.war_get_attacker_battle_score(war) + score);
				}
			}

			if(a_nation && d_nation) {
				nations::adjust_prestige(state, a_nation, score / -50.0f);
				nations::adjust_prestige(state, d_nation, score / 50.0f);

				adjust_leader_prestige(state, a_leader, -score / 50.f / 100.f);
				adjust_leader_prestige(state, b_leader, score / 50.f / 100.f);
			}

			// Both victories and losses give leadership points to showcase growing experience of the high command
			if(a_nation)
				adjust_leadership_from_battle(state, a_nation, score / state.defines.alice_battle_lost_score_to_leadership);
			if(d_nation)
				adjust_leadership_from_battle(state, d_nation, score / state.defines.alice_battle_won_score_to_leadership);

			// Report
			if(nation_participating_in_battle(state, b, state.local_player_nation)) {
				land_battle_report rep;
				rep.attacker_infantry_losses = state.world.land_battle_get_attacker_infantry_lost(b);
				rep.attacker_infantry = state.world.land_battle_get_attacker_infantry(b);
				rep.attacker_cavalry_losses = state.world.land_battle_get_attacker_cav_lost(b);
				rep.attacker_cavalry = state.world.land_battle_get_attacker_cav(b);
				rep.attacker_support_losses = state.world.land_battle_get_attacker_support_lost(b);
				rep.attacker_support = state.world.land_battle_get_attacker_support(b);

				rep.defender_infantry_losses = state.world.land_battle_get_defender_infantry_lost(b);
				rep.defender_infantry = state.world.land_battle_get_defender_infantry(b);
				rep.defender_cavalry_losses = state.world.land_battle_get_defender_cav_lost(b);
				rep.defender_cavalry = state.world.land_battle_get_defender_cav(b);
				rep.defender_support_losses = state.world.land_battle_get_defender_support_lost(b);
				rep.defender_support = state.world.land_battle_get_defender_support(b);

				rep.attacker_won = (result == battle_result::attacker_won);

				rep.attacking_nation = get_land_battle_lead_attacker(state, b);
				rep.defending_nation = get_land_battle_lead_defender(state, b);
				rep.attacking_general = state.world.land_battle_get_general_from_attacking_general(b);
				rep.defending_general = state.world.land_battle_get_general_from_defending_general(b);

				rep.location = state.world.land_battle_get_location_from_land_battle_location(b);
				rep.player_on_winning_side = bool(war)
					? is_attacker(state, war, state.local_player_nation) != state.world.land_battle_get_war_attacker_is_attacker(b)
					: state.world.land_battle_get_war_attacker_is_attacker(b);

				if(war) {
					if(rep.player_on_winning_side) {
						rep.warscore_effect = score;
						rep.prestige_effect = score / 50.0f;
					} else {
						rep.warscore_effect = -score;
						rep.prestige_effect = -score / 50.0f;
					}
				}
				auto discard = state.land_battle_reports.try_push(rep);
			}
		}
	}


	if(result != battle_result::indecisive) { // so we don't restart battles as the war is ending
		auto par_range = state.world.land_battle_get_army_battle_participation(b);
		while(par_range.begin() != par_range.end()) {
			auto n = (*par_range.begin()).get_army();
			n.set_battle_from_army_battle_participation(dcon::land_battle_id{});
			army_arrives_in_province(state, n, location, crossing_type::none, b);
		}
	}

	state.world.delete_land_battle(b);
}

void end_battle(sys::state& state, dcon::naval_battle_id b, battle_result result) {
	auto war = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
	auto location = state.world.naval_battle_get_location_from_naval_battle_location(b);

	assert(war);
	assert(location);

	auto a_nation = get_naval_battle_lead_attacker(state, b);
	auto d_nation = get_naval_battle_lead_defender(state, b);

	for(auto n : state.world.naval_battle_get_navy_battle_participation(b)) {
		auto role_in_war = get_role(state, war, n.get_navy().get_controller_from_navy_control());
		bool battle_attacker = (role_in_war == war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);


		auto transport_cap = military::free_transport_capacity(state, n.get_navy());
		if(transport_cap < 0) {
			for(auto em : n.get_navy().get_army_transport()) {
				auto em_regs = em.get_army().get_army_membership();
				while(em_regs.begin() != em_regs.end() && transport_cap < 0) {
					auto reg_id = (*em_regs.begin()).get_regiment();
					disband_regiment_w_pop_death<regiment_dmg_source::combat>(state, reg_id);
					++transport_cap;
				}
				if(transport_cap >= 0)
					break;
			}
		}

		// only check if a retreat path can be made. If it can't (no accesible port anywhere) stackwipe them. Navies does not normally get stackwiped if the navy automatically retreats before the battle is retreatable (navies are close enough to the center)
		// They can however, get stackwiped if a retreat path to a accesible port cannot be made
		if(battle_attacker && result == battle_result::defender_won) {
			// If the navy is already in the retreating state (from having manually initiated a retreat earlier) we don't need to try to initiate another retreat.
			if(!state.world.navy_get_is_retreating(n.get_navy()) && state.world.navy_get_path(n.get_navy()).size() == 0) {
				if(!retreat<battle_is_ending::yes>(state, n.get_navy())) {
					n.get_navy().set_controller_from_navy_control(dcon::nation_id{});
					n.get_navy().set_is_retreating(true);
				}
			}
			
			
		}
		else if(!battle_attacker && result == battle_result::attacker_won) {
			if(!state.world.navy_get_is_retreating(n.get_navy()) && state.world.navy_get_path(n.get_navy()).size() == 0) {
				if(!retreat<battle_is_ending::yes>(state, n.get_navy())) {
					n.get_navy().set_controller_from_navy_control(dcon::nation_id{});
					n.get_navy().set_is_retreating(true);
				}
			}
			
			
		}
	}

	/*
	On finishing a naval battle:
	Each winning combatant get a random `on_battle_won` event, and each losing combatant gets a random `on_battle_lost` event.

	Both sides compute their scaled losses fraction, which is (1 + (sum over ship type: supply-consumption-score x strength-losses))
	as a percentage of (total possible naval supply + 1). The scaled losses fraction of the loser / the sum of the scaled losses
	forms the base of the prestige gain for the nation and the leader in charge on the winning side. The winning leader gets that
	value / 100 as added prestige. The winning nations gets (defineLEADER_PRESTIGE_NAVAL_GAIN + 1) x (prestige-from-tech-modifier +
	1) x that value. Similarly, the losing nation and leader have their prestige reduced, calculated in the same way.

	War score is gained based on the difference in losses (in absolute terms) divided by 5 plus 0.1 to a minimum of 0.1
	*/

	if(result != battle_result::indecisive) {
		state.world.war_set_number_of_battles(war, uint8_t(state.world.war_get_number_of_battles(war) + 1));

		auto a_leader = state.world.naval_battle_get_admiral_from_attacking_admiral(b);
		auto b_leader = state.world.naval_battle_get_admiral_from_defending_admiral(b);

		if(result == battle_result::attacker_won) {
			auto score = std::max(0.0f,
					(state.world.naval_battle_get_defender_loss_value(b) - state.world.naval_battle_get_attacker_loss_value(b)) / 10.0f);
			if(state.world.naval_battle_get_war_attacker_is_attacker(b)) {
				state.world.war_set_attacker_battle_score(war, state.world.war_get_attacker_battle_score(war) + score);
			} else {
				state.world.war_set_defender_battle_score(war, state.world.war_get_defender_battle_score(war) + score);
			}


			if(a_nation && d_nation) {
				nations::adjust_prestige(state, a_nation, score / 50.0f);
				nations::adjust_prestige(state, d_nation, score / -50.0f);
				adjust_leader_prestige(state, a_leader, score / 50.f / 100.f);
				adjust_leader_prestige(state, b_leader, score / -50.f / 100.f);

				// Report
				if(nation_participating_in_battle(state, b, state.local_player_nation)) {
					naval_battle_report rep;
					rep.attacker_big_losses = state.world.naval_battle_get_attacker_big_ships_lost(b);
					rep.attacker_big_ships = state.world.naval_battle_get_attacker_big_ships(b);
					rep.attacker_small_losses = state.world.naval_battle_get_attacker_small_ships_lost(b);
					rep.attacker_small_ships = state.world.naval_battle_get_attacker_small_ships(b);
					rep.attacker_transport_losses = state.world.naval_battle_get_attacker_transport_ships_lost(b);
					rep.attacker_transport_ships = state.world.naval_battle_get_attacker_transport_ships(b);

					rep.defender_big_losses = state.world.naval_battle_get_defender_big_ships_lost(b);
					rep.defender_big_ships = state.world.naval_battle_get_defender_big_ships(b);
					rep.defender_small_losses = state.world.naval_battle_get_defender_small_ships_lost(b);
					rep.defender_small_ships = state.world.naval_battle_get_defender_small_ships(b);
					rep.defender_transport_losses = state.world.naval_battle_get_defender_transport_ships_lost(b);
					rep.defender_transport_ships = state.world.naval_battle_get_defender_transport_ships(b);

					rep.attacker_won = (result == battle_result::attacker_won);

					rep.attacking_nation = get_naval_battle_lead_attacker(state, b);
					rep.defending_nation = get_naval_battle_lead_defender(state, b);
					rep.attacking_admiral = state.world.naval_battle_get_admiral_from_attacking_admiral(b);
					rep.defending_admiral = state.world.naval_battle_get_admiral_from_defending_admiral(b);

					rep.location = state.world.naval_battle_get_location_from_naval_battle_location(b);
					rep.player_on_winning_side =
						is_attacker(state, war, state.local_player_nation) == state.world.naval_battle_get_war_attacker_is_attacker(b);

					if(rep.player_on_winning_side) {
						rep.warscore_effect = score;
						rep.prestige_effect = score / 50.0f;
					} else {
						rep.warscore_effect = -score;
						rep.prestige_effect = -score / 50.0f;
					}
					auto discard = state.naval_battle_reports.try_push(rep);
				}
			}
		} else if(result == battle_result::defender_won) {
			auto score = std::max(0.0f,
					(state.world.naval_battle_get_attacker_loss_value(b) - state.world.naval_battle_get_defender_loss_value(b)) / 10.0f);
			if(state.world.naval_battle_get_war_attacker_is_attacker(b)) {
				state.world.war_set_attacker_battle_score(war, state.world.war_get_attacker_battle_score(war) + score);
			} else {
				state.world.war_set_defender_battle_score(war, state.world.war_get_defender_battle_score(war) + score);
			}

			if(a_nation && d_nation) {
				nations::adjust_prestige(state, a_nation, score / -50.0f);
				nations::adjust_prestige(state, d_nation, score / 50.0f);
				adjust_leader_prestige(state, a_leader, score / -50.f / 100.f);
				adjust_leader_prestige(state, b_leader, score / 50.f / 100.f);

				// Report
				if(nation_participating_in_battle(state, b, state.local_player_nation)) {
					naval_battle_report rep;
					rep.attacker_big_losses = state.world.naval_battle_get_attacker_big_ships_lost(b);
					rep.attacker_big_ships = state.world.naval_battle_get_attacker_big_ships(b);
					rep.attacker_small_losses = state.world.naval_battle_get_attacker_small_ships_lost(b);
					rep.attacker_small_ships = state.world.naval_battle_get_attacker_small_ships(b);
					rep.attacker_transport_losses = state.world.naval_battle_get_attacker_transport_ships_lost(b);
					rep.attacker_transport_ships = state.world.naval_battle_get_attacker_transport_ships(b);

					rep.defender_big_losses = state.world.naval_battle_get_defender_big_ships_lost(b);
					rep.defender_big_ships = state.world.naval_battle_get_defender_big_ships(b);
					rep.defender_small_losses = state.world.naval_battle_get_defender_small_ships_lost(b);
					rep.defender_small_ships = state.world.naval_battle_get_defender_small_ships(b);
					rep.defender_transport_losses = state.world.naval_battle_get_defender_transport_ships_lost(b);
					rep.defender_transport_ships = state.world.naval_battle_get_defender_transport_ships(b);

					rep.attacker_won = (result == battle_result::attacker_won);

					rep.attacking_nation = get_naval_battle_lead_attacker(state, b);
					rep.defending_nation = get_naval_battle_lead_defender(state, b);
					rep.attacking_admiral = state.world.naval_battle_get_admiral_from_attacking_admiral(b);
					rep.defending_admiral = state.world.naval_battle_get_admiral_from_defending_admiral(b);

					rep.location = state.world.naval_battle_get_location_from_naval_battle_location(b);
					rep.player_on_winning_side =
						is_attacker(state, war, state.local_player_nation) != state.world.naval_battle_get_war_attacker_is_attacker(b);

					if(rep.player_on_winning_side) {
						rep.warscore_effect = score;
						rep.prestige_effect = score / 50.0f;
					} else {
						rep.warscore_effect = -score;
						rep.prestige_effect = -score / 50.0f;
					}
					auto discard = state.naval_battle_reports.try_push(rep);
				}
			}
		}
	}

	if(result != battle_result::indecisive) { // so we don't restart battles as the war is ending
		auto par_range = state.world.naval_battle_get_navy_battle_participation(b);
		while(par_range.begin() != par_range.end()) {
			auto n = (*par_range.begin()).get_navy();
			n.set_battle_from_navy_battle_participation(dcon::naval_battle_id{});
			navy_arrives_in_province(state, n, location, b);
		}
	}

	state.world.delete_naval_battle(b);
}

inline constexpr float combat_modifier_table[] = { 0.0f, 0.02f, 0.04f, 0.06f, 0.08f, 0.10f, 0.12f, 0.16f, 0.20f, 0.25f, 0.30f,
		0.35f, 0.40f, 0.45f, 0.50f, 0.60f, 0.70f, 0.80f, 0.90f };


template<regiment_dmg_source damage_source>
void regiment_add_pending_damage_safe(sys::state& state, dcon::regiment_id reg, float value) {
	float new_pending_dmg = std::min(state.world.regiment_get_strength(reg), value);
	if constexpr(damage_source == regiment_dmg_source::combat) {
		state.world.regiment_set_pending_combat_damage(reg, state.world.regiment_get_pending_combat_damage(reg) + new_pending_dmg);
	}
	else if constexpr(damage_source == regiment_dmg_source::attrition) {
		state.world.regiment_set_pending_attrition_damage(reg, state.world.regiment_get_pending_attrition_damage(reg) + new_pending_dmg);
	}
}


void reduce_regiment_strength_safe(sys::state& state, dcon::regiment_id reg, float value) {
	if(state.world.regiment_get_strength(reg) >= value) {
		state.world.regiment_set_strength(reg, state.world.regiment_get_strength(reg) - value);
	} else {
		state.world.regiment_set_strength(reg, 0.0f);
	}
}
void reduce_ship_strength_safe(sys::state& state, dcon::ship_id reg, float value) {
	if(state.world.ship_get_strength(reg) >= value) {
		state.world.ship_set_strength(reg, state.world.ship_get_strength(reg) - value);
	} else {
		state.world.ship_set_strength(reg, 0.0f);
	}
}

template<regiment_dmg_source damage_source>
void regiment_take_damage(sys::state& state, dcon::regiment_id reg, float value) {
	regiment_add_pending_damage_safe<damage_source>(state, reg, value);
	reduce_regiment_strength_safe(state, reg, value);
}

dcon::nation_id tech_nation_for_regiment(sys::state& state, dcon::regiment_id r) {
	auto army = state.world.regiment_get_army_from_army_membership(r);
	auto nation = state.world.army_get_controller_from_army_control(army);
	if(nation)
		return nation;
	auto rf = state.world.army_get_controller_from_army_rebel_control(army);
	auto ruler = state.world.rebel_faction_get_ruler_from_rebellion_within(rf);
	if(ruler)
		return ruler;
	return state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
}
dcon::nation_id tech_nation_for_army(sys::state& state, dcon::army_id army) {
	auto nation = state.world.army_get_controller_from_army_control(army);
	if(nation)
		return nation;
	auto rf = state.world.army_get_controller_from_army_rebel_control(army);
	auto ruler = state.world.rebel_faction_get_ruler_from_rebellion_within(rf);
	if(ruler)
		return ruler;
	return state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
}

bool will_recieve_attrition(sys::state& state, dcon::navy_id a) {
	return relative_attrition_amount(state, a, state.world.navy_get_location_from_navy_location(a)) > 0.0f;
}

float peacetime_attrition_limit(sys::state& state, dcon::nation_id n, dcon::province_id prov) {
	auto supply_limit = supply_limit_in_province(state, n, prov);
	auto prov_attrition_mod = state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::attrition);
	auto attrition_mod = 1.0f + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_attrition);

	return (supply_limit + prov_attrition_mod) / (attrition_mod * 3.0f);
}

bool will_recieve_attrition(sys::state& state, dcon::army_id a) {
	auto prov = state.world.army_get_location_from_army_location(a);

	if(prov.index() >= state.province_definitions.first_sea_province.index())
		return true;

	return relative_attrition_amount(state, a, prov) > 0.0f;
}

float relative_attrition_amount(sys::state& state, dcon::navy_id a, dcon::province_id prov) {


	/*Sea attrition is always 0% if adjacent to a land province which is controlled by the navy controller or the navy controller is allied or in a common war with the land province controller.
	If adjacent to a land province and the above isn't the case, the unit will only take attrition if it is outside the supply range decided by define:SUPPLY_RANGE * supply_range modifiers. Any port lvl 1 or higher counts as a supply origin point
	If on the deep ocean (all adjacent provinces are sea), the unit will always take attrition.
	Sea attrition only ticks monthly, and does not tick on province arrival.
	Each valid monthly attrition tick has a base of 1.0% added to (months_outside_naval_range * 2.0%)
	Navies docked in a land province or in battles cannot take monthly attrition
	*/

	auto navy_controller = state.world.navy_get_controller_from_navy_control(a);
	if(bool(state.world.navy_get_battle_from_navy_battle_participation(a))
		|| prov.index() < state.province_definitions.first_sea_province.index()
		|| province::sea_province_is_adjacent_to_accessible_coast(state, prov, navy_controller)) {
		return 0.0f;
	}
	if(province::province_is_deep_waters(state, prov)) {
		return 0.01f + (state.world.navy_get_months_outside_naval_range(a) * 0.02f);
	}

	/*bool any_adj_land_prov = false;
	for(auto adj : state.world.province_get_province_adjacency(prov)) {
		auto indx = adj.get_connected_provinces(0).id != prov ? 0 : 1;
		auto adj_prov = adj.get_connected_provinces(indx);
		if(adj_prov.id.index() < state.province_definitions.first_sea_province.index()) {
			any_adj_land_prov = true;
			auto controller = state.world.province_get_nation_from_province_control(adj_prov);
			if(navy_controller == controller || nations::are_allied(state, navy_controller, controller) || military::are_in_common_war(state, navy_controller, controller)) {
				return 0.0f;
			}
		}
	}
	if(province::sea_province_is_adjacent_to_accessible_coast(state, prov, navy_controller)) {
		return 0.0f;
	}
	if(!any_adj_land_prov) {
		return 0.01f + (state.world.navy_get_months_outside_naval_range(a) * 0.02f);
	}*/
	for(auto p : state.world.nation_get_province_control(navy_controller)) {
		if(auto nb_level = p.get_province().get_building_level(uint8_t(economy::province_building_type::naval_base)); nb_level > 0) {
			auto dist = province::naval_range_distance(state, p.get_province(), prov, navy_controller);
			if(dist.is_reachable && dist.distance <= state.defines.supply_range * (1.0f + state.world.nation_get_modifier_values(navy_controller, sys::national_mod_offsets::supply_range))) {
				return 0.0f;
			}
		}
	}
	return 0.01f + (state.world.navy_get_months_outside_naval_range(a) * 0.02f);
	
}

float sum_army_weight(sys::state& state, const std::vector<dcon::army_id>& armies) {
	float total_army_weight = 0;
	for(auto ar : armies) {
		if(!state.world.army_get_black_flag(ar) && !state.world.army_get_is_retreating(ar) && !bool(state.world.army_get_battle_from_army_battle_participation(ar))) {
			for(auto rg : state.world.army_get_army_membership(ar)) {
				total_army_weight += (state.defines.pop_size_per_regiment / 1000.0f) * rg.get_regiment().get_strength();
			}
		}
	}
	return total_army_weight;
}

float local_army_weight(sys::state& state, dcon::province_id prov) {
	float total_army_weight = 0;
	for(auto ar : state.world.province_get_army_location(prov)) {
		if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false) {
			for(auto rg : ar.get_army().get_army_membership()) {
				total_army_weight += (state.defines.pop_size_per_regiment / 1000.0f) * rg.get_regiment().get_strength();
			}
		}
	}
	return total_army_weight;
}
float local_army_weight_max(sys::state& state, dcon::province_id prov) {
	float total_army_weight = 0;
	for(auto ar : state.world.province_get_army_location(prov)) {
		if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false) {
			for(auto rg : ar.get_army().get_army_membership()) {
				total_army_weight += (state.defines.pop_size_per_regiment / 1000.0f);
			}
		}
	}
	return total_army_weight;
}
float local_enemy_army_weight_max(sys::state& state, dcon::province_id prov, dcon::nation_id nation) {
	float total_army_weight = 0;
	for(auto ar : state.world.province_get_army_location(prov)) {
		if(
			ar.get_army().get_black_flag() == false
			&& ar.get_army().get_is_retreating() == false
			&& are_at_war(state, nation, ar.get_army().get_controller_from_army_control())
		) {
			for(auto rg : ar.get_army().get_army_membership()) {
				total_army_weight += (state.defines.pop_size_per_regiment / 1000.0f);
			}
		}
	}
	return total_army_weight;
}

float relative_attrition_amount(sys::state& state, dcon::army_id a, dcon::province_id prov, const std::vector<dcon::army_id>& extra_armies) {
	return relative_attrition_amount(state, a, prov, sum_army_weight(state, extra_armies));
}

float relative_attrition_amount(sys::state& state, dcon::army_id a, dcon::province_id prov, float additional_army_weight) {
	// an army cannot take attrition if it is blackflagged, retreating, or in a battle
	if(state.world.army_get_black_flag(a) || state.world.army_get_is_retreating(a) || bool(state.world.army_get_battle_from_army_battle_participation(a))) {
		return 0.0f;
	}
	auto ar = fatten(state.world, a);
	auto army_controller = ar.get_controller_from_army_control();
	// if sea province, use the sea transport attrition, and apply national attrition modifiers
	if(prov.index() >= state.province_definitions.first_sea_province.index()) {
		auto value = state.defines.alice_army_sea_transport_attrition * (1.0f + army_controller.get_modifier_values(sys::national_mod_offsets::land_attrition));
		return value * 0.01f;
	}

	/*
	First we calculate (total-strength / define:POP_SIZE_PER_REGIMENT) x MAX(national-attrition-modifier + province-attrition-modifier + 1, with a maximum value of 0) -
	effective-province-supply-limit (rounded down to the nearest integer). We then reduce that value to at most the max-attrition
	modifier of the province, and finally we add (define:SEIGE_ATTRITION + fort-level * define:ALICE_FORT_SIEGE_ATTRITION_PER_LEVEL) if the army is conducting a siege. Units taking
	attrition lose max-strength x attrition-value x 0.01 points of strength. This strength loss is treated just like damage
	taken in combat, meaning that it will reduce the size of the backing pop.
	*/

	float total_army_weight = local_army_weight(state, prov) + additional_army_weight;

	auto supply_limit = supply_limit_in_province(state, army_controller, prov);
	auto attrition_mods = std::max( 1.0f + army_controller.get_modifier_values(sys::national_mod_offsets::land_attrition) + state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::attrition), 0.0f);

	float hostile_fort = state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::fort));

	auto max_attrition = std::max(0.f, state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::max_attrition));

	float siege_attrition = 0.0f;
	if(state.world.province_get_siege_progress(prov) > 0.f) {
		siege_attrition = state.defines.siege_attrition + hostile_fort * state.defines.alice_fort_siege_attrition_per_level;
	}

	// Multiplying army weight by local attrition modifier (often coming from terrain) wasn't a correct approach
	auto value = std::clamp((total_army_weight - supply_limit) * attrition_mods, 0.0f, max_attrition) + siege_attrition;
	return value * 0.01f;
}
float attrition_amount(sys::state& state, dcon::navy_id a) {
	return relative_attrition_amount(state, a, state.world.navy_get_location_from_navy_location(a));
}
float attrition_amount(sys::state& state, dcon::army_id a) {
	return relative_attrition_amount(state, a, state.world.army_get_location_from_army_location(a));
}

void apply_attrition_to_army(sys::state& state, dcon::army_id army) {
	auto prov = state.world.army_get_location_from_army_location(army);
	float attrition_value = relative_attrition_amount(state, army, prov);
	if(attrition_value == 0.0f) {
		return;
	}
	for(auto rg : state.world.army_get_army_membership(army)) {
		military::regiment_take_damage<military::regiment_dmg_source::attrition>(state, rg.get_regiment(), attrition_value);
	}
}
void apply_monthly_attrition_to_navy(sys::state& state, dcon::navy_id navy) {
	auto prov = state.world.navy_get_location_from_navy_location(navy);
	float attrition_value = relative_attrition_amount(state, navy, prov);
	if(attrition_value == 0.0f) {
		return;
	}
	for(auto ship : state.world.navy_get_navy_membership(navy)) {
		reduce_ship_strength_safe(state, ship.get_ship(), attrition_value);
	}
	// increment months outside naval range counter. Check to avoid overflow in egde cases
	auto current = state.world.navy_get_months_outside_naval_range(navy);
	if(current != std::numeric_limits<uint8_t>().max()) {
		state.world.navy_set_months_outside_naval_range(navy, uint8_t(current + 1));
	}
}

void apply_attrition(sys::state& state) {

	concurrency::parallel_for(uint32_t(0), state.world.province_size(), [&](int32_t i) {
		dcon::province_id prov{ dcon::province_id::value_base_t(i) };
		assert(state.world.province_is_valid(prov));

		for(auto ar : state.world.province_get_army_location(prov)) {
			apply_attrition_to_army(state, ar.get_army());
		}
		for(auto nv : state.world.province_get_navy_location(prov)) {
			apply_monthly_attrition_to_navy(state, nv.get_navy());
		}
	});
}

void apply_regiment_damage(sys::state& state) {
	for(uint32_t i = state.world.regiment_size(); i-- > 0;) {
		dcon::regiment_id s{ dcon::regiment_id::value_base_t(i) };
		if(state.world.regiment_is_valid(s)) {
			auto& pending_combat_damage = state.world.regiment_get_pending_combat_damage(s);
			auto& pending_attrition_damage = state.world.regiment_get_pending_attrition_damage(s);
			auto& current_strength = state.world.regiment_get_strength(s);
			auto backing_pop = state.world.regiment_get_pop_from_regiment_source(s);
			auto in_nation = state.world.army_get_controller_from_army_control(state.world.regiment_get_army_from_army_membership(s));

		
			if(pending_combat_damage > 0) {
				auto tech_nation = tech_nation_for_regiment(state, s);
				if(bool(in_nation)) {
					// give war exhaustion for the losses
					auto& current_war_ex = state.world.nation_get_war_exhaustion(in_nation);
					auto extra_war_ex = get_war_exhaustion_from_land_losses<regiment_dmg_source::combat>(state, pending_combat_damage, in_nation);
					state.world.nation_set_war_exhaustion(in_nation, std::min(current_war_ex + extra_war_ex, state.world.nation_get_modifier_values(in_nation, sys::national_mod_offsets::max_war_exhaustion)));
				}
				if(backing_pop) {
					auto& psize = state.world.pop_get_size(backing_pop);
					float damage_modifier = std::max(state.defines.soldier_to_pop_damage - state.world.nation_get_modifier_values(tech_nation, sys::national_mod_offsets::soldier_to_pop_loss), 0.0f);
					state.world.pop_set_size(backing_pop, psize - state.defines.pop_size_per_regiment * pending_combat_damage * damage_modifier);
				}
				state.world.regiment_set_pending_combat_damage(s, 0.0f);
			}
			if(pending_attrition_damage > 0) {
				auto tech_nation = tech_nation_for_regiment(state, s);
				if(bool(in_nation)) {
					// give war exhaustion for the losses
					auto& current_war_ex = state.world.nation_get_war_exhaustion(in_nation);
					auto extra_war_ex = get_war_exhaustion_from_land_losses<regiment_dmg_source::attrition>(state, pending_attrition_damage, in_nation);
					state.world.nation_set_war_exhaustion(in_nation, std::min(current_war_ex + extra_war_ex, state.world.nation_get_modifier_values(in_nation, sys::national_mod_offsets::max_war_exhaustion)));
				}
				if(backing_pop) {
					auto& psize = state.world.pop_get_size(backing_pop);
					float damage_modifier = std::max(state.defines.soldier_to_pop_damage - state.world.nation_get_modifier_values(tech_nation, sys::national_mod_offsets::soldier_to_pop_loss), 0.0f);
					state.world.pop_set_size(backing_pop, psize - state.defines.pop_size_per_regiment * pending_attrition_damage * damage_modifier);
				}
				state.world.regiment_set_pending_attrition_damage(s, 0.0f);
			}
			
			auto psize = state.world.pop_get_size(backing_pop);
			// Check if the regiment has no attached pop without having been deleted (from demotion, migration etc).
			// The find soldier function cannot find a pop for an invalid nation id (rebel armies) so it will take care of that
			if(!bool(backing_pop)) {
				// try to find a new pop to replace the old. if not possible, then delete the regiment
				auto new_pop = find_available_soldier_anywhere(state, in_nation, state.world.regiment_get_type(s));
				if(bool(new_pop)) {
					state.world.try_create_regiment_source(s, new_pop);
				}
				else {
					military::delete_regiment_safe_wrapper(state, s);
				}
			}
			else if(psize <= 1.0f) {
				// try to find a new pop
				auto new_pop = find_available_soldier_anywhere(state, in_nation, state.world.regiment_get_type(s));
				if(bool(new_pop)) {
					state.world.try_create_regiment_source(s, new_pop);
				}
				else {
					military::delete_regiment_safe_wrapper(state, s);
				}
				state.world.delete_pop(backing_pop);
			}
		}
	}
}

uint16_t unit_type_to_reserve_regiment_type(unit_type utype) {
	switch(utype) {
	case unit_type::infantry:
		return reserve_regiment::type_infantry;
	case unit_type::support:
	case unit_type::special:
		return reserve_regiment::type_support;
	case unit_type::cavalry:
		return reserve_regiment::type_cavalry;
	default:
		assert(false);
	}
	return reserve_regiment::type_infantry;
}

uint32_t get_reserves_count_by_side(sys::state& state, dcon::land_battle_id b, bool attacker) {
	uint32_t count = 0;
	auto reserves = state.world.land_battle_get_reserves(b);
	for(uint32_t i = 0; i < reserves.size(); i++) {
		bool battle_attacker = (reserves[i].flags & reserve_regiment::is_attacking) != 0;
		if((battle_attacker && attacker) || (!battle_attacker && !attacker)) {
			count++;
		}
	}
	return count;
}

void add_regiment_to_reserves(sys::state& state, dcon::land_battle_id bat, dcon::regiment_id reg, bool is_attacking) {
	auto reserves = state.world.land_battle_get_reserves(bat);
	auto type = state.military_definitions.unit_base_definitions[state.world.regiment_get_type(reg)].type;
	uint16_t reserve_type = unit_type_to_reserve_regiment_type(type);
	uint16_t bitmask = reserve_type;
	if(is_attacking) {
		bitmask = bitmask | reserve_regiment::is_attacking;
	}
	reserves.push_back(reserve_regiment{ reg,  bitmask });

}

bool is_regiment_in_reserve(sys::state& state, dcon::regiment_id reg) {
	auto army = state.world.regiment_get_army_from_army_membership(reg);
	auto bat = state.world.army_get_battle_from_army_battle_participation(army);
	assert(bat);
	auto reserves = state.world.land_battle_get_reserves(bat);
	for(uint32_t i = reserves.size(); i-- > 0;) {
		if(reserves[i].regiment == reg) {
			return true;
		}
	}
	return false;
}
// gets the effective default organization of a regiment (ie max org, based on techs and leading general)
float unit_get_effective_default_org(sys::state& state, dcon::regiment_id reg) {
	auto army = state.world.regiment_get_army_from_army_membership(reg);
	auto type = state.world.regiment_get_type(reg);
	auto base_org = state.world.nation_get_unit_stats(tech_nation_for_regiment(state, reg), type).default_organisation;
	auto leader = state.world.army_get_general_from_army_leadership(army);
	auto leader_bg = get_leader_background_wrapper(state, leader);
	auto leader_per = get_leader_personality_wrapper(state, leader);
	return base_org * ((1.0f + state.world.leader_trait_get_organisation(leader_bg) + state.world.leader_trait_get_organisation(leader_per)) *
		   (1.0f + state.world.leader_get_prestige(leader) * state.defines.leader_prestige_to_max_org_factor));
}

// gets the effective default organization of a ship (ie max org, based on techs and leading admiral)
float unit_get_effective_default_org(sys::state& state, dcon::ship_id ship) {
	auto navy = state.world.ship_get_navy_from_navy_membership(ship);
	auto owner = state.world.navy_get_controller_from_navy_control(navy);
	auto type = state.world.ship_get_type(ship);
	auto base_org = state.world.nation_get_unit_stats(state.world.navy_get_controller_from_navy_control(navy), type).default_organisation;
	auto leader = state.world.navy_get_admiral_from_navy_leadership(navy);
	auto leader_bg = get_leader_background_wrapper(state, leader);
	auto leader_per = get_leader_personality_wrapper(state, leader);
	return base_org * ((1.0f + state.world.leader_trait_get_organisation(leader_bg) + state.world.leader_trait_get_organisation(leader_per)) *
		   (1.0f + state.world.leader_get_prestige(leader) * state.defines.leader_prestige_to_max_org_factor) * (1.0f + state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::naval_organisation)));
}

// implementation of what it sorts by can change, for now it sorts by strength and puts the highest str brigades in front of the queue
void sort_reserves_by_deployment_order(sys::state& state, dcon::dcon_vv_fat_id<reserve_regiment> reserves) {
	std::sort(reserves.begin(), reserves.end(), [&state](reserve_regiment a, reserve_regiment b) { return state.world.regiment_get_strength(b.regiment) > state.world.regiment_get_strength(a.regiment); });
}
// gets the recon efficiency of an army, for display in ui mostly
float get_army_recon_eff(sys::state& state, dcon::army_id army) {
	dcon::nation_id tech_nation = tech_nation_for_army(state, army);
	float total_strength = 0;
	float total_recon_strength = 0;
	for(auto r : state.world.army_get_army_membership(army)) {
		auto reg = r.get_regiment();
		if(reg.get_strength() > 0.0f) {
			total_strength += reg.get_strength();
			float recon = state.world.nation_get_unit_stats(tech_nation, reg.get_type()).reconnaissance_or_fire_range;
			if(recon > 0.0f) {
				total_recon_strength += reg.get_strength();
			}
		}
	}
	// guard for DBZ errors if this is run on an army that has no regiments, before it is disbanded.
	if(total_strength == 0) {
		return 0;
	}
	return std::clamp((total_recon_strength / total_strength) / state.defines.recon_unit_ratio, 0.0f, 1.0f);
}

// gets the siege efficiency of an army, for display in ui mostly
float get_army_siege_eff(sys::state& state, dcon::army_id army) {
	dcon::nation_id tech_nation = tech_nation_for_army(state, army);
	float total_strength = 0;
	float total_siege_strength = 0;
	for(auto r : state.world.army_get_army_membership(army)) {
		auto reg = r.get_regiment();
		if(reg.get_strength() > 0.0f) {
			total_strength += reg.get_strength();
			float siege = state.world.nation_get_unit_stats(tech_nation, reg.get_type()).siege_or_torpedo_attack;
			if(siege > 0.0f) {
				total_siege_strength += reg.get_strength();
			}
		}
	}
	// guard for DBZ errors if this is run on an army that has no regiments, before it is disbanded.
	if(total_strength == 0) {
		return 0;
	}
	return std::clamp((total_siege_strength / total_strength) / state.defines.engineer_unit_ratio, 0.0f, 1.0f);
}

// calculates the effective digin of a battle after recon units are taken into account.
uint8_t get_effective_battle_dig_in(sys::state& state, dcon::land_battle_id battle) {
	float total_attacking_strength = 0;
	float total_recon_strength = 0;
	float highest_recon = 0;
	uint8_t current_dig_in = state.world.land_battle_get_defender_bonus(battle) & defender_bonus_dig_in_mask;
	for(auto a : state.world.land_battle_get_army_battle_participation(battle)) {
		auto army = a.get_army();
		dcon::nation_id tech_nation = tech_nation_for_army(state, army);
		if(is_attacker_in_battle(state, army)) {
			for(auto r : state.world.army_get_army_membership(army)) {
				auto reg = r.get_regiment();
				if(reg.get_strength() > 0.0f) {
					total_attacking_strength += reg.get_strength();
					float recon = state.world.nation_get_unit_stats(tech_nation, reg.get_type()).reconnaissance_or_fire_range;
					if(recon > 0.0f) {
						highest_recon = std::max(recon, highest_recon);
						total_recon_strength += reg.get_strength();
					}
				}
			}
		}
	}
	// if there is 0 attacking strength left the day before the battle ends, return early to avoid DBZ error
	if(total_attacking_strength == 0) {
		return current_dig_in;
	}
	return uint8_t( current_dig_in / (1 + (highest_recon * std::min(total_recon_strength / total_attacking_strength, state.defines.recon_unit_ratio) / state.defines.recon_unit_ratio)));

}

// gets the land combat target of a regiment in a battle, given its combat width position and the opposing frontline. Will return a regiment id 0 if it is unable to taget any regiment
dcon::regiment_id get_land_combat_target(sys::state& state, dcon::regiment_id damage_dealer, int32_t position, const std::array<dcon::regiment_id, 30>& opposing_line)
{
	auto tech_nation = tech_nation_for_regiment(state, damage_dealer);
	const auto& stats = state.world.nation_get_unit_stats(tech_nation, state.world.regiment_get_type(damage_dealer));
	dcon::regiment_id target = opposing_line[position];
	if(auto mv = stats.maneuver; !target && mv > 0.0f) {
		// special case if combat witdh positon (i) is 1 and maneuve is 1 or higher, if that is the case i - cnt * 2 = -1 which would be negative instead of targeting position 0
		if(position == 1 && mv >= 1.0f && opposing_line[0]) {
			target = opposing_line[0];
		} else {
			for(int32_t cnt = 1; position - cnt * 2 >= 0 && cnt <= int32_t(mv); ++cnt) {
				if(opposing_line[position - cnt * 2]) {
					target = opposing_line[position - cnt * 2];
					break;
				}
			}
		}
	}
	return target;
}

// caluclates expected strength damage, has no side effects
float get_reg_str_damage(sys::state& state, dcon::regiment_id damage_dealer, dcon::regiment_id damage_receiver, float battle_modifiers, bool backline, bool attacker, float fort_mod = 1.0f) {
	auto dmg_dealer_tech_nation = tech_nation_for_regiment(state, damage_dealer);
	auto dmg_receiver_tech_nation = tech_nation_for_regiment(state, damage_receiver);
	auto dmg_dealer_str = state.world.regiment_get_strength(damage_dealer);
	auto receiver_exp = state.world.regiment_get_experience(damage_receiver);
	const auto& dmg_dealer_stats = state.world.nation_get_unit_stats(dmg_dealer_tech_nation, state.world.regiment_get_type(damage_dealer));
	float unit_dmg_stat;
	float unit_dmg_support;
	// if the damage deal is attacker
	if(attacker) {
		unit_dmg_stat = dmg_dealer_stats.attack_or_gun_power;
	}
	// if the damage deal is defender
	else {
		unit_dmg_stat = dmg_dealer_stats.defence_or_hull;
		// if the damage dealer is defending, set fort mod to 1;
		fort_mod = 1.f;
	}
	// if the damage dealer is in the backline (can use support)
	if(backline) {
		unit_dmg_support = dmg_dealer_stats.support;
	}
	// if the dmg dealer is frontline
	else {
		unit_dmg_support = 1.f;
	}

	return dmg_dealer_str * str_dam_mul *(unit_dmg_stat * 0.1f + 1.0f) * unit_dmg_support * battle_modifiers / (fort_mod * (state.defines.base_military_tactics + state.world.nation_get_modifier_values(dmg_receiver_tech_nation, sys::national_mod_offsets::military_tactics)) * (1 + receiver_exp));
}
// caluclates expected org damage, has no side effects
float get_reg_org_damage(sys::state& state, dcon::regiment_id damage_dealer, dcon::regiment_id damage_receiver, float battle_modifiers, bool backline, bool attacker, float fort_mod = 1.0f) {
	auto dmg_dealer_tech_nation = tech_nation_for_regiment(state, damage_dealer);
	auto dmg_receiver_tech_nation = tech_nation_for_regiment(state, damage_receiver);
	auto dmg_dealer_str = state.world.regiment_get_strength(damage_dealer);
	auto receiver_exp = state.world.regiment_get_experience(damage_receiver);
	const auto& dmg_dealer_stats = state.world.nation_get_unit_stats(dmg_dealer_tech_nation, state.world.regiment_get_type(damage_dealer));
	const auto& dmg_receiver_stats = state.world.nation_get_unit_stats(dmg_receiver_tech_nation, state.world.regiment_get_type(damage_receiver));

	auto receiver_max_org_divisor = unit_get_effective_default_org(state, damage_receiver) / 30;

	float unit_dmg_stat;
	float unit_dmg_support;
	// if the damage deal is attacker
	if(attacker) {
		unit_dmg_stat = dmg_dealer_stats.attack_or_gun_power;
	}
	// if the damage deal is defender
	else {
		unit_dmg_stat = dmg_dealer_stats.defence_or_hull;
		// if the damage dealer is defending, set fort mod to 1;
		fort_mod = 1.f;
	}
	// if the damage dealer is in the backline (can use support)
	if(backline) {
		unit_dmg_support = dmg_dealer_stats.support;
	}
	// if the dmg dealer is frontline
	else {
		unit_dmg_support = 1.f;
	}
	return dmg_dealer_str * (org_dam_mul / receiver_max_org_divisor) * (unit_dmg_stat * 0.1f + 1.0f) * unit_dmg_support * battle_modifiers / (fort_mod * dmg_receiver_stats.discipline_or_evasion * (1.0f + state.world.nation_get_modifier_values(dmg_receiver_tech_nation, sys::national_mod_offsets::land_organisation)) * (1.0f + receiver_exp));
}
// defines the general algorithm for getting the effective fort level with said amount of total strength of units who are enemies with the fort controller,
// total strength of siege units who are enemies with the fort controller, and the highest siege stat among them.
int32_t get_effective_fort_level(sys::state& state, dcon::province_id location, float total_strength, float strength_siege_units, float max_siege_value) {
	/*
		We find the effective level of the fort by subtracting: (rounding this value down to to the nearest integer)
		greatest-siege-value-present x
		((the ratio of the strength of regiments with siege present to the total strength of all regiments) ^
		define:ENGINEER_UNIT_RATIO) / define:ENGINEER_UNIT_RATIO, reducing it to a minimum of 0.
		*/

	return std::clamp(int32_t(state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::fort_level)) -
									 int32_t(max_siege_value *
										 std::min(strength_siege_units / total_strength, state.defines.engineer_unit_ratio) /
										 state.defines.engineer_unit_ratio),
				0, 10);

}
// gets the fort level for combat in the specified province, taking all of the units in the province into account
int32_t get_combat_fort_level(sys::state& state, dcon::province_id location) {
	auto location_controller = state.world.province_get_nation_from_province_control(location);
	float total_enemy_strength = 0;
	float strength_siege_units = 0;
	float max_siege_value = 0;
	for(auto ar : state.world.province_get_army_location(location)) {
		auto army = ar.get_army();
		auto army_controller = army.get_controller_from_army_control();
		// only units in a battle and hostile to the fort controller can count towards reducing fort level
		if(army.get_battle_from_army_battle_participation() && are_enemies(state, location_controller, army_controller)) {
			auto army_stats = tech_nation_for_army(state, ar.get_army());
			for(auto r : army.get_army_membership()) {
				auto reg_str = r.get_regiment().get_strength();
				if(reg_str > 0.001f) {
					auto type = r.get_regiment().get_type();
					auto& stats = state.world.nation_get_unit_stats(army_stats, type);

					total_enemy_strength += reg_str;

					if(stats.siege_or_torpedo_attack > 0.0f) {
						strength_siege_units += reg_str;
						max_siege_value = std::max(max_siege_value, stats.siege_or_torpedo_attack);
					}
				}
			}
		}
	}
	return get_effective_fort_level(state, location, total_enemy_strength, strength_siege_units, max_siege_value);
}
// computes the coordination penalty (from 0.0 with no penalty, to 1.0 which is full penalty) based on how much we outnumber the enemy
// The penalty is applied to the "friendly ships" side
float naval_battle_get_coordination_penalty(sys::state& state, uint32_t friendly_ships, uint32_t enemy_ships) {
	// guard against dbz error, battle should end anyway before this, but incase it dosent it wont crash.
	if(friendly_ships == 0) {
		return 0.0f;
	}
	return std::clamp(1 - (float(enemy_ships) / float(friendly_ships)), 0.0f, 1.0f);

}
// computes the coordination bonus (from 0.0 with no bonus, to 1.0 which is full bonus) based on how much the enemy outnumbers our side
// The bonus is applied to the "friendly ships" side
float naval_battle_get_coordination_bonus(sys::state& state, uint32_t friendly_ships, uint32_t enemy_ships) {
	return naval_battle_get_coordination_penalty(state, enemy_ships, friendly_ships);

}

// gets the stacking penalty depending on the ratio of friendly & enenmy ships. The stacking penalty is supposed to be from the perspective of the friendly ships
float get_damage_reduction_stacking_penalty(sys::state& state, uint32_t friendly_ships, uint32_t enemy_ships) {

	// if the max targets per ship is only one, stacking penalties are effectively disabled anyway. Also avoid DBZ error later
	if(state.defines.naval_combat_max_targets == 1.0f) {
		return 1.0f;
	}
	float outnumber_ratio = std::clamp(float(friendly_ships + 1) / float(enemy_ships + 1), 1.0f, state.defines.naval_combat_max_targets);
	return 1.0f - (state.defines.alice_naval_combat_stacking_damage_penalty * (outnumber_ratio - 1) / (state.defines.naval_combat_max_targets - 1));
}

// gets a target for a ship in a naval battle, returns the target index of the target if a valid target was found, otherwise returns -1 if no valid target was found or the random chance roll failed
int16_t get_naval_battle_target(sys::state& state, const ship_in_battle& ship, dcon::naval_battle_id battle, uint32_t defender_ships, uint32_t attacker_ships) {

	bool is_attacker = (ship.flags & ship_in_battle::is_attacking) != 0;
	dcon::leader_id navy_admiral = state.world.navy_get_admiral_from_navy_leadership(state.world.ship_get_navy_from_navy_membership(ship.ship));

	auto admiral_recon = state.world.leader_trait_get_reconnaissance(get_leader_personality_wrapper(state, navy_admiral)) + state.world.leader_trait_get_reconnaissance(get_leader_background_wrapper(state, navy_admiral));

	uint32_t friendly_ships = is_attacker ? attacker_ships : defender_ships;

	uint32_t enemy_ships = is_attacker ? defender_ships : attacker_ships;

	auto target_pick_chance = std::clamp((state.defines.naval_combat_seeking_chance + admiral_recon) + (state.defines.alice_naval_combat_enemy_stacking_target_select_bonus * naval_battle_get_coordination_bonus(state, friendly_ships, enemy_ships)) - (state.defines.naval_combat_stacking_target_select * naval_battle_get_coordination_penalty(state, friendly_ships, enemy_ships)), state.defines.naval_combat_seeking_chance_min, 1.0f);

	auto slots = state.world.naval_battle_get_slots(battle);

	// comput the chance it is able to get a target
	auto random = rng::get_random(state, ship.ship.value * state.current_date.to_raw_value());
	if((random % 100000 + 1) <= target_pick_chance * 100000) {
		// if the ship is attacking
		if(is_attacker) {
			// if no ships left to target, return -1
			if(defender_ships == 0) {
				return -1;
			}
			auto pick = rng::get_random(state, uint32_t(ship.ship.value)) % defender_ships;

			for(uint32_t k = slots.size(); k-- > 0;) {
				auto ship_battle = slots[k];
				switch(slots[k].flags & ship_in_battle::mode_mask) {

				case ship_in_battle::mode_seeking:
				case ship_in_battle::mode_approaching:
				case ship_in_battle::mode_retreating:
				case ship_in_battle::mode_engaged:
					if((slots[k].flags & ship_in_battle::is_attacking) == 0 && slots[k].ships_targeting_this < state.defines.naval_combat_max_targets) {
						if(pick == 0) {
							return int16_t(k);
						} else {
							--pick;
						}
					}
					break;
				default:
					break;
				}
			}
		} else {
			// if no ships left to target, return -1
			if(attacker_ships == 0) {
				return -1;
			}
			auto pick = rng::get_random(state, uint32_t(ship.ship.value)) % attacker_ships;

			for(uint32_t k = slots.size(); k-- > 0;) {
				switch(slots[k].flags & ship_in_battle::mode_mask) {

				case ship_in_battle::mode_seeking:
				case ship_in_battle::mode_approaching:
				case ship_in_battle::mode_retreating:
				case ship_in_battle::mode_engaged:
					if((slots[k].flags & ship_in_battle::is_attacking) != 0 && slots[k].ships_targeting_this < state.defines.naval_combat_max_targets) {
						if(pick == 0) {
							return int16_t(k);
						} else {
							--pick;
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}
	
	return -1;

}

void notify_on_new_land_battle(sys::state& state, dcon::land_battle_id battle, dcon::nation_id nation_as) {
	war_role battle_role = war_role::none;
	for(auto n : state.world.land_battle_get_army_battle_participation(battle)) {
		auto army_controller = n.get_army().get_controller_from_army_control();
		if(army_controller == nation_as) {
			if(is_attacker_in_battle(state, n.get_army())) {
				assert(battle_role != war_role::defender);
				battle_role = war_role::attacker;
				break;

			} else {
				assert(battle_role != war_role::attacker);
				battle_role = war_role::defender;
				break;
			}
		}

	}
	auto location = state.world.land_battle_get_location_from_land_battle_location(battle);
	// notify if attacking
	dcon::nation_id enemy_nation = (battle_role == war_role::attacker) ? get_land_battle_lead_defender(state, battle) : get_land_battle_lead_attacker(state, battle);
	bool show_notification = ((enemy_nation == dcon::nation_id{ } && state.user_settings.notify_rebels_defeat) || enemy_nation != dcon::nation_id{ });
	if(battle_role == war_role::attacker && show_notification) {
		notification::post(state, notification::message{
			.body = [=](sys::state& state, text::layout_base& layout) {

				auto identity = state.world.nation_get_identity_from_identity_holder(nation_as);
				auto govt_type = state.world.nation_get_government_type(nation_as);
				auto ruler_name = state.world.national_identity_get_government_ruler_name(identity, govt_type);
				auto govt_type_ruler = state.world.government_type_get_ruler_name(govt_type);
				auto location_name = state.world.province_get_name(location);

				auto enemy_name = bool(enemy_nation) ? text::get_name(state, enemy_nation) : state.world.national_identity_get_name( state.national_definitions.rebel_id);

				text::add_line(state, layout, "ATTACKTHEM_2", text::variable_type::monarchtitle, text::produce_simple_string(state, govt_type_ruler));
				text::add_line(state, layout, "ATTACKTHEM_3", text::variable_type::prov, text::produce_simple_string(state, location_name));
				text::add_line(state, layout, "alice_attackthem_4", text::variable_type::defender, text::produce_simple_string(state, enemy_name));
			},

			.title = "ATTACKTHEM_1",
			.source = nation_as,
			.target = dcon::nation_id{ },
			.third = dcon::nation_id{ },
			.type = sys::message_base_type::land_combat_starts_by_nation,
			.province_source = location,

		});


	}
	// notify if defending
	else if(battle_role == war_role::defender && show_notification) {
		notification::post(state, notification::message{
			.body = [=](sys::state& state, text::layout_base& layout) {

				auto identity = state.world.nation_get_identity_from_identity_holder(nation_as);
				auto govt_type = state.world.nation_get_government_type(nation_as);
				auto ruler_name = state.world.national_identity_get_government_ruler_name(identity, govt_type);
				auto govt_type_ruler = state.world.government_type_get_ruler_name(govt_type);
				auto location_name = state.world.province_get_name(location);


				auto enemy_name = bool(enemy_nation) ? text::get_name(state, enemy_nation) : state.world.national_identity_get_name(state.national_definitions.rebel_id);

				text::add_line(state, layout, "ATTACKUS_2", text::variable_type::monarchtitle, text::produce_simple_string(state, govt_type_ruler));
				text::add_line(state, layout, "ATTACKUS_3", text::variable_type::prov, text::produce_simple_string(state, location_name));
				text::add_line(state, layout, "alice_attackus_4", text::variable_type::attacker, text::produce_simple_string(state, enemy_name));
			},

			.title = "ATTACKUS_1",
			.source = nation_as,
			.target = dcon::nation_id{ },
			.third = dcon::nation_id{ },
			.type = sys::message_base_type::land_combat_starts_on_nation,
			.province_source = location
		});

	}
}
		


void update_land_battles(sys::state& state) {
	auto isize = state.world.land_battle_size();
	auto to_delete = ve::vectorizable_buffer<uint8_t, dcon::land_battle_id>(isize);

	concurrency::parallel_for(0, int32_t(isize), [&](int32_t index) {
		dcon::land_battle_id b{ dcon::land_battle_id::value_base_t(index) };

		if(!state.world.land_battle_is_valid(b))
			return;


		if(state.world.land_battle_get_start_date(b) == state.current_date) {
			notify_on_new_land_battle(state, b, state.local_player_nation);
		}

		// fill to combat width
		auto combat_width = state.world.land_battle_get_combat_width(b);

		auto& att_back = state.world.land_battle_get_attacker_back_line(b);
		auto& def_back = state.world.land_battle_get_defender_back_line(b);
		auto& att_front = state.world.land_battle_get_attacker_front_line(b);
		auto& def_front = state.world.land_battle_get_defender_front_line(b);

		auto reserves = state.world.land_battle_get_reserves(b);

		if((state.current_date.value - state.world.land_battle_get_start_date(b).value) % 5 == 4) {
			uint8_t new_dice = make_dice_rolls(state, uint32_t(index));
			state.world.land_battle_set_dice_rolls(b, new_dice);
			auto attacker_dice = new_dice & 0x0F;
			auto defender_dice = (new_dice >> 4) & 0x0F;
			auto& def_bonus = state.world.land_battle_get_defender_bonus(b);
			// if the attacker rolls higher than the defender and dig-in is higher than 0, remove 1 level of dig-in from the defender
			if(attacker_dice > defender_dice && (def_bonus & defender_bonus_dig_in_mask) > 0) {
				uint8_t prev_dig_in = uint8_t(def_bonus & defender_bonus_dig_in_mask);
				uint8_t new_dig_in = uint8_t(prev_dig_in - 1);
				state.world.land_battle_set_defender_bonus(b, uint8_t(def_bonus & ~defender_bonus_dig_in_mask));
				state.world.land_battle_set_defender_bonus(b, uint8_t(def_bonus | new_dig_in));
			}
			
		}

		// calculate modifier values for each side

		/*
		Combat modifiers:
		In addition to the dice, both sides are affected by:
		Attack or defend trait of the leader in charge (attack for attackers, defend for defenders)
		define:GAS_ATTACK_MODIFIER if the side has gas attack and the other side doesn't have gas defense
		terrain defense bonus (defender only)
		-1 for attacking across a river or -2 for attacking from the sea
		minus the least dig in value on the opposing side / 2 (if the side has at a fraction of units with reconnaissance at least
		define:RECON_UNIT_RATIO) or divided by (greatest-regiment-reconnaissance x (leader-reconnaissance + 1) x
		fraction-of-reconnaissance-unit-strength / total-strength)
		*/

		auto both_dice = state.world.land_battle_get_dice_rolls(b);
		auto defender_mods = state.world.land_battle_get_defender_bonus(b);
		auto dig_in_value = get_effective_battle_dig_in(state, b);
		auto crossing_value = defender_mods & defender_bonus_crossing_mask;

		auto attacking_nation = get_land_battle_lead_attacker(state, b);
		auto defending_nation = get_land_battle_lead_defender(state, b);

		bool attacker_gas =
			state.world.nation_get_has_gas_attack(attacking_nation) && !state.world.nation_get_has_gas_defense(defending_nation);
		bool defender_gas =
			state.world.nation_get_has_gas_attack(defending_nation) && !state.world.nation_get_has_gas_defense(attacking_nation);

		int32_t crossing_adjustment =
			(crossing_value == defender_bonus_crossing_none ? 0 : (crossing_value == defender_bonus_crossing_river ? -1 : -2));

		auto attacker_dice = both_dice & 0x0F;
		auto defender_dice = (both_dice >> 4) & 0x0F;

		auto location = state.world.land_battle_get_location_from_land_battle_location(b);
		auto terrain_bonus = state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::defense);

		auto attacker_per = fatten(state.world, get_leader_personality_wrapper(state, state.world.land_battle_get_general_from_attacking_general(b)));
		auto attacker_bg = fatten(state.world, get_leader_background_wrapper(state, state.world.land_battle_get_general_from_attacking_general(b)));

		auto attack_bonus =
			int32_t(state.world.leader_trait_get_attack(attacker_per) + state.world.leader_trait_get_attack(attacker_bg));
		/*
		auto attacker_org_bonus =
			  (1.0f + state.world.leader_trait_get_organisation(attacker_per) + state.world.leader_trait_get_organisation(attacker_bg))
			* (1.0f + state.world.leader_get_prestige(state.world.land_battle_get_general_from_attacking_general(b)) * state.defines.leader_prestige_to_max_org_factor);
		*/

		auto defender_per = fatten(state.world, get_leader_personality_wrapper(state, state.world.land_battle_get_general_from_defending_general(b)));
		auto defender_bg = fatten(state.world, get_leader_background_wrapper(state, state.world.land_battle_get_general_from_defending_general(b)));

		auto atk_leader_exp_mod = 1 + attacker_per.get_experience() + attacker_bg.get_experience();
		auto def_leader_exp_mod = 1 + defender_per.get_experience() + defender_bg.get_experience();

		auto defence_bonus =
			int32_t(state.world.leader_trait_get_defense(defender_per) + state.world.leader_trait_get_defense(defender_bg));
		/*
		auto defender_org_bonus =
				(1.0f + state.world.leader_trait_get_organisation(defender_per) + state.world.leader_trait_get_organisation(defender_bg))
			* (1.0f + state.world.leader_get_prestige(state.world.land_battle_get_general_from_defending_general(b)) * state.defines.leader_prestige_to_max_org_factor);

		*/


		auto attacker_mod = combat_modifier_table[std::clamp(attacker_dice + attack_bonus + crossing_adjustment + int32_t(attacker_gas ? state.defines.gas_attack_modifier : 0.0f) + 3, 0, 18)];
		auto defender_mod = combat_modifier_table[std::clamp(defender_dice + defence_bonus + dig_in_value + int32_t(defender_gas ? state.defines.gas_attack_modifier : 0.0f) + int32_t(terrain_bonus) + 3, 0, 18)];

		float defender_fort = 1.0f;
		auto local_control = state.world.province_get_nation_from_province_control(location);
		if((!attacking_nation && local_control) ||
				(attacking_nation && (!bool(local_control) || military::are_at_war(state, attacking_nation, local_control)))) {
			defender_fort = 1.0f + 0.1f * get_combat_fort_level(state, location);
		}

		// apply damage to all regiments

		// Effective military tactics = define:BASE_MILITARY_TACTICS + tactics-from-tech

		/*
		Units attack the opposing front row, and may look either left or right of their position up to `maneuver` positions.
		Units on the attacking side use their `attack` stat to do damage, while units on the defending side use `defense`.
		Strength damage dealt: unit-strength x (attack/defense x 0.1 + 1) x Modifier-Table\[modifiers + 2\] x 2 /
		((effective-fort-level x 0.1 + 1) x opposing-effective-military-tactics x (experience-of-opposing-regiment x 0.1 + 1))
		Organization damage dealt: unit-strength x (attack/defense x 0.1 + 1) x Modifier-Table\[modifiers + 2\] x 2 /
		((effective-fort-level x 0.1 + 1) x opposing-discipline (if non zero) x (experience-of-opposing-regiment x 0.1 + 1)) Units
		attacking from the back row have these values multiplied by the unit's support value.

		If the unit is in a province controlled by a hostile power, we find the effective level of the fort as with a siege (We find
		the effective level of the fort by subtracting: (rounding this value down to to the nearest integer)
		greatest-siege-value-present x
		((the ratio of the strength of regiments with siege present to the total strength of all regiments) ^
		define:ENGINEER_UNIT_RATIO) / define:ENGINEER_UNIT_RATIO, reducing it to a minimum of 0.)

		When a regiment takes strength damage, the size of the pop backing it is reduced by: define:POP_SIZE_PER_REGIMENT x
		damage-amount x define:SOLDIER_TO_POP_DAMAGE / (max-strength x (solder-to-pop-loss-from-tech + 1)). Note that this applied to
		damage from attrition as well.
		*/

		state.world.land_battle_set_attacker_casualties(b, 0);
		state.world.land_battle_set_defender_casualties(b, 0);

		float attacker_casualties = 0;
		float defender_casualties = 0;

		// the max org divisor to org damage is calculated by getting the effective default org (ie max org) of a unit, and dividing it by 30.
		// 30 is the startind default org for most units, so org damage is normalized to what it was previously, if no org bonuses.
		// this will scale the org damage taken to the actual max org of the unit

		for(int32_t i = 0; i < combat_width; ++i) {
			// Attackers backline shooting defenders frontline
			if(att_back[i]) {
				assert(state.world.regiment_is_valid(att_back[i]));

				auto att_back_target = get_land_combat_target(state, att_back[i], i, def_front);

				if(att_back_target) {

					auto str_damage = get_reg_str_damage(state, att_back[i], att_back_target, attacker_mod, true, true, defender_fort);
					auto org_damage = get_reg_org_damage(state, att_back[i], att_back_target, attacker_mod, true, true, defender_fort);


					military::regiment_take_damage<regiment_dmg_source::combat>(state, att_back_target, str_damage);

					defender_casualties += str_damage;

					adjust_regiment_experience(state, attacking_nation, att_back[i], str_damage * 5.f * state.defines.exp_gain_div * atk_leader_exp_mod);

					auto& org = state.world.regiment_get_org(att_back_target);
					state.world.regiment_set_org(att_back_target, std::max(0.0f, org - org_damage));
					switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(att_back_target)].type) {
					case unit_type::infantry:
					{
						auto& cur_lost = state.world.land_battle_get_defender_infantry_lost(b);
						state.world.land_battle_set_defender_infantry_lost(b, cur_lost + str_damage);
						break;
					}					
					case unit_type::cavalry:
					{
						auto& cur_lost = state.world.land_battle_get_defender_cav_lost(b);
						state.world.land_battle_set_defender_cav_lost(b, cur_lost + str_damage);
						break;
					}					
					case unit_type::support:
						// fallthrough
					case unit_type::special:
					{
						auto& cur_lost = state.world.land_battle_get_defender_support_lost(b);
						state.world.land_battle_set_defender_support_lost(b, cur_lost + str_damage);
						break;
					}					
					default:
						break;
					}
				}
			}

			// Defence backline shooting attackers frontline
			if(def_back[i]) {

				assert(state.world.regiment_is_valid(def_back[i]));

				auto def_back_target = get_land_combat_target(state, def_back[i], i, att_front);

				if(def_back_target) {

					auto str_damage = get_reg_str_damage(state, def_back[i], def_back_target, defender_mod, true, false);
					auto org_damage = get_reg_org_damage(state, def_back[i], def_back_target, defender_mod, true, false);

					military::regiment_take_damage<regiment_dmg_source::combat>(state, def_back_target, str_damage);

					attacker_casualties += str_damage;

					adjust_regiment_experience(state, defending_nation, def_back[i], str_damage * 5.f * state.defines.exp_gain_div * def_leader_exp_mod);

					auto& org = state.world.regiment_get_org(def_back_target);
					state.world.regiment_set_org(def_back_target, std::max(0.0f, org - org_damage));

					switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(def_back_target)].type) {
					case unit_type::infantry:
					{
						auto& cur_lost = state.world.land_battle_get_attacker_infantry_lost(b);
						state.world.land_battle_set_attacker_infantry_lost(b, cur_lost + str_damage);
						break;
					}				
					case unit_type::cavalry:
					{
						auto& cur_lost = state.world.land_battle_get_attacker_cav_lost(b);
						state.world.land_battle_set_attacker_cav_lost(b, cur_lost + str_damage);
						break;
					}					
					case unit_type::support:
						// fallthrough
					case unit_type::special:
					{
						auto& cur_lost = state.world.land_battle_get_attacker_support_lost(b);
						state.world.land_battle_set_attacker_support_lost(b, cur_lost + str_damage);
						break;
					}						
					default:
						break;
					}
				}
			}

			// Attackers frontline attacking defenders frontline targets
			if(att_front[i]) {
				assert(state.world.regiment_is_valid(att_front[i]));

				auto att_front_target = get_land_combat_target(state, att_front[i], i, def_front);

				if(att_front_target) {
					assert(state.world.regiment_is_valid(att_front_target));

					auto str_damage = get_reg_str_damage(state, att_front[i], att_front_target, attacker_mod, false, true, defender_fort);
					auto org_damage = get_reg_org_damage(state, att_front[i], att_front_target, attacker_mod, false, true, defender_fort);


					military::regiment_take_damage<regiment_dmg_source::combat>(state, att_front_target, str_damage);
					defender_casualties += str_damage;

					adjust_regiment_experience(state, attacking_nation, att_front[i], str_damage * 5.f * state.defines.exp_gain_div * atk_leader_exp_mod);

					auto& org = state.world.regiment_get_org(att_front_target);
					state.world.regiment_set_org(att_front_target, std::max(0.0f, org - org_damage));
					switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(att_front_target)].type) {
					case unit_type::infantry:
					{
						auto& cur_lost = state.world.land_battle_get_defender_infantry_lost(b);
						state.world.land_battle_set_defender_infantry_lost(b, cur_lost + str_damage);
						break;
					}					
					case unit_type::cavalry:
					{
						auto& cur_lost = state.world.land_battle_get_defender_cav_lost(b);
						state.world.land_battle_set_defender_cav_lost(b, cur_lost + str_damage);
						break;
					}				
					case unit_type::support:
						// fallthrough
					case unit_type::special:
					{
						auto& cur_lost = state.world.land_battle_get_defender_support_lost(b);
						state.world.land_battle_set_defender_support_lost(b, cur_lost + str_damage);
						break;
					}			
					default:
						break;
					}
				}
			}

			// Defenders frontline attacks attackers frontline
			if(def_front[i]) {
				assert(state.world.regiment_is_valid(def_front[i]));

				auto tech_def_nation = tech_nation_for_regiment(state, def_front[i]);
				auto& def_stats = state.world.nation_get_unit_stats(tech_def_nation, state.world.regiment_get_type(def_front[i]));

				auto def_front_target = get_land_combat_target(state, def_front[i], i, att_front);

				if(def_front_target) {
					assert(state.world.regiment_is_valid(def_front_target));

					auto str_damage = get_reg_str_damage(state, def_front[i], def_front_target, defender_mod, false, false);
					auto org_damage = get_reg_org_damage(state, def_front[i], def_front_target, defender_mod, false, false);

					military::regiment_take_damage<regiment_dmg_source::combat>(state, def_front_target, str_damage);
					attacker_casualties += str_damage;

					adjust_regiment_experience(state, defending_nation, def_front[i], str_damage * 5.f * state.defines.exp_gain_div * def_leader_exp_mod);

					auto& org = state.world.regiment_get_org(def_front_target);
					state.world.regiment_set_org(def_front_target, std::max(0.0f, org - org_damage));
					switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(def_front_target)].type) {
					case unit_type::infantry:
					{
						auto& cur_lost = state.world.land_battle_get_attacker_infantry_lost(b);
						state.world.land_battle_set_attacker_infantry_lost(b, cur_lost + str_damage);
						break;
					}					
					case unit_type::cavalry:
					{
						auto& cur_lost = state.world.land_battle_get_attacker_cav_lost(b);
						state.world.land_battle_set_attacker_cav_lost(b, cur_lost + str_damage);
						break;
					}					
					case unit_type::support:
						// fallthrough
					case unit_type::special:
					{
						auto& cur_lost = state.world.land_battle_get_attacker_support_lost(b);
						state.world.land_battle_set_attacker_support_lost(b, cur_lost + str_damage);
						break;
					}
					default:
						break;
					}
				}
			}
		}

		state.world.land_battle_set_attacker_casualties(b, attacker_casualties);
		state.world.land_battle_set_defender_casualties(b, defender_casualties);

		for(auto& ref : state.lua_on_battle_tick) {
			lua_rawgeti(state.lua_game_loop_environment, LUA_REGISTRYINDEX, ref);
			lua_pushnumber(state.lua_game_loop_environment, b.index());
			auto result = lua_pcall(state.lua_game_loop_environment, 1, 0, 0);
			if(result) {
				state.lua_notification(lua_tostring(state.lua_game_loop_environment, -1));
				lua_settop(state.lua_game_loop_environment, 0);
			}
			assert(lua_gettop(state.lua_game_loop_environment) == 0);
		}


		// clear dead / retreated regiments out
		// puts them back into the reserve
		for(int32_t i = 0; i < combat_width; ++i) {
			if(def_back[i]) {
				if(state.world.regiment_get_strength(def_back[i]) <= state.defines.alice_reg_move_to_reserve_str) {
					add_regiment_to_reserves(state, b, def_back[i], false);
					def_back[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(def_back[i]) < state.defines.alice_reg_move_to_reserve_org) {
					add_regiment_to_reserves(state, b, def_back[i], false);
					def_back[i] = dcon::regiment_id{};
				}
			}
			if(def_front[i]) {
				if(state.world.regiment_get_strength(def_front[i]) <= state.defines.alice_reg_move_to_reserve_str) {
					add_regiment_to_reserves(state, b, def_front[i], false);
					def_front[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(def_front[i]) < state.defines.alice_reg_move_to_reserve_org) {
					add_regiment_to_reserves(state, b, def_front[i], false);
					def_front[i] = dcon::regiment_id{};
				}
			}
			if(att_back[i]) {
				if(state.world.regiment_get_strength(att_back[i]) <= state.defines.alice_reg_move_to_reserve_str) {
					add_regiment_to_reserves(state, b, att_back[i], true);
					att_back[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(att_back[i]) < state.defines.alice_reg_move_to_reserve_org) {
					add_regiment_to_reserves(state, b, att_back[i], true);
					att_back[i] = dcon::regiment_id{};
				}
			}
			if(att_front[i]) {
				if(state.world.regiment_get_strength(att_front[i]) <= state.defines.alice_reg_move_to_reserve_str) {
					add_regiment_to_reserves(state, b, att_front[i], true);
					att_front[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(att_front[i]) < state.defines.alice_reg_move_to_reserve_org) {
					add_regiment_to_reserves(state, b, att_front[i], true);
					att_front[i] = dcon::regiment_id{};
				}
			}
		}


		auto compact = [](std::array<dcon::regiment_id, 30>& a) {
			int32_t low = 0;
			while(low < 30 && a[low]) {
				low += 2;
			}
			int32_t high = low + 2;
			while(high < 30 && !a[high])
				high += 2;

			while(high < 30) {
				a[low] = a[high];
				a[high] = dcon::regiment_id{};

				high += 2;
				while(high < 30 && !a[high])
					high += 2;

				low += 2;
			}

			low = 1;
			while(low < 30 && a[low]) {
				low += 2;
			}
			high = low + 2;
			while(high < 30 && !a[high])
				high += 2;

			while(high < 30) {
				a[low] = a[high];
				a[high] = dcon::regiment_id{};

				high += 2;
				while(high < 30 && !a[high])
					high += 2;

				low += 2;
			}
			};

		compact(att_back);
		compact(att_front);
		compact(def_back);
		compact(def_front);

		// prefer slot zero
		if(!att_back[0]) {
			std::swap(att_back[0], att_back[1]);
		}
		if(!def_back[0]) {
			std::swap(def_back[0], def_back[1]);
		}
		if(!att_front[0]) {
			std::swap(att_front[0], att_front[1]);
		}
		if(!def_front[0]) {
			std::swap(def_front[0], def_front[1]);
		}

		// sorts the reserves by some criteria so the most fit brigades for deployment are pushed to the front of the queue
		sort_reserves_by_deployment_order(state, reserves);

		// won't use troops from the reserve which > alice_reg_deploy_from_reserve_org, or >= alice_reg_deploy_from_reserve_str
		// back row
		for(int32_t i = 0; i < combat_width; ++i) {
			if(!att_back[i]) {
				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::is_attacking | reserve_regiment::type_support) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str && state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
						att_back[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}
			}
			// if the attacker backline support brigade is not full, see if a higher str replacement can be found
			else if(state.world.regiment_get_strength(att_back[i]) < 1.0f) {
				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::is_attacking | reserve_regiment::type_support) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.world.regiment_get_strength(att_back[i]) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str &&
					state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
						att_back[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}
			}

			if(!def_back[i]) {
				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::type_support) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str && state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
						def_back[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}
			}
			// if the defender backline support brigade is not full, see if a higher str replacement can be found
			else if(state.world.regiment_get_strength(def_back[i]) < 1.0f) {
				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::type_support) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.world.regiment_get_strength(def_back[i]) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str &&
					state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
						def_back[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}
			}
		}

		// front row
		for(int32_t i = 0; i < combat_width; ++i) {
			if(!att_front[i]) {

				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::is_attacking | reserve_regiment::type_infantry) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str && state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
						att_front[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}

				if(!att_front[i]) {
					for(uint32_t j = reserves.size(); j-- > 0;) {
						if(reserves[j].flags == (reserve_regiment::is_attacking | reserve_regiment::type_cavalry) &&
						state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str && state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
							att_front[i] = reserves[j].regiment;
							std::swap(reserves[j], reserves[reserves.size() - 1]);
							reserves.pop_back();
							break;
						}
					}
				}
				if(!att_front[i] && att_back[i]) {
					std::swap(att_front[i], att_back[i]);
				}
			}

			if(!def_front[i]) {
				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::type_infantry) &&
					state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str && state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
						def_front[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}

				if(!def_front[i]) {
					for(uint32_t j = reserves.size(); j-- > 0;) {
						if(reserves[j].flags == (reserve_regiment::type_cavalry) &&
						state.world.regiment_get_strength(reserves[j].regiment) > state.defines.alice_reg_deploy_from_reserve_str && state.world.regiment_get_org(reserves[j].regiment) >= state.defines.alice_reg_deploy_from_reserve_org) {
							def_front[i] = reserves[j].regiment;
							std::swap(reserves[j], reserves[reserves.size() - 1]);
							reserves.pop_back();
							break;
						}
					}
				}
				if(!def_front[i] && def_back[i]) {
					std::swap(def_front[i], def_back[i]);
				}
			}
		}

		if(!def_front[0]) {
			to_delete.set(b, uint8_t(1));
			return;
		} else if(!att_front[0]) {
			to_delete.set(b, uint8_t(2));
			return;
		}
	});

	for(auto i = isize; i-- > 0;) {
		dcon::land_battle_id b{ dcon::land_battle_id::value_base_t(i) };
		if(state.world.land_battle_is_valid(b) && to_delete.get(b) != 0) {
			end_battle(state, b, to_delete.get(b) == uint8_t(1) ? battle_result::attacker_won : battle_result::defender_won);
		}
	}
}

// just a helper method for determining if a naval battle slot index is valid or invalid
bool naval_slot_index_valid(int16_t index) {
	return index > -1;
}

float required_avg_dist_to_center_for_retreat(sys::state& state) {
	return state.defines.naval_combat_retreat_min_distance * naval_battle_distance_to_center;
}


// pass a value of -1 to clear target
void ship_in_battle_set_target(ship_in_battle& ship, int16_t target_index, dcon::dcon_vv_fat_id<ship_in_battle>& ship_slots) {
	if(naval_slot_index_valid(ship.target_slot)) {
		assert(ship_slots[ship.target_slot].ships_targeting_this > 0);
		ship_slots[ship.target_slot].ships_targeting_this--;
	}
	ship.target_slot = target_index;
	if(naval_slot_index_valid(target_index)) {
		ship_slots[target_index].ships_targeting_this++;
	}
}

// updates the ship in a naval battle after it has been hit by another ship, and may alter its state and battle-related statistics accordingly. Returns false if the target is no longer targetable (sunk), otherwise true
bool update_ship_in_naval_battle_after_hit(sys::state& state, ship_in_battle& ship, dcon::naval_battle_id battle, int32_t& defender_ships, int32_t& attacker_ships) {

	auto ship_owner =
		state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(ship.ship));
	auto type = state.world.ship_get_type(ship.ship);

	auto slots = state.world.naval_battle_get_slots(battle);

	switch(ship.flags & ship_in_battle::mode_mask) {
	case ship_in_battle::mode_seeking:
	case ship_in_battle::mode_approaching:
	case ship_in_battle::mode_engaged:
	case ship_in_battle::mode_retreating:
		if(state.world.ship_get_strength(ship.ship) <= 0) {
			if((ship.flags & ship_in_battle::is_attacking) != 0) {
				if((ship.flags & ship_in_battle::type_mask) == ship_in_battle::type_big) {
					state.world.naval_battle_set_attacker_big_ships_lost(battle, state.world.naval_battle_get_attacker_big_ships_lost(battle) + 1);
				} else if((ship.flags & ship_in_battle::type_mask) == ship_in_battle::type_small) {
					state.world.naval_battle_set_attacker_small_ships_lost(battle, state.world.naval_battle_get_attacker_small_ships_lost(battle) + 1);
				} else if((ship.flags & ship_in_battle::type_mask) == ship_in_battle::type_transport) {
					state.world.naval_battle_set_attacker_transport_ships_lost(battle, state.world.naval_battle_get_attacker_transport_ships_lost(battle) + 1);
				}
				state.world.naval_battle_set_attacker_loss_value(battle, state.world.naval_battle_get_attacker_loss_value(battle) + state.military_definitions.unit_base_definitions[type].supply_consumption_score);
				attacker_ships--;
			} else {
				if((ship.flags & ship_in_battle::type_mask) == ship_in_battle::type_big) {
					state.world.naval_battle_set_defender_big_ships_lost(battle, state.world.naval_battle_get_defender_big_ships_lost(battle) + 1);
				} else if((ship.flags & ship_in_battle::type_mask) == ship_in_battle::type_small) {
					state.world.naval_battle_set_defender_small_ships_lost(battle, state.world.naval_battle_get_defender_small_ships_lost(battle) + 1);
				} else if((ship.flags & ship_in_battle::type_mask) == ship_in_battle::type_transport) {
					state.world.naval_battle_set_defender_transport_ships_lost(battle, state.world.naval_battle_get_defender_transport_ships_lost(battle) + 1);
				}
				state.world.naval_battle_set_defender_loss_value(battle, state.world.naval_battle_get_defender_loss_value(battle) + state.military_definitions.unit_base_definitions[type].supply_consumption_score);
				defender_ships--;
			}
			ship.flags &= ~ship_in_battle::mode_mask;
			ship.flags |= ship_in_battle::mode_sunk;
			// reset the ship's target as it is now sunk.
			ship_in_battle_set_target(ship, -1, slots);
			ship.ship = dcon::ship_id{ };
			return false;
		}
		break;
	default:
		break;
	}
	return true;


}

/*
Torpedo attack: is treated as 0 except against big ships
Damage to organization is (gun-power + torpedo-attack) * Modifier-Table\[modifiers + 2\] (see above) * strength * ( 1 / target-hull) *
(1 / (1 + target-ship-exp)) * define:NAVAL_COMBAT_DAMAGE_ORG_MULT

Damage to strength is
Damage to organization is (gun-power + torpedo-attack) * Modifier-Table\[modifiers + 2\] (see above) * strength * ( 1 / target-hull) *
(1 / (1 + target-ship-exp)) * (if no org: define:NAVAL_COMBAT_DAMAGE_MULT_NO_ORG) * define:NAVAL_COMBAT_DAMAGE_STR_MULT

Evasion reduces the chance for an attack to hit the target in vanilla V2, however here we just put it as an additional damage reduction modifier (20% evasion = 20% damage reduction) at the end.

In addition, a custom define has been added which is not in base V2: alice_naval_combat_stacking_damage_penalty. This reduces damage done by the outnumbering side, maxed out at the amount in the define.
*/


// returns the strength damage in percent (from 1.0 to 0.0) that the target would receive if it was attacked by the damage dealer. Has no side effects
float get_ship_strength_damage(sys::state& state, const ship_in_battle& damage_dealer, const ship_in_battle& target, float battle_modifiers, int32_t attacker_ships, int32_t defender_ships) {
	bool target_is_big = (target.flags & ship_in_battle::type_mask) == ship_in_battle::type_big;

	bool is_attacker = (damage_dealer.flags & ship_in_battle::is_attacking) != 0;

	auto dmg_dealer_owner =
		state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(damage_dealer.ship));

	auto dmg_dealer_type = state.world.ship_get_type(damage_dealer.ship);

	auto& dmg_dealer_stats = state.world.nation_get_unit_stats(dmg_dealer_owner, dmg_dealer_type);

	auto dmg_dealer_str = state.world.ship_get_strength(damage_dealer.ship);


	auto target_owner =
		state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(target.ship));
	auto target_type = state.world.ship_get_type(target.ship);
	assert(target_type);
	auto& target_stats = state.world.nation_get_unit_stats(target_owner, target_type);

	auto& targ_ship_exp = state.world.ship_get_experience(target.ship);

	bool target_no_org = state.world.ship_get_org(target.ship) == 0.0f;


	int32_t friendly_ships;
	int32_t enemy_ships;

	if(is_attacker) {
		friendly_ships = attacker_ships;
		enemy_ships = defender_ships;
	} else {
		friendly_ships = defender_ships;
		enemy_ships = attacker_ships;
	}

	auto stacking_dmg_penalty = get_damage_reduction_stacking_penalty(state, friendly_ships, enemy_ships);


	return (dmg_dealer_stats.attack_or_gun_power + (target_is_big ? dmg_dealer_stats.siege_or_torpedo_attack : 0.0f)) * (1 / target_stats.defence_or_hull) * (1 / (1 + targ_ship_exp)) * dmg_dealer_str * battle_modifiers * (target_no_org ? state.defines.naval_combat_damage_mult_no_org : 1.0f) * (1.0f - target_stats.discipline_or_evasion) * stacking_dmg_penalty * state.defines.naval_combat_damage_str_mult;


	// old formula:
	/*float str_damage = (dmg_dealer_stats.attack_or_gun_power + (target_is_big ? dmg_dealer_stats.siege_or_torpedo_attack : 0.0f)) *
		(battle_modifiers) * state.defines.naval_combat_damage_str_mult /
		((target_stats.defence_or_hull + 1.0f) * (1 + targ_ship_exp));*/
}


// returns the org damage in percent (from 1.0 to 0.0) that the target would receive if it was attacked by the damage dealer. Has no side effects
float get_ship_org_damage(sys::state& state, const ship_in_battle& damage_dealer, const ship_in_battle& target, float battle_modifiers, int32_t attacker_ships, int32_t defender_ships ) {
	bool target_is_big = (target.flags & ship_in_battle::type_mask) == ship_in_battle::type_big;

	bool is_attacker = (damage_dealer.flags & ship_in_battle::is_attacking) != 0;

	auto dmg_dealer_owner =
		state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(damage_dealer.ship));

	auto dmg_dealer_type = state.world.ship_get_type(damage_dealer.ship);

	auto& dmg_dealer_stats = state.world.nation_get_unit_stats(dmg_dealer_owner, dmg_dealer_type);

	auto dmg_dealer_str = state.world.ship_get_strength(damage_dealer.ship);


	auto target_owner =
		state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(target.ship));
	auto target_type = state.world.ship_get_type(target.ship);
	assert(target_type);
	auto& target_stats = state.world.nation_get_unit_stats(target_owner, target_type);

	auto& targ_ship_exp = state.world.ship_get_experience(target.ship);

	int32_t friendly_ships;
	int32_t enemy_ships;

	if(is_attacker) {
		friendly_ships = attacker_ships;
		enemy_ships = defender_ships;
	}
	else {
		friendly_ships = defender_ships;
		enemy_ships = attacker_ships;
	}

	auto stacking_dmg_penalty = get_damage_reduction_stacking_penalty(state, friendly_ships, enemy_ships);

	// this is the org damage in raw numbers instead of percentages, ie what needs to be effectively subtacted from the "default org" member of a given unit
	float raw_org_dmg = (dmg_dealer_stats.attack_or_gun_power + (target_is_big ? dmg_dealer_stats.siege_or_torpedo_attack : 0.0f)) * (1 / target_stats.defence_or_hull) * (1 / (1 + targ_ship_exp)) * dmg_dealer_str * battle_modifiers * (1.0f - target_stats.discipline_or_evasion) * stacking_dmg_penalty * state.defines.naval_combat_damage_org_mult * 100;
	// this calculates the percentages to be returned, and possibly subtracted from the percentage "org" dcon member later
	float percentage_org_dmg = raw_org_dmg / unit_get_effective_default_org(state, target.ship);
	return percentage_org_dmg;


	// old forumla: 
	/*auto max_org_divisor = unit_get_effective_default_org(state, tship) / 30;
	float org_damage = (1 / max_org_divisor) * (ship_stats.attack_or_gun_power + (target_is_big ? ship_stats.siege_or_torpedo_attack : 0.0f)) *
		(is_attacker ? attacker_mod : defender_mod) * state.defines.naval_combat_damage_org_mult /
		((ship_target_stats.defence_or_hull + 1.0f) *
				(1.0f + state.world.nation_get_modifier_values(ship_target_owner,
					sys::national_mod_offsets::naval_organisation))
			* (1 + targ_ship_exp));*/
}

// processes one ship in a battle shooting at another, and modifies both the damage dealer, target and number of defending&attacking ships accordingly
void ship_do_damage(sys::state& state, dcon::naval_battle_id battle, ship_in_battle& damage_dealer, ship_in_battle& target, float battle_modifiers, float leader_exp_mod, int32_t& defender_ships, int32_t& attacker_ships) {
	assert(target.ship);

	auto ship_target_owner =
		state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(target.ship));
	auto ttype = state.world.ship_get_type(target.ship);
	assert(ttype);

	auto dmg_dealer_owner = state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(damage_dealer.ship));


	float org_damage = get_ship_org_damage(state, damage_dealer, target, battle_modifiers, attacker_ships, defender_ships);


	float str_damage = get_ship_strength_damage(state, damage_dealer, target, battle_modifiers, attacker_ships, defender_ships);

	auto& torg = state.world.ship_get_org(target.ship);
	state.world.ship_set_org(target.ship, std::max(0.0f, torg - org_damage));
	military::reduce_ship_strength_safe(state, target.ship, str_damage);


	adjust_ship_experience(state, dmg_dealer_owner, damage_dealer.ship, str_damage * 5.f * state.defines.exp_gain_div * leader_exp_mod);

	if(!update_ship_in_naval_battle_after_hit(state, target, battle, defender_ships, attacker_ships)) {
		// if the previus target is no longer targetable (eg sunk), then switch to seeking mode immediately
		damage_dealer.flags &= ~ship_in_battle::mode_mask;
		damage_dealer.flags |= ship_in_battle::mode_seeking;
	}


}

// checks if a given ship should start retreating. No side effects
bool should_ship_retreat(sys::state& state, const ship_in_battle& ship, dcon::naval_battle_id battle) {

	switch(ship.flags & ship_in_battle::mode_mask) {
		case ship_in_battle::mode_seeking:
		case ship_in_battle::mode_approaching:
		case ship_in_battle::mode_engaged:
		{

	
			// always start retreating if the ship has 0% org and meets the define requirement for avg distance to center
			float cur_dist_center_line = state.world.naval_battle_get_avg_distance_from_center_line(battle);
			float min_dist_for_retreat = required_avg_dist_to_center_for_retreat(state);
			if(state.world.ship_get_org(ship.ship) <= 0.0f && cur_dist_center_line <= min_dist_for_retreat) {
				return true;
			}
			else if(state.world.ship_get_strength(ship.ship) <= state.defines.naval_combat_retreat_str_org_level ||
			state.world.ship_get_org(ship.ship) <= state.defines.naval_combat_retreat_str_org_level) {
				// compute random chance for an early retreat
				uint64_t random = rng::get_random(state, ship.ship.value * state.current_date.to_raw_value());
				if((random % 100000 + 1) <= (state.defines.naval_combat_retreat_chance * 100000)) {
					return true;
				}
			}
			break;
		}
		default:
		{
			break;
		}
		
	}
	return false;
}



// makes a single ship start retreating to disengage
void single_ship_start_retreat(sys::state& state, ship_in_battle& ship, dcon::naval_battle_id battle) {

	auto slots = state.world.naval_battle_get_slots(battle);

	ship.flags &= ~ship_in_battle::mode_mask;
	ship.flags |= ship_in_battle::mode_retreating;
	// decrement the number of ships targeting this ships' target, as it is now retreating and unable to target anything.
	ship_in_battle_set_target(ship, -1, slots);
}


void notify_on_new_naval_battle(sys::state& state, dcon::naval_battle_id battle, dcon::nation_id nation_as) {
	war_role battle_role = war_role::none;
	for(auto n : state.world.naval_battle_get_navy_battle_participation(battle)) {
		auto navy_controller = n.get_navy().get_controller_from_navy_control();
		if(navy_controller == nation_as) {
			if(is_attacker_in_battle(state, n.get_navy())) {
				assert(battle_role != war_role::defender);
				battle_role = war_role::attacker;
				break;
				
			}
			else {
				assert(battle_role != war_role::attacker);
				battle_role = war_role::defender;
				break;		
			}
		}

	}
	auto location = state.world.naval_battle_get_location_from_naval_battle_location(battle);
	// notify if attacking
	if(battle_role == war_role::attacker) {
		notification::post(state, notification::message{
			.body = [=](sys::state& state, text::layout_base& layout) {

				auto identity = state.world.nation_get_identity_from_identity_holder(nation_as);
				auto govt_type = state.world.nation_get_government_type(nation_as);
				auto ruler_name = state.world.national_identity_get_government_ruler_name(identity, govt_type);
				auto govt_type_ruler = state.world.government_type_get_ruler_name(govt_type);
				auto location_name = state.world.province_get_name(location);


				auto enemy_nation = get_naval_battle_lead_defender(state, battle);
				auto enemy_name = text::get_name(state, enemy_nation);

				text::add_line(state, layout, "NAVALATTACKTHEM_2", text::variable_type::monarchtitle, text::produce_simple_string(state, govt_type_ruler));
				text::add_line(state, layout, "NAVALATTACKTHEM_3", text::variable_type::prov, text::produce_simple_string(state, location_name));
				text::add_line(state, layout, "alice_navalattackitem_4", text::variable_type::defender, text::produce_simple_string(state, enemy_name));
			},

			.title = "NAVALATTACKTHEM_1",
			.source = nation_as,
			.target = dcon::nation_id{ },
			.third = dcon::nation_id{ },
			.type = sys::message_base_type::naval_combat_starts_by_nation,
			.province_source = location,

		});


	}
	// notify if defending
	else if(battle_role == war_role::defender) {
		notification::post(state, notification::message{
			.body = [=](sys::state& state, text::layout_base& layout) {

				auto identity = state.world.nation_get_identity_from_identity_holder(nation_as);
				auto govt_type = state.world.nation_get_government_type(nation_as);
				auto ruler_name = state.world.national_identity_get_government_ruler_name(identity, govt_type);
				auto govt_type_ruler = state.world.government_type_get_ruler_name(govt_type);
				auto location_name = state.world.province_get_name(location);


				auto enemy_nation = get_naval_battle_lead_attacker(state, battle);
				auto enemy_name = text::get_name(state, enemy_nation);

				text::add_line(state, layout, "NAVALATTACKUS_2", text::variable_type::monarchtitle, text::produce_simple_string(state, govt_type_ruler));
				text::add_line(state, layout, "NAVALATTACKUS_3", text::variable_type::prov, text::produce_simple_string(state, location_name));
				text::add_line(state, layout, "NAVALATTACKUS_4", text::variable_type::attacker, text::produce_simple_string(state, enemy_name));
			},

			.title = "NAVALATTACKUS_1",
			.source = nation_as,
			.target = dcon::nation_id{ },
			.third = dcon::nation_id{ },
			.type = sys::message_base_type::naval_combat_starts_on_nation,
			.province_source = location
		});

	}
}


/*
Naval battle general info:

When a battle starts, all ships will start 100 distance from the "center line", and will move towards it once a target is found. No ship may move past the center line into the enemy navies' half.
Manual retreating is only available once all non-retreating or sunk ships are on average close enough to the center line.
How close they have to be is specified in define:NAVAL_COMBAT_RETREAT_MIN_DISTANCE, which is treated as a decimal between 1 ( can retreat within 100 distance of the center), to 0 (Have to be exacly on the center)

When the manual retreat condition is not satisfied, automatic retreats from having very low org (Not sure about the exact threshold, but it appears to be quite low, so i set it to 0 org) are disabled aswell.
Ships can however still retreat even in this state if they get the random roll from define:NAVAL_COMBAT_RETREAT_CHANCE whilst being below define:NAVAL_COMBAT_RETREAT_STR_ORG_LEVEL strength or org.

When a manual retreat of navies are ordered, all ships which are part of the navy(ies) will start retreating, and the navy will only leave the battle once all of its ships are disengaged or sunk.

Ships are supposed to be "pushed back" from the center line when a new target is acquired, however the exact formula of how this is done is unknown (specifically how combat duration affects it), so currently it does not push them back

Ships are also supposed to fire conurrently with eachother rather than sequentially, however it does not do this so far. It isn't that important since navies will start outside of firing range of eachother, and speeds are partially randomized so the same side dosen't a deterministic "first shot"

If one side of a naval battle has less than a tenth of the total hull of the other side when the battle starts, the side with less hull is instantly stackwiped.

A navy is also stackwiped if there is no accessible ports for the retreat to path to (yes PORTS, not coast). 

*/



void update_naval_battles(sys::state& state) {
	auto isize = state.world.naval_battle_size();
	auto to_delete = ve::vectorizable_buffer<uint8_t, dcon::naval_battle_id>(isize);

	concurrency::parallel_for(0, int32_t(isize), [&](int32_t index) {
		dcon::naval_battle_id b{ dcon::naval_battle_id::value_base_t(index) };

		if(!state.world.naval_battle_is_valid(b))
			return;

		auto slots = state.world.naval_battle_get_slots(b);

		
		if(state.world.naval_battle_get_start_date(b) == state.current_date) {
			// notify if needed about new battle
			notify_on_new_naval_battle(state, b, state.local_player_nation);


			// compare total hull of new battles to see if it's an instant wipe
			float attacker_hull = 0;
			float defender_hull = 0;
			for(uint32_t j = slots.size(); j-- > 0;) {
				if(state.world.ship_is_valid(slots[j].ship)) {
					bool is_attacking = (slots[j].flags & ship_in_battle::is_attacking) != 0;
					auto controller = state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(slots[j].ship));
					auto type = state.world.ship_get_type(slots[j].ship);
					if(is_attacking) {
						attacker_hull += state.world.nation_get_unit_stats(controller, type).defence_or_hull;
					}
					else {
						defender_hull += state.world.nation_get_unit_stats(controller, type).defence_or_hull;
					}
				}
			}
			if(defender_hull * 10.0f < attacker_hull) {
				for(uint32_t j = slots.size(); j-- > 0;) {
					bool is_attacking = (slots[j].flags & ship_in_battle::is_attacking) != 0;
					if(!is_attacking) {
						uint16_t type = slots[j].flags & ship_in_battle::type_mask;
						auto unit_type = state.world.ship_get_type(slots[j].ship);
						switch(type) {
							case ship_in_battle::type_big:
							{
								state.world.naval_battle_set_defender_big_ships_lost(b, state.world.naval_battle_get_defender_big_ships_lost(b) + 1);
								break;
							}
							case ship_in_battle::type_small:
							{
								state.world.naval_battle_set_defender_small_ships_lost(b, state.world.naval_battle_get_defender_small_ships_lost(b) + 1);
								break;
								
							}
							case ship_in_battle::type_transport:
							{
								state.world.naval_battle_set_defender_transport_ships_lost(b, state.world.naval_battle_get_defender_transport_ships_lost(b) + 1);
								break;
							}
							default:
								assert(false);
						}
						state.world.naval_battle_set_defender_loss_value(b, state.world.naval_battle_get_defender_loss_value(b) + state.military_definitions.unit_base_definitions[unit_type].supply_consumption_score);
						state.world.delete_ship(slots[j].ship);
						slots[j].flags &= ~ship_in_battle::mode_mask;
						slots[j].flags |= ship_in_battle::mode_sunk;
						slots[j].ship = dcon::ship_id{ };
						to_delete.set(b, uint8_t(1));
						return;
						
					}
				}
			}
			else if(attacker_hull * 10.0f < defender_hull) {
				for(uint32_t j = slots.size(); j-- > 0;) {
					bool is_attacking = (slots[j].flags & ship_in_battle::is_attacking) != 0;
					if(is_attacking) {
						uint16_t type = slots[j].flags & ship_in_battle::type_mask;
						auto unit_type = state.world.ship_get_type(slots[j].ship);
						switch(type) {
							case ship_in_battle::type_big:
							{
								state.world.naval_battle_set_attacker_big_ships_lost(b, state.world.naval_battle_get_attacker_big_ships_lost(b) + 1);
								break;
							}
							case ship_in_battle::type_small:
							{
								state.world.naval_battle_set_attacker_small_ships_lost(b, state.world.naval_battle_get_attacker_small_ships_lost(b) + 1);
								break;

							}
							case ship_in_battle::type_transport:
							{
								state.world.naval_battle_set_attacker_transport_ships_lost(b, state.world.naval_battle_get_attacker_transport_ships_lost(b) + 1);
								break;
							}
							default:
								assert(false);
						}
						state.world.naval_battle_set_attacker_loss_value(b, state.world.naval_battle_get_attacker_loss_value(b) + state.military_definitions.unit_base_definitions[unit_type].supply_consumption_score);
						state.world.delete_ship(slots[j].ship);
						slots[j].flags &= ~ship_in_battle::mode_mask;
						slots[j].flags |= ship_in_battle::mode_sunk;
						slots[j].ship = dcon::ship_id{ };
						to_delete.set(b, uint8_t(2));
						return;

					}
				}
			}
		}
		
		int32_t attacker_ships = 0;
		int32_t defender_ships = 0;

		for(uint32_t j = slots.size(); j-- > 0;) {
			switch(slots[j].flags & ship_in_battle::mode_mask) {
			case ship_in_battle::mode_seeking:
			case ship_in_battle::mode_approaching:
			case ship_in_battle::mode_retreating:
			case ship_in_battle::mode_engaged:
				if((slots[j].flags & ship_in_battle::is_attacking) != 0)
					++attacker_ships;
				else
					++defender_ships;
				break;
			default:
				break;
			}
		}

		if(defender_ships == 0) {
			to_delete.set(b, uint8_t(1));
			return;
		} else if(attacker_ships == 0) {
			to_delete.set(b, uint8_t(2));
			return;
		}

		if((state.current_date.value - state.world.naval_battle_get_start_date(b).value) % 5 == 4) {
			state.world.naval_battle_set_dice_rolls(b, make_dice_rolls(state, uint32_t(index)));
		}

		auto both_dice = state.world.naval_battle_get_dice_rolls(b);
		auto attacker_dice = both_dice & 0x0F;
		auto defender_dice = (both_dice >> 4) & 0x0F;

		auto attacker_per = fatten(state.world, get_leader_personality_wrapper(state, state.world.naval_battle_get_admiral_from_attacking_admiral(b)));
		auto attacker_bg = fatten(state.world, get_leader_background_wrapper(state, state.world.naval_battle_get_admiral_from_attacking_admiral(b)));

		auto attack_bonus =
			int32_t(state.world.leader_trait_get_attack(attacker_per) + state.world.leader_trait_get_attack(attacker_bg));
		/*auto attacker_org_bonus =
				1.0f + state.world.leader_trait_get_organisation(attacker_per) + state.world.leader_trait_get_organisation(attacker_bg);*/

		auto defender_per = fatten(state.world, get_leader_personality_wrapper(state, state.world.naval_battle_get_admiral_from_defending_admiral(b)));
		auto defender_bg = fatten(state.world, get_leader_background_wrapper(state, state.world.naval_battle_get_admiral_from_defending_admiral(b)));

		auto atk_leader_exp_mod = 1 + attacker_per.get_experience() + attacker_bg.get_experience();
		auto def_leader_exp_mod = 1 + defender_per.get_experience() + defender_bg.get_experience();

		auto defence_bonus =
			int32_t(state.world.leader_trait_get_defense(defender_per) + state.world.leader_trait_get_defense(defender_bg));
		/*auto defender_org_bonus =
				1.0f + state.world.leader_trait_get_organisation(defender_per) + state.world.leader_trait_get_organisation(defender_bg);*/

		auto attacker_mod = combat_modifier_table[std::clamp(attacker_dice + attack_bonus + 3, 0, 18)];
		auto defender_mod = combat_modifier_table[std::clamp(defender_dice + defence_bonus + 3, 0, 18)];

		for(uint32_t j = slots.size(); j-- > 0;) {
			auto ship_owner =
				state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(slots[j].ship));
			auto ship_type = state.world.ship_get_type(slots[j].ship);
			auto& ship_battle = slots[j];
			assert((slots[j].flags & ship_in_battle::mode_mask) == ship_in_battle::mode_sunk || (slots[j].flags & ship_in_battle::mode_mask) == ship_in_battle::mode_retreated || ship_type);

			auto& ship_stats = state.world.nation_get_unit_stats(ship_owner, ship_type);

			auto aship = slots[j].ship;
			auto aship_owner =
				state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(aship));

			bool is_attacker = (slots[j].flags & ship_in_battle::is_attacking) != 0;

			switch(slots[j].flags & ship_in_battle::mode_mask) {
			case ship_in_battle::mode_approaching:
			{
				assert(naval_slot_index_valid(slots[j].target_slot));
				assert(state.world.ship_is_valid(slots[j].ship));

				auto target_mode = slots[slots[j].target_slot].flags & ship_in_battle::mode_mask;
				if(target_mode == ship_in_battle::mode_retreated || target_mode == ship_in_battle::mode_sunk) {
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_seeking;
					break;
				}

				/*
				An approaching ship:
				Has its distance reduced by (random-value-in-range-\[0.0 - 0.5) + 0.5) x max-speed x
				define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR * 100 to a minimum of 0. Switches to engaged when its distance + the
				target's distance is less than its fire range
				*/

				float speed = ship_stats.maximum_speed * naval_battle_speed_mult * state.defines.naval_combat_speed_to_distance_factor *
				(0.5f + float(rng::get_random(state, uint32_t(slots[j].ship.value)) & 0x7FFF) / float(0xFFFF));
				auto old_distance = slots[j].get_distance();
				auto adjust = std::clamp(uint16_t(speed), uint16_t(0), old_distance);
				slots[j].set_distance( old_distance - adjust);

				if(slots[j].get_distance() + slots[slots[j].target_slot].get_distance() < naval_battle_distance_to_center * ship_stats.reconnaissance_or_fire_range) {

					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_engaged;

					// do one round of attacking right after arriving at a new target, to reduce idle time while finding new targets
					auto leader_exp_mod = (is_attacker ? atk_leader_exp_mod : def_leader_exp_mod);
					ship_do_damage(state, b, slots[j], slots[slots[j].target_slot], is_attacker ? attacker_mod : defender_mod, leader_exp_mod, defender_ships, attacker_ships);
				}

				break;
			}
			case ship_in_battle::mode_engaged:
			{
				assert(naval_slot_index_valid(slots[j].target_slot));
				assert(state.world.ship_is_valid(slots[j].ship));
				
				auto target_mode = slots[slots[j].target_slot].flags & ship_in_battle::mode_mask;
				if(target_mode == ship_in_battle::mode_retreated || target_mode == ship_in_battle::mode_sunk) {
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_seeking;
					break;
				}

				auto distance = slots[j].get_distance();
				auto target_distance = slots[slots[j].target_slot].get_distance();
				// if target is out of range, switch to approaching
				if(distance + target_distance > naval_battle_distance_to_center * ship_stats.reconnaissance_or_fire_range) {
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_approaching;
					break;
				}
			

				auto leader_exp_mod = (is_attacker ? atk_leader_exp_mod : def_leader_exp_mod);

				ship_do_damage(state, b, slots[j], slots[slots[j].target_slot], is_attacker ? attacker_mod : defender_mod, leader_exp_mod, defender_ships, attacker_ships);


				break;
			}
			case ship_in_battle::mode_retreating:
			{
				/*
				A retreating ship will increase its distance by define:NAVAL_COMBAT_RETREAT_SPEED_MOD x
				define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR x (random value in the range \[0.0 - 0.5) + 0.5) x ship-max-speed.
				*/
				assert(state.world.ship_is_valid(slots[j].ship));
				float speed = ship_stats.maximum_speed * naval_battle_speed_mult * state.defines.naval_combat_retreat_speed_mod *
					state.defines.naval_combat_speed_to_distance_factor *
					(0.5f + float(rng::get_random(state, uint32_t(slots[j].ship.value)) & 0x7FFF) / float(0xFFFF));

				auto old_distance = slots[j].get_distance();
				auto new_distance = std::min(uint16_t(uint16_t(speed) + old_distance), naval_battle_distance_to_center);
				slots[j].set_distance(new_distance);

				// set to retreated if distance is far enough
				if(slots[j].get_distance() >= naval_battle_distance_to_center) {
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_retreated;
					if((slots[j].flags & ship_in_battle::is_attacking) != 0) {
						attacker_ships--;
					}
					else {
						defender_ships--;
					}
				}

				break;
			}
			case ship_in_battle::mode_seeking:
			{
				/*
				When a target is selected, distance is increased by random-value-in-range-\[0.0, 1.0) x (1.0 -
				combat-duration^define:NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) / NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) x
				NAVAL_COMBAT_SHIFT_BACK_ON_NEXT_TARGET to a maximum of 1000, and the ship switches to approaching.
				*/
				int16_t target_index = get_naval_battle_target(state, slots[j], b, defender_ships, attacker_ships);
				assert(state.world.ship_is_valid(slots[j].ship));
				// get ship target, put into target_index variable
				if(naval_slot_index_valid(target_index)) {

					ship_in_battle_set_target(slots[j], target_index, slots);

					auto old_distance = slots[j].get_distance();
					auto new_distance = std::min(uint16_t(old_distance + 40), naval_battle_distance_to_center);

					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_approaching;
					//slots[j].distance = new_distance;

				}
				break;
			}
			default:
				// if the ship is sunk, make sure no ships are still targeting it
				break;
			}
		}
		uint32_t sum_dist_from_center = 0;
		uint32_t count = 0;
		for(uint32_t j = slots.size(); j-- > 0;) { // calculate average distance from center line before testing retreats. Only counts non retreating or sunk units
			uint16_t mode = slots[j].flags & ship_in_battle::mode_mask;
			switch(mode) {
				case ship_in_battle::mode_approaching:
				case ship_in_battle::mode_engaged:
				case ship_in_battle::mode_seeking:
					sum_dist_from_center += slots[j].get_distance();
;					count++;
					break;
				default:
					break;
			}
			

		}
		if(count == 0) {
			count = 1;
		}
		state.world.naval_battle_set_avg_distance_from_center_line(b, float(sum_dist_from_center) / float(count));

		for(uint32_t j = slots.size(); j-- > 0;) { // test health&str to see if they should try to retreat

			if(should_ship_retreat(state, slots[j], b)) {
				single_ship_start_retreat(state, slots[j], b);
			}
		}

		// check if all ships of a retreating navy has fully retreated. If so then remove them from the battle
		std::vector<dcon::navy_id> to_retreat{ };
		// iterate though navies in battle backwards, to remove as we go if needed
		for(auto iterator = state.world.naval_battle_get_navy_battle_participation(b).end(); iterator.operator--() != state.world.naval_battle_get_navy_battle_participation(b).begin(); ) {
			auto navy = (*iterator).get_navy();
			if(navy.get_is_retreating()) {
				std::vector<ship_in_battle*> to_be_removed{ };

				bool can_retreat = [&]() {
					for(uint32_t j = slots.size(); j-- > 0;) {
						if(state.world.ship_get_navy_from_navy_membership(slots[j].ship) == navy) {
							if((slots[j].flags & ship_in_battle::mode_mask) != ship_in_battle::mode_retreated &&
							   (slots[j].flags & ship_in_battle::mode_mask) != ship_in_battle::mode_sunk) {
								return false;
							} else if((slots[j].flags & ship_in_battle::mode_mask) == ship_in_battle::mode_retreated) {
								to_be_removed.push_back(&slots[j]);
							}

						}

					}
					return true;
					}();

				if(can_retreat) {
					// anonymize ship ID of the ships that are retreating out of the battle. Targets depend on the index of the slots vector so can't remove em fully
					for(auto battle_ship : to_be_removed) {
						assert((battle_ship->flags & ship_in_battle::mode_mask) == ship_in_battle::mode_retreated);
						assert(state.world.ship_is_valid(battle_ship->ship));
						battle_ship->ship = dcon::ship_id{ };

					}
					state.world.delete_navy_battle_participation(*iterator);
				}

			}
		}
		if(!to_retreat.empty()) {
			update_battle_leaders(state, b);
		}
	});


	for(auto i = isize; i-- > 0;) {
		dcon::naval_battle_id b{ dcon::naval_battle_id::value_base_t(i) };
		if(state.world.naval_battle_is_valid(b) && to_delete.get(b) != 0) {
			end_battle(state, b, to_delete.get(b) == uint8_t(1) ? battle_result::attacker_won : battle_result::defender_won);
		}
	}

	for(uint32_t i = state.world.ship_size(); i-- > 0;) {
		dcon::ship_id s{ dcon::ship_id::value_base_t(i) };
		if(state.world.ship_is_valid(s)) {
			if(state.world.ship_get_strength(s) <= 0.0f) {
				state.world.delete_ship(s);
			}
		}
	}
}

uint8_t make_dice_rolls(sys::state& state, uint32_t seed) {
	auto rvals = rng::get_random_pair(state, seed);
	auto low_roll = rvals.low % 10;
	auto high_roll = rvals.high % 10;
	return uint8_t((high_roll << 4) | low_roll);
}

void navy_arrives_in_province(sys::state& state, dcon::navy_id n, dcon::province_id p, dcon::naval_battle_id from) {
	assert(state.world.navy_is_valid(n));
	assert(!state.world.navy_get_battle_from_navy_battle_participation(n));

	state.world.navy_set_location_from_navy_location(n, p);
	if(p.index() < state.province_definitions.first_sea_province.index()) {
		state.world.navy_set_months_outside_naval_range(n, uint8_t(0));
	}
	else {
		auto owner_nation = state.world.navy_get_controller_from_navy_control(n);
		if(province::sea_province_is_adjacent_to_accessible_coast(state, p, owner_nation)) {
			state.world.navy_set_months_outside_naval_range(n, uint8_t(0));
		}
	}
	auto ships = state.world.navy_get_navy_membership(n);
	if(!state.world.navy_get_is_retreating(n) && p.index() >= state.province_definitions.first_sea_province.index() && ships.begin() != ships.end()) {
		auto owner_nation = state.world.navy_get_controller_from_navy_control(n);

		// look for existing battle
		for(auto b : state.world.province_get_naval_battle_location(p)) {
			if(b.get_battle() != from) {
				auto battle_war = b.get_battle().get_war_from_naval_battle_in_war();
				auto owner_role = get_role(state, battle_war, owner_nation);
				if(owner_role != war_role::none) {
					add_navy_to_battle(state, n, b.get_battle(), owner_role);
					return; // done -- only one battle per
				}
			}
		}

		// start battle
		dcon::naval_battle_id gather_to_battle;
		dcon::war_id battle_in_war;

		for(auto o : state.world.province_get_navy_location(p)) {
			if(o.get_navy() == n)
				continue;
			if(o.get_navy().get_is_retreating() || o.get_navy().get_battle_from_navy_battle_participation())
				continue;

			auto other_nation = o.get_navy().get_controller_from_navy_control();

			if(auto par = internal_find_war_between(state, owner_nation, other_nation); par.role != war_role::none) {
				auto new_battle = fatten(state.world, state.world.create_naval_battle());
				new_battle.set_avg_distance_from_center_line(naval_battle_distance_to_center);
				new_battle.set_war_attacker_is_attacker(par.role == war_role::attacker);
				new_battle.set_start_date(state.current_date);
				new_battle.set_war_from_naval_battle_in_war(par.w);
				new_battle.set_location_from_naval_battle_location(p);
				new_battle.set_dice_rolls(make_dice_rolls(state, uint32_t(new_battle.id.value)));

				add_navy_to_battle(state, n, new_battle, par.role);
				add_navy_to_battle(state, o.get_navy(), new_battle,
						par.role == war_role::attacker ? war_role::defender : war_role::attacker);

				gather_to_battle = new_battle.id;
				battle_in_war = par.w;
				break;
			}
		}

		if(gather_to_battle) {
			for(auto o : state.world.province_get_navy_location(p)) {
				if(o.get_navy() == n)
					continue;
				if(o.get_navy().get_is_retreating() || o.get_navy().get_battle_from_navy_battle_participation())
					continue;

				auto other_nation = o.get_navy().get_controller_from_navy_control();

				if(auto role = get_role(state, battle_in_war, other_nation); role != war_role::none) {
					add_navy_to_battle(state, o.get_navy(), gather_to_battle, role);
				}
			}
		}
	}
}

void update_movement(sys::state& state) {
	// Army movement
	for(auto a : state.world.in_army) {
		auto arrival = a.get_arrival_time();
		auto path = a.get_path();
		auto from = state.world.army_get_location_from_army_location(a);
		auto army_owner = state.world.army_get_controller_from_army_control(a);
		assert(!arrival || arrival >= state.current_date);

		// US7AC1 Handle "move to siege" order
		if (path.size() > 0 && army_owner && a.get_special_order() == military::special_army_order::move_to_siege) {
			// Army was ordered to chain siege and it has not yet finished siege
			auto province_controller = state.world.province_get_nation_from_province_control(from);

			// Must be able to siege the province the army is in
			if(siege_potential(state, army_owner, province_controller) && state.world.province_get_nation_from_province_control(from) != army_owner && command::can_stop_army_movement(state, army_owner, a)) {
				// Delay the army until it finishes siege
				state.world.army_set_arrival_time(a, sys::date{ });
				state.world.army_set_unused_travel_days(a, 0.0f);
				continue;
			}
			else if (arrival == sys::date{}) {
				auto next_dest = path.at(path.size() - 1);
				auto arrival_data = arrival_time_to(state, a, next_dest);
				state.world.army_set_arrival_time(a, arrival_data.arrival_time);
				state.world.army_set_unused_travel_days(a, arrival_data.unused_travel_days);
			}
		}
		// US8AC1 Handle "strategic redeployment" order
		else if(path.size() > 0 && army_owner && a.get_special_order() == military::special_army_order::strategic_redeployment) {
			// While moving - limit the org
			for(auto r : state.world.army_get_army_membership(a)) {
				r.get_regiment().set_org(0.1f);
			}
		}
		// US8AC1 Movement finished - reset the order to let army reorg
		else if(path.size() == 0 && a.get_special_order() == military::special_army_order::strategic_redeployment) {
			a.set_special_order(military::special_army_order::none);
		}

		// US5AC1 Army arrives to province
		if(arrival == state.current_date) {
			assert(path.size() > 0);
			auto dest = path.at(path.size() - 1);
			path.pop_back();

			// Can the army reach the target
			if(dest.index() >= state.province_definitions.first_sea_province.index()) { // sea province
				// check for embarkation possibility, then embark
				auto to_navy = find_embark_target(state, a.get_controller_from_army_control(), dest, a);
				if(to_navy) {
					a.set_location_from_army_location(dest);
					a.set_navy_from_army_transport(to_navy);
					a.set_black_flag(false);
				} else {
					// if there are not enough transports by the time the movement happens, eject them back to land and check for enemy armies to collide with
					stop_army_movement(state, a);
					a.set_is_retreating(false);
					army_arrives_in_province(state, a, from, military::crossing_type::sea, dcon::land_battle_id{});
				}
			} else { // land province
				if(a.get_black_flag()) {
					auto n = state.world.province_get_nation_from_province_ownership(dest);
					// Since AI and pathfinding can lead armies into unowned provinces that are completely locked by other nations,
					// make armies go back to home territories for black flag removal
					if(n == a.get_controller_from_army_control().id) {
						a.set_black_flag(false);
					}
					army_arrives_in_province<apply_attrition_on_arrival::yes>(state, a, dest,
							(state.world.province_adjacency_get_type(state.world.get_province_adjacency_by_province_pair(dest, from)) &
								province::border::river_crossing_bit) != 0
									? military::crossing_type::river
									: military::crossing_type::none, dcon::land_battle_id{});
					a.set_navy_from_army_transport(dcon::navy_id{});
				} else if(province::has_access_to_province(state, a.get_controller_from_army_control(), dest)) {
					if(auto n = a.get_navy_from_army_transport()) {
						if(!n.get_battle_from_navy_battle_participation()) {
							army_arrives_in_province<apply_attrition_on_arrival::yes>(state, a, dest, military::crossing_type::sea, dcon::land_battle_id{});
							a.set_navy_from_army_transport(dcon::navy_id{});
						} else {
							stop_army_movement(state, a);
						}
					} else {
						auto path_bits = state.world.province_adjacency_get_type(state.world.get_province_adjacency_by_province_pair(dest, from));
						if((path_bits & province::border::non_adjacent_bit) != 0) { // strait crossing
							if(province::is_crossing_blocked(state, a.get_controller_from_army_control(), from, dest)) {
								// if the strait is blocked by the time the movement happens, stop the unit and check for enemy armies to collide with
								stop_army_movement(state, a);
								a.set_is_retreating(false);
								army_arrives_in_province(state, a, from, military::crossing_type::sea, dcon::land_battle_id{});
							} else {
								army_arrives_in_province<apply_attrition_on_arrival::yes>(state, a, dest, military::crossing_type::sea, dcon::land_battle_id{});
							}
						} else {
							army_arrives_in_province<apply_attrition_on_arrival::yes>(state, a, dest,
									(path_bits & province::border::river_crossing_bit) != 0
											? military::crossing_type::river
											: military::crossing_type::none, dcon::land_battle_id{});
						}
					}
				} else {
					// if the dest prov is inaccesible when the movement happens, stop movement and check for collision with enemy armies
					stop_army_movement(state, a);
					a.set_is_retreating(false);
					army_arrives_in_province(state, a, from, military::crossing_type::sea, dcon::land_battle_id{});
				}
			}

			
			if(!a.get_battle_from_army_battle_participation() && a.get_special_order() == military::special_army_order::pursue_to_engage) {
				auto target_army = a.get_army_pursuit_as_source().get_target();

				if(!target_army) {
					state.world.army_set_special_order(a, military::special_army_order::none);
					continue;
				}
				
				// Update the path
				auto npath = command::can_move_army(state, army_owner, a, state.world.army_get_location_from_army_location(target_army), true);

				// Has valid path and has to change direction
				if(npath.size() > 0) {
					auto new_next_dest = npath.at(npath.size() - 1);
					auto cur_next_dest = path.at(path.size() - 1);
					if(cur_next_dest != new_next_dest) {
						command::execute_move_army(state, army_owner, a, state.world.army_get_location_from_army_location(target_army), true, military::special_army_order::pursue_to_engage);
					}
				}
				else {
					// Continue moving to the last known location
					state.world.army_set_special_order(a, military::special_army_order::none);
				}
			}

			if(a.get_battle_from_army_battle_participation()) {
				// nothing -- movement paused
			}
			else if(path.size() > 0) {
				// Army was ordered chain move
				auto next_dest = path.at(path.size() - 1);
				update_movement_arrival_days_on_unit(state, next_dest, a.get_location_from_army_location(), a.id);
			} else {
				a.set_arrival_time(sys::date{});
				a.set_unused_travel_days(0.0f);
				if(a.get_is_retreating()) {
					a.set_is_retreating(false);
				}
				if(a.get_moving_to_merge()) {
					a.set_moving_to_merge(false);
					[&]() {
						for(auto ar : state.world.province_get_army_location(dest)) {
							if(ar.get_army().get_controller_from_army_control() == a.get_controller_from_army_control() && ar.get_army() != a && !ar.get_army().get_moving_to_merge()) {
								auto regs = state.world.army_get_army_membership(a);
								while(regs.begin() != regs.end()) {
									(*regs.begin()).set_army(ar.get_army());
								}
								return;
							}
						}
						}();
				}
				if(state.world.army_get_is_rebel_hunter(a)
					&& state.world.province_get_nation_from_province_control(dest)
					&& state.world.nation_get_is_player_controlled(state.world.army_get_controller_from_army_control(a))
					&& !state.world.army_get_battle_from_army_battle_participation(a)
					&& !state.world.army_get_navy_from_army_transport(a)) {

					military::send_rebel_hunter_to_next_province(state, a, state.world.army_get_location_from_army_location(a));
				}
			}
		}
		}

	// Navy movement
	for(auto n : state.world.in_navy) {
		auto arrival = n.get_arrival_time();
		assert(!arrival || arrival >= state.current_date);
		if(n.get_battle_from_navy_battle_participation() && bool(arrival)) {
			n.set_arrival_time(arrival + 1);
			continue;
		}
		if(auto path = n.get_path(); arrival == state.current_date) {
			assert(path.size() > 0);
			auto from = n.get_location_from_navy_location();
			auto dest = path.at(path.size() - 1);
			path.pop_back();

			if(dest.index() < state.province_definitions.first_sea_province.index()) { // land province
				if(province::has_naval_access_to_province(state, n.get_controller_from_navy_control(), dest)) {

					n.set_location_from_navy_location(dest);

					// check for whether there are troops to disembark
					auto attached = state.world.navy_get_army_transport(n);
					while(attached.begin() != attached.end()) {
						auto a = (*attached.begin()).get_army();

						a.set_navy_from_army_transport(dcon::navy_id{});
						stop_army_movement(state, a);
						auto acontroller = a.get_controller_from_army_control();

						// ai code
						if(acontroller && !acontroller.get_is_player_controlled()) {
							auto army_dest = a.get_ai_province();
							a.set_location_from_army_location(dest);
							if(army_dest && army_dest != dest) {
								auto apath = province::make_land_path(state, dest, army_dest, acontroller, a);
								if(apath.size() > 0) {
									move_army_fast(state, a, apath, acontroller);
									auto activity = ai::army_activity(a.get_ai_activity());
									if(activity == ai::army_activity::transport_guard) {
										a.set_ai_activity(uint8_t(ai::army_activity::on_guard));
									} else if(activity == ai::army_activity::transport_attack) {
										a.set_ai_activity(uint8_t(ai::army_activity::attack_gathered));
									}
								} else {
									a.set_ai_activity(uint8_t(ai::army_activity::on_guard));
								}
							} else {
								a.set_ai_activity(uint8_t(ai::army_activity::on_guard));
							}
						}
						army_arrives_in_province(state, a, dest, military::crossing_type::sea, dcon::land_battle_id{});
					}
				} else {
					// if the destination province becomes inaccesible by the time the movement happens, stop movement and check for enemy navy collision
					stop_navy_movement(state, n);
					n.set_is_retreating(false);
					navy_arrives_in_province(state, n, from, dcon::naval_battle_id{});
				}
			} else { // sea province

				auto adj = state.world.get_province_adjacency_by_province_pair(dest, from);
				auto path_bits = state.world.province_adjacency_get_type(adj);
				if((path_bits & province::border::non_adjacent_bit) != 0 && province::is_crossing_blocked(state, n.get_controller_from_navy_control(), adj)) { // hostile canal crossing
					// if the canal province becomes hostile by the time the movement happens, stop movement and check for enemy navy collision
					stop_navy_movement(state, n);
					n.set_is_retreating(false);
					navy_arrives_in_province(state, n, from, dcon::naval_battle_id{});
					
				}
				else {
					navy_arrives_in_province(state, n, dest, dcon::naval_battle_id{});

					// take embarked units along with
					for(auto a : state.world.navy_get_army_transport(n)) {
						a.get_army().set_location_from_army_location(dest);
						stop_army_movement(state, a.get_army());
					}
				}
			}

			if(path.size() > 0) {
				auto next_dest = path.at(path.size() - 1);
				update_movement_arrival_days_on_unit(state, next_dest, n.get_location_from_navy_location(), n.id);
			} else {
				n.set_arrival_time(sys::date{});
				n.set_unused_travel_days(0.0f);
				if(n.get_is_retreating()) {
					n.set_is_retreating(false);
				}
				if(n.get_moving_to_merge()) {
					n.set_moving_to_merge(false);
					[&]() {
						for(auto ar : state.world.province_get_navy_location(dest)) {
							if(ar.get_navy().get_controller_from_navy_control() == n.get_controller_from_navy_control() && ar.get_navy() != n && !ar.get_navy().get_moving_to_merge()) {
								auto regs = state.world.navy_get_navy_membership(n);
								while(regs.begin() != regs.end()) {
									(*regs.begin()).set_navy(ar.get_navy());
								}
								auto a = state.world.navy_get_army_transport(n);
								while(a.begin() != a.end()) {
									(*a.begin()).set_navy(ar.get_navy());
								}
								return;
							}
						}
						}();
				}
			}
		}
	}
}

template<typename T>
void update_movement_arrival_days_on_unit(sys::state& state, dcon::province_id to, dcon::province_id from, T army) {
	auto arrival_data = military::arrival_time_to_in_days(state, army, to, from);
	if constexpr(std::is_same<dcon::army_id, T>()) {
		auto& unused_travel_days = state.world.army_get_unused_travel_days(army);
		state.world.army_set_unused_travel_days(army, unused_travel_days + arrival_data.unused_travel_days);
		if(unused_travel_days >= 1.0f && arrival_data.travel_days != 1) {
			state.world.army_set_arrival_time(army, state.current_date + (arrival_data.travel_days - 1));
			state.world.army_set_unused_travel_days(army, unused_travel_days - 1.0f);
		} else {
			state.world.army_set_arrival_time(army, state.current_date + arrival_data.travel_days);
		}
	}
	else {
		auto& unused_travel_days = state.world.navy_get_unused_travel_days(army);
		state.world.navy_set_unused_travel_days(army, unused_travel_days + arrival_data.unused_travel_days);
		if(unused_travel_days >= 1.0f && arrival_data.travel_days != 1) {
			state.world.navy_set_arrival_time(army, state.current_date + (arrival_data.travel_days - 1));
			state.world.navy_set_unused_travel_days(army, unused_travel_days - 1.0f);
		} else {
			state.world.navy_set_arrival_time(army, state.current_date + arrival_data.travel_days);
		}
	}
	
}
template void update_movement_arrival_days_on_unit<dcon::army_id>(sys::state& state, dcon::province_id to, dcon::province_id from, dcon::army_id army);
template void update_movement_arrival_days_on_unit<dcon::navy_id>(sys::state& state, dcon::province_id to, dcon::province_id from, dcon::navy_id army);


template<typename T>
void update_movement_arrival_days(sys::state& state, dcon::province_id to, dcon::province_id from, T army, float& unused_travel_days, sys::date& arrival_time) {
	auto arrival_data = military::arrival_time_to_in_days(state, army, to, from);
	unused_travel_days += arrival_data.unused_travel_days;
	if(unused_travel_days >= 1.0f && arrival_data.travel_days != 1) {
		arrival_time += (arrival_data.travel_days - 1);
		unused_travel_days -= 1.0f;
	} else {
		arrival_time += arrival_data.travel_days;
	}
}

template void update_movement_arrival_days<dcon::navy_id>(sys::state& state, dcon::province_id to, dcon::province_id from, dcon::navy_id army, float& unused_travel_days, sys::date& arrival_time);
template void update_movement_arrival_days<dcon::army_id>(sys::state& state, dcon::province_id to, dcon::province_id from, dcon::army_id army, float& unused_travel_days, sys::date& arrival_time);

float fractional_distance_covered(sys::state& state, dcon::army_id a) {
	auto date = state.world.army_get_arrival_time(a);
	if(!date)
		return 0.0f;
	auto p = state.world.army_get_path(a);
	if(p.size() == 0)
		return 0.0f;
	if(state.world.army_get_battle_from_army_battle_participation(a))
		return 0.0f;

	auto dest = *(p.end() - 1);
	auto full_time = arrival_time_to(state, a, dest);

	auto difference = full_time.arrival_time.value - state.current_date.value;
	auto covered = date.value - state.current_date.value;

	if(difference <= 0)
		return 1.0f;

	return 1.0f - float(covered) / float(difference);
}
float fractional_distance_covered(sys::state& state, dcon::navy_id a) {
	auto date = state.world.navy_get_arrival_time(a);
	if(!date)
		return 0.0f;
	auto p = state.world.navy_get_path(a);
	if(p.size() == 0)
		return 0.0f;
	if(state.world.navy_get_battle_from_navy_battle_participation(a))
		return 0.0f;

	auto dest = *(p.end() - 1);
	auto full_time = arrival_time_to(state, a, dest);

	auto difference = full_time.arrival_time.value - state.current_date.value;
	auto covered = date.value - state.current_date.value;

	if(difference <= 0)
		return 1.0f;

	return 1.0f - float(covered) / float(difference);
}

int32_t transport_capacity(sys::state& state, dcon::navy_id n) {
	int32_t total = 0;
	for(auto s : state.world.navy_get_navy_membership(n)) {
		if(state.military_definitions.unit_base_definitions[s.get_ship().get_type()].type == unit_type::transport)
			++total;
	}
	return total;
}
int32_t free_transport_capacity(sys::state& state, dcon::navy_id n) {
	int32_t used_total = 0;
	for(auto a : state.world.navy_get_army_transport(n)) {
		auto regs = a.get_army().get_army_membership();
		used_total += int32_t(regs.end() - regs.begin());
	}
	return transport_capacity(state, n) - used_total;
}

constexpr inline float siege_speed_mul = 1.0f / 37.5f;

void send_rebel_hunter_to_next_province(sys::state& state, dcon::army_id ar, dcon::province_id prov) {
	auto a = fatten(state.world, ar);
	auto controller = a.get_controller_from_army_control();
	static std::vector<rebel::impl::prov_str> rebel_provs;
	rebel_provs.clear();
	rebel::get_hunting_targets(state, controller, rebel_provs);
	rebel::sort_hunting_targets(state, rebel::impl::arm_str{ ar, ai::estimate_army_offensive_strength(state, ar) }, rebel_provs);

	for(auto& next_prov : rebel_provs) {
		if(prov == next_prov.p)
			continue;

		auto path = province::make_land_path(state, prov, next_prov.p, controller, a);
		if(move_army_fast(state, a, path, controller)) {
			break;
		}
	}
	if(!a.get_arrival_time()) {
		auto home = a.get_ai_province();
		if(home == prov)
			return;

		auto path = province::make_land_path(state, prov, home, controller, a);
		move_army_fast(state, a, path, controller);
	}
}

bool siege_potential(sys::state& state, dcon::nation_id army_controller, dcon::nation_id province_controller) {
	bool will_siege = false;
	if(!army_controller) {					 // rebel army
		will_siege = bool(province_controller); // siege anything not rebel controlled
	} else {
		if(!province_controller) {
			will_siege = true; // siege anything rebel controlled
		} else if(are_at_war(state, province_controller, army_controller)) {
			will_siege = true;
		}
	}

	return will_siege;
}

// US5AC2 Army siege
void update_siege_progress(sys::state& state) {
	static auto new_nation_controller = ve::vectorizable_buffer<dcon::nation_id, dcon::province_id>(state.world.province_size());
	static auto new_rebel_controller = ve::vectorizable_buffer<dcon::rebel_faction_id, dcon::province_id>(state.world.province_size());
	province::ve_for_each_land_province(state, [&](auto ids) {
		new_nation_controller.set(ids, dcon::nation_id{});
		new_rebel_controller.set(ids, dcon::rebel_faction_id{});
	});

	concurrency::parallel_for(0, state.province_definitions.first_sea_province.index(), [&](int32_t id) {
		dcon::province_id prov{ dcon::province_id::value_base_t(id) };

		auto controller = state.world.province_get_nation_from_province_control(prov);
		auto owner = state.world.province_get_nation_from_province_ownership(prov);

		if(!owner) // skip unowned provinces
			return;

		float max_siege_value = 0.0f;
		float strength_siege_units = 0.0f;
		float max_recon_value = 0.0f;
		float strength_recon_units = 0.0f;
		float total_sieging_strength = 0.0f;
		bool owner_involved = false;
		bool core_owner_involved = false;

		dcon::army_id first_army;

		for(auto ar : state.world.province_get_army_location(prov)) {
			// Only stationary, non black flagged regiments with at least 0.001 strength contribute to a siege.

			if(ar.get_army().get_battle_from_army_battle_participation() || ar.get_army().get_black_flag() ||
					ar.get_army().get_navy_from_army_transport() || ar.get_army().get_arrival_time()) {

				// skip -- blackflag or embarked or moving or fighting
			} else {
				auto army_controller = ar.get_army().get_controller_from_army_control();

				if(siege_potential(state, army_controller, controller)) {
					if(!first_army)
						first_army = ar.get_army();

					auto army_stats = army_controller ? army_controller : ar.get_army().get_army_rebel_control().get_controller().get_ruler_from_rebellion_within();

					owner_involved = owner_involved || owner == army_controller;
					core_owner_involved =
						core_owner_involved || bool(state.world.get_core_by_prov_tag_key(prov, state.world.nation_get_identity_from_identity_holder(army_controller)));

					for(auto r : ar.get_army().get_army_membership()) {
						auto reg_str = r.get_regiment().get_strength();
						if(reg_str > 0.001f) {
							auto type = r.get_regiment().get_type();
							auto& stats = state.world.nation_get_unit_stats(army_stats, type);

							total_sieging_strength += reg_str;

							if(stats.siege_or_torpedo_attack > 0.0f) {
								strength_siege_units += reg_str;
								max_siege_value = std::max(max_siege_value, stats.siege_or_torpedo_attack);
							}
							if(stats.reconnaissance_or_fire_range > 0.0f) {
								strength_recon_units += reg_str;
								max_recon_value = std::max(max_recon_value, stats.reconnaissance_or_fire_range);
							}
						}
					}
				}
			}
		}

		if(total_sieging_strength == 0.0f) {
			// Garrison recovers at 10% per day when not being sieged (to 100%)

			auto local_battles = state.world.province_get_land_battle_location(prov);
			if(local_battles.begin() != local_battles.end()) {
				// ongoing battle: do nothing
			} else {
				auto& progress = state.world.province_get_siege_progress(prov);
				state.world.province_set_siege_progress(prov, std::max(0.0f, progress - 0.1f));
			}
		} else {
			assert(bool(first_army));

			/*
			We find the effective level of the fort by subtracting: (rounding this value down to to the nearest integer)
			greatest-siege-value-present x
			((the ratio of the strength of regiments with siege present to the total strength of all regiments) ^
			define:ENGINEER_UNIT_RATIO) / define:ENGINEER_UNIT_RATIO, reducing it to a minimum of 0.
			*/

			int32_t effective_fort_level = get_effective_fort_level(state, prov, total_sieging_strength, strength_siege_units, max_siege_value);

			/*
			We calculate the siege speed modifier as: 1 + define:RECON_SIEGE_EFFECT x greatest-reconnaissance-value-present x ((the
			ratio of the strength of regiments with reconnaissance present to the total strength of all regiments) ^
			define:RECON_UNIT_RATIO) / define:RECON_UNIT_RATIO.
			*/

			float siege_speed_modifier =
				1.0f + state.defines.recon_siege_effect * max_recon_value *
				std::min(strength_recon_units / total_sieging_strength, state.defines.recon_unit_ratio) /
				state.defines.recon_unit_ratio;

			/*
			 We calculate the modifier for number of brigades: first we get the "number of brigades" as total-strength-of-regiments x
			1000 / define:POP_SIZE_PER_REGIMENT, and capping it to at most define:SIEGE_BRIGADES_MAX. Then we calculate the bonus as
			(number-of-brigades - define:SIEGE_BRIGADES_MIN) x define:SIEGE_BRIGADES_BONUS if number-of-brigades is greater the minimum,
			and as number-of-brigades / define:SIEGE_BRIGADES_MIN otherwise.
			*/

			float num_brigades =
				std::min(state.defines.siege_brigades_max, total_sieging_strength * 1000.0f / state.defines.pop_size_per_regiment);
			float num_brigades_modifier =
				num_brigades > state.defines.siege_brigades_min
				? 1.0f + (num_brigades - state.defines.siege_brigades_min) * state.defines.siege_brigades_bonus
				: num_brigades / state.defines.siege_brigades_min;

			/*
			Finally, the amount subtracted from the garrison each day is:
			siege-speed-modifier x number-of-brigades-modifier x Progress-Table\[random-int-from-0-to-9\] x (1.25 if the owner is
			sieging it back) x (1.1 if the sieger is not the owner but does have a core) / (effective_fort_level x define:ALICE_FORT_SIEGE_SLOWDOWN + 1.0)
			*/

			static constexpr float progress_table[] = { 0.0f, 0.2f, 0.5f, 0.75f, 0.75f, 1, 1.1f, 1.1f, 1.25f, 1.25f };

			float added_progress = siege_speed_modifier * num_brigades_modifier *
				progress_table[rng::get_random(state, uint32_t(prov.value)) % 10] *
				(owner_involved ? 1.25f : (core_owner_involved ? 1.1f : 1.0f)) / (effective_fort_level * state.defines.alice_fort_siege_slowdown + 1.0f);

			auto& progress = state.world.province_get_siege_progress(prov);
			state.world.province_set_siege_progress(prov, progress + siege_speed_mul * added_progress);

			if(progress >= 1.0f) {
				progress = 0.0f;

				/*
				The garrison returns to 100% immediately after the siege is complete and the controller changes. If your siege returns a
				province to its owner's control without the owner participating, you get +2.5 relations with the owner.
				*/

				// if siege won from rebels : divide by reduction_after_reoccupation define
				auto old_rf = state.world.province_get_rebel_faction_from_province_rebel_control(prov);
				if(old_rf) {
					for(auto pop : state.world.province_get_pop_location(prov)) {
						if(pop.get_pop().get_rebel_faction_from_pop_rebellion_membership() == old_rf) {
							auto mil = pop_demographics::get_militancy(state, pop.get_pop()) / state.defines.alice_rebel_reduction_after_reoccupation;
							pop_demographics::set_militancy(state, pop.get_pop().id, mil);
						}
					}
				}

				state.world.province_set_former_controller(prov, controller);
				state.world.province_set_former_rebel_controller(prov, old_rf);

				auto new_controller = state.world.army_get_controller_from_army_control(first_army);
				auto rebel_controller = state.world.army_get_controller_from_army_rebel_control(first_army);

				if((!new_controller && !rebel_controller) || (new_controller && !are_at_war(state, new_controller, owner))) {
					new_controller = owner;
				}

				new_nation_controller.set(prov, new_controller);
				new_rebel_controller.set(prov, rebel_controller);
			}
		}
	});

	province::for_each_land_province(state, [&](dcon::province_id prov) {
		if(auto nc = new_nation_controller.get(prov); nc) {
			province::set_province_controller(state, prov, nc);
			eject_ships(state, prov);

			auto cc = state.world.province_get_nation_from_province_control(prov);
			auto oc = state.world.province_get_former_controller(prov);

			if(cc || oc) {
				notification::post(state, notification::message{
					[prov, cc, oc,
						crc = state.world.province_get_rebel_faction_from_province_rebel_control(prov),
						orc = state.world.province_get_former_rebel_controller(prov)](sys::state& state, text::layout_base& contents) {

						text::add_line(state, contents, "msg_siegeover_1", text::variable_type::x, prov);
						if(oc)
							text::add_line(state, contents, "msg_siegeover_2", text::variable_type::x, oc);
						else
							text::add_line(state, contents, "msg_siegeover_3", text::variable_type::x, state.world.rebel_type_get_title(state.world.rebel_faction_get_type(orc)));
						if(cc)
							text::add_line(state, contents, "msg_siegeover_4", text::variable_type::x, cc);
						else
							text::add_line(state, contents, "msg_siegeover_5", text::variable_type::x, state.world.rebel_type_get_title(state.world.rebel_faction_get_type(crc)));
					},
					"msg_siegeover_title",
					cc, oc, dcon::nation_id{},
					sys::message_base_type::siegeover,
					dcon::province_id{ }
				});
			}

			for(auto ar : state.world.province_get_army_location(prov)) {
				auto a = ar.get_army();

				if(a.get_is_rebel_hunter()
					&& a.get_controller_from_army_control().get_is_player_controlled()
					&& !a.get_battle_from_army_battle_participation()
					&& !a.get_navy_from_army_transport()
					&& !a.get_arrival_time()) {

					send_rebel_hunter_to_next_province(state, a, prov);

				}
			}

			/*
			TODO: When a province controller changes as the result of a siege, and it does not go back to the owner a random,
			`on_siege_win` event is fired, subject to the conditions of the events being met.
			*/
			// is controler != owner ...
			// event::fire_fixed_event(state, );
		}
		if(auto nr = new_rebel_controller.get(prov); nr) {
			province::set_province_controller(state, prov, nr);
			eject_ships(state, prov);

			auto cc = state.world.province_get_nation_from_province_control(prov);
			auto oc = state.world.province_get_former_controller(prov);
			auto within = state.world.rebel_faction_get_ruler_from_rebellion_within(nr);
			auto t = state.world.rebel_faction_get_type(nr);
			if(trigger::evaluate(state, state.world.rebel_type_get_siege_won_trigger(t), trigger::to_generic(prov), trigger::to_generic(prov), trigger::to_generic(nr))) {
				effect::execute(state, state.world.rebel_type_get_siege_won_effect(t), trigger::to_generic(prov), trigger::to_generic(prov), trigger::to_generic(nr), uint32_t(state.current_date.value), uint32_t(within.index() ^ (nr.index() << 4)));
			}
		}
	});
}

void update_blackflag_status(sys::state& state, dcon::province_id p) {
	for(auto ar : state.world.province_get_army_location(p)) {
		if(!ar.get_army().get_battle_from_army_battle_participation() && !ar.get_army().get_navy_from_army_transport()) {
			auto controller = ar.get_army().get_controller_from_army_control();
			ar.get_army().set_black_flag(!province::has_access_to_province(state, controller, p));
		}
	}
}

void eject_ships(sys::state& state, dcon::province_id p) {
	auto sea_zone = state.world.province_get_port_to(p);

	if(!sea_zone)
		return;

	static std::vector<dcon::navy_id> to_eject;
	to_eject.clear();

	for(auto n : state.world.province_get_navy_location(p)) {
		if(!province::has_naval_access_to_province(state, n.get_navy().get_controller_from_navy_control(), p)) {
			to_eject.push_back(n.get_navy().id);
		}
	}
	for(auto n : to_eject) {
		navy_arrives_in_province(state, n, sea_zone, dcon::naval_battle_id{});

		for(auto a : state.world.navy_get_army_transport(n)) {
			a.get_army().set_location_from_army_location(sea_zone);
			stop_army_movement(state, a.get_army());
		}
	}
}

void increase_dig_in(sys::state& state) {
	if(state.current_date.value % int32_t(state.defines.dig_in_increase_each_days) == 0) {
		for(auto ar : state.world.in_army) {
			if(ar.get_is_retreating() || ar.get_black_flag() || bool(ar.get_battle_from_army_battle_participation()) ||
					bool(ar.get_navy_from_army_transport()) || bool(ar.get_arrival_time())) {

				continue;
			}
			auto& current_dig_in = ar.get_dig_in();
			if(current_dig_in <
					int32_t(ar.get_controller_from_army_control().get_modifier_values(sys::national_mod_offsets::dig_in_cap))) {
				ar.set_dig_in(uint8_t(current_dig_in + 1));
			}
		}
	}
}

economy::commodity_set get_required_supply(sys::state& state, dcon::nation_id owner, dcon::army_id army) {
	uint32_t total_commodities = state.world.commodity_size();

	economy::commodity_set commodities;
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		commodities.commodity_amounts[i] = 0.0f;
	}

	for(auto r : state.world.army_get_army_membership(army)) {
		auto reg = fatten(state.world, r);
		auto type = state.world.regiment_get_type(r.get_regiment());

		auto o_sc_mod = std::max(0.01f, state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
		auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(supply_cost.commodity_type[i]) {
				commodities.commodity_amounts[i] += supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption * o_sc_mod;
				commodities.commodity_type[i] = supply_cost.commodity_type[i];
			} else {
				break;
			}
		}

	}

	return commodities;
}

economy::commodity_set get_required_supply(sys::state& state, dcon::nation_id owner, dcon::navy_id navy) {
	// supply amount = type_consumption * (2 - admin_eff)*[(type_consumption_mod^0.01)*land_spending]
	float supply_amount = .0f;
	int32_t amount_of_units = 0;

	economy::commodity_set commodities;
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		commodities.commodity_amounts[i] = 0.0f;
	}

	for(auto sh : state.world.navy_get_navy_membership(navy)) {
		auto shp = fatten(state.world, sh.get_ship());
		auto type = state.world.ship_get_type(sh.get_ship());

		if(owner) {
			auto o_sc_mod = std::max(0.01f, state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::supply_consumption) + 1.0f);
			auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
				if(supply_cost.commodity_type[i]) {
					commodities.commodity_amounts[i] +=
						supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(owner, type).supply_consumption *
						o_sc_mod;
					commodities.commodity_type[i] = supply_cost.commodity_type[i];
				} else {
					break;
				}
			}
		}
	};

	return commodities;
}

void recover_org(sys::state& state) {
	/*
	- Units that are not on the frontline of a battle, and not embarked recover organization daily at: (national-organization-regeneration-modifier
	+ morale-from-tech + leader-morale-trait + 1) x the-unit's-supply-factor / 5 up to the maximum organization of 100%
	- Additionally, the prestige of the leader factors in morale as unit-morale
	+ (leader-prestige x defines:LEADER_PRESTIGE_TO_MORALE_FACTOR).
	- Similarly, unit-max-org + (leader-prestige x defines:LEADER_PRESTIGE_TO_MAX_ORG_FACTOR) allows for maximum org.
	*/

	for(auto ar : state.world.in_army) {
		if(ar.get_navy_from_army_transport() || ar.get_black_flag())
			continue;

		auto in_nation = ar.get_controller_from_army_control();
		auto tech_nation = in_nation ? in_nation : ar.get_controller_from_army_rebel_control().get_ruler_from_rebellion_within();

		auto leader = ar.get_general_from_army_leadership();

		// US13AC3 US13AC4 US13AC5 Morale (Organization Regain): increases a unit's organization by 0.01 * discipline for each % of morale.
		// Max org is applied in battle
		auto regen_mod = tech_nation.get_modifier_values(sys::national_mod_offsets::org_regain)
			+ leader.get_personality().get_morale() + leader.get_background().get_morale() + 1.0f
			+ leader.get_prestige() * state.defines.leader_prestige_to_morale_factor;
		// US13AC2
		auto spending_level = (in_nation ? in_nation.get_effective_land_spending() : 1.0f);
		auto army_regen = regen_mod * spending_level / 150.f;
		for(auto reg : ar.get_army_membership()) {
			if(reg.get_regiment().get_army_from_army_membership().get_battle_from_army_battle_participation() && !is_regiment_in_reserve(state, reg.get_regiment())) {
				continue;
			}
			// the max org divisor to org recovery is calculated by getting the effective default org (ie max org) of a unit, and dividing it by 30.
			// 30 is the starting default org for most units, so org regen is normalized to what it was previously
			// this will scale the org regen to the actual max org of the unit

			auto max_org_divisor = unit_get_effective_default_org(state, reg.get_regiment()) / 30;
			auto reg_regen = army_regen / max_org_divisor;

			auto c_org = reg.get_regiment().get_org();
			// US13AC7 Unfulfilled supply doesn't lower max org as it makes half the game unplayable
			// US13AC8 Unfilfilled supply doesn't prevent org regain as it makes half the game unplayable
			// US13AC6 Max organization of the regiment is 100% (1.0)
			auto max_org = 1.f;
			reg.get_regiment().set_org(std::min(c_org + reg_regen, max_org));
		}
	}

	// US17
	for(auto ar : state.world.in_navy) {
		if(ar.get_navy_battle_participation().get_battle())
			continue;

		auto in_nation = ar.get_controller_from_navy_control();

		auto leader = ar.get_admiral_from_navy_leadership();
		auto regen_mod = in_nation.get_modifier_values(sys::national_mod_offsets::org_regain)
			+ leader.get_personality().get_morale() + leader.get_background().get_morale() + 1.0f
			+ leader.get_prestige() * state.defines.leader_prestige_to_morale_factor;
		float oversize_amount =
			in_nation.get_naval_supply_points() > 0
			? std::min(float(in_nation.get_used_naval_supply_points()) / float(in_nation.get_naval_supply_points()), 1.75f)
			: 1.75f;
		float over_size_penalty = oversize_amount > 1.0f ? 2.0f - oversize_amount : 1.0f;
		auto spending_level = in_nation.get_effective_naval_spending() * over_size_penalty;
		auto navy_regen = regen_mod * spending_level / 150.0f;
		for(auto reg : ar.get_navy_membership()) {
			auto c_org = reg.get_ship().get_org();

			auto max_org_divisor = unit_get_effective_default_org(state, reg.get_ship()) / 30;

			auto ship_regen = navy_regen / max_org_divisor;
			// Unfulfilled supply doesn't lower max org as it makes half the game unplayable
			auto max_org = std::max(c_org, 0.25f + 0.75f * spending_level);
			reg.get_ship().set_org(std::min(c_org + ship_regen, max_org));
		}
	}
}
// stops the unit movement completly and clears all other auxillary movement effects (arrival date, path etc)
void stop_army_movement(sys::state& state, dcon::army_id army) {
	assert(army);
	state.world.army_get_path(army).clear();
	state.world.army_set_arrival_time(army, sys::date{ });
	state.world.army_set_unused_travel_days(army, 0.0f);
}

// stops the unit movement completly and clears all other auxillary movement effects (arrival date, path etc)
void stop_navy_movement(sys::state& state, dcon::navy_id navy) {
	assert(navy);
	state.world.navy_get_path(navy).clear();
	state.world.navy_set_arrival_time(navy, sys::date{ });
	state.world.navy_set_unused_travel_days(navy, 0.0f);
}




// Just a wrapper for regiment_get_strength and ship_get_strength where unit is unknown
float unit_get_strength(sys::state& state, dcon::regiment_id regiment_id) {
	return state.world.regiment_get_strength(regiment_id);
}
// Just a wrapper for regiment_get_strength and ship_get_strength where unit is unknown
float unit_get_strength(sys::state& state, dcon::ship_id ship_id) {
	return state.world.ship_get_strength(ship_id);
}

bool province_has_enemy_unit(sys::state& state, dcon::province_id location, dcon::nation_id our_nation) {
	for(auto army : state.world.province_get_army_location(location)) {
		if(!army.get_army()) {
			// no armies present
			return false;
		}
		if(are_enemies(state, our_nation, army.get_army().get_controller_from_army_control())) {
			return true;
		}
	}
	return false;
}
bool province_has_enemy_fleet(sys::state& state, dcon::province_id location, dcon::nation_id our_nation) {
	for(auto navy : state.world.province_get_navy_location(location)) {
		if(!navy.get_navy()) {
			// no fleet present
			return false;
		} else if(are_at_war(state, our_nation, navy.get_navy().get_controller_from_navy_control())) {
			// someone who we are at war with has a fleet in the province
			return true;
		}
	}
	return false;
}
// returns true if there is a battle at the location, where one of the participants is an enemy to our_nation
//bool enemy_battle(sys::state& state, dcon::province_id location, dcon::nation_id our_nation) {
//	auto battle = get_province_battle(state, location);
//	if(battle) {
//		for(auto par : state.world.land_battle_get_army_battle_participation(battle)) {
//			auto army_controller = par.get_army().get_controller_from_army_control();
//			if(are_at_war(state, our_nation, army_controller)) {
//				return true;
//			}
//		}
//	}
//	return false;
//}


/* === Army reinforcement === */

// returns true if the province "location" will get a reinforce bonus by being adjacent to a "allied" province.
// checks if the province parameter "location" is adjacent to a "allied" controlled province from the perspective of the "our_nation" param.
// "allied" means either: controlled by yourself, or controlled by an nation who is fighting the controller of "location"
bool get_allied_prov_adjacency_reinforcement_bonus(sys::state& state, dcon::province_id location, dcon::nation_id our_nation) {
	auto location_controller = state.world.province_get_nation_from_province_control(location);
	for(auto adj : state.world.province_get_province_adjacency(location)) {
		auto indx = adj.get_connected_provinces(0).id != location ? 0 : 1;
		auto prov = adj.get_connected_provinces(indx);

		if(prov.id.index() >= state.province_definitions.first_sea_province.index() || province::is_crossing_blocked(state, our_nation, location, prov) ||
			!state.world.province_get_nation_from_province_ownership(prov)) {
			// if its a sea province, a blockaded sea strait or uncolonized
			return false;
		}
		auto prov_controller = state.world.province_get_nation_from_province_control(prov);
		// enemy battles or units will not allow for reinforcements
		if(province_has_enemy_unit(state, prov, our_nation)) {
			return false;
		}
		// checks if the province controlled by us, or is controlled by someone who is at enemies with the owner of location
		else if(prov_controller == our_nation || (are_enemies(state, location_controller, prov_controller))) {
			return true;
		}
	}
	return false;
}


// calculate the reinforcement location mod for units not in a battle
float calculate_location_reinforce_modifier_no_battle(sys::state& state, dcon::province_id location, dcon::nation_id in_nation) {
	float location_modifier = 1.0f;
	auto location_controller = state.world.province_get_nation_from_province_control(location);
	auto location_owner = state.world.province_get_nation_from_province_ownership(location);
	// if we arent rebels
	if(bool(in_nation)) {
		// in your owned territory, occupied or not
		if(location_owner == in_nation) {
			location_modifier = 2.0f;
		}
		// uncolonized (unowned) territory
		else if(!location_owner) {
			// if its a coastal uncolonized prov
			if(state.world.province_get_is_coast(location)) {
				location_modifier = 0.1f;
			} else {
				location_modifier = 0.0f;
			}
		}
		//if you are at war with the location controller, or the controller is rebels
		else if(are_enemies(state, in_nation, location_controller)) {
			// if we are eligible to get the 50% bonus by being adj to an allied province
			if(get_allied_prov_adjacency_reinforcement_bonus(state, location, in_nation)) {

				location_modifier = 0.5f;
			}
			// if its coastal and not blockaded by the enemy, give 25%
			else if(state.world.province_get_is_coast(location) && !province_is_blockaded_by_enemy(state, location, in_nation)) {
				location_modifier = 0.25f;
			}
			// if its neither, give 10%
			else {
				location_modifier = 0.1f;
			}

		}
		// if the units has access to the province, if they dont, they are blackflagged and shall get no reinforcements
		else if(!province::has_access_to_province(state, in_nation, location)) {
			location_modifier = 0.0f;
		}
		// territory whom we do not own, but are not at war with, while having access to it, 100% bonus
		else {
			location_modifier = 1.0f;
		}
	}
	//if we are rebels
	else {
		// if it is uncolonized
		if(!location_owner) {
			location_modifier = 0.0f;
		} else if(!location_controller) {
			location_modifier = 1.0f;
		} else {
			if(get_allied_prov_adjacency_reinforcement_bonus(state, location, in_nation)) {

				location_modifier = 0.5f;
			} else {
				// rebels get no reinforcements if they dont control any provinces
				location_modifier = 0.0f;
			}
		}
	}
	return location_modifier;

}



// calculate the reinforcement location mod for units in a battle
float calculate_location_reinforce_modifier_battle(sys::state& state, dcon::province_id location, dcon::nation_id in_nation) {
	float highest_adj_prov_modifier = 0.0f;
	// iterate over adjacent provinces
	for(auto adj : state.world.province_get_province_adjacency(location)) {
		auto indx = adj.get_connected_provinces(0).id != location ? 0 : 1;
		auto prov = adj.get_connected_provinces(indx);
		if(prov.id.index() >= state.province_definitions.first_sea_province.index() || province::is_crossing_blocked(state, in_nation, location, prov)) {
			// if it is a sea province, or a blockaded sea strait, ignore it
			continue;
		}
		// if there are enemy battles or enemy units sourrinding the province, it will get no reinforcements
		if(province_has_enemy_unit(state, prov, in_nation)) {
			highest_adj_prov_modifier = std::max(highest_adj_prov_modifier, 0.0f);
		} else {
			highest_adj_prov_modifier = std::max(highest_adj_prov_modifier, calculate_location_reinforce_modifier_no_battle(state, prov, in_nation));
		}
	}
	return highest_adj_prov_modifier;


}


// Calculates max reinforcement for units in the army
template<reinforcement_estimation_type reinf_est_type>
float calculate_army_combined_reinforce(sys::state& state, dcon::army_id a) {
	auto ar = fatten(state.world, a);
	if(ar.get_navy_from_army_transport() || ar.get_is_retreating() || ar.get_black_flag())
		return 0.0f;

	auto in_nation = ar.get_controller_from_army_control();
	auto tech_nation = in_nation ? in_nation : ar.get_controller_from_army_rebel_control().get_ruler_from_rebellion_within();

	float reinf_fufillment = 0.0f;

	switch(reinf_est_type) {
		case reinforcement_estimation_type::today:
			reinf_fufillment = (in_nation ? std::clamp(state.world.nation_get_land_reinforcement_buffer(in_nation) / economy::unit_reinforcement_demand_divisor, 0.f, 1.f) : 1.0f);
			break;
		case reinforcement_estimation_type::monthly:
			reinf_fufillment = (in_nation ? state.world.nation_get_effective_land_spending(in_nation) : 1.0f);
			break;
		case reinforcement_estimation_type::full_supplies:
			reinf_fufillment = 1.0f;
			break;
	}


	float location_modifier;
	if(ar.get_battle_from_army_battle_participation()) {
		location_modifier = calculate_location_reinforce_modifier_battle(state, ar.get_location_from_army_location(), in_nation);
	} else {
		location_modifier = calculate_location_reinforce_modifier_no_battle(state, ar.get_location_from_army_location(), in_nation);
	}
	auto combined = state.defines.reinforce_speed * reinf_fufillment * location_modifier * (1.0f + tech_nation.get_modifier_values(sys::national_mod_offsets::reinforce_speed) + tech_nation.get_modifier_values(sys::national_mod_offsets::reinforce_rate));

	assert(std::isfinite(combined));
	return std::clamp(combined, 0.f, 1.f);
}


// calculates average effective army spending for all regiments on one side of a battle.
float calculate_average_battle_supply_spending(sys::state& state, dcon::land_battle_id b, bool attacker) {
	assert(b);
	float total = 0;
	int32_t count = 0;
	for(auto army : state.world.land_battle_get_army_battle_participation(b)) {
		bool battle_attacker = is_attacker_in_battle(state, army.get_army());
		if((battle_attacker && attacker) || (!battle_attacker && !attacker)) {
			auto controller = army.get_army().get_controller_from_army_control();
			float army_reinf = (controller ? controller.get_effective_land_spending() : 1.0f);
			for(auto reg : army.get_army().get_army_membership()) {
				total += army_reinf;
				count++;
			}
		}
	}
	// fix for crash if the user hovers over the battle right as it ends, count might be 0 and would result in div by zero error
	if(count == 0)
		count = 1;
	return total / count;
}

// calculates average location modifier for all regiments on one side of a battle.
float calculate_average_battle_location_modifier(sys::state& state, dcon::land_battle_id b, bool attacker) {
	assert(b);
	auto location = state.world.land_battle_get_location_from_land_battle_location(b);
	float total = 0;
	int32_t count = 0;
	for(auto army : state.world.land_battle_get_army_battle_participation(b)) {
		bool battle_attacker = is_attacker_in_battle(state, army.get_army());
		if((battle_attacker && attacker) || (!battle_attacker && !attacker)) {
			auto controller = army.get_army().get_controller_from_army_control();
			float army_reinf = calculate_location_reinforce_modifier_battle(state, location, controller);
			for(auto reg : army.get_army().get_army_membership()) {
				total += army_reinf;
				count++;
			}
		}
	}
	// fix for crash if the user hovers over the battle right as it ends, count might be 0 and would result in div by zero error
	if(count == 0)
		count = 1;
	return total / count;
}

// calculates average national modifiers for all regiments on one side of a battle.
float calculate_average_battle_national_modifiers(sys::state& state, dcon::land_battle_id b, bool attacker) {
	assert(b);
	auto location = state.world.land_battle_get_location_from_land_battle_location(b);
	float total = 0;
	int32_t count = 0;
	for(auto army : state.world.land_battle_get_army_battle_participation(b)) {
		bool battle_attacker = is_attacker_in_battle(state, army.get_army());
		if((battle_attacker && attacker) || (!battle_attacker && !attacker)) {
			auto controller = army.get_army().get_controller_from_army_control();
			float army_reinf = (1.0f + state.world.nation_get_modifier_values(controller, sys::national_mod_offsets::reinforce_speed)) *
				(1.0f + state.world.nation_get_modifier_values(controller, sys::national_mod_offsets::reinforce_rate));
			for(auto reg : army.get_army().get_army_membership()) {
				total += army_reinf;
				count++;
			}
		}
	}
	// fix for crash if the user hovers over the battle right as it ends, count might be 0 and would result in div by zero error
	if(count == 0)
		count = 1;
	return total / count;
}

// US14 Calculates reinforcement for a particular regiment
// Combined = max reinforcement for units in the army from calculate_army_combined_reinforce
// potential_reinf = if true, will not cap max reinforcement to max unit strength, aka it will ignore current unit strength when returning reinforcement rate!
float regiment_calculate_reinforcement(sys::state& state, dcon::regiment_fat_id reg, float combined, bool potential_reinf = false) {
	auto pop = reg.get_pop_from_regiment_source();
	if((reg.get_army_from_army_membership().get_battle_from_army_battle_participation() && !is_regiment_in_reserve(state, reg)) ||
		!pop) {
		return 0.0f;
	}
	float newstr;
	float curstr;
	auto pop_size = pop.get_size();
	if(!potential_reinf) {
		auto limit_fraction = std::max(state.defines.alice_full_reinforce, std::min(1.0f, pop_size / state.defines.pop_size_per_regiment));
		curstr = reg.get_strength();
		newstr = std::min(curstr + combined, limit_fraction);
	} else {
		curstr = reg.get_strength();
		newstr = curstr + combined;
	}

	assert(std::isfinite(newstr));
	assert(std::isfinite(curstr));

	return newstr - curstr;
}

// calculates the raw amount of reinforcements one side of a battle can potentially receive every month, for display to the user
float calculate_battle_reinforcement(sys::state& state, dcon::land_battle_id b, bool attacker) {
	float total = 0;
	for(auto army : state.world.land_battle_get_army_battle_participation(b)) {
		bool battle_attacker = is_attacker_in_battle(state, army.get_army());
		if((battle_attacker && attacker) || (!battle_attacker && !attacker)) {
			float combined = calculate_army_combined_reinforce<reinforcement_estimation_type::monthly>(state, army.get_army());
			for(auto reg : state.world.army_get_army_membership(army.get_army())) {
				total += regiment_calculate_reinforcement(state, reg.get_regiment(), combined, true) * state.defines.pop_size_per_regiment;
			}
		}
	}
	return total;
}



// Calculates reinforcement for a particular unit from scratch, unit type is unknown
// potential_reinf = if true, will not cap max reinforcement to max unit strength, aka it will ignore current unit strength when returning reinforcement rate!
// reinforcement estimation decides if it will return the reinforcement at this specific day, average it over the month, or estimate with always full supplies
template<reinforcement_estimation_type reinf_estimation>
float unit_calculate_reinforcement(sys::state& state, dcon::regiment_id reg, bool potential_reinf) {
	auto fat_reg = dcon::fatten(state.world, reg);
	auto ar = fat_reg.get_army_from_army_membership();
	auto combined = calculate_army_combined_reinforce<reinf_estimation>(state, ar);

	return regiment_calculate_reinforcement(state, fat_reg, combined, potential_reinf);
}

template float unit_calculate_reinforcement<reinforcement_estimation_type::today>(sys::state& state, dcon::regiment_id reg, bool potential_reinf);
template float unit_calculate_reinforcement<reinforcement_estimation_type::monthly>(sys::state& state, dcon::regiment_id reg, bool potential_reinf);
template float unit_calculate_reinforcement<reinforcement_estimation_type::full_supplies>(sys::state& state, dcon::regiment_id reg, bool potential_reinf);

void reinforce_regiments(sys::state& state) {
	/*
	A unit that is not retreating, not embarked, not in combat is reinforced (has its strength increased) by:
define:REINFORCE_SPEED x (technology-reinforcement-modifier + 1.0) x (2 if in owned province, 0.1 in an unowned port province, 1
in a controlled province, 0.5 if in a province adjacent to a province with a war ally, 0.25 in a hostile, unblockaded port,
and 0.1 in any other hostile province) x (national-reinforce-speed-modifier + 1) x army-supplies x (number of actual regiments /
max possible regiments (feels like a bug to me) or 0.5 if mobilized)
	*/

	for(auto ar : state.world.in_army) {
		if(ar.get_navy_from_army_transport() || ar.get_is_retreating())
			continue;

		auto in_nation = ar.get_controller_from_army_control();
		auto combined = calculate_army_combined_reinforce<reinforcement_estimation_type::today>(state, ar);
		for(auto reg : ar.get_army_membership()) {
			auto reinforcement = regiment_calculate_reinforcement(state, reg.get_regiment(), combined);
			assert(std::isfinite(reinforcement));
			reg.get_regiment().set_strength(reg.get_regiment().get_strength() + reinforcement);
			adjust_regiment_experience(state, in_nation.id, reg.get_regiment(), reinforcement * 5.f * state.defines.exp_gain_div);
		}
	}
	// reset all reinforcement buffers
	for(auto nation : state.world.in_nation) {
		if(bool(nation)) {
			state.world.nation_set_land_reinforcement_buffer(nation, 0.0f);
		}
	}
}

/* === Navy reinforcement === */
// Calculates max reinforcement for units in the navy
template<reinforcement_estimation_type reinf_estimation>
float calculate_navy_combined_reinforce(sys::state& state, dcon::navy_id navy_id) {
	auto n = dcon::fatten(state.world, navy_id);
	auto in_nation = n.get_controller_from_navy_control();

	float oversize_amount =
		in_nation.get_naval_supply_points() > 0
		? std::min(float(in_nation.get_used_naval_supply_points()) / float(in_nation.get_naval_supply_points()), 1.75f)
		: 1.75f;
	float over_size_penalty = oversize_amount > 1.0f ? 2.0f - oversize_amount : 1.0f;
	auto reinf_fufillment = 0.0f;
	switch(reinf_estimation) {
		case reinforcement_estimation_type::today:
			reinf_fufillment = std::clamp(state.world.nation_get_naval_reinforcement_buffer(in_nation) / economy::unit_reinforcement_demand_divisor, 0.f, 1.f) * over_size_penalty;
			break;
		case reinforcement_estimation_type::monthly:
			reinf_fufillment = state.world.nation_get_effective_naval_spending(in_nation) * over_size_penalty;
			break;
		case reinforcement_estimation_type::full_supplies:
			reinf_fufillment = over_size_penalty;


	}

	auto rr_mod = n.get_location_from_navy_location().get_modifier_values(sys::provincial_mod_offsets::local_repair) + 1.0f;
	auto reinf_mod = in_nation.get_modifier_values(sys::national_mod_offsets::reinforce_speed) + 1.0f;
	auto combined = state.defines.reinforce_speed * rr_mod * reinf_mod * reinf_fufillment;

	return combined;
}
// Calculates reinforcement for a particular unit from scratch, unit type is unknown
template<reinforcement_estimation_type reinf_estimation>
float unit_calculate_reinforcement(sys::state& state, dcon::ship_id ship_id) {
	auto combined = calculate_navy_combined_reinforce<reinf_estimation>(state, state.world.ship_get_navy_from_navy_membership(ship_id));
	auto curstr = state.world.ship_get_strength(ship_id);
	auto newstr = std::min(curstr + combined, 1.0f);
	return newstr - curstr;
}

template float unit_calculate_reinforcement<reinforcement_estimation_type::today>(sys::state& state, dcon::ship_id ship_id);
template float unit_calculate_reinforcement<reinforcement_estimation_type::monthly>(sys::state& state, dcon::ship_id ship_id);
template float unit_calculate_reinforcement<reinforcement_estimation_type::full_supplies>(sys::state& state, dcon::ship_id ship_id);

// Calculates reinforcement for a particular ship
// Combined = max reinforcement for units in the navy from calculate_navy_combined_reinforce
float ship_calculate_reinforcement(sys::state& state, dcon::ship_id ship_id, float combined) {
	auto curstr = state.world.ship_get_strength(ship_id);
	auto newstr = std::min(curstr + combined, 1.0f);
	return newstr - curstr;
}

void repair_ships(sys::state& state) {
	/*
	US18. A ship that is docked at a naval base is repaired (has its strength increase) by:
maximum-strength x (technology-repair-rate + provincial-modifier-to-repair-rate + 1) x (national-reinforce-speed-modifier + 1) x navy-supplies x DEFINE:REINFORCE_SPEED
	*/
	for(auto n : state.world.in_navy) {
		auto nb_level = n.get_location_from_navy_location().get_building_level(uint8_t(economy::province_building_type::naval_base));
		if(!n.get_arrival_time() && nb_level > 0) {
			auto in_nation = n.get_controller_from_navy_control();
			auto combined = calculate_navy_combined_reinforce<reinforcement_estimation_type::today>(state, n);

			for(auto reg : n.get_navy_membership()) {
				auto ship = reg.get_ship();
				auto reinforcement = ship_calculate_reinforcement(state, ship, combined);
				ship.set_strength(ship.get_strength() + reinforcement);
				adjust_ship_experience(state, in_nation.id, reg.get_ship(), std::min(0.f, reinforcement * 5.f * state.defines.exp_gain_div));
			}
		}
	}
	// reset all reinforcement buffers
	for(auto nation : state.world.in_nation) {
		if(bool(nation)) {
			state.world.nation_set_naval_reinforcement_buffer(nation, 0.0f);
		}
	}
}

/* === Mobilization === */
void start_mobilization(sys::state& state, dcon::nation_id n) {
	if(state.world.nation_get_is_mobilized(n))
		return;

	state.world.nation_set_is_mobilized(n, true);
	/*
	At most, national-mobilization-impact-modifier x (define:MIN_MOBILIZE_LIMIT v nation's-number-of-regiments regiments may be
	created by mobilization).
	*/
	auto real_regs = std::max(int32_t(state.world.nation_get_active_regiments(n)), int32_t(state.defines.min_mobilize_limit));
	state.world.nation_set_mobilization_remaining(n,
			uint16_t(real_regs * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_impact)));

	auto schedule_array = state.world.nation_get_mobilization_schedule(n);
	schedule_array.clear();

	for(auto pr : state.world.nation_get_province_ownership(n)) {
		if(pr.get_province().get_is_colonial())
			continue;
		if(pr.get_province().get_nation_from_province_control() != n)
			continue;
		if(mobilized_regiments_possible_from_province(state, pr.get_province()) <= 0)
			continue;

		schedule_array.push_back(mobilization_order{ sys::date{}, pr.get_province().id });
	}

	std::sort(schedule_array.begin(), schedule_array.end(),
			[&, cap = state.world.nation_get_capital(n)](mobilization_order const& a, mobilization_order const& b) {
				auto a_dist = province::direct_distance(state, a.where, cap);
				auto b_dist = province::direct_distance(state, b.where, cap);
				if(a_dist != b_dist)
					return a_dist > b_dist;
				return a.where.value < b.where.value;
			});

	int32_t delay = 0;

	for(uint32_t count = schedule_array.size(); count-- > 0;) {
		/*
		Province by province, mobilization advances by define:MOBILIZATION_SPEED_BASE x (1 + define:MOBILIZATION_SPEED_RAILS_MULT x
		average-railroad-level-in-state / 5) until it reaches 1
		*/
		auto province_speed = state.defines.mobilization_speed_base *
			float(1.0f + state.defines.mobilization_speed_rails_mult *
											 (state.world.province_get_building_level(schedule_array[count].where, uint8_t(economy::province_building_type::railroad))) / 5.0f);
		auto days = std::max(1, int32_t(1.0f / province_speed));
		delay += days;
		schedule_array[count].when = state.current_date + delay;
	}

	/*
	Mobilizing increases crisis tension by define:CRISIS_TEMPERATURE_ON_MOBILIZE
	*/
	if(state.current_crisis_state == sys::crisis_state::heating_up) {
		for(auto& par : state.crisis_participants) {
			if(!par.id)
				break;
			if(par.id == n)
				state.crisis_temperature += state.defines.crisis_temperature_on_mobilize;
		}
	}

	notification::post(state, notification::message{ [n = n](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_mobilize_start_1", text::variable_type::x, n);
		},
		"msg_mobilize_start_title",
		n, dcon::nation_id{}, dcon::nation_id{},
		sys::message_base_type::mobilization_start,
		dcon::province_id{ }
	});
}
void end_mobilization(sys::state& state, dcon::nation_id n) {
	if(!state.world.nation_get_is_mobilized(n))
		return;

	state.world.nation_set_is_mobilized(n, false);
	state.world.nation_set_mobilization_remaining(n, 0);
	auto schedule_array = state.world.nation_get_mobilization_schedule(n);
	schedule_array.clear();

	std::vector<dcon::regiment_id> mobs_to_be_deleted;
	for(auto ar : state.world.nation_get_army_control(n)) {
		for(auto rg : ar.get_army().get_army_membership()) {
			auto pop = rg.get_regiment().get_pop_from_regiment_source();
			if(!pop || pop.get_poptype() != state.culture_definitions.soldiers) {
				mobs_to_be_deleted.push_back(rg.get_regiment().id);
			}
		}
	}
	for(auto regiment : mobs_to_be_deleted) {
		delete_regiment_safe_wrapper(state, regiment);
	}

	notification::post(state, notification::message{ [n = n](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_mobilize_end_1", text::variable_type::x, n);
		},
		"msg_mobilize_end_title",
		n, dcon::nation_id{}, dcon::nation_id{},
		sys::message_base_type::mobilization_end,
		dcon::province_id{ }
	});
}
void advance_mobilizations(sys::state& state) {
	for(auto n : state.world.in_nation) {
		auto& to_mobilize = n.get_mobilization_remaining();
		if(to_mobilize > 0) {
			auto schedule = n.get_mobilization_schedule();
			auto s_size = schedule.size();
			if(s_size > 0) {
				auto back = schedule[s_size - 1];
				if(state.current_date == back.when) {
					schedule.pop_back();
					auto mob_infantry = military::get_best_infantry(state, n);

					// mobilize the province

					if(state.world.province_get_nation_from_province_control(back.where) ==
						state.world.province_get_nation_from_province_ownership(back.where)) { // only if un occupied
						/*
						In those provinces, mobilized regiments come from non-soldier, non-slave, poor-strata pops with a culture that is
						either the primary culture of the nation or an accepted culture.
						*/
						for(auto pop : state.world.province_get_pop_location(back.where)) {
							if(pop_eligible_for_mobilization(state, pop.get_pop())) {
								/*
								The number of regiments these pops can provide is determined by pop-size x mobilization-size /
								define:POP_SIZE_PER_REGIMENT.
								*/
								auto available = int32_t(pop.get_pop().get_size() * mobilization_size(state, n) / state.defines.pop_size_per_regiment);
								if(available > 0) {

									bool army_is_new = false;

									auto a = [&]() {
										for(auto ar : state.world.province_get_army_location(back.where)) {
											if(ar.get_army().get_controller_from_army_control() == n)
												return ar.get_army().id;
										}
										auto new_army = fatten(state.world, state.world.create_army());
										new_army.set_controller_from_army_control(n);
										new_army.set_is_ai_controlled(n.get_mobilized_is_ai_controlled()); //toggle

										army_is_new = true;
										return new_army.id;
										}();

									while(available > 0 && to_mobilize > 0 && mob_infantry) {
										auto new_reg = military::create_new_regiment(state, dcon::nation_id{}, mob_infantry);
										state.world.regiment_set_org(new_reg, 0.1f);
										state.world.regiment_set_experience(new_reg, std::clamp(state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_unit_start_experience), 0.f, 1.f));
										state.world.try_create_army_membership(new_reg, a);
										auto p = pop.get_pop();
										assert(p);
										state.world.try_create_regiment_source(new_reg, p);

										--available;
										n.set_mobilization_remaining(uint16_t(n.get_mobilization_remaining() - 1));
									}
									if(army_is_new) {
										military::army_arrives_in_province(state, a, back.where, military::crossing_type::none, dcon::land_battle_id{});
										military::move_land_to_merge(state, n, a, back.where, dcon::province_id{});
									}
								}
							}

							if(to_mobilize == 0)
								break;
						}
					}
				}
			} else {
				n.set_mobilization_remaining(uint16_t(0));
			}
		}
	}
}

bool can_retreat_from_battle(sys::state& state, dcon::naval_battle_id battle) {
	return state.world.naval_battle_get_avg_distance_from_center_line(battle) <= required_avg_dist_to_center_for_retreat(state);
}
bool can_retreat_from_battle(sys::state& state, dcon::land_battle_id battle) {
	return (state.world.land_battle_get_start_date(battle) + days_before_retreat < state.current_date);
}

bool state_claimed_in_war(sys::state& state, dcon::war_id w, dcon::nation_id from, dcon::nation_id target, dcon::state_definition_id cb_state) {
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(wg.get_wargoal().get_target_nation() == target) {
			if(auto bits = wg.get_wargoal().get_type().get_type_bits(); (bits & (cb_flag::po_transfer_provinces | cb_flag::po_demand_state)) != 0) {
				if((bits & cb_flag::all_allowed_states) != 0) {
					auto state_filter = wg.get_wargoal().get_type().get_allowed_states();

					if((bits & cb_flag::po_transfer_provinces) != 0) {
						auto holder = state.world.national_identity_get_nation_from_identity_holder(wg.get_wargoal().get_associated_tag());

						if(state_filter) {
							for(auto si : state.world.nation_get_state_ownership(target)) {
								if(si.get_state().get_definition() == cb_state && trigger::evaluate(state, state_filter, trigger::to_generic(si.get_state().id), trigger::to_generic(from), trigger::to_generic(holder))) {
									return true;
								}
							}
						}
					} else if((bits & cb_flag::po_demand_state) != 0) {

						if(state_filter) {
							for(auto si : state.world.nation_get_state_ownership(target)) {
								if(si.get_state().get_definition() == cb_state && trigger::evaluate(state, state_filter, trigger::to_generic(si.get_state().id), trigger::to_generic(from), trigger::to_generic(from))) {
									return true;
								}
							}
						}
					}
				} else {
					if(wg.get_wargoal().get_associated_state() == cb_state)
						return true;
				}
			}
			if((wg.get_wargoal().get_type().get_type_bits() & cb_flag::po_annex) != 0) {
				return true;
			}
		}
	}

	return false;
}

bool war_goal_would_be_duplicate(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target, dcon::cb_type_id cb_type, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {
	// handle case of invalid war gracefully to simplify some logic
	if(!w) {
		return false;
	}

	if(cb_state) { // ensure that the state will not be annexed, transferred, or liberated
		if(state_claimed_in_war(state, w, source, target, cb_state))
			return true;
	}

	// ensure no exact duplicate
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(wg.get_wargoal().get_type() == cb_type && wg.get_wargoal().get_associated_state() == cb_state && wg.get_wargoal().get_associated_tag() == cb_tag && wg.get_wargoal().get_secondary_nation() == cb_secondary_nation && wg.get_wargoal().get_target_nation() == target) {
			return true;
		}
	}

	// annexing a state... but we already added for annexing the whole nation
	if((state.world.cb_type_get_type_bits(cb_type) & cb_flag::po_demand_state) != 0) {
		for(auto wg : state.world.war_get_wargoals_attached(w)) {
			if((wg.get_wargoal().get_type().get_type_bits() & cb_flag::po_annex) != 0 && wg.get_wargoal().get_target_nation() == target) {
				return true;
			}
		}
	}

	// if all applicable states: ensure that at least one state affected will not be transferred, annexed, or liberated
	auto bits = state.world.cb_type_get_type_bits(cb_type);
	if((bits & cb_flag::po_annex) != 0) {
		for(auto si : state.world.nation_get_state_ownership(target)) {
			if(!state_claimed_in_war(state, w, source, target, si.get_state().get_definition()))
				return false;
		}
		return true;
	}
	if((bits & cb_flag::all_allowed_states) != 0) {
		auto state_filter = state.world.cb_type_get_allowed_states(cb_type);
		if((bits & cb_flag::po_transfer_provinces) != 0) {
			auto target_tag = state.world.nation_get_identity_from_identity_holder(target);
			auto holder = state.world.national_identity_get_nation_from_identity_holder(cb_tag);

			if(state_filter) {
				for(auto si : state.world.nation_get_state_ownership(target)) {
					if(trigger::evaluate(state, state_filter, trigger::to_generic(si.get_state().id), trigger::to_generic(source), trigger::to_generic(holder))
						&& !state_claimed_in_war(state, w, source, target, si.get_state().get_definition())) {

						return false;
					}
				}
				return true;
			}
		} else if((bits & cb_flag::po_demand_state) != 0) {
			auto target_tag = state.world.nation_get_identity_from_identity_holder(target);

			if(state_filter) {
				for(auto si : state.world.nation_get_state_ownership(target)) {
					if(trigger::evaluate(state, state_filter, trigger::to_generic(si.get_state().id), trigger::to_generic(source), trigger::to_generic(source))
						&& !state_claimed_in_war(state, w, source, target, si.get_state().get_definition())) {

						return false;
					}
				}
				return true;
			}
		}
	}

	return false;
}


void update_blackflag_status(sys::state& state) {
	if(state.military_definitions.pending_blackflag_update) {
		state.military_definitions.pending_blackflag_update = false;

		for(auto a : state.world.in_army) {
			if(a.get_controller_from_army_control() && !a.get_navy_from_army_transport() && !province::has_access_to_province(state, a.get_controller_from_army_control(), a.get_location_from_army_location())) {
				a.set_black_flag(true);
			} else {
				a.set_black_flag(false);
			}
		}
	}
}

bool rebel_army_in_province(sys::state& state, dcon::province_id p) {
	for(auto ar : state.world.province_get_army_location(p)) {
		if(ar.get_army().get_controller_from_army_rebel_control())
			return true;
	}
	return false;
}
dcon::province_id find_land_rally_pt(sys::state& state, dcon::nation_id by, dcon::province_id start) {
	float distance = 2.0f;
	dcon::province_id closest;
	auto region = state.world.province_get_connected_region_id(start);

	for(auto p : state.world.nation_get_province_ownership(by)) {
		if(!p.get_province().get_land_rally_point())
			continue;
		if(p.get_province().get_connected_region_id() != region)
			continue;
		if(p.get_province().get_nation_from_province_control() != by)
			continue;
		if(auto dist = province::sorting_distance(state, start, p.get_province()); !closest || dist < distance) {
			distance = dist;
			closest = p.get_province();
		}
	}

	return closest;
}
dcon::province_id find_naval_rally_pt(sys::state& state, dcon::nation_id by, dcon::province_id start) {
	float distance = 2.0f;
	dcon::province_id closest;

	for(auto p : state.world.nation_get_province_ownership(by)) {
		if(!p.get_province().get_naval_rally_point())
			continue;
		if(p.get_province().get_nation_from_province_control() != by)
			continue;
		if(auto dist = province::sorting_distance(state, start, p.get_province()); !closest || dist < distance) {
			distance = dist;
			closest = p.get_province();
		}
	}

	return closest;
}
void move_land_to_merge(sys::state& state, dcon::nation_id by, dcon::army_id a, dcon::province_id start, dcon::province_id dest) {
	if(state.world.nation_get_is_player_controlled(by) == false)
		return; // AI doesn't use rally points or templates
	if(!dest)
		dest = find_land_rally_pt(state, by, start);
	if(!dest || state.world.province_get_nation_from_province_control(dest) != by)
		return;
	if(state.world.army_get_battle_from_army_battle_participation(a))
		return;

	if(dest == start) { // merge in place
		for(auto ar : state.world.province_get_army_location(start)) {
			if(ar.get_army().get_controller_from_army_control() == by && ar.get_army() != a) {
				auto regs = state.world.army_get_army_membership(a);
				while(regs.begin() != regs.end()) {
					(*regs.begin()).set_army(ar.get_army());
				}
				return;
			}
		}
	} else {
		auto path = province::make_land_path(state, start, dest, by, a);
		if(move_army_fast(state, a, path, by)) {
			state.world.army_set_moving_to_merge(a, true);
		}
	}
}

bool move_navy_fast(sys::state& state, dcon::navy_id navy, const std::span<dcon::province_id, std::dynamic_extent> naval_path, bool reset) {
	if(naval_path.size() > 0) {
		auto existing_path = state.world.navy_get_path(navy);
		auto old_first_prov = existing_path.size() > 0 ? existing_path.at(existing_path.size() - 1) : dcon::province_id{};
		if(reset) {
			existing_path.clear();
		}
		auto append_size = uint32_t(naval_path.size());
		auto old_size = existing_path.size();
		auto new_size = old_size + append_size;
		existing_path.resize(new_size);

		for(uint32_t i = old_size; i-- > 0; ) {
			existing_path.at(append_size + i) = existing_path.at(i);
		}
		for(uint32_t i = 0; i < append_size; ++i) {
			existing_path.at(i) = naval_path[i];
		}

		if(existing_path.at(new_size - 1) != old_first_prov) {
			auto arrival_info = military::arrival_time_to(state, navy, naval_path.back());
			state.world.navy_set_arrival_time(navy, arrival_info.arrival_time);
			state.world.navy_set_unused_travel_days(navy, arrival_info.unused_travel_days);
		}
		return true;
	} else {
		return false;
	}
}

template<ai_path_length path_length_to_use>
bool move_navy_fast(sys::state& state, dcon::navy_id navy, dcon::province_id destination, bool reset) {
	if(reset || state.world.navy_get_path(navy).size() == 0) {
		auto naval_path = province::make_naval_path(state, state.world.navy_get_location_from_navy_location(navy), destination, state.world.navy_get_controller_from_navy_control(navy));
		if constexpr(path_length_to_use.length != 0) {
			while(naval_path.size() > path_length_to_use.length) {
				naval_path.erase(naval_path.begin());
			}

		}
		return move_navy_fast(state, navy, naval_path, reset);
	} else {
		auto from_prov = state.world.navy_get_path(navy).at(0);
		auto naval_path = province::make_naval_path(state, from_prov, destination, state.world.navy_get_controller_from_navy_control(navy));
		if constexpr(path_length_to_use.length != 0) {
			while(naval_path.size() > path_length_to_use.length) {
				naval_path.erase(naval_path.begin());
			}

		}
		return move_navy_fast(state, navy, naval_path, reset);
	}

}

template bool move_navy_fast < ai_path_length{ 4 } > (sys::state& state, dcon::navy_id navy, dcon::province_id destination, bool reset);
template bool move_navy_fast < ai_path_length{ 0 } > (sys::state& state, dcon::navy_id navy, dcon::province_id destination, bool reset);


bool move_army_fast(sys::state& state, dcon::army_id army, const std::span<dcon::province_id, std::dynamic_extent> army_path, dcon::nation_id nation_as, bool reset) {
	if(army_path.size() > 0) {
		auto existing_path = state.world.army_get_path(army);
		auto old_first_prov = existing_path.size() > 0 ? existing_path.at(existing_path.size() - 1) : dcon::province_id{};
		if(reset) {
			existing_path.clear();
		}

		auto append_size = uint32_t(army_path.size());
		auto old_size = existing_path.size();
		auto new_size = old_size + append_size;
		existing_path.resize(new_size);

		for(uint32_t i = old_size; i-- > 0; ) {
			existing_path.at(append_size + i) = existing_path.at(i);
		}
		for(uint32_t i = 0; i < append_size; ++i) {
			existing_path.at(i) = army_path[i];
		}

		if(existing_path.at(new_size - 1) != old_first_prov) {
			auto arrival_data = military::arrival_time_to(state, army, army_path.back());
			state.world.army_set_arrival_time(army, arrival_data.arrival_time);
			state.world.army_set_unused_travel_days(army, arrival_data.unused_travel_days);
		}

		state.world.army_set_dig_in(army, 0);
		return true;
	} else {
		return false;
	}
}

// Extracted as a separate function for the AI to use instead of command (remove redundancy in AI code)
template<ai_path_length path_length_to_use>
bool move_army_fast(sys::state& state, dcon::army_id army, dcon::province_id destination, dcon::nation_id nation_as, bool reset) {
	bool blackflag = state.world.army_get_black_flag(army);

	// When AI takes over the nation (on tag swaps, players leaving or not joining on savegame load), AI armies can have leftover special orders
	if(!state.world.nation_get_is_player_controlled(nation_as)) {
		state.world.army_set_special_order(army, (uint8_t)military::special_army_order::none);
	}

	if(reset || state.world.army_get_path(army).size() == 0) {
		auto army_path = blackflag ? province::make_unowned_land_path(state, state.world.army_get_location_from_army_location(army), destination) : province::make_land_path(state, state.world.army_get_location_from_army_location(army), destination, nation_as, army);
		if constexpr(path_length_to_use.length != 0) {
			while(army_path.size() > path_length_to_use.length) {
				army_path.erase(army_path.begin());
			}

		}
		return move_army_fast(state, army, army_path, nation_as, reset);

	} else {
		auto from_prov = state.world.army_get_path(army).at(0);
		auto army_path = blackflag ? province::make_unowned_land_path(state, from_prov, destination) : province::make_land_path(state, from_prov, destination, nation_as, army);
		if constexpr(path_length_to_use.length != 0) {
			while(army_path.size() > path_length_to_use.length) {
				army_path.erase(army_path.begin());
			}
		}
		return move_army_fast(state, army, army_path, nation_as, reset);
	}
}

template bool move_army_fast<ai_path_length{ 4 }>(sys::state& state, dcon::army_id army, dcon::province_id destination, dcon::nation_id nation_as, bool reset);
template bool move_army_fast < ai_path_length{ 0 } > (sys::state& state, dcon::army_id army, dcon::province_id destination, dcon::nation_id nation_as, bool reset);

void move_navy_to_merge(sys::state& state, dcon::nation_id by, dcon::navy_id a, dcon::province_id start, dcon::province_id dest) {
	if(state.world.nation_get_is_player_controlled(by) == false)
		return; // AI doesn't use rally points or templates
	if(!dest)
		dest = find_naval_rally_pt(state, by, start);
	if(!dest || state.world.province_get_nation_from_province_control(dest) != by)
		return;

	if(dest == start) { // merge in place
		for(auto ar : state.world.province_get_navy_location(start)) {
			if(ar.get_navy().get_controller_from_navy_control() == by && ar.get_navy() != a) {
				auto regs = state.world.navy_get_navy_membership(a);
				while(regs.begin() != regs.end()) {
					(*regs.begin()).set_navy(ar.get_navy());
				}
				return;
			}
		}
	} else {
		auto path = province::make_naval_path(state, start, dest, state.world.navy_get_controller_from_navy_control(a));
		if(!path.empty()) {
			military::move_navy_fast(state, a, path);
			state.world.navy_set_moving_to_merge(a, true);
		}
	}
}

bool pop_eligible_for_mobilization(sys::state& state, dcon::pop_id p) {
	auto const pop = dcon::fatten(state.world, p);
	return pop.get_poptype() != state.culture_definitions.soldiers
		&& pop.get_poptype() != state.culture_definitions.slaves
		&& pop.get_is_primary_or_accepted_culture()
		&& pop.get_poptype().get_strata() == uint8_t(culture::pop_strata::poor);
}
template<regiment_dmg_source damage_source>
void disband_regiment_w_pop_death(sys::state& state, dcon::regiment_id reg_id) {
	auto base_pop = state.world.regiment_get_pop_from_regiment_source(reg_id);
	auto army = state.world.regiment_get_army_from_army_membership(reg_id);
	auto controller = state.world.army_get_controller_from_army_control(army);
	if(!controller && base_pop) {
		// When a rebel regiment is destroyed, divide the militancy of the backing pop by define:REDUCTION_AFTER_DEFEAT.
		auto mil = pop_demographics::get_militancy(state, base_pop) / state.defines.reduction_after_defeat;
		pop_demographics::set_militancy(state, base_pop, mil);
	}
	else if(controller) {
		// give war exhaustion for the losses
		auto& current_war_ex = state.world.nation_get_war_exhaustion(controller);
		float extra_war_ex = get_war_exhaustion_from_land_losses<damage_source>(state, state.world.regiment_get_strength(reg_id), controller);
		state.world.nation_set_war_exhaustion(controller, std::min(current_war_ex + extra_war_ex, state.world.nation_get_modifier_values(controller, sys::national_mod_offsets::max_war_exhaustion)));

	}
	demographics::reduce_pop_size_safe(state, base_pop, int32_t(state.world.regiment_get_strength(reg_id) * state.defines.pop_size_per_regiment * state.defines.soldier_to_pop_damage));
	military::delete_regiment_safe_wrapper(state, reg_id);
}


bool can_attack(sys::state& state, dcon::nation_id n) {
	// nations without land are not supported yet
	if(state.world.nation_get_owned_province_count(n) == 0)
		return false;

	return true;
}

// used by both functions
// attempts to be cheap...
bool can_attack_internal(sys::state& state, dcon::nation_id n, dcon::nation_id target) {	
	if(target == n)
		return false;	

	// can't declare war on nations without land currently
	if(state.world.nation_get_owned_province_count(target) == 0)
		return false;

	// no decs against allies
	if(nations::are_allied(state, n, target))
		return false;

	// cannot declare war on your own sphereling
	if(state.world.nation_get_in_sphere_of(target) == n)
		return false;

	// when declaring a war, alliances with the spherelord are also checked
	if(nations::would_war_conflict_with_sphere_leader<nations::war_initiation::declare_war>(state, n, target)) {
		return false;
	}

	if(military::has_truce_with(state, n, target))
		return false;

	// subjects check
	auto overlord_source = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(n));
	if(overlord_source && overlord_source != target && state.defines.alice_allow_subjects_declare_wars == 0.0)
		return false;

	// the most expensive check
	if(nations::has_units_inside_other_nation(state, n, target)) {
		return false;
	}

	return true;
}

// cheaper version: some checks could be more strict to be less computationally expensive
bool can_attack_ai(sys::state& state, dcon::nation_id n, dcon::nation_id target) {
	// we don't allow attacking war allies or war enemies
	// for AI we do it in a cheaper way:
	// don't declare war when you have war allies
	if(state.world.nation_get_is_at_war(n))
		return false;
	auto overlord = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target));
	// we do not check can_attack(state, source) because it is checked during iteration over source
	// common checks, both for player and AI
	if(!can_attack_internal(state, n, target)) {
		return false;
	}
	if(overlord) {
		if(!can_attack_internal(state, n, overlord)) {
			return false;
		}
	}

	// check existence of cb
	if(!military::can_use_cb_against(state, n, target))
		return false;

	return true;
}

// we can allow expensive logic there
bool can_attack_expensive_checks(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(military::are_allied_in_war(state, source, target) || military::are_at_war(state, source, target))
		return false;
	return true;
}

bool can_attack(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	auto overlord = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target));

	if(!can_attack(state, source)) {
		return false;
	}

	if(!can_attack_internal(state, source, target)) {
		return false;
	}
	if(!can_attack_expensive_checks(state, source, target)) {
		return false;
	}
	if(overlord) {
		if(!can_attack_internal(state, source, overlord)) {
			return false;
		}
		if(!can_attack_expensive_checks(state, source, overlord)) {
			return false;
		}
	}

	// anti player bias
	if(state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < state.defines.declarewar_diplomatic_cost)
		return false;

	// have to check only against target
	if(!military::can_use_cb_against(state, source, target))
		return false;

	return true;
}

} // namespace military
