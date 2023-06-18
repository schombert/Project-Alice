#include "diplomatic_messages.hpp"
#include "system_state.hpp"
#include "commands.hpp"
#include "nations.hpp"

namespace diplomatic_message {

void decline(sys::state& state, message const& m) {
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
	case type::call_ally_request: {
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
	case type::be_crisis_primary_attacker:
		nations::reject_crisis_participation(state);
		break;
	case type::be_crisis_primary_defender:
		nations::reject_crisis_participation(state);
		break;
	case type::peace_offer:
		military::reject_peace_offer(state, m.data.peace);
		break;
	case type::take_crisis_side_offer:
		// TODO: notify offer rejected
		break;
	case type::crisis_peace_offer:
		// TODO: notify rejected
		/*
		Crisis resolution offers function much in the same way as peace offers. Every refused crisis offer increases the temperature
		of the current crisis by define:CRISIS_TEMPERATURE_ON_OFFER_DECLINE.
		*/
		state.crisis_temperature += state.defines.crisis_temperature_on_offer_decline;
		nations::cleanup_crisis_peace_offer(state, m.data.peace);
		break;
	}
}

bool can_accept_crisis_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, sys::crisis_join_offer const& offer) {
	if(state.current_crisis_mode != sys::crisis_mode::heating_up)
		return false;
	if(from != state.primary_crisis_attacker && from != state.primary_crisis_defender)
		return false;

	if(to == offer.target || from == offer.target)
		return false;

	// to must be merely interested participant
	for(auto& par : state.crisis_participants) {
		if(!par.id) {
			return false; // not in crisis
		}
		if(par.id == to) {
			if(par.merely_interested == false)
				return false; // already in crisis
			break;
		}
	}

	// target must be in crisis
	for(auto& par : state.crisis_participants) {
		if(!par.id) {
			return false; // not in crisis
		}
		if(par.id == offer.target) {
			if(par.merely_interested)
				return false;
			if(par.supports_attacker && from == state.primary_crisis_attacker)
				return false;
			if(!par.supports_attacker && from == state.primary_crisis_defender)
				return false;

			break;
		}
	}

	if(!military::cb_instance_conditions_satisfied(state, to, offer.target, offer.wargoal_type, offer.wargoal_state,
				 offer.wargoal_tag, offer.wargoal_secondary_nation)) {

		return false;
	}

	return true;
}

void add_to_crisis_with_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, sys::crisis_join_offer const& offer) {

	for(auto& par : state.crisis_participants) {
		if(!par.id) {
			return; // not in crisis
		}
		if(par.id == to) {
			par.merely_interested = false;
			par.supports_attacker = (from == state.primary_crisis_attacker);
			par.joined_with_offer = offer;
			break;
		}
	}

	auto infamy = military::crisis_cb_addition_infamy_cost(state, offer.wargoal_type, to, offer.target) *
								state.defines.crisis_wargoal_infamy_mult;
	state.world.nation_get_infamy(from) += infamy;
}

bool can_accept_crisis_peace_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, dcon::peace_offer_id peace) {
	if(state.current_crisis_mode != sys::crisis_mode::heating_up)
		return false;
	if(from != state.primary_crisis_attacker && from != state.primary_crisis_defender)
		return false;
	if(to != state.primary_crisis_attacker && to != state.primary_crisis_defender)
		return false;
	if(from == to)
		return false;

	// check: all goals in offer are part of the crisis
	for(auto wg : state.world.peace_offer_get_peace_offer_item(peace)) {
		bool found = [&]() {
			for(auto& par : state.crisis_participants) {
				if(!par.id) {
					return false; // not in crisis
				}
				if(par.id == wg.get_wargoal().get_added_by()) {
					if(wg.get_wargoal().get_associated_state() == par.joined_with_offer.wargoal_state &&
							wg.get_wargoal().get_associated_tag() == par.joined_with_offer.wargoal_tag &&
							wg.get_wargoal().get_secondary_nation() == par.joined_with_offer.wargoal_secondary_nation &&
							wg.get_wargoal().get_target_nation() == par.joined_with_offer.target &&
							wg.get_wargoal().get_type() == par.joined_with_offer.wargoal_type)
						return true;
					else
						return false;
				}
			}
			return false;
		}();

		if(!found) {
			if(wg.get_wargoal().get_added_by() == state.primary_crisis_attacker) {
				if(state.current_crisis == sys::crisis_type::colonial) {
					auto colonizers = state.world.state_definition_get_colonization(state.crisis_colony);
					auto num_colonizers = colonizers.end() - colonizers.begin();
					if(num_colonizers >= 2) {
						auto col_a = (*colonizers.begin()).get_colonizer();
						auto col_b = (*(colonizers.begin() + 1)).get_colonizer();

						if(wg.get_wargoal().get_associated_state() == state.crisis_colony &&
								wg.get_wargoal().get_type() == state.military_definitions.crisis_colony &&
								((wg.get_wargoal().get_added_by() == col_a && wg.get_wargoal().get_target_nation() == col_b) ||
										(wg.get_wargoal().get_added_by() == col_b && wg.get_wargoal().get_target_nation() == col_a))) {

							found = true;
						}
					}
				} else if(state.current_crisis == sys::crisis_type::liberation) {
					if(wg.get_wargoal().get_associated_tag() == state.crisis_liberation_tag &&
							wg.get_wargoal().get_associated_state() == state.world.state_instance_get_definition(state.crisis_state) &&
							wg.get_wargoal().get_type() == state.military_definitions.crisis_liberate &&
							wg.get_wargoal().get_target_nation() ==
									state.world.state_instance_get_nation_from_state_ownership(state.crisis_state)) {

						found = true;
					}
				}
			}
		}

		if(!found)
			return false;
	}

	return true;
}

