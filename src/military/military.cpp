#include "military.hpp"
#include "dcon_generated.hpp"
#include "prng.hpp"
#include "effects.hpp"
#include "events.hpp"
#include "ai.hpp"

namespace military {

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

void reset_unit_stats(sys::state& state) {
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)};
		state.world.for_each_nation([&](dcon::nation_id nid) {
			state.world.nation_get_unit_stats(nid, uid) = state.military_definitions.unit_base_definitions[uid];
		});
	}
}

void apply_base_unit_stat_modifiers(sys::state& state) {
	assert(state.military_definitions.base_army_unit.index() < 2);
	assert(state.military_definitions.base_naval_unit.index() < 2);
	assert(state.military_definitions.base_army_unit.index() != -1);
	assert(state.military_definitions.base_naval_unit.index() != -1);
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)};
		auto base_id = state.military_definitions.unit_base_definitions[uid].is_land
			? state.military_definitions.base_army_unit
			: state.military_definitions.base_naval_unit;
		state.world.for_each_nation([&](dcon::nation_id nid) {
			auto& base_stats = state.world.nation_get_unit_stats(nid, base_id);
			auto& current_stats = state.world.nation_get_unit_stats(nid, uid);
			current_stats += base_stats;
		});
	}
}

void restore_unsaved_values(sys::state& state) {
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id u{dcon::unit_type_id::value_base_t(i)};
		if(state.military_definitions.unit_base_definitions[u].icon == 3) {
			state.military_definitions.artillery = u;
			break;
		}
	}
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
	if(can_use &&
			!trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(actor))) {
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
				if(n != target && n != actor) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor),
								 trigger::to_generic(n.id))) {
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
	if(can_use && !trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(actor))) {
		return false;
	}

	auto allowed_countries = state.world.cb_type_get_allowed_countries(cb);
	auto allowed_states = state.world.cb_type_get_allowed_states(cb);

	if(!allowed_countries && allowed_states) {
		bool any_allowed = false;
		for(auto si : state.world.nation_get_state_ownership(target)) {
			if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
						 trigger::to_generic(actor))) {
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
				if(n != target && n != actor) {
					if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor),
								 trigger::to_generic(n.id))) {
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
	if(can_use &&
			!trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(actor))) {
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
						for(auto si : state.world.nation_get_state_ownership(target)) {
							if(si.get_state().get_definition() == st &&
									trigger::evaluate(state, allowed_substates, trigger::to_generic(si.get_state().id), trigger::to_generic(actor),
											trigger::to_generic(actor))) {
								return true;
							}
						}
					} else {
						for(auto si : state.world.nation_get_state_ownership(target)) {
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

		if(trigger::evaluate(state, allowed_countries, trigger::to_generic(target), trigger::to_generic(actor),
					 trigger::to_generic(secondary_nation))) {
			bool validity = false;
			if(allowed_states) { // check whether any state within the target is valid for free / liberate
				if((state.world.cb_type_get_type_bits(cb) & cb_flag::all_allowed_states) == 0) {
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

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids) {
	// TODO: implement function
	return false;
}

template<typename T>
auto province_is_under_siege(sys::state const& state, T ids) {
	return state.world.province_get_siege_progress(ids) > 0.0f;
}

template<typename T>
auto battle_is_ongoing_in_province(sys::state const& state, T ids) {
	ve::apply(
			[&](dcon::province_id p) {
				auto battles = state.world.province_get_land_battle_location(p);
				return battles.begin() != battles.end();
			},
			ids);
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
			if(p.get_province().get_building_level(economy::province_building_type::naval_base) > 0)
				return true;
		}
	}
	return false;
}

bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
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
					return participation{wa.get_war().id, is_attacker ? war_role::attacker : war_role::defender};
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
	} else if(auto uni_rel = state.world.get_unilateral_relationship_by_unilateral_pair(prov_controller, n);
						state.world.unilateral_relationship_get_military_access(uni_rel)) {
		modifier = 2.0f;
	} else if(bool(state.world.get_core_by_prov_tag_key(p, state.world.nation_get_identity_from_identity_holder(n)))) {
		modifier = 2.0f;
	} else if(state.world.province_get_siege_progress(p) > 0.0f) {
		modifier = 2.0f;
	}
	auto base_supply_lim = (state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::supply_limit) + 1.0f);
	auto national_supply_lim = (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::supply_limit) + 1.0f);
	return int32_t(base_supply_lim * modifier * national_supply_lim);
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

dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, dcon::culture_id pop_culture) {
	if(state.world.province_get_is_colonial(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum && pop.get_pop().get_culture() == pop_culture) {
					auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
					auto regs = pop.get_pop().get_regiment_source();
					auto building = pop.get_pop().get_province_land_construction();

					if(amount > ((regs.end() - regs.begin()) + (building.end() - building.begin()))) {
						return pop.get_pop().id;
					}
				}
			}
		}
		return dcon::pop_id{};
	} else if(!state.world.province_get_is_owner_core(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum && pop.get_pop().get_culture() == pop_culture) {
					auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
					auto regs = pop.get_pop().get_regiment_source();
					auto building = pop.get_pop().get_province_land_construction();

					if(amount > ((regs.end() - regs.begin()) + (building.end() - building.begin()))) {
						return pop.get_pop().id;
					}
				}
			}
		}
		return dcon::pop_id{};
	} else {
		float divisor = state.defines.pop_size_per_regiment;
		float minimum = state.defines.pop_min_size_for_regiment;

		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum && pop.get_pop().get_culture() == pop_culture) {
					auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
					auto regs = pop.get_pop().get_regiment_source();
					auto building = pop.get_pop().get_province_land_construction();

					if(amount > ((regs.end() - regs.begin()) + (building.end() - building.begin()))) {
						return pop.get_pop().id;
					}
				}
			}
		}
		return dcon::pop_id{};
	}
}

dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, bool require_accepted) {
	if(state.world.province_get_is_colonial(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		dcon::pop_id non_preferred;
		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum) {
					auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
					auto regs = pop.get_pop().get_regiment_source();
					auto building = pop.get_pop().get_province_land_construction();

					if(amount > ((regs.end() - regs.begin()) + (building.end() - building.begin()))) {
						if(require_accepted == pop.get_pop().get_is_primary_or_accepted_culture())
							return pop.get_pop().id;
						else
							non_preferred = pop.get_pop().id;
					}
				}
			}
		}
		return non_preferred;
	} else if(!state.world.province_get_is_owner_core(p)) {
		float divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
		float minimum = state.defines.pop_min_size_for_regiment;

		dcon::pop_id non_preferred;
		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum) {
					auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
					auto regs = pop.get_pop().get_regiment_source();
					auto building = pop.get_pop().get_province_land_construction();

					if(amount > ((regs.end() - regs.begin()) + (building.end() - building.begin()))) {
						if(require_accepted == pop.get_pop().get_is_primary_or_accepted_culture())
							return pop.get_pop().id;
						else
							non_preferred = pop.get_pop().id;
					}
				}
			}
		}
		return non_preferred;
	} else {
		float divisor = state.defines.pop_size_per_regiment;
		float minimum = state.defines.pop_min_size_for_regiment;

		dcon::pop_id non_preferred;
		for(auto pop : state.world.province_get_pop_location(p)) {
			if(pop.get_pop().get_poptype() == state.culture_definitions.soldiers) {
				if(pop.get_pop().get_size() >= minimum) {
					auto amount = int32_t((pop.get_pop().get_size() / divisor) + 1);
					auto regs = pop.get_pop().get_regiment_source();
					auto building = pop.get_pop().get_province_land_construction();

					if(amount > ((regs.end() - regs.begin()) + (building.end() - building.begin()))) {
						if(require_accepted == pop.get_pop().get_is_primary_or_accepted_culture())
							return pop.get_pop().id;
						else
							non_preferred = pop.get_pop().id;
					}
				}
			}
		}
		return non_preferred;
	}
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

		if(pop.get_pop().get_poptype() != state.culture_definitions.soldiers &&
				pop.get_pop().get_poptype() != state.culture_definitions.slaves && pop.get_pop().get_is_primary_or_accepted_culture() &&
				pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {

			/*
			The number of regiments these pops can provide is determined by pop-size x mobilization-size /
			define:POP_SIZE_PER_REGIMENT.
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
	state.world.execute_serial_over_nation([&](auto ids) { state.world.nation_set_recruitable_regiments(ids, ve::int_vector(0)); });
	state.world.for_each_province([&](dcon::province_id p) {
		auto owner = state.world.province_get_nation_from_province_ownership(p);
		if(owner) {
			state.world.nation_get_recruitable_regiments(owner) += uint16_t(regiments_max_possible_from_province(state, p));
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
			state.world.nation_get_active_regiments(owner) += uint16_t(num_regs);
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
			dcon::unit_type_id u{dcon::unit_type_id::value_base_t(i)};
			if(state.world.nation_get_active_unit(n, u) && state.military_definitions.unit_base_definitions[u].is_land) {
				auto& reg_stats = state.world.nation_get_unit_stats(n, u);
				total += ((reg_stats.defence_or_hull + ld_mod) + (reg_stats.attack_or_gun_power + lo_mod)) *
								 state.military_definitions.unit_base_definitions[u].discipline;
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
	uint32_t supply = uint32_t(state.defines.naval_base_supply_score_base * (std::pow(2, (dcon::fatten(state.world, prov).get_building_level(economy::province_building_type::naval_base) -1))));
	if(dcon::fatten(state.world, prov).get_building_level(economy::province_building_type::naval_base) != 0) { 
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
			int32_t nb_level = 0;
			for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
				if(p.get_province().get_nation_from_province_ownership() == n) {
					if(p.get_province().get_is_coast()) {
						saw_coastal = true;
					}
					nb_level = std::max(nb_level, int32_t(p.get_province().get_building_level(economy::province_building_type::naval_base)));
				}
			}
			bool is_core_or_connected = si.get_state().get_capital().get_is_owner_core() ||
																	si.get_state().get_capital().get_connected_region_id() == cap_region;
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
float mobilization_impact(sys::state const& state, dcon::nation_id n) {
	// Mobilization impact = 1 - mobilization-size x (national-mobilization-economy-impact-modifier +
	// technology-mobilization-impact-modifier), to a minimum of zero.
	return std::clamp(1.0f - mobilization_size(state, n) *
															 state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_impact),
			0.0f, 1.0f);
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
					n,
					sys::message_setting_type::cb_fab_cancelled
					});
				}

				n.set_constructing_cb_is_discovered(false);
				n.set_constructing_cb_progress(0.0f);
				n.set_constructing_cb_target(dcon::nation_id{});
				n.set_constructing_cb_type(dcon::cb_type_id{});
			}
		}

		if(n.get_constructing_cb_type() && !nations::is_involved_in_crisis(state, n)) {
			/*
			CB fabrication by a nation is paused while that nation is in a crisis (nor do events related to CB fabrication
			happen). CB fabrication is advanced by points equal to: define:CB_GENERATION_BASE_SPEED x cb-type-construction-speed x
			(national-cb-construction-speed-modifiers + technology-cb-construction-speed-modifier + 1).
			*/

			n.get_constructing_cb_progress() += state.defines.cb_generation_base_speed * n.get_constructing_cb_type().get_construction_speed() * (n.get_modifier_values(sys::national_mod_offsets::cb_generation_speed_modifier) + 1.0f);

			/*
			Each day, a fabricating CB has a define:CB_DETECTION_CHANCE_BASE out of 1000 chance to be detected. If discovered, the
			fabricating country gains the infamy for that war goal x the fraction of fabrication remaining. If discovered
			relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states with
			a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED
			*/
			if(!n.get_constructing_cb_is_discovered()) {
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
				add_cb(state, n, n.get_constructing_cb_type(), n.get_constructing_cb_target());

				if(n == state.local_player_nation) {
					notification::post(state, notification::message{
						[t = n.get_constructing_cb_target(), c = n.get_constructing_cb_type()](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_fab_finished_1", text::variable_type::x, state.world.cb_type_get_name(c), text::variable_type::y, t);
						},
						"msg_fab_finished_title",
						n,
						sys::message_setting_type::cb_fab_finished
					});
				}

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
	current_cbs.push_back(
			military::available_cb{target, state.current_date + int32_t(state.defines.created_cb_valid_time) * 30, cb});
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
float truce_break_cb_infamy(sys::state& state, dcon::cb_type_id t) {
	float total = 0.0f;
	auto bits = state.world.cb_type_get_type_bits(t);

	if((bits & cb_flag::po_clear_union_sphere) != 0) {
		total += state.defines.breaktruce_infamy_clear_union_sphere;
	}
	if((bits & cb_flag::po_gunboat) != 0) {
		total += state.defines.breaktruce_infamy_gunboat;
	}
	if((bits & cb_flag::po_annex) != 0) {
		total += state.defines.breaktruce_infamy_annex;
	}
	if((bits & cb_flag::po_demand_state) != 0) {
		total += state.defines.breaktruce_infamy_demand_state;
	}
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		total += state.defines.breaktruce_infamy_add_to_sphere;
	}
	if((bits & cb_flag::po_disarmament) != 0) {
		total += state.defines.breaktruce_infamy_disarmament;
	}
	if((bits & cb_flag::po_reparations) != 0) {
		total += state.defines.breaktruce_infamy_reparations;
	}
	if((bits & cb_flag::po_transfer_provinces) != 0) {
		total += state.defines.breaktruce_infamy_transfer_provinces;
	}
	if((bits & cb_flag::po_remove_prestige) != 0) {
		total += state.defines.breaktruce_infamy_prestige;
	}
	if((bits & cb_flag::po_make_puppet) != 0) {
		total += state.defines.breaktruce_infamy_make_puppet;
	}
	if((bits & cb_flag::po_release_puppet) != 0) {
		total += state.defines.breaktruce_infamy_release_puppet;
	}
	if((bits & cb_flag::po_status_quo) != 0) {
		total += state.defines.breaktruce_infamy_status_quo;
	}
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		total += state.defines.breaktruce_infamy_install_communist_gov_type;
	}
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		total += state.defines.breaktruce_infamy_uninstall_communist_gov_type;
	}
	if((bits & cb_flag::po_remove_cores) != 0) {
		total += state.defines.breaktruce_infamy_remove_cores;
	}
	if((bits & cb_flag::po_colony) != 0) {
		total += state.defines.breaktruce_infamy_colony;
	}
	if((bits & cb_flag::po_destroy_forts) != 0) {
		total += state.defines.breaktruce_infamy_destroy_forts;
	}
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		total += state.defines.breaktruce_infamy_destroy_naval_bases;
	}

	return total * state.world.cb_type_get_break_truce_infamy_factor(t);
}

