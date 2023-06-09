#include "military.hpp"
#include "dcon_generated.hpp"
#include "prng.hpp"
#include "effects.hpp"

namespace military {

void reset_unit_stats(sys::state& state) {
	for(uint32_t i = 0; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)};
		state.world.for_each_nation([&](dcon::nation_id nid) {
			state.world.nation_get_unit_stats(nid, uid) = state.military_definitions.unit_base_definitions[uid];
		});
	}
}

void apply_base_unit_stat_modifiers(sys::state& state) {
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size(); ++i) {
		dcon::unit_type_id uid = dcon::unit_type_id{dcon::unit_type_id::value_base_t(i)};
		auto base_id = state.military_definitions.unit_base_definitions[uid].is_land ? state.military_definitions.base_army_unit
																																								 : state.military_definitions.base_naval_unit;
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

bool cb_conditions_satisfied(sys::state& state, dcon::nation_id actor, dcon::nation_id target, dcon::cb_type_id cb) {
	auto can_use = state.world.cb_type_get_can_use(cb);
	if(can_use &&
			!trigger::evaluate(state, can_use, trigger::to_generic(target), trigger::to_generic(actor), trigger::to_generic(actor))) {
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

dcon::war_id find_war_between(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	for(auto wa : state.world.nation_get_war_participant(a)) {
		auto is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() == b) {
				if(o.get_is_attacker() != is_attacker)
					return wa.get_war().id;
				else
					return dcon::war_id{};
			}
		}
	}
	return dcon::war_id{};
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
		if(is_defender_wargoal(state, w, wg.get_wargoal()) &&
				(wg.get_wargoal().get_type().get_type_bits() & cb_flag::po_status_quo) != 0)
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
	float modifier = 2.0f;
	auto prov_controller = state.world.province_get_nation_from_province_control(p);
	auto self_controlled = prov_controller == n;
	if(state.world.province_get_nation_from_province_ownership(p) == n && self_controlled) {
		modifier = 2.5f;
	} else if(self_controlled ||
						bool(state.world.province_get_rebel_faction_from_province_rebel_control(p))) { // TODO: check for sieging
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
					nb_level = std::max(nb_level, int32_t(p.get_province().get_naval_base_level()));
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
					dcon::cb_type_id cbt = n.get_constructing_cb_type();
					notification::message m;
					m.type = sys::message_setting_type::cb_justify_no_longer_valid;
					m.primary = n;
					m.title = [=](sys::state& state, text::layout_base& layout) {
						text::substitution_map sub{};
						text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
								state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
						text::add_to_substitution_map(sub, text::variable_type::target, target);
						text::add_to_substitution_map(sub, text::variable_type::casus, state.world.cb_type_get_name(cbt));
						TEXT_NOTIF_MSG_TITLE(cb_justify_no_longer_valid);
					};
					m.body = [=](sys::state& state, text::layout_base& layout) {
						text::substitution_map sub{};
						text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
								state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
						text::add_to_substitution_map(sub, text::variable_type::target, target);
						text::add_to_substitution_map(sub, text::variable_type::casus, state.world.cb_type_get_name(cbt));
						TEXT_NOTIF_MSG_BODY(cb_justify_no_longer_valid);
					};
					notification::post(state, std::move(m));
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

			n.get_constructing_cb_progress() += state.defines.cb_generation_base_speed *
																					n.get_constructing_cb_type().get_construction_speed() *
																					(n.get_modifier_values(sys::national_mod_offsets::cb_generation_speed_modifier) + 1.0f);

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

			// TODO: cb fabrication events

			/*
			When fabrication progress reaches 100, the CB will remain valid for define:CREATED_CB_VALID_TIME months (so x30 days
			for us). Note that pending CBs have their target nation fixed, but all other parameters are flexible.
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
	current_cbs.push_back(
			military::available_cb{target, state.current_date + int32_t(state.defines.created_cb_valid_time) * 30, cb});

	if(n == state.local_player_nation) {
		notification::message m;
		m.type = sys::message_setting_type::gaincb;
		m.primary = n;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::enemy, target);
			text::add_to_substitution_map(sub, text::variable_type::casus, state.world.cb_type_get_name(cb));
			TEXT_NOTIF_MSG_TITLE(wegaincb);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::enemy, target);
			text::add_to_substitution_map(sub, text::variable_type::casus, state.world.cb_type_get_name(cb));
			TEXT_NOTIF_MSG_BODY(wegaincb);
		};
		notification::post(state, std::move(m));
	}
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
		total += state.world.province_get_naval_base_level(p);
	}
	auto fac_range = state.world.province_get_factory_location(p);
	total += int32_t(fac_range.end() - fac_range.begin());
	total += state.world.province_get_fort_level(p);

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
						total += 2.8f * float(province_point_cost(state, tprov.get_province(), target)) / float(sum_target_prov_values);
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

	notification::message m;
	m.primary = n;
	m.secondary = target;
	if(state.local_player_nation == n) {
		m.type = sys::message_setting_type::cb_detected_target;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::country, n);
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{infamy});
			text::add_to_substitution_map(sub, text::variable_type::target, target);
			text::add_to_substitution_map(sub, text::variable_type::casus,
					state.world.cb_type_get_name(state.world.nation_get_constructing_cb_type(n)));
			TEXT_NOTIF_MSG_TITLE(our_cb_detected);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::country, n);
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{infamy});
			text::add_to_substitution_map(sub, text::variable_type::target, target);
			text::add_to_substitution_map(sub, text::variable_type::casus,
					state.world.cb_type_get_name(state.world.nation_get_constructing_cb_type(n)));
			TEXT_NOTIF_MSG_BODY(our_cb_detected);
		};
	} else if(state.local_player_nation == target) {
		m.type = sys::message_setting_type::cb_towards_us_detected;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::country, n);
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{infamy});
			text::add_to_substitution_map(sub, text::variable_type::target, target);
			text::add_to_substitution_map(sub, text::variable_type::casus,
					state.world.cb_type_get_name(state.world.nation_get_constructing_cb_type(n)));
			TEXT_NOTIF_MSG_TITLE(cb_towards_us_detected);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::country, n);
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{infamy});
			text::add_to_substitution_map(sub, text::variable_type::target, target);
			text::add_to_substitution_map(sub, text::variable_type::casus,
					state.world.cb_type_get_name(state.world.nation_get_constructing_cb_type(n)));
			TEXT_NOTIF_MSG_BODY(cb_towards_us_detected);
		};
	} else {
		m.type = sys::message_setting_type::cb_detected_cause;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::country, n);
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{infamy});
			text::add_to_substitution_map(sub, text::variable_type::target, target);
			text::add_to_substitution_map(sub, text::variable_type::casus,
					state.world.cb_type_get_name(state.world.nation_get_constructing_cb_type(n)));
			TEXT_NOTIF_MSG_TITLE(others_cb_detected);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
					state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
			text::add_to_substitution_map(sub, text::variable_type::country, n);
			text::add_to_substitution_map(sub, text::variable_type::badboy, text::fp_two_places{infamy});
			text::add_to_substitution_map(sub, text::variable_type::target, target);
			text::add_to_substitution_map(sub, text::variable_type::casus,
					state.world.cb_type_get_name(state.world.nation_get_constructing_cb_type(n)));
			TEXT_NOTIF_MSG_BODY(others_cb_detected);
		};
	}
	notification::post(state, std::move(m));
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
			dcon::nation_id n = state.world.leader_get_nation_from_leader_loyalty(l);
			notification::message m;
			m.type = sys::message_setting_type::leaderdied;
			m.primary = n;
			m.title = [=](sys::state& state, text::layout_base& layout) {
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
						state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
				text::add_to_substitution_map(sub, text::variable_type::name, state.to_string_view(state.world.leader_get_name(l)));
				TEXT_NOTIF_MSG_TITLE(leaderdied);
			};
			m.body = [=](sys::state& state, text::layout_base& layout) {
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::monarchtitle,
						state.culture_definitions.governments[state.world.nation_get_government_type(n)].ruler_name);
				text::add_to_substitution_map(sub, text::variable_type::name, state.to_string_view(state.world.leader_get_name(l)));
				TEXT_NOTIF_MSG_BODY(leaderdied);
			};
			notification::post(state, std::move(m));
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
	return reg.id;
}
dcon::ship_id create_new_ship(sys::state& state, dcon::nation_id n, dcon::unit_type_id t) {
	auto shp = fatten(state.world, state.world.create_ship());
	shp.set_type(t);
	// TODO make name
	shp.set_strength(state.world.nation_get_unit_stats(n, t).defence_or_hull);
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

void add_to_war(sys::state& state, dcon::war_id w, dcon::nation_id n, bool as_attacker) {
	auto participant = state.world.force_create_war_participant(w, n);
	state.world.war_participant_set_is_attacker(participant, as_attacker);
	state.world.nation_set_is_at_war(n, true);
	state.world.nation_set_disarmed_until(n, sys::date{});

	for(auto dep : state.world.nation_get_overlord_as_ruler(n)) {
		add_to_war(state, w, dep.get_subject(), as_attacker);
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

	add_to_war(state, new_war, primary_attacker, true);
	add_to_war(state, new_war, real_target, false);

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
		if(drel.get_are_allied() && standard_war_joining_is_possible(state, wfor, n, false)) {

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
				standard_war_joining_is_possible(state, wfor, n, true)) {

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
}

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
			state.world.war_set_is_great(w, true);
			auto it = state.key_to_text_sequence.find(std::string_view{"great_war_name"}); // misspelling is intentional; DO NOT CORRECT
			if(it != state.key_to_text_sequence.end()) {
				state.world.war_set_name(w, it->second);
			}

			// TODO: notify
		}
	}
}

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

			float prestige_loss = std::min(state.defines.war_failed_goal_prestige_base,
																state.defines.war_failed_goal_prestige * nations::prestige_score(state, n)) *
														wg.get_wargoal().get_type().get_penalty_factor();
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
}

void cleanup_war(sys::state& state, dcon::war_id w, war_result result) {
	auto par = state.world.war_get_war_participant(w);
	while(par.begin() != par.end()) {
		if((*par.begin()).get_is_attacker()) {
			remove_from_war(state, w, (*par.begin()).get_nation(), result == war_result::defender_won);
		} else {
			remove_from_war(state, w, (*par.begin()).get_nation(), result == war_result::attacker_won);
		}
	}

	auto po = state.world.war_get_war_settlement(w);
	while(po.begin() != po.end()) {
		state.world.delete_peace_offer((*po.begin()).get_peace_offer());
	}

	auto wg = state.world.war_get_wargoals_attached(w);
	while(wg.begin() != wg.end()) {
		state.world.delete_wargoal((*wg.begin()).get_wargoal());
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
	assert(war);

	auto bits = state.world.cb_type_get_type_bits(wargoal);
	bool for_attacker = is_attacker(state, war, from);

	// po_add_to_sphere: leaves its current sphere and has its opinion of that nation set to hostile. Is added to the nation that
	// added the war goal's sphere with max influence.
	if((bits & cb_flag::po_add_to_sphere) != 0) {
		if(secondary_nation)
			take_from_sphere(state, secondary_nation, from);
		else
			take_from_sphere(state, target, from);
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
		state.world.force_create_overlord(target, from);
		take_from_sphere(state, target, from);
	}

	// po_destory_forts: reduces fort levels to zero in any targeted states
	if((bits & cb_flag::po_destroy_forts) != 0) {
		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					prov.get_province().set_fort_level(0);
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
							 trigger::to_generic(from))) {
					province::for_each_province_in_state_instance(state, si.get_state(),
							[&](dcon::province_id prov) { state.world.province_set_fort_level(prov, 0); });
				}
			}
		}
	}

	// po_destory_naval_bases: as above
	if((bits & cb_flag::po_destroy_naval_bases) != 0) {
		if((bits & cb_flag::all_allowed_states) == 0) {
			for(auto prov : state.world.state_definition_get_abstract_state_membership(wargoal_state)) {
				if(prov.get_province().get_nation_from_province_ownership() == target) {
					prov.get_province().set_naval_base_level(0);
				}
			}
		} else if(auto allowed_states = state.world.cb_type_get_allowed_states(wargoal); allowed_states) {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(from),
							 trigger::to_generic(from))) {
					province::for_each_province_in_state_instance(state, si.get_state(),
							[&](dcon::province_id prov) { state.world.province_set_naval_base_level(prov, 0); });
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

		if(target_existed && state.world.nation_get_owned_province_count(target) == 0) {
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
	float prestige_gain = successful_cb_prestige(state, wargoal, from);
	nations::adjust_prestige(state, from, prestige_gain);
	nations::adjust_prestige(state, target, -prestige_gain);

	/*
	A war goal added in this way will result in a status quo war goal on that side being removed.
	*/
}
void implement_peace_offer(sys::state& state, dcon::peace_offer_id offer) {
	dcon::nation_id from = state.world.peace_offer_get_nation_from_pending_peace_offer(offer);
	dcon::nation_id target = state.world.peace_offer_get_target(offer);

	notification::message m;
	m.primary = from;
	m.secondary = target;
	if(state.local_player_nation == from) {
		m.type = sys::message_setting_type::peace_accept_cause;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_TITLE(peace_they_accept);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_BODY(peace_they_accept);
		};
	} else if(state.local_player_nation == target) {
		m.type = sys::message_setting_type::peace_accept_target;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_TITLE(peace_we_accept);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_BODY(peace_we_accept);
		};
	} else {
		m.type = sys::message_setting_type::peace_accept_other;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_TITLE(peace_other_accept);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_BODY(peace_other_accept);
		};
	}
	notification::post(state, std::move(m));

	auto wg_range = state.world.peace_offer_get_peace_offer_item(offer);
	while(wg_range.begin() != wg_range.end()) {
		auto wg_offered = *(wg_range.begin());
		implement_war_goal(state, state.world.peace_offer_get_war_from_war_settlement(offer), wg_offered.get_wargoal().get_type(),
				wg_offered.get_wargoal().get_added_by(), wg_offered.get_wargoal().get_target_nation(),
				wg_offered.get_wargoal().get_secondary_nation(), wg_offered.get_wargoal().get_associated_state(),
				wg_offered.get_wargoal().get_associated_tag());
		state.world.delete_wargoal(wg_offered.get_wargoal());
	}

	auto war = state.world.peace_offer_get_war_from_war_settlement(offer);

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
		remove_from_war(state, war, target, state.world.peace_offer_get_is_concession(offer) == false);

	} else if(state.world.war_get_primary_attacker(war) == target || state.world.war_get_primary_defender(war) == target) {

		if(state.world.war_get_is_great(war) && state.world.peace_offer_get_is_concession(offer) == true) {
			implement_war_goal(state, war, state.military_definitions.standard_great_war, target, from, dcon::nation_id{},
					dcon::state_definition_id{}, dcon::national_identity_id{});
		}
		remove_from_war(state, war, from, state.world.peace_offer_get_is_concession(offer) == false);

	} else {
		assert(false);
	}
}

