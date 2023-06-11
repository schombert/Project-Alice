#include "commands.hpp"
#include "system_state.hpp"
#include "nations.hpp"

namespace command {

bool console_command(sys::state& state, command_type t) {
	return (uint8_t(t) & 0x80) != 0;
}

void set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state,
		dcon::national_focus_id focus) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_nat_focus;
	p.source = source;
	p.data.nat_focus.focus = focus;
	p.data.nat_focus.target_state = target_state;
	auto b = state.incoming_commands.try_push(p);
}

bool can_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state,
		dcon::national_focus_id focus) {
	if(!focus) {
		return true;
	} else {
		auto num_focuses_set = nations::national_focuses_in_use(state, source);
		auto num_focuses_total = nations::max_national_focuses(state, source);
		auto state_owner = state.world.state_instance_get_nation_from_state_ownership(target_state);

		if(state_owner == source) {
			if(focus == state.national_definitions.flashpoint_focus)
				return false;
			if(auto ideo = state.world.national_focus_get_ideology(focus); ideo) {
				if(state.world.ideology_get_enabled(ideo) == false ||
						(state.world.ideology_get_is_civilized_only(ideo) && !state.world.nation_get_is_civilized(source))) {
					return false;
				}
			}
			return num_focuses_set < num_focuses_total || bool(state.world.state_instance_get_owner_focus(target_state));
		} else {
			auto pc = state.world.nation_get_primary_culture(source);
			if(nations::nation_accepts_culture(state, state_owner, pc))
				return false;

			auto ident = state.world.nation_get_identity_from_identity_holder(source);
			if(state.world.national_identity_get_is_not_releasable(ident))
				return false;

			bool state_contains_core = false;
			province::for_each_province_in_state_instance(state, target_state, [&](dcon::province_id p) {
				state_contains_core = state_contains_core || bool(state.world.get_core_by_prov_tag_key(p, ident));
			});
			return state_contains_core && state.world.nation_get_rank(source) > uint16_t(state.defines.colonial_rank) &&
						 focus == state.national_definitions.flashpoint_focus &&
						 (num_focuses_set < num_focuses_total || bool(state.world.nation_get_state_from_flashpoint_focus(source))) &&
						 bool(state.world.state_instance_get_nation_from_flashpoint_focus(target_state)) == false;
		}
	}
}

void execute_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state,
		dcon::national_focus_id focus) {
	if(!can_set_national_focus(state, source, target_state, focus))
		return;

	if(state.world.state_instance_get_nation_from_state_ownership(target_state) == source) {
		state.world.state_instance_set_owner_focus(target_state, focus);
	} else {
		state.world.nation_set_state_from_flashpoint_focus(source, target_state);
	}
}

void start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::start_research;
	p.source = source;
	p.data.start_research.tech = tech;
	auto b = state.incoming_commands.try_push(p);
}

bool can_start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech) {
	/* Nations can only start researching technologies if, they are not uncivilized, the tech
		 activation date is past by, and all the previous techs (if any) of the same folder index
		 are already researched fully. And they are not already researched. */
	if(state.world.nation_get_active_technologies(source, tech))
		return false; // Already researched
	if(nations::current_research(state, source) == tech)
		return false; // Already being researched
	if(!state.world.nation_get_is_civilized(source))
		return false; // Must be civilized
	if(state.current_date.to_ymd(state.start_date).year >= state.world.technology_get_year(tech)) {
		// Find previous technology before this one
		dcon::technology_id prev_tech = dcon::technology_id(dcon::technology_id::value_base_t(tech.index() - 1));
		// Previous technology is from the same folder so we have to check that we have researched it beforehand
		if(tech.index() != 0 && state.world.technology_get_folder_index(prev_tech) == state.world.technology_get_folder_index(tech)) {
			// Only allow if all previously researched techs are researched
			return state.world.nation_get_active_technologies(source, prev_tech);
		}
		return true; // First technology on folder can always be researched
	}
	return false;
}

void execute_start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech) {
	if(!can_start_research(state, source, tech))
		return;
	state.world.nation_set_current_research(source, tech);
}

void make_leader(sys::state& state, dcon::nation_id source, bool general) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::make_leader;
	p.source = source;
	p.data.make_leader.is_general = general;
	auto b = state.incoming_commands.try_push(p);
}
bool can_make_leader(sys::state& state, dcon::nation_id source, bool general) {
	return state.world.nation_get_leadership_points(source) >= state.defines.leader_recruit_cost;
}
void execute_make_leader(sys::state& state, dcon::nation_id source, bool general) {
	if(!can_make_leader(state, source, general))
		return;

	military::make_new_leader(state, source, general);
}

// -----------------------------------------------------------------------------
void give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::war_subsidies;
	p.source = source;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	/* Can only perform if, the nations are not at war, the nation isn't already being given war subsidies, and there is
	 * defines:WARSUBSIDY_DIPLOMATIC_COST diplomatic points available. And the target isn't equal to the sender. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(rel && state.world.unilateral_relationship_get_war_subsidies(rel))
		return false; // Can't already be giving war subsidies
	return state.world.nation_get_diplomatic_points(source) >= state.defines.warsubsidy_diplomatic_cost; // Enough diplomatic points
}
void execute_give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_give_war_subsidies(state, source, target))
		return;
	nations::adjust_relationship(state, source, target, state.defines.warsubsidy_relation_on_accept);
	state.world.nation_get_diplomatic_points(source) -= state.defines.warsubsidy_diplomatic_cost;
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	state.world.unilateral_relationship_set_war_subsidies(rel, true);
}

// -----------------------------------------------------------------------------
void cancel_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_war_subsidies;
	p.source = source;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_cancel_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	/* Can only perform if, the nations are not at war, the nation is already being given war subsidies, and there is
	 * defines:CANCELWARSUBSIDY_DIPLOMATIC_COST diplomatic points available. And the target isn't equal to the sender. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(rel && !state.world.unilateral_relationship_get_war_subsidies(rel))
		return false; // Must already be giving war subsidies
	return state.world.nation_get_diplomatic_points(source) >=
				 state.defines.cancelwarsubsidy_diplomatic_cost; // Enough diplomatic points
}
void execute_cancel_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_cancel_war_subsidies(state, source, target))
		return;
	nations::adjust_relationship(state, source, target, state.defines.cancelwarsubsidy_relation_on_accept);
	state.world.nation_get_diplomatic_points(source) -= state.defines.cancelwarsubsidy_diplomatic_cost;
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	state.world.unilateral_relationship_set_war_subsidies(rel, false);
}

// -----------------------------------------------------------------------------
void increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::increase_relations;
	p.source = source;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	/* Can only perform if, the nations are not at war, the relation value isn't maxxed out at 200, and has
	 * defines:INCREASERELATION_DIPLOMATIC_COST diplomatic points. And the target can't be the same as the sender. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(source, target);
	if(rel && state.world.diplomatic_relation_get_value(rel) >= 200.f)
		return false; // Maxxed out
	return state.world.nation_get_diplomatic_points(source) >=
				 state.defines.increaserelation_diplomatic_cost; // Enough diplomatic points
}
void execute_increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_increase_relations(state, source, target))
		return;
	nations::adjust_relationship(state, source, target, state.defines.increaserelation_relation_on_accept);
	state.world.nation_get_diplomatic_points(source) -= state.defines.increaserelation_diplomatic_cost;
}

// -----------------------------------------------------------------------------
void decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::decrease_relations;
	p.source = source;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	/* Can only perform if, the nations are not at war, the relation value isn't maxxed out at -200, and has
	 * defines:DECREASERELATION_DIPLOMATIC_COST diplomatic points. And not done to self. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(source, target);
	if(rel && state.world.diplomatic_relation_get_value(rel) <= -200.f)
		return false; // Maxxed out
	return state.world.nation_get_diplomatic_points(source) >=
				 state.defines.decreaserelation_diplomatic_cost; // Enough diplomatic points
}
void execute_decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_decrease_relations(state, source, target))
		return;

	nations::adjust_relationship(state, source, target, state.defines.decreaserelation_relation_on_accept);
	state.world.nation_get_diplomatic_points(source) -= state.defines.decreaserelation_diplomatic_cost;
}

// -----------------------------------------------------------------------------
void begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id prov,
		economy::province_building_type type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::begin_province_building_construction;
	p.source = source;
	p.data.start_province_building.location = prov;
	p.data.start_province_building.type = type;
	auto b = state.incoming_commands.try_push(p);
}
bool can_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p,
		economy::province_building_type type) {

	switch(type) {
	case economy::province_building_type::railroad:
		return province::can_build_railroads(state, p, source);
	case economy::province_building_type::fort:
		return province::can_build_fort(state, p, source);
	case economy::province_building_type::naval_base:
		return province::can_build_naval_base(state, p, source);
	}

	return false;
}
void execute_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p,
		economy::province_building_type type) {

	if(!can_begin_province_building_construction(state, source, p, type))
		return;

	if(type == economy::province_building_type::railroad && source != state.world.province_get_nation_from_province_ownership(p)) {
		float amount = 0.0f;

		auto& base_cost = state.economy_definitions.railroad_definition.cost;

		for(uint32_t j = 0; j < economy::commodity_set::set_size; ++j) {
			if(base_cost.commodity_type[j]) {
				amount += base_cost.commodity_amounts[j] * state.world.commodity_get_current_price(base_cost.commodity_type[j]);
			} else {
				break;
			}
		}

		nations::adjust_foreign_investment(state, source, state.world.province_get_nation_from_province_ownership(p), amount);
	}

	auto new_rr = fatten(state.world, state.world.force_create_province_building_construction(p, source));
	new_rr.set_is_pop_project(false);
	new_rr.set_type(uint8_t(type));
}

void begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location,
		dcon::factory_type_id type, bool is_upgrade) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::begin_factory_building_construction;
	p.source = source;
	p.data.start_factory_building.location = location;
	p.data.start_factory_building.type = type;
	p.data.start_factory_building.is_upgrade = is_upgrade;
	auto b = state.incoming_commands.try_push(p);
}

bool can_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location,
		dcon::factory_type_id type, bool is_upgrade) {

	auto owner = state.world.state_instance_get_nation_from_state_ownership(location);

	/*
	The factory building must be unlocked by the nation.
	Factories cannot be built in a colonial state.
	*/

	if(state.world.nation_get_active_building(source, type) == false && !state.world.factory_type_get_is_available_from_start(type))
		return false;
	if(state.world.province_get_is_colonial(state.world.state_instance_get_capital(location)))
		return false;

	/* There can't be duplicate factories... */
	if(!is_upgrade) {
		// Check factories being built
		bool has_dup = false;
		economy::for_each_new_factory(state, location, [&](economy::new_factory const& nf) { has_dup = has_dup || nf.type == type; });
		if(has_dup)
			return false;

		// Check actual factories
		auto d = state.world.state_instance_get_definition(location);
		for(auto p : state.world.state_definition_get_abstract_state_membership(d))
			if(p.get_province().get_nation_from_province_ownership() == owner)
				for(auto f : p.get_province().get_factory_location())
					if(f.get_factory().get_building_type() == type)
						return false;
	}

	if(owner != source) {
		/*
		For foreign investment: the target nation must allow foreign investment, the nation doing the investing must be a great
		power while the target is not a great power, and the nation doing the investing must not be at war with the target nation.
		The nation being invested in must be civilized.
		*/
		if(state.world.nation_get_is_great_power(source) == false || state.world.nation_get_is_great_power(owner) == true)
			return false;
		if(state.world.nation_get_is_civilized(owner) == false)
			return false;

		auto rules = state.world.nation_get_combined_issue_rules(owner);
		if((rules & issue_rule::allow_foreign_investment) == 0)
			return false;

		if(military::are_at_war(state, source, owner))
			return false;
	} else {
		/*
		The nation must have the rule set to allow building / upgrading if this is a domestic target.
		*/
		if(state.world.nation_get_is_civilized(source) == false)
			return false;

		auto rules = state.world.nation_get_combined_issue_rules(owner);
		if(is_upgrade) {
			if((rules & issue_rule::expand_factory) == 0)
				return false;
		} else {
			if((rules & issue_rule::build_factory) == 0)
				return false;
		}
	}

	if(is_upgrade) {
		// no double upgrade
		for(auto p : state.world.state_instance_get_state_building_construction(location)) {
			if(p.get_type() == type)
				return false;
		}

		// must already exist as a factory
		// For upgrades: no upgrading past max level.
		auto d = state.world.state_instance_get_definition(location);
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == owner) {
				for(auto f : p.get_province().get_factory_location()) {
					if(f.get_factory().get_building_type() == type && f.get_factory().get_level() < uint8_t(255)) {
						return true;
					}
				}
			}
		}
		return false;
	} else {
		// coastal factories must be built on coast
		if(state.world.factory_type_get_is_coastal(type)) {
			if(!province::state_is_coastal(state, location))
				return false;
		}

		// For new factories: no more than defines:FACTORIES_PER_STATE existing + under construction new factories must be
		// present.
		int32_t num_factories = 0;

		auto d = state.world.state_instance_get_definition(location);
		for(auto p : state.world.state_definition_get_abstract_state_membership(d)) {
			if(p.get_province().get_nation_from_province_ownership() == owner) {
				for(auto f : p.get_province().get_factory_location()) {
					++num_factories;
				}
			}
		}
		for(auto p : state.world.state_instance_get_state_building_construction(location)) {
			if(p.get_is_upgrade() == false)
				++num_factories;
		}
		return num_factories <= int32_t(state.defines.factories_per_state);
	}
}

void execute_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location,
		dcon::factory_type_id type, bool is_upgrade) {

	if(!can_begin_factory_building_construction(state, source, location, type, is_upgrade))
		return;

	auto new_up = fatten(state.world, state.world.force_create_state_building_construction(location, source));
	new_up.set_is_pop_project(false);
	new_up.set_is_upgrade(is_upgrade);
	new_up.set_type(type);

	if(source != state.world.state_instance_get_nation_from_state_ownership(location)) {
		float amount = 0.0f;

		auto& base_cost = state.world.factory_type_get_construction_costs(type);

		for(uint32_t j = 0; j < economy::commodity_set::set_size; ++j) {
			if(base_cost.commodity_type[j]) {
				amount += base_cost.commodity_amounts[j] * state.world.commodity_get_current_price(base_cost.commodity_type[j]);
			} else {
				break;
			}
		}

		nations::adjust_foreign_investment(state, source, state.world.state_instance_get_nation_from_state_ownership(location),
				amount);
	}
}

void start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::unit_type_id type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::begin_naval_unit_construction;
	p.source = source;
	p.data.naval_unit_construction.location = location;
	p.data.naval_unit_construction.type = type;
	auto b = state.incoming_commands.try_push(p);
}