int32_t province_point_cost(sys::state& state, dcon::province_id p, dcon::nation_id n) {
	/*
	All provinces have a base value of 1. For non colonial provinces: each level of naval base increases its value by 1. If it is
	a state capital, its value increases by 1 for every factory in the state (factory level does not matter). Provinces get 1
	point per fort level. This value is the doubled for non-overseas provinces where the owner has a core. It is then tripled for
	the nation's capital province.
	*/
	int32_t total = 1;
	if(!state.world.province_get_is_colonial(p)) {
		total += state.world.province_get_building_level(p, economy::province_building_type::naval_base);
	}
	auto fac_range = state.world.province_get_factory_location(p);
	total += int32_t(fac_range.end() - fac_range.begin());
	total += state.world.province_get_building_level(p, economy::province_building_type::fort);

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
		if(sum_target_prov_values > 0) {

			if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal);
					allowed_states && (bits & cb_flag::all_allowed_states) != 0) {
				bool any_allowed = false;
				for(auto si : state.world.nation_get_state_ownership(target)) {
					if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
								 trigger::to_generic(from))) {

						province::for_each_province_in_state_instance(state, si.get_state(), [&](dcon::province_id tprov) {
							total += 2.8f * float(province_point_cost(state, tprov, target)) / float(sum_target_prov_values);
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

float crisis_cb_addition_infamy_cost(sys::state& state, dcon::cb_type_id type, dcon::nation_id from, dcon::nation_id target) {
	if((state.world.cb_type_get_type_bits(type) & (military::cb_flag::always | military::cb_flag::is_not_constructing_cb)) != 0) {
		// not a constructible CB
		return 0.0f;
	}

	return cb_infamy(state, type);
}
float cb_addition_infamy_cost(sys::state& state, dcon::war_id war, dcon::cb_type_id type, dcon::nation_id from,
		dcon::nation_id target) {
	if((state.world.cb_type_get_type_bits(type) & (military::cb_flag::always | military::cb_flag::is_not_constructing_cb)) != 0) {
		// not a constructible CB
		return 0.0f;
	}

	auto other_cbs = state.world.nation_get_available_cbs(from);
	for(auto& cb : other_cbs) {
		if(cb.target == target && cb_conditions_satisfied(state, from, target, cb.cb_type))
			return 0.0f;
	}

	if(state.world.war_get_is_great(war))
		return cb_infamy(state, type) * state.defines.gw_justify_cb_badboy_impact;
	else
		return cb_infamy(state, type);
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
	return (bits & (cb_flag::po_demand_state | cb_flag::po_transfer_provinces | cb_flag::po_destroy_naval_bases |
										 cb_flag::po_destroy_forts)) != 0 &&
				 (bits & cb_flag::all_allowed_states) == 0;
}

void execute_cb_discovery(sys::state& state, dcon::nation_id n) {
	/*
	 If discovered, the fabricating country gains the infamy for that war goal x the fraction of fabrication remaining. If
	 discovered relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states
	 with a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED
	*/
	auto infamy = cb_infamy(state, state.world.nation_get_constructing_cb_type(n));
	state.world.nation_get_infamy(n) += ((100.0f - state.world.nation_get_constructing_cb_progress(n)) / 100.0f) * infamy;

	auto target = state.world.nation_get_constructing_cb_target(n);

	nations::adjust_relationship(state, n, target, state.defines.on_cb_detected_relation_change);

	for(auto si : state.world.nation_get_state_ownership(target)) {
		if(si.get_state().get_flashpoint_tag()) {
			si.get_state().set_flashpoint_tension(
					std::min(100.0f, si.get_state().get_flashpoint_tension() + state.defines.tension_on_cb_discovered));
		}
	}

	notification::post(state, notification::message{
		[n, target, infamy](sys::state& state, text::layout_base& contents) {
			if(n == state.local_player_nation) {
				text::add_line(state, contents, "msg_fab_discovered_1", text::variable_type::x, text::fp_one_place{infamy});
			} else {
				text::add_line(state, contents, "msg_fab_discovered_2", text::variable_type::x, n, text::variable_type::y, target);
			}
		},
		"msg_fab_discovered_title",
		n,
		sys::message_setting_type::cb_detected_by_nation
	});
	notification::post(state, notification::message{
		[n, target, infamy](sys::state& state, text::layout_base& contents) {
			if(n == state.local_player_nation) {
				text::add_line(state, contents, "msg_fab_discovered_1", text::variable_type::x, text::fp_one_place{infamy});
			} else {
				text::add_line(state, contents, "msg_fab_discovered_2", text::variable_type::x, n, text::variable_type::y, target);
			}
		},
		"msg_fab_discovered_title",
		target,
		sys::message_setting_type::cb_detected_by_nation
	});
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

	l.set_personality(dcon::leader_trait_id{dcon::leader_trait_id::value_base_t(1 + trait_pair.high % num_personalities)});
	l.set_background(dcon::leader_trait_id{dcon::leader_trait_id::value_base_t(
			state.military_definitions.first_background_trait.index() + 1 + trait_pair.low % num_backgrounds)});

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
				n,
				sys::message_setting_type::leader_dies
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

void monthly_leaders_update(sys::state& state) {
	/*
	- A nation gets ((number-of-officers / total-population) / officer-optimum)^1 x officer-leadership-amount +
	national-modifier-to-leadership x (national-modifier-to-leadership-modifier + 1) leadership points per month.
	*/

	state.world.execute_serial_over_nation(
			[&, optimum_officers = state.world.pop_type_get_research_optimum(state.culture_definitions.officers)](auto ids) {
				auto ofrac = state.world.nation_get_demographics(ids, demographics::to_key(state, state.culture_definitions.officers)) /
										 ve::max(state.world.nation_get_demographics(ids, demographics::total), 1.0f);
				auto omod = ve::min(1.0f, ofrac / optimum_officers) * float(state.culture_definitions.officer_leadership_points);
				auto nmod = (state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::leadership_modifier) + 1.0f) *
										state.world.nation_get_modifier_values(ids, sys::national_mod_offsets::leadership);

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
					auto too_many_generals =
							(existing_leaders.admirals > 0 && navy_count > 0)
									? float(existing_leaders.generals) / float(existing_leaders.admirals) > float(army_count) / float(navy_count)
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
		dcon::leader_id l{dcon::leader_id::value_base_t(i)};
		auto age_in_days = state.world.leader_get_since(l).to_raw_value() * 365;
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
		if(state.current_date < state.world.diplomatic_relation_get_truce_until(rel))
			return true;
	}
	return false;
}

dcon::regiment_id create_new_regiment(sys::state& state, dcon::nation_id n, dcon::unit_type_id t) {
	auto reg = fatten(state.world, state.world.create_regiment());
	reg.set_type(t);
	// TODO make name
	reg.set_strength(1.0f);
	reg.set_org(1.0f);
	return reg.id;
}
dcon::ship_id create_new_ship(sys::state& state, dcon::nation_id n, dcon::unit_type_id t) {
	auto shp = fatten(state.world, state.world.create_ship());
	shp.set_type(t);
	// TODO make name
	shp.set_strength(1.0f);
	shp.set_org(1.0f);
	return shp.id;
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
	do {
		cleanup_war(state, w, war_result::draw);
		w = find_war_between(state, a, b);
	} while(w);
}

void add_to_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_attacker, bool on_war_creation) {
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
						n,
						sys::message_setting_type::war_subsidies_end_by_nation
					});
					notification::post(state, notification::message{
						[source = n, target = ul.get_target().id](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_wsub_end_1", text::variable_type::x, source, text::variable_type::y, target);
						},
						"msg_wsub_end_title",
						ul.get_target().id,
						sys::message_setting_type::war_subsidies_end_on_nation
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
						n,
						sys::message_setting_type::war_subsidies_end_on_nation
					});
					notification::post(state, notification::message{
						[source = ul.get_source().id, target = n](sys::state& state, text::layout_base& contents) {
							text::add_line(state, contents, "msg_wsub_end_1", text::variable_type::x, source, text::variable_type::y, target);
						},
						"msg_wsub_end_title",
						ul.get_target().id,
						sys::message_setting_type::war_subsidies_end_by_nation
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
			auto old_name = state.world.war_get_name(w);
			state.world.war_set_is_great(w, true);
			auto it = state.key_to_text_sequence.find(std::string_view{"great_war_name"}); // misspelling is intentional; DO NOT CORRECT
			if(it != state.key_to_text_sequence.end()) {
				state.world.war_set_name(w, it->second);
			}

			notification::post(state, notification::message{
				[old_name, w](sys::state& state, text::layout_base& contents) {
					text::substitution_map sub;
					text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
					text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(state.world.war_get_primary_defender(w)));
					text::add_to_substitution_map(sub, text::variable_type::second_country, state.world.war_get_primary_defender(w));
					text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(state.world.war_get_primary_attacker(w)));
					text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
					text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

					std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
					std::string old_war_name = text::resolve_string_substitution(state, old_name, sub);
					text::add_line(state, contents, "msg_war_becomes_great_1", text::variable_type::x, std::string_view{old_war_name}, text::variable_type::val, std::string_view{resolved_war_name});
				},
				"msg_war_becomes_great_title",
				state.local_player_nation,
				sys::message_setting_type::war_becomes_great
			});
		}
	}

	notification::post(state, notification::message{
		[w, n](sys::state& state, text::layout_base& contents) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
			text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(state.world.war_get_primary_defender(w)));
			text::add_to_substitution_map(sub, text::variable_type::second_country, state.world.war_get_primary_defender(w));
			text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(state.world.war_get_primary_attacker(w)));
			text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
			text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

			std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
			text::add_line(state, contents, "msg_war_join_1", text::variable_type::x, n, text::variable_type::val, std::string_view{resolved_war_name});
		},
		"msg_war_join_title",
		n,
		sys::message_setting_type::war_join_by
	});
	if(get_role(state, w, state.local_player_nation) != war_role::none) {
		notification::post(state, notification::message{
			[w, n](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(state.world.war_get_primary_defender(w)));
				text::add_to_substitution_map(sub, text::variable_type::second_country, state.world.war_get_primary_defender(w));
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(state.world.war_get_primary_attacker(w)));
				text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
				text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

				std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
				text::add_line(state, contents, "msg_war_join_1", text::variable_type::x, n, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_war_join_title",
			state.local_player_nation,
			sys::message_setting_type::war_join_on
		});
	}

	if(!on_war_creation && state.world.nation_get_is_player_controlled(n) == false) {
		ai::add_free_ai_cbs_to_war(state, n, w);
	}

	for(auto o : state.world.nation_get_army_control(n)) {
		if(o.get_army().get_is_retreating() || o.get_army().get_black_flag() || o.get_army().get_navy_from_army_transport() || o.get_army().get_battle_from_army_battle_participation())
			continue;

		army_arrives_in_province(state, o.get_army(), o.get_army().get_location_from_army_location(), crossing_type::none);
	}
	for(auto o : state.world.nation_get_navy_control(n)) {
		if(o.get_navy().get_is_retreating() || o.get_navy().get_battle_from_navy_battle_participation())
			continue;

		auto loc = o.get_navy().get_location_from_navy_location();
		if(loc.id.index() >= state.province_definitions.first_sea_province.index())
			navy_arrives_in_province(state, o.get_navy(), loc);
	}
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
	auto new_war = fatten(state.world, state.world.create_war());

	auto real_target = primary_defender;
	auto target_ol_rel = state.world.nation_get_overlord_as_subject(primary_defender);
	if(state.world.overlord_get_ruler(target_ol_rel))
		real_target = state.world.overlord_get_ruler(target_ol_rel);

	new_war.set_primary_attacker(primary_attacker);
	new_war.set_primary_defender(real_target);
	new_war.set_start_date(state.current_date);
	new_war.set_over_state(primary_wargoal_state);
	new_war.set_over_tag(primary_wargoal_tag);
	if(primary_wargoal_secondary) {
		new_war.set_over_tag(state.world.nation_get_identity_from_identity_holder(primary_wargoal_secondary));
	}

	add_to_war(state, new_war, primary_attacker, true, true);
	add_to_war(state, new_war, real_target, false, false);

	if(primary_wargoal) {
		add_wargoal(state, new_war, primary_attacker, primary_defender, primary_wargoal, primary_wargoal_state, primary_wargoal_tag,
				primary_wargoal_secondary);
		new_war.set_name(state.world.cb_type_get_war_name(primary_wargoal));
	} else {
		auto it =
				state.key_to_text_sequence.find(std::string_view{"agression_war_name"}); // misspelling is intentional; DO NOT CORRECT
		if(it != state.key_to_text_sequence.end()) {
			new_war.set_name(it->second);
		}
	}

	if(state.world.nation_get_is_player_controlled(primary_attacker) == false)
		ai::add_free_ai_cbs_to_war(state, primary_attacker, new_war);

	notification::post(state, notification::message{
		[primary_attacker, primary_defender, w = new_war.id](sys::state& state, text::layout_base& contents) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
			text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(primary_defender));
			text::add_to_substitution_map(sub, text::variable_type::second_country, primary_defender);
			text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(primary_attacker));
			text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
			text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

			std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
			text::add_line(state, contents, "msg_war_1", text::variable_type::x, primary_attacker, text::variable_type::y, primary_defender, text::variable_type::val, std::string_view{resolved_war_name});
		},
		"msg_war_title",
		primary_attacker,
		sys::message_setting_type::war_by_nation
	});
	notification::post(state, notification::message{
		[primary_attacker, primary_defender, w = new_war.id](sys::state& state, text::layout_base& contents) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
			text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(primary_defender));
			text::add_to_substitution_map(sub, text::variable_type::second_country, primary_defender);
			text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(primary_attacker));
			text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
			text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

			std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
			text::add_line(state, contents, "msg_war_1", text::variable_type::x, primary_attacker, text::variable_type::y, primary_defender, text::variable_type::val, std::string_view{resolved_war_name});
		},
		"msg_war_title",
		primary_defender,
		sys::message_setting_type::war_on_nation
	});

	return new_war;
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
	for(auto drel : state.world.nation_get_diplomatic_relation(n)) {
		auto other_nation = drel.get_related_nations(0) != n ? drel.get_related_nations(0) : drel.get_related_nations(1);
		if(drel.get_are_allied() && standard_war_joining_is_possible(state, wfor, other_nation, false)) {

			diplomatic_message::message m;
			std::memset(&m, 0, sizeof(m));
			m.from = n;
			m.to = other_nation;
			m.type = diplomatic_message::type_t::call_ally_request;
			m.data.war = wfor;
			diplomatic_message::post(state, m);
		}
	}
	if(state.world.nation_get_in_sphere_of(n)) {
		if(joining_war_does_not_violate_constraints(state, state.world.nation_get_in_sphere_of(n), wfor, false)) {

			diplomatic_message::message m;
			std::memset(&m, 0, sizeof(m));
			m.from = n;
			m.to = state.world.nation_get_in_sphere_of(n);
			m.type = diplomatic_message::type_t::call_ally_request;
			m.data.war = wfor;
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
			diplomatic_message::post(state, m);
		}
	}
}
void add_wargoal(sys::state& state, dcon::war_id wfor, dcon::nation_id added_by, dcon::nation_id target, dcon::cb_type_id type,
		dcon::state_definition_id sd, dcon::national_identity_id tag, dcon::nation_id secondary_nation) {

	if(sd) {
		auto for_attacker = is_attacker(state, wfor, added_by);
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
		if(targets.size() == 0)
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

	if(auto on_add = state.world.cb_type_get_on_add(type); on_add) {
		effect::execute(state, on_add, trigger::to_generic(added_by), trigger::to_generic(added_by), trigger::to_generic(target),
				uint32_t(state.current_date.value), uint32_t((added_by.index() << 7) ^ target.index() ^ (type.index() << 3)));
		demographics::regenerate_from_pop_data(state);
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
			state.local_player_nation,
			sys::message_setting_type::wargoal_added
		});
	}
}

/*
void join_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool is_attacker) {
	auto wp = fatten(state.world, state.world.force_create_war_participant(w, n));
	wp.set_is_attacker(is_attacker);
	state.world.nation_set_is_at_war(n, true);

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
			auto old_name = state.world.war_get_name(w);
			state.world.war_set_is_great(w, true);
			auto it = state.key_to_text_sequence.find(std::string_view{"great_war_name"}); // misspelling is intentional; DO NOT CORRECT
			if(it != state.key_to_text_sequence.end()) {
				state.world.war_set_name(w, it->second);
			}

			notification::post(state, notification::message{
				[old_name, w](sys::state& state, text::layout_base& contents) {
					text::substitution_map sub;
					text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
					text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(state.world.war_get_primary_defender(w)));
					text::add_to_substitution_map(sub, text::variable_type::second_country, state.world.war_get_primary_defender(w));
					text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(state.world.war_get_primary_attacker(w)));
					text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
					text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

					std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
					std::string old_war_name = text::resolve_string_substitution(state, old_name, sub);
					text::add_line(state, contents, "msg_war_becomes_great_1", text::variable_type::x, std::string_view{old_war_name}, text::variable_type::val, std::string_view{resolved_war_name});
				},
				"msg_war_becomes_great_title",
				state.local_player_nation,
				sys::message_setting_type::war_becomes_great
			});
		}
	}

	notification::post(state, notification::message{
		[w, n](sys::state& state, text::layout_base& contents) {
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
			text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(state.world.war_get_primary_defender(w)));
			text::add_to_substitution_map(sub, text::variable_type::second_country, state.world.war_get_primary_defender(w));
			text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(state.world.war_get_primary_attacker(w)));
			text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
			text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

			std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
			text::add_line(state, contents, "msg_war_join_1", text::variable_type::x, n, text::variable_type::val, std::string_view{resolved_war_name});
		},
		"msg_war_join_title",
		n,
		sys::message_setting_type::war_join_by
	});
	if(get_role(state, w, state.local_player_nation) != war_role::none) {
		notification::post(state, notification::message{
			[w, n](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(state.world.war_get_primary_defender(w)));
				text::add_to_substitution_map(sub, text::variable_type::second_country, state.world.war_get_primary_defender(w));
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(state.world.war_get_primary_attacker(w)));
				text::add_to_substitution_map(sub, text::variable_type::third, state.world.war_get_over_tag(w));
				text::add_to_substitution_map(sub, text::variable_type::state, state.world.war_get_over_state(w));

				std::string resolved_war_name = text::resolve_string_substitution(state, state.world.war_get_name(w), sub);
				text::add_line(state, contents, "msg_war_join_1", text::variable_type::x, n, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_war_join_title",
			state.local_player_nation,
			sys::message_setting_type::war_join_on
		});
	}
}
*/