void reject_peace_offer(sys::state& state, dcon::peace_offer_id offer) {
	dcon::nation_id from = state.world.peace_offer_get_nation_from_pending_peace_offer(offer);
	dcon::nation_id target = state.world.peace_offer_get_target(offer);
	notification::message m;
	m.primary = from;
	m.secondary = target;
	if(state.local_player_nation == from) {
		m.type = sys::message_setting_type::peace_decline_cause;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_TITLE(peace_they_decline);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_BODY(peace_they_decline);
		};
	} else if(state.local_player_nation == target) {
		m.type = sys::message_setting_type::peace_decline_target;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_TITLE(peace_we_decline);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_BODY(peace_we_decline);
		};
	} else {
		m.type = sys::message_setting_type::peace_decline_other;
		m.title = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::country, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_TITLE(peace_other_decline);
		};
		m.body = [=](sys::state& state, text::layout_base& layout) {
			text::substitution_map sub{};
			text::add_to_substitution_map(sub, text::variable_type::actor, from);
			text::add_to_substitution_map(sub, text::variable_type::recipient, target);
			TEXT_NOTIF_MSG_BODY(peace_other_decline);
		};
	}
	notification::post(state, std::move(m));

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

float primary_warscore(sys::state& state, dcon::war_id w) {
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

	for(auto wg : state.world.war_get_wargoals_attached(w)) {
		if(is_attacker(state, w, wg.get_wargoal().get_added_by())) {
			total += wg.get_wargoal().get_ticking_war_score();
		} else {
			total -= wg.get_wargoal().get_ticking_war_score();
		}
	}

	total += std::clamp(state.world.war_get_attacker_battle_score(w) - state.world.war_get_defender_battle_score(w),
			-state.defines.max_warscore_from_battles, state.defines.max_warscore_from_battles);

	return std::clamp(total, -100.0f, 100.0f);
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
			if(prv.get_province().get_nation_from_province_control() == primary)
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
		if((wg.get_wargoal().get_added_by() == primary || is_pattacker || is_pdefender) &&
				wg.get_wargoal().get_target_nation() == secondary) {
			total += wg.get_wargoal().get_ticking_war_score();
		} else if(wg.get_wargoal().get_added_by() == secondary &&
							(wg.get_wargoal().get_target_nation() == primary || is_tpattacker || is_tpdefender)) {
			total -= wg.get_wargoal().get_ticking_war_score();
		}
	}

	return std::clamp(total, 0.0f, 100.0f);
}

