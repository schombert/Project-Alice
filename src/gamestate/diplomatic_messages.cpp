#include "diplomatic_messages.hpp"
#include "system_state.hpp"
#include "commands.hpp"
#include "nations.hpp"
#include "ai.hpp"

namespace diplomatic_message {

void decline(sys::state& state, message const& m) {
	switch(m.type) {
	case type::none:
		break;
	case type::access_request:
		if(!command::can_ask_for_access(state, m.from, m.to))
			return;

		nations::adjust_relationship(state, m.from, m.to, state.defines.askmilaccess_relation_on_decline);

		notification::post(state, notification::message{
			[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_access_refused_1", text::variable_type::x, target, text::variable_type::y, source);
			},
			"msg_access_refused_title",
			m.to,
			sys::message_setting_type::mil_access_declined_by_nation
		});
		notification::post(state, notification::message{
			[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_access_refused_1", text::variable_type::x, target, text::variable_type::y, source);
			},
			"msg_access_refused_title",
			m.from,
			sys::message_setting_type::mil_access_declined_on_nation
		});
		break;
	case type::alliance_request:
		if(!command::can_ask_for_alliance(state, m.from, m.to))
			return;

		nations::adjust_relationship(state, m.from, m.to, state.defines.alliance_relation_on_decline);

		notification::post(state, notification::message{
			[from = m.from, to = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_alliance_declined_1", text::variable_type::x, to, text::variable_type::y, from);
			},
			"msg_alliance_declined_title",
			m.to,
			sys::message_setting_type::alliance_declined_by_nation
		});
		notification::post(state, notification::message{
			[from = m.from, to = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_alliance_declined_1", text::variable_type::x, to, text::variable_type::y, from);
			},
			"msg_alliance_declined_title",
			m.from,
			sys::message_setting_type::alliance_declined_on_nation
		});
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
			if(rel && state.world.diplomatic_relation_get_are_allied(rel)) {
				state.world.diplomatic_relation_set_are_allied(rel, false);

				notification::post(state, notification::message{
					[from = m.from, to = m.to](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_alliance_ends_1", text::variable_type::x, to, text::variable_type::y, from);
					},
					"msg_alliance_ends_title",
					m.from,
					sys::message_setting_type::alliance_ends
				});

				notification::post(state, notification::message{
					[from = m.from, to = m.to](sys::state& state, text::layout_base& contents) {
						text::add_line(state, contents, "msg_alliance_ends_1", text::variable_type::x, to, text::variable_type::y, from);
					},
					"msg_alliance_ends_title",
					m.to,
					sys::message_setting_type::alliance_ends
				});
			}
		}

		notification::post(state, notification::message{
			[from = m.from, to = m.to, pa = state.world.war_get_primary_attacker(m.data.war), pd = state.world.war_get_primary_defender(m.data.war), name = state.world.war_get_name(m.data.war), tag = state.world.war_get_over_tag(m.data.war), st = state.world.war_get_over_state(m.data.war)](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_ally_call_decline_1", text::variable_type::x, to, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_ally_call_declined_title",
			m.from,
			sys::message_setting_type::ally_called_declined_by_nation
		});

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
		notification::post(state, notification::message{
			[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_crisis_joffer_reject_1", text::variable_type::x, target, text::variable_type::y, source);

			},
			"msg_crisis_joffer_rejected",
			m.to,
			sys::message_setting_type::crisis_join_offer_declined_by_nation
		});
		notification::post(state, notification::message{
			[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_crisis_joffer_reject_1", text::variable_type::x, target, text::variable_type::y, source);

			},
			"msg_crisis_joffer_rejected",
			m.from,
			sys::message_setting_type::crisis_join_offer_declined_from_nation
		});
		break;
	case type::crisis_peace_offer:
		// TODO: notify rejected
		/*
		Crisis resolution offers function much in the same way as peace offers. Every refused crisis offer increases the temperature
		of the current crisis by define:CRISIS_TEMPERATURE_ON_OFFER_DECLINE.
		*/
		state.crisis_temperature += state.defines.crisis_temperature_on_offer_decline;
		nations::cleanup_crisis_peace_offer(state, m.data.peace);

		notification::post(state, notification::message{
			[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_crisis_not_settled_1", text::variable_type::x, target, text::variable_type::y, source);

			},
			"msg_crisis_not_settled_title",
			m.from,
			sys::message_setting_type::crisis_resolution_declined_from_nation
		});

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

	auto cb_type = state.world.cb_type_get_type_bits(offer.wargoal_type);
	if((cb_type & military::cb_flag::always) == 0 && (cb_type & military::cb_flag::is_not_constructing_cb) != 0)
		return false;

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
		break;
	case type::access_request: {
		if(!command::can_ask_for_access(state, m.from, m.to, true))
			return;

		nations::adjust_relationship(state, m.from, m.to, state.defines.askmilaccess_relation_on_accept);
		auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(m.to, m.from);
		if(!rel) {
			rel = state.world.force_create_unilateral_relationship(m.to, m.from);
		}
		state.world.unilateral_relationship_set_military_access(rel, true);

		notification::post(state, notification::message{
			[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_access_granted_1", text::variable_type::x, target, text::variable_type::y, source);
			},
			"msg_access_granted_title",
			m.to,
			sys::message_setting_type::mil_access_start_by_nation
		});
		notification::post(state, notification::message{
			[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_access_granted_1", text::variable_type::x, target, text::variable_type::y, source);
			},
			"msg_access_granted_title",
			m.from,
			sys::message_setting_type::mil_access_start_on_nation
		});

		break;
	}
	case type::alliance_request: {
		if(!command::can_ask_for_alliance(state, m.from, m.to, true))
			return;

		nations::adjust_relationship(state, m.from, m.to, state.defines.alliance_relation_on_accept);
		nations::make_alliance(state, m.from, m.to);
		break;
	}
	case type::call_ally_request: {
		if(!command::can_call_to_arms(state, m.from, m.to, m.data.war, true))
			return;

		military::add_to_war(state, m.data.war, m.to, military::is_attacker(state, m.data.war, m.from));

		nations::adjust_relationship(state, m.from, m.to, state.defines.callally_relation_on_accept);

		notification::post(state, notification::message{
			[from = m.from, to = m.to, pa = state.world.war_get_primary_attacker(m.data.war), pd = state.world.war_get_primary_defender(m.data.war), name = state.world.war_get_name(m.data.war), tag = state.world.war_get_over_tag(m.data.war), st = state.world.war_get_over_state(m.data.war)](sys::state& state, text::layout_base& contents) {
				text::substitution_map sub;
				text::add_to_substitution_map(sub, text::variable_type::order, std::string_view(""));
				text::add_to_substitution_map(sub, text::variable_type::second, state.world.nation_get_adjective(pd));
				text::add_to_substitution_map(sub, text::variable_type::second_country, pd);
				text::add_to_substitution_map(sub, text::variable_type::first, state.world.nation_get_adjective(pa));
				text::add_to_substitution_map(sub, text::variable_type::third, tag);
				text::add_to_substitution_map(sub, text::variable_type::state, st);

				std::string resolved_war_name = text::resolve_string_substitution(state, name, sub);
				text::add_line(state, contents, "msg_ally_call_accepted_1", text::variable_type::x, to, text::variable_type::y, from, text::variable_type::val, std::string_view{resolved_war_name});
			},
			"msg_ally_call_accepted_title",
			m.from,
			sys::message_setting_type::ally_called_accepted_by_nation
		});

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

			notification::post(state, notification::message{
				[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, "msg_crisis_joffer_accepted_1", text::variable_type::x, target, text::variable_type::y, source);
					
				},
				"msg_crisis_joffer_accepted",
					m.to,
				sys::message_setting_type::crisis_join_offer_accepted_by_nation
			});
			notification::post(state, notification::message{
				[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, "msg_crisis_joffer_accepted_1", text::variable_type::x, target, text::variable_type::y, source);
					
				},
				"msg_crisis_joffer_accepted",
				m.from,
				sys::message_setting_type::crisis_join_offer_accepted_from_nation
			});
		}
		break;
	case type::crisis_peace_offer:
		if(can_accept_crisis_peace_offer(state, m.from, m.to, m.data.peace)) {
			nations::accept_crisis_peace_offer(state, m.from, m.to, m.data.peace);

			notification::post(state, notification::message{
				[source = m.from, target = m.to](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, "msg_crisis_settled_1", text::variable_type::x, target, text::variable_type::y, source);

				},
				"msg_crisis_settled_title",
				state.local_player_nation,
				sys::message_setting_type::crisis_resolution_accepted
			});
		}
		break;
	}
}

