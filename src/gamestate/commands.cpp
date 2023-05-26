#include "commands.hpp"
#include "system_state.hpp"
#include "nations.hpp"

namespace command {

void set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::change_nat_focus;
	p.source = source;
	p.data.nat_focus.focus = focus;
	p.data.nat_focus.target_state = target_state;
	auto b = state.incoming_commands.try_push(p);
}

bool can_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus) {
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
				if(state.world.ideology_get_enabled(ideo) == false
					|| (state.world.ideology_get_is_civilized_only(ideo) && !state.world.nation_get_is_civilized(source))) {
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
			return state_contains_core && state.world.nation_get_rank(source) > uint16_t(state.defines.colonial_rank) && focus == state.national_definitions.flashpoint_focus && (num_focuses_set < num_focuses_total || bool(state.world.nation_get_state_from_flashpoint_focus(source))) && bool(state.world.state_instance_get_nation_from_flashpoint_focus(target_state)) == false;
		}
	}
}

void execute_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus) {
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
	/* Can only perform if, the nations are not at war, the nation isn't already being given war subsidies, and there is defines:WARSUBSIDY_DIPLOMATIC_COST diplomatic points available. And the target isn't equal to the sender. */
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
	/* Can only perform if, the nations are not at war, the nation is already being given war subsidies, and there is defines:CANCELWARSUBSIDY_DIPLOMATIC_COST diplomatic points available. And the target isn't equal to the sender. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(rel && !state.world.unilateral_relationship_get_war_subsidies(rel))
		return false; // Must already be giving war subsidies
	return state.world.nation_get_diplomatic_points(source) >= state.defines.cancelwarsubsidy_diplomatic_cost; // Enough diplomatic points
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
	/* Can only perform if, the nations are not at war, the relation value isn't maxxed out at 200, and has defines:INCREASERELATION_DIPLOMATIC_COST diplomatic points. And the target can't be the same as the sender. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(source, target);
	if(rel && state.world.diplomatic_relation_get_value(rel) >= 200.f)
		return false; // Maxxed out
	return state.world.nation_get_diplomatic_points(source) >= state.defines.increaserelation_diplomatic_cost; // Enough diplomatic points
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
	/* Can only perform if, the nations are not at war, the relation value isn't maxxed out at -200, and has defines:DECREASERELATION_DIPLOMATIC_COST diplomatic points. And not done to self. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(source, target);
	if(rel && state.world.diplomatic_relation_get_value(rel) <= -200.f)
		return false; // Maxxed out
	return state.world.nation_get_diplomatic_points(source) >= state.defines.decreaserelation_diplomatic_cost; // Enough diplomatic points
}
void execute_decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_decrease_relations(state, source, target))
		return;

	nations::adjust_relationship(state, source, target, state.defines.decreaserelation_relation_on_accept);
	state.world.nation_get_diplomatic_points(source) -= state.defines.decreaserelation_diplomatic_cost;
}

// -----------------------------------------------------------------------------
void begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id prov, economy::province_building_type type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::begin_province_building_construction;
	p.source = source;
	p.data.start_province_building.location = prov;
	p.data.start_province_building.type = type;
	auto b = state.incoming_commands.try_push(p);
}
bool can_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type) {

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
void execute_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type) {

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

void begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type, bool is_upgrade) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::begin_factory_building_construction;
	p.source = source;
	p.data.start_factory_building.location = location;
	p.data.start_factory_building.type = type;
	p.data.start_factory_building.is_upgrade = is_upgrade;
	auto b = state.incoming_commands.try_push(p);
}

bool can_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type, bool is_upgrade) {

	auto owner = state.world.state_instance_get_nation_from_state_ownership(location);

	/*
	The factory building must be unlocked by the nation.
	Factories cannot be built in a colonial state.
	*/

	if(state.world.nation_get_active_building(source, type) == false && !state.world.factory_type_get_is_available_from_start(type))
		return false;
	if(state.world.province_get_is_colonial(state.world.state_instance_get_capital(location)))
		return false;

	if(owner != source) {
		/*
		For foreign investment: the target nation must allow foreign investment, the nation doing the investing must be a great power while the target is not a great power, and the nation doing the investing must not be at war with the target nation. The nation being invested in must be civilized.
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

		//For new factories: no more than 7 existing + under construction new factories must be present.
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
		return num_factories <= 7;
	}
}

void execute_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::state_instance_id location, dcon::factory_type_id type, bool is_upgrade) {

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

		nations::adjust_foreign_investment(state, source, state.world.state_instance_get_nation_from_state_ownership(location), amount);
	}
}

void start_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::begin_unit_construction;
	p.source = source;
	p.data.unit_construction.location = location;
	p.data.unit_construction.type = type;
	auto b = state.incoming_commands.try_push(p);
}

bool can_start_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	/*
	The province must be owned and controlled by the building nation, without an ongoing siege.
	The unit type must be available from start / unlocked by the nation
	*/

	if(state.world.province_get_nation_from_province_ownership(location) != source)
		return false;
	if(state.world.province_get_nation_from_province_control(location) != source)
		return false;
	if(state.world.nation_get_active_unit(source, type) == false && state.military_definitions.unit_base_definitions[type].active == false)
		return false;

	if(state.military_definitions.unit_base_definitions[type].is_land) {
		/*
		Each soldier pop can only support so many regiments (including under construction and rebel regiments)
		If the unit is culturally restricted, there must be an available primary culture/accepted culture soldier pop with space
		*/
		if(state.military_definitions.unit_base_definitions[type].primary_culture) {
			auto total_built = military::main_culture_regiments_created_from_province(state, location);
			auto under_const = military::main_culture_regiments_under_construction_in_province(state, location);
			auto possible = military::main_culture_regiments_max_possible_from_province(state, location);
			return possible > total_built + under_const;
		} else {
			auto total_built = military::regiments_created_from_province(state, location);
			auto under_const = military::regiments_under_construction_in_province(state, location);
			auto possible = military::regiments_max_possible_from_province(state, location);
			return possible > total_built + under_const;
		}
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

void execute_start_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	if(!can_start_unit_construction(state, source, location, type))
		return;

	if(state.military_definitions.unit_base_definitions[type].is_land) {
		auto c = fatten(state.world, state.world.try_create_province_land_construction(location, source));
		c.set_type(type);
	} else {
		auto c = fatten(state.world, state.world.try_create_province_naval_construction(location, source));
		c.set_type(type);
	}
}


void cancel_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::cancel_unit_construction;
	p.source = source;
	p.data.unit_construction.location = location;
	p.data.unit_construction.type = type;
	auto b = state.incoming_commands.try_push(p);
}

bool can_cancel_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	return state.world.province_get_nation_from_province_ownership(location) == source;
}