bool can_embark_onto_sea_tile(sys::state& state, dcon::nation_id from, dcon::province_id p, dcon::army_id a) {
	int32_t max_cap = 0;
	for(auto n : state.world.province_get_navy_location(p)) {
		if(n.get_navy().get_controller_from_navy_control() == from) {
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
	return min_speed *
				 (1.0f + state.world.province_get_railroad_level(state.world.army_get_location_from_army_location(a)) *
										 state.economy_definitions.railroad_definition.infrastructure) *
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
	return min_speed * (leader_move + 1.0f);
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

void army_arrives_in_province(sys::state& state, dcon::army_id a, dcon::province_id p) {
	state.world.army_set_location_from_army_location(a, p);
	if(!state.world.army_get_black_flag(a)) {
		// TODO: start battle
	}
}

void navy_arrives_in_province(sys::state& state, dcon::navy_id n, dcon::province_id p) {
	state.world.navy_set_location_from_navy_location(n, p);

	// TODO: start battle
}

void update_movement(sys::state& state) {
	for(auto a : state.world.in_army) {
		if(auto path = a.get_path(); a.get_arrival_time() == state.current_date && path.size() > 0) {
			auto dest = path.at(path.size() - 1);
			path.pop_back();

			if(dest.index() < state.province_definitions.first_sea_province.index()) { // sea province
				// check for embarkation possibility, then embark
				auto to_navy = find_embark_target(state, a.get_controller_from_army_control(), dest, a);
				if(to_navy) {
					a.set_location_from_army_location(dest);
					a.set_navy_from_army_transport(to_navy);
				} else {
					path.clear();
				}
			} else { // land province
				if(a.get_black_flag() || province::has_access_to_province(state, a.get_controller_from_army_control(), dest)) {
					army_arrives_in_province(state, a, dest);
					a.set_navy_from_army_transport(dcon::navy_id{});
				} else {
					path.clear();
				}
			}

			if(path.size() > 0) {
				auto next_dest = path.at(path.size() - 1);
				a.set_arrival_time(arrival_time_to(state, a, next_dest));
			} else {
				a.set_arrival_time(sys::date{});
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
						a.set_location_from_army_location(dest);
						a.get_path().clear();
						a.set_arrival_time(sys::date{});
					}
				} else {
					path.clear();
				}
			} else { // sea province
				// TODO check for battle

				navy_arrives_in_province(state, n, dest);

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

} // namespace military