bool ai_will_accept(sys::state& state, message const& m) {
	if(m.from == state.local_player_nation && state.cheat_data.always_accept_deals)
		return true;

	switch(m.type) {
		case type::none:
			std::abort();
			break;
		case type::access_request:
			return ai::ai_will_grant_access(state, m.to, m.from);
		case type::alliance_request:
			return ai::ai_will_accept_alliance(state, m.to, m.from);
		case type::call_ally_request:
			if(!command::can_call_to_arms(state, m.from, m.to, m.data.war))
				return false;
			return ai::will_join_war(state, m.to, m.data.war, military::get_role(state, m.data.war, m.from) == military::war_role::attacker);
		case type::be_crisis_primary_defender:
			return ai::will_be_crisis_primary_defender(state, m.to);
		case type::be_crisis_primary_attacker:
			return ai::will_be_crisis_primary_attacker(state, m.to);
		case type::peace_offer:
			return ai::will_accept_peace_offer(state, m.to, m.from, m.data.peace);
		case type::take_crisis_side_offer:
			return ai::will_join_crisis_with_offer(state, m.to, m.data.crisis_offer);
		case type::crisis_peace_offer:
			return ai::will_accept_crisis_peace_offer(state, m.to, m.data.peace);
	}
	return false;
}

void post(sys::state& state, message const& m) {
	if(state.world.nation_get_is_player_controlled(m.to) == false) {
		if(ai_will_accept(state, m)) {
			accept(state, m);
		} else {
			decline(state, m);
		}
		return;
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