bool can_start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::unit_type_id type) {
	/*
	The province must be owned and controlled by the building nation, without an ongoing siege.
	The unit type must be available from start / unlocked by the nation
	*/

	if(state.world.province_get_nation_from_province_ownership(location) != source)
		return false;
	if(state.world.province_get_nation_from_province_control(location) != source)
		return false;
	if(state.world.nation_get_active_unit(source, type) == false &&
			state.military_definitions.unit_base_definitions[type].active == false)
		return false;

	if(state.military_definitions.unit_base_definitions[type].is_land) {
		return false;
	} else {
		/*
		The province must be coastal
		The province must have a naval base of sufficient level, depending on the unit type
		The province may not be overseas for some unit types
		Some units have a maximum number per port where they can built that must be respected
		*/
		if(!state.world.province_get_is_coast(location))
			return false;

		auto overseas_allowed = state.military_definitions.unit_base_definitions[type].can_build_overseas;
		auto level_req = state.military_definitions.unit_base_definitions[type].min_port_level;

		if(state.world.province_get_naval_base_level(location) < level_req)
			return false;

		if(!overseas_allowed && province::is_overseas(state, location))
			return false;

		return true;
	}
}

void execute_start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::unit_type_id type) {
	if(!can_start_naval_unit_construction(state, source, location, type))
		return;

	auto c = fatten(state.world, state.world.try_create_province_naval_construction(location, source));
	c.set_type(type);
}

void start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::culture_id soldier_culture, dcon::unit_type_id type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::begin_land_unit_construction;
	p.source = source;
	p.data.land_unit_construction.location = location;
	p.data.land_unit_construction.type = type;
	p.data.land_unit_construction.pop_culture = soldier_culture;
	auto b = state.incoming_commands.try_push(p);
}
bool can_start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::culture_id soldier_culture, dcon::unit_type_id type) {
	/*
	The province must be owned and controlled by the building nation, without an ongoing siege.
	The unit type must be available from start / unlocked by the nation
	*/

	if(state.world.province_get_nation_from_province_ownership(location) != source)
		return false;
	if(state.world.province_get_nation_from_province_control(location) != source)
		return false;
	if(state.world.nation_get_active_unit(source, type) == false &&
			state.military_definitions.unit_base_definitions[type].active == false)
		return false;

	if(state.military_definitions.unit_base_definitions[type].is_land) {
		/*
		Each soldier pop can only support so many regiments (including under construction and rebel regiments)
		If the unit is culturally restricted, there must be an available primary culture/accepted culture soldier pop with space
		*/
		auto soldier = military::find_available_soldier(state, location, soldier_culture);
		return bool(soldier);
	} else {
		return false;
	}
}
void execute_start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::culture_id soldier_culture, dcon::unit_type_id type) {

	if(!can_start_land_unit_construction(state, source, location, soldier_culture, type))
		return;

	auto soldier = military::find_available_soldier(state, location, soldier_culture);

	auto c = fatten(state.world, state.world.try_create_province_land_construction(soldier, source));
	c.set_type(type);
}

void cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::unit_type_id type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_naval_unit_construction;
	p.source = source;
	p.data.naval_unit_construction.location = location;
	p.data.naval_unit_construction.type = type;
	auto b = state.incoming_commands.try_push(p);
}

bool can_cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::unit_type_id type) {
	return state.world.province_get_nation_from_province_ownership(location) == source;
}

void execute_cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::unit_type_id type) {
	if(!can_cancel_naval_unit_construction(state, source, location, type))
		return;

	dcon::province_naval_construction_id c;
	for(auto lc : state.world.province_get_province_naval_construction(location)) {
		if(lc.get_type() == type) {
			c = lc.id;
		}
	}
	state.world.delete_province_naval_construction(c);
}

void cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::culture_id soldier_culture, dcon::unit_type_id type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_land_unit_construction;
	p.source = source;
	p.data.land_unit_construction.location = location;
	p.data.land_unit_construction.type = type;
	p.data.land_unit_construction.pop_culture = soldier_culture;
	auto b = state.incoming_commands.try_push(p);
}
bool can_cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::culture_id soldier_culture, dcon::unit_type_id type) {
	return state.world.province_get_nation_from_province_ownership(location) == source;
}
void execute_cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::culture_id soldier_culture, dcon::unit_type_id type) {

	if(!can_cancel_land_unit_construction(state, source, location, soldier_culture, type))
		return;

	dcon::province_land_construction_id c;
	for(auto pop : state.world.province_get_pop_location(location)) {
		for(auto lc : pop.get_pop().get_province_land_construction()) {
			if(lc.get_type() == type) {
				c = lc.id;
			}
		}
	}
	state.world.delete_province_land_construction(c);
}

void delete_factory(sys::state& state, dcon::nation_id source, dcon::factory_id f) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::delete_factory;
	p.source = source;
	p.data.factory.location = state.world.factory_get_province_from_factory_location(f);
	p.data.factory.type = state.world.factory_get_building_type(f);
	auto b = state.incoming_commands.try_push(p);
}
bool can_delete_factory(sys::state& state, dcon::nation_id source, dcon::factory_id f) {
	auto loc = state.world.factory_get_province_from_factory_location(f);
	if(state.world.province_get_nation_from_province_ownership(loc) != source)
		return false;
	auto rules = state.world.nation_get_combined_issue_rules(source);
	if((rules & issue_rule::destroy_factory) == 0)
		return false;
	return true;
}
void execute_delete_factory(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
	if(state.world.province_get_nation_from_province_ownership(location) != source)
		return;

	auto rules = state.world.nation_get_combined_issue_rules(source);
	if((rules & issue_rule::destroy_factory) == 0)
		return;

	for(auto f : state.world.province_get_factory_location(location)) {
		if(f.get_factory().get_building_type() == type) {
			state.world.delete_factory(f.get_factory());
			return;
		}
	}
}

void change_factory_settings(sys::state& state, dcon::nation_id source, dcon::factory_id f, uint8_t priority, bool subsidized) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_factory_settings;
	p.source = source;
	p.data.factory.location = state.world.factory_get_province_from_factory_location(f);
	p.data.factory.type = state.world.factory_get_building_type(f);
	p.data.factory.priority = priority;
	p.data.factory.subsidize = subsidized;
	auto b = state.incoming_commands.try_push(p);
}
bool can_change_factory_settings(sys::state& state, dcon::nation_id source, dcon::factory_id f, uint8_t priority,
		bool subsidized) {
	auto loc = state.world.factory_get_province_from_factory_location(f);
	if(state.world.province_get_nation_from_province_ownership(loc) != source)
		return false;

	auto rules = state.world.nation_get_combined_issue_rules(source);

	auto current_priority = economy::factory_priority(state, f);
	if(priority >= 4)
		return false;

	if(current_priority != priority) {
		if((rules & issue_rule::factory_priority) == 0)
			return false;
	}

	if(subsidized && (rules & issue_rule::can_subsidise) == 0) {
		return false;
	}

	return true;
}
void execute_change_factory_settings(sys::state& state, dcon::nation_id source, dcon::province_id location,
		dcon::factory_type_id type, uint8_t priority, bool subsidized) {

	if(state.world.province_get_nation_from_province_ownership(location) != source)
		return;

	auto rules = state.world.nation_get_combined_issue_rules(source);

	if(subsidized && (rules & issue_rule::can_subsidise) == 0) {
		return;
	}

	for(auto f : state.world.province_get_factory_location(location)) {
		if(f.get_factory().get_building_type() == type) {
			auto current_priority = economy::factory_priority(state, f.get_factory());
			if(current_priority != priority) {
				if((rules & issue_rule::factory_priority) == 0)
					return;
				economy::set_factory_priority(state, f.get_factory(), priority);
			}
			f.get_factory().set_subsidized(subsidized);
			return;
		}
	}
}

void make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::make_vassal;
	p.source = source;
	p.data.tag_target.ident = t;
	auto b = state.incoming_commands.try_push(p);
}
bool can_make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	return nations::can_release_as_vassal(state, source, t);
}
void execute_make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	if(!can_make_vassal(state, source, t))
		return;

	nations::liberate_nation_from(state, t, source);
	auto holder = state.world.national_identity_get_nation_from_identity_holder(t);
	state.world.force_create_overlord(holder, source);
	if(state.world.nation_get_is_great_power(source)) {
		auto sr = state.world.force_create_gp_relationship(holder, source);
		auto& flags = state.world.gp_relationship_get_status(sr);
		flags = uint8_t((flags & ~nations::influence::level_mask) | nations::influence::level_in_sphere);
	}
	nations::remove_cores_from_owned(state, holder, state.world.nation_get_identity_from_identity_holder(source));
	auto& inf = state.world.nation_get_infamy(source);
	inf = std::max(0.0f, inf + state.defines.release_nation_infamy);
	nations::adjust_prestige(state, source, state.defines.release_nation_prestige);
}

void release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::release_and_play_nation;
	p.source = source;
	p.data.tag_target.ident = t;
	auto b = state.incoming_commands.try_push(p);
}
bool can_release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	return nations::can_release_as_vassal(state, source, t);
}
void execute_release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	if(!can_release_and_play_as(state, source, t))
		return;

	nations::liberate_nation_from(state, t, source);
	auto holder = state.world.national_identity_get_nation_from_identity_holder(t);
	nations::remove_cores_from_owned(state, holder, state.world.nation_get_identity_from_identity_holder(source));

	auto old_controller = state.world.nation_get_is_player_controlled(holder);
	state.world.nation_set_is_player_controlled(holder, state.world.nation_get_is_player_controlled(source));
	state.world.nation_set_is_player_controlled(source, old_controller);

	if(state.local_player_nation == source) {
		state.local_player_nation = holder;
	} else if(state.local_player_nation == holder) {
		state.local_player_nation = source;
	}
}

inline bool can_change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values) {
	return true;
}

void change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_budget;
	p.source = source;
	p.data.budget_data = values;
	auto b = state.incoming_commands.try_push(p);
}
void execute_change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values) {
	if(!can_change_budget_settings(state, source, values))
		return;

	if(values.administrative_spending != int8_t(-127)) {
		state.world.nation_set_administrative_spending(source, std::clamp(values.administrative_spending, int8_t(0), int8_t(100)));
	}
	if(values.construction_spending != int8_t(-127)) {
		state.world.nation_set_construction_spending(source, std::clamp(values.construction_spending, int8_t(0), int8_t(100)));
	}
	if(values.education_spending != int8_t(-127)) {
		state.world.nation_set_education_spending(source, std::clamp(values.education_spending, int8_t(0), int8_t(100)));
	}
	if(values.land_spending != int8_t(-127)) {
		state.world.nation_set_land_spending(source, std::clamp(values.land_spending, int8_t(0), int8_t(100)));
	}
	if(values.middle_tax != int8_t(-127)) {
		state.world.nation_set_middle_tax(source, std::clamp(values.middle_tax, int8_t(0), int8_t(100)));
	}
	if(values.poor_tax != int8_t(-127)) {
		state.world.nation_set_poor_tax(source, std::clamp(values.poor_tax, int8_t(0), int8_t(100)));
	}
	if(values.rich_tax != int8_t(-127)) {
		state.world.nation_set_rich_tax(source, std::clamp(values.rich_tax, int8_t(0), int8_t(100)));
	}
	if(values.military_spending != int8_t(-127)) {
		state.world.nation_set_military_spending(source, std::clamp(values.military_spending, int8_t(0), int8_t(100)));
	}
	if(values.naval_spending != int8_t(-127)) {
		state.world.nation_set_naval_spending(source, std::clamp(values.naval_spending, int8_t(0), int8_t(100)));
	}
	if(values.social_spending != int8_t(-127)) {
		state.world.nation_set_social_spending(source, std::clamp(values.social_spending, int8_t(0), int8_t(100)));
	}
	if(values.tariffs != int8_t(-127)) {
		state.world.nation_set_tariffs(source, std::clamp(values.tariffs, int8_t(-100), int8_t(100)));
	}
	economy::bound_budget_settings(state, source);
}

void start_election(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::start_election;
	p.source = source;
	auto b = state.incoming_commands.try_push(p);
}
bool can_start_election(sys::state& state, dcon::nation_id source) {
	auto type = state.world.nation_get_government_type(source);
	return type && state.culture_definitions.governments[type].has_elections && !politics::is_election_ongoing(state, source);
}
void execute_start_election(sys::state& state, dcon::nation_id source) {
	if(!can_start_election(state, source))
		return;
	politics::start_election(state, source);
}

void change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_influence_priority;
	p.source = source;
	p.data.influence_priority.influence_target = influence_target;
	p.data.influence_priority.priority = priority;
	auto b = state.incoming_commands.try_push(p);
}
bool can_change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		uint8_t priority) {
	// The source must be a great power, while the target must not be a great power.
	return state.world.nation_get_is_great_power(source) && !state.world.nation_get_is_great_power(influence_target);
}
void execute_change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		uint8_t priority) {
	if(!can_change_influence_priority(state, source, influence_target, priority))
		return;
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel) {
		rel = state.world.force_create_gp_relationship(influence_target, source);
	}
	auto& flags = state.world.gp_relationship_get_status(rel);
	switch(priority) {
	case 0:
		flags = (flags & ~nations::influence::priority_mask) | nations::influence::priority_zero;
		break;
	case 1:
		flags = (flags & ~nations::influence::priority_mask) | nations::influence::priority_one;
		break;
	case 2:
		flags = (flags & ~nations::influence::priority_mask) | nations::influence::priority_two;
		break;
	case 3:
		flags = (flags & ~nations::influence::priority_mask) | nations::influence::priority_three;
		break;
	default:
		break;
	}
}

void discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::discredit_advisors;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	p.data.influence_action.gp_target = affected_gp;
	auto b = state.incoming_commands.try_push(p);
}
bool can_discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:DISCREDIT_INFLUENCE_COST influence points. The source may not be
	currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a
	secondary target for this action. To discredit a nation, you must have an opinion of at least "opposed" with the influenced
	nation and you must have a an equal or better opinion level with the influenced nation than the nation you are discrediting
	does.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) ||
			state.world.nation_get_is_great_power(influence_target))
		return false;

	if(source == affected_gp)
		return false;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel)
		return false;

	if(state.world.gp_relationship_get_influence(rel) < state.defines.discredit_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel == nations::influence::level_hostile)
		return false;

	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	if((state.world.gp_relationship_get_status(orel) & nations::influence::is_banned) != 0)
		return false;

	return nations::influence::is_influence_level_greater_or_equal(clevel,
			nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	if(!can_discredit_advisors(state, source, influence_target, affected_gp))
		return;

	/*
	A nation is discredited for define:DISCREDIT_DAYS. Being discredited twice does not add these durations together; it just
	resets the timer from the current day. Discrediting a nation "increases" your relationship with them by
	define:DISCREDIT_RELATION_ON_ACCEPT. Discrediting costs define:DISCREDIT_INFLUENCE_COST influence points.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	if(!orel)
		orel = state.world.force_create_gp_relationship(influence_target, affected_gp);

	state.world.gp_relationship_get_influence(rel) -= state.defines.discredit_influence_cost;
	nations::adjust_relationship(state, source, affected_gp, state.defines.discredit_relation_on_accept);
	state.world.gp_relationship_get_status(orel) |= nations::influence::is_discredited;
	state.world.gp_relationship_set_penalty_expires_date(orel, state.current_date + int32_t(state.defines.discredit_days));
}

void expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::expel_advisors;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	p.data.influence_action.gp_target = affected_gp;
	auto b = state.incoming_commands.try_push(p);
}
bool can_expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:EXPELADVISORS_INFLUENCE_COST influence points. The source may
	not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power
	can be a secondary target for this action. To expel advisors you must have at least neutral opinion with the influenced nation
	and an equal or better opinion level than that of the nation you are expelling.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) ||
			state.world.nation_get_is_great_power(influence_target))
		return false;

	if(source == affected_gp)
		return false;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel)
		return false;

	if(state.world.gp_relationship_get_influence(rel) < state.defines.expeladvisors_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel == nations::influence::level_hostile || clevel == nations::influence::level_opposed)
		return false;

	return nations::influence::is_influence_level_greater_or_equal(clevel,
			nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	if(!can_expel_advisors(state, source, influence_target, affected_gp))
		return;

	/*
	Expelling a nation's advisors "increases" your relationship with them by define:EXPELADVISORS_RELATION_ON_ACCEPT. This action
	costs define:EXPELADVISORS_INFLUENCE_COST influence points. Being expelled cancels any ongoing discredit effect. Being
	expelled reduces your influence to zero.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);

	state.world.gp_relationship_get_influence(rel) -= state.defines.expeladvisors_influence_cost;
	nations::adjust_relationship(state, source, affected_gp, state.defines.expeladvisors_relation_on_accept);

	if(orel) {
		state.world.gp_relationship_set_influence(orel, 0.0f);
		state.world.gp_relationship_get_status(orel) &= ~nations::influence::is_discredited;
	}
}

void ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::ban_embassy;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	p.data.influence_action.gp_target = affected_gp;
	auto b = state.incoming_commands.try_push(p);
}
bool can_ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:BANEMBASSY_INFLUENCE_COST influence points. The source may not
	be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can
	be a secondary target for this action. To ban a nation you must be at least friendly with the influenced nation and have an
	equal or better opinion level than that of the nation you are expelling.
	*/

	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) ||
			state.world.nation_get_is_great_power(influence_target))
		return false;

	if(source == affected_gp)
		return false;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel)
		return false;

	if(state.world.gp_relationship_get_influence(rel) < state.defines.banembassy_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel != nations::influence::level_friendly || clevel != nations::influence::level_in_sphere)
		return false;

	return nations::influence::is_influence_level_greater_or_equal(clevel,
			nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	if(!can_ban_embassy(state, source, influence_target, affected_gp))
		return;

	/*
	Banning a nation's embassy "increases" your relationship with them by define:BANEMBASSY_RELATION_ON_ACCEPT. This action costs
	define:BANEMBASSY_INFLUENCE_COST influence points. The ban embassy effect lasts for define:BANEMBASSY_DAYS. If you are already
	banned, being banned again simply restarts the timer. Being banned cancels out any ongoing discredit effect.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	if(!orel)
		orel = state.world.force_create_gp_relationship(influence_target, affected_gp);

	state.world.gp_relationship_get_influence(rel) -= state.defines.banembassy_influence_cost;
	nations::adjust_relationship(state, source, affected_gp, state.defines.banembassy_relation_on_accept);
	state.world.gp_relationship_get_status(orel) |= nations::influence::is_banned;
	state.world.gp_relationship_set_penalty_expires_date(orel, state.current_date + int32_t(state.defines.banembassy_days));
}

void increase_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::increase_opinion;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_increase_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {
	/*
	The source must be a great power. The source must have define:INCREASEOPINION_INFLUENCE_COST influence points. The source may
	not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power
	can be a secondary target for this action. Your current opinion must be less than friendly
	*/
	if(!state.world.nation_get_is_great_power(source) || state.world.nation_get_is_great_power(influence_target))
		return false;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel)
		return false;

	if(state.world.gp_relationship_get_influence(rel) < state.defines.increaseopinion_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel == nations::influence::level_friendly)
		return false;

	return true;
}
void execute_increase_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {
	if(!can_increase_opinion(state, source, influence_target))
		return;

	/*
	Increasing the opinion of a nation costs define:INCREASEOPINION_INFLUENCE_COST influence points. Opinion can be increased to a
	maximum of friendly.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);

	state.world.gp_relationship_get_influence(rel) -= state.defines.increaseopinion_influence_cost;
	auto& l = state.world.gp_relationship_get_status(rel);
	l = nations::influence::increase_level(l);
}

void decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::decrease_opinion;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	p.data.influence_action.gp_target = affected_gp;
	auto b = state.incoming_commands.try_push(p);
}
bool can_decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:DECREASEOPINION_INFLUENCE_COST influence points. The source may
	not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power
	can be a secondary target for this action. Decreasing the opinion of another nation requires that you have an opinion of at
	least "opposed" with the influenced nation and you must have a an equal or better opinion level with the influenced nation
	than the nation you are lowering their opinion of does. The secondary target must neither have the influenced nation in sphere
	nor may it already be at hostile opinion with them.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) ||
			state.world.nation_get_is_great_power(influence_target))
		return false;

	if(source == affected_gp)
		return false;

	if(state.world.nation_get_in_sphere_of(influence_target) == affected_gp)
		return false;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel)
		return false;

	if(state.world.gp_relationship_get_influence(rel) < state.defines.decreaseopinion_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel == nations::influence::level_hostile)
		return false;

	if((nations::influence::level_mask &
				 state.world.gp_relationship_get_status(state.world.get_gp_relationship_by_gp_influence_pair(influence_target,
						 affected_gp))) == nations::influence::level_hostile)
		return false;

	return nations::influence::is_influence_level_greater_or_equal(clevel,
			nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	/*
	Decreasing the opinion of a nation "increases" your relationship with them by define:DECREASEOPINION_RELATION_ON_ACCEPT. This
	actions costs define:DECREASEOPINION_INFLUENCE_COST influence points. Opinion of the influenced nation of the secondary target
	decreases by one step.
	*/
	if(!can_decrease_opinion(state, source, influence_target, affected_gp))
		return;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	if(!orel)
		orel = state.world.force_create_gp_relationship(influence_target, affected_gp);

	state.world.gp_relationship_get_influence(rel) -= state.defines.decreaseopinion_influence_cost;
	nations::adjust_relationship(state, source, affected_gp, state.defines.decreaseopinion_relation_on_accept);

	auto& l = state.world.gp_relationship_get_status(orel);
	l = nations::influence::decrease_level(l);
}

void add_to_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::add_to_sphere;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_add_to_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {
	/*
	The source must be a great power. The source must have define:ADDTOSPHERE_INFLUENCE_COST influence points. The source may not
	be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can
	be a secondary target for this action. The nation must have a friendly opinion of you and my not be in the sphere of another
	nation.
	*/
	if(!state.world.nation_get_is_great_power(source) || state.world.nation_get_is_great_power(influence_target))
		return false;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel)
		return false;

	if(state.world.gp_relationship_get_influence(rel) < state.defines.addtosphere_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel != nations::influence::level_friendly)
		return false;

	if(state.world.nation_get_in_sphere_of(influence_target))
		return false;

	return true;
}
void execute_add_to_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {
	if(!can_add_to_sphere(state, source, influence_target))
		return;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);

	state.world.gp_relationship_get_influence(rel) -= state.defines.addtosphere_influence_cost;
	auto& l = state.world.gp_relationship_get_status(rel);
	l = nations::influence::increase_level(l);

	state.world.nation_set_in_sphere_of(influence_target, source);
}

void remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::remove_from_sphere;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	p.data.influence_action.gp_target = affected_gp;
	auto b = state.incoming_commands.try_push(p);
}
bool can_remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. The source may
	not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power
	can be a secondary target for this action. To preform this action you must have an opinion level of friendly with the nation
	you are removing from a sphere.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) ||
			state.world.nation_get_is_great_power(influence_target))
		return false;

	if(state.world.nation_get_in_sphere_of(influence_target) != affected_gp)
		return false;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel)
		return false;

	if(state.world.gp_relationship_get_influence(rel) < state.defines.removefromsphere_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel != nations::influence::level_friendly || clevel != nations::influence::level_in_sphere)
		return false;

	return true;
}
void execute_remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target,
		dcon::nation_id affected_gp) {
	/*
	Removing a nation from a sphere costs define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. If you remove a nation from
	your own sphere you lose define:REMOVEFROMSPHERE_PRESTIGE_COST prestige and gain define:REMOVEFROMSPHERE_INFAMY_COST infamy.
	Removing a nation from the sphere of another nation "increases" your relationship with the former sphere leader by
	define:REMOVEFROMSPHERE_RELATION_ON_ACCEPT points. The removed nation then becomes friendly with its former sphere leader.
	*/
	if(!can_remove_from_sphere(state, source, influence_target, affected_gp))
		return;

	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);

	state.world.gp_relationship_get_influence(rel) -= state.defines.removefromsphere_influence_cost;

	state.world.nation_set_in_sphere_of(influence_target, dcon::nation_id{});

	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	auto& l = state.world.gp_relationship_get_status(orel);
	l = nations::influence::decrease_level(l);

	if(source != affected_gp)
		nations::adjust_relationship(state, source, affected_gp, state.defines.removefromsphere_relation_on_accept);
	else {
		state.world.nation_get_infamy(source) += state.defines.removefromsphere_infamy_cost;
		nations::adjust_prestige(state, source, -state.defines.removefromsphere_prestige_cost);
	}
}

void upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::upgrade_colony_to_state;
	p.source = source;
	p.data.generic_location.prov = state.world.state_instance_get_capital(si);
	auto b = state.incoming_commands.try_push(p);
}
bool can_upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si) {
	return state.world.state_instance_get_nation_from_state_ownership(si) == source && province::can_integrate_colony(state, si);
}
void execute_upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si) {
	if(!can_upgrade_colony_to_state(state, source, si))
		return;

	province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
		// Provinces in the state stop being colonial.
		state.world.province_set_is_colonial(p, false);

		// All timed modifiers active for provinces in the state expire
		auto timed_modifiers = state.world.province_get_current_modifiers(p);
		for(uint32_t i = timed_modifiers.size(); i-- > 0;) {
			if(bool(timed_modifiers[i].expiration)) {
				timed_modifiers.remove_at(i);
			}
		}
	});

	// Gain define:COLONY_TO_STATE_PRESTIGE_GAIN x(1.0 + colony - prestige - from - tech) x(1.0 + prestige - from - tech)
	nations::adjust_prestige(state, source,
			state.defines.colony_to_state_prestige_gain *
					(1.0f + state.world.nation_get_modifier_values(source, sys::national_mod_offsets::colonial_prestige)));

	// An event from `on_colony_to_state` happens(with the state in scope)
	event::fire_fixed_event(state, state.national_definitions.on_colony_to_state, trigger::to_generic(si), event::slot_type::state,
			source, -1, event::slot_type::none);

	// An event from `on_colony_to_state_free_slaves` happens(with the state in scope)
	event::fire_fixed_event(state, state.national_definitions.on_colony_to_state_free_slaves, trigger::to_generic(si),
			event::slot_type::state, source, -1, event::slot_type::none);

	// Update is colonial nation
	state.world.nation_set_is_colonial_nation(source, false);
	for(auto p : state.world.nation_get_province_ownership(source)) {
		if(p.get_province().get_is_colonial()) {
			state.world.nation_set_is_colonial_nation(source, true);
			return;
		}
	}
}

void invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id pr) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::invest_in_colony;
	p.source = source;
	p.data.generic_location.prov = pr;
	auto b = state.incoming_commands.try_push(p);
}
bool can_invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(p);
	if(!province::is_colonizing(state, source, state_def))
		return province::can_start_colony(state, source, state_def);
	else
		return province::can_invest_in_colony(state, source, state_def);
}
void execute_invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id pr) {
	if(!can_invest_in_colony(state, source, pr))
		return;

	auto state_def = state.world.province_get_state_from_abstract_state_membership(pr);
	if(province::is_colonizing(state, source, state_def)) {
		uint8_t greatest_other_level = 0;
		dcon::nation_id second_colonizer;
		for(auto rel : state.world.state_definition_get_colonization(state_def)) {
			if(rel.get_colonizer() != source) {
				if(rel.get_level() >= greatest_other_level) {
					greatest_other_level = rel.get_level();
					second_colonizer = rel.get_colonizer();
				}
			}
		}

		for(auto rel : state.world.state_definition_get_colonization(state_def)) {
			if(rel.get_colonizer() == source) {

				if(state.world.state_definition_get_colonization_stage(state_def) == 1) {
					rel.get_points_invested() += uint16_t(state.defines.colonization_interest_cost);
				} else if(rel.get_level() <= 4) {
					rel.get_points_invested() += uint16_t(state.defines.colonization_influence_cost);
				} else {
					rel.get_points_invested() += uint16_t(
							state.defines.colonization_extra_guard_cost * (rel.get_level() - 4) + state.defines.colonization_influence_cost);
				}

				rel.get_level() += uint8_t(1);
				rel.set_last_investment(state.current_date);

				/*
				If you get define:COLONIZATION_INTEREST_LEAD points it moves into phase 2, kicking out all but the second-most
				colonizer (in terms of points). In phase 2 if you get define:COLONIZATION_INFLUENCE_LEAD points ahead of the other
				colonizer, the other colonizer is kicked out and the phase moves to 3.
				*/
				if(state.world.state_definition_get_colonization_stage(state_def) == 1) {
					if(rel.get_level() >= int32_t(state.defines.colonization_interest_lead)) {

						state.world.state_definition_set_colonization_stage(state_def, uint8_t(2));
						auto col_range = state.world.state_definition_get_colonization(state_def);
						while(int32_t(col_range.end() - col_range.begin()) > 2) {
							for(auto r : col_range) {
								if(r.get_colonizer() != source && r.get_colonizer() != second_colonizer) {
									state.world.delete_colonization(r);
									break;
								}
							}
						}
					}
				} else if(rel.get_level() >= int32_t(state.defines.colonization_interest_lead) + greatest_other_level) {
					state.world.state_definition_set_colonization_stage(state_def, uint8_t(3));
					auto col_range = state.world.state_definition_get_colonization(state_def);
					while(int32_t(col_range.end() - col_range.begin()) > 1) {
						for(auto r : col_range) {
							if(r.get_colonizer() != source) {
								state.world.delete_colonization(r);
								break;
							}
						}
					}
				}
				return;
			}
		}

	} else {
		bool adjacent = [&]() {
			for(auto p : state.world.state_definition_get_abstract_state_membership(state_def)) {
				if(!p.get_province().get_nation_from_province_ownership()) {
					for(auto adj : p.get_province().get_province_adjacency()) {
						auto indx = adj.get_connected_provinces(0) != p.get_province() ? 0 : 1;
						auto o = adj.get_connected_provinces(indx).get_nation_from_province_ownership();
						if(o == source)
							return true;
						if(o.get_overlord_as_subject().get_ruler() == source)
							return true;
					}
				}
			}
			return false;
		}();

		auto new_rel = fatten(state.world, state.world.force_create_colonization(state_def, source));
		new_rel.set_level(uint8_t(1));
		new_rel.set_last_investment(state.current_date);
		new_rel.set_points_invested(uint16_t(state.defines.colonization_interest_cost_initial +
																				 (adjacent ? state.defines.colonization_interest_cost_neighbor_modifier : 0.0f)));

		state.world.state_definition_set_colonization_stage(state_def, uint8_t(1));
	}
}