void accept(sys::state& state, message const& m) {
	switch(m.type) {
	case type::none:
		std::abort();
		break;
	case type::access_request: {
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
	case type::alliance_request: {
		if(!command::can_ask_for_alliance(state, m.from, m.to))
			return;

		nations::adjust_relationship(state, m.from, m.to, state.defines.alliance_relation_on_accept);
		nations::make_alliance(state, m.from, m.to);
		break;
	}
	case type::call_ally_request: {
		if(!command::can_call_to_arms(state, m.from, m.to, m.data.war))
			return;

		military::join_war(state, m.data.war, m.to, military::is_attacker(state, m.data.war, m.from));

		nations::adjust_relationship(state, m.from, m.to, state.defines.callally_relation_on_accept);
		break;
	}
	case type::be_crisis_primary_attacker:
		nations::add_as_primary_crisis_attacker(state, m.to);
		break;
	case type::be_crisis_primary_defender:
		nations::add_as_primary_crisis_defender(state, m.to);
		break;
	case type::peace_offer:
		military::implement_peace_offer(state, m.data.peace);
		break;
	case type::take_crisis_side_offer:
		if(can_accept_crisis_offer(state, m.from, m.to, m.data.crisis_offer)) {
			add_to_crisis_with_offer(state, m.from, m.to, m.data.crisis_offer);
			// TODO : notify offer accepted
		}
		break;
	case type::crisis_peace_offer:
		if(can_accept_crisis_peace_offer(state, m.from, m.to, m.data.peace)) {
			nations::accept_crisis_peace_offer(state, m.from, m.to, m.data.peace);
			// TODO: notify
		}
		break;
	}
}

void post(sys::state& state, message const& m) {
	if(state.world.nation_get_is_player_controlled(m.to) == false) {
		// TODO : call AI logic to decide responses to requests

		switch(m.type) {
		case type::none:
			std::abort();
			return;
		case type::access_request:
			accept(state, m);
			return;
		case type::alliance_request:
			accept(state, m);
			return;
		case type::call_ally_request:
			accept(state, m);
			return;
		case type::be_crisis_primary_defender:
			nations::add_as_primary_crisis_defender(state, m.to);
			return;
		case type::be_crisis_primary_attacker:
			nations::add_as_primary_crisis_attacker(state, m.to);
			return;
		case type::peace_offer:
			accept(state, m);
			return;
		case type::take_crisis_side_offer:
			accept(state, m);
			return;
		case type::crisis_peace_offer:
			accept(state, m);
			return;
		}
	}

	for(auto& i : state.pending_messages) {
		if(i.type == type::none) {
			std::memcpy(&i, &m, sizeof(message));
			i.when = state.current_date;

			if(i.to == state.local_player_nation) {
				state.new_requests.push(i);
			}

			return;
		}
	}
}

void update_pending(sys::state& state) {
	for(auto& m : state.pending_messages) {
		if(m.type != type::none && m.when + expiration_in_days <= state.current_date) {

			decline(state, m);
			m.type = type::none;
		}
	}
}

} // namespace diplomatic_message