void remove_from_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_loss) {
	dcon::war_participant_id par;
	for(auto p : state.world.nation_get_war_participant(n)) {
		if(p.get_war() == w) {
			par = p.id;
			break;
		}
	}

	if(!par)
		return;

	for(auto vas : state.world.nation_get_overlord_as_ruler(n)) {
		remove_from_war(state, w, vas.get_subject(), as_loss);
	}

	/*
	When a losing peace offer is accepted, the ruling party in the losing nation has its party loyalty reduced by
	define:PARTY_LOYALTY_HIT_ON_WAR_LOSS percent in all provinces (this includes accepting a crisis resolution offer in which you
	lose).
	*/

	if(as_loss) {
		auto rp_ideology = state.world.nation_get_ruling_party(n).get_ideology();
		if(rp_ideology) {
			for(auto prv : state.world.nation_get_province_ownership(n)) {
				prv.get_province().get_party_loyalty(rp_ideology) *= (1.0f - state.defines.party_loyalty_hit_on_war_loss);
			}
		}
	}

	/*
	When a war goal is failed (the nation it targets leaves the war without that war goal being enacted): the nation that added it
	loses WAR_FAILED_GOAL_PRESTIGE_BASE^(WAR_FAILED_GOAL_PRESTIGE x current-prestige) x CB-penalty-factor prestige. Every pop in
	that nation gains CB-penalty-factor x define:WAR_FAILED_GOAL_MILITANCY militancy.
	*/

	float pop_militancy = 0.0f;

	std::vector<dcon::wargoal_id> rem_wargoals;
	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(wg.get_wargoal().get_added_by() == n) {
			rem_wargoals.push_back(wg.get_wargoal().id);

			float prestige_loss = std::min(state.defines.war_failed_goal_prestige_base, state.defines.war_failed_goal_prestige * nations::prestige_score(state, n)) * wg.get_wargoal().get_type().get_penalty_factor();
			nations::adjust_prestige(state, n, prestige_loss);

			pop_militancy += state.defines.war_failed_goal_militancy * wg.get_wargoal().get_type().get_penalty_factor();
		}
	}

	for(auto g : rem_wargoals) {
		state.world.delete_wargoal(g);
	}

	if(pop_militancy > 0) {
		for(auto prv : state.world.nation_get_province_ownership(n)) {
			for(auto pop : prv.get_province().get_pop_location()) {
				auto& mil = pop.get_pop().get_militancy();
				mil = std::min(mil + pop_militancy, 10.0f);
			}
		}
	}

	state.world.delete_war_participant(par);
	auto rem_wars = state.world.nation_get_war_participant(n);
	if(rem_wars.begin() == rem_wars.end()) {
		state.world.nation_set_is_at_war(n, false);
	}

	// Remove invalid occupations
	for(auto p : state.world.nation_get_province_ownership(n)) {
		if(auto c = p.get_province().get_nation_from_province_control(); c && c != n) {
			if(!military::are_at_war(state, c, n)) {
				state.world.province_set_rebel_faction_from_province_rebel_control(p.get_province(), dcon::rebel_faction_id{});
				state.world.province_set_last_control_change(p.get_province(), state.current_date);
				state.world.province_set_nation_from_province_control(p.get_province(), n);
				state.world.province_set_siege_progress(p.get_province(), 0.0f);

				military::eject_ships(state, p.get_province());
				military::update_blackflag_status(state, p.get_province());
			}
		}
	}
	for(auto p : state.world.nation_get_province_control(n)) {
		if(auto c = p.get_province().get_nation_from_province_ownership(); c && c != n) {
			if(!military::are_at_war(state, c, n)) {
				state.world.province_set_rebel_faction_from_province_rebel_control(p.get_province(), dcon::rebel_faction_id{});
				state.world.province_set_last_control_change(p.get_province(), state.current_date);
				state.world.province_set_nation_from_province_control(p.get_province(), c);
				state.world.province_set_siege_progress(p.get_province(), 0.0f);

				military::eject_ships(state, p.get_province());
				military::update_blackflag_status(state, p.get_province());
			}
		}
	}

	if(as_loss) {
		state.world.nation_set_last_war_loss(n, state.current_date);
	}
}

void cleanup_war(sys::state& state, dcon::war_id w, war_result result) {
	auto par = state.world.war_get_war_participant(w);

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

	// NOTE: we don't do this in case any of the peace offers are in flight
	// auto po = state.world.war_get_war_settlement(w);
	// while(po.begin() != po.end()) {
	//	state.world.delete_peace_offer((*po.begin()).get_peace_offer());
	//}

	auto wg = state.world.war_get_wargoals_attached(w);
	while(wg.begin() != wg.end()) {
		state.world.delete_wargoal((*wg.begin()).get_wargoal());
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

void take_from_sphere(sys::state& state, dcon::nation_id member, dcon::nation_id new_gp) {
	auto existing_sphere_leader = state.world.nation_get_in_sphere_of(member);
	if(existing_sphere_leader) {
		auto rel = state.world.get_gp_relationship_by_gp_influence_pair(member, existing_sphere_leader);
		assert(rel);
		state.world.gp_relationship_get_status(rel) &= ~nations::influence::level_mask;
		state.world.gp_relationship_get_status(rel) |= nations::influence::level_hostile;
		state.world.nation_set_in_sphere_of(member, dcon::nation_id{});
	}

	if(!nations::is_great_power(state, new_gp))
		return;

	auto nrel = state.world.get_gp_relationship_by_gp_influence_pair(member, new_gp);
	if(!nrel) {
		nrel = state.world.force_create_gp_relationship(member, new_gp);
	}

	state.world.gp_relationship_get_status(nrel) &= ~nations::influence::level_mask;
	state.world.gp_relationship_get_status(nrel) |= nations::influence::level_in_sphere;
	state.world.gp_relationship_set_influence(nrel, state.defines.max_influence);
	state.world.nation_set_in_sphere_of(member, new_gp);
}

void implement_war_goal(sys::state& state, dcon::war_id war, dcon::cb_type_id wargoal, dcon::nation_id from,
		dcon::nation_id target, dcon::nation_id secondary_nation, dcon::state_definition_id wargoal_state,
		dcon::national_identity_id wargoal_t) {
	assert(from);
	assert(target);
	assert(wargoal);

	auto bits = state.world.cb_type_get_type_bits(wargoal);
	bool for_attacker = is_attacker(state, war, from);

	// po_add_to_sphere: leaves its current sphere and has its opinion of that nation set to hostile. Is added to the nation that
	// added the war goal's sphere with max influence.
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		if(secondary_nation) {
			if(state.world.nation_get_owned_province_count(secondary_nation) != 0)
				take_from_sphere(state, secondary_nation, from);
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
		auto ol = state.world.nation_get_overlord_as_subject(secondary_nation);
		if(ol) {
			nations::release_vassal(state, ol);
		}
	}

	// po_make_puppet: the target nation releases all of its vassals and then becomes a vassal of the acting nation.
	if((bits & cb_flag::po_make_puppet) != 0) {
		for(auto sub : state.world.nation_get_overlord_as_ruler(target)) {
			nations::release_vassal(state, sub);
		}
		nations::make_vassal(state, target, from);
		take_from_sphere(state, target, from);
	}

	// po_destory_forts: reduces fort levels to zero in any targeted states
	if((bits & cb_flag::po_destroy_forts) != 0) {
		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					prov.get_province().set_building_level(economy::province_building_type::fort, 0);
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
							 trigger::to_generic(from))) {
					province::for_each_province_in_state_instance(state, si.get_state(),
							[&](dcon::province_id prov) { state.world.province_set_building_level(prov, economy::province_building_type::fort, 0); });
				}
			}
		}
	}

	// po_destory_naval_bases: as above
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					prov.get_province().set_building_level(economy::province_building_type::naval_base, 0);
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
							 trigger::to_generic(from))) {
					province::for_each_province_in_state_instance(state, si.get_state(),
							[&](dcon::province_id prov) { state.world.province_set_building_level(prov, economy::province_building_type::naval_base, 0); });
				}
			}
		}
	}

	// po_disarmament: a random define:DISARMAMENT_ARMY_HIT fraction of the nations units are destroyed. All current unit
	// constructions are canceled. The nation is disarmed. Disarmament lasts until define:REPARATIONS_YEARS or the nation is at
	// war again.
	if((bits & cb_flag::po_disarmament) != 0) {
		// TODO: destroy units
		state.world.nation_set_disarmed_until(target, state.current_date + int32_t(state.defines.reparations_years) * 365);
	}

	// po_reparations: the nation is set to pay reparations for define:REPARATIONS_YEARS
	if((bits & cb_flag::po_reparations) != 0) {
		state.world.nation_set_reparations_until(target, state.current_date + int32_t(state.defines.reparations_years) * 365);
		auto urel = state.world.get_unilateral_relationship_by_unilateral_pair(target, from);
		if(!urel) {
			urel = state.world.force_create_unilateral_relationship(target, from);
		}
		state.world.unilateral_relationship_set_reparations(urel, true);
	}

	// po_remove_prestige: the target loses (current-prestige x define:PRESTIGE_REDUCTION) + define:PRESTIGE_REDUCTION_BASE
	// prestige
	if((bits & cb_flag::po_remove_prestige) != 0) {
		nations::adjust_prestige(state, target,
				-(state.defines.prestige_reduction * nations::prestige_score(state, target) + state.defines.prestige_reduction_base));
	}

	// po_install_communist_gov: The target switches its government type and ruling ideology (if possible) to that of the nation
	// that added the war goal. Relations with the nation that added the war goal are set to 0. The nation leaves its current
	// sphere and enters the actor's sphere if it is a GP. If the war continues, the war leader on the opposite side gains the
	// appropriate `counter_wargoal_on_install_communist_gov` CB, if any and allowed by the conditions of that CB.
	if((bits & cb_flag::po_install_communist_gov_type) != 0) {
		politics::change_government_type(state, target, state.world.nation_get_government_type(from));
		politics::force_ruling_party_ideology(state, target,
				state.world.political_party_get_ideology(state.world.nation_get_ruling_party(from)));
		take_from_sphere(state, target, from);
	}

	// po_uninstall_communist_gov_type: The target switches its government type to that of the nation that added the war goal. The
	// nation leaves its current sphere and enters the actor's sphere if it is a GP.
	if((bits & cb_flag::po_uninstall_communist_gov_type) != 0) {
		politics::change_government_type(state, target, state.world.nation_get_government_type(from));
		take_from_sphere(state, target, from);
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

		auto holder = state.world.national_identity_get_nation_from_identity_holder(wargoal_t);
		if(!holder) {
			holder = state.world.create_nation();
			state.world.nation_set_identity_from_identity_holder(holder, wargoal_t);
		}
		auto lprovs = state.world.nation_get_province_ownership(holder);
		if(lprovs.begin() == lprovs.end()) {
			nations::create_nation_based_on_template(state, holder, from);
		}

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
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
							 trigger::to_generic(holder))) {
					prior_states.push_back(si.get_state());
				}
			}
			for(auto si : prior_states) {
				for(auto prov :
						state.world.state_definition_get_abstract_state_membership(state.world.state_instance_get_definition(si))) {

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
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
							 trigger::to_generic(from))) {
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

	// po_annex: nation is annexed, vassals and substates are freed, diplomatic relations are dissolved.
	if((bits & cb_flag::po_annex) != 0) {
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
	nations::adjust_prestige(state, from, prestige_gain);
	nations::adjust_prestige(state, target, -prestige_gain);
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
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_peace_offer_accepted_1", text::variable_type::x, target, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_peace_offer_accepted_title",
			target,
			sys::message_setting_type::peace_accepted_by_nation
		});
		notification::post(state, notification::message{
			[from, target, pa = state.world.war_get_primary_attacker(war), pd = state.world.war_get_primary_defender(war), name = state.world.war_get_name(war), tag = state.world.war_get_over_tag(war), st = state.world.war_get_over_state(war)](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_peace_offer_accepted_1", text::variable_type::x, target, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_peace_offer_accepted_title",
			from,
			sys::message_setting_type::peace_accepted_from_nation
		});
	}

	auto wg_range = state.world.peace_offer_get_peace_offer_item(offer);
	while(wg_range.begin() != wg_range.end()) {
		auto wg_offered = *(wg_range.begin());
		auto wg = wg_offered.get_wargoal();
		implement_war_goal(state, state.world.peace_offer_get_war_from_war_settlement(offer), wg.get_type(),
				wg.get_added_by(), wg.get_target_nation(), wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag());

		if(state.world.wargoal_is_valid(wg)) // prevent double deletion if the nation has been killed, thereby deleting the wargoal
			state.world.delete_wargoal(wg);
	}

	if(war) {
		// Nothing to implement, war should be already cleaned up
		if(state.world.nation_get_owned_province_count(from) == 0 || state.world.nation_get_owned_province_count(target) == 0)
			return;
		
		if(state.world.war_get_primary_attacker(war) == from && state.world.war_get_primary_defender(war) == target) {
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

			cleanup_war(state, war,
					state.world.peace_offer_get_is_concession(offer) ? war_result::defender_won : war_result::attacker_won);

		} else if(state.world.war_get_primary_attacker(war) == target && state.world.war_get_primary_defender(war) == from) {
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

			cleanup_war(state, war,
					state.world.peace_offer_get_is_concession(offer) ? war_result::attacker_won : war_result::defender_won);

		} else if(state.world.war_get_primary_attacker(war) == from || state.world.war_get_primary_defender(war) == from) {

			if(state.world.war_get_is_great(war) && state.world.peace_offer_get_is_concession(offer) == false) {
				implement_war_goal(state, war, state.military_definitions.standard_great_war, from, target, dcon::nation_id{},
						dcon::state_definition_id{}, dcon::national_identity_id{});
			}

			if(state.world.nation_get_owned_province_count(state.world.war_get_primary_attacker(war)) == 0)
				cleanup_war(state, war, war_result::defender_won);
			else if(state.world.nation_get_owned_province_count(state.world.war_get_primary_defender(war)) == 0)
				cleanup_war(state, war, war_result::attacker_won);
			else
				remove_from_war(state, war, target, state.world.peace_offer_get_is_concession(offer) == false);

		} else if(state.world.war_get_primary_attacker(war) == target || state.world.war_get_primary_defender(war) == target) {

			if(state.world.war_get_is_great(war) && state.world.peace_offer_get_is_concession(offer) == true) {
				implement_war_goal(state, war, state.military_definitions.standard_great_war, target, from, dcon::nation_id{},
						dcon::state_definition_id{}, dcon::national_identity_id{});
			}

			if(state.world.nation_get_owned_province_count(state.world.war_get_primary_attacker(war)) == 0)
				cleanup_war(state, war, war_result::defender_won);
			else if(state.world.nation_get_owned_province_count(state.world.war_get_primary_defender(war)) == 0)
				cleanup_war(state, war, war_result::attacker_won);
			else
				remove_from_war(state, war, from, state.world.peace_offer_get_is_concession(offer) == false);

		} else {
			assert(false);
		}
	} else { // crisis offer
		bool crisis_attackers_won = from == state.primary_crisis_attacker;

		for(auto& par : state.crisis_participants) {
			if(!par.id)
				break;

			if(par.merely_interested == false && par.id != state.primary_crisis_attacker && par.id != state.primary_crisis_defender) {
				if(par.joined_with_offer.wargoal_type) {

					bool was_part_of_offer = false;
					for(auto wg : state.world.peace_offer_get_peace_offer_item(offer)) {
						if(wg.get_wargoal().get_added_by() == par.id)
							was_part_of_offer = true;
					}
					if(!was_part_of_offer) {
						float prestige_loss = std::min(state.defines.war_failed_goal_prestige_base,
																			state.defines.war_failed_goal_prestige * state.defines.crisis_wargoal_prestige_mult *
																					nations::prestige_score(state, par.id)) *
																	state.world.cb_type_get_penalty_factor(par.joined_with_offer.wargoal_type);
						nations::adjust_prestige(state, par.id, prestige_loss);

						auto pop_militancy = state.defines.war_failed_goal_militancy * state.defines.crisis_wargoal_militancy_mult *
																 state.world.cb_type_get_penalty_factor(par.joined_with_offer.wargoal_type);
						if(pop_militancy > 0) {
							for(auto prv : state.world.nation_get_province_ownership(par.id)) {
								for(auto pop : prv.get_province().get_pop_location()) {
									auto& mil = pop.get_pop().get_militancy();
									mil = std::min(mil + pop_militancy, 10.0f);
								}
							}
						}

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
				} else {
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
		}

		if(crisis_attackers_won) {

			float p_factor = state.defines.crisis_winner_prestige_factor_base +
											 state.defines.crisis_winner_prestige_factor_year * float(state.current_date.value) / float(365);

			nations::adjust_prestige(state, state.primary_crisis_defender,
					-p_factor * nations::prestige_score(state, state.primary_crisis_attacker));
			nations::adjust_prestige(state, state.primary_crisis_attacker,
					p_factor * nations::prestige_score(state, state.primary_crisis_attacker));

			auto rp_ideology = state.world.nation_get_ruling_party(state.primary_crisis_defender).get_ideology();
			if(rp_ideology) {
				for(auto prv : state.world.nation_get_province_ownership(state.primary_crisis_defender)) {
					prv.get_province().get_party_loyalty(rp_ideology) *= (1.0f - state.defines.party_loyalty_hit_on_war_loss);
				}
			}
		} else {

			float p_factor = state.defines.crisis_winner_prestige_factor_base +
											 state.defines.crisis_winner_prestige_factor_year * float(state.current_date.value) / float(365);

			nations::adjust_prestige(state, state.primary_crisis_attacker,
					-p_factor * nations::prestige_score(state, state.primary_crisis_attacker));
			nations::adjust_prestige(state, state.primary_crisis_defender,
					p_factor * nations::prestige_score(state, state.primary_crisis_attacker));

			auto rp_ideology = state.world.nation_get_ruling_party(state.primary_crisis_attacker).get_ideology();
			if(rp_ideology) {
				for(auto prv : state.world.nation_get_province_ownership(state.primary_crisis_attacker)) {
					prv.get_province().get_party_loyalty(rp_ideology) *= (1.0f - state.defines.party_loyalty_hit_on_war_loss);
				}
			}
		}
	}
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
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_peace_offer_rejected_1", text::variable_type::x, target, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_peace_offer_rejected_title",
			target,
			sys::message_setting_type::peace_rejected_by_nation
		});
		notification::post(state, notification::message{
			[from, target, pa = state.world.war_get_primary_attacker(war), pd = state.world.war_get_primary_defender(war), name = state.world.war_get_name(war), tag = state.world.war_get_over_tag(war), st = state.world.war_get_over_state(war)](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_peace_offer_rejected_1", text::variable_type::x, target, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_peace_offer_rejected_title",
			from,
			sys::message_setting_type::peace_rejected_from_nation
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

	state.world.delete_peace_offer(offer);
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

		Increases by occupation-percentage x define:TWS_FULFILLED_SPEED (up to define:TWS_CB_LIMIT_DEFAULT) when the percentage
		occupied is >= define:TWS_FULFILLED_IDLE_SPACE or when the occupation percentage is > 0 and the current occupation score
		is negative. If there is no occupation, the score decreases by define:TWS_NOT_FULFILLED_SPEED. This can only take the
		score into negative after define:TWS_GRACE_PERIOD_DAYS, at which point it can go to -define:TWS_CB_LIMIT_DEFAULT.
		*/

		auto bits = wg.get_type().get_type_bits();
		if((bits & (cb_flag::po_annex | cb_flag::po_transfer_provinces | cb_flag::po_demand_state)) != 0) {
			float total_count = 0.0f;
			float occupied = 0.0f;
			if(wg.get_associated_state()) {
				for(auto prv : wg.get_associated_state().get_abstract_state_membership()) {
					if(prv.get_province().get_nation_from_province_ownership() == wg.get_target_nation()) {
						++total_count;
						if(get_role(state, war, prv.get_province().get_nation_from_province_control()) == role) {
							++occupied;
						}
					}
				}
			} else if((bits & cb_flag::po_annex) != 0) {
				for(auto prv : wg.get_target_nation().get_province_ownership()) {
					++total_count;
					if(get_role(state, war, prv.get_province().get_nation_from_province_control()) == role) {
						++occupied;
					}
				}
			} else if(auto allowed_states = wg.get_type().get_allowed_states(); allowed_states) {
				auto from_slot = wg.get_secondary_nation().id ? wg.get_secondary_nation().id
																											: wg.get_associated_tag().get_nation_from_identity_holder().id;
				for(auto st : wg.get_target_nation().get_state_ownership()) {
					if(trigger::evaluate(state, allowed_states, trigger::to_generic(st.get_state().id),
								 trigger::to_generic(wg.get_added_by().id), trigger::to_generic(from_slot))) {

						province::for_each_province_in_state_instance(state, st.get_state(), [&](dcon::province_id prv) {
							++total_count;
							if(get_role(state, war, state.world.province_get_nation_from_province_control(prv)) == role) {
								++occupied;
							}
						});
					}
				}
			}

			if(total_count > 0.0f) {
				float fraction = occupied / total_count;
				if(fraction >= state.defines.tws_fulfilled_idle_space || (wg.get_ticking_war_score() < 0 && total_count > 0.0f)) {
					wg.get_ticking_war_score() += state.defines.tws_fulfilled_speed * fraction;
				} else if(total_count == 0.0f) {
					if(wg.get_ticking_war_score() > 0.0f ||
							war.get_start_date() + int32_t(state.defines.tws_grace_period_days) <= state.current_date) {

						wg.get_ticking_war_score() -= state.defines.tws_not_fulfilled_speed;
					}
				}
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
					ratio =
							war.get_defender_battle_score() > 0.0f ? war.get_attacker_battle_score() / war.get_defender_battle_score() : 10.0f;
				} else {
					ratio =
							war.get_attacker_battle_score() > 0.0f ? war.get_defender_battle_score() / war.get_attacker_battle_score() : 10.0f;
				}
				if(ratio >= wg.get_type().get_tws_battle_factor()) {
					auto effective_percentage = std::min(ratio / state.defines.tws_battle_max_aspect, 1.0f);
					wg.get_ticking_war_score() += state.defines.tws_fulfilled_speed * effective_percentage;
				} else if(ratio <= 1.0f / wg.get_type().get_tws_battle_factor() && ratio > 0.0f) {
					auto effective_percentage = std::min(1.0f / (ratio * state.defines.tws_battle_max_aspect), 1.0f);
					wg.get_ticking_war_score() -= state.defines.tws_fulfilled_speed * effective_percentage;
				} else if(ratio == 0.0f) {
					wg.get_ticking_war_score() -= state.defines.tws_fulfilled_speed;
				}
			}
		}

		wg.get_ticking_war_score() =
				std::clamp(wg.get_ticking_war_score(), -state.defines.tws_cb_limit_default, state.defines.tws_cb_limit_default);
	}
}

void update_war_cleanup(sys::state& state) {
	state.world.for_each_war([&](dcon::war_id w) {
		if(state.world.nation_get_owned_province_count(state.world.war_get_primary_attacker(w)) == 0)
			military::cleanup_war(state, w, military::war_result::defender_won);
		if(state.world.nation_get_owned_province_count(state.world.war_get_primary_defender(w)) == 0)
			military::cleanup_war(state, w, military::war_result::attacker_won);
	});
}

float primary_warscore(sys::state& state, dcon::war_id w) {
	return std::clamp(
		primary_warscore_from_occupation(state, w)
		+ primary_warscore_from_battles(state, w)
		+ primary_warscore_from_war_goals(state, w), -100.0f, 100.0f);
}

float primary_warscore_from_occupation(sys::state& state, dcon::war_id w) {
	float total = 0.0f;

	auto pattacker = state.world.war_get_primary_attacker(w);
	auto pdefender = state.world.war_get_primary_defender(w);

	int32_t sum_attacker_prov_values = 0;
	int32_t sum_attacker_occupied_values = 0;
	for(auto prv : state.world.nation_get_province_ownership(pattacker)) {
		auto v = province_point_cost(state, prv.get_province(), pattacker);
		sum_attacker_prov_values += v;
		if(get_role(state, w, prv.get_province().get_nation_from_province_control()) == war_role::defender)
			sum_attacker_occupied_values += v;
	}

	int32_t sum_defender_prov_values = 0;
	int32_t sum_defender_occupied_values = 0;
	for(auto prv : state.world.nation_get_province_ownership(pdefender)) {
		auto v = province_point_cost(state, prv.get_province(), pdefender);
		sum_defender_prov_values += v;
		if(get_role(state, w, prv.get_province().get_nation_from_province_control()) == war_role::attacker)
			sum_defender_occupied_values += v;
	}

	if(sum_defender_prov_values > 0)
		total += (float(sum_defender_occupied_values) * 100.0f) / float(sum_defender_prov_values);
	if(sum_attacker_prov_values > 0)
		total -= (float(sum_attacker_occupied_values) * 100.0f) / float(sum_attacker_prov_values);

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

float directed_warscore(sys::state& state, dcon::war_id w, dcon::nation_id primary, dcon::nation_id secondary) {
	float total = 0.0f;

	auto is_pattacker = state.world.war_get_primary_attacker(w) == primary;
	auto is_pdefender = state.world.war_get_primary_defender(w) == primary;

	auto is_tpattacker = state.world.war_get_primary_attacker(w) == secondary;
	auto is_tpdefender = state.world.war_get_primary_defender(w) == secondary;

	int32_t sum_attacker_prov_values = 0;
	int32_t sum_attacker_occupied_values = 0;
	for(auto prv : state.world.nation_get_province_ownership(primary)) {
		auto v = province_point_cost(state, prv.get_province(), primary);
		sum_attacker_prov_values += v;

		if(is_tpattacker) {
			if(get_role(state, w, prv.get_province().get_nation_from_province_control()) == war_role::attacker)
				sum_attacker_occupied_values += v;
		} else if(is_tpdefender) {
			if(get_role(state, w, prv.get_province().get_nation_from_province_control()) == war_role::defender)
				sum_attacker_occupied_values += v;
		} else {
			if(prv.get_province().get_nation_from_province_control() == secondary)
				sum_attacker_occupied_values += v;
		}
	}

	int32_t sum_defender_prov_values = 0;
	int32_t sum_defender_occupied_values = 0;
	for(auto prv : state.world.nation_get_province_ownership(secondary)) {
		auto v = province_point_cost(state, prv.get_province(), secondary);
		sum_defender_prov_values += v;

		if(is_pattacker) {
			if(get_role(state, w, prv.get_province().get_nation_from_province_control()) == war_role::attacker)
				sum_defender_occupied_values += v;
		} else if(is_pdefender) {
			if(get_role(state, w, prv.get_province().get_nation_from_province_control()) == war_role::defender)
				sum_defender_occupied_values += v;
		} else {
			if(prv.get_province().get_nation_from_province_control() == primary)
				sum_defender_occupied_values += v;
		}
	}

	if(sum_defender_prov_values > 0)
		total += (float(sum_defender_occupied_values) * 100.0f) / float(sum_defender_prov_values);
	if(sum_attacker_prov_values > 0)
		total -= (float(sum_attacker_occupied_values) * 100.0f) / float(sum_attacker_prov_values);

	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if((wg.get_wargoal().get_added_by() == primary || is_pattacker || is_pdefender)
			&& wg.get_wargoal().get_target_nation() == secondary) {

			total += wg.get_wargoal().get_ticking_war_score();
		} else if(wg.get_wargoal().get_added_by() == secondary
			&& (wg.get_wargoal().get_target_nation() == primary || is_pattacker || is_pdefender)) {

			total -= wg.get_wargoal().get_ticking_war_score();
		} else if(wg.get_wargoal().get_added_by() == primary
			&& (wg.get_wargoal().get_target_nation() == secondary || is_tpattacker || is_tpdefender)) {

			total += wg.get_wargoal().get_ticking_war_score();
		} else if((wg.get_wargoal().get_added_by() == secondary || is_tpattacker || is_tpdefender)
			&& (wg.get_wargoal().get_target_nation() == primary)) {

			total -= wg.get_wargoal().get_ticking_war_score();
		}
	}

	return std::clamp(total, 0.0f, 100.0f);
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
	 slowest ship or regiment x (1 + infrastructure-provided-by-railroads x railroad-level-of-origin) x
	 (possibly-some-modifier-for-crossing-water) x (define:LAND_SPEED_MODIFIER or define:NAVAL_SPEED_MODIFIER) x (leader-speed-trait
	 + 1)
	*/
	auto leader = state.world.army_get_general_from_army_leadership(a);
	auto bg = state.world.leader_get_background(leader);
	auto per = state.world.leader_get_personality(leader);
	auto leader_move = state.world.leader_trait_get_speed(bg) + state.world.leader_trait_get_speed(per);
	return min_speed * (state.world.army_get_is_retreating(a) ? 2.0f : 1.0f) *
				 (1.0f + state.world.province_get_building_level(state.world.army_get_location_from_army_location(a), economy::province_building_type::railroad) *
										 state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure) *
				 (leader_move + 1.0f);
}
float effective_navy_speed(sys::state& state, dcon::navy_id n) {
	auto owner = state.world.navy_get_controller_from_navy_control(n);

	float min_speed = 10000.0f;
	for(auto reg : state.world.navy_get_navy_membership(n)) {
		auto reg_speed = state.world.nation_get_unit_stats(owner, reg.get_ship().get_type()).maximum_speed;
		min_speed = std::min(min_speed, reg_speed);
	}

	auto leader = state.world.navy_get_admiral_from_navy_leadership(n);
	auto bg = state.world.leader_get_background(leader);
	auto per = state.world.leader_get_personality(leader);
	auto leader_move = state.world.leader_trait_get_speed(bg) + state.world.leader_trait_get_speed(per);
	return min_speed * (state.world.navy_get_is_retreating(n) ? 2.0f : 1.0f) * (leader_move + 1.0f);
}

sys::date arrival_time_to(sys::state& state, dcon::army_id a, dcon::province_id p) {
	auto current_location = state.world.army_get_location_from_army_location(a);
	auto adj = state.world.get_province_adjacency_by_province_pair(current_location, p);
	float distance = province::distance(state, adj);
	float sum_mods = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::movement_cost) +
									 state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::movement_cost);
	float effective_distance = distance * (sum_mods + 1.0f);

	float effective_speed = effective_army_speed(state, a);

	int32_t days = effective_speed > 0.0f ? int32_t(std::ceil(effective_distance / effective_speed)) : 50;
	return state.current_date + days;
}
sys::date arrival_time_to(sys::state& state, dcon::navy_id n, dcon::province_id p) {
	auto current_location = state.world.navy_get_location_from_navy_location(n);
	auto adj = state.world.get_province_adjacency_by_province_pair(current_location, p);
	float distance = province::distance(state, adj);
	float sum_mods = state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::movement_cost) +
									 state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::movement_cost);
	float effective_distance = distance * (sum_mods + 1.0f);

	float effective_speed = effective_navy_speed(state, n);

	int32_t days = effective_speed > 0.0f ? int32_t(std::ceil(effective_distance / effective_speed)) : 50;
	return state.current_date + days;
}