void execute_cancel_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	if(!can_cancel_unit_construction(state, source, location, type))
		return;

	if(state.military_definitions.unit_base_definitions[type].is_land) {
		dcon::province_land_construction_id c;
		for(auto lc : state.world.province_get_province_land_construction(location)) {
			if(lc.get_type() == type) {
				c = lc.id;
			}
		}
		state.world.delete_province_land_construction(c);
	} else {
		dcon::province_naval_construction_id c;
		for(auto lc : state.world.province_get_province_naval_construction(location)) {
			if(lc.get_type() == type) {
				c = lc.id;
			}
		}
		state.world.delete_province_naval_construction(c);
	}
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
bool can_change_factory_settings(sys::state& state, dcon::nation_id source, dcon::factory_id f, uint8_t priority, bool subsidized) {
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
void execute_change_factory_settings(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type, uint8_t priority, bool subsidized) {

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
bool can_change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority) {
	//The source must be a great power, while the target must not be a great power.
	return state.world.nation_get_is_great_power(source) && !state.world.nation_get_is_great_power(influence_target);
}
void execute_change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority) {
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

void discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::discredit_advisors;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	p.data.influence_action.gp_target = affected_gp;
	auto b = state.incoming_commands.try_push(p);
}
bool can_discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:DISCREDIT_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To discredit a nation, you must have an opinion of at least "opposed" with the influenced nation and you must have a an equal or better opinion level with the influenced nation than the nation you are discrediting does.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) || state.world.nation_get_is_great_power(influence_target))
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

	return nations::influence::is_influence_level_greater_or_equal(clevel, nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	if(!can_discredit_advisors(state, source, influence_target, affected_gp))
		return;

	/*
	A nation is discredited for define:DISCREDIT_DAYS. Being discredited twice does not add these durations together; it just resets the timer from the current day. Discrediting a nation "increases" your relationship with them by define:DISCREDIT_RELATION_ON_ACCEPT. Discrediting costs define:DISCREDIT_INFLUENCE_COST influence points. 
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
bool can_expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:EXPELADVISORS_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To expel advisors you must have at least neutral opinion with the influenced nation and an equal or better opinion level than that of the nation you are expelling.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) || state.world.nation_get_is_great_power(influence_target))
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

	return nations::influence::is_influence_level_greater_or_equal(clevel, nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	if(!can_expel_advisors(state, source, influence_target, affected_gp))
		return;

	/*
	Expelling a nation's advisors "increases" your relationship with them by define:EXPELADVISORS_RELATION_ON_ACCEPT. This action costs define:EXPELADVISORS_INFLUENCE_COST influence points. Being expelled cancels any ongoing discredit effect. Being expelled reduces your influence to zero. 
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
	The source must be a great power. The source must have define:BANEMBASSY_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To ban a nation you must be at least friendly with the influenced nation and have an equal or better opinion level than that of the nation you are expelling.
	*/

	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) || state.world.nation_get_is_great_power(influence_target))
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

	return nations::influence::is_influence_level_greater_or_equal(clevel, nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	if(!can_ban_embassy(state, source, influence_target, affected_gp))
		return;

	/*
	Banning a nation's embassy "increases" your relationship with them by define:BANEMBASSY_RELATION_ON_ACCEPT. This action costs define:BANEMBASSY_INFLUENCE_COST influence points. The ban embassy effect lasts for define:BANEMBASSY_DAYS. If you are already banned, being banned again simply restarts the timer. Being banned cancels out any ongoing discredit effect.
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
	The source must be a great power. The source must have define:INCREASEOPINION_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. Your current opinion must be less than friendly
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
	Increasing the opinion of a nation costs define:INCREASEOPINION_INFLUENCE_COST influence points. Opinion can be increased to a maximum of friendly.
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
bool can_decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:DECREASEOPINION_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. Decreasing the opinion of another nation requires that you have an opinion of at least "opposed" with the influenced nation and you must have a an equal or better opinion level with the influenced nation than the nation you are lowering their opinion of does. The secondary target must neither have the influenced nation in sphere nor may it already be at hostile opinion with them.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) || state.world.nation_get_is_great_power(influence_target))
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

	if((nations::influence::level_mask & state.world.gp_relationship_get_status(state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp))) == nations::influence::level_hostile)
		return false;

	return nations::influence::is_influence_level_greater_or_equal(clevel, nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	Decreasing the opinion of a nation "increases" your relationship with them by define:DECREASEOPINION_RELATION_ON_ACCEPT. This actions costs define:DECREASEOPINION_INFLUENCE_COST influence points. Opinion of the influenced nation of the secondary target decreases by one step.
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
	The source must be a great power. The source must have define:ADDTOSPHERE_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. The nation must have a friendly opinion of you and my not be in the sphere of another nation.
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

void remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	payload p;
	memset(&p, 0, sizeof(payload));
	p.type = command_type::remove_from_sphere;
	p.source = source;
	p.data.influence_action.influence_target = influence_target;
	p.data.influence_action.gp_target = affected_gp;
	auto b = state.incoming_commands.try_push(p);
}
bool can_remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	The source must be a great power. The source must have define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To preform this action you must have an opinion level of friendly with the nation you are removing from a sphere.
	*/
	if(!state.world.nation_get_is_great_power(source) || !state.world.nation_get_is_great_power(affected_gp) || state.world.nation_get_is_great_power(influence_target))
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
void execute_remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	Removing a nation from a sphere costs define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. If you remove a nation from your own sphere you lose define:REMOVEFROMSPHERE_PRESTIGE_COST prestige and gain define:REMOVEFROMSPHERE_INFAMY_COST infamy. Removing a nation from the sphere of another nation "increases" your relationship with the former sphere leader by define:REMOVEFROMSPHERE_RELATION_ON_ACCEPT points. The removed nation then becomes friendly with its former sphere leader.
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
		//Provinces in the state stop being colonial.
		state.world.province_set_is_colonial(p, false);

		//All timed modifiers active for provinces in the state expire
		auto timed_modifiers = state.world.province_get_current_modifiers(p);
		for(uint32_t i = timed_modifiers.size(); i-- > 0;) {
			if(bool(timed_modifiers[i].expiration)) {
				timed_modifiers.remove_at(i);
			}
		}
	});

	//Gain define:COLONY_TO_STATE_PRESTIGE_GAIN x(1.0 + colony - prestige - from - tech) x(1.0 + prestige - from - tech)
	nations::adjust_prestige(state, source, state.defines.colony_to_state_prestige_gain * (1.0f + state.world.nation_get_modifier_values(source, sys::national_mod_offsets::colonial_prestige)));

	//An event from `on_colony_to_state` happens(with the state in scope)
	event::fire_fixed_event(state, state.national_definitions.on_colony_to_state, trigger::to_generic(si), source, -1);

	//An event from `on_colony_to_state_free_slaves` happens(with the state in scope)
	event::fire_fixed_event(state, state.national_definitions.on_colony_to_state_free_slaves, trigger::to_generic(si), source, -1);

	//Update is colonial nation
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
	if(province::is_colonizing(state, source, state_def))
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
					rel.get_points_invested() += uint16_t(state.defines.colonization_extra_guard_cost * (rel.get_level() - 4) + state.defines.colonization_influence_cost);
				}

				rel.get_level() += uint8_t(1);
				rel.set_last_investment(state.current_date);

				/*
				If you get define:COLONIZATION_INTEREST_LEAD points it moves into phase 2, kicking out all but the second-most colonizer (in terms of points). In phase 2 if you get define:COLONIZATION_INFLUENCE_LEAD points ahead of the other colonizer, the other colonizer is kicked out and the phase moves to 3.
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
		new_rel.set_points_invested(uint16_t(state.defines.colonization_interest_cost_initial + (adjacent ? state.defines.colonization_interest_cost_neighbor_modifier : 0.0f)));

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
void execute_abandon_colony(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(p);
	
	for(auto rel : state.world.state_definition_get_colonization(state_def)) {
		if(rel.get_colonizer() == source) {
			state.world.delete_colonization(rel);
		}
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
				execute_begin_province_building_construction(state, c->source, c->data.start_province_building.location, c->data.start_province_building.type);
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
				execute_begin_factory_building_construction(state, c->source, c->data.start_factory_building.location, c->data.start_factory_building.type, c->data.start_factory_building.is_upgrade);
				break;
			case command_type::begin_unit_construction:
				execute_start_unit_construction(state, c->source, c->data.unit_construction.location, c->data.unit_construction.type);
				break;
			case command_type::cancel_unit_construction:
				execute_cancel_unit_construction(state, c->source, c->data.unit_construction.location, c->data.unit_construction.type);
				break;
			case command_type::delete_factory:
				execute_delete_factory(state, c->source, c->data.factory.location, c->data.factory.type);
				break;
			case command_type::change_factory_settings:
				execute_change_factory_settings(state, c->source, c->data.factory.location, c->data.factory.type, c->data.factory.priority, c->data.factory.subsidize);
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
				execute_change_influence_priority(state, c->source, c->data.influence_priority.influence_target, c->data.influence_priority.priority);
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
		}

		state.incoming_commands.pop();
		c = state.incoming_commands.front();
	}

	if(command_executed) {
		state.game_state_updated.store(true, std::memory_order::release);
	}
}

}