void abandon_colony(sys::state& state, dcon::nation_id source, dcon::province_id pr) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::abandon_colony;
	p.source = source;
	p.data.generic_location.prov = pr;
	auto b = state.incoming_commands.try_push(p);
}

bool can_abandon_colony(sys::state& state, dcon::nation_id source, dcon::province_id pr) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(pr);
	return province::is_colonizing(state, source, state_def);
}

void execute_abandon_colony(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(p);

	for(auto rel : state.world.state_definition_get_colonization(state_def)) {
		if(rel.get_colonizer() == source) {
			state.world.delete_colonization(rel);
		}
	}
}

void finish_colonization(sys::state& state, dcon::nation_id source, dcon::province_id pr) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::finish_colonization;
	p.source = source;
	p.data.generic_location.prov = pr;
	auto b = state.incoming_commands.try_push(p);
}
bool can_finish_colonization(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(p);
	if(state.world.state_definition_get_colonization_stage(state_def) != 3)
		return false;
	auto rng = state.world.state_definition_get_colonization(state_def);
	if(rng.begin() == rng.end())
		return false;
	return (*rng.begin()).get_colonizer() == source;
}
void execute_finish_colonization(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	if(!can_finish_colonization(state, source, p))
		return;

	auto state_def = state.world.province_get_state_from_abstract_state_membership(p);

	for(auto pr : state.world.state_definition_get_abstract_state_membership(state_def)) {
		if(!pr.get_province().get_nation_from_province_ownership()) {
			province::change_province_owner(state, pr.get_province(), source);
		}
	}

	state.world.state_definition_set_colonization_temperature(state_def, 0.0f);
	state.world.state_definition_set_colonization_stage(state_def, uint8_t(0));

	auto rng = state.world.state_definition_get_colonization(state_def);
	;
	while(rng.begin() != rng.end()) {
		state.world.delete_colonization(*rng.begin());
	}
}

void intervene_in_war(sys::state& state, dcon::nation_id source, dcon::war_id w, bool for_attacker) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::intervene_in_war;
	p.source = source;
	p.data.war_target.war = w;
	p.data.war_target.for_attacker = for_attacker;
	auto b = state.incoming_commands.try_push(p);
}
bool can_intervene_in_war(sys::state& state, dcon::nation_id source, dcon::war_id w, bool for_attacker) {
	/*
	Must be a great power. Must not be involved in or interested in a crisis. Must be at least define:MIN_MONTHS_TO_INTERVENE
	since the war started.
	*/
	if(!nations::is_great_power(state, source))
		return false;
	if(nations::is_involved_in_crisis(state, source))
		return false;
	if(state.current_date < state.world.war_get_start_date(w) + int32_t(30.0f * state.defines.min_months_to_intervene))
		return false;

	/*
	Standard war-joining conditions: can't join if you are already at war against any attacker or defender. Can't join a war
	against your sphere leader or overlord (doesn't apply here obviously). Can't join a crisis war prior to great wars being
	invented (i.e. you have to be in the crisis). Can't join as an attacker against someone you have a truce with.
	*/
	if(!military::joining_war_does_not_violate_constraints(state, source, w, for_attacker))
		return false;
	if(state.world.war_get_is_crisis_war(w) && !state.world.war_get_is_great(w))
		return false;
	if(for_attacker && military::joining_as_attacker_would_break_truce(state, source, w))
		return false;

	if(!state.world.war_get_is_great(w)) {
		/*
		If it is not a great war:
		Must be at least friendly with the primary defender. May only join on the defender's side. Defenders must either have no
		wargoals or only status quo. Primary defender must be at defines:MIN_WARSCORE_TO_INTERVENE or less.
		*/

		if(for_attacker)
			return false;
		if(military::defenders_have_non_status_quo_wargoal(state, w))
			return false;
		if(military::primary_warscore(state, w) < -state.defines.min_warscore_to_intervene)
			return false;
	} else {
		/*
		If the war is a great war:
		It is then possible to join the attacking side as well.
		Must have define:GW_INTERVENE_MIN_RELATIONS with the primary defender/attacker to intervene, must have at most
		define:GW_INTERVENE_MAX_EXHAUSTION war exhaustion. Can't join if any nation in your sphere is on the other side Can't join
		if you are allied to any allied to any nation on the other side Can't join if you have units within a nation on the other
		side
		*/
		if(state.world.nation_get_war_exhaustion(source) >= state.defines.gw_intervene_max_exhaustion)
			return false;

		auto primary_on_side = for_attacker ? state.world.war_get_primary_attacker(w) : state.world.war_get_primary_defender(w);
		auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(primary_on_side, source);
		if(state.world.diplomatic_relation_get_value(rel) < state.defines.gw_intervene_min_relations)
			return false;

		for(auto p : state.world.war_get_war_participant(w)) {
			if(p.get_is_attacker() != for_attacker) { // scan nations on other side
				if(p.get_nation().get_in_sphere_of() == source)
					return false;

				auto irel = state.world.get_diplomatic_relation_by_diplomatic_pair(p.get_nation(), source);
				if(state.world.diplomatic_relation_get_are_allied(irel))
					return false;

				for(auto prov : p.get_nation().get_province_ownership()) {
					for(auto arm : prov.get_province().get_army_location()) {
						if(arm.get_army().get_controller_from_army_control() == source)
							return false;
					}
				}
			}
		}
	}
	return false;
}
void execute_intervene_in_war(sys::state& state, dcon::nation_id source, dcon::war_id w, bool for_attacker) {
	if(!can_intervene_in_war(state, source, w, for_attacker))
		return;

	military::join_war(state, w, source, for_attacker);

	if(!state.world.war_get_is_great(w)) {
		bool status_quo_added = false;
		for(auto wg : state.world.war_get_wargoals_attached(w)) {
			if(military::is_defender_wargoal(state, w, wg.get_wargoal()) &&
					(wg.get_wargoal().get_type().get_type_bits() & military::cb_flag::po_status_quo) != 0) {
				status_quo_added = true;
				break;
			}
		}
		if(!status_quo_added) {
			dcon::cb_type_id status_quo;
			for(auto c : state.world.in_cb_type) {
				if((c.get_type_bits() & military::cb_flag::po_status_quo) != 0) {
					status_quo = c;
					break;
				}
			}
			assert(status_quo);
			military::add_wargoal(state, w, source, state.world.war_get_primary_attacker(w), status_quo, dcon::state_definition_id{},
					dcon::national_identity_id{}, dcon::nation_id{});
		}
	}
}

void suppress_movement(sys::state& state, dcon::nation_id source, dcon::movement_id m) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::intervene_in_war;
	p.source = source;
	p.data.movement.iopt = state.world.movement_get_associated_issue_option(m);
	p.data.movement.tag = state.world.movement_get_associated_independence(m);
	auto b = state.incoming_commands.try_push(p);
}
bool can_suppress_movement(sys::state& state, dcon::nation_id source, dcon::movement_id m) {
	if(state.world.movement_get_nation_from_movement_within(m) != source)
		return false;
	return state.world.nation_get_suppression_points(source) >= rebel::get_suppression_point_cost(state, m);
}
void execute_suppress_movement(sys::state& state, dcon::nation_id source, dcon::issue_option_id iopt,
		dcon::national_identity_id tag) {
	dcon::movement_id m;
	if(iopt) {
		m = rebel::get_movement_by_position(state, source, iopt);
	} else if(tag) {
		m = rebel::get_movement_by_independence(state, source, tag);
	}
	if(!m)
		return;
	if(!can_suppress_movement(state, source, m))
		return;

	state.world.nation_get_suppression_points(source) -= rebel::get_suppression_point_cost(state, m);
	rebel::suppress_movement(state, source, m);
}

void civilize_nation(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::civilize_nation;
	p.source = source;
	auto b = state.incoming_commands.try_push(p);
}
bool can_civilize_nation(sys::state& state, dcon::nation_id source) {
	return state.world.nation_get_modifier_values(source, sys::national_mod_offsets::civilization_progress_modifier) >= 1.0f;
}
void execute_civilize_nation(sys::state& state, dcon::nation_id source) {
	if(!can_civilize_nation(state, source))
		return;
	nations::make_civilized(state, source);
}

void appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id pa) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::appoint_ruling_party;
	p.source = source;
	p.data.political_party.p = pa;
	auto b = state.incoming_commands.try_push(p);
}
bool can_appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id p) {
	/*
	The ideology of the ruling party must be permitted by the government form. There can't be an ongoing election. It can't be the
	current ruling party. The government must allow the player to set the ruling party. The ruling party can manually be changed
	at most once per year.
	*/
	if(state.world.nation_get_ruling_party(source) == p)
		return false;

	if(!politics::can_appoint_ruling_party(state, source))
		return false;

	auto last_change = state.world.nation_get_ruling_party_last_appointed(source);
	if(last_change && state.current_date < last_change + 365)
		return false;

	if(politics::is_election_ongoing(state, source))
		return false;

	auto gov = state.world.nation_get_government_type(source);
	auto new_ideology = state.world.political_party_get_ideology(p);
	if((state.culture_definitions.governments[gov].ideologies_allowed & ::culture::to_bits(new_ideology)) == 0) {
		return false;
	}

	return true;
}
void execute_appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id p) {
	if(!can_appoint_ruling_party(state, source, p))
		return;

	politics::appoint_ruling_party(state, source, p);
}

void enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_reform_option;
	p.source = source;
	p.data.reform_selection.r = r;
	auto b = state.incoming_commands.try_push(p);
}
bool can_enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r) {
	bool is_military = state.world.reform_get_reform_type(state.world.reform_option_get_parent_reform(r)) ==
										 uint8_t(culture::issue_category::military);
	if(is_military)
		return politics::can_enact_military_reform(state, source, r);
	else
		return politics::can_enact_economic_reform(state, source, r);
}
void execute_enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r) {
	if(!can_enact_reform(state, source, r))
		return;

	/*
	For military/economic reforms:
	- Run the `on_execute` member
	*/
	auto e = state.world.reform_option_get_on_execute_effect(r);
	if(e) {
		auto t = state.world.reform_option_get_on_execute_trigger(r);
		if(!t || trigger::evaluate(state, t, trigger::to_generic(source), trigger::to_generic(source), 0))
			effect::execute(state, e, trigger::to_generic(source), trigger::to_generic(source), 0, uint32_t(state.current_date.value),
					uint32_t(source.index()));
	}

	// - Subtract research points (see discussion of when the reform is possible for how many)
	bool is_military = state.world.reform_get_reform_type(state.world.reform_option_get_parent_reform(r)) ==
										 uint8_t(culture::issue_category::military);
	if(is_military) {
		float base_cost = float(state.world.reform_option_get_technology_cost(r));
		float reform_factor = politics::get_military_reform_multiplier(state, source);

		state.world.nation_get_research_points(source) -= base_cost * reform_factor;
	} else {
		float base_cost = float(state.world.reform_option_get_technology_cost(r));
		float reform_factor = politics::get_economic_reform_multiplier(state, source);

		state.world.nation_get_research_points(source) -= base_cost * reform_factor;
	}

	/*
	In general:
	- Increase the share of conservatives in the upper house by defines:CONSERVATIVE_INCREASE_AFTER_REFORM (and then normalize
	again)
	*/

	for(auto id : state.world.in_ideology) {
		if(id == state.culture_definitions.conservative) {
			state.world.nation_get_upper_house(source, id) += state.defines.conservative_increase_after_reform * 100.0f;
		}
		state.world.nation_get_upper_house(source, id) /= (1.0f + state.defines.conservative_increase_after_reform);
	}
	state.world.nation_set_reforms(source, state.world.reform_option_get_parent_reform(r), r);

	culture::update_nation_issue_rules(state, source);
	sys::update_single_nation_modifiers(state, source);
}

void enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_issue_option;
	p.source = source;
	p.data.issue_selection.r = i;
	auto b = state.incoming_commands.try_push(p);
}
bool can_enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i) {
	auto type = state.world.issue_get_issue_type(state.world.issue_option_get_parent_issue(i));
	if(type == uint8_t(culture::issue_type::political))
		return politics::can_enact_political_reform(state, source, i);
	else if(type == uint8_t(culture::issue_type::social))
		return politics::can_enact_social_reform(state, source, i);
	else
		return false;
}
void execute_enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i) {
	if(!can_enact_issue(state, source, i))
		return;

	auto e = state.world.issue_option_get_on_execute_effect(i);
	if(e) {
		auto t = state.world.issue_option_get_on_execute_trigger(i);
		if(!t || trigger::evaluate(state, t, trigger::to_generic(source), trigger::to_generic(source), 0))
			effect::execute(state, e, trigger::to_generic(source), trigger::to_generic(source), 0, uint32_t(state.current_date.value),
					uint32_t(source.index()));
	}

	/*
	For political and social based reforms:
	- Every issue-based movement with greater popular support than the movement supporting the given issue (if there is such a
	movement; all movements if there is no such movement) has its radicalism increased by 3v(support-of-that-movement /
	support-of-movement-behind-issue (or 1 if there is no such movement) - 1.0) x defines:WRONG_REFORM_RADICAL_IMPACT.
	*/
	auto winner = rebel::get_movement_by_position(state, source, i);
	float winner_support = winner ? state.world.movement_get_pop_support(winner) : 1.0f;
	for(auto m : state.world.nation_get_movement_within(source)) {
		if(m.get_movement().get_associated_issue_option() && m.get_movement().get_associated_issue_option() != i &&
				m.get_movement().get_pop_support() > winner_support) {

			m.get_movement().get_transient_radicalism() +=
					std::min(3.0f, m.get_movement().get_pop_support() / winner_support - 1.0f) * state.defines.wrong_reform_radical_impact;
		}
	}

	/*
	- For every ideology, the pop gains defines:MIL_REFORM_IMPACT x pop-support-for-that-ideology x ideology's support for doing
	the opposite of the reform (calculated in the same way as determining when the upper house will support the reform or repeal)
	militancy
	*/
	auto issue = state.world.issue_option_get_parent_issue(i);
	auto current = state.world.nation_get_issues(source, issue.id).id;
	bool is_social = state.world.issue_get_issue_type(issue) == uint8_t(culture::issue_category::social);

	for(auto id : state.world.in_ideology) {

		auto condition = is_social ? (i.index() > current.index() ? state.world.ideology_get_remove_social_reform(id)
																															: state.world.ideology_get_add_social_reform(id))
															 : (i.index() > current.index() ? state.world.ideology_get_remove_political_reform(id)
																															: state.world.ideology_get_add_political_reform(id));
		if(condition) {
			auto factor =
					trigger::evaluate_additive_modifier(state, condition, trigger::to_generic(source), trigger::to_generic(source), 0);
			auto const idsupport_key = pop_demographics::to_key(state, id);
			if(factor > 0) {
				for(auto pr : state.world.nation_get_province_ownership(source)) {
					for(auto pop : pr.get_province().get_pop_location()) {
						auto base_mil = pop.get_pop().get_militancy();
						pop.get_pop().set_militancy(base_mil + pop.get_pop().get_demographics(idsupport_key) * factor *
																											 state.defines.mil_reform_impact); // intentionally left to be clamped below
					}
				}
			}
		}
	}

	/*
	- Each pop in the nation gains defines:CON_REFORM_IMPACT x pop support of the issue consciousness

	- If the pop is part of a movement for some other issue (or for independence), it gains defines:WRONG_REFORM_MILITANCY_IMPACT
	militancy. All other pops lose defines:WRONG_REFORM_MILITANCY_IMPACT militancy.
	*/

	auto const isupport_key = pop_demographics::to_key(state, i);
	for(auto pr : state.world.nation_get_province_ownership(source)) {
		for(auto pop : pr.get_province().get_pop_location()) {
			auto base_con = pop.get_pop().get_consciousness();
			auto adj_con = base_con + pop.get_pop().get_demographics(isupport_key) * state.defines.con_reform_impact;
			pop.get_pop().set_consciousness(std::clamp(adj_con, 0.0f, 10.0f));

			if(auto m = pop.get_pop().get_movement_from_pop_movement_membership(); m && m.get_associated_issue_option() != i) {
				auto base_mil = pop.get_pop().get_militancy();
				pop.get_pop().set_militancy(std::clamp(base_mil + state.defines.wrong_reform_militancy_impact, 0.0f, 10.0f));
			} else {
				auto base_mil = pop.get_pop().get_militancy();
				pop.get_pop().set_militancy(std::clamp(base_mil - state.defines.wrong_reform_militancy_impact, 0.0f, 10.0f));
			}
		}
	}

	/*
	In general:
	- Increase the share of conservatives in the upper house by defines:CONSERVATIVE_INCREASE_AFTER_REFORM (and then normalize
	again)
	- If slavery is forbidden (rule slavery_allowed is false), remove all slave states and free all slaves.
	*/
	for(auto id : state.world.in_ideology) {
		if(id == state.culture_definitions.conservative) {
			state.world.nation_get_upper_house(source, id) += state.defines.conservative_increase_after_reform * 100.0f;
		}
		state.world.nation_get_upper_house(source, id) /= (1.0f + state.defines.conservative_increase_after_reform);
	}

	state.world.nation_set_issues(source, issue, i);

	culture::update_nation_issue_rules(state, source);
	sys::update_single_nation_modifiers(state, source);
}

void become_interested_in_crisis(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::become_interested_in_crisis;
	p.source = source;
	auto b = state.incoming_commands.try_push(p);
}
bool can_become_interested_in_crisis(sys::state& state, dcon::nation_id source) {
	/*
	Not already interested in the crisis. Is a great power. Not at war. The crisis must have already gotten its initial backers.
	*/
	if(!nations::is_great_power(state, source))
		return false;

	if(state.world.nation_get_is_at_war(source))
		return false;

	if(state.world.nation_get_disarmed_until(source) && state.current_date < state.world.nation_get_disarmed_until(source))
		return false;

	if(state.current_crisis_mode != sys::crisis_mode::heating_up)
		return false;

	for(auto& i : state.crisis_participants) {
		if(i.id == source)
			return false;
		if(!i.id)
			return true;
	}

	return true;
}
void execute_become_interested_in_crisis(sys::state& state, dcon::nation_id source) {
	if(!can_become_interested_in_crisis(state, source))
		return;

	for(auto& i : state.crisis_participants) {
		if(!i.id) {
			i.id = source;
			i.merely_interested = true;
			return;
		}
	}
}

void take_sides_in_crisis(sys::state& state, dcon::nation_id source, bool join_attacker) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::take_sides_in_crisis;
	p.source = source;
	p.data.crisis_join.join_attackers = join_attacker;
	auto b = state.incoming_commands.try_push(p);
}
bool can_take_sides_in_crisis(sys::state& state, dcon::nation_id source, bool join_attacker) {
	/*
	Must not be involved in the crisis already. Must be interested in the crisis. Must be a great power. Must not be disarmed. The
	crisis must have already gotten its initial backers.
	*/

	if(state.current_crisis_mode != sys::crisis_mode::heating_up)
		return false;

	for(auto& i : state.crisis_participants) {
		if(i.id == source)
			return i.merely_interested == true;
		if(!i.id)
			return false;
	}
	return false;
}
void execute_take_sides_in_crisis(sys::state& state, dcon::nation_id source, bool join_attacker) {
	if(!can_take_sides_in_crisis(state, source, join_attacker))
		return;

	for(auto& i : state.crisis_participants) {
		if(i.id == source) {
			i.merely_interested = false;
			i.supports_attacker = join_attacker;
			return;
		}
		if(!i.id)
			return;
	}
}

bool can_change_stockpile_settings(sys::state& state, dcon::nation_id source, dcon::commodity_id c, float target_amount,
		bool draw_on_stockpiles) {
	return true;
}

void change_stockpile_settings(sys::state& state, dcon::nation_id source, dcon::commodity_id c, float target_amount,
		bool draw_on_stockpiles) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_stockpile_settings;
	p.source = source;
	p.data.stockpile_settings.amount = target_amount;
	p.data.stockpile_settings.c = c;
	p.data.stockpile_settings.draw_on_stockpiles = draw_on_stockpiles;
	auto b = state.incoming_commands.try_push(p);
}

void execute_change_stockpile_settings(sys::state& state, dcon::nation_id source, dcon::commodity_id c, float target_amount,
		bool draw_on_stockpiles) {
	state.world.nation_set_stockpile_targets(source, c, target_amount);
	state.world.nation_set_drawing_on_stockpiles(source, c, draw_on_stockpiles);
}

void take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::take_decision;
	p.source = source;
	p.data.decision.d = d;
	auto b = state.incoming_commands.try_push(p);
}
bool can_take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d) {
	{
		auto condition = state.world.decision_get_potential(d);
		if(condition && !trigger::evaluate(state, condition, trigger::to_generic(source), trigger::to_generic(source), 0))
			return false;
	}
	{
		auto condition = state.world.decision_get_allow(d);
		if(condition && !trigger::evaluate(state, condition, trigger::to_generic(source), trigger::to_generic(source), 0))
			return false;
	}
	return true;
}
void execute_take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d) {
	if(!can_take_decision(state, source, d))
		return;

	if(auto e = state.world.decision_get_effect(d); e)
		effect::execute(state, e, trigger::to_generic(source), trigger::to_generic(source), 0, uint32_t(state.current_date.value),
				uint32_t(source.index() << 4 ^ d.index()));
}

void make_event_choice(sys::state& state, event::pending_human_n_event const& e, uint8_t option_id) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::make_n_event_choice;
	p.source = e.n;
	p.data.pending_human_n_event.date = e.date;
	p.data.pending_human_n_event.e = e.e;
	p.data.pending_human_n_event.from_slot = e.from_slot;
	p.data.pending_human_n_event.ft = e.ft;
	p.data.pending_human_n_event.opt_choice = option_id;
	p.data.pending_human_n_event.primary_slot = e.primary_slot;
	p.data.pending_human_n_event.pt = e.pt;
	p.data.pending_human_n_event.r_hi = e.r_hi;
	p.data.pending_human_n_event.r_lo = e.r_lo;
	auto b = state.incoming_commands.try_push(p);
}
void make_event_choice(sys::state& state, event::pending_human_f_n_event const& e, uint8_t option_id) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::make_f_n_event_choice;
	p.source = e.n;
	p.data.pending_human_f_n_event.date = e.date;
	p.data.pending_human_f_n_event.e = e.e;
	p.data.pending_human_f_n_event.opt_choice = option_id;
	p.data.pending_human_f_n_event.r_hi = e.r_hi;
	p.data.pending_human_f_n_event.r_lo = e.r_lo;
	auto b = state.incoming_commands.try_push(p);
}
void make_event_choice(sys::state& state, event::pending_human_p_event const& e, uint8_t option_id) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::make_p_event_choice;
	p.source = state.world.province_get_nation_from_province_ownership(e.p);
	p.data.pending_human_p_event.date = e.date;
	p.data.pending_human_p_event.e = e.e;
	p.data.pending_human_p_event.p = e.p;
	p.data.pending_human_p_event.from_slot = e.from_slot;
	p.data.pending_human_p_event.ft = e.ft;
	p.data.pending_human_p_event.opt_choice = option_id;
	p.data.pending_human_p_event.r_hi = e.r_hi;
	p.data.pending_human_p_event.r_lo = e.r_lo;
	auto b = state.incoming_commands.try_push(p);
}
void make_event_choice(sys::state& state, event::pending_human_f_p_event const& e, uint8_t option_id) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::make_f_p_event_choice;
	p.source = state.world.province_get_nation_from_province_ownership(e.p);
	p.data.pending_human_f_p_event.date = e.date;
	p.data.pending_human_f_p_event.e = e.e;
	p.data.pending_human_f_p_event.p = e.p;
	p.data.pending_human_f_p_event.opt_choice = option_id;
	p.data.pending_human_f_p_event.r_hi = e.r_hi;
	p.data.pending_human_f_p_event.r_lo = e.r_lo;
	auto b = state.incoming_commands.try_push(p);
}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_n_event_data const& e) {
	event::take_option(state,
			event::pending_human_n_event{e.r_lo, e.r_hi, e.primary_slot, e.from_slot, e.e, source, e.date, e.pt, e.ft},
			e.opt_choice);
}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_f_n_event_data const& e) {
	event::take_option(state, event::pending_human_f_n_event{e.r_lo, e.r_hi, e.e, source, e.date}, e.opt_choice);
}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_p_event_data const& e) {
	if(source != state.world.province_get_nation_from_province_ownership(e.p))
		return;

	event::take_option(state, event::pending_human_p_event{e.r_lo, e.r_hi, e.from_slot, e.e, e.p, e.date, e.ft}, e.opt_choice);
}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_f_p_event_data const& e) {
	if(source != state.world.province_get_nation_from_province_ownership(e.p))
		return;

	event::take_option(state, event::pending_human_f_p_event{e.r_lo, e.r_hi, e.e, e.p, e.date}, e.opt_choice);
}

void fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::fabricate_cb;
	p.source = source;
	p.data.cb_fabrication.target = target;
	p.data.cb_fabrication.type = type;
	auto b = state.incoming_commands.try_push(p);
}
bool can_fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type) {
	if(source == target)
		return false;

	if(state.world.nation_get_constructing_cb_type(source))
		return false;

	/*
	Can't fabricate on someone you are at war with. Can't fabricate on anyone except your overlord if you are a vassal. Requires
	defines:MAKE_CB_DIPLOMATIC_COST diplomatic points. Can't fabricate on your sphere members
	*/

	auto ol = state.world.nation_get_overlord_as_subject(source);
	if(state.world.overlord_get_ruler(ol) && state.world.overlord_get_ruler(ol) != target)
		return false;

	if(state.world.nation_get_in_sphere_of(target) == source)
		return false;

	if(state.world.nation_get_diplomatic_points(source) < state.defines.make_cb_diplomatic_cost)
		return false;

	if(military::are_at_war(state, target, source))
		return false;

	/*
	must be able to fabricate cb
	*/

	if((state.world.cb_type_get_type_bits(type) & (military::cb_flag::always | military::cb_flag::is_not_constructing_cb)) != 0)
		return false;

	if(!military::cb_conditions_satisfied(state, source, target, type))
		return false;

	return true;
}

void execute_fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type) {
	if(!can_fabricate_cb(state, source, target, type))
		return;

	state.world.nation_set_constructing_cb_target(source, target);
	state.world.nation_set_constructing_cb_type(source, type);
	state.world.nation_get_diplomatic_points(source) -= state.defines.make_cb_diplomatic_cost;
}

bool can_cancel_cb_fabrication(sys::state& state, dcon::nation_id source) {
	return true;
}

void cancel_cb_fabrication(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_cb_fabrication;
	p.source = source;
	auto b = state.incoming_commands.try_push(p);
}
void execute_cancel_cb_fabrication(sys::state& state, dcon::nation_id source) {
	state.world.nation_set_constructing_cb_target(source, dcon::nation_id{});
	state.world.nation_set_constructing_cb_is_discovered(source, false);
	state.world.nation_set_constructing_cb_progress(source, 0.0f);
	state.world.nation_set_constructing_cb_type(source, dcon::cb_type_id{});
}

void ask_for_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::ask_for_military_access;
	p.source = asker;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_ask_for_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	/*
	Must have defines:ASKMILACCESS_DIPLOMATIC_COST diplomatic points. Must not be at war against each other. Must not already have
	military access.
	*/
	if(asker == target)
		return false;

	if(state.world.nation_get_diplomatic_points(asker) < state.defines.askmilaccess_diplomatic_cost)
		return false;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, asker);
	if(state.world.unilateral_relationship_get_military_access(rel))
		return false;

	if(military::are_at_war(state, asker, target))
		return false;

	return true;
}
void execute_ask_for_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	if(!can_ask_for_access(state, asker, target))
		return;

	state.world.nation_get_diplomatic_points(asker) -= state.defines.askmilaccess_diplomatic_cost;

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.type = diplomatic_message::type::access_request;

	diplomatic_message::post(state, m);
}

void ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::ask_for_alliance;
	p.source = asker;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	/*
	Must not have an alliance. Must not be in a war against each other. Costs defines:ALLIANCE_DIPLOMATIC_COST diplomatic points.
	Great powers may not form an alliance while there is an active crisis. Vassals and substates may only form an alliance with
	their overlords.
	*/
	if(asker == target)
		return false;

	if(state.world.nation_get_diplomatic_points(asker) < state.defines.alliance_diplomatic_cost)
		return false;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(target, asker);
	if(state.world.diplomatic_relation_get_are_allied(rel))
		return false;

	if(state.world.nation_get_is_great_power(asker) && state.world.nation_get_is_great_power(target) &&
			state.current_crisis != sys::crisis_type::none) {
		return false;
	}

	auto ol = state.world.nation_get_overlord_as_subject(asker);
	if(state.world.overlord_get_ruler(ol) && state.world.overlord_get_ruler(ol) != target)
		return false;

	if(military::are_at_war(state, asker, target))
		return false;

	return true;
}
void execute_ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	if(!can_ask_for_alliance(state, asker, target))
		return;

	state.world.nation_get_diplomatic_points(asker) -= state.defines.alliance_diplomatic_cost;

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.type = diplomatic_message::type::alliance_request;

	diplomatic_message::post(state, m);
}