void add_army_to_battle(sys::state& state, dcon::army_id a, dcon::land_battle_id b, war_role r) {
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
				reserves.push_back(
						reserve_regiment{reg.get_regiment().id, reserve_regiment::is_attacking | reserve_regiment::type_infantry});
				state.world.land_battle_get_attacker_infantry(b) += reg.get_regiment().get_strength();
				break;
			case unit_type::cavalry:
				reserves.push_back(
						reserve_regiment{reg.get_regiment().id, reserve_regiment::is_attacking | reserve_regiment::type_cavalry});
				state.world.land_battle_get_attacker_cav(b) += reg.get_regiment().get_strength();
				break;
			case unit_type::special:
			case unit_type::support:
				reserves.push_back(
						reserve_regiment{reg.get_regiment().id, reserve_regiment::is_attacking | reserve_regiment::type_support});
				state.world.land_battle_get_attacker_support(b) += reg.get_regiment().get_strength();
				break;
			default:
				assert(false);
			}
		}
	} else {
		auto& def_bonus = state.world.land_battle_get_defender_bonus(b);
		auto prev_dig_in = def_bonus | defender_bonus_dig_in_mask;
		auto new_dig_in = std::min(prev_dig_in, state.world.army_get_dig_in(a) & defender_bonus_dig_in_mask);
		def_bonus &= ~defender_bonus_dig_in_mask;
		def_bonus |= new_dig_in;

		if(!state.world.land_battle_get_general_from_defending_general(b)) {
			state.world.land_battle_set_general_from_defending_general(b, state.world.army_get_general_from_army_leadership(a));
		}
		auto reserves = state.world.land_battle_get_reserves(b);
		for(auto reg : state.world.army_get_army_membership(a)) {
			auto type = state.military_definitions.unit_base_definitions[reg.get_regiment().get_type()].type;
			switch(type) {
			case unit_type::infantry:
				reserves.push_back(reserve_regiment{reg.get_regiment().id, reserve_regiment::type_infantry});
				state.world.land_battle_get_defender_infantry(b) += reg.get_regiment().get_strength();
				break;
			case unit_type::cavalry:
				reserves.push_back(reserve_regiment{reg.get_regiment().id, reserve_regiment::type_cavalry});
				state.world.land_battle_get_defender_cav(b) += reg.get_regiment().get_strength();
				break;
			case unit_type::special:
			case unit_type::support:
				reserves.push_back(reserve_regiment{reg.get_regiment().id, reserve_regiment::type_support});
				state.world.land_battle_get_defender_support(b) += reg.get_regiment().get_strength();
				break;
			default:
				assert(false);
			}
		}
	}

	state.world.army_set_battle_from_army_battle_participation(a, b);
	state.world.army_set_arrival_time(a, sys::date{}); // pause movement
}

void army_arrives_in_province(sys::state& state, dcon::army_id a, dcon::province_id p, crossing_type crossing, dcon::land_battle_id from) {
	state.world.army_set_location_from_army_location(a, p);
	if(!state.world.army_get_black_flag(a) && !state.world.army_get_is_retreating(a)) {
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

				auto cw_a = state.defines.base_combat_width -
					state.world.nation_get_modifier_values(owner_nation, sys::national_mod_offsets::combat_width);
				auto cw_b = state.defines.base_combat_width -
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

				auto cw_a = state.defines.base_combat_width -
					state.world.nation_get_modifier_values(owner_nation, sys::national_mod_offsets::combat_width);
				auto cw_b = state.defines.base_combat_width -
					state.world.nation_get_modifier_values(other_nation, sys::national_mod_offsets::combat_width);
				new_battle.set_combat_width(uint8_t(
					std::clamp(int32_t(std::min(cw_a, cw_b) *
						(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::combat_width) + 1.0f)),
						2, 30)));

				add_army_to_battle(state, a, new_battle, par.role);
				add_army_to_battle(state, o.get_army(), new_battle,
						par.role == war_role::attacker ? war_role::defender : war_role::attacker);

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
}

