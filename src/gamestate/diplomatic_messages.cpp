#include "diplomatic_messages.hpp"
#include "system_state.hpp"
#include "commands.hpp"

namespace diplomatic_message {


void decline_message(sys::state& state, message const& m) {
	switch(m.type) {
		case type::none:
			std::abort();
			break;
		case type::access_request:
			if(!command::can_ask_for_access(state, m.from, m.to))
				return;

			nations::adjust_relationship(state, m.from, m.to, state.defines.askmilaccess_relation_on_decline);
			break;
		case type::alliance_request:
			if(!command::can_ask_for_alliance(state, m.from, m.to))
				return;

			nations::adjust_relationship(state, m.from, m.to, state.defines.alliance_relation_on_decline);
			break;
		case type::call_ally_request:
		{
			if(!command::can_call_to_arms(state, m.from, m.to, m.data.war))
				return;

			nations::adjust_relationship(state, m.from, m.to, state.defines.callally_relation_on_decline);
			auto was_defensive = false;
			for(auto wp : state.world.war_get_war_participant(m.data.war)) {
				if(wp.get_nation() == m.from) {
					was_defensive = !wp.get_is_attacker();
					break;
				}
			}
			if(was_defensive) {
				auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(m.from, m.to);
				if(rel) {
					state.world.diplomatic_relation_set_are_allied(rel, false);
				}
			}
			break;
		}
	}

}
void accept_message(sys::state& state, message const& m) {
	switch(m.type) {
		case type::none:
			std::abort();
			break;
		case type::access_request:
		{
			if(!command::can_ask_for_access(state, m.from, m.to))
				return;

			nations::adjust_relationship(state, m.from, m.to, state.defines.askmilaccess_relation_on_accept);
			auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(m.to, m.from);
			if(!rel) {
				rel = state.world.force_create_unilateral_relationship(m.to, m.from);
			}
			state.world.unilateral_relationship_set_military_access(rel, true);

			break;
		}
		case type::alliance_request:
		{
			if(!command::can_ask_for_alliance(state, m.from, m.to))
				return;

			nations::adjust_relationship(state, m.from, m.to, state.defines.alliance_relation_on_accept);
			auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(m.from, m.to);
			if(!rel) {
				rel = state.world.force_create_diplomatic_relation(m.from, m.to);
			}
			state.world.diplomatic_relation_set_are_allied(rel, true);
			break;
		}
		case type::call_ally_request:
		{
			if(!command::can_call_to_arms(state, m.from, m.to, m.data.war))
				return;

			// TODO: join war

			nations::adjust_relationship(state, m.from, m.to, state.defines.callally_relation_on_accept);
			break;
		}
	}
}

void post_message(sys::state& state, message const& m) {
	if(state.world.nation_get_is_player_controlled(m.to) == false) {
		// TODO : call AI logic to decide responses to requests

		switch(m.type) {
			case type::none:
				std::abort();
				return;
			case type::access_request:
				accept_message(state, m);
				return;
			case type::alliance_request:
				decline_message(state, m);
				return;
			case type::call_ally_request:
				decline_message(state, m);
				return;
		}
	}

	for(auto& i : state.pending_messages) {
		if(i.type == type::none) {
			std::memcpy(&i, &m, sizeof(message));

			// TODO: post new message notification to message target

			return;
		}
	}
}

void update_pending_messages(sys::state& state) {
	for(auto& m : state.pending_messages) {
		if(m.type != type::none && m.when + 31 <= state.current_date) {

			decline_message(state, m);
			m.type = type::none;
		}
	}
}

}