void call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::call_to_arms;
	p.source = asker;
	p.data.call_to_arms.target = target;
	p.data.call_to_arms.war = w;
	auto b = state.incoming_commands.try_push(p);
}
bool can_call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w) {
	if(asker == target)
		return false;

	if(state.world.nation_get_diplomatic_points(asker) < state.defines.callally_diplomatic_cost)
		return false;

	if(military::is_civil_war(state, w))
		return false;

	if(!military::standard_war_joining_is_possible(state, w, target, military::is_attacker(state, w, target)))
		return false;

	if(state.world.war_get_is_crisis_war(w) && !state.military_definitions.great_wars_enabled)
		return false;

	return true;
}
void execute_call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w) {
	if(!can_call_to_arms(state, asker, target, w))
		return;

	state.world.nation_get_diplomatic_points(asker) -= state.defines.callally_diplomatic_cost;

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.data.war = w;
	m.type = diplomatic_message::type::call_ally_request;

	diplomatic_message::post(state, m);
}

void respond_to_diplomatic_message(sys::state& state, dcon::nation_id source, dcon::nation_id from, diplomatic_message::type type,
		bool accept) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::respond_to_diplomatic_message;
	p.source = source;
	p.data.message.accept = accept;
	p.data.message.from = from;
	p.data.message.type = type;
	auto b = state.incoming_commands.try_push(p);
}
void execute_respond_to_diplomatic_message(sys::state& state, dcon::nation_id source, dcon::nation_id from,
		diplomatic_message::type type, bool accept) {
	for(auto& m : state.pending_messages) {
		if(m.type == type && m.from == from && m.to == source) {

			if(accept)
				diplomatic_message::accept(state, m);
			else
				diplomatic_message::decline(state, m);

			m.type = diplomatic_message::type::none;

			return;
		}
	}
}

void cancel_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_military_access;
	p.source = source;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_cancel_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(source == target)
		return false;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(state.world.nation_get_diplomatic_points(source) < state.defines.cancelaskmilaccess_diplomatic_cost)
		return false;

	if(state.world.unilateral_relationship_get_military_access(rel))
		return true;
	else
		return false;
}
void execute_cancel_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_cancel_military_access(state, source, target))
		return;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	state.world.unilateral_relationship_set_military_access(rel, false);

	state.world.nation_get_diplomatic_points(source) -= state.defines.cancelaskmilaccess_diplomatic_cost;
	nations::adjust_relationship(state, source, target, state.defines.cancelaskmilaccess_relation_on_accept);
}

void cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_given_military_access;
	p.source = source;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(source, target);

	if(state.world.nation_get_diplomatic_points(source) < state.defines.cancelgivemilaccess_diplomatic_cost)
		return false;

	if(state.world.unilateral_relationship_get_military_access(rel))
		return true;
	else
		return false;
}
void execute_cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_cancel_given_military_access(state, source, target))
		return;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(source, target);
	state.world.unilateral_relationship_set_military_access(rel, false);

	state.world.nation_get_diplomatic_points(source) -= state.defines.cancelgivemilaccess_diplomatic_cost;
	nations::adjust_relationship(state, source, target, state.defines.cancelgivemilaccess_relation_on_accept);
}

void cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_alliance;
	p.source = source;
	p.data.diplo_action.target = target;
	auto b = state.incoming_commands.try_push(p);
}
bool can_cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(source == target)
		return false;

	if(state.world.nation_get_diplomatic_points(source) < state.defines.cancelalliance_diplomatic_cost)
		return false;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(target, source);
	if(!state.world.diplomatic_relation_get_are_allied(rel))
		return false;

	if(military::are_allied_in_war(state, source, target))
		return false;

	return true;
}
void execute_cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_cancel_alliance(state, source, target))
		return;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(source, target);
	state.world.diplomatic_relation_set_are_allied(rel, false);

	state.world.nation_get_diplomatic_points(source) -= state.defines.cancelalliance_diplomatic_cost;
	nations::adjust_relationship(state, source, target, state.defines.cancelalliance_relation_on_accept);
}

void declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {

	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::declare_war;
	p.source = source;
	p.data.new_war.target = target;
	p.data.new_war.primary_cb = primary_cb;
	p.data.new_war.cb_state = cb_state;
	p.data.new_war.cb_tag = cb_tag;
	p.data.new_war.cb_secondary_nation = cb_secondary_nation;
	auto b = state.incoming_commands.try_push(p);
}

bool can_declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {
	dcon::nation_id real_target = target;

	auto target_ol_rel = state.world.nation_get_overlord_as_subject(target);
	if(state.world.overlord_get_ruler(target_ol_rel))
		real_target = state.world.overlord_get_ruler(target_ol_rel);

	if(source == target || source == real_target)
		return false;

	if(military::are_allied_in_war(state, source, real_target) || military::are_at_war(state, source, real_target))
		return false;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(real_target, source);
	if(state.world.diplomatic_relation_get_are_allied(rel))
		return false;

	auto source_ol_rel = state.world.nation_get_overlord_as_subject(source);
	if(state.world.overlord_get_ruler(source_ol_rel) && state.world.overlord_get_ruler(source_ol_rel) != real_target)
		return false;

	if(state.world.nation_get_in_sphere_of(real_target) == source)
		return false;

	if(state.world.nation_get_diplomatic_points(source) < state.defines.declarewar_diplomatic_cost)
		return false;

	// check CB validity
	if(!military::cb_instance_conditions_satisfied(state, source, target, primary_cb, cb_state, cb_tag, cb_secondary_nation))
		return false;

	return true;
}

void execute_declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {

	if(!can_declare_war(state, source, target, primary_cb, cb_state, cb_tag, cb_secondary_nation)) {
		return;
	}

	state.world.nation_get_diplomatic_points(source) -= state.defines.declarewar_diplomatic_cost;
	nations::adjust_relationship(state, source, target, state.defines.declarewar_relation_on_accept);

	dcon::nation_id real_target = target;

	auto target_ol_rel = state.world.nation_get_overlord_as_subject(target);
	if(state.world.overlord_get_ruler(target_ol_rel))
		real_target = state.world.overlord_get_ruler(target_ol_rel);

	if(military::has_truce_with(state, source, real_target)) {
		auto cb_infamy = military::truce_break_cb_infamy(state, primary_cb);
		auto cb_militancy = military::truce_break_cb_militancy(state, primary_cb);
		auto cb_prestige_loss = military::truce_break_cb_prestige_cost(state, primary_cb);

		state.world.nation_get_infamy(source) += cb_infamy;
		nations::adjust_prestige(state, source, cb_prestige_loss);

		for(auto prov : state.world.nation_get_province_ownership(source)) {
			for(auto pop : prov.get_province().get_pop_location()) {
				auto& mil = pop.get_pop().get_militancy();
				mil = std::min(mil + cb_militancy, 10.0f);
			}
		}
	}

	// remove used cb
	auto current_cbs = state.world.nation_get_available_cbs(source);
	for(uint32_t i = current_cbs.size(); i-- > 0;) {
		if(current_cbs[i].cb_type == primary_cb && current_cbs[i].target == target) {
			current_cbs.remove_at(i);
			break;
		}
	}

	auto war = military::create_war(state, source, target, primary_cb, cb_state, cb_tag, cb_secondary_nation);
	military::call_defender_allies(state, war);
}

void add_war_goal(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target, dcon::cb_type_id cb_type,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::add_war_goal;
	p.source = source;
	p.data.new_war_goal.target = target;
	p.data.new_war_goal.cb_type = cb_type;
	p.data.new_war_goal.cb_state = cb_state;
	p.data.new_war_goal.cb_tag = cb_tag;
	p.data.new_war_goal.cb_secondary_nation = cb_secondary_nation;
	p.data.new_war_goal.war = w;
	auto b = state.incoming_commands.try_push(p);
}
bool can_add_war_goal(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target, dcon::cb_type_id cb_type,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {
	/*
	The nation adding the war goal must have positive war score against the target of the war goal (see below). And the nation
	must be already able to use the CB in question (e.g. it as fabricated previously) or it must be a constructible CB and the
	nation adding the war goal must have overall jingoism support >= defines:WARGOAL_JINGOISM_REQUIREMENT (x
	defines:GW_JINGOISM_REQUIREMENT_MOD in a great war).
	*/
	if(source == target)
		return false;

	if(state.world.nation_get_diplomatic_points(source) < state.defines.addwargoal_diplomatic_cost)
		return false;

	bool is_attacker = military::is_attacker(state, w, source);
	bool target_in_war = false;

	for(auto par : state.world.war_get_war_participant(w)) {
		if(par.get_nation() == target) {
			if(par.get_is_attacker() == is_attacker)
				return false;
			target_in_war = true;
			break;
		}
	}

	if(!target_in_war)
		return false;

	if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::always) == 0) {
		bool cb_fabbed = false;
		for(auto& fab_cb : state.world.nation_get_available_cbs(source)) {
			if(fab_cb.cb_type == cb_type && fab_cb.target == target) {
				cb_fabbed = true;
				break;
			}
		}
		if(!cb_fabbed) {
			if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::is_not_constructing_cb) == 0)
				return false; // can only add a constructable cb this way

			if(state.world.war_get_is_great(w)) {
				if(state.world.nation_get_demographics(source, demographics::to_key(state, state.culture_definitions.jingoism)) <
						state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod)
					return false;
			} else {
				if(state.world.nation_get_demographics(source, demographics::to_key(state, state.culture_definitions.jingoism)) <
						state.defines.wargoal_jingoism_requirement)
					return false;
			}
		}
	}
	if(!military::cb_instance_conditions_satisfied(state, source, target, cb_type, cb_state, cb_tag, cb_secondary_nation))
		return false;

	return true;
}
void execute_add_war_goal(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target,
		dcon::cb_type_id cb_type, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag,
		dcon::nation_id cb_secondary_nation) {

	if(!can_add_war_goal(state, source, w, target, cb_type, cb_state, cb_tag, cb_secondary_nation))
		return;

	state.world.nation_get_diplomatic_points(source) -= state.defines.addwargoal_diplomatic_cost;
	nations::adjust_relationship(state, source, target, state.defines.addwargoal_relation_on_accept);

	float infamy = military::cb_addition_infamy_cost(state, w, cb_type, source, target);
	state.world.nation_get_infamy(source) += infamy;

	military::add_wargoal(state, w, source, target, cb_type, cb_state, cb_tag, cb_secondary_nation);
}

void switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::switch_nation;
	p.source = source;
	p.data.tag_target.ident = t;
	auto b = state.incoming_commands.try_push(p);
}
bool can_switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	dcon::nation_id n = state.world.national_identity_get_nation_from_identity_holder(t);
	if(state.world.nation_get_is_player_controlled(n))
		return false;
	return true;
}
void execute_switch_nation(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	if(!can_switch_nation(state, source, t))
		return;

	state.world.nation_set_is_player_controlled(state.local_player_nation, false);
	if(source == state.local_player_nation) {
		state.local_player_nation = state.world.national_identity_get_nation_from_identity_holder(t);
	}
	state.world.nation_set_is_player_controlled(state.local_player_nation, true);
}

void start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war, bool is_concession) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::start_peace_offer;
	p.source = source;
	p.data.new_offer.target = target;
	p.data.new_offer.war = war;
	p.data.new_offer.is_concession = is_concession;
	auto b = state.incoming_commands.try_push(p);
}
bool can_start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war,
		bool is_concession) {

	{
		auto ol = state.world.nation_get_overlord_as_subject(source);
		if(state.world.overlord_get_ruler(ol))
			return false;
	}
	{
		auto ol = state.world.nation_get_overlord_as_subject(target);
		if(state.world.overlord_get_ruler(ol))
			return false;
	}

	if(state.world.war_get_primary_attacker(war) == source) {
		if(military::get_role(state, war, target) != military::war_role::defender)
			return false;
	} else if(state.world.war_get_primary_defender(war) == source) {
		if(military::get_role(state, war, target) != military::war_role::attacker)
			return false;
	} else if(state.world.war_get_primary_attacker(war) == target) {
		if(military::get_role(state, war, source) != military::war_role::defender)
			return false;
	} else if(state.world.war_get_primary_defender(war) == target) {
		if(military::get_role(state, war, source) != military::war_role::attacker)
			return false;
	}

	if(state.world.war_get_is_crisis_war(war)) {
		if((state.world.war_get_primary_attacker(war) != source || state.world.war_get_primary_defender(war) != target) &&
				(state.world.war_get_primary_attacker(war) != target || state.world.war_get_primary_defender(war) != source)) {

			return false; // no separate peace
		}
	}

	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	return !pending;
}
void execute_start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war,
		bool is_concession) {
	if(!can_start_peace_offer(state, source, target, war, is_concession))
		return;

	auto offer = fatten(state.world, state.world.create_peace_offer());
	offer.set_target(target);
	offer.set_war_from_war_settlement(war);
	offer.set_is_concession(is_concession);
	offer.set_nation_from_pending_peace_offer(source);
}

void add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::add_peace_offer_term;
	p.source = source;
	p.data.offer_wargoal.wg = goal;
	auto b = state.incoming_commands.try_push(p);
}
bool can_add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal) {
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	if(!pending)
		return false;

	auto war = state.world.peace_offer_get_war_from_war_settlement(pending);
	auto wg = fatten(state.world, goal);
	auto target = state.world.peace_offer_get_target(pending);

	if(wg.get_war_from_wargoals_attached() != war)
		return false;

	int32_t total = military::cost_of_peace_offer(state, pending);
	int32_t new_wg_cost = military::peace_cost(state, war, wg.get_type(), wg.get_added_by(), wg.get_target_nation(),
			wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag());

	if(total + new_wg_cost > 100)
		return false;

	if(state.world.war_get_primary_attacker(war) == source && state.world.war_get_primary_defender(war) == target) {
		return true;
	}
	if(state.world.war_get_primary_attacker(war) == target && state.world.war_get_primary_defender(war) == source) {
		return true;
	}

	if(state.world.peace_offer_get_is_concession(pending)) {
		if(state.world.war_get_primary_attacker(war) == source || state.world.war_get_primary_defender(war) == source) {
			if(wg.get_added_by() == target)
				return true;
			return false;
		} else {
			if(wg.get_target_nation() == source)
				return true;
			if(wg.get_target_nation().get_overlord_as_subject().get_ruler() == source)
				return true;
			return false;
		}
	} else {
		if(state.world.war_get_primary_attacker(war) == source || state.world.war_get_primary_defender(war) == source) {
			if(wg.get_target_nation() == target)
				return true;
			if(wg.get_target_nation().get_overlord_as_subject().get_ruler() == target)
				return true;
			return false;
		} else {
			if(wg.get_added_by() == target)
				return true;
			return false;
		}
	}
}
void execute_add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal) {
	if(!can_add_to_peace_offer(state, source, goal)) {
		return;
	}
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	state.world.force_create_peace_offer_item(pending, goal);
}