void add_navy_to_battle(sys::state& state, dcon::navy_id n, dcon::naval_battle_id b, war_role r) {
	bool battle_attacker = (r == war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);
	if(battle_attacker) {
		// try add admiral as leader
		if(!state.world.naval_battle_get_admiral_from_attacking_admiral(b)) {
			state.world.naval_battle_set_admiral_from_attacking_admiral(b, state.world.navy_get_admiral_from_navy_leadership(n));
		}
		// put ships in slots
		auto slots = state.world.naval_battle_get_slots(b);
		for(auto ship : state.world.navy_get_navy_membership(n)) {
			auto type = state.military_definitions.unit_base_definitions[ship.get_ship().get_type()].type;
			switch(type) {
			case unit_type::big_ship:
				slots.push_back(ship_in_battle{ship.get_ship().id, 0,
						1000 | ship_in_battle::mode_seeking | ship_in_battle::is_attacking | ship_in_battle::type_big});
				state.world.naval_battle_get_attacker_big_ships(b)++;
				break;
			case unit_type::light_ship:
				slots.push_back(ship_in_battle{ship.get_ship().id, 0,
						1000 | ship_in_battle::mode_seeking | ship_in_battle::is_attacking | ship_in_battle::type_small});
				state.world.naval_battle_get_attacker_small_ships(b)++;
				break;
			case unit_type::transport:
				slots.push_back(ship_in_battle{ship.get_ship().id, 0,
						1000 | ship_in_battle::mode_seeking | ship_in_battle::is_attacking | ship_in_battle::type_transport});
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
			auto type = state.military_definitions.unit_base_definitions[ship.get_ship().get_type()].type;
			switch(type) {
			case unit_type::big_ship:
				slots.push_back(ship_in_battle{ship.get_ship().id, 0, 1000 | ship_in_battle::mode_seeking | ship_in_battle::type_big});
				state.world.naval_battle_get_defender_big_ships(b)++;
				break;
			case unit_type::light_ship:
				slots.push_back(ship_in_battle{ship.get_ship().id, 0, 1000 | ship_in_battle::mode_seeking | ship_in_battle::type_small});
				state.world.naval_battle_get_defender_small_ships(b)++;
				break;
			case unit_type::transport:
				slots.push_back(
						ship_in_battle{ship.get_ship().id, 0, 1000 | ship_in_battle::mode_seeking | ship_in_battle::type_transport});
				state.world.naval_battle_get_defender_transport_ships(b)++;
				break;
			default:
				assert(false);
			}
		}
	}

	state.world.navy_set_battle_from_navy_battle_participation(n, b);
	state.world.navy_set_arrival_time(n, sys::date{}); // pause movement

	for(auto em : state.world.navy_get_army_transport(n)) {
		em.get_army().set_arrival_time(sys::date{});
	}
}

bool retreat(sys::state& state, dcon::navy_id n) {
	auto province_start = state.world.navy_get_location_from_navy_location(n);
	auto nation_controller = state.world.navy_get_controller_from_navy_control(n);

	auto retreat_path = province::make_naval_retreat_path(state, nation_controller, province_start);
	if(retreat_path.size() > 0) {
		state.world.navy_set_is_retreating(n, true);
		auto existing_path = state.world.navy_get_path(n);
		existing_path.load_range(retreat_path.data(), retreat_path.data() + retreat_path.size());

		state.world.navy_set_arrival_time(n, arrival_time_to(state, n, retreat_path.back()));

		for(auto em : state.world.navy_get_army_transport(n)) {
			em.get_army().get_path().clear();
		}
		return true;
	} else {
		return false;
	}
}

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

		state.world.army_set_arrival_time(n, arrival_time_to(state, n, retreat_path.back()));
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

void cleanup_army(sys::state& state, dcon::army_id n) {
	auto regs = state.world.army_get_army_membership(n);
	while(regs.begin() != regs.end()) {
		state.world.delete_regiment((*regs.begin()).get_regiment());
	}

	auto b = state.world.army_get_battle_from_army_battle_participation(n);
	if(b) {
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
		} else {
			assert(state.world.army_get_controller_from_army_rebel_control(n));
			for(auto bp : state.world.land_battle_get_army_battle_participation_as_battle(b)) {
				if(bp.get_army() != n && bp.get_army().get_army_rebel_control()) {
					should_end = false;
				}
			}
		}
		
		if(should_end) {
			bool as_attacker = state.world.land_battle_get_war_attacker_is_attacker(b);
			end_battle(state, b, as_attacker ? battle_result::defender_won : battle_result::attacker_won);
		}
	}

	state.world.delete_army(n);
}

void cleanup_navy(sys::state& state, dcon::navy_id n) {
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
	if(b) {
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

void end_battle(sys::state& state, dcon::land_battle_id b, battle_result result) {
	auto war = state.world.land_battle_get_war_from_land_battle_in_war(b);
	auto location = state.world.land_battle_get_location_from_land_battle_location(b);

	assert(location);

	static std::vector<dcon::army_id> to_delete;
	to_delete.clear();

	for(auto n : state.world.land_battle_get_army_battle_participation(b)) {
		auto nation_owner = state.world.army_get_controller_from_army_control(n.get_army());

		auto role_in_war = bool(war)
			? get_role(state, war, n.get_army().get_controller_from_army_control())
			: (bool(nation_owner) ? war_role::defender : war_role::attacker);

		bool battle_attacker = (role_in_war == war_role::attacker) == state.world.land_battle_get_war_attacker_is_attacker(b);

		auto members = n.get_army().get_army_membership();
		if(members.begin() == members.end()) {
			to_delete.push_back(n.get_army());
			continue;
		}

		if(battle_attacker && result == battle_result::defender_won) {
			if(!can_retreat_from_battle(state, b)) {
				to_delete.push_back(n.get_army());
			} else {
				if(!retreat(state, n.get_army()))
					to_delete.push_back(n.get_army());
			}
		} else if(!battle_attacker && result == battle_result::attacker_won) {
			if(!can_retreat_from_battle(state, b)) {
				to_delete.push_back(n.get_army());
			} else {
				if(!retreat(state, n.get_army()))
					to_delete.push_back(n.get_army());
			}
		} else {
			auto path = n.get_army().get_path();
			if(path.size() > 0) {
				state.world.army_set_arrival_time(n.get_army(), arrival_time_to(state, n.get_army(), path.at(path.size() - 1)));
			}
		}

		if(result != battle_result::indecisive) { // so we don't restart battles as the war is ending
			army_arrives_in_province(state, n.get_army(), location, crossing_type::none, b);
		}
	}

	/*
	On finishing a battle:
	Each winning combatant get a random `on_battle_won` event, and each losing combatant gets a random `on_battle_lost` event

	War score is gained based on the difference in losses (in absolute terms) divided by 5 plus 0.1 to a minimum of 0.1
	*/

	if(result != battle_result::indecisive) {
		if(war)
			state.world.war_get_number_of_battles(war)++;

		if(result == battle_result::attacker_won) {
			auto total_def_loss = state.world.land_battle_get_defender_cav_lost(b) + state.world.land_battle_get_defender_infantry_lost(b) + state.world.land_battle_get_defender_support_lost(b);
			auto total_att_loss = state.world.land_battle_get_attacker_cav_lost(b) + state.world.land_battle_get_attacker_infantry_lost(b) + state.world.land_battle_get_attacker_support_lost(b);
			auto score = std::max(0.0f, 3.0f * (total_def_loss - total_att_loss) / 10.0f);

			if(war) {
				if(state.world.land_battle_get_war_attacker_is_attacker(b)) {
					state.world.war_get_attacker_battle_score(war) += score;
				} else {
					state.world.war_get_defender_battle_score(war) += score;
				}
			}

			auto a_nation = get_land_battle_lead_attacker(state, b);
			auto d_nation = get_land_battle_lead_defender(state, b);

			if(a_nation && d_nation) { // no prestige for beating up rebels
				nations::adjust_prestige(state, a_nation, score / 50.0f);
				nations::adjust_prestige(state, d_nation, score / -50.0f);
			}

			// Report
			[&]() {
				for(auto nv : state.world.land_battle_get_army_battle_participation(b)) {
					if(state.local_player_nation == nv.get_army().get_controller_from_army_control()) {
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

						return;
					}
				}
			}();

		} else if(result == battle_result::defender_won) {
			auto total_def_loss = state.world.land_battle_get_defender_cav_lost(b) + state.world.land_battle_get_defender_infantry_lost(b) + state.world.land_battle_get_defender_support_lost(b);
			auto total_att_loss = state.world.land_battle_get_attacker_cav_lost(b) + state.world.land_battle_get_attacker_infantry_lost(b) + state.world.land_battle_get_attacker_support_lost(b);
			auto score = std::max(0.0f, 3.0f * (total_att_loss - total_def_loss) / 10.0f);

			if(war) {
				if(state.world.land_battle_get_war_attacker_is_attacker(b)) {
					state.world.war_get_attacker_battle_score(war) += score;
				} else {
					state.world.war_get_defender_battle_score(war) += score;
				}
			}

			auto a_nation = get_land_battle_lead_attacker(state, b);
			auto d_nation = get_land_battle_lead_defender(state, b);

			if(a_nation && d_nation) {
				nations::adjust_prestige(state, a_nation, score / -50.0f);
				nations::adjust_prestige(state, d_nation, score / 50.0f);
			}

			// Report

			[&]() {
				for(auto nv : state.world.land_battle_get_army_battle_participation(b)) {
					if(state.local_player_nation == nv.get_army().get_controller_from_army_control()) {
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

						return;
					}
				}
			}();
		}
	}

	state.world.delete_land_battle(b);

	for(auto n : to_delete) {
		cleanup_army(state, n);
	}
}

void end_battle(sys::state& state, dcon::naval_battle_id b, battle_result result) {
	auto war = state.world.naval_battle_get_war_from_naval_battle_in_war(b);
	auto location = state.world.naval_battle_get_location_from_naval_battle_location(b);

	assert(war);
	assert(location);

	static std::vector<dcon::navy_id> to_delete;
	to_delete.clear();
	static std::vector<dcon::army_id> army_to_delete;
	army_to_delete.clear();

	for(auto n : state.world.naval_battle_get_navy_battle_participation(b)) {
		auto role_in_war = get_role(state, war, n.get_navy().get_controller_from_navy_control());
		bool battle_attacker = (role_in_war == war_role::attacker) == state.world.naval_battle_get_war_attacker_is_attacker(b);

		auto members = n.get_navy().get_navy_membership();
		if(members.begin() == members.end()) {
			to_delete.push_back(n.get_navy());
			continue;
		}

		auto transport_cap = military::free_transport_capacity(state, n.get_navy());
		if(transport_cap < 0) {
			for(auto em : n.get_navy().get_army_transport()) {
				auto em_regs = em.get_army().get_army_membership();
				while(em_regs.begin() != em_regs.end() && transport_cap < 0) {
					state.world.delete_regiment((*em_regs.begin()).get_regiment());
					++transport_cap;
				}
				if(em_regs.begin() == em_regs.end())
					army_to_delete.push_back(em.get_army());
				if(transport_cap >= 0)
					break;
			}
		}

		if(battle_attacker && result == battle_result::defender_won) {
			if(!can_retreat_from_battle(state, b)) {
				to_delete.push_back(n.get_navy());
			} else {
				if(!retreat(state, n.get_navy()))
					to_delete.push_back(n.get_navy());
			}
		} else if(!battle_attacker && result == battle_result::attacker_won) {
			if(!can_retreat_from_battle(state, b)) {
				to_delete.push_back(n.get_navy());
			} else {
				if(!retreat(state, n.get_navy()))
					to_delete.push_back(n.get_navy());
			}
		} else {
			auto path = n.get_navy().get_path();
			if(path.size() > 0) {
				state.world.navy_set_arrival_time(n.get_navy(), arrival_time_to(state, n.get_navy(), path.at(path.size() - 1)));
			}

			for(auto em : n.get_navy().get_army_transport()) {
				auto apath = em.get_army().get_path();
				if(apath.size() > 0) {
					state.world.army_set_arrival_time(em.get_army(), arrival_time_to(state, em.get_army(), apath.at(apath.size() - 1)));
				}
			}
		}

		if(result != battle_result::indecisive) { // so we don't restart battles as the war is ending
			navy_arrives_in_province(state, n.get_navy(), location, b);
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
		state.world.war_get_number_of_battles(war)++;

		if(result == battle_result::attacker_won) {
			auto score = std::max(0.0f,
					(state.world.naval_battle_get_defender_loss_value(b) - state.world.naval_battle_get_attacker_loss_value(b)) / 10.0f);
			if(state.world.naval_battle_get_war_attacker_is_attacker(b)) {
				state.world.war_get_attacker_battle_score(war) += score;
			} else {
				state.world.war_get_defender_battle_score(war) += score;
			}

			auto a_nation = get_naval_battle_lead_attacker(state, b);
			auto d_nation = get_naval_battle_lead_defender(state, b);

			assert(a_nation);
			assert(d_nation);

			nations::adjust_prestige(state, a_nation, score / 50.0f);
			nations::adjust_prestige(state, d_nation, score / -50.0f);

			// Report
			[&]() {
				for(auto nv : state.world.naval_battle_get_navy_battle_participation(b)) {
					if(state.local_player_nation == nv.get_navy().get_controller_from_navy_control()) {
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

						return;
					}
				}
			}();

		} else if(result == battle_result::defender_won) {
			auto score = std::max(0.0f,
					(state.world.naval_battle_get_attacker_loss_value(b) - state.world.naval_battle_get_defender_loss_value(b)) / 10.0f);
			if(state.world.naval_battle_get_war_attacker_is_attacker(b)) {
				state.world.war_get_attacker_battle_score(war) += score;
			} else {
				state.world.war_get_defender_battle_score(war) += score;
			}

			auto a_nation = get_naval_battle_lead_attacker(state, b);
			auto d_nation = get_naval_battle_lead_defender(state, b);

			assert(a_nation);
			assert(d_nation);

			nations::adjust_prestige(state, a_nation, score / -50.0f);
			nations::adjust_prestige(state, d_nation, score / 50.0f);

			// Report
			[&]() {
				for(auto nv : state.world.naval_battle_get_navy_battle_participation(b)) {
					if(state.local_player_nation == nv.get_navy().get_controller_from_navy_control()) {
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

						return;
					}
				}
			}();
		}
	}

	state.world.delete_naval_battle(b);

	for(auto n : to_delete) {
		cleanup_navy(state, n);
	}
	for(auto a : army_to_delete) {
		cleanup_army(state, a);
	}
}

inline constexpr float combat_modifier_table[] = {0.0f, 0.02f, 0.04f, 0.06f, 0.08f, 0.10f, 0.12f, 0.16f, 0.20f, 0.25f, 0.30f,
		0.35f, 0.40f, 0.45f, 0.50f, 0.60f, 0.70f, 0.80f, 0.90f};

dcon::nation_id tech_nation_for_regiment(sys::state& state, dcon::regiment_id r) {
	auto army = state.world.regiment_get_army_from_army_membership(r);
	auto nation = state.world.army_get_controller_from_army_control(army);

	if(nation)
		return nation;

	auto rf = state.world.army_get_controller_from_army_rebel_control(army);
	return state.world.rebel_faction_get_ruler_from_rebellion_within(rf);
}

bool will_recieve_attrition(sys::state& state, dcon::navy_id a) {
	return false;
}

float peacetime_attrition_limit(sys::state& state, dcon::nation_id n, dcon::province_id prov) {
	auto supply_limit = supply_limit_in_province(state, n, prov);
	auto prov_attrition_mod = state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::attrition);
	auto attrition_mod = 1.0f + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::land_attrition);

	return (supply_limit + prov_attrition_mod) / attrition_mod;
}

bool will_recieve_attrition(sys::state& state, dcon::army_id a) {
	auto prov = state.world.army_get_location_from_army_location(a);

	if(state.world.province_get_siege_progress(prov) > 0.f)
		return true;

	float total_army_weight = 0;
	for(auto ar : state.world.province_get_army_location(prov)) {
		if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false &&
				!bool(ar.get_army().get_navy_from_army_transport())) {

			for(auto rg : ar.get_army().get_army_membership()) {
				total_army_weight += 3.0f * rg.get_regiment().get_strength();
			}
		}
	}

	auto prov_attrition_mod = state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::attrition);

	
	auto ar = fatten(state.world, a);

	auto army_controller = ar.get_controller_from_army_control();
	auto supply_limit = supply_limit_in_province(state, army_controller, prov);
	auto attrition_mod = 1.0f + army_controller.get_modifier_values(sys::national_mod_offsets::land_attrition);

	float greatest_hostile_fort = 0.0f;

	for(auto adj : state.world.province_get_province_adjacency(prov)) {
		if((adj.get_type() & (province::border::impassible_bit | province::border::coastal_bit)) == 0) {
			auto other = adj.get_connected_provinces(0) != prov ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);
			if(other.get_building_level(economy::province_building_type::fort) > 0) {
				if(are_at_war(state, army_controller, other.get_nation_from_province_control())) {
					greatest_hostile_fort = std::max(greatest_hostile_fort, float(other.get_building_level(economy::province_building_type::fort)));
				}
			}
		}
	}

	return total_army_weight * attrition_mod - (supply_limit + prov_attrition_mod + greatest_hostile_fort) > 0;
}

void apply_attrition(sys::state& state) {
	concurrency::parallel_for(0, state.province_definitions.first_sea_province.index(), [&](int32_t i) {
		dcon::province_id prov{dcon::province_id::value_base_t(i)};
		float total_army_weight = 0;
		for(auto ar : state.world.province_get_army_location(prov)) {
			if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false &&
					!bool(ar.get_army().get_navy_from_army_transport())) {

				for(auto rg : ar.get_army().get_army_membership()) {
					total_army_weight += 3.0f * rg.get_regiment().get_strength();
				}
			}
		}

		auto prov_attrition_mod = state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::attrition);

		for(auto ar : state.world.province_get_army_location(prov)) {
			if(ar.get_army().get_black_flag() == false && ar.get_army().get_is_retreating() == false &&
					!bool(ar.get_army().get_navy_from_army_transport())) {

				auto army_controller = ar.get_army().get_controller_from_army_control();
				auto supply_limit = supply_limit_in_province(state, army_controller, prov);
				auto attrition_mod = 1.0f + army_controller.get_modifier_values(sys::national_mod_offsets::land_attrition);
				
				float greatest_hostile_fort = 0.0f;

				for(auto adj : state.world.province_get_province_adjacency(prov)) {
					if((adj.get_type() & (province::border::impassible_bit | province::border::coastal_bit)) == 0) {
						auto other = adj.get_connected_provinces(0) != prov ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);
						if(other.get_building_level(economy::province_building_type::fort) > 0) {
							if(are_at_war(state, army_controller, other.get_nation_from_province_control())) {
								greatest_hostile_fort = std::max(greatest_hostile_fort, float(other.get_building_level(economy::province_building_type::fort)));
							}
						}
					}
				}

				/*
				First we calculate (total-strength + leader-attrition-trait) x (attrition-modifier-from-technology + 1) -
				effective-province-supply-limit (rounded down to the nearest integer) + province-attrition-modifier +
				the-level-of-the-highest-hostile-fort-in-an-adjacent-province. We then reduce that value to at most the max-attrition
				modifier of the province, and finally we add define:SEIGE_ATTRITION if the army is conducting a siege. Units taking
				attrition lose max-strength x attrition-value x 0.01 points of strength. This strength loss is treated just like damage
				taken in combat, meaning that it will reduce the size of the backing pop.
				*/

				float attrition_value =
					std::clamp(total_army_weight * attrition_mod - (supply_limit + prov_attrition_mod + greatest_hostile_fort), 0.0f, state.world.province_get_modifier_values(prov, sys::provincial_mod_offsets::max_attrition))
					+ state.world.province_get_siege_progress(prov) > 0.f ? state.defines.siege_attrition : 0.0f;

				for(auto rg : ar.get_army().get_army_membership()) {
					rg.get_regiment().get_pending_damage() += attrition_value * 0.01f;
					rg.get_regiment().get_strength() -= attrition_value * 0.01f;
				}
			}
		}
	});
}

