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
		dcon::technology_id prev_tech = dcon::technology_id(dcon::technology_id::value_base_t(tech_id.index() - 1));
		// Previous technology is from the same folder so we have to check that we have researched it beforehand
		if(tech_id.index() != 0 && state.world.technology_get_folder_index(prev_tech) == state.world.technology_get_folder_index(tech)) {
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
		}

		state.incoming_commands.pop();
		c = state.incoming_commands.front();
	}

	if(command_executed) {
		state.game_state_updated.store(true, std::memory_order::release);
	}
}

}