void send_peace_offer(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::send_peace_offer;
	p.source = source;
	auto b = state.incoming_commands.try_push(p);
}
bool can_send_peace_offer(sys::state& state, dcon::nation_id source) {
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	if(!pending)
		return false;
	return true;
}
void execute_send_peace_offer(sys::state& state, dcon::nation_id source) {
	auto pending_offer = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	auto in_war = state.world.peace_offer_get_war_from_war_settlement(pending_offer);
	auto target = state.world.peace_offer_get_target(pending_offer);

	// A peace offer must be accepted when war score reaches 100.
	if(military::directed_warscore(state, in_war, source, target) >= 100.0f) {
		military::implement_peace_offer(state, pending_offer);
	} else {
		diplomatic_message::message m;
		memset(&m, 0, sizeof(diplomatic_message::message));
		m.to = target;
		m.from = source;
		m.data.peace = pending_offer;
		m.type = diplomatic_message::type::peace_offer;

		diplomatic_message::post(state, m);
	}
}

void c_change_diplo_points(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_diplo_points;
	p.source = source;
	p.data.cheat.value = value;
	auto b = state.incoming_commands.try_push(p);
}
void execute_c_change_diplo_points(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_diplomatic_points(source) += value;
}
void c_change_money(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_money;
	p.source = source;
	p.data.cheat.value = value;
	auto b = state.incoming_commands.try_push(p);
}
void execute_c_change_money(sys::state& state, dcon::nation_id source, float value) {
	// state.world.nation_get_diplomatic_points(source) += value;
}
void c_westernize(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_westernize;
	p.source = source;
	auto b = state.incoming_commands.try_push(p);
}
void execute_c_westernize(sys::state& state, dcon::nation_id source) {
	state.world.nation_set_is_civilized(source, true);
}
void c_unwesternize(sys::state& state, dcon::nation_id source) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_unwesternize;
	p.source = source;
	auto b = state.incoming_commands.try_push(p);
}
void execute_c_unwesternize(sys::state& state, dcon::nation_id source) {
	state.world.nation_set_is_civilized(source, false);
}
void c_change_research_points(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_research_points;
	p.source = source;
	p.data.cheat.value = value;
	auto b = state.incoming_commands.try_push(p);
}
void execute_c_change_research_points(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_research_points(source) += value;
}
void c_change_cb_progress(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_cb_progress;
	p.source = source;
	p.data.cheat.value = value;
	auto b = state.incoming_commands.try_push(p);
}
void execute_c_change_cb_progress(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_constructing_cb_progress(source) += value;
}
void c_change_infamy(sys::state& state, dcon::nation_id source, float value) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::c_change_infamy;
	p.source = source;
	p.data.cheat.value = value;
	auto b = state.incoming_commands.try_push(p);
}
void execute_c_change_infamy(sys::state& state, dcon::nation_id source, float value) {
	state.world.nation_get_infamy(source) += value;
}

void move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::move_army;
	p.source = source;
	p.data.army_movement.a = a;
	p.data.army_movement.dest = dest;
	auto b = state.incoming_commands.try_push(p);
}

std::vector<dcon::province_id> can_move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest) {
	if(source != state.world.army_get_controller_from_army_control(a))
		return std::vector<dcon::province_id>{};
	if(state.world.army_get_is_retreating(a))
		return std::vector<dcon::province_id>{};
	if(!dest)
		return std::vector<dcon::province_id>{}; // stop movement

	auto last_province = state.world.army_get_location_from_army_location(a);
	auto movement = state.world.army_get_path(a);
	if(movement.size() > 0) {
		last_province = movement.at(movement.size() - 1);
	}

	if(last_province == dest)
		return std::vector<dcon::province_id>{};

	// TODO: check if in battle

	if(dest.index() < state.province_definitions.first_sea_province.index()) {
		if(state.world.army_get_black_flag(a)) {
			return province::make_unowned_land_path(state, last_province, dest);
		} else if(province::has_access_to_province(state, source, dest)) {
			return province::make_land_path(state, last_province, dest, source, a);
		} else {
			return std::vector<dcon::province_id>{};
		}
	} else {
		if(!military::can_embark_onto_sea_tile(state, source, dest, a))
			return std::vector<dcon::province_id>{};

		if(state.world.army_get_black_flag(a)) {
			return province::make_unowned_land_path(state, last_province, dest);
		} else {
			return province::make_land_path(state, last_province, dest, source, a);
		}
	}
}

void execute_move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest) {
	if(source != state.world.army_get_controller_from_army_control(a))
		return;
	if(state.world.army_get_is_retreating(a))
		return;

	if(!dest) {
		state.world.army_get_path(a).clear();
		state.world.army_set_arrival_time(a, sys::date{});
		return;
	}

	auto path = can_move_army(state, source, a, dest);
	if(path.size() > 0) {
		auto existing_path = state.world.army_get_path(a);
		auto append_size = uint32_t(path.size());
		auto old_size = existing_path.size();
		auto new_size = old_size + append_size;
		existing_path.resize(new_size);

		for(uint32_t i = 0; i < old_size; ++i) {
			existing_path.at(append_size + i) = existing_path.at(i);
		}
		for(uint32_t i = 0; i < append_size; ++i) {
			existing_path.at(i) = path[i];
		}

		if(old_size == 0) {
			state.world.army_set_arrival_time(a, military::arrival_time_to(state, a, path.back()));
		}
	}
}

void move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::move_navy;
	p.source = source;
	p.data.navy_movement.n = n;
	p.data.navy_movement.dest = dest;
	auto b = state.incoming_commands.try_push(p);
}
std::vector<dcon::province_id> can_move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest) {
	if(source != state.world.navy_get_controller_from_navy_control(n))
		return std::vector<dcon::province_id>{};
	if(state.world.navy_get_is_retreating(n))
		return std::vector<dcon::province_id>{};
	if(!dest)
		return std::vector<dcon::province_id>{}; // stop movement

	auto last_province = state.world.navy_get_location_from_navy_location(n);
	auto movement = state.world.navy_get_path(n);
	if(movement.size() > 0) {
		last_province = movement.at(movement.size() - 1);
	}

	if(last_province == dest)
		return std::vector<dcon::province_id>{};

	if(state.world.navy_get_battle_from_navy_battle_participation(n))
		return std::vector<dcon::province_id>{};

	if(dest.index() < state.province_definitions.first_sea_province.index()) {
		return province::make_naval_path(state, last_province, dest);
	} else {
		if(!state.world.province_get_is_coast(dest))
			return std::vector<dcon::province_id>{};

		if(!province::has_naval_access_to_province(state, source, dest))
			return std::vector<dcon::province_id>{};

		return province::make_naval_path(state, last_province, dest);
	}
}
void execute_move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest) {
	if(source != state.world.navy_get_controller_from_navy_control(n))
		return;
	if(state.world.navy_get_is_retreating(n))
		return;

	if(!dest) {
		state.world.navy_get_path(n).clear();
		state.world.navy_set_arrival_time(n, sys::date{});
		return;
	}

	auto path = can_move_navy(state, source, n, dest);
	if(path.size() > 0) {
		auto existing_path = state.world.navy_get_path(n);
		auto append_size = uint32_t(path.size());
		auto old_size = existing_path.size();
		auto new_size = old_size + append_size;
		existing_path.resize(new_size);

		for(uint32_t i = 0; i < old_size; ++i) {
			existing_path.at(append_size + i) = existing_path.at(i);
		}
		for(uint32_t i = 0; i < append_size; ++i) {
			existing_path.at(i) = path[i];
		}

		if(old_size == 0) {
			state.world.navy_set_arrival_time(n, military::arrival_time_to(state, n, path.back()));
		}
	}
}

void embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::embark_army;
	p.source = source;
	p.data.army_movement.a = a;
	auto b = state.incoming_commands.try_push(p);
}

bool can_embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	if(source != state.world.army_get_controller_from_army_control(a))
		return false;
	if(state.world.army_get_is_retreating(a))
		return false;

	auto location = state.world.army_get_location_from_army_location(a);
	if(location.index() >= state.province_definitions.first_sea_province.index())
		return false;

	// TODO: test not in battle

	if(state.world.army_get_navy_from_army_transport(a)) {
		return true;
	} else {
		return military::can_embark_onto_sea_tile(state, source, location, a);
	}
}

void execute_embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	if(source != state.world.army_get_controller_from_army_control(a))
		return;
	if(state.world.army_get_is_retreating(a))
		return;

	// TODO: test not in battle

	auto location = state.world.army_get_location_from_army_location(a);
	if(location.index() >= state.province_definitions.first_sea_province.index())
		return;

	if(state.world.army_get_navy_from_army_transport(a)) {
		state.world.army_set_navy_from_army_transport(a, dcon::navy_id{});
		military::army_arrives_in_province(state, a, location);
	} else {
		auto to_navy = military::find_embark_target(state, source, location, a);
		if(to_navy) {
			state.world.army_set_navy_from_army_transport(a, to_navy);
		}
	}
}

void merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::merge_armies;
	p.source = source;
	p.data.merge_army.a = a;
	p.data.merge_army.b = b;
	auto x = state.incoming_commands.try_push(p);
}
bool can_merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b) {
	if(state.world.army_get_controller_from_army_control(a) != source)
		return false;
	if(state.world.army_get_controller_from_army_control(b) != source)
		return false;
	if(state.world.army_get_is_retreating(a) || state.world.army_get_is_retreating(b))
		return false;

	if(state.world.army_get_location_from_army_location(a) != state.world.army_get_location_from_army_location(b))
		return false;

	// TODO: check for if they are in combat

	return true;
}

void execute_merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b) {
	if(!can_merge_armies(state, source, a, b))
		return;

	// take leader
	auto a_leader = state.world.army_get_general_from_army_leadership(a);
	auto b_leader = state.world.army_get_general_from_army_leadership(b);
	if(!a_leader && b_leader) {
		state.world.army_set_general_from_army_leadership(a, b_leader);
	}

	// stop movement
	state.world.army_get_path(a).clear();
	state.world.army_set_arrival_time(a, sys::date{});

	auto regs = state.world.army_get_army_membership(b);
	while(regs.begin() != regs.end()) {
		auto reg = (*regs.begin()).get_regiment();
		reg.set_army_from_army_membership(a);
	}

	state.world.delete_army(b);
}

void merge_navies(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::navy_id b) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::merge_navies;
	p.source = source;
	p.data.merge_navy.a = a;
	p.data.merge_navy.b = b;
	auto x = state.incoming_commands.try_push(p);
}
bool can_merge_navies(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::navy_id b) {
	if(state.world.navy_get_controller_from_navy_control(a) != source)
		return false;
	if(state.world.navy_get_controller_from_navy_control(b) != source)
		return false;
	if(state.world.navy_get_is_retreating(a) || state.world.navy_get_is_retreating(b))
		return false;

	if(state.world.navy_get_location_from_navy_location(a) != state.world.navy_get_location_from_navy_location(b))
		return false;

	if(state.world.navy_get_battle_from_navy_battle_participation(a) ||
			state.world.navy_get_battle_from_navy_battle_participation(b))
		return false;

	return true;
}
void execute_merge_navies(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::navy_id b) {
	if(!can_merge_navies(state, source, a, b))
		return;

	// take leader
	auto a_leader = state.world.navy_get_admiral_from_navy_leadership(a);
	auto b_leader = state.world.navy_get_admiral_from_navy_leadership(b);
	if(!a_leader && b_leader) {
		state.world.navy_set_admiral_from_navy_leadership(a, b_leader);
	}

	// stop movement
	state.world.navy_get_path(a).clear();
	state.world.navy_set_arrival_time(a, sys::date{});

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

	state.world.delete_navy(b);
}

void split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::split_army;
	p.source = source;
	p.data.army_movement.a = a;
	auto b = state.incoming_commands.try_push(p);
}
bool can_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	return state.world.army_get_controller_from_army_control(a) == source && !state.world.army_get_is_retreating(a);
}
void execute_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	if(!can_split_army(state, source, a))
		return;

	static std::vector<dcon::regiment_id> to_transfer;
	to_transfer.clear();

	for(auto t : state.world.army_get_army_membership(a)) {
		if(t.get_regiment().get_pending_split()) {
			t.get_regiment().set_pending_split(false);
			to_transfer.push_back(t.get_regiment().id);
		}
	}

	if(to_transfer.size() > 0) {
		auto new_u = fatten(state.world, state.world.create_army());
		new_u.set_controller_from_army_control(source);
		new_u.set_location_from_army_location(state.world.army_get_location_from_army_location(a));
		new_u.set_black_flag(state.world.army_get_black_flag(a));

		for(auto t : to_transfer) {
			state.world.regiment_set_army_from_army_membership(t, new_u);
		}

		auto old_regs = state.world.army_get_army_membership(a);
		if(old_regs.begin() == old_regs.end()) {
			state.world.leader_set_army_from_army_leadership(state.world.army_get_general_from_army_leadership(a), new_u);
			state.world.delete_army(a);
		}
	}

}

void split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::split_navy;
	p.source = source;
	p.data.navy_movement.n = a;
	auto b = state.incoming_commands.try_push(p);
}
bool can_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	auto embarked = state.world.navy_get_army_transport(a);
	return state.world.navy_get_controller_from_navy_control(a) == source && !state.world.navy_get_is_retreating(a) &&
				 !bool(state.world.navy_get_battle_from_navy_battle_participation(a)) && embarked.begin() == embarked.end();
}
void execute_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	if(!can_split_navy(state, source, a))
		return;

	static std::vector<dcon::ship_id> to_transfer;
	to_transfer.clear();

	for(auto t : state.world.navy_get_navy_membership(a)) {
		if(t.get_ship().get_pending_split()) {
			t.get_ship().set_pending_split(false);
			to_transfer.push_back(t.get_ship().id);
		}
	}

	if(to_transfer.size() > 0) {
		auto new_u = fatten(state.world, state.world.create_navy());
		new_u.set_controller_from_navy_control(source);
		new_u.set_location_from_navy_location(state.world.navy_get_location_from_navy_location(a));

		for(auto t : to_transfer) {
			state.world.ship_set_navy_from_navy_membership(t, new_u);
		}

		auto old_regs = state.world.navy_get_navy_membership(a);
		if(old_regs.begin() == old_regs.end()) {
			state.world.leader_set_navy_from_navy_leadership(state.world.navy_get_admiral_from_navy_leadership(a), new_u);
			state.world.delete_navy(a);
		}
	}
}

void delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::delete_army;
	p.source = source;
	p.data.army_movement.a = a;
	auto b = state.incoming_commands.try_push(p);
}
bool can_delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	return state.world.army_get_controller_from_army_control(a) == source && !state.world.army_get_is_retreating(a) &&
				 province::has_naval_access_to_province(state, source, state.world.army_get_location_from_army_location(a));
}
void execute_delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	if(!can_delete_army(state, source, a))
		return;

	auto r = state.world.army_get_army_membership(a);
	while(r.begin() != r.end()) {
		state.world.delete_regiment((*r.begin()).get_regiment());
	}
	state.world.delete_army(a);
}


void delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::delete_navy;
	p.source = source;
	p.data.navy_movement.n = a;
	auto b = state.incoming_commands.try_push(p);
}

bool can_delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	auto embarked = state.world.navy_get_army_transport(a);
	return state.world.navy_get_controller_from_navy_control(a) == source && !state.world.navy_get_is_retreating(a) &&
				 embarked.begin() == embarked.end() && !bool(state.world.navy_get_battle_from_navy_battle_participation(a))
				 && province::has_naval_access_to_province(state, source, state.world.navy_get_location_from_navy_location(a));
}
void execute_delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	if(!can_delete_navy(state, source, a))
		return;

	auto r = state.world.navy_get_navy_membership(a);
	while(r.begin() != r.end()) {
		state.world.delete_ship((*r.begin()).get_ship());
	}
	state.world.delete_navy(a);
}

void mark_regiments_to_split(sys::state& state, dcon::nation_id source,
		std::array<dcon::regiment_id, num_packed_units> const& list) {

	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::designate_split_regiments;
	p.source = source;
	std::copy_n(list.data(), num_packed_units, p.data.split_regiments.regs);
	auto b = state.incoming_commands.try_push(p);
}
void execute_mark_regiments_to_split(sys::state& state, dcon::nation_id source, dcon::regiment_id const* regs) {
	for(uint32_t i = 0; i < num_packed_units; ++i) {
		if(regs[i]) {
			if(source == state.world.army_get_controller_from_army_control(state.world.regiment_get_army_from_army_membership(regs[i]))) {
				state.world.regiment_set_pending_split(regs[i], !state.world.regiment_get_pending_split(regs[i]));
			}
		}
	}
}

void mark_ships_to_split(sys::state& state, dcon::nation_id source, std::array<dcon::ship_id, num_packed_units> const& list) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::designate_split_ships;
	p.source = source;
	std::copy_n(list.data(), num_packed_units, p.data.split_ships.ships);
	auto b = state.incoming_commands.try_push(p);
}
void execute_mark_ships_to_split(sys::state& state, dcon::nation_id source, dcon::ship_id const* regs) {
	for(uint32_t i = 0; i < num_packed_units; ++i) {
		if(regs[i]) {
			if(source ==
					state.world.navy_get_controller_from_navy_control(state.world.ship_get_navy_from_navy_membership(regs[i]))) {
				state.world.ship_set_pending_split(regs[i], !state.world.ship_get_pending_split(regs[i]));
			}
		}
	}
}

void retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::naval_battle_id b) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::naval_retreat;
	p.source = source;
	p.data.naval_battle.b = b;
	auto discard = state.incoming_commands.try_push(p);
}
bool can_retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::naval_battle_id b) {
	if(state.world.naval_battle_get_start_date(b) + 10 < state.current_date)
		return false;
	if(source != military::get_naval_battle_lead_attacker(state, b) && source != military::get_naval_battle_lead_defender(state, b))
		return false;

	return true;
}
void execute_retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::naval_battle_id b) {
	if(state.world.naval_battle_get_start_date(b) + 10 < state.current_date)
		return;

	if(source == military::get_naval_battle_lead_attacker(state, b)) {
		military::end_battle(state, b, military::battle_result::defender_won);
	} else if(source == military::get_naval_battle_lead_attacker(state, b)) {
		military::end_battle(state, b, military::battle_result::attacker_won);
	}

}

void execute_pending_commands(sys::state& state) {
	auto* c = state.incoming_commands.front();
	bool command_executed = false;
	while(c) {
		command_executed = true;

		switch(c->type) {
		case command_type::invalid:
			std::abort(); // invalid command
			break;
		case command_type::change_nat_focus:
			execute_set_national_focus(state, c->source, c->data.nat_focus.target_state, c->data.nat_focus.focus);
			break;
		case command_type::start_research:
			execute_start_research(state, c->source, c->data.start_research.tech);
			break;
		case command_type::make_leader:
			execute_make_leader(state, c->source, c->data.make_leader.is_general);
			break;
		case command_type::begin_province_building_construction:
			execute_begin_province_building_construction(state, c->source, c->data.start_province_building.location,
					c->data.start_province_building.type);
			break;
		case command_type::war_subsidies:
			execute_give_war_subsidies(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::cancel_war_subsidies:
			execute_cancel_war_subsidies(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::increase_relations:
			execute_increase_relations(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::decrease_relations:
			execute_decrease_relations(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::begin_factory_building_construction:
			execute_begin_factory_building_construction(state, c->source, c->data.start_factory_building.location,
					c->data.start_factory_building.type, c->data.start_factory_building.is_upgrade);
			break;
		case command_type::begin_naval_unit_construction:
			execute_start_naval_unit_construction(state, c->source, c->data.naval_unit_construction.location,
					c->data.naval_unit_construction.type);
			break;
		case command_type::cancel_naval_unit_construction:
			execute_cancel_naval_unit_construction(state, c->source, c->data.naval_unit_construction.location,
					c->data.naval_unit_construction.type);
			break;
		case command_type::begin_land_unit_construction:
			execute_start_land_unit_construction(state, c->source, c->data.land_unit_construction.location,
					c->data.land_unit_construction.pop_culture, c->data.land_unit_construction.type);
			break;
		case command_type::cancel_land_unit_construction:
			execute_cancel_land_unit_construction(state, c->source, c->data.land_unit_construction.location,
					c->data.land_unit_construction.pop_culture, c->data.land_unit_construction.type);
			break;
		case command_type::delete_factory:
			execute_delete_factory(state, c->source, c->data.factory.location, c->data.factory.type);
			break;
		case command_type::change_factory_settings:
			execute_change_factory_settings(state, c->source, c->data.factory.location, c->data.factory.type, c->data.factory.priority,
					c->data.factory.subsidize);
			break;
		case command_type::make_vassal:
			execute_make_vassal(state, c->source, c->data.tag_target.ident);
			break;
		case command_type::release_and_play_nation:
			execute_release_and_play_as(state, c->source, c->data.tag_target.ident);
			break;
		case command_type::change_budget:
			execute_change_budget_settings(state, c->source, c->data.budget_data);
			break;
		case command_type::start_election:
			execute_start_election(state, c->source);
			break;
		case command_type::change_influence_priority:
			execute_change_influence_priority(state, c->source, c->data.influence_priority.influence_target,
					c->data.influence_priority.priority);
			break;
		case command_type::expel_advisors:
			execute_expel_advisors(state, c->source, c->data.influence_action.influence_target, c->data.influence_action.gp_target);
			break;
		case command_type::ban_embassy:
			execute_ban_embassy(state, c->source, c->data.influence_action.influence_target, c->data.influence_action.gp_target);
			break;
		case command_type::discredit_advisors:
			execute_discredit_advisors(state, c->source, c->data.influence_action.influence_target, c->data.influence_action.gp_target);
			break;
		case command_type::decrease_opinion:
			execute_decrease_opinion(state, c->source, c->data.influence_action.influence_target, c->data.influence_action.gp_target);
			break;
		case command_type::remove_from_sphere:
			execute_remove_from_sphere(state, c->source, c->data.influence_action.influence_target, c->data.influence_action.gp_target);
			break;
		case command_type::increase_opinion:
			execute_increase_opinion(state, c->source, c->data.influence_action.influence_target);
			break;
		case command_type::add_to_sphere:
			execute_add_to_sphere(state, c->source, c->data.influence_action.influence_target);
			break;
		case command_type::upgrade_colony_to_state:
			execute_upgrade_colony_to_state(state, c->source, state.world.province_get_state_membership(c->data.generic_location.prov));
			break;
		case command_type::invest_in_colony:
			execute_invest_in_colony(state, c->source, c->data.generic_location.prov);
			break;
		case command_type::abandon_colony:
			execute_abandon_colony(state, c->source, c->data.generic_location.prov);
			break;
		case command_type::finish_colonization:
			execute_finish_colonization(state, c->source, c->data.generic_location.prov);
			break;
		case command_type::intervene_in_war:
			execute_intervene_in_war(state, c->source, c->data.war_target.war, c->data.war_target.for_attacker);
			break;
		case command_type::suppress_movement:
			execute_suppress_movement(state, c->source, c->data.movement.iopt, c->data.movement.tag);
			break;
		case command_type::civilize_nation:
			execute_civilize_nation(state, c->source);
			break;
		case command_type::appoint_ruling_party:
			execute_appoint_ruling_party(state, c->source, c->data.political_party.p);
			break;
		case command_type::change_issue_option:
			execute_enact_issue(state, c->source, c->data.issue_selection.r);
			break;
		case command_type::change_reform_option:
			execute_enact_reform(state, c->source, c->data.reform_selection.r);
			break;
		case command_type::become_interested_in_crisis:
			execute_become_interested_in_crisis(state, c->source);
			break;
		case command_type::take_sides_in_crisis:
			execute_take_sides_in_crisis(state, c->source, c->data.crisis_join.join_attackers);
			break;
		case command_type::change_stockpile_settings:
			execute_change_stockpile_settings(state, c->source, c->data.stockpile_settings.c, c->data.stockpile_settings.amount,
					c->data.stockpile_settings.draw_on_stockpiles);
			break;
		case command_type::take_decision:
			execute_take_decision(state, c->source, c->data.decision.d);
			break;
		case command_type::make_n_event_choice:
			execute_make_event_choice(state, c->source, c->data.pending_human_n_event);
			break;
		case command_type::make_f_n_event_choice:
			execute_make_event_choice(state, c->source, c->data.pending_human_f_n_event);
			break;
		case command_type::make_p_event_choice:
			execute_make_event_choice(state, c->source, c->data.pending_human_p_event);
			break;
		case command_type::make_f_p_event_choice:
			execute_make_event_choice(state, c->source, c->data.pending_human_f_p_event);
			break;
		case command_type::cancel_cb_fabrication:
			execute_cancel_cb_fabrication(state, c->source);
			break;
		case command_type::fabricate_cb:
			execute_fabricate_cb(state, c->source, c->data.cb_fabrication.target, c->data.cb_fabrication.type);
			break;
		case command_type::ask_for_military_access:
			execute_ask_for_access(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::ask_for_alliance:
			execute_ask_for_alliance(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::call_to_arms:
			execute_call_to_arms(state, c->source, c->data.call_to_arms.target, c->data.call_to_arms.war);
			break;
		case command_type::respond_to_diplomatic_message:
			execute_respond_to_diplomatic_message(state, c->source, c->data.message.from, c->data.message.type, c->data.message.accept);
			break;
		case command_type::cancel_military_access:
			execute_cancel_military_access(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::cancel_alliance:
			execute_cancel_alliance(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::cancel_given_military_access:
			execute_cancel_given_military_access(state, c->source, c->data.diplo_action.target);
			break;
		case command_type::declare_war:
			execute_declare_war(state, c->source, c->data.new_war.target, c->data.new_war.primary_cb, c->data.new_war.cb_state,
					c->data.new_war.cb_tag, c->data.new_war.cb_secondary_nation);
			break;
		case command_type::add_war_goal:
			execute_add_war_goal(state, c->source, c->data.new_war_goal.war, c->data.new_war_goal.target, c->data.new_war_goal.cb_type,
					c->data.new_war_goal.cb_state, c->data.new_war_goal.cb_tag, c->data.new_war_goal.cb_secondary_nation);
			break;
		case command_type::start_peace_offer:
			execute_start_peace_offer(state, c->source, c->data.new_offer.target, c->data.new_offer.war,
					c->data.new_offer.is_concession);
			break;
		case command_type::add_peace_offer_term:
			execute_add_to_peace_offer(state, c->source, c->data.offer_wargoal.wg);
			break;
		case command_type::send_peace_offer:
			execute_send_peace_offer(state, c->source);
			break;
		case command_type::move_army:
			execute_move_army(state, c->source, c->data.army_movement.a, c->data.army_movement.dest);
			break;
		case command_type::move_navy:
			execute_move_navy(state, c->source, c->data.navy_movement.n, c->data.navy_movement.dest);
			break;
		case command_type::embark_army:
			execute_embark_army(state, c->source, c->data.army_movement.a);
			break;
		case command_type::merge_armies:
			execute_merge_armies(state, c->source, c->data.merge_army.a, c->data.merge_army.b);
			break;
		case command_type::merge_navies:
			execute_merge_navies(state, c->source, c->data.merge_navy.a, c->data.merge_navy.b);
			break;
		case command_type::split_army:
			execute_split_army(state, c->source, c->data.army_movement.a);
			break;
		case command_type::split_navy:
			execute_split_navy(state, c->source, c->data.navy_movement.n);
			break;
		case command_type::delete_army:
			execute_delete_army(state, c->source, c->data.army_movement.a);
			break;
		case command_type::delete_navy:
			execute_delete_navy(state, c->source, c->data.navy_movement.n);
			break;
		case command_type::designate_split_regiments:
			execute_mark_regiments_to_split(state, c->source, c->data.split_regiments.regs);
			break;
		case command_type::designate_split_ships:
			execute_mark_ships_to_split(state, c->source, c->data.split_ships.ships);
			break;
		case command_type::naval_retreat:
			execute_retreat_from_naval_battle(state, c->source, c->data.naval_battle.b);
			break;

		// console commands
		case command_type::switch_nation:
			execute_switch_nation(state, c->source, c->data.tag_target.ident);
			break;
		case command_type::c_change_diplo_points:
			execute_c_change_diplo_points(state, c->source, c->data.cheat.value);
			break;
		case command_type::c_change_money:
			execute_c_change_money(state, c->source, c->data.cheat.value);
			break;
		case command_type::c_westernize:
			execute_c_westernize(state, c->source);
			break;
		case command_type::c_unwesternize:
			execute_c_unwesternize(state, c->source);
			break;
		case command_type::c_change_research_points:
			execute_c_change_research_points(state, c->source, c->data.cheat.value);
			break;
		case command_type::c_change_cb_progress:
			execute_c_change_cb_progress(state, c->source, c->data.cheat.value);
			break;
		case command_type::c_change_infamy:
			execute_c_change_infamy(state, c->source, c->data.cheat.value);
			break;
		}

		state.incoming_commands.pop();
		c = state.incoming_commands.front();
	}

	if(command_executed) {
		state.game_state_updated.store(true, std::memory_order::release);
	}
}

} // namespace command