void apply_regiment_damage(sys::state& state) {
	for(uint32_t i = state.world.regiment_size(); i-- > 0;) {
		dcon::regiment_id s{dcon::regiment_id::value_base_t(i)};
		if(state.world.regiment_is_valid(s)) {
			auto& pending_damage = state.world.regiment_get_pending_damage(s);
			auto current_strength = state.world.regiment_get_strength(s);

			if(pending_damage > 0) {
				auto backing_pop = state.world.regiment_get_pop_from_regiment_source(s);
				auto tech_nation = tech_nation_for_regiment(state, s);

				if(backing_pop) {
					auto& psize = state.world.pop_get_size(backing_pop);
					psize -= state.defines.pop_size_per_regiment * pending_damage * state.defines.soldier_to_pop_damage /
									 (3.0f * (1.0f + state.world.nation_get_modifier_values(tech_nation,
																			 sys::national_mod_offsets::soldier_to_pop_loss)));
					if(psize <= 1.0f) {
						state.world.delete_pop(backing_pop);
					}
				}
				pending_damage = 0.0f;
			}
			if(current_strength <= 0.0f) {
				// When a rebel regiment is destroyed, divide the militancy of the backing pop by define:REDUCTION_AFTER_DEFEAT.
				auto army = state.world.regiment_get_army_from_army_membership(s);
				auto controller = state.world.army_get_controller_from_army_control(army);
				if(!controller) {
					auto pop_backer = state.world.regiment_get_pop_from_regiment_source(s);
					if(pop_backer) {
						state.world.pop_get_militancy(pop_backer) /= state.defines.reduction_after_defeat;
					}
				} else {
					auto maxr = state.world.nation_get_recruitable_regiments(controller);
					if(maxr > 0) {
						auto pop_backer = state.world.regiment_get_pop_from_regiment_source(s);
						if(pop_backer) {
							auto& wex = state.world.nation_get_war_exhaustion(controller);
							wex = std::min(wex + 0.5f / float(maxr),
									state.world.nation_get_modifier_values(controller, sys::national_mod_offsets::max_war_exhaustion));
						}
					}
				}
				state.world.delete_regiment(s);
				auto army_regs = state.world.army_get_army_membership(army);
				if(army_regs.begin() == army_regs.end()) {
					military::cleanup_army(state, army);
				}
			}
		}
	}
}

