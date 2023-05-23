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
			case command_type::begin_factory_building_construction:
				execute_begin_factory_building_construction(state, c->source, c->data.start_factory_building.location, c->data.start_factory_building.type, c->data.start_factory_building.is_upgrade);
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