void update_land_battles(sys::state& state) {
	auto to_delete = ve::vectorizable_buffer<uint8_t, dcon::land_battle_id>(state.world.land_battle_size());

	concurrency::parallel_for(0, int32_t(state.world.land_battle_size()), [&](int32_t index) {
		dcon::land_battle_id b{dcon::land_battle_id::value_base_t(index)};

		if(!state.world.land_battle_is_valid(b))
			return;

		// fill to combat width
		auto combat_width = state.world.land_battle_get_combat_width(b);

		auto& att_back = state.world.land_battle_get_attacker_back_line(b);
		auto& def_back = state.world.land_battle_get_defender_back_line(b);
		auto& att_front = state.world.land_battle_get_attacker_front_line(b);
		auto& def_front = state.world.land_battle_get_defender_front_line(b);

		auto reserves = state.world.land_battle_get_reserves(b);

		if((state.current_date.value - state.world.land_battle_get_start_date(b).value) % 5 == 4) {
			state.world.land_battle_set_dice_rolls(b, make_dice_rolls(state, uint32_t(index)));
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
		auto dig_in_value = defender_mods & defender_bonus_dig_in_mask;
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

		auto attacker_per = state.world.leader_get_personality(state.world.land_battle_get_general_from_attacking_general(b));
		auto attacker_bg = state.world.leader_get_background(state.world.land_battle_get_general_from_attacking_general(b));

		auto attack_bonus =
				int32_t(state.world.leader_trait_get_attack(attacker_per) + state.world.leader_trait_get_attack(attacker_bg));
		auto attacker_org_bonus =
				1.0f + state.world.leader_trait_get_organisation(attacker_per) + state.world.leader_trait_get_organisation(attacker_bg);

		auto defender_per = state.world.leader_get_personality(state.world.land_battle_get_general_from_attacking_general(b));
		auto defender_bg = state.world.leader_get_background(state.world.land_battle_get_general_from_attacking_general(b));

		auto defence_bonus =
				int32_t(state.world.leader_trait_get_defense(defender_per) + state.world.leader_trait_get_defense(defender_bg));
		auto defender_org_bonus =
				1.0f + state.world.leader_trait_get_organisation(defender_per) + state.world.leader_trait_get_organisation(defender_bg);

		auto attacker_mod = combat_modifier_table[std::clamp(attacker_dice + attack_bonus + crossing_adjustment +  int32_t(attacker_gas ? state.defines.gas_attack_modifier : 0.0f) + 3, 0, 19)];
		auto defender_mod = combat_modifier_table[std::clamp(defender_dice + defence_bonus + dig_in_value +  int32_t(defender_gas ? state.defines.gas_attack_modifier : 0.0f) + int32_t(terrain_bonus) + 3, 0, 19)];

		float defender_fort = 1.0f;
		auto local_control = state.world.province_get_nation_from_province_control(location);
		if((!attacking_nation && local_control) ||
				(attacking_nation && (!bool(local_control) || military::are_at_war(state, attacking_nation, local_control)))) {
			defender_fort = 1.0f + 0.1f * state.world.province_get_building_level(location, economy::province_building_type::fort);
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

		for(int32_t i = 0; i < combat_width; ++i) {
			if(att_back[i] && def_front[i]) {
				auto tech_att_nation = tech_nation_for_regiment(state, att_back[i]);
				auto tech_def_nation = tech_nation_for_regiment(state, def_front[i]);

				auto& att_stats = state.world.nation_get_unit_stats(tech_att_nation, state.world.regiment_get_type(att_back[i]));
				auto& def_stats = state.world.nation_get_unit_stats(tech_def_nation, state.world.regiment_get_type(def_front[i]));

				auto str_damage =
						(att_stats.attack_or_gun_power * 0.1f + 1.0f) * att_stats.support * attacker_mod /
						(defender_fort * (state.defines.base_military_tactics + state.world.nation_get_modifier_values(tech_def_nation, sys::national_mod_offsets::military_tactics)));
				auto org_damage =
						(att_stats.attack_or_gun_power * 0.1f + 1.0f) * att_stats.support * attacker_mod /
						(defender_fort * defender_org_bonus *
								(1.0f + state.world.nation_get_modifier_values(tech_def_nation, sys::national_mod_offsets::land_organisation)));

				auto& cstr = state.world.regiment_get_strength(def_front[i]);
				str_damage = std::min(str_damage, cstr);
				state.world.regiment_get_pending_damage(def_front[i]) += str_damage;
				cstr -= str_damage;
				auto& org = state.world.regiment_get_org(def_front[i]);
				org = std::max(0.0f, org - org_damage);
				switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(def_front[i])].type) {
					case unit_type::infantry:
						state.world.land_battle_get_defender_infantry_lost(b) += str_damage;
						break;
					case unit_type::cavalry:
						state.world.land_battle_get_defender_cav_lost(b) += str_damage;
						break;
					case unit_type::support:
						// fallthrough
					case unit_type::special:
						state.world.land_battle_get_defender_support_lost(b) += str_damage;
						break;
				}
			}

			if(def_back[i] && att_front[i]) {
				auto tech_def_nation = tech_nation_for_regiment(state, def_back[i]);
				auto tech_att_nation = tech_nation_for_regiment(state, att_front[i]);

				auto& def_stats = state.world.nation_get_unit_stats(tech_def_nation, state.world.regiment_get_type(def_back[i]));
				auto& att_stats = state.world.nation_get_unit_stats(tech_att_nation, state.world.regiment_get_type(att_front[i]));

				auto str_damage = (def_stats.attack_or_gun_power * 0.1f + 1.0f) * def_stats.support * defender_mod / ((state.defines.base_military_tactics + state.world.nation_get_modifier_values(tech_att_nation, sys::national_mod_offsets::military_tactics)));
				auto org_damage = (def_stats.attack_or_gun_power * 0.1f + 1.0f) * def_stats.support * defender_mod / (attacker_org_bonus * (1.0f + state.world.nation_get_modifier_values(tech_att_nation, sys::national_mod_offsets::land_organisation)));

				auto& cstr = state.world.regiment_get_strength(att_front[i]);
				str_damage = std::min(str_damage, cstr);
				state.world.regiment_get_pending_damage(att_front[i]) += str_damage;
				cstr -= str_damage;
				auto& org = state.world.regiment_get_org(att_front[i]);
				org = std::max(0.0f, org - org_damage);
				switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(att_front[i])].type) {
					case unit_type::infantry:
						state.world.land_battle_get_attacker_infantry_lost(b) += str_damage;
						break;
					case unit_type::cavalry:
						state.world.land_battle_get_attacker_cav_lost(b) += str_damage;
						break;
					case unit_type::support:
						// fallthrough
					case unit_type::special:
						state.world.land_battle_get_attacker_support_lost(b) += str_damage;
						break;
				}
			}

			if(att_front[i]) {
				auto tech_att_nation = tech_nation_for_regiment(state, att_front[i]);
				auto& att_stats = state.world.nation_get_unit_stats(tech_att_nation, state.world.regiment_get_type(att_front[i]));

				auto att_front_target = def_front[i];
				if(auto mv = state.military_definitions.unit_base_definitions[state.world.regiment_get_type(att_front[i])].maneuver_or_evasion; !att_front_target && mv > 0.0f) {
					for(int32_t cnt = 1; i - cnt * 2 >= 0 && cnt <= int32_t(mv); ++cnt) {
						if(def_front[i - cnt * 2]) {
							att_front_target = def_front[i - cnt * 2];
							break;
						}
					}
				}

				if(att_front_target) {
					auto tech_def_nation = tech_nation_for_regiment(state, att_front_target);
					auto& def_stats = state.world.nation_get_unit_stats(tech_def_nation, state.world.regiment_get_type(att_front_target));

					auto str_damage =
							(att_stats.attack_or_gun_power * 0.1f + 1.0f) * attacker_mod /
							(defender_fort * (state.defines.base_military_tactics + state.world.nation_get_modifier_values(tech_def_nation, sys::national_mod_offsets::military_tactics)));
					auto org_damage =
							(att_stats.attack_or_gun_power * 0.1f + 1.0f) * attacker_mod /
							(defender_fort * defender_org_bonus * (1.0f + state.world.nation_get_modifier_values(tech_def_nation, sys::national_mod_offsets::land_organisation)));

					auto& cstr = state.world.regiment_get_strength(att_front_target);
					str_damage = std::min(str_damage, cstr);
					state.world.regiment_get_pending_damage(att_front_target) += str_damage;
					cstr -= str_damage;
					auto& org = state.world.regiment_get_org(att_front_target);
					org = std::max(0.0f, org - org_damage);
					switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(att_front_target)].type) {
						case unit_type::infantry:
							state.world.land_battle_get_defender_infantry_lost(b) += str_damage;
							break;
						case unit_type::cavalry:
							state.world.land_battle_get_defender_cav_lost(b) += str_damage;
							break;
						case unit_type::support:
							// fallthrough
						case unit_type::special:
							state.world.land_battle_get_defender_support_lost(b) += str_damage;
							break;
					}
				}
			}

			if(def_front[i] && att_front[i]) {
				auto tech_def_nation = tech_nation_for_regiment(state, def_front[i]);
				auto& def_stats = state.world.nation_get_unit_stats(tech_def_nation, state.world.regiment_get_type(def_front[i]));

				auto def_front_target = att_front[i];

				if(auto mv = state.military_definitions.unit_base_definitions[state.world.regiment_get_type(def_front[i])].maneuver_or_evasion; !def_front_target && mv > 0.0f) {
					for(int32_t cnt = 1; i - cnt * 2 >= 0 && cnt <= int32_t(mv); ++cnt) {
						if(att_front[i - cnt * 2]) {
							def_front_target = att_front[i - cnt * 2];
							break;
						}
					}
				}

				if(def_front_target) {
					auto tech_att_nation = tech_nation_for_regiment(state, def_front_target);
					auto& att_stats = state.world.nation_get_unit_stats(tech_att_nation, state.world.regiment_get_type(def_front_target));

					auto str_damage = (def_stats.attack_or_gun_power * 0.1f + 1.0f) * defender_mod / ((state.defines.base_military_tactics + state.world.nation_get_modifier_values(tech_att_nation, sys::national_mod_offsets::military_tactics)));
					auto org_damage = (def_stats.attack_or_gun_power * 0.1f + 1.0f) * defender_mod / (attacker_org_bonus * (1.0f + state.world.nation_get_modifier_values(tech_att_nation, sys::national_mod_offsets::land_organisation)));

					auto& cstr = state.world.regiment_get_strength(def_front_target);
					str_damage = std::min(str_damage, cstr);
					state.world.regiment_get_pending_damage(def_front_target) += str_damage;
					cstr -= str_damage;
					auto& org = state.world.regiment_get_org(def_front_target);
					org = std::max(0.0f, org - org_damage);
					switch(state.military_definitions.unit_base_definitions[state.world.regiment_get_type(def_front_target)].type) {
						case unit_type::infantry:
							state.world.land_battle_get_attacker_infantry_lost(b) += str_damage;
							break;
						case unit_type::cavalry:
							state.world.land_battle_get_attacker_cav_lost(b) += str_damage;
							break;
						case unit_type::support:
							// fallthrough
						case unit_type::special:
							state.world.land_battle_get_attacker_support_lost(b) += str_damage;
							break;
					}
				}
			}
		}

		// clear dead / retreated regiments out

		for(int32_t i = 0; i < combat_width; ++i) {
			if(def_back[i]) {
				if(state.world.regiment_get_strength(def_back[i]) <= 0.0f) {
					auto type = state.military_definitions.unit_base_definitions[state.world.regiment_get_type(def_back[i])].type;
					switch(type) {
					case unit_type::infantry:
						state.world.land_battle_get_defender_infantry_lost(b)++;
						break;
					case unit_type::cavalry:
						state.world.land_battle_get_defender_cav_lost(b)++;
						break;
					default:
						state.world.land_battle_get_defender_support_lost(b)++;
						break;
					}
					def_back[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(def_back[i]) < 0.1f) {
					def_back[i] = dcon::regiment_id{};
				}
			}
			if(def_front[i]) {
				if(state.world.regiment_get_strength(def_front[i]) <= 0.0f) {
					auto type = state.military_definitions.unit_base_definitions[state.world.regiment_get_type(def_front[i])].type;
					switch(type) {
					case unit_type::infantry:
						state.world.land_battle_get_defender_infantry_lost(b)++;
						break;
					case unit_type::cavalry:
						state.world.land_battle_get_defender_cav_lost(b)++;
						break;
					default:
						state.world.land_battle_get_defender_support_lost(b)++;
						break;
					}
					def_front[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(def_front[i]) < 0.1f) {
					def_front[i] = dcon::regiment_id{};
				}
			}
			if(att_back[i]) {
				if(state.world.regiment_get_strength(att_back[i]) <= 0.0f) {
					auto type = state.military_definitions.unit_base_definitions[state.world.regiment_get_type(att_back[i])].type;
					switch(type) {
					case unit_type::infantry:
						state.world.land_battle_get_attacker_infantry_lost(b)++;
						break;
					case unit_type::cavalry:
						state.world.land_battle_get_attacker_cav_lost(b)++;
						break;
					default:
						state.world.land_battle_get_attacker_support_lost(b)++;
						break;
					}
					att_back[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(att_back[i]) < 0.1f) {
					att_back[i] = dcon::regiment_id{};
				}
			}
			if(att_front[i]) {
				if(state.world.regiment_get_strength(att_front[i]) <= 0.0f) {
					auto type = state.military_definitions.unit_base_definitions[state.world.regiment_get_type(att_front[i])].type;
					switch(type) {
					case unit_type::infantry:
						state.world.land_battle_get_attacker_infantry_lost(b)++;
						break;
					case unit_type::cavalry:
						state.world.land_battle_get_attacker_cav_lost(b)++;
						break;
					default:
						state.world.land_battle_get_attacker_support_lost(b)++;
						break;
					}
					att_front[i] = dcon::regiment_id{};
				} else if(state.world.regiment_get_org(att_front[i]) < 0.1f) {
					att_front[i] = dcon::regiment_id{};
				}
			}
		}



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

		// back row

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
				while(low < 30 && a[low])
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
				while(low < 30 && a[low])
					low += 2;
			}
			};

		compact(att_back);
		compact(att_front);
		compact(def_back);
		compact(def_front);

		for(int32_t i = 0; i < combat_width; ++i) {
			if(!att_back[i]) {
				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::is_attacking | reserve_regiment::type_support)) {
						att_back[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}
			}

			if(!def_back[i]) {
				for(uint32_t j = reserves.size(); j-- > 0;) {
					if(reserves[j].flags == (reserve_regiment::type_support)) {
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
					if(reserves[j].flags == (reserve_regiment::is_attacking | reserve_regiment::type_infantry)) {
						att_front[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}

				if(!att_front[i]) {
					for(uint32_t j = reserves.size(); j-- > 0;) {
						if(reserves[j].flags == (reserve_regiment::is_attacking | reserve_regiment::type_cavalry)) {
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
					if(reserves[j].flags == (reserve_regiment::type_infantry)) {
						def_front[i] = reserves[j].regiment;
						std::swap(reserves[j], reserves[reserves.size() - 1]);
						reserves.pop_back();
						break;
					}
				}

				if(!def_front[i]) {
					for(uint32_t j = reserves.size(); j-- > 0;) {
						if(reserves[j].flags == (reserve_regiment::type_cavalry)) {
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

	for(auto b : state.world.in_land_battle) {
		if(to_delete.get(b) != 0) {
 			end_battle(state, b, to_delete.get(b) == uint8_t(1) ? battle_result::attacker_won : battle_result::defender_won);
		}
	}
}

void update_naval_battles(sys::state& state) {
	auto to_delete = ve::vectorizable_buffer<uint8_t, dcon::naval_battle_id>(state.world.naval_battle_size());

	concurrency::parallel_for(0, int32_t(state.world.naval_battle_size()), [&](int32_t index) {
		dcon::naval_battle_id b{dcon::naval_battle_id::value_base_t(index)};

		if(!state.world.naval_battle_is_valid(b))
			return;

		int32_t attacker_ships = 0;
		int32_t defender_ships = 0;

		auto slots = state.world.naval_battle_get_slots(b);

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

		auto attacker_per = state.world.leader_get_personality(state.world.naval_battle_get_admiral_from_attacking_admiral(b));
		auto attacker_bg = state.world.leader_get_background(state.world.naval_battle_get_admiral_from_attacking_admiral(b));

		auto attack_bonus =
				int32_t(state.world.leader_trait_get_attack(attacker_per) + state.world.leader_trait_get_attack(attacker_bg));
		auto attacker_org_bonus =
				1.0f + state.world.leader_trait_get_organisation(attacker_per) + state.world.leader_trait_get_organisation(attacker_bg);

		auto defender_per = state.world.leader_get_personality(state.world.naval_battle_get_admiral_from_attacking_admiral(b));
		auto defender_bg = state.world.leader_get_background(state.world.naval_battle_get_admiral_from_attacking_admiral(b));

		auto defence_bonus =
				int32_t(state.world.leader_trait_get_defense(defender_per) + state.world.leader_trait_get_defense(defender_bg));
		auto defender_org_bonus =
				1.0f + state.world.leader_trait_get_organisation(defender_per) + state.world.leader_trait_get_organisation(defender_bg);

		auto attacker_mod = combat_modifier_table[std::clamp(attacker_dice + attack_bonus + 3, 0, 19)];
		auto defender_mod = combat_modifier_table[std::clamp(defender_dice + defence_bonus + 3, 0, 19)];

		for(uint32_t j = slots.size(); j-- > 0;) {
			auto ship_owner =
					state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(slots[j].ship));
			auto& ship_stats = state.world.nation_get_unit_stats(ship_owner, state.world.ship_get_type(slots[j].ship));

			switch(slots[j].flags & ship_in_battle::mode_mask) {
			case ship_in_battle::mode_approaching: {
				auto target_mode = slots[slots[j].target_slot].flags & ship_in_battle::mode_mask;
				if(target_mode == ship_in_battle::mode_retreated || target_mode == ship_in_battle::mode_sunk) {
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_seeking;
					break;
				}

				/*
				An approaching ship:
				Has its distance reduced by (random-value-in-range-\[0.0 - 0.5) + 0.5) x max-speed x
				define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR * 1000 to a minimum of 0. Switches to engaged when its distance + the
				target's distance is less than its fire range
				*/

				float speed = ship_stats.maximum_speed * 1000.0f * state.defines.naval_combat_speed_to_distance_factor *
											(0.5f + float(rng::get_random(state, uint32_t(slots[j].ship.value)) & 0x7FFF) / float(0xFFFF));
				auto old_distance = slots[j].flags & ship_in_battle::distance_mask;
				int32_t adjust = std::clamp(int32_t(std::ceil(speed)), 0, old_distance);
				slots[j].flags &= ~ship_in_battle::distance_mask;
				slots[j].flags |= ship_in_battle::distance_mask & (old_distance - adjust);

				if(old_distance == adjust ||
						(old_distance - adjust) + (slots[slots[j].target_slot].flags & ship_in_battle::distance_mask) <
								int32_t(1000.0f * ship_stats.reconnaissance_or_fire_range)) {

					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_engaged;
				}

				break;
			}
			case ship_in_battle::mode_engaged: {
				auto target_mode = slots[slots[j].target_slot].flags & ship_in_battle::mode_mask;
				if(target_mode == ship_in_battle::mode_retreated || target_mode == ship_in_battle::mode_sunk) {
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_seeking;
					break;
				}
				bool target_is_big = (slots[slots[j].target_slot].flags & ship_in_battle::type_mask) == ship_in_battle::type_big;
				bool is_attacker = (slots[j].flags & ship_in_battle::is_attacking) != 0;
				auto tship = slots[slots[j].target_slot].ship;
				assert(tship);

				auto ship_target_owner =
						state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(tship));
				auto& ship_target_stats = state.world.nation_get_unit_stats(ship_target_owner, state.world.ship_get_type(tship));

				/*
				Torpedo attack: is treated as 0 except against big ships
				Damage to organization is (gun-power + torpedo-attack) x Modifier-Table\[modifiers + 2\] (see above) x target-strength x
				define:NAVAL_COMBAT_DAMAGE_ORG_MULT / (target-max-hull x target-experience x 0.1 + 1) Damage to strength is (gun-power +
				torpedo-attack) x Modifier-Table\[modifiers + 2\] (see above) x attacker-strength x define:NAVAL_COMBAT_DAMAGE_STR_MULT x
				define:NAVAL_COMBAT_DAMAGE_MULT_NO_ORG (if target has no org) / (target-max-hull x target-experience x 0.1 + 1)
				*/

				float org_damage = (ship_stats.attack_or_gun_power + (target_is_big ? ship_stats.siege_or_torpedo_attack : 0.0f)) *
													 (is_attacker ? attacker_mod : defender_mod) * state.defines.naval_combat_damage_org_mult /
													 ((ship_target_stats.defence_or_hull + 1.0f) * (is_attacker ? defender_org_bonus : attacker_org_bonus) *
															 (1.0f + state.world.nation_get_modifier_values(ship_target_owner,
																					 sys::national_mod_offsets::naval_organisation)));
				float str_damage = (ship_stats.attack_or_gun_power + (target_is_big ? ship_stats.siege_or_torpedo_attack : 0.0f)) *
													 (is_attacker ? attacker_mod : defender_mod) * state.defines.naval_combat_damage_str_mult /
													 (ship_target_stats.defence_or_hull + 1.0f);

				auto& torg = state.world.ship_get_org(tship);
				torg = std::max(0.0f, torg - org_damage);
				auto& tstr = state.world.ship_get_strength(tship);
				tstr = std::max(0.0f, tstr - str_damage);

				break;
			}
			case ship_in_battle::mode_retreating: {
				/*
				A retreating ship will increase its distance by define:NAVAL_COMBAT_RETREAT_SPEED_MOD x
				define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR x (random value in the range \[0.0 - 0.5) + 0.5) x ship-max-speed.
				*/

				float speed = ship_stats.maximum_speed * 1000.0f * state.defines.naval_combat_retreat_speed_mod *
											state.defines.naval_combat_speed_to_distance_factor *
											(0.5f + float(rng::get_random(state, uint32_t(slots[j].ship.value)) & 0x7FFF) / float(0xFFFF));

				auto old_distance = slots[j].flags & ship_in_battle::distance_mask;
				int32_t new_distance = std::min(int32_t(std::ceil(speed)) + old_distance, 1000);
				slots[j].flags &= ~ship_in_battle::distance_mask;
				slots[j].flags |= ship_in_battle::distance_mask & (new_distance);

				break;
			}
			case ship_in_battle::mode_seeking: {
				/*
				When a target is selected, distance is increased by random-value-in-range-\[0.0, 1.0) x (1.0 -
				combat-duration^define:NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) / NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) x
				NAVAL_COMBAT_SHIFT_BACK_ON_NEXT_TARGET to a maximum of 1000, and the ship switches to approaching.
				*/

				if((slots[j].flags & ship_in_battle::is_attacking) != 0) {
					auto pick = rng::get_random(state, uint32_t(slots[j].ship.value)) % defender_ships;

					[&]() {
						for(uint32_t k = slots.size(); k-- > 0;) {
							switch(slots[k].flags & ship_in_battle::mode_mask) {

							case ship_in_battle::mode_seeking:
							case ship_in_battle::mode_approaching:
							case ship_in_battle::mode_retreating:
							case ship_in_battle::mode_engaged:
								if((slots[k].flags & ship_in_battle::is_attacking) == 0) {
									if(pick == 0) {
										slots[j].target_slot = uint16_t(k);
										return;
									} else {
										--pick;
									}
								}
								break;
							default:
								break;
							}
						}
					}();

					auto old_distance = slots[j].flags & ship_in_battle::distance_mask;
					int32_t new_distance = std::min(old_distance + 400, 1000);

					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_approaching;
					slots[j].flags &= ~ship_in_battle::distance_mask;
					slots[j].flags |= ship_in_battle::distance_mask & new_distance;
				} else {
					auto pick = rng::get_random(state, uint32_t(slots[j].ship.value)) % attacker_ships;

					[&]() {
						for(uint32_t k = slots.size(); k-- > 0;) {
							switch(slots[k].flags & ship_in_battle::mode_mask) {

							case ship_in_battle::mode_seeking:
							case ship_in_battle::mode_approaching:
							case ship_in_battle::mode_retreating:
							case ship_in_battle::mode_engaged:
								if((slots[k].flags & ship_in_battle::is_attacking) != 0) {
									if(pick == 0) {
										slots[j].target_slot = uint16_t(k);
										return;
									} else {
										--pick;
									}
								}
								break;
							default:
								break;
							}
						}
					}();

					auto old_distance = slots[j].flags & ship_in_battle::distance_mask;
					int32_t new_distance = std::min(old_distance + 400, 1000);

					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_approaching;
					slots[j].flags &= ~ship_in_battle::distance_mask;
					slots[j].flags |= ship_in_battle::distance_mask & new_distance;
				}

				break;
			}
			default:
				break;
			}
		}

		for(uint32_t j = slots.size(); j-- > 0;) { // test health, retreat and/or sink
			auto ship_owner =
					state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(slots[j].ship));
			auto type = state.world.ship_get_type(slots[j].ship);

			switch(slots[j].flags & ship_in_battle::mode_mask) {
			case ship_in_battle::mode_seeking:
			case ship_in_battle::mode_approaching:
			case ship_in_battle::mode_engaged:
				if(state.world.ship_get_strength(slots[j].ship) <= 0) {
					if((slots[j].flags & ship_in_battle::is_attacking) != 0) {
						if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_big) {
							state.world.naval_battle_get_attacker_big_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_small) {
							state.world.naval_battle_get_attacker_small_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_transport) {
							state.world.naval_battle_get_attacker_transport_ships_lost(b)++;
						}
						state.world.naval_battle_get_attacker_loss_value(b) += state.military_definitions.unit_base_definitions[type].supply_consumption_score;
					} else {
						if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_big) {
							state.world.naval_battle_get_defender_big_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_small) {
							state.world.naval_battle_get_defender_small_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_transport) {
							state.world.naval_battle_get_defender_transport_ships_lost(b)++;
						}
						state.world.naval_battle_get_defender_loss_value(b) += state.military_definitions.unit_base_definitions[type].supply_consumption_score;
					}
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_sunk;
					break;
				}
				if(state.world.ship_get_strength(slots[j].ship) <= state.defines.naval_combat_retreat_str_org_level ||
						state.world.ship_get_org(slots[j].ship) <= state.defines.naval_combat_retreat_str_org_level) {

					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_retreating;
				}
				break;
			case ship_in_battle::mode_retreating:
				if(state.world.ship_get_strength(slots[j].ship) <= 0) {
					if((slots[j].flags & ship_in_battle::is_attacking) != 0) {
						if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_big) {
							state.world.naval_battle_get_attacker_big_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_small) {
							state.world.naval_battle_get_attacker_small_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_transport) {
							state.world.naval_battle_get_attacker_transport_ships_lost(b)++;
						}
					} else {
						if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_big) {
							state.world.naval_battle_get_defender_big_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_small) {
							state.world.naval_battle_get_defender_small_ships_lost(b)++;
						} else if((slots[j].flags & ship_in_battle::type_mask) == ship_in_battle::type_transport) {
							state.world.naval_battle_get_defender_transport_ships_lost(b)++;
						}
					}
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_sunk;
					break;
				}
				if((slots[j].flags & ship_in_battle::distance_mask) >= 1000) {
					slots[j].flags &= ~ship_in_battle::mode_mask;
					slots[j].flags |= ship_in_battle::mode_retreated;
				}
				break;
			default:
				break;
			}
		}
	});

	for(auto b : state.world.in_naval_battle) {
		if(to_delete.get(b) != 0) {
			end_battle(state, b, to_delete.get(b) == uint8_t(1) ? battle_result::attacker_won : battle_result::defender_won);
		}
	}
	for(uint32_t i = state.world.ship_size(); i-- > 0;) {
		dcon::ship_id s{dcon::ship_id::value_base_t(i)};
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
	state.world.navy_set_location_from_navy_location(n, p);
	if(!state.world.navy_get_is_retreating(n)) {
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

		// TODO: notify on new battle
	}
}

void update_movement(sys::state& state) {
	for(auto a : state.world.in_army) {
		if(auto path = a.get_path(); a.get_arrival_time() == state.current_date && path.size() > 0) {
			auto dest = path.at(path.size() - 1);
			path.pop_back();
			auto from = state.world.army_get_location_from_army_location(a);

			if(dest.index() >= state.province_definitions.first_sea_province.index()) { // sea province
				// check for embarkation possibility, then embark
				auto to_navy = find_embark_target(state, a.get_controller_from_army_control(), dest, a);
				if(to_navy) {
					a.set_location_from_army_location(dest);
					a.set_navy_from_army_transport(to_navy);
					a.set_black_flag(false);
				} else {
					path.clear();
				}
			} else { // land province
				if(a.get_black_flag()) {
					if(province::has_access_to_province(state, a.get_controller_from_army_control(), dest)) {
						a.set_black_flag(false);
					}
					army_arrives_in_province(state, a, dest,
							(state.world.province_adjacency_get_type(state.world.get_province_adjacency_by_province_pair(dest, from)) &
									province::border::river_crossing_bit) != 0
									? military::crossing_type::river
									: military::crossing_type::none, dcon::land_battle_id{});
					a.set_navy_from_army_transport(dcon::navy_id{});
				} else if(province::has_access_to_province(state, a.get_controller_from_army_control(), dest)) {
					if(auto n = a.get_navy_from_army_transport()) {
						if(!n.get_battle_from_navy_battle_participation()) {
							army_arrives_in_province(state, a, dest, military::crossing_type::sea, dcon::land_battle_id{});
							a.set_navy_from_army_transport(dcon::navy_id{});
						} else {
							path.clear();
						}
					} else {
						army_arrives_in_province(state, a, dest,
								(state.world.province_adjacency_get_type(state.world.get_province_adjacency_by_province_pair(dest, from)) &
										province::border::river_crossing_bit) != 0
										? military::crossing_type::river
										: military::crossing_type::none, dcon::land_battle_id{});
					}
				} else {
					path.clear();
				}
			}

			if(path.size() > 0) {
				auto next_dest = path.at(path.size() - 1);
				a.set_arrival_time(arrival_time_to(state, a, next_dest));
			} else {
				a.set_arrival_time(sys::date{});
				if(a.get_is_retreating()) {
					a.set_is_retreating(false);
					army_arrives_in_province(state, a, dest,
							(state.world.province_adjacency_get_type(state.world.get_province_adjacency_by_province_pair(dest, from)) &
									province::border::river_crossing_bit) != 0
									? military::crossing_type::river
									: military::crossing_type::none, dcon::land_battle_id{});
				}
			}
		}
	}

	for(auto n : state.world.in_navy) {
		if(auto path = n.get_path(); n.get_arrival_time() == state.current_date && path.size() > 0) {
			auto dest = path.at(path.size() - 1);
			path.pop_back();

			if(dest.index() < state.province_definitions.first_sea_province.index()) { // land province
				if(province::has_access_to_province(state, n.get_controller_from_navy_control(), dest)) {

					n.set_location_from_navy_location(dest);

					// check for whether there are troops to disembark
					auto attached = state.world.navy_get_army_transport(n);
					while(attached.begin() != attached.end()) {
						auto a = (*attached.begin()).get_army();

						a.set_navy_from_army_transport(dcon::navy_id{});
						a.get_path().clear();
						a.set_arrival_time(sys::date{});

						army_arrives_in_province(state, a, dest, military::crossing_type::none, dcon::land_battle_id{});
					}
				} else {
					path.clear();
				}
			} else { // sea province

				navy_arrives_in_province(state, n, dest, dcon::naval_battle_id{});

				// take embarked units along with
				for(auto a : state.world.navy_get_army_transport(n)) {
					a.get_army().set_location_from_army_location(dest);
					a.get_army().get_path().clear();
					a.get_army().set_arrival_time(sys::date{});
				}
			}

			if(path.size() > 0) {
				auto next_dest = path.at(path.size() - 1);
				n.set_arrival_time(arrival_time_to(state, n, next_dest));
			} else {
				n.set_arrival_time(sys::date{});
				if(n.get_is_retreating()) {
					if(dest.index() >= state.province_definitions.first_sea_province.index())
						navy_arrives_in_province(state, n, dest, dcon::naval_battle_id{});
					n.set_is_retreating(false);
				}
			}
		}
	}
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

void update_siege_progress(sys::state& state) {
	concurrency::parallel_for(0, state.province_definitions.first_sea_province.index(), [&](int32_t id) {
		dcon::province_id prov{dcon::province_id::value_base_t(id)};

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
				bool will_siege = false;

				auto army_controller = ar.get_army().get_controller_from_army_control();
				if(!army_controller) {					 // rebel army
					will_siege = bool(controller); // siege anything not rebel controlled
				} else {
					if(!controller) {
						will_siege = true; // siege anything rebel controlled
					} else if(are_at_war(state, controller, army_controller)) {
						will_siege = true;
					}
				}

				if(will_siege) {
					if(!first_army)
						first_army = ar.get_army();

					auto army_stats = army_controller
																? army_controller
																: ar.get_army().get_army_rebel_control().get_controller().get_ruler_from_rebellion_within();

					owner_involved = owner_involved || owner == army_controller;
					core_owner_involved =
							core_owner_involved || bool(state.world.get_core_by_prov_tag_key(prov,
																				 state.world.nation_get_identity_from_identity_holder(army_controller)));

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

			auto& progress = state.world.province_get_siege_progress(prov);
			progress = std::max(progress, progress - 0.1f);
		} else {
			/*
			We find the effective level of the fort by subtracting: (rounding this value down to to the nearest integer)
			greatest-siege-value-present x
			((the ratio of the strength of regiments with siege present to the total strength of all regiments) ^
			define:ENGINEER_UNIT_RATIO) / define:ENGINEER_UNIT_RATIO, reducing it to a minimum of 0.
			*/

			int32_t effective_fort_level =
					std::clamp(state.world.province_get_building_level(prov, economy::province_building_type::fort) -
												 int32_t(max_siege_value *
																 std::min(strength_siege_units / total_sieging_strength, state.defines.engineer_unit_ratio) /
																 state.defines.engineer_unit_ratio),
							0, 9);

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
			sieging it back) x (1.1 if the sieger is not the owner but does have a core) / Siege-Table\[effective-fort-level\]
			*/

			static constexpr float siege_table[] = {1.0f, 2.0f, 2.8f, 3.4f, 3.8f, 4.2f, 4.5f, 4.8f, 5.0f, 5.2f};
			static constexpr float progress_table[] = {0.0f, 0.2f, 0.5f, 0.75f, 0.75f, 1, 1.1f, 1.1f, 1.25f, 1.25f};

			float added_progress = siege_speed_modifier * num_brigades_modifier *
														 progress_table[rng::get_random(state, uint32_t(prov.value)) % 10] *
														 (owner_involved ? 1.25f : (core_owner_involved ? 1.1f : 1.0f)) / siege_table[effective_fort_level];

			auto& progress = state.world.province_get_siege_progress(prov);
			progress += added_progress;

			if(progress >= 1.0f) {
				progress = 0.0f;

				/*
				The garrison returns to 100% immediately after the siege is complete and the controller changes. If your siege returns a
				province to its owner's control without the owner participating, you get +2.5 relations with the owner.
				*/

				state.world.province_set_former_controller(prov, controller);
				state.world.province_set_former_rebel_controller(prov, state.world.province_get_rebel_faction_from_province_rebel_control(prov));

				auto new_controller = state.world.army_get_controller_from_army_control(first_army);
				auto rebel_controller = state.world.army_get_controller_from_army_rebel_control(first_army);
				assert(bool(new_controller) != bool(rebel_controller));
				if(!new_controller || are_at_war(state, new_controller, owner)) {
					state.world.province_set_nation_from_province_control(prov, new_controller);
					state.world.province_set_rebel_faction_from_province_rebel_control(prov, rebel_controller);
				} else {
					state.world.province_set_nation_from_province_control(prov, owner);
					state.world.province_set_rebel_faction_from_province_rebel_control(prov, dcon::rebel_faction_id{});
				}

				state.world.province_set_last_control_change(prov, state.current_date);
				update_blackflag_status(state, prov);
			}
		}
	});

	province::for_each_land_province(state, [&](dcon::province_id prov) {
		if(state.world.province_get_last_control_change(prov) == state.current_date) {
			eject_ships(state, prov);

			auto cc = state.world.province_get_nation_from_province_control(prov);
			auto oc = state.world.province_get_former_controller(prov);

			if(cc) {
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
					cc,
					sys::message_setting_type::siegeover_by_nation
				});
			}
			if(oc) {
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
					oc,
					sys::message_setting_type::siegeover_on_nation
				});
			}

			/*
			TODO: When a province controller changes as the result of a siege, and it does not go back to the owner a random,
			`on_siege_win` event is fired, subject to the conditions of the events being met.
			*/
			// is controler != owner ...
			// event::fire_fixed_event(state, );
		}
	});
}

void update_blackflag_status(sys::state& state, dcon::province_id p) {
	for(auto ar : state.world.province_get_army_location(p)) {
		ar.get_army().set_black_flag(!province::has_access_to_province(state, ar.get_army().get_controller_from_army_control(), p));
	}
}

void eject_ships(sys::state& state, dcon::province_id p) {
	if(!state.world.province_get_is_coast(p))
		return;

	dcon::province_id sea_zone;
	for(auto a : state.world.province_get_province_adjacency(p)) {
		auto other = a.get_connected_provinces(0) == p ? a.get_connected_provinces(1) : a.get_connected_provinces(0);
		if(other.id.index() >= state.province_definitions.first_sea_province.index()) {
			sea_zone = other.id;
			break;
		}
	}
	assert(sea_zone);

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
			a.get_army().get_path().clear();
			a.get_army().set_arrival_time(sys::date{});
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
				++current_dig_in;
			}
		}
	}
}

void recover_org(sys::state& state) {
	/*
	Units that are not in combat and not embarked recover organization daily at: (national-organization-regeneration-modifier +
	morale-from-tech + leader-morale-trait + 1) x the-unit's-supply-factor / 5 up to the maximum organization possible for the unit
	times (0.25 + 0.75 x effective land or naval spending).
	*/
	for(auto ar : state.world.in_army) {
		if(ar.get_army_battle_participation().get_battle() || ar.get_navy_from_army_transport())
			continue;

		auto in_nation = ar.get_controller_from_army_control();
		auto tech_nation = in_nation ? in_nation : ar.get_controller_from_army_rebel_control().get_ruler_from_rebellion_within();

		auto leader = ar.get_general_from_army_leadership();
		auto regen_mod = tech_nation.get_modifier_values(sys::national_mod_offsets::org_regain) +
										 leader.get_personality().get_morale() + leader.get_background().get_morale() + 1.0f;
		auto spending_level = (in_nation ? in_nation.get_effective_land_spending() : 1.0f);
		auto modified_regen = regen_mod * spending_level / 150.0f;

		for(auto reg : ar.get_army_membership()) {
			reg.get_regiment().set_org(std::min(reg.get_regiment().get_org() + modified_regen, 0.25f + 0.75f * spending_level));
		}
	}

	for(auto ar : state.world.in_navy) {
		if(ar.get_navy_battle_participation().get_battle())
			continue;

		auto in_nation = ar.get_controller_from_navy_control();

		auto leader = ar.get_admiral_from_navy_leadership();
		auto regen_mod = in_nation.get_modifier_values(sys::national_mod_offsets::org_regain) +
										 leader.get_personality().get_morale() + leader.get_background().get_morale() + 1.0f;
		float oversize_amount =
				in_nation.get_naval_supply_points() > 0
						? std::min(float(in_nation.get_used_naval_supply_points()) / float(in_nation.get_naval_supply_points()), 1.75f)
						: 1.75f;
		float over_size_penalty = oversize_amount > 1.0f ? 2.0f - oversize_amount : 1.0f;
		auto spending_level = in_nation.get_effective_naval_spending() * over_size_penalty;
		auto modified_regen = regen_mod * spending_level / 150.0f;

		for(auto reg : ar.get_navy_membership()) {
			reg.get_ship().set_org(std::min(reg.get_ship().get_org() + modified_regen, 0.25f + 0.75f * spending_level));
		}
	}
}

void reinforce_regiments(sys::state& state) {
	/*
	A unit that is not retreating, not embarked, not in combat is reinforced (has its strength increased) by:
define:REINFORCE_SPEED x (technology-reinforcement-modifier + 1.0) x (2 if in owned province, 0.1 in an unowned port province, 1
in a controlled province, 0.5 if in a province adjacent to a province with military access, 0.25 in a hostile, unblockaded port,
and 0.1 in any other hostile province) x (national-reinforce-speed-modifier + 1) x army-supplies x (number of actual regiments /
max possible regiments (feels like a bug to me) or 0.5 if mobilized)
	*/

	for(auto ar : state.world.in_army) {
		if(ar.get_battle_from_army_battle_participation() || ar.get_navy_from_army_transport() || ar.get_is_retreating())
			continue;

		auto in_nation = ar.get_controller_from_army_control();
		auto tech_nation = in_nation ? in_nation : ar.get_controller_from_army_rebel_control().get_ruler_from_rebellion_within();

		auto spending_level = (in_nation ? in_nation.get_effective_land_spending() : 1.0f);

		float location_modifier = 1.0f;
		if(ar.get_location_from_army_location().get_nation_from_province_ownership() == in_nation) {
			location_modifier = 2.0f;
		} else if(ar.get_location_from_army_location().get_nation_from_province_control() == in_nation) {
			location_modifier = 1.0f;
		} else {
			location_modifier = 0.1f;
		}

		auto combined = state.defines.reinforce_speed * spending_level * location_modifier *
										(1.0f + tech_nation.get_modifier_values(sys::national_mod_offsets::reinforce_speed)) *
										(1.0f + tech_nation.get_modifier_values(sys::national_mod_offsets::reinforce_rate));

		for(auto reg : ar.get_army_membership()) {
			auto pop = reg.get_regiment().get_pop_from_regiment_source();
			auto pop_size = pop.get_size();
			auto limit_fraction = std::min(1.0f, pop_size / state.defines.pop_size_per_regiment);
			reg.get_regiment().set_strength(std::min(reg.get_regiment().get_strength() + combined, limit_fraction));
		}
	}
}

void repair_ships(sys::state& state) {
	/*
	A ship that is docked at a naval base is repaired (has its strength increase) by:
maximum-strength x (technology-repair-rate + provincial-modifier-to-repair-rate + 1) x ship-supplies x
(national-reinforce-speed-modifier + 1) x navy-supplies
	*/
	for(auto n : state.world.in_navy) {
		auto nb_level = n.get_location_from_navy_location().get_building_level(economy::province_building_type::naval_base);
		if(!n.get_arrival_time() && nb_level > 0) {

			auto in_nation = n.get_controller_from_navy_control();

			float oversize_amount =
					in_nation.get_naval_supply_points() > 0
							? std::min(float(in_nation.get_used_naval_supply_points()) / float(in_nation.get_naval_supply_points()), 1.75f)
							: 1.75f;
			float over_size_penalty = oversize_amount > 1.0f ? 2.0f - oversize_amount : 1.0f;
			auto spending_level = in_nation.get_effective_naval_spending() * over_size_penalty;

			auto rr_mod = n.get_location_from_navy_location().get_modifier_values(sys::provincial_mod_offsets::local_repair) + 1.0f;
			auto reinf_mod = in_nation.get_modifier_values(sys::national_mod_offsets::reinforce_speed) + 1.0f;
			auto repair_val = rr_mod * reinf_mod * spending_level;

			for(auto reg : n.get_navy_membership()) {
				reg.get_ship().set_strength(std::min(reg.get_ship().get_strength() + repair_val, 1.0f));
			}
		}
	}
}

void start_mobilization(sys::state& state, dcon::nation_id n) {
	state.world.nation_set_is_mobilized(n, true);
	/*
	At most, national-mobilization-impact-modifier x (define:MIN_MOBILIZE_LIMIT v nation's-number-of-regiments regiments may be
	created by mobilization).
	*/
	auto real_regs = std::max(int32_t(state.world.nation_get_recruitable_regiments(n)), int32_t(state.defines.min_mobilize_limit));

	state.world.nation_set_mobilization_remaining(n,
			uint16_t(real_regs * state.world.nation_get_modifier_values(n, sys::national_mod_offsets::mobilization_impact)));

	auto schedule_array = state.world.nation_get_mobilization_schedule(n);
	schedule_array.clear();

	for(auto pr : state.world.nation_get_province_ownership(n)) {
		if(pr.get_province().get_is_colonial() == false) {
			schedule_array.push_back(mobilization_order{pr.get_province().id, sys::date{}});
		}
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
																					 (state.world.province_get_building_level(schedule_array[count].where, economy::province_building_type::railroad)) / 5.0f);
		auto days = std::max(1, int32_t(1.0f / province_speed));
		delay += days;
		schedule_array[count].when = state.current_date + delay;
	}

	/*
	Mobilizing increases crisis tension by define:CRISIS_TEMPERATURE_ON_MOBILIZE
	*/
	if(state.current_crisis_mode == sys::crisis_mode::heating_up) {
		for(auto& par : state.crisis_participants) {
			if(!par.id)
				break;
			if(par.id == n)
				state.crisis_temperature += state.defines.crisis_temperature_on_mobilize;
		}
	}
}
void end_mobilization(sys::state& state, dcon::nation_id n) {
	state.world.nation_set_is_mobilized(n, false);
	state.world.nation_set_mobilization_remaining(n, 0);
	auto schedule_array = state.world.nation_get_mobilization_schedule(n);
	schedule_array.clear();

	for(auto ar : state.world.nation_get_army_control(n)) {
		for(auto rg : ar.get_army().get_army_membership()) {
			auto pop = rg.get_regiment().get_pop_from_regiment_source();
			if(pop && pop.get_poptype() != state.culture_definitions.soldiers) {
				rg.get_regiment().set_strength(0.0f);
				rg.get_regiment().set_pop_from_regiment_source(dcon::pop_id{});
			}
		}
	}
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

					// mobilize the province

					if(state.world.province_get_nation_from_province_control(back.where) ==
							state.world.province_get_nation_from_province_ownership(back.where)) { // only if un occupied
						/*
						In those provinces, mobilized regiments come from non-soldier, non-slave, poor-strata pops with a culture that is
						either the primary culture of the nation or an accepted culture.
						*/
						for(auto pop : state.world.province_get_pop_location(back.where)) {

							if(pop.get_pop().get_poptype() != state.culture_definitions.soldiers &&
									pop.get_pop().get_poptype() != state.culture_definitions.slaves &&
									pop.get_pop().get_is_primary_or_accepted_culture() &&
									pop.get_pop().get_poptype().get_strata() == uint8_t(culture::pop_strata::poor)) {

								/*
								The number of regiments these pops can provide is determined by pop-size x mobilization-size /
								define:POP_SIZE_PER_REGIMENT.
								*/

								auto available =
										int32_t(pop.get_pop().get_size() * mobilization_size(state, n) / state.defines.pop_size_per_regiment);
								if(available > 0) {

									auto a = [&]() {
										for(auto ar : state.world.province_get_army_location(back.where)) {
											if(ar.get_army().get_controller_from_army_control() == n)
												return ar.get_army().id;
										}
										auto new_army = fatten(state.world, state.world.create_army());
										new_army.set_controller_from_army_control(n);
										military::army_arrives_in_province(state, new_army, back.where, military::crossing_type::none, dcon::land_battle_id{});
										return new_army.id;
									}();

									while(available > 0 && to_mobilize > 0) {
										auto new_reg = military::create_new_regiment(state, dcon::nation_id{}, state.military_definitions.infantry);
										state.world.try_create_army_membership(new_reg, a);
										state.world.try_create_regiment_source(new_reg, pop.get_pop());

										--available;
										--to_mobilize;
									}
								}
							}

							if(to_mobilize == 0)
								break;
						}
					}
				}
			} else {
				to_mobilize = 0;
			}
		}
	}
}

bool can_retreat_from_battle(sys::state& state, dcon::naval_battle_id battle) {
	return (state.world.naval_battle_get_start_date(battle) + days_before_retreat < state.current_date);
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


} // namespace military
