#include "commands.hpp"
#include "demographics.hpp"
#include "economy_templates.hpp"
#include "economy_stats.hpp"
#include "construction.hpp"
#include "effects.hpp"
#include "gui_event.hpp"
#include "serialization.hpp"
#include "system_state.hpp"
#include "nations.hpp"
#include "nations_templates.hpp"
#include "politics.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "triggers.hpp"
#include "ai.hpp"
#include "gui_console.hpp"
#include "network.hpp"
#include "economy_government.hpp"
#include "gui_error_window.hpp"
#include "diplomatic_messages.hpp"
#include "game_scene.hpp"
#include "province.hpp"
#include "economy.hpp"
#include "economy_factory_view.hpp"

namespace command {

bool is_console_command(command_type t) {
	return uint8_t(t) == 255;
}

void add_to_command_queue(sys::state& state, command_data& p) {
#ifndef NDEBUG
	assert(command::can_perform_command(state, p));
#endif

	switch(p.header.type) {

	case command_type::notify_player_joins:
	case command_type::notify_player_leaves:
	case command_type::notify_player_picks_nation:
	case command_type::notify_player_ban:
	case command_type::notify_player_kick:
	case command_type::notify_save_loaded:
	case command_type::notify_reload:
	case command_type::notify_player_oos:
	case command_type::notify_pause_game:
	case command_type::notify_player_fully_loaded:
	case command_type::notify_player_is_loading:
	case command_type::chat_message:
	case command_type::change_ai_nation_state:
	case command_type::notify_start_game:
	case command_type::notify_stop_game:
	case command_type::resync_lobby:
	case command_type::notify_oos_gamestate:
		// Notifications can be sent because it's an-always do thing
		break;
	case command_type::change_game_rule_setting:
	    // changing game rule can not happen whilst the game is in progress
		if(state.current_scene.game_in_progress || network::check_any_players_loading(state))
			return;
		state.network_state.is_new_game = false;
		break;

	default:
		// Normal commands are discarded iff we are not in the game, or if any other client is loading
		if(!state.current_scene.game_in_progress || network::check_any_players_loading(state))
			return;
		state.network_state.is_new_game = false;
		break;
	}

	switch(state.network_mode) {
	case sys::network_mode_type::single_player:
	{
		bool b = state.incoming_commands.try_push(p);
		break;
	}
	case sys::network_mode_type::client:
	case sys::network_mode_type::host:
	{
		if(is_console_command(p.header.type))
			break;
		state.network_state.outgoing_commands.push(p);
		break;
	}
	default:
		break;
	}
}

//size_t command_size(command_type type) {
//	switch(type) {
//	case command_type::change_nat_focus:
//		return sizeof(change_nat_focus);
//	}
//}
//std::unique_ptr<command> create_command(command_type type) {
//	switch(type) {
//	case command_type::change_nat_focus:
//		return std::make_unique<change_nat_focus>(type);
//	}
//}
//
//std::unique_ptr<uint8_t> command_data::serialize() {
//	std::unique_ptr<uint8_t> data(new uint8_t[size()])
//	switch(type) {
//	case:
//	}
//}
//size_t command_data::size() {
//	size_t size = sizeof(command_data) - sizeof(cmd_payload);
//	size += std::visit([](auto&& arg) {
//		using T = std::decay_t<decltype(arg)>;
//		return sizeof(T);
//	}, payload);
//	return size;
//}


void set_rally_point(sys::state& state, dcon::nation_id source, dcon::province_id location, bool naval, bool enable) {
	command_data p{ command_type::set_rally_point, state.local_player_id };
	auto data = rally_point_data{ location, naval, enable };
	p << data;
	add_to_command_queue(state, p);
}

void execute_set_rally_point(sys::state& state, dcon::nation_id source, dcon::province_id location, bool naval, bool enable) {
	if(state.world.province_get_nation_from_province_ownership(location) != source)
		return;
	if(naval) {
		if(state.world.province_get_is_coast(location))
			state.world.province_set_naval_rally_point(location, enable);
	} else {
		state.world.province_set_land_rally_point(location, enable);
	}
}

void save_game(sys::state& state, dcon::nation_id source, bool and_quit, const std::string& filename) {
	command_data p{ command_type::save_game, state.local_player_id };
	uint8_t truncated_length = std::min<uint8_t>(uint8_t(filename.length()), std::numeric_limits<uint8_t>::max());
	auto data = save_game_data{ and_quit,  truncated_length };
	p << data;

	p.push_ptr(filename.data(), truncated_length);
	add_to_command_queue(state, p);

}

bool can_save_game(sys::state& state, command_data& command) {
	auto& payload = command.get_payload<save_game_data>();

	// check that the filename length is correct before reading from it
	if(!command.check_variable_size_payload<save_game_data>(payload.filename_len)) {
		assert(false && "Variable command with a inconsistent size recieved!");
		return false;
	}

	return true;
}

void execute_save_game(sys::state& state, dcon::nation_id source, bool and_quit, const std::string& filename) {
	sys::write_save_file(state, sys::save_type::normal, "", filename);

	if(and_quit) {
		window::close_window(state);
	}
}

void set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state,
		dcon::national_focus_id focus) {

	command_data p{ command_type::change_nat_focus, state.local_player_id };
	auto data = national_focus_data{ target_state, focus };
	p << data;
	add_to_command_queue(state, p);

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
			auto prov = state.world.state_instance_get_capital(target_state);
			auto k = state.world.national_focus_get_limit(focus);
			if(k && !trigger::evaluate(state, k, trigger::to_generic(prov), trigger::to_generic(state_owner), -1))
				return false;
			return num_focuses_set < num_focuses_total || bool(state.world.state_instance_get_owner_focus(target_state));
		} else if (focus == state.national_definitions.flashpoint_focus) {
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
			bool rank_high = state.world.nation_get_rank(source) > uint16_t(state.defines.colonial_rank);
			return state_contains_core && rank_high &&
				(num_focuses_set < num_focuses_total || bool(state.world.nation_get_state_from_flashpoint_focus(source))) &&
				bool(state.world.state_instance_get_nation_from_flashpoint_focus(target_state)) == false;
		} else {
			return false;
		}
	}
}

void execute_set_national_focus(sys::state& state, dcon::nation_id source, dcon::state_instance_id target_state, dcon::national_focus_id focus) {
	if(state.world.state_instance_get_nation_from_state_ownership(target_state) == source) {
		state.world.state_instance_set_owner_focus(target_state, focus);
	} else {
		if(focus)
			state.world.nation_set_state_from_flashpoint_focus(source, target_state);
		else
			state.world.nation_set_state_from_flashpoint_focus(source, dcon::state_instance_id{});
	}
}

void start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech) {

	command_data p{ command_type::start_research, state.local_player_id };
	auto data = start_research_data{ tech };
	p << data;
	add_to_command_queue(state, p);
}

bool can_start_research(sys::state& state, dcon::nation_id source, dcon::technology_id tech) {
	/* Nations can only start researching technologies if, they are not uncivilized, the tech
		 activation date is past by, and all the previous techs (if any) of the same folder index
		 are already researched fully. And they are not already researched. */
	if(!tech)
		return false;
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
	if(state.world.nation_get_current_research(source))
		state.world.nation_set_research_points(source, 0.0f);
	state.world.nation_set_current_research(source, tech);
}

void make_leader(sys::state& state, dcon::nation_id source, bool general) {
	command_data p{ command_type::make_leader, state.local_player_id };
	auto data = make_leader_data{ general };
	p << data;
	add_to_command_queue(state, p);

}
bool can_make_leader(sys::state& state, dcon::nation_id source, bool general) {
	return state.world.nation_get_leadership_points(source) >= state.defines.leader_recruit_cost;
}
void execute_make_leader(sys::state& state, dcon::nation_id source, bool general) {
	military::make_new_leader(state, source, general);
}

void set_factory_type_priority(sys::state& state, dcon::nation_id source, dcon::factory_type_id ftid, float value) {

	command_data p{ command_type::set_factory_type_priority, state.local_player_id };
	auto data = set_factory_priority_data{ ftid, value };
	p << data;
	add_to_command_queue(state, p);

};
bool can_set_factory_type_priority(sys::state& state, dcon::nation_id source, dcon::factory_type_id ftid, float value) {
	return (value >= 0.f);
};
void execute_set_factory_type_priority(sys::state& state, dcon::nation_id source, dcon::factory_type_id ftid, float value) {
	state.world.nation_set_factory_type_experience_priority_national(source, ftid, value);
}


void give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {

	command_data p{ command_type::war_subsidies, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

}
bool can_give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	/* Can only perform if, the nations are not at war, the nation isn't already being given war subsidies, and there is
	 * defines:WARSUBSIDY_DIPLOMATIC_COST diplomatic points available. And the target isn't equal to the sender. */
	if(source == target)
		return false; // Can't negotiate with self
	if(military::are_at_war(state, source, target))
		return false; // Can't be at war
	//if(!state.world.nation_get_is_at_war(target))
	//	return false; // target must be at war
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(rel && state.world.unilateral_relationship_get_war_subsidies(rel))
		return false; // Can't already be giving war subsidies

	if(state.world.nation_get_is_player_controlled(source))
		return state.world.nation_get_diplomatic_points(source) >= state.defines.warsubsidy_diplomatic_cost; // Enough diplomatic points
	return true;
}
void execute_give_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	nations::adjust_relationship(state, source, target, state.defines.warsubsidy_relation_on_accept);
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.warsubsidy_diplomatic_cost);
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(!rel)
		rel = state.world.force_create_unilateral_relationship(target, source);

	state.world.unilateral_relationship_set_war_subsidies(rel, true);

	notification::post(state, notification::message{
		[source, target](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_wsub_start_1", text::variable_type::x, source, text::variable_type::y, target);
		},
		"msg_wsub_start_title",
		source, target, dcon::nation_id{},
		sys::message_base_type::war_subsidies_start,
		dcon::province_id{ }
	});
}


void cancel_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {


	command_data p{ command_type::cancel_war_subsidies, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

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
	if(state.world.nation_get_is_player_controlled(source))
		return state.world.nation_get_diplomatic_points(source) >= state.defines.cancelwarsubsidy_diplomatic_cost; // Enough diplomatic points
	return true;
}
void execute_cancel_war_subsidies(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	nations::adjust_relationship(state, source, target, state.defines.cancelwarsubsidy_relation_on_accept);
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.cancelwarsubsidy_diplomatic_cost);
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(rel)
		state.world.unilateral_relationship_set_war_subsidies(rel, false);

	if(source != state.local_player_nation) {
		notification::post(state, notification::message{
			[source, target](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_wsub_end_1", text::variable_type::x, source, text::variable_type::y, target);
			},
			"msg_wsub_end_title",
			source, target, dcon::nation_id{},
			sys::message_base_type::war_subsidies_end, dcon::province_id{ }
		});
	}
}


void increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {

	command_data p{ command_type::increase_relations, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

}
bool can_increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	/* Can only perform if, the nations are not at war, the relation value isn't maxed out at 200, and has
	 * defines:INCREASERELATION_DIPLOMATIC_COST diplomatic points. And the target can't be the same as the sender. */

	if(source == target)
		return false; // Can't negotiate with self

	if(military::are_at_war(state, source, target))
		return false; // Can't be at war

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(source, target);
	if(state.world.diplomatic_relation_get_value(rel) >= 200.f)
		return false; // Maxed out

	if(state.world.nation_get_is_player_controlled(source))
		return state.world.nation_get_diplomatic_points(source) >= state.defines.increaserelation_diplomatic_cost; // Enough diplomatic points
	return true;
}
void execute_increase_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	nations::adjust_relationship(state, source, target, state.defines.increaserelation_relation_on_accept);
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.increaserelation_diplomatic_cost);

	notification::post(state, notification::message{
		[source, target](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_inc_rel_1", text::variable_type::x, source, text::variable_type::y, target);
		},
		"msg_inc_rel_title",
		source, target, dcon::nation_id{},
		sys::message_base_type::increase_relation, dcon::province_id{ }
	});
}

void decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {

	command_data p{ command_type::decrease_relations, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

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
	if(state.world.nation_get_is_player_controlled(source))
		return state.world.nation_get_diplomatic_points(source) >= state.defines.decreaserelation_diplomatic_cost; // Enough diplomatic points
	return true;
}
void execute_decrease_relations(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	nations::adjust_relationship(state, source, target, state.defines.decreaserelation_relation_on_accept);
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.decreaserelation_diplomatic_cost);

	notification::post(state, notification::message{
		[source, target](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_dec_rel_1", text::variable_type::x, source, text::variable_type::y, target);
		},
		"msg_dec_rel_title",
		source, target, dcon::nation_id{},
		sys::message_base_type::decrease_relation, dcon::province_id{ }
	});
}

void begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id prov, economy::province_building_type type) {

	command_data p{ command_type::begin_province_building_construction, state.local_player_id };
	auto data = province_building_data{ prov, type };
	p << data;
	add_to_command_queue(state, p);

}
bool can_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type) {

	switch(type) {
	case economy::province_building_type::railroad:
		return province::can_build_railroads(state, p, source);
	case economy::province_building_type::fort:
		return province::can_build_fort(state, p, source);
	case economy::province_building_type::naval_base:
		return province::can_build_naval_base(state, p, source);
	case economy::province_building_type::bank:
	case economy::province_building_type::university:
		return province::can_build_province_building(state, p, source, type);
	default:
		return false;
	}
}
void execute_begin_province_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id p, economy::province_building_type type) {
	if(type == economy::province_building_type::naval_base) {
		auto si = state.world.province_get_state_membership(p);
		if(si)
			si.set_naval_base_is_taken(true);
	}

	if(type != economy::province_building_type::fort && type != economy::province_building_type::naval_base && source != state.world.province_get_nation_from_province_ownership(p)) {
		float amount = 0.0f;
		auto& base_cost = state.economy_definitions.building_definitions[int32_t(type)].cost;
		for(uint32_t j = 0; j < economy::commodity_set::set_size; ++j) {
			if(base_cost.commodity_type[j]) {
				amount += base_cost.commodity_amounts[j] * state.world.commodity_get_cost(base_cost.commodity_type[j]); //base cost
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


void cancel_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
	command_data p{ command_type::cancel_factory_building_construction, state.local_player_id };
	auto data = factory_building_data{ };
	data.location = location;
	data.type = type;
	p << data;
	add_to_command_queue(state, p);

}
bool can_cancel_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	for(auto c : state.world.province_get_factory_construction(location)) {
		if(c.get_type() == type) {
			if(c.get_is_pop_project())
				return false;
			if(c.get_nation() != source)
				return false;
			return true;
		}
	}
	return false;
}
void execute_cancel_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type) {
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	for(auto c : state.world.province_get_factory_construction(location)) {
		if(c.get_type() == type) {
			if(c.get_is_pop_project())
				return;
			if(c.get_nation() != source)
				return;

			state.world.delete_factory_construction(c);
			return;
		}
	}
}
void begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type, bool is_upgrade, dcon::factory_type_id refit_target) {

	command_data p{ command_type::begin_factory_building_construction, state.local_player_id };
	auto data = factory_building_data{ location, type, is_upgrade, refit_target };
	p << data;
	add_to_command_queue(state, p);

}

bool can_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type, bool is_upgrade, dcon::factory_type_id refit_target) {

	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto sid = state.world.province_get_state_membership(location);

	/*
	The factory building must be unlocked by the nation.
	Factories cannot be built in a colonial state.
	*/

	if(!state.world.nation_get_active_building(source, type) && !state.world.factory_type_get_is_available_from_start(type))
		return false;
	if(!economy::can_build_factory_in_colony(state, state.world.state_instance_get_capital(sid)))
		return false;

	// New factory construction
	if(!is_upgrade && !refit_target) {
		// Disallow building in colonies unless define flag is set
		if(economy::is_colony(state, sid) && !economy::can_build_factory_type_in_colony(state, sid, type))
			return false;
		/* There can't be duplicate factories */
		// Check factories being built
		bool has_dup = false;
		economy::for_each_new_factory(state, location, [&](economy::new_factory const& nf) { has_dup = has_dup || nf.type == type; });
		if(has_dup)
			return false;

		// Check actual factories
		for(auto f : state.world.province_get_factory_location(location))
			if(f.get_factory().get_building_type() == type)
				return false;
	}

	// Factory refit from one type into another
	if(refit_target) {
		if(type == refit_target) {
			return false;
		}

		if(owner != source) {
			return false;
		}

		// Refit target must be unlocked and available
		if(!state.world.nation_get_active_building(source, refit_target) && !state.world.factory_type_get_is_available_from_start(refit_target))
			return false;

		// Disallow building in colonies unless define flag is set
		if(economy::is_colony(state, sid) && !economy::can_build_factory_type_in_colony(state, sid, refit_target))
			return false;

		// Check if this factory is already being refit
		bool has_dup = false;
		economy::for_each_upgraded_factory(state, location, [&](economy::upgraded_factory const& nf) { has_dup = has_dup || nf.type == type; });
		if(has_dup)
			return false;

		// We deliberately allow for duplicates to existing factories as this scenario is handled when construction is finished
	}

	if(state.world.nation_get_is_civilized(source) == false)
		return false;

	// If Foreign target
	if(owner != source) {
		/*
		For foreign investment: the target nation must allow foreign investment, the nation doing the investing must be a great
		power while the target is not a great power, and the nation doing the investing must not be at war with the target nation.
		The nation being invested in must be civilized.
		Overlord can invest in its subjects ignoring GP and owner's rules.
		*/
		auto rel = state.world.nation_get_overlord_as_subject(owner);
		auto overlord = state.world.overlord_get_ruler(rel);
		if(overlord != source) {
			if(state.world.nation_get_is_great_power(source) == false || state.world.nation_get_is_great_power(owner) == true)
				return false;
			auto rules = state.world.nation_get_combined_issue_rules(owner);
			if((rules & issue_rule::allow_foreign_investment) == 0)
				return false;
		}
		if(state.world.nation_get_is_civilized(owner) == false)
			return false;

		if(military::are_at_war(state, source, owner))
			return false;

		// Refit in foreign countries is not allowed
		if(refit_target) {
			return false;
		}
	}
	// Else Internal target
	else {
		/*
		The nation must have the rule set to allow building / upgrading if this is a domestic target.
		*/
		auto rules = state.world.nation_get_combined_issue_rules(owner);
		if(is_upgrade) {
			if((rules & issue_rule::expand_factory) == 0)
				return false;
		} else if (refit_target) {
			if((rules & issue_rule::build_factory) != 0) {
				// In state capitalism economies, any factory can be refitted into any type.
			}
			else {
				// For capitalist economies, during refit FROM and TO types must match in output good or inputs.
				auto output_1 = state.world.factory_type_get_output(type);
				auto output_2 = state.world.factory_type_get_output(refit_target);
				auto inputs_1 = state.world.factory_type_get_inputs(type);
				auto inputs_2 = state.world.factory_type_get_inputs(refit_target);
				auto inputs_match = true;

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					auto input_1 = inputs_1.commodity_type[i];
					auto input_2 = inputs_2.commodity_type[i];

					if(input_1 != input_2) {
						inputs_match = false;
						break;
					}
				}
				if(output_1 != output_2 && !inputs_match) {
					return false;
				}
			}
		} else {
			if((rules & issue_rule::build_factory) == 0)
				return false;
		}
	}

	/* If mod uses Factory Province limits */
	// Upgrade
	if(is_upgrade) {
		if(!economy::do_resource_potentials_allow_upgrade(state, source, location, type)) {
			return false;
		}
	}
	// Refit into another factory type
	else if(refit_target) {
		if(!economy::do_resource_potentials_allow_refit(state, source, location, type, refit_target)) {
			return false;
		}
	}
	// Construction
	else {
		if(!economy::do_resource_potentials_allow_construction(state, source, location, type)) {
			return false;
		}
	}

	// Factory Upgrade
	if(is_upgrade) {
		// no double upgrade
		for(auto p : state.world.province_get_factory_construction(location)) {
			if(p.get_type() == type)
				return false;
		}

		// Disallow building in colonies unless define flag is set
		if(economy::is_colony(state, sid) && !economy::can_build_factory_type_in_colony(state, sid, type))
			return false;

		// must already exist as a factory
		// For upgrades: no upgrading past max level.
		for(auto f : state.world.province_get_factory_location(location))
			if(f.get_factory().get_building_type() == type)
				return true;
		return false;
	} else {
		// coastal factories must be built on coast
		if(state.world.factory_type_get_is_coastal(type)) {
			if(!state.world.province_get_port_to(location))
				return false;
		}

		int32_t num_factories = economy::province_factory_count(state, location);
		return num_factories < int32_t(state.defines.factories_per_state);
	}
}

void execute_begin_factory_building_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::factory_type_id type, bool is_upgrade, dcon::factory_type_id refit_target) {
	auto new_up = fatten(state.world, state.world.force_create_factory_construction(location, source));
	new_up.set_is_pop_project(false);
	new_up.set_is_upgrade(is_upgrade);
	new_up.set_type(type);
	new_up.set_refit_target(refit_target);

	if(source != state.world.province_get_nation_from_province_ownership(location)) {
		float amount = 0.0f;
		auto& base_cost = state.world.factory_type_get_construction_costs(type);
		for(uint32_t j = 0; j < economy::commodity_set::set_size; ++j) {
			if(base_cost.commodity_type[j]) {
				amount += base_cost.commodity_amounts[j] * state.world.commodity_get_cost(base_cost.commodity_type[j]); //base cost
			} else {
				break;
			}
		}
		nations::adjust_foreign_investment(state, source, state.world.province_get_nation_from_province_ownership(location), amount);
	}
}

void start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type, dcon::province_id template_province) {

	command_data p{ command_type::begin_naval_unit_construction, state.local_player_id };
	auto data = naval_unit_construction_data{ location, type, template_province };
	p << data;
	add_to_command_queue(state, p);


}

bool can_start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type, dcon::province_id template_province) {
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
	auto disarm = state.world.nation_get_disarmed_until(source);
	if(disarm && state.current_date < disarm)
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

		if(state.world.province_get_building_level(location, uint8_t(economy::province_building_type::naval_base)) < level_req)
			return false;

		if(!overseas_allowed && province::is_overseas(state, location))
			return false;

		return true;
	}
}

void execute_start_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type, dcon::province_id template_province) {
	auto c = fatten(state.world, state.world.try_create_province_naval_construction(location, source));
	c.set_type(type);
	c.set_start_date(state.current_date);
	c.set_template_province(template_province);
}

void start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type, dcon::province_id template_province) {


	command_data p{ command_type::begin_land_unit_construction, state.local_player_id };
	auto data = land_unit_construction_data{ location, soldier_culture, type, template_province };
	p << data;
	add_to_command_queue(state, p);

}
bool can_start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type, dcon::province_id template_province) {
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
	if(state.military_definitions.unit_base_definitions[type].primary_culture && soldier_culture != state.world.nation_get_primary_culture(source) && state.world.nation_get_accepted_cultures(source, soldier_culture) == false) {
		return false;
	}
	auto disarm = state.world.nation_get_disarmed_until(source);
	if(disarm && state.current_date < disarm)
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
void execute_start_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type, dcon::province_id template_province) {
	auto soldier = military::find_available_soldier(state, location, soldier_culture);

	auto c = fatten(state.world, state.world.try_create_province_land_construction(soldier, source));
	c.set_start_date(state.current_date);
	c.set_type(type);
	c.set_template_province(template_province);
}

void cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {

	command_data p{ command_type::cancel_naval_unit_construction, state.local_player_id };
	auto data = naval_unit_construction_data{  };
	data.location = location;
	data.type = type;
	p << data;
	add_to_command_queue(state, p);
}

bool can_cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	return state.world.province_get_nation_from_province_ownership(location) == source;
}

void execute_cancel_naval_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::unit_type_id type) {
	dcon::province_naval_construction_id c;
	for(auto lc : state.world.province_get_province_naval_construction(location)) {
		if(lc.get_type() == type) {
			c = lc.id;
		}
	}
	state.world.delete_province_naval_construction(c);
}

void cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type) {


	command_data p{ command_type::cancel_land_unit_construction, state.local_player_id };
	auto data = land_unit_construction_data{ };
	data.location = location;
	data.pop_culture = soldier_culture;
	data.type = type;
	p << data;
	add_to_command_queue(state, p);

}
bool can_cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type) {
	return state.world.province_get_nation_from_province_ownership(location) == source;
}
void execute_cancel_land_unit_construction(sys::state& state, dcon::nation_id source, dcon::province_id location, dcon::culture_id soldier_culture, dcon::unit_type_id type) {
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


	command_data p{ command_type::delete_factory, state.local_player_id };
	auto data = factory_data{  };
	data.id = f;
	p << data;
	add_to_command_queue(state, p);

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
void execute_delete_factory(sys::state& state, dcon::nation_id source, dcon::factory_id fid) {
	// Delete existing factories
	state.world.delete_factory(fid);
}

void change_factory_settings(sys::state& state, dcon::nation_id source, dcon::factory_id f, uint8_t priority, bool subsidized) {

	command_data p{ command_type::change_factory_settings, state.local_player_id };
	auto data = factory_data{ f, priority, subsidized };
	p << data;
	add_to_command_queue(state, p);
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
void execute_change_factory_settings(sys::state& state, dcon::nation_id source, dcon::factory_id fid, uint8_t priority, bool subsidized) {
	auto rules = state.world.nation_get_combined_issue_rules(source);

	if(subsidized && (rules & issue_rule::can_subsidise) == 0) {
		return;
	}

	auto f = dcon::fatten(state.world, fid);

	auto current_priority = economy::factory_priority(state, f);
	if(current_priority != priority) {
		if((rules & issue_rule::factory_priority) == 0)
			return;
		economy::set_factory_priority(state, f, priority);
	}
	if(subsidized && !f.get_subsidized()) {
		auto& scale = f.get_primary_employment();
		f.set_primary_employment(std::max(scale, 0.5f));
	}
	f.set_subsidized(subsidized);
	return;
}

void make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {

	command_data p{ command_type::make_vassal, state.local_player_id };
	auto data = tag_target_data{  };
	data.ident = t;
	p << data;
	add_to_command_queue(state, p);
}
bool can_make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	return nations::can_release_as_vassal(state, source, t);
}
void execute_make_vassal(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {
	nations::liberate_nation_from(state, t, source);
	auto holder = state.world.national_identity_get_nation_from_identity_holder(t);
	state.world.force_create_overlord(holder, source);
	state.trade_route_cached_values_out_of_date = true;
	if(state.world.nation_get_is_great_power(source)) {
		nations::sphere_nation(state, holder, source);
	}
	nations::remove_cores_from_owned(state, holder, state.world.nation_get_identity_from_identity_holder(source));
	auto& inf = state.world.nation_get_infamy(source);
	state.world.nation_set_infamy(source, std::max(0.0f, inf + state.defines.release_nation_infamy));
	nations::adjust_prestige(state, source, state.defines.release_nation_prestige);
}

void release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t) {

	command_data p{ command_type::release_and_play_nation, state.local_player_id };
	auto data = tag_target_data{ t,};
	p << data;
	add_to_command_queue(state, p);

}
bool can_release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t, dcon::mp_player_id player) {
	return nations::can_release_as_vassal(state, source, t);
}
void execute_release_and_play_as(sys::state& state, dcon::nation_id source, dcon::national_identity_id t, dcon::mp_player_id player) {
	nations::liberate_nation_from(state, t, source);
	auto holder = state.world.national_identity_get_nation_from_identity_holder(t);
	nations::remove_cores_from_owned(state, holder, state.world.nation_get_identity_from_identity_holder(source));

	if(state.network_mode == sys::network_mode_type::single_player) {
		nations::switch_all_players(state, holder, source);
	}
	else {
		network::switch_one_player(state, holder, source, player);
	}


	for(auto p : state.world.nation_get_province_ownership(holder)) {
		auto pid = p.get_province();
		state.world.province_set_is_colonial(pid, false);
	}
}

inline bool can_change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values) {
	return true;
}

void change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values) {

	command_data p{ command_type::change_budget, state.local_player_id };
	p << values;
	add_to_command_queue(state, p);

}
void execute_change_budget_settings(sys::state& state, dcon::nation_id source, budget_settings_data const& values) {
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
	if(values.tariffs_import != int8_t(-127)) {
		state.world.nation_set_tariffs_import(source, std::clamp(values.tariffs_import, int8_t(-100), int8_t(100)));
	}
	if(values.tariffs_export != int8_t(-127)) {
		state.world.nation_set_tariffs_export(source, std::clamp(values.tariffs_export, int8_t(-100), int8_t(100)));
	}
	if(values.domestic_investment != int8_t(-127)) {
		state.world.nation_set_domestic_investment_spending(source, std::clamp(values.domestic_investment, int8_t(0), int8_t(100)));
	}
	if(values.overseas != int8_t(-127)) {
		state.world.nation_set_overseas_spending(source, std::clamp(values.overseas, int8_t(0), int8_t(100)));
	}
	economy::bound_budget_settings(state, source);
}

void start_election(sys::state& state, dcon::nation_id source) {
	command_data p{ command_type::start_election, state.local_player_id };
	add_to_command_queue(state, p);

}
bool can_start_election(sys::state& state, dcon::nation_id source) {
	auto type = state.world.nation_get_government_type(source);
	return state.world.government_type_get_has_elections(type) && !politics::is_election_ongoing(state, source);
}
void execute_start_election(sys::state& state, dcon::nation_id source) {
	politics::start_election(state, source);
}

void change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority) {


	command_data p{ command_type::change_influence_priority, state.local_player_id };
	auto data = influence_priority_data{ influence_target, priority };
	p << data;
	add_to_command_queue(state, p);

}
bool can_change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority) {
	// The source must be a great power, while the target must not be a great power.
	return state.world.nation_get_is_great_power(source) && !state.world.nation_get_is_great_power(influence_target);
}
void execute_change_influence_priority(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, uint8_t priority) {
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	if(!rel) {
		rel = state.world.force_create_gp_relationship(influence_target, source);
	}
	auto& flags = state.world.gp_relationship_get_status(rel);
	switch(priority) {
	case 0:
		state.world.gp_relationship_set_status(rel, uint8_t((flags & ~nations::influence::priority_mask) | nations::influence::priority_zero));
		break;
	case 1:
		state.world.gp_relationship_set_status(rel, uint8_t((flags & ~nations::influence::priority_mask) | nations::influence::priority_one));
		break;
	case 2:
		state.world.gp_relationship_set_status(rel, uint8_t((flags & ~nations::influence::priority_mask) | nations::influence::priority_two));
		break;
	case 3:
		state.world.gp_relationship_set_status(rel, uint8_t((flags & ~nations::influence::priority_mask) | nations::influence::priority_three));
		break;
	default:
		break;
	}
}

void discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {

	command_data p{ command_type::discredit_advisors, state.local_player_id };
	auto data = influence_action_data{ influence_target, affected_gp };
	p << data;
	add_to_command_queue(state, p);

}
bool can_discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
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
void execute_discredit_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	A nation is discredited for define:DISCREDIT_DAYS. Being discredited twice does not add these durations together; it just
	resets the timer from the current day. Discrediting a nation "increases" your relationship with them by
	define:DISCREDIT_RELATION_ON_ACCEPT. Discrediting costs define:DISCREDIT_INFLUENCE_COST influence points.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	if(!orel)
		orel = state.world.force_create_gp_relationship(influence_target, affected_gp);

	auto& current_influence = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, current_influence - state.defines.discredit_influence_cost);

	nations::adjust_relationship(state, source, affected_gp, state.defines.discredit_relation_on_accept);
	auto& current_status = state.world.gp_relationship_get_status(orel);
	state.world.gp_relationship_set_status(orel, uint8_t(current_status | nations::influence::is_discredited));
	state.world.gp_relationship_set_penalty_expires_date(orel, state.current_date + int32_t(state.defines.discredit_days));

	notification::post(state, notification::message{
		[source, influence_target, affected_gp, enddate = state.current_date + int32_t(state.defines.discredit_days)](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_discredit_1", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
			text::add_line(state, contents, "msg_discredit_2", text::variable_type::x, enddate);
		},
		"msg_discredit_title",
		source, affected_gp, influence_target,
		sys::message_base_type::discredit, dcon::province_id{ }
	});
}

void expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	command_data p{ command_type::expel_advisors, state.local_player_id };
	auto data = influence_action_data{ influence_target, affected_gp };
	p << data;
	add_to_command_queue(state, p);
}
bool can_expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
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
void execute_expel_advisors(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	Expelling a nation's advisors "increases" your relationship with them by define:EXPELADVISORS_RELATION_ON_ACCEPT. This action
	costs define:EXPELADVISORS_INFLUENCE_COST influence points. Being expelled cancels any ongoing discredit effect. Being
	expelled reduces your influence to zero.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);

	auto& current_influence = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, current_influence - state.defines.expeladvisors_influence_cost);

	nations::adjust_relationship(state, source, affected_gp, state.defines.expeladvisors_relation_on_accept);

	if(orel) {
		state.world.gp_relationship_set_influence(orel, 0.0f);
		auto& current_status = state.world.gp_relationship_get_status(orel);
		state.world.gp_relationship_set_status(orel, uint8_t(current_status & ~nations::influence::is_discredited));
	}

	notification::post(state, notification::message{
		[source, influence_target, affected_gp](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_expel_1", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
		},
		"msg_expel_title",
		source, affected_gp, influence_target,
		sys::message_base_type::expell, dcon::province_id{ }
	});
}

void ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {

	command_data p{ command_type::ban_embassy, state.local_player_id };
	auto data = influence_action_data{ influence_target, affected_gp };
	p << data;
	add_to_command_queue(state, p);
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
	if(clevel != nations::influence::level_friendly && clevel != nations::influence::level_in_sphere)
		return false;

	return nations::influence::is_influence_level_greater_or_equal(clevel,
			nations::influence::get_level(state, affected_gp, influence_target));
}
void execute_ban_embassy(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	Banning a nation's embassy "increases" your relationship with them by define:BANEMBASSY_RELATION_ON_ACCEPT. This action costs
	define:BANEMBASSY_INFLUENCE_COST influence points. The ban embassy effect lasts for define:BANEMBASSY_DAYS. If you are already
	banned, being banned again simply restarts the timer. Being banned cancels out any ongoing discredit effect.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	if(!orel)
		orel = state.world.force_create_gp_relationship(influence_target, affected_gp);

	auto& current_influence = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, current_influence - state.defines.banembassy_influence_cost);
	nations::adjust_relationship(state, source, affected_gp, state.defines.banembassy_relation_on_accept);
	auto& current_status = state.world.gp_relationship_get_status(orel);
	state.world.gp_relationship_set_status(orel, uint8_t(current_status | nations::influence::is_banned));
	state.world.gp_relationship_set_influence(orel, 0.0f);
	state.world.gp_relationship_set_penalty_expires_date(orel, state.current_date + int32_t(state.defines.banembassy_days));

	notification::post(state, notification::message{
		[source, influence_target, affected_gp, enddate = state.current_date + int32_t(state.defines.banembassy_days)](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_ban_1", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
			text::add_line(state, contents, "msg_ban_2", text::variable_type::x, enddate);
		},
		"msg_ban_title",
		source, affected_gp, influence_target,
		sys::message_base_type::ban, dcon::province_id{ }
	});
}

void increase_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {

	command_data p{ command_type::increase_opinion, state.local_player_id };
	auto data = influence_action_data{ };
	data.influence_target = influence_target;
	p << data;
	add_to_command_queue(state, p);

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
	if(clevel == nations::influence::level_friendly || clevel == nations::influence::level_in_sphere)
		return false;

	return true;
}
void execute_increase_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {
	/*
	Increasing the opinion of a nation costs define:INCREASEOPINION_INFLUENCE_COST influence points. Opinion can be increased to a
	maximum of friendly.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);

	auto& current_influence = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, current_influence - state.defines.increaseopinion_influence_cost);

	auto& l = state.world.gp_relationship_get_status(rel);
	state.world.gp_relationship_set_status(rel, uint8_t(nations::influence::increase_level(l)));

	notification::post(state, notification::message{
		[source, influence_target](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_op_inc_1", text::variable_type::x, source, text::variable_type::y, influence_target);
		},
		"msg_op_inc_title",
		source, influence_target, dcon::nation_id{},
		sys::message_base_type::increase_opinion, dcon::province_id{ }
	});
}

void decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	command_data p{ command_type::decrease_opinion, state.local_player_id };
	auto data = influence_action_data{ influence_target, affected_gp };
	p << data;
	add_to_command_queue(state, p);
}
bool can_decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
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
void execute_decrease_opinion(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	Decreasing the opinion of a nation "increases" your relationship with them by define:DECREASEOPINION_RELATION_ON_ACCEPT. This
	actions costs define:DECREASEOPINION_INFLUENCE_COST influence points. Opinion of the influenced nation of the secondary target
	decreases by one step.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	if(!orel)
		orel = state.world.force_create_gp_relationship(influence_target, affected_gp);

	auto& current_influence = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, current_influence - state.defines.decreaseopinion_influence_cost);
	nations::adjust_relationship(state, source, affected_gp, state.defines.decreaseopinion_relation_on_accept);

	auto& l = state.world.gp_relationship_get_status(orel);
	state.world.gp_relationship_set_status(orel, uint8_t(nations::influence::decrease_level(l)));

	notification::post(state, notification::message{
		[source, influence_target, affected_gp](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_op_dec_1", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
		},
		"msg_op_dec_title",
		source, affected_gp, influence_target,
		sys::message_base_type::decrease_opinion, dcon::province_id{ }
	});
}

void add_to_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target) {

	command_data p{ command_type::add_to_sphere, state.local_player_id };
	auto data = influence_action_data{ };
	data.influence_target = influence_target;
	p << data;
	add_to_command_queue(state, p);

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
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);

	auto& current_influence = state.world.gp_relationship_get_influence(rel);
	state.world.gp_relationship_set_influence(rel, current_influence - state.defines.addtosphere_influence_cost);

	nations::sphere_nation(state, influence_target, source);

	notification::post(state, notification::message{
		[source, influence_target](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_add_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target);
		},
		"msg_add_sphere_title",
		source, influence_target, dcon::nation_id{},
		sys::message_base_type::add_to_sphere, dcon::province_id{ }
	});
}

void remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {

	command_data p{ command_type::remove_from_sphere, state.local_player_id };
	auto data = influence_action_data{ influence_target, affected_gp };
	p << data;
	add_to_command_queue(state, p);

}
bool can_remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
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

	if(source != affected_gp && state.world.gp_relationship_get_influence(rel) < state.defines.removefromsphere_influence_cost)
		return false;

	if((state.world.gp_relationship_get_status(rel) & nations::influence::is_banned) != 0)
		return false;

	if(military::are_at_war(state, source, influence_target))
		return false;

	auto clevel = (nations::influence::level_mask & state.world.gp_relationship_get_status(rel));
	if(clevel != nations::influence::level_friendly && clevel != nations::influence::level_in_sphere)
		return false;

	return true;
}
void execute_remove_from_sphere(sys::state& state, dcon::nation_id source, dcon::nation_id influence_target, dcon::nation_id affected_gp) {
	/*
	Removing a nation from a sphere costs define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. If you remove a nation from
	your own sphere you lose define:REMOVEFROMSPHERE_PRESTIGE_COST prestige and gain define:REMOVEFROMSPHERE_INFAMY_COST infamy.
	Removing a nation from the sphere of another nation "increases" your relationship with the former sphere leader by
	define:REMOVEFROMSPHERE_RELATION_ON_ACCEPT points. The removed nation then becomes friendly with its former sphere leader.
	*/
	auto rel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, source);
	auto orel = state.world.get_gp_relationship_by_gp_influence_pair(influence_target, affected_gp);
	auto l = state.world.gp_relationship_get_status(orel);

	nations::remove_from_sphere(state, influence_target, uint8_t(nations::influence::decrease_level(l)));

	if(source != affected_gp) {
		auto& current_influence = state.world.gp_relationship_get_influence(rel);
		state.world.gp_relationship_set_influence(rel, current_influence - state.defines.removefromsphere_influence_cost);
		nations::adjust_relationship(state, source, affected_gp, state.defines.removefromsphere_relation_on_accept);
	} else {
		auto& current_infamy = state.world.nation_get_infamy(source);
		state.world.nation_set_infamy(source, current_infamy + state.defines.removefromsphere_infamy_cost);
		nations::adjust_prestige(state, source, -state.defines.removefromsphere_prestige_cost);
	}

	notification::post(state, notification::message{
		[source, influence_target, affected_gp](sys::state& state, text::layout_base& contents) {
			if(source == affected_gp)
				text::add_line(state, contents, "msg_rem_sphere_1", text::variable_type::x, source, text::variable_type::y, influence_target);
			else
				text::add_line(state, contents, "msg_rem_sphere_2", text::variable_type::x, source, text::variable_type::y, influence_target, text::variable_type::val, affected_gp);
		},
		"msg_rem_sphere_title",
		source, affected_gp, influence_target,
		sys::message_base_type::rem_sphere, dcon::province_id{ }
	});
}

void upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si) {


	command_data p{ command_type::upgrade_colony_to_state, state.local_player_id };
	auto data = generic_location_data{ state.world.state_instance_get_capital(si) };
	p << data;
	add_to_command_queue(state, p);

}
bool can_upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si) {
	return state.world.state_instance_get_nation_from_state_ownership(si) == source && province::can_integrate_colony(state, si);
}
void execute_upgrade_colony_to_state(sys::state& state, dcon::nation_id source, dcon::state_instance_id si) {
	province::upgrade_colonial_state(state, source, si);
}

void invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id pr) {

	command_data p{ command_type::invest_in_colony, state.local_player_id };
	auto data = generic_location_data{ pr };
	p << data;
	add_to_command_queue(state, p);

}
bool can_invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(p);
	if(!province::is_colonizing(state, source, state_def))
		return province::can_start_colony(state, source, state_def);
	else
		return province::can_invest_in_colony(state, source, state_def);
}
void execute_invest_in_colony(sys::state& state, dcon::nation_id source, dcon::province_id pr) {
	auto state_def = state.world.province_get_state_from_abstract_state_membership(pr);
	if(province::is_colonizing(state, source, state_def)) {
		province::increase_colonial_investment(state, source, state_def);
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

	command_data p{ command_type::abandon_colony, state.local_player_id };
	auto data = generic_location_data{ pr };
	p << data;
	add_to_command_queue(state, p);

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

void finish_colonization(sys::state& state, dcon::nation_id source, dcon::state_definition_id d) {

	command_data p{ command_type::finish_colonization, state.local_player_id };
	auto data = generic_state_definition_data { d };
	p << data;
	add_to_command_queue(state, p);

}
bool can_finish_colonization(sys::state& state, dcon::nation_id source, dcon::state_definition_id d) {
	if(state.world.state_definition_get_colonization_stage(d) != 3)
		return false;
	auto rng = state.world.state_definition_get_colonization(d);
	if(rng.begin() == rng.end())
		return false;
	return (*rng.begin()).get_colonizer() == source;
}
void execute_finish_colonization(sys::state& state, dcon::nation_id source, dcon::state_definition_id d) {
	for(auto pr : state.world.state_definition_get_abstract_state_membership(d)) {
		if(!pr.get_province().get_nation_from_province_ownership()) {
			province::change_province_owner(state, pr.get_province(), source);
		}
	}

	state.world.state_definition_set_colonization_temperature(d, 0.0f);
	state.world.state_definition_set_colonization_stage(d, uint8_t(0));

	auto rng = state.world.state_definition_get_colonization(d);

	while(rng.begin() != rng.end()) {
		state.world.delete_colonization(*rng.begin());
	}
}

void intervene_in_war(sys::state& state, dcon::nation_id source, dcon::war_id w, bool for_attacker) {


	command_data p{ command_type::intervene_in_war, state.local_player_id };
	auto data = war_target_data{ w, for_attacker };
	p << data;
	add_to_command_queue(state, p);

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
	if(!for_attacker && military::has_truce_with(state, source, state.world.war_get_primary_attacker(w)))
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

		auto defender = state.world.war_get_primary_defender(w);
		auto rel_w_defender = state.world.get_gp_relationship_by_gp_influence_pair(defender, source);
		auto inf = state.world.gp_relationship_get_status(rel_w_defender) & nations::influence::level_mask;
		if(inf != nations::influence::level_friendly && inf != nations::influence::level_in_sphere)
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
	return true;
}
void execute_intervene_in_war(sys::state& state, dcon::nation_id source, dcon::war_id w, bool for_attacker) {
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
			military::add_wargoal(state, w, source, state.world.war_get_primary_attacker(w), state.military_definitions.standard_status_quo, dcon::state_definition_id{},
					dcon::national_identity_id{}, dcon::nation_id{});
		}
	}

	military::add_to_war(state, w, source, for_attacker);
}

void suppress_movement(sys::state& state, dcon::nation_id source, dcon::movement_id m) {
	command_data p{ command_type::suppress_movement, state.local_player_id };
	auto data = movement_data{ state.world.movement_get_associated_issue_option(m), state.world.movement_get_associated_independence(m) };
	p << data;
	add_to_command_queue(state, p);

}
bool can_suppress_movement(sys::state& state, dcon::nation_id source, dcon::movement_id m) {
	if(state.world.movement_get_nation_from_movement_within(m) != source)
		return false;
	if(state.world.movement_get_pop_movement_membership(m).begin() == state.world.movement_get_pop_movement_membership(m).end())
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
	auto& cur_sup_points = state.world.nation_get_suppression_points(source);
	state.world.nation_set_suppression_points(source, cur_sup_points - rebel::get_suppression_point_cost(state, m));
	rebel::suppress_movement(state, source, m);
}

void civilize_nation(sys::state& state, dcon::nation_id source) {
	command_data p{ command_type::civilize_nation, state.local_player_id };
	add_to_command_queue(state, p);

}
bool can_civilize_nation(sys::state& state, dcon::nation_id source) {
	return state.world.nation_get_modifier_values(source, sys::national_mod_offsets::civilization_progress_modifier) >= 1.0f && !state.world.nation_get_is_civilized(source);
}
void execute_civilize_nation(sys::state& state, dcon::nation_id source) {
	nations::make_civilized(state, source);
}

void appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id pa) {

	command_data p{ command_type::appoint_ruling_party, state.local_player_id };
	auto data = political_party_data{ pa };
	p << data;
	add_to_command_queue(state, p);
}
bool can_appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id p) {
	/*
	The ideology of the ruling party must be permitted by the government form. There can't be an ongoing election. It can't be the
	current ruling party. The government must allow the player to set the ruling party. The ruling party can manually be changed
	at most once per year.
	*/
	if(state.world.nation_get_ruling_party(source) == p)
		return false;
	if(!politics::political_party_is_active(state, source, p))
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
	if((state.world.government_type_get_ideologies_allowed(gov) & ::culture::to_bits(new_ideology)) == 0) {
		return false;
	}

	return true;
}
void execute_appoint_ruling_party(sys::state& state, dcon::nation_id source, dcon::political_party_id p) {
	politics::appoint_ruling_party(state, source, p);
}

void enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r) {

	command_data p{ command_type::change_reform_option, state.local_player_id };
	auto data = reform_selection_data{ r };
	p << data;
	add_to_command_queue(state, p);

}
bool can_enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r) {
	if(source == state.local_player_nation && state.cheat_data.always_allow_reforms)
		return true;

	bool is_military = state.world.reform_get_reform_type(state.world.reform_option_get_parent_reform(r)) ==
		uint8_t(culture::issue_category::military);
	if(is_military)
		return politics::can_enact_military_reform(state, source, r);
	else
		return politics::can_enact_economic_reform(state, source, r);
}
void execute_enact_reform(sys::state& state, dcon::nation_id source, dcon::reform_option_id r) {
	nations::enact_reform(state, source, r);
	event::update_future_events(state);
}

void enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i) {

	command_data p{ command_type::change_issue_option, state.local_player_id };
	auto data = issue_selection_data{ i };
	p << data;
	add_to_command_queue(state, p);
}
bool can_enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i) {
	if(source == state.local_player_nation && state.cheat_data.always_allow_reforms)
		return true;

	auto type = state.world.issue_get_issue_type(state.world.issue_option_get_parent_issue(i));
	if(type == uint8_t(culture::issue_type::political))
		return politics::can_enact_political_reform(state, source, i);
	else if(type == uint8_t(culture::issue_type::social))
		return politics::can_enact_social_reform(state, source, i);
	else
		return false;
}
void execute_enact_issue(sys::state& state, dcon::nation_id source, dcon::issue_option_id i) {
	nations::enact_issue(state, source, i);
	event::update_future_events(state);
}

void become_interested_in_crisis(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::become_interested_in_crisis, state.local_player_id };
	add_to_command_queue(state, p);

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

	if(state.current_crisis_state != sys::crisis_state::heating_up)
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
	for(auto& i : state.crisis_participants) {
		if(!i.id) {
			i.id = source;
			i.merely_interested = true;
			return;
		}
	}
}

void take_sides_in_crisis(sys::state& state, dcon::nation_id source, bool join_attacker) {
	command_data p{ command_type::take_sides_in_crisis, state.local_player_id };
	auto data = crisis_join_data{ join_attacker };
	p << data;
	add_to_command_queue(state, p);

}
bool can_take_sides_in_crisis(sys::state& state, dcon::nation_id source, bool join_attacker) {
	/*
	Must not be involved in the crisis already. Must be interested in the crisis. Must be a great power. Must not be disarmed. The
	crisis must have already gotten its initial backers.
	*/

	if(state.current_crisis_state != sys::crisis_state::heating_up)
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
	for(auto& i : state.crisis_participants) {
		if(i.id == source) {
			i.merely_interested = false;
			i.supports_attacker = join_attacker;

			notification::post(state, notification::message{
				[source, join_attacker](sys::state& state, text::layout_base& contents) {
					text::add_line(state, contents, join_attacker ? "msg_crisis_vol_join_1" : "msg_crisis_vol_join_2", text::variable_type::x, source);
				},
				"msg_crisis_vol_join_title",
				source, dcon::nation_id{}, dcon::nation_id{},
				sys::message_base_type::crisis_voluntary_join, dcon::province_id{ }
			});

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

	command_data p{ command_type::change_stockpile_settings, state.local_player_id };
	auto data = stockpile_settings_data{ target_amount, c, draw_on_stockpiles  };
	p << data;
	add_to_command_queue(state, p);

}

void execute_change_stockpile_settings(sys::state& state, dcon::nation_id source, dcon::commodity_id c, float target_amount,
		bool draw_on_stockpiles) {
	state.world.nation_set_stockpile_targets(source, c, target_amount);
	state.world.nation_set_drawing_on_stockpiles(source, c, draw_on_stockpiles);
}

void take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d) {

	command_data p{ command_type::take_decision, state.local_player_id };
	auto data = decision_data{ d };
	p << data;
	add_to_command_queue(state, p);
}
bool can_take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d) {
	if(!(state.world.nation_get_is_player_controlled(source) && state.cheat_data.always_potential_decisions)) {
		auto condition = state.world.decision_get_potential(d);
		if(condition && !trigger::evaluate(state, condition, trigger::to_generic(source), trigger::to_generic(source), 0))
			return false;
	}
	if(!(state.world.nation_get_is_player_controlled(source) && state.cheat_data.always_allow_decisions)) {
		auto condition = state.world.decision_get_allow(d);
		if(condition && !trigger::evaluate(state, condition, trigger::to_generic(source), trigger::to_generic(source), 0))
			return false;
	}
	return true;
}
void execute_take_decision(sys::state& state, dcon::nation_id source, dcon::decision_id d) {
	nations::take_decision(state, source, d);
	if(auto e = state.world.decision_get_effect(d); e) {
		event::update_future_events(state);
	}
}

bool can_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_n_event_data const& e) {
	for(auto i = state.pending_n_event.size(); i-- > 0;) {
		if(event::pending_human_n_event{e.r_lo, e.r_hi, e.primary_slot, e.from_slot, e.date, e.e, source, e.pt, e.ft} == state.pending_n_event[i]) {
			if(e.opt_choice > state.world.national_event_get_options(e.e).size() || !event::is_valid_option(state.world.national_event_get_options(e.e)[e.opt_choice])) {
				return false; // invalid option
			}
			else {
				return true;
			}
		}
	}
	return false;
}

bool can_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_f_n_event_data const& e) {
	for(auto i = state.pending_f_n_event.size(); i-- > 0;) {
		if(event::pending_human_f_n_event{e.r_lo, e.r_hi, e.date, e.e, source } == state.pending_f_n_event[i]) {
			if(e.opt_choice > state.world.free_national_event_get_options(e.e).size() || !event::is_valid_option(state.world.free_national_event_get_options(e.e)[e.opt_choice])) {
				return false;
			}
			else {
				return true;
			}
		}
	}
	return false;
}

bool can_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_p_event_data const& e) {
	for(auto i = state.pending_p_event.size(); i-- > 0;) {
		if(event::pending_human_p_event{e.r_lo, e.r_hi, e.from_slot, e.date, e.e, e.p, e.ft } == state.pending_p_event[i]) {
			if(e.opt_choice > state.world.provincial_event_get_options(e.e).size() || !event::is_valid_option(state.world.provincial_event_get_options(e.e)[e.opt_choice])) {
				return false;
			} else {
				return true;
			}
		}
	}
	return false;
}


bool can_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_f_p_event_data const& e) {
	for(auto i = state.pending_f_p_event.size(); i-- > 0;) {
		if(event::pending_human_f_p_event{e.r_lo, e.r_hi, e.date, e.e, e.p } == state.pending_f_p_event[i]) {
			if(e.opt_choice > state.world.free_provincial_event_get_options(e.e).size() || !event::is_valid_option(state.world.free_provincial_event_get_options(e.e)[e.opt_choice])) {
				return false;
			} else {
				return true;
			}
		}
	}
	return false;
}




void make_event_choice(sys::state& state, event::pending_human_n_event const& e, uint8_t option_id) {
	command_data p{ command_type::make_n_event_choice, state.local_player_id };
	auto data = pending_human_n_event_data{e.r_lo, e.r_hi, e.primary_slot, e.from_slot, e.date, e.e, option_id, e.pt, e.ft };
	p << data;
	add_to_command_queue(state, p);

}
void make_event_choice(sys::state& state, event::pending_human_f_n_event const& e, uint8_t option_id) {


	command_data p{ command_type::make_f_n_event_choice, state.local_player_id };
	auto data = pending_human_f_n_event_data{ e.r_lo, e.r_hi, e.date, e.e, option_id };
	p << data;
	add_to_command_queue(state, p);

}
void make_event_choice(sys::state& state, event::pending_human_p_event const& e, uint8_t option_id) {

	command_data p{ command_type::make_p_event_choice, state.local_player_id };
	auto data = pending_human_p_event_data{ e.r_lo, e.r_hi, e.from_slot, e.date, e.e, e.p, option_id, e.ft };
	p << data;
	add_to_command_queue(state, p);

}
void make_event_choice(sys::state& state, event::pending_human_f_p_event const& e, uint8_t option_id) {

	command_data p{ command_type::make_f_p_event_choice, state.local_player_id };
	auto data = pending_human_f_p_event_data{ e.r_lo, e.r_hi, e.date, e.e, e.p, option_id };
	p << data;
	add_to_command_queue(state, p);

}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_n_event_data const& e) {
	event::take_option(state,
			event::pending_human_n_event {e.r_lo, e.r_hi, e.primary_slot, e.from_slot, e.date, e.e, source, e.pt, e.ft}, e.opt_choice);
	event::update_future_events(state);
}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_f_n_event_data const& e) {
	event::take_option(state, event::pending_human_f_n_event {e.r_lo, e.r_hi, e.date, e.e, source}, e.opt_choice);
	event::update_future_events(state);
}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_p_event_data const& e) {
	if(source != state.world.province_get_nation_from_province_ownership(e.p))
		return;

	event::take_option(state, event::pending_human_p_event {e.r_lo, e.r_hi, e.from_slot, e.date, e.e, e.p, e.ft}, e.opt_choice);
	event::update_future_events(state);
}
void execute_make_event_choice(sys::state& state, dcon::nation_id source, pending_human_f_p_event_data const& e) {
	if(source != state.world.province_get_nation_from_province_ownership(e.p))
		return;

	event::take_option(state, event::pending_human_f_p_event {e.r_lo, e.r_hi, e.date, e.e, e.p}, e.opt_choice);
	event::update_future_events(state);
}

void fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type, dcon::state_definition_id target_state) {

	command_data p{ command_type::fabricate_cb, state.local_player_id};
	auto data = cb_fabrication_data{target, type, target_state };
	p << data;
	add_to_command_queue(state, p);
}

bool valid_target_state_for_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type, dcon::state_definition_id target_state)
{
	auto actor = state.local_player_nation;
	dcon::cb_type_id cb = type;
	auto war = military::find_war_between(state, actor, target);
	auto allowed_substate_regions = state.world.cb_type_get_allowed_substate_regions(cb);
	if(allowed_substate_regions) {
		for(auto v : state.world.nation_get_overlord_as_ruler(target)) {
			if(v.get_subject().get_is_substate()) {
				for(auto si : state.world.nation_get_state_ownership(target)) {
					if(trigger::evaluate(state, allowed_substate_regions, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {
						auto def = si.get_state().get_definition().id;
						if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, v.get_subject(), cb, def, dcon::national_identity_id{}, dcon::nation_id{})) {
							return true;
						}
					}
				}
			}
		}
	} else {
		auto allowed_states = state.world.cb_type_get_allowed_states(cb);
		if(auto ac = state.world.cb_type_get_allowed_countries(cb); ac) {
			auto in_nation = target;
			auto target_identity = state.world.nation_get_identity_from_identity_holder(target);
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(in_nation))) {
					auto def = si.get_state().get_definition().id;
					if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, target, cb, def, target_identity, dcon::nation_id{})) {
						return true;
					}
				}
			}
		} else {
			for(auto si : state.world.nation_get_state_ownership(target)) {
				if(trigger::evaluate(state, allowed_states, trigger::to_generic(si.get_state().id), trigger::to_generic(actor), trigger::to_generic(actor))) {
					auto def = si.get_state().get_definition().id;
					if(!military::war_goal_would_be_duplicate(state, state.local_player_nation, war, target, cb, def, dcon::national_identity_id{}, dcon::nation_id{})) {
						return true;
					}
				}
			}
		}
	}

	return false;
}
bool can_fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type, dcon::state_definition_id target_state) {
	if(source == target)
		return false;

	if(!type) {
		return false;
	}

	if(state.world.nation_get_constructing_cb_type(source))
		return false;

	/*
	Can't fabricate on someone you are at war with. Requires
	defines:MAKE_CB_DIPLOMATIC_COST diplomatic points. Can't fabricate on your sphere members
	*/

	// Allow subjects to declare wars if can_use of CB definition allows for that as per Vanilla logic.
	auto ol = state.world.nation_get_overlord_as_subject(source);
	if(state.defines.alice_allow_subjects_declare_wars < 1 && state.world.overlord_get_ruler(ol) && state.world.overlord_get_ruler(ol) != target)
		return false;

	if(state.world.nation_get_in_sphere_of(target) == source)
		return false;

	if(state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < state.defines.make_cb_diplomatic_cost)
		return false;

	if(military::are_at_war(state, target, source))
		return false;

	if(military::has_truce_with(state, source, target))
		return false;

	/*
	must be able to fabricate cb
	*/

	auto bits = state.world.cb_type_get_type_bits(type);
	if((bits & (military::cb_flag::always | military::cb_flag::is_not_constructing_cb)) != 0)
		return false;

	if(!military::cb_conditions_satisfied(state, source, target, type))
		return false;

	if(military::cb_requires_selection_of_a_state(state, type) && target_state && !valid_target_state_for_cb(state, source, target, type, target_state)) {
		return false;
	}

	return true;
}

void execute_fabricate_cb(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id type, dcon::state_definition_id target_state = dcon::state_definition_id{}) {
	state.world.nation_set_constructing_cb_target(source, target);
	state.world.nation_set_constructing_cb_type(source, type);
	state.world.nation_set_constructing_cb_target_state(source, target_state);
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.make_cb_diplomatic_cost);
}

bool can_cancel_cb_fabrication(sys::state& state, dcon::nation_id source) {
	return true;
}

void cancel_cb_fabrication(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::cancel_cb_fabrication, state.local_player_id };
	add_to_command_queue(state, p);

}
void execute_cancel_cb_fabrication(sys::state& state, dcon::nation_id source) {
	state.world.nation_set_constructing_cb_target(source, dcon::nation_id{});
	state.world.nation_set_constructing_cb_is_discovered(source, false);
	state.world.nation_set_constructing_cb_progress(source, 0.0f);
	state.world.nation_set_constructing_cb_target_state(source, dcon::state_definition_id{});
	state.world.nation_set_constructing_cb_type(source, dcon::cb_type_id{});
}

void ask_for_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {

	command_data p{ command_type::ask_for_military_access, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

}
bool can_ask_for_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target, bool ignore_cost) {
	/*
	Must have defines:ASKMILACCESS_DIPLOMATIC_COST diplomatic points. Must not be at war against each other. Must not already have
	military access.
	*/
	if(asker == target)
		return false;

	if(!ignore_cost && state.world.nation_get_is_player_controlled(asker) && state.world.nation_get_diplomatic_points(asker) < state.defines.askmilaccess_diplomatic_cost)
		return false;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, asker);
	if(state.world.unilateral_relationship_get_military_access(rel))
		return false;

	if(military::are_at_war(state, asker, target))
		return false;

	return true;
}
void execute_ask_for_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	auto& current_diplo = state.world.nation_get_diplomatic_points(asker);
	state.world.nation_set_diplomatic_points(asker, current_diplo - state.defines.askmilaccess_diplomatic_cost);

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.type = diplomatic_message::type::access_request;

	diplomatic_message::post(state, m);
}

void give_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {

	command_data p{ command_type::give_military_access, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

}
bool can_give_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target, bool ignore_cost) {
	if(asker == target)
		return false;

	if(!ignore_cost && state.world.nation_get_is_player_controlled(asker) && state.world.nation_get_diplomatic_points(asker) < state.defines.givemilaccess_diplomatic_cost)
		return false;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(asker, target);
	if(state.world.unilateral_relationship_get_military_access(rel))
		return false;

	if(military::are_at_war(state, asker, target))
		return false;

	return true;
}
void execute_give_military_access(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	auto& current_diplo = state.world.nation_get_diplomatic_points(asker);
	state.world.nation_set_diplomatic_points(asker, current_diplo - state.defines.givemilaccess_diplomatic_cost);

	auto urel = state.world.get_unilateral_relationship_by_unilateral_pair(asker, target);
	if(!urel) {
		urel = state.world.force_create_unilateral_relationship(asker, target);
	}
	state.world.unilateral_relationship_set_military_access(urel, true);
	nations::adjust_relationship(state, asker, target, state.defines.givemilaccess_relation_on_accept);
}

void ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {


	command_data p{ command_type::ask_for_alliance, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target, bool ignore_cost) {
	/*
	Must not have an alliance. Must not be in a war against each other. Costs defines:ALLIANCE_DIPLOMATIC_COST diplomatic points.
	Great powers may not form an alliance while there is an active crisis. Vassals and substates may only form an alliance with
	their overlords.
	*/
	if(asker == target)
		return false;

	if(!ignore_cost && state.world.nation_get_is_player_controlled(asker) && state.world.nation_get_diplomatic_points(asker) < state.defines.alliance_diplomatic_cost)
		return false;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(target, asker);
	if(state.world.diplomatic_relation_get_are_allied(rel))
		return false;

	if(state.world.nation_get_is_great_power(asker) && state.world.nation_get_is_great_power(target) &&
			state.current_crisis_state != sys::crisis_state::inactive) {
		return false;
	}

	auto ol = state.world.nation_get_overlord_as_subject(asker);
	if(state.world.overlord_get_ruler(ol) && state.defines.alice_allow_subjects_declare_wars == 0)
		return false;
	auto ol2 = state.world.nation_get_overlord_as_subject(target);
	if(state.world.overlord_get_ruler(ol2) && state.defines.alice_allow_subjects_declare_wars == 0)
		return false;

	if(military::are_at_war(state, asker, target))
		return false;
	return true;
}
void execute_ask_for_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	if(!can_ask_for_alliance(state, asker, target))
		return;

	auto& current_diplo = state.world.nation_get_diplomatic_points(asker);
	state.world.nation_set_diplomatic_points(asker, current_diplo - state.defines.alliance_diplomatic_cost);

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.type = diplomatic_message::type::alliance_request;

	diplomatic_message::post(state, m);
}

void toggle_interested_in_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {

	command_data p{ command_type::toggle_interested_in_alliance, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

}
bool can_toggle_interested_in_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	if(asker == target)
		return false;
	return true;
}
void execute_toggle_interested_in_alliance(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	if(!can_toggle_interested_in_alliance(state, asker, target))
		return;
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, asker);
	if(!rel)
		rel = state.world.force_create_unilateral_relationship(target, asker);
	state.world.unilateral_relationship_set_interested_in_alliance(rel, !state.world.unilateral_relationship_get_interested_in_alliance(rel));
}

void ask_for_free_trade_agreement(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {

	command_data p{ command_type::ask_for_free_trade_agreement, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_ask_for_free_trade_agreement(sys::state& state, dcon::nation_id asker, dcon::nation_id target, bool ignore_cost) {
	/*
	Must have defines:ASKMILACCESS_DIPLOMATIC_COST diplomatic points. Must not be at war against each other.
	*/
	if(asker == target)
		return false;

	if(state.world.nation_get_is_player_controlled(asker) && !ignore_cost && state.world.nation_get_diplomatic_points(asker) < state.defines.askmilaccess_diplomatic_cost)
		return false;

	auto asker_overlord = state.world.nation_get_overlord_as_subject(asker);
	auto target_overlord = state.world.nation_get_overlord_as_subject(target);

	// Cannot negotitate trade agreement if the asker is a subject, or the target is a subject of someone other than the asker
	if(state.world.overlord_get_ruler(asker_overlord)) {
		return false;
	}
	if(state.world.overlord_get_ruler(target_overlord) && state.world.overlord_get_ruler(target_overlord) != asker) {
		return false;
	}

	auto asker_sphere = state.world.nation_get_in_sphere_of(asker);
	auto target_sphere = state.world.nation_get_in_sphere_of(target);
	// Cannot negotiate trade agreement if the asker is in a sphere, or the target is in a sphere that isn't the asker's sphere
	if(bool(asker_sphere)) {
		return false;
	}
	if(bool(target_sphere) && target_sphere != asker) {
		return false;
	}

	// Can't free trade if embargo is imposed
	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, asker);
	auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(asker, target);
	if(source_tariffs_rel) {
		if(state.world.unilateral_relationship_get_embargo(source_tariffs_rel)) {
			return false;
		}
	}
	if(target_tariffs_rel) {
		if(state.world.unilateral_relationship_get_embargo(target_tariffs_rel)) {
			return false;
		}
	}

	if(military::are_at_war(state, asker, target))
		return false;

	return true;
}
void execute_ask_for_free_trade_agreement(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	auto& current_diplo = state.world.nation_get_diplomatic_points(asker);
	state.world.nation_set_diplomatic_points(asker, current_diplo - state.defines.askmilaccess_diplomatic_cost);

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.type = diplomatic_message::type::free_trade_agreement;

	diplomatic_message::post(state, m);
}

void switch_embargo_status(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {

	command_data p{ command_type::switch_embargo_status, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_switch_embargo_status(sys::state& state, dcon::nation_id asker, dcon::nation_id target, bool ignore_cost) {
	/*
	Must have defines:ASKMILACCESS_DIPLOMATIC_COST diplomatic points. Must not be at war against each other.
	Even if nations have already free trade agreement - they can prolongate it for further years.
	*/
	if(asker == target)
		return false;

	if(state.world.nation_get_is_player_controlled(asker) && !ignore_cost && state.world.nation_get_diplomatic_points(asker) < state.defines.askmilaccess_diplomatic_cost)
		return false;

	auto ol = state.world.nation_get_overlord_as_subject(asker);
	auto ol2 = state.world.nation_get_overlord_as_subject(target);

	if(state.world.overlord_get_ruler(ol) || state.world.overlord_get_ruler(ol2)) {
		return false; // Subjects can't embargo or be embargoed
	}

	if(!economy::has_active_embargo(state, asker, target)) {
		auto asker_sphere = state.world.nation_get_in_sphere_of(asker);
		auto target_sphere = state.world.nation_get_in_sphere_of(target);

		if(target_sphere && target_sphere == asker) {
			return false; // cannot embargo your own sphereling
		}

		if(asker_sphere) {
			return false; // Spherelings can't embargo, but they can cancel direct embargo's (lingering from before they got sphered for example)
		}
	}



	// Can't embargo if free trade is in place
	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, asker);
	auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(asker, target);
	if(source_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
		if(state.current_date < enddt) {
			return false;
		}
	}
	if(target_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
		if(state.current_date < enddt) {
			return false;
		}
	}

	if(military::are_at_war(state, asker, target))
		return false;

	return true;
}

void execute_switch_embargo_status(sys::state& state, dcon::nation_id from, dcon::nation_id to) {
	if (state.world.nation_get_is_player_controlled(from)) {
		auto& current_diplo = state.world.nation_get_diplomatic_points(from);
		state.world.nation_set_diplomatic_points(from, current_diplo - state.defines.askmilaccess_diplomatic_cost);
	}

	auto rel_1 = state.world.get_unilateral_relationship_by_unilateral_pair(to, from);
	if(!rel_1) {
		rel_1 = state.world.force_create_unilateral_relationship(to, from);
	}
	if(state.world.unilateral_relationship_get_embargo(rel_1)) {
		nations::remove_embargo(state, rel_1);
	}
	else {
		nations::do_embargo(state, rel_1);
	}
}

void revoke_trade_rights(sys::state& state, dcon::nation_id source, dcon::nation_id target) {


	command_data p{ command_type::revoke_trade_rights, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_revoke_trade_rights(sys::state& state, dcon::nation_id source, dcon::nation_id target, bool ignore_cost) {
	/*
	Must have defines:ASKMILACCESS_DIPLOMATIC_COST diplomatic points. Must not be at war against each other.
	*/
	if(source == target)
		return false;

	if(state.world.nation_get_is_player_controlled(source) && !ignore_cost && state.world.nation_get_diplomatic_points(source) < state.defines.askmilaccess_diplomatic_cost)
		return false;

	auto asker_overlord = state.world.nation_get_overlord_as_subject(source);
	auto target_overlord = state.world.nation_get_overlord_as_subject(target);

	// Cannot cancel trade agreement if the asker is a subject, or the target is a subject of someone other than the asker
	if(state.world.overlord_get_ruler(asker_overlord)) {
		return false;
	}
	if(state.world.overlord_get_ruler(target_overlord) && state.world.overlord_get_ruler(target_overlord) != source) {
		return false;
	}

	auto rights = economy::nation_gives_free_trade_rights(state, source, target);

	auto our_rights = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);

	if(!rights) {
		return false; // Nation doesn't give trade rights
	}
	if(!our_rights || rights != our_rights) {
		return false; // We do not own the rights to this agreement, someone else does (spherelord/overlord)
	}
	auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(rights);

	if(state.current_date < enddt) {
		return false; // Cannot revoke yet
	}

	return true;
}
void execute_revoke_trade_rights(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.askmilaccess_diplomatic_cost);
	nations::revoke_free_trade_agreement_one_way(state, target, source);
}

void state_transfer(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::state_definition_id sid) {

	command_data p{ command_type::state_transfer, state.local_player_id };
	auto data = state_transfer_data{ target, sid };
	p << data;
	add_to_command_queue(state, p);
}
bool can_state_transfer(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::state_definition_id sid) {
	/* (No state specified) To state transfer: Can't be same asker into target, both must be players. If any are great powers,
	they can't state transfer when a crisis occurs. They can't be subjects. They can't be in a state of war */
	if(asker == target)
		return false;
	//if(!state.world.nation_get_is_player_controlled(asker) || !state.world.nation_get_is_player_controlled(target))
	//	return false;
	if(state.current_crisis_state != sys::crisis_state::inactive)
		return false;
	auto ol = state.world.nation_get_overlord_as_subject(asker);
	if(state.world.overlord_get_ruler(ol))
		return false;
	//auto ol2 = state.world.nation_get_overlord_as_subject(target);
	//if(state.world.overlord_get_ruler(ol2))
	//	return false;

	// Asker and target must be in a subject relation
	if(state.defines.alice_state_transfer_limits > 0.0f) {
		auto ol2 = state.world.nation_get_overlord_as_subject(target);
		if(state.world.overlord_get_ruler(ol) != target && state.world.overlord_get_ruler(ol2) != asker)
			return false;
	}
	if(state.world.nation_get_is_at_war(asker) || state.world.nation_get_is_at_war(target))
		return false;
	//Redundant, if we're at war already, we will return false:
	//if(military::are_at_war(state, asker, target))
	//	return false;
	// "Asker gives to target"
	for(const auto ab : state.world.state_definition_get_abstract_state_membership(sid)) {
		if(ab.get_province().get_province_ownership().get_nation() == asker) {
			// Must be controlled by us (if not, it means there are rebels, and we don't allow
			// state transfers on states with rebels)
			if(ab.get_province().get_province_control().get_nation() != asker)
				return false;
		}
	}
	if(state.world.nation_get_owned_state_count(asker) == 1)
		return false;
	return true;
}
void execute_state_transfer(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::state_definition_id sid) {
	if(!can_state_transfer(state, asker, target, sid))
		return;

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.type = diplomatic_message::type::state_transfer;
	m.data.state = sid;
	diplomatic_message::post(state, m);
}

bool can_command_units(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	// disable in SP for now. Maybe could be a game rule later?
	if(state.network_mode == sys::network_mode_type::single_player) {
		return false;
	}
	if(asker == target)
		return false;

	// can't control players units for now. Could consider expanding it to be refusable for players prehaps
	if(state.world.nation_get_is_player_controlled(target)) {
		return false;
	}
	auto asker_wars = state.world.nation_get_war_participant(asker);
	auto target_wars = state.world.nation_get_war_participant(target);

	// both overlord and subject must be at war to command units
	if(asker_wars.begin() == asker_wars.end() || target_wars.begin() == target_wars.end()) {
		return false;
	}

	if(!nations::is_nation_subject_of(state, target, asker)) {
		return false;
	}
	if(nations::is_commanding_subject_units(state, target, asker)) {
		return false;
	}
	return true;
}


void command_units(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {

	command_data p{ command_type::command_units, state.local_player_id };
	auto data = command_units_data{ target };
	p << data;
	add_to_command_queue(state, p);
}


void execute_command_units(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	state.world.nation_set_overlord_commanding_units(target, true);
	ai::remove_ai_data(state, target);
}



bool can_give_back_units(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	if(asker == target)
		return false;

	// can't control players units for now. Could consider expanding it to be refusable for players prehaps
	if(state.world.nation_get_is_player_controlled(target)) {
		return false;
	}

	if(!nations::is_nation_subject_of(state, target, asker)) {
		return false;
	}
	if(!nations::is_commanding_subject_units(state, target, asker)) {
		return false;
	}
	return true;
}


void give_back_units(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {

	command_data p{ command_type::give_back_units, state.local_player_id };
	auto data = command_units_data{ target };
	p << data;
	add_to_command_queue(state, p);
}

void execute_give_back_units(sys::state& state, dcon::nation_id asker, dcon::nation_id target) {
	military::give_back_units(state, target);
}


void call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w, bool automatic_call) {

	command_data p{ command_type::call_to_arms, state.local_player_id };
	auto data = call_to_arms_data{ target, w, automatic_call };
	p << data;
	add_to_command_queue(state, p);
}
bool can_call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w, bool ignore_cost, bool automatic_call) {
	if(asker == target)
		return false;
	// asker must be in the war that the target is being called into
	bool asker_is_in_war = [&]() {
		for(const auto wp : state.world.nation_get_war_participant(asker)) {
			if(wp.get_war() == w) {
				return true;
			}
		}
		return false;

	}();
	if(!asker_is_in_war) {
		return false;
	}

	if(!ignore_cost && state.world.nation_get_is_player_controlled(asker) && state.world.nation_get_diplomatic_points(asker) < state.defines.callally_diplomatic_cost)
		return false;

	if((!nations::are_allied(state, asker, target) && !nations::is_nation_subject_of(state, target, asker)) && !(state.world.war_get_primary_defender(w) == asker && state.world.nation_get_in_sphere_of(asker) == target))
		return false;

	if(military::is_civil_war(state, w))
		return false;

	if(!military::standard_war_joining_is_possible(state, w, target, military::is_attacker(state, w, asker)))
		return false;

	if(state.world.war_get_is_crisis_war(w) && !state.military_definitions.great_wars_enabled)
		return false;

	bool asker_is_attacker = military::is_attacker(state, w, asker);

	// cannot join a war aganst spherelord.
	for(auto participant : military::get_one_side_war_participants(state, w, !asker_is_attacker)) {
		if(nations::would_war_conflict_with_sphere_leader<nations::war_initiation::join_war>(state, target, participant)) {
			return false;
		}
	}

	// an automatic defensive call bypasses any truces there may be with the other side. Truce is checked against the original attacker or defender

	if(!automatic_call || (automatic_call && asker_is_attacker)) {
		auto truce_target = military::is_attacker(state, w, asker) ? state.world.war_get_original_target(w) : state.world.war_get_original_attacker(w);
		if(nations::nation_is_in_war(state, truce_target, w)) {
			if(military::has_truce_with(state, target, truce_target)) {
				return false;
			}
		}
	}


	return true;
}
void execute_call_to_arms(sys::state& state, dcon::nation_id asker, dcon::nation_id target, dcon::war_id w, bool automatic_call) {
	auto& current_diplo = state.world.nation_get_diplomatic_points(asker);
	state.world.nation_set_diplomatic_points(asker, current_diplo - state.defines.callally_diplomatic_cost);

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = asker;
	m.data.war = w;
	m.type = diplomatic_message::type::call_ally_request;
	m.automatic_call = automatic_call;

	diplomatic_message::post(state, m);
}

void respond_to_diplomatic_message(sys::state& state, dcon::nation_id source, dcon::nation_id from, diplomatic_message::type type,
		bool accept) {

	command_data p{ command_type::respond_to_diplomatic_message, state.local_player_id };
	auto data = message_data{ from, type, accept };
	p << data;
	add_to_command_queue(state, p);

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

	command_data p{ command_type::cancel_military_access, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_cancel_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target, bool ignore_cost) {
	if(source == target)
		return false;

	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(!ignore_cost && state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < state.defines.cancelaskmilaccess_diplomatic_cost)
		return false;

	if(state.world.unilateral_relationship_get_military_access(rel))
		return true;
	else
		return false;
}
void execute_cancel_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(target, source);
	if(rel)
		state.world.unilateral_relationship_set_military_access(rel, false);

	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.cancelaskmilaccess_diplomatic_cost);
	nations::adjust_relationship(state, source, target, state.defines.cancelaskmilaccess_relation_on_accept);

	notification::post(state, notification::message{
		[source, target](sys::state& state, text::layout_base& contents) {
			text::add_line(state, contents, "msg_access_canceled_a_1", text::variable_type::x, source, text::variable_type::y, target);
		},
		"msg_access_canceled_a_title",
		source, target, dcon::nation_id{},
		sys::message_base_type::mil_access_end, dcon::province_id{ }
	});
}

void cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {

	command_data p{ command_type::cancel_given_military_access, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target, bool ignore_cost) {
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(source, target);

	if(!ignore_cost && state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < state.defines.cancelgivemilaccess_diplomatic_cost)
		return false;

	if(state.world.unilateral_relationship_get_military_access(rel)) {
		// subjects cannot cancel military access from their overlords
		if(nations::is_nation_subject_of(state, source, target)) {
			return false;
		}
		else {
			return true;
		}

	}

	else {
		return false;
	}

}
void execute_cancel_given_military_access(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(source, target);
	if(rel)
		state.world.unilateral_relationship_set_military_access(rel, false);

	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.cancelgivemilaccess_diplomatic_cost);
	nations::adjust_relationship(state, source, target, state.defines.cancelgivemilaccess_relation_on_accept);

	if(source != state.local_player_nation) {
		notification::post(state, notification::message{
			[source, target](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_access_canceled_b_1", text::variable_type::x, source, text::variable_type::y, target);
			},
			"msg_access_canceled_b_title",
			source, target, dcon::nation_id{},
			sys::message_base_type::mil_access_end, dcon::province_id{ }
		});
	}
}

void cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target) {

	command_data p{ command_type::cancel_alliance, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target, bool ignore_cost) {
	if(source == target)
		return false;

	if(!ignore_cost && state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < state.defines.cancelalliance_diplomatic_cost)
		return false;

	auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(target, source);
	if(!state.world.diplomatic_relation_get_are_allied(rel))
		return false;

	//if(military::are_allied_in_war(state, source, target))
	//	return false;

	auto ol = state.world.nation_get_overlord_as_subject(source);
	if(state.world.overlord_get_ruler(ol) == target)
		return false;

	return true;
}
void execute_cancel_alliance(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.cancelalliance_diplomatic_cost);
	nations::adjust_relationship(state, source, target, state.defines.cancelalliance_relation_on_accept);

	nations::break_alliance(state, source, target);
}

void declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation, bool call_attacker_allies, bool run_conference) {

	command_data p{ command_type::declare_war, state.local_player_id };
	auto data = new_war_data{ target, cb_state, cb_tag, cb_secondary_nation, primary_cb, call_attacker_allies, run_conference  };
	p << data;
	add_to_command_queue(state, p);
}

bool can_declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {

	if(!military::can_attack(state, source, target)) {
		return false;
	}
	// check CB validity
	if(!military::cb_instance_conditions_satisfied(state, source, target, primary_cb, cb_state, cb_tag, cb_secondary_nation))
		return false;

	return true;
}

void execute_declare_war(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::cb_type_id primary_cb,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation, bool call_attacker_allies, bool run_conference) {
	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.declarewar_diplomatic_cost);
	nations::adjust_relationship(state, source, target, state.defines.declarewar_relation_on_accept);

	dcon::nation_id real_target = target;

	auto target_ol_rel = state.world.nation_get_overlord_as_subject(target);
	if(state.world.overlord_get_ruler(target_ol_rel) && state.world.overlord_get_ruler(target_ol_rel) != source)
		real_target = state.world.overlord_get_ruler(target_ol_rel);

	// Infamy, militancy and prestige loss for truce break
	if(military::has_truce_with(state, source, real_target)) {
		auto cb_infamy = military::truce_break_cb_infamy(state, primary_cb, target);
		auto cb_militancy = military::truce_break_cb_militancy(state, primary_cb);
		auto cb_prestige_loss = military::truce_break_cb_prestige_cost(state, primary_cb);

		auto& current_infamy = state.world.nation_get_infamy(source);
		state.world.nation_set_infamy(source, current_infamy + cb_infamy);
		nations::adjust_prestige(state, source, cb_prestige_loss);

		for(auto prov : state.world.nation_get_province_ownership(source)) {
			for(auto pop : prov.get_province().get_pop_location()) {
				auto mil = pop_demographics::get_militancy(state, pop.get_pop());
				pop_demographics::set_militancy(state, pop.get_pop().id, std::min(mil + cb_militancy, 10.0f));
			}
		}
	}
	// Infamy for war declaration when applicable
	else {
		auto cb_infamy = military::war_declaration_infamy_cost(state, primary_cb, source, target, cb_state);
		auto& current_infamy = state.world.nation_get_infamy(source);
		state.world.nation_set_infamy(source, current_infamy + cb_infamy);
	}

	// remove used cb
	auto current_cbs = state.world.nation_get_available_cbs(source);
	for(uint32_t i = current_cbs.size(); i-- > 0;) {
		if(current_cbs[i].cb_type == primary_cb && current_cbs[i].target == target) {
			current_cbs.remove_at(i);
			break;
		}
	}

	if(run_conference && state.current_crisis_state == sys::crisis_state::inactive) {
		nations::cleanup_crisis(state);

		nations::crisis_add_wargoal(state.crisis_attacker_wargoals, sys::full_wg{
					source,
						target,
						cb_secondary_nation,
						cb_tag,
						cb_state,
						primary_cb
		});

		state.crisis_defender = target;
		state.crisis_attacker = source;
		if(state.world.nation_get_is_great_power(target)) {
			state.primary_crisis_defender = target;
		}
		if(state.world.nation_get_is_great_power(source)) {
			state.primary_crisis_attacker = source;
		}

		notification::post(state, notification::message{
			[st = cb_state](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_new_crisis_1", text::variable_type::x, st);
			},
			"msg_new_crisis_title",
			state.local_player_nation, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::crisis_starts
		});

		state.last_crisis_end_date = state.current_date;
		nations::crisis_state_transition(state, sys::crisis_state::finding_attacker);
	}
	else {
		auto war = military::create_war(state, source, target, primary_cb, cb_state, cb_tag, cb_secondary_nation);
		military::call_defender_allies(state, war);
		if(call_attacker_allies) {
			military::call_attacker_allies(state, war);
		}
	}
}

void add_war_goal(sys::state& state, dcon::nation_id source, dcon::war_id w, dcon::nation_id target, dcon::cb_type_id cb_type,
		dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag, dcon::nation_id cb_secondary_nation) {



	command_data p{ command_type::add_war_goal, state.local_player_id };
	auto data = new_war_goal_data{ target, cb_state, cb_tag,cb_secondary_nation, w, cb_type  };
	p << data;
	add_to_command_queue(state, p);

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

	if(state.world.nation_get_is_player_controlled(source) && state.cheat_data.always_allow_wargoals)
		return true;

	if(state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < state.defines.addwargoal_diplomatic_cost)
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

	if(!is_attacker && military::defenders_have_status_quo_wargoal(state, w))
		return false;

	if(!target_in_war)
		return false;

	// prevent duplicate war goals
	if(military::war_goal_would_be_duplicate(state, source, w, target, cb_type, cb_state, cb_tag, cb_secondary_nation))
		return false;

	if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::always) == 0) {
		bool cb_fabbed = false;
		for(auto& fab_cb : state.world.nation_get_available_cbs(source)) {
			if(fab_cb.cb_type == cb_type && fab_cb.target == target) {
				cb_fabbed = true;
				break;
			}
		}
		if(!cb_fabbed && state.defines.alice_can_add_constructable_cbs_as_wargoals != 0.f) {
			if((state.world.cb_type_get_type_bits(cb_type) & military::cb_flag::is_not_constructing_cb) != 0)
				return false; // can only add a constructable cb this way

			auto totalpop = state.world.nation_get_demographics(source, demographics::total);
			auto jingoism_perc = totalpop > 0 ? state.world.nation_get_demographics(source, demographics::to_key(state, state.culture_definitions.jingoism)) / totalpop : 0.0f;

			if(state.world.war_get_is_great(w)) {
				if(jingoism_perc < state.defines.wargoal_jingoism_requirement * state.defines.gw_wargoal_jingoism_requirement_mod)
					return false;
			} else {
				if(jingoism_perc < state.defines.wargoal_jingoism_requirement)
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

	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - state.defines.addwargoal_diplomatic_cost);
	nations::adjust_relationship(state, source, target, state.defines.addwargoal_relation_on_accept);

	float infamy = military::cb_addition_infamy_cost(state, w, cb_type, source, target, cb_state);
	auto& current_infamy = state.world.nation_get_infamy(source);
	state.world.nation_set_infamy(source, current_infamy + infamy);

	military::add_wargoal(state, w, source, target, cb_type, cb_state, cb_tag, cb_secondary_nation);
}

void start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war, bool is_concession) {

	command_data p{ command_type::start_peace_offer, state.local_player_id };
	auto data = new_offer_data{ target, war, is_concession  };
	p << data;
	add_to_command_queue(state, p);
}
bool can_start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war, bool is_concession) {
	assert(source);
	assert(target);
	{
		auto ol = state.world.nation_get_overlord_as_subject(source);
		if(state.world.overlord_get_ruler(ol) && !(state.world.war_get_primary_attacker(war) == source || state.world.war_get_primary_defender(war) == source))
			return false;
	}
	{
		auto ol = state.world.nation_get_overlord_as_subject(target);
		if(state.world.overlord_get_ruler(ol) && !(state.world.war_get_primary_attacker(war) == target || state.world.war_get_primary_defender(war) == target))
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
	} else {
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
void execute_start_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::war_id war, bool is_concession) {
	auto offer = fatten(state.world, state.world.create_peace_offer());
	offer.set_target(target);
	offer.set_war_from_war_settlement(war);
	offer.set_is_concession(is_concession);
	offer.set_nation_from_pending_peace_offer(source);
}

void start_crisis_peace_offer(sys::state& state, dcon::nation_id source, bool is_concession) {


	command_data p{ command_type::start_crisis_peace_offer, state.local_player_id };
	auto data = new_offer_data{};
	data.is_concession = is_concession;
	p << data;
	add_to_command_queue(state, p);
}
bool can_start_crisis_peace_offer(sys::state& state, dcon::nation_id source, bool is_concession) {
	if(source != state.primary_crisis_attacker && source != state.primary_crisis_defender)
		return false;
	if(state.current_crisis_state != sys::crisis_state::heating_up)
		return false;

	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	return !pending;
}
void execute_start_crisis_peace_offer(sys::state& state, dcon::nation_id source, bool is_concession) {
	auto offer = fatten(state.world, state.world.create_peace_offer());
	offer.set_is_concession(is_concession);
	offer.set_nation_from_pending_peace_offer(source);
	offer.set_is_crisis_offer(true);
}

void add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal) {

	command_data p{ command_type::add_peace_offer_term, state.local_player_id };
	auto data = offer_wargoal_data{ goal };
	p << data;
	add_to_command_queue(state, p);

}
bool can_add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal) {
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	if(!pending)
		return false;

	auto war = state.world.peace_offer_get_war_from_war_settlement(pending);
	auto wg = fatten(state.world, goal);
	auto target = state.world.peace_offer_get_target(pending);

	if(!war)
		return false;

	if(wg.get_war_from_wargoals_attached() != war)
		return false;

	//int32_t total = military::cost_of_peace_offer(state, pending);
	//int32_t new_wg_cost = military::peace_cost(state, war, wg.get_type(), wg.get_added_by(), wg.get_target_nation(), wg.get_secondary_nation(), wg.get_associated_state(), wg.get_associated_tag());

	//if(total + new_wg_cost > 100)
	//	return false;

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
			if(wg.get_added_by().get_overlord_as_subject().get_ruler() == target)
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
			if(wg.get_added_by().get_overlord_as_subject().get_ruler() == target)
				return true;
			return false;
		}
	}
}
void execute_add_to_peace_offer(sys::state& state, dcon::nation_id source, dcon::wargoal_id goal) {
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	state.world.force_create_peace_offer_item(pending, goal);
}

void add_to_crisis_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id wargoal_from, dcon::nation_id target,
		dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag,
		dcon::nation_id cb_secondary_nation) {


	command_data p{ command_type::add_wargoal_to_crisis_offer, state.local_player_id };
	auto data = crisis_invitation_data{ };
	data.invited = wargoal_from;
	data.target = target;
	data.cb_type = primary_cb;
	data.cb_state = cb_state;
	data.cb_tag = cb_tag;
	data.cb_secondary_nation = cb_secondary_nation;

	p << data;
	add_to_command_queue(state, p);
}
bool can_add_to_crisis_peace_offer(sys::state& state, dcon::nation_id source, dcon::nation_id wargoal_from,
		dcon::nation_id target, dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag,
		dcon::nation_id cb_secondary_nation) {

 	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	if(!pending)
		return false;

	auto war = state.world.peace_offer_get_war_from_war_settlement(pending);
	if(war)
		return false;

	bool found = [&]() {
		for(auto wg : state.crisis_attacker_wargoals) {
			if(wg.added_by == wargoal_from && cb_state == wg.state && cb_tag == wg.wg_tag &&
						cb_secondary_nation == wg.secondary_nation && target == wg.target_nation &&
						primary_cb == wg.cb)
				return true;
		}
		for(auto wg : state.crisis_defender_wargoals) {
			if(wg.added_by == wargoal_from && cb_state == wg.state && cb_tag == wg.wg_tag &&
						cb_secondary_nation == wg.secondary_nation && target == wg.target_nation &&
						primary_cb == wg.cb)
				return true;
		}
		return false;
	}();

	if(!found)
		return false;

	// no duplicates
	for(auto item : state.world.peace_offer_get_peace_offer_item(pending)) {
		auto wg = item.get_wargoal();
		if(wg.get_added_by() == wargoal_from && cb_state == wg.get_associated_state() && cb_tag == wg.get_associated_tag() &&
						cb_secondary_nation == wg.get_secondary_nation() && target == wg.get_target_nation() &&
						primary_cb == wg.get_type())
			return false;
	}

	return true;
}
void execute_add_to_crisis_peace_offer(sys::state& state, dcon::nation_id source, crisis_invitation_data const& data) {
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);

	auto wg = fatten(state.world, state.world.create_wargoal());
	wg.set_peace_offer_from_peace_offer_item(pending);
	wg.set_added_by(data.invited);
	wg.set_associated_state(data.cb_state);
	wg.set_associated_tag(data.cb_tag);
	wg.set_secondary_nation(data.cb_secondary_nation);
	wg.set_target_nation(data.target);
	wg.set_type(data.cb_type);
}

void send_peace_offer(sys::state& state, dcon::nation_id source) {
	command_data p{ command_type::send_peace_offer, state.local_player_id };
	add_to_command_queue(state, p);

}
bool can_send_peace_offer(sys::state& state, dcon::nation_id source) {
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	if(!pending || !state.world.peace_offer_get_war_from_war_settlement(pending))
		return false;
	return true;
}
void execute_send_peace_offer(sys::state& state, dcon::nation_id source) {
	auto pending_offer = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	auto in_war = state.world.peace_offer_get_war_from_war_settlement(pending_offer);

	if(!in_war) {
		return;
	}

	auto target = state.world.peace_offer_get_target(pending_offer);
	// If the war was force peaced, dont sent the diplo message
	if(military::try_force_peace(state, source, target, in_war, pending_offer)) {
		return;
	}
	else {
		diplomatic_message::message m;
		memset(&m, 0, sizeof(diplomatic_message::message));
		m.to = target;
		m.from = source;
		m.data.peace = pending_offer;
		m.type = diplomatic_message::type::peace_offer;

		diplomatic_message::post(state, m);
	}
}

void send_crisis_peace_offer(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::send_crisis_peace_offer, state.local_player_id };
	add_to_command_queue(state, p);

}
bool can_send_crisis_peace_offer(sys::state& state, dcon::nation_id source) {
	auto pending = state.world.nation_get_peace_offer_from_pending_peace_offer(source);
	if(!pending || state.world.peace_offer_get_war_from_war_settlement(pending))
		return false;
	if(source != state.primary_crisis_attacker && source != state.primary_crisis_defender)
		return false;
	if(state.current_crisis_state != sys::crisis_state::heating_up)
		return false;

	return true;
}
void execute_send_crisis_peace_offer(sys::state& state, dcon::nation_id source) {
	auto pending_offer = state.world.nation_get_peace_offer_from_pending_peace_offer(source);

	if(!can_send_crisis_peace_offer(state, source)) {
		if(pending_offer) {
			if(!state.world.peace_offer_get_war_from_war_settlement(pending_offer))
				nations::cleanup_crisis_peace_offer(state, pending_offer);
			else
				state.world.delete_peace_offer(pending_offer);
		}
		return;
	}

	auto target = state.primary_crisis_attacker == source ? state.primary_crisis_defender : state.primary_crisis_attacker;

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = target;
	m.from = source;
	m.data.peace = pending_offer;
	m.type = diplomatic_message::type::crisis_peace_offer;

	diplomatic_message::post(state, m);
}

void stop_army_movement(sys::state& state, dcon::nation_id source, dcon::army_id army) {

	command_data p{ command_type::stop_army_movement, state.local_player_id };
	auto data = stop_army_movement_data{ army };
	p << data;
	add_to_command_queue(state, p);

}

bool can_stop_army_movement(sys::state& state, dcon::nation_id source, dcon::army_id army) {
	if(source != military::get_effective_unit_commander(state, army))
		return false;
	if(state.world.army_get_is_retreating(army))
		return false;

	return true;
}

void execute_stop_army_movement(sys::state& state, dcon::nation_id source, dcon::army_id army) {
	military::stop_army_movement(state, army);
}



void stop_navy_movement(sys::state& state, dcon::nation_id source, dcon::navy_id navy) {

	command_data p{ command_type::stop_navy_movement, state.local_player_id };
	auto data = stop_navy_movement_data{ navy };
	p << data;
	add_to_command_queue(state, p);
}

bool can_stop_navy_movement(sys::state& state, dcon::nation_id source, dcon::navy_id navy) {
	if(source != military::get_effective_unit_commander(state, navy))
		return false;
	if(state.world.navy_get_is_retreating(navy))
		return false;

	return true;
}

void execute_stop_navy_movement(sys::state& state, dcon::nation_id source, dcon::navy_id navy) {
	military::stop_navy_movement(state, navy);
}

void move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest, bool reset, military::special_army_order order) {

	command_data p{ command_type::move_army, state.local_player_id };
	auto data = army_movement_data{ a, dest, reset, order };
	p << data;
	add_to_command_queue(state, p);

}


bool can_move_or_stop_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest) {
	auto army_loc = state.world.army_get_location_from_army_location(a);
	if(command::can_move_army(state, source, a, dest, true).empty() && !(army_loc == dest && command::can_stop_army_movement(state, source, a))) {
		return false;
	}
	else {
		return true;
	}
}

bool can_move_retreat_or_stop_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest) {
	auto navy_loc = state.world.navy_get_location_from_navy_location(n);
	if(navy_loc == dest && command::can_stop_navy_movement(state, source, n)) {
		return true;
	}
	auto battle = state.world.navy_get_battle_from_navy_battle_participation(n);
	if(bool(battle)) {
		return !command::can_retreat_from_naval_battle(state, source, n, military::retreat_type::manual, dest).empty();
	}
	else {
		return !command::can_move_navy(state, source, n, dest, true).empty();
	}

}

void move_or_stop_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest, military::special_army_order order) {
	auto army_loc = state.world.army_get_location_from_army_location(a);
	if(army_loc == dest) {
		command::stop_army_movement(state, source, a);
	}
	else {
		command::move_army(state, source, a, dest, true, order);
	}
}

void move_retreat_or_stop_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest) {
	auto army_loc = state.world.navy_get_location_from_navy_location(n);
	auto battle = state.world.navy_get_battle_from_navy_battle_participation(n);
	if(army_loc == dest) {
		command::stop_navy_movement(state, source, n);
	} else {
		if(bool(battle)) {
			command::retreat_from_naval_battle(state, source, n, dest);
		}
		else {
			command::move_navy(state, source, n, dest, true);
		}

	}
}


bool can_partial_retreat_from(sys::state& state, dcon::land_battle_id b) {
	if(!b)
		return true;
	if(!military::can_retreat_from_battle(state, b))
		return false;
	return gamerule::check_gamerule(state, state.hardcoded_gamerules.allow_partial_retreat, uint8_t(gamerule::partial_retreat_settings::enable));
}
bool can_partial_retreat_from(sys::state& state, dcon::naval_battle_id b) {
	if(!b)
		return true;
	if(!military::is_battle_retreatable(state, b, military::retreat_type::manual))
		return false;
	return gamerule::check_gamerule(state, state.hardcoded_gamerules.allow_partial_retreat, uint8_t(gamerule::partial_retreat_settings::enable));
}

std::vector<dcon::province_id> can_move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest, bool reset) {
	if(source != military::get_effective_unit_commander(state, a))
		return std::vector<dcon::province_id>{};
	if(state.world.army_get_is_retreating(a))
		return std::vector<dcon::province_id>{};
	if(!dest)
		return std::vector<dcon::province_id>{}; // stop movement

	// Behavior for shift+click movement. Otherwise - path is cleared beforehand
	// the "reset" param dictaties whether or not it checks if you can move from the armies *current position* (reset is true), or from the province the army is currently queued to walk to (reset is false)
	auto last_province = state.world.army_get_location_from_army_location(a);
	if(!reset) {
		auto movement = state.world.army_get_path(a);
		if(movement.size() > 0) {
			last_province = movement.at(0);
		}
	}
	// pass army owner directly instead of source nation for path calculation, as the army owner may be diffrent than source if commanding a subject's units
	auto army_owner = state.world.army_get_controller_from_army_control(a);

	return calculate_army_path(state, army_owner, a, last_province, dest);
}


std::vector<dcon::province_id> calculate_army_path(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id last_province, dcon::province_id dest) {
	if(last_province == dest) {
		return std::vector<dcon::province_id>{};
	}

	if(!can_partial_retreat_from(state, state.world.army_get_battle_from_army_battle_participation(a)))
		return std::vector<dcon::province_id>{};

	if(dest.index() < state.province_definitions.first_sea_province.index()) {
		if(state.network_mode != sys::network_mode_type::single_player) {
			if(state.world.army_get_battle_from_army_battle_participation(a)) {
				// MP special ruleset (basically applies to most MP games)
				// Being able to withdraw:
				// extern - YOUR own territory
				// extern - Allied territory (this is in a state of war)
				// extern - Territory that you have military access to (this is regulated in MP)
				// handled 1.0 - Enemy territory that does not have troops at the time of withdrawal (regulated in mp)
				// handled 1.1 - Allied/your territory that is being occupied in a war
				// extern - To ships (including when a naval battle is in progress)
				// Not being able to withdraw:
				// handled 1.2 - When all provinces adjacent to a battle are surrounded by enemies
				// handled 1.3 - No more than 1 province, to avoid multiprovince
				// handled 1.4 - Where there are enemy troops (your territory / ally or enemy, it doesn't matter)
				bool b_10 = false; // Also handles 1-1, occupied territory of allies or yours
				// 1.0/1.1 - Enemy territory
				if(military::are_at_war(state, state.world.province_get_nation_from_province_control(dest), source)) {
					auto units = state.world.province_get_army_location_as_location(dest);
					b_10 = true;  // Enemy territory with no enemy units -- can retreat
					for(const auto unit : units) {
						if(unit.get_army().get_controller_from_army_control() == source)
							continue;
						if(unit.get_army().get_controller_from_army_rebel_control()
						|| military::are_at_war(state, unit.get_army().get_controller_from_army_control(), source)) {
							b_10 = false;  // Enemy territory with enemy units -- CAN'T retreat
							break;
						}
					}
				}
				// 1.2 - Sorrounding/encirclement of land units
				bool b_12 = false;
				for(const auto adj : state.world.province_get_province_adjacency(dest)) {
					auto other = adj.get_connected_provinces(adj.get_connected_provinces(0) == dest ? 1 : 0);
					if(other.id.index() < state.province_definitions.first_sea_province.index()) {
						auto units = state.world.province_get_army_location_as_location(dest);
						bool has_enemy_units = false;
						for(const auto unit : units) {
							if(unit.get_army().get_controller_from_army_rebel_control()
							|| military::are_at_war(state, unit.get_army().get_controller_from_army_control(), source)) {
								has_enemy_units = true;
								break;
							}
						}
						if(!has_enemy_units) { //Not a full encirclement -- can retreat
							b_12 = true;
							break;
						}
					}
				}
				// 1.3 - Not more than 1 province
				bool b_13 = true; /*
				for(const auto adj : state.world.province_get_province_adjacency(dest)) {
					auto other = adj.get_connected_provinces(adj.get_connected_provinces(0) == dest ? 1 : 0);
					if(last_province == other) {
						b_13 = true; //Is adjacent to destination, hence a single province retreat!?
						break;
					}
				}*/

				if(state.world.army_get_black_flag(a)) {
					return province::make_unowned_land_path(state, last_province, dest);
				} else if(province::has_access_to_province(state, source, dest) && b_12 && b_13) {
					return province::make_land_path(state, last_province, dest, source, a);
				} else if(b_10) {
					return province::make_unowned_land_path(state, last_province, dest);
				} else {
					return std::vector<dcon::province_id>{};
				}
			} else {
				if(state.world.army_get_black_flag(a)) {
					return province::make_unowned_land_path(state, last_province, dest);
				} else if(province::has_access_to_province(state, source, dest)) {
					return province::make_land_path(state, last_province, dest, source, a);
				} else {
					return std::vector<dcon::province_id>{};
				}
			}
		} else {
			if(state.world.army_get_black_flag(a)) {
				return province::make_unowned_land_path(state, last_province, dest);
			} else if(province::has_access_to_province(state, source, dest)) {
				return province::make_land_path(state, last_province, dest, source, a);
			} else {
				return std::vector<dcon::province_id>{};
			}
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


void execute_move_army(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::province_id dest, bool reset, military::special_army_order special_order) {
	auto army_owner = state.world.army_get_controller_from_army_control(a);
	if(source != military::get_effective_unit_commander(state, a))
		return;
	if(state.world.army_get_is_retreating(a))
		return;

	auto battle = state.world.army_get_battle_from_army_battle_participation(a);
	if(dest.index() < state.province_definitions.first_sea_province.index()) {
		/* Case for land destinations */
		// the previous call was to "province::has_naval_access_to_province" instead, was there a reason for that?
		if(battle && !province::has_access_to_province(state, army_owner, dest)) {
			return;
		}
	} else {
		/* Case for naval destinations, we check the land province adjacent henceforth */
		if(battle && !military::can_embark_onto_sea_tile(state, army_owner, dest, a)) {
			return;
		}
	}

	// Invalid destination province: reset existing path
	if(!dest) {
		military::stop_army_movement(state, a);
		return;
	}

	state.world.army_set_special_order(a, (uint8_t)special_order);

	// Build new path
	auto path = can_move_army(state, source, a, dest, reset);

	if(military::move_army_fast(state, a, path, army_owner, reset)) {
		state.world.army_set_is_rebel_hunter(a, false);

		// US9AC1 Command army to pursue the target
		if(special_order == military::special_army_order::pursue_to_engage) {
			// Target the first army in the target province
			auto al = (*state.world.province_get_army_location(dest).begin());
			if(al) {
				state.world.force_create_army_pursuit(a, al.get_army());
			}
			else {
				state.world.force_create_army_pursuit(a, dcon::army_id{});
			}
		}
	} else if(reset) {
		military::stop_army_movement(state, a);
	}
	state.world.army_set_moving_to_merge(a, false);

	// Move away FROM battle
	if(battle) {
		state.world.army_set_is_retreating(a, true);
		state.world.army_set_battle_from_army_battle_participation(a, dcon::land_battle_id{});
		for(auto reg : state.world.army_get_army_membership(a)) {
			{
				auto& line = state.world.land_battle_get_attacker_front_line(battle);
				for(auto& lr : line) {
					if(lr == reg.get_regiment())
						lr = dcon::regiment_id{};
				}
			}
			{
				auto& line = state.world.land_battle_get_attacker_back_line(battle);
				for(auto& lr : line) {
					if(lr == reg.get_regiment())
						lr = dcon::regiment_id{};
				}
			}
			{
				auto& line = state.world.land_battle_get_defender_front_line(battle);
				for(auto& lr : line) {
					if(lr == reg.get_regiment())
						lr = dcon::regiment_id{};
				}
			}
			{
				auto& line = state.world.land_battle_get_defender_back_line(battle);
				for(auto& lr : line) {
					if(lr == reg.get_regiment())
						lr = dcon::regiment_id{};
				}
			}
			auto res = state.world.land_battle_get_reserves(battle);
			for(uint32_t i = res.size(); i-- > 0;) {
				if(res[i].regiment == reg.get_regiment()) {
					res[i] = res[res.size() - 1];
					res.pop_back();
				}
			}
		}

		//update leaders
		military::update_battle_leaders(state, battle);
	}
}

void move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest, bool reset) {



	command_data p{ command_type::move_navy, state.local_player_id };
	auto data = navy_movement_data{ n, dest, reset };
	p << data;
	add_to_command_queue(state, p);
}
std::vector<dcon::province_id> can_move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest, bool reset) {
	if(source != military::get_effective_unit_commander(state, n))
		return std::vector<dcon::province_id>{};
	if(state.world.navy_get_is_retreating(n))
		return std::vector<dcon::province_id>{};
	if(bool(state.world.navy_get_battle_from_navy_battle_participation(n))) {
		return std::vector<dcon::province_id>{}; // can't move while in battles, use retreat command
	}
	if(!dest)
		return std::vector<dcon::province_id>{}; // stop movement

	// Behavior for shift+click movement. Otherwise - path is cleared beforehand. If movement is reset, make the path from the units current location. If not reset, make the path from the destination prov
	auto last_province = state.world.navy_get_location_from_navy_location(n);
	if(!reset) {
		auto movement = state.world.navy_get_path(n);
		if(movement.size() > 0) {
			last_province = movement.at(0);
		}
	}
	// pass navy owner directly instead of source nation for path calculation, as the navy owner may be diffrent than source if commanding a subject's units
	auto navy_owner = state.world.navy_get_controller_from_navy_control(n);
	return calculate_navy_path(state, navy_owner, n, last_province, dest);
}


std::vector<dcon::province_id> calculate_navy_path(sys::state & state, dcon::nation_id source, dcon::navy_id n, dcon::province_id last_province, dcon::province_id dest) {

	if(last_province == dest)
		return std::vector<dcon::province_id>{};

	if(!can_partial_retreat_from(state, state.world.navy_get_battle_from_navy_battle_participation(n)))
		return std::vector<dcon::province_id>{};

	if(dest.index() >= state.province_definitions.first_sea_province.index()) {
		return province::make_naval_path(state, last_province, dest, source);
	} else {
		if(!state.world.province_get_is_coast(dest))
			return std::vector<dcon::province_id>{};

		if(!province::has_naval_access_to_province(state, source, dest))
			return std::vector<dcon::province_id>{};

		return province::make_naval_path(state, last_province, dest, source);
	}
}

void execute_move_navy(sys::state& state, dcon::nation_id source, dcon::navy_id n, dcon::province_id dest, bool reset) {

	auto navy_owner = state.world.navy_get_controller_from_navy_control(n);
	if(source != military::get_effective_unit_commander(state, n))
		return;
	if(state.world.navy_get_is_retreating(n))
		return;

	auto battle = state.world.navy_get_battle_from_navy_battle_participation(n);
	if(bool(battle)) {
		return;
	}
	if(!dest) {
		military::stop_navy_movement(state, n);
		return;
	}

	auto path = can_move_navy(state, source, n, dest, reset);
	if(!military::move_navy_fast(state, n, path, reset)) {
		if(reset) {
			military::stop_navy_movement(state, n);
		}
	}
	state.world.navy_set_moving_to_merge(n, false);
}

void embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {

	command_data p{ command_type::embark_army, state.local_player_id };
	auto data = army_movement_data{ };
	data.a = a;
	p << data;
	add_to_command_queue(state, p);
}

bool can_embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	auto army_owner = state.world.army_get_controller_from_army_control(a);
	if(source != military::get_effective_unit_commander(state, a))
		return false;
	if(state.world.army_get_is_retreating(a))
		return false;

	auto location = state.world.army_get_location_from_army_location(a);
	if(location.index() >= state.province_definitions.first_sea_province.index())
		return false;

	if(state.world.army_get_battle_from_army_battle_participation(a))
		return false;

	if(state.world.army_get_navy_from_army_transport(a)) {
		return true;
	} else {
		return military::can_embark_onto_sea_tile(state, army_owner, location, a);
	}
}

void execute_embark_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	auto army_owner = state.world.army_get_controller_from_army_control(a);
	if(source != military::get_effective_unit_commander(state, a))
		return;
	if(state.world.army_get_is_retreating(a))
		return;

	if(state.world.army_get_battle_from_army_battle_participation(a))
		return;

	auto location = state.world.army_get_location_from_army_location(a);
	if(location.index() >= state.province_definitions.first_sea_province.index())
		return;

	if(state.world.army_get_navy_from_army_transport(a)) {
		state.world.army_set_navy_from_army_transport(a, dcon::navy_id{});
		military::army_arrives_in_province(state, a, location, military::crossing_type::none);
	} else {
		auto to_navy = military::find_embark_target(state, army_owner, location, a);
		if(to_navy) {
			state.world.army_set_navy_from_army_transport(a, to_navy);
			state.world.army_set_black_flag(a, false);
		}
	}
	state.world.army_set_is_rebel_hunter(a, false);
	state.world.army_set_dig_in(a, 0);
}

void merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b) {

	command_data p{ command_type::merge_armies, state.local_player_id };
	auto data = merge_army_data{ a, b };
	p << data;
	add_to_command_queue(state, p);
}
bool can_merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b) {

	if(state.world.army_get_controller_from_army_control(a) != state.world.army_get_controller_from_army_control(b)) {
		return false;
	}

	if(military::get_effective_unit_commander(state, a) != source)
		return false;
	if(military::get_effective_unit_commander(state, b) != source)
		return false;
	if(state.world.army_get_is_retreating(a) || state.world.army_get_is_retreating(b))
		return false;
	if(state.world.army_get_navy_from_army_transport(a))
		return false;
	if(state.world.army_get_navy_from_army_transport(b))
		return false;

	if(state.world.army_get_location_from_army_location(a) != state.world.army_get_location_from_army_location(b))
		return false;

	if(state.world.army_get_battle_from_army_battle_participation(a) ||
			state.world.army_get_battle_from_army_battle_participation(b))
		return false;

	return true;
}

void execute_merge_armies(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::army_id b) {
	// take leader
	auto a_leader = state.world.army_get_general_from_army_leadership(a);
	auto b_leader = state.world.army_get_general_from_army_leadership(b);
	if(!a_leader && b_leader) {
		state.world.army_set_general_from_army_leadership(a, b_leader);
	}

	// stop movement
	military::stop_army_movement(state, a);

	// set dig in to the lowest value of the two armies
	state.world.army_set_dig_in(a,std::min(
		state.world.army_get_dig_in(a),
		state.world.army_get_dig_in(b)
	));

	auto regs = state.world.army_get_army_membership(b);
	while(regs.begin() != regs.end()) {
		auto reg = (*regs.begin()).get_regiment();
		reg.set_army_from_army_membership(a);
	}

	if(source == state.local_player_nation) {
		state.deselect(b);
	}
	military::cleanup_army(state, b);
}

void merge_navies(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::navy_id b) {

	command_data p{ command_type::merge_navies, state.local_player_id };
	auto data = merge_navy_data{ a, b };
	p << data;
	add_to_command_queue(state, p);
}
bool can_merge_navies(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::navy_id b) {

	if(state.world.navy_get_controller_from_navy_control(a) != state.world.navy_get_controller_from_navy_control(b)) {
		return false;
	}

	if(military::get_effective_unit_commander(state, a) != source)
		return false;
	if(military::get_effective_unit_commander(state, b) != source)
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

	military::stop_navy_movement(state, a);

	military::merge_navies_impl(state, a, b);

	if(source == state.local_player_nation) {
		state.deselect(b);
	}
	military::cleanup_navy(state, b);

}


void disband_undermanned_regiments(sys::state& state, dcon::nation_id source, dcon::army_id a) {

	command_data p{ command_type::disband_undermanned, state.local_player_id };
	auto data = army_movement_data{ };
	data.a = a;
	p << data;
	add_to_command_queue(state, p);
}
bool can_disband_undermanned_regiments(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	return state.world.army_get_controller_from_army_control(a) == source && !state.world.army_get_is_retreating(a) &&
		!bool(state.world.army_get_battle_from_army_battle_participation(a));
}
void execute_disband_undermanned_regiments(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	std::vector<dcon::regiment_id> regs;
	for(auto r : state.world.army_get_army_membership(a)) {
		auto pop = r.get_regiment().get_pop_from_regiment_source();
		if(!pop || pop.get_size() < state.defines.pop_min_size_for_regiment)
			regs.push_back(r.get_regiment());
	}
	for(auto r : regs)
		military::delete_regiment_safe_wrapper(state, r);
}

void toggle_rebel_hunting(sys::state& state, dcon::nation_id source, dcon::army_id a) {

	command_data p{ command_type::toggle_hunt_rebels, state.local_player_id };
	auto data = army_movement_data{  };
	data.a = a;
	p << data;
	add_to_command_queue(state, p);
}
void execute_toggle_rebel_hunting(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	auto owner = state.world.army_get_controller_from_army_control(a);
	if(owner != source)
		return;

	auto current_state = state.world.army_get_is_rebel_hunter(a);
	if(current_state) {
		state.world.army_set_is_rebel_hunter(a, false);
	} else {
		auto path = state.world.army_get_path(a);
		if(path.size() > 0) {
			state.world.army_set_ai_province(a, path.at(0));
		} else {
			state.world.army_set_ai_province(a, state.world.army_get_location_from_army_location(a));
			if(!state.world.army_get_battle_from_army_battle_participation(a)
				&& !state.world.army_get_navy_from_army_transport(a)) {

				military::send_rebel_hunter_to_next_province(state, a, state.world.army_get_location_from_army_location(a));
			}
		}
		state.world.army_set_is_rebel_hunter(a, true);
	}
}

void toggle_unit_ai_control(sys::state& state, dcon::nation_id source, dcon::army_id a) {

	command_data p{ command_type::toggle_unit_ai_control, state.local_player_id };
	auto data = army_movement_data{ };
	data.a = a;
	p << data;
	add_to_command_queue(state, p);
}
void execute_toggle_unit_ai_control(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	auto owner = state.world.army_get_controller_from_army_control(a);
	if(owner != source)
		return;
	auto current_state = state.world.army_get_is_ai_controlled(a);
	if(current_state) {
		state.world.army_set_ai_activity(a, 0);
		state.world.army_set_is_ai_controlled(a, false);
	} else {
		//turn off rebel control
		state.world.army_set_is_rebel_hunter(a, false);
		auto path = state.world.army_get_path(a);
		if(path.size() > 0) {
			state.world.army_set_ai_province(a, path.at(0));
		} else {
			state.world.army_set_ai_province(a, state.world.army_get_location_from_army_location(a));
			if(!state.world.army_get_battle_from_army_battle_participation(a)
				&& !state.world.army_get_navy_from_army_transport(a)) {

				military::send_rebel_hunter_to_next_province(state, a, state.world.army_get_location_from_army_location(a));
			}
		}
		state.world.army_set_ai_activity(a, 0);
		state.world.army_set_is_ai_controlled(a, true);
	}
}

void toggle_mobilized_is_ai_controlled(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::toggle_mobilized_is_ai_controlled, state.local_player_id };
	add_to_command_queue(state, p);
}
void execute_toggle_mobilized_is_ai_controlled(sys::state& state, dcon::nation_id source) {
	state.world.nation_set_mobilized_is_ai_controlled(source, !state.world.nation_get_mobilized_is_ai_controlled(source));
}

void change_unit_type(sys::state& state, dcon::nation_id source, dcon::regiment_id regiments[num_packed_units], dcon::ship_id ships[num_packed_units], dcon::unit_type_id new_type) {


	command_data p{ command_type::change_unit_type, state.local_player_id };
	auto data = change_unit_type_data{};
	data.new_type = new_type;
	for(unsigned i = 0; i < num_packed_units; i++) {
		if(regiments[i]) {
			data.regs[i] = regiments[i];
		}
		if(ships[i]) {
			data.ships[i] = ships[i];
		}
	}
	p << data;
	add_to_command_queue(state, p);
}
bool can_change_unit_type(sys::state& state, dcon::nation_id source, dcon::regiment_id regiments[num_packed_units], dcon::ship_id ships[num_packed_units], dcon::unit_type_id new_type) {
	if(regiments[0] && ships[0]) {
		// One type can't suit both land and sea units
		return false;
	}

	auto const& ut = state.military_definitions.unit_base_definitions[new_type];

	if(ut.is_land && ships[0]) {
		return false; // Land unit used for ships
	}
	else if(!ut.is_land && regiments[0]) {
		return false; // Sea unit used for land
	}

	if(!ut.active && !state.world.nation_get_active_unit(state.local_player_nation, new_type)) {
		return false; // Unit is not yet unlocked
	}

	if(!ut.is_land && ut.type == military::unit_type::big_ship) {
		for(unsigned i = 0; i < num_packed_units; i++) {
			if(!ships[i]) {
				break;
			}
			auto shiptype = state.world.ship_get_type(ships[i]);
			auto st = state.military_definitions.unit_base_definitions[shiptype];
			if(st.type != military::unit_type::big_ship) {
				return false; // Small ships can't become big ships
			}
		}
	}

	// Army-level checks
	for(unsigned i = 0; i < num_packed_units; i++) {
		if(!regiments[i]) {
			break;
		}
		auto a = state.world.regiment_get_army_from_army_membership(regiments[i]);

		if(state.world.army_get_controller_from_army_control(a) != source || state.world.army_get_is_retreating(a) || state.world.army_get_navy_from_army_transport(a) ||
		bool(state.world.army_get_battle_from_army_battle_participation(a))) {
			return false;
		}
	}
	// Navy-level checks
	for(unsigned i = 0; i < num_packed_units; i++) {
		if(!ships[i]) {
			break;
		}
		auto n = state.world.ship_get_navy_from_navy_membership(ships[i]);
		auto embarked = state.world.navy_get_army_transport(n);
		if(state.world.navy_get_controller_from_navy_control(n) != source || state.world.navy_get_is_retreating(n) ||
			bool(state.world.navy_get_battle_from_navy_battle_participation(n)) || embarked.begin() != embarked.end()) {
			return false;
		}

		if(ut.min_port_level) {
			auto fnid = dcon::fatten(state.world, n);

			auto loc = fnid.get_location_from_navy_location();

			// Ship requires naval base level for construction but province location doesn't have one
			if(loc.get_building_level(uint8_t(economy::province_building_type::naval_base)) < ut.min_port_level) {
				return false;
			}
		}
	}

	return true;
}
void execute_change_unit_type(sys::state& state, dcon::nation_id source, dcon::regiment_id regiments[num_packed_units], dcon::ship_id ships[num_packed_units], dcon::unit_type_id new_type) {
	for(unsigned i = 0; i < num_packed_units; i++) {
		if(regiments[i]) {
			if(state.world.regiment_get_type(regiments[i]) != new_type) {
				military::upgrade_regiment(state, regiments[i], new_type);
			}
		}
		if(ships[i]) {
			if(state.world.ship_get_type(ships[i]) != new_type) {
				military::upgrade_ship(state, ships[i], new_type);
			}
		}
	}
}

void toggle_select_province(sys::state& state, dcon::nation_id source, dcon::province_id prov) {

	command_data p{ command_type::toggle_select_province, state.local_player_id };
	auto data = generic_location_data{ prov };
	p << data;
	add_to_command_queue(state, p);

}
bool can_toggle_select_province(sys::state& state, dcon::nation_id source, dcon::province_id prov) {
	if(state.world.province_get_nation_from_province_control(prov) != source)
		return false;
	if(state.world.province_get_nation_from_province_ownership(prov) != source)
		return false;
	return true;
}
void execute_toggle_select_province(sys::state& state, dcon::nation_id source, dcon::province_id prov) {
	sys::toggle_modifier_from_province(state, prov, state.economy_definitions.selector_modifier, sys::date{});
}

void toggle_immigrator_province(sys::state& state, dcon::nation_id source, dcon::province_id prov) {

	command_data p{ command_type::toggle_immigrator_province, state.local_player_id };
	auto data = generic_location_data{ prov };
	p << data;
	add_to_command_queue(state, p);
}
bool can_toggle_immigrator_province(sys::state& state, dcon::nation_id source, dcon::province_id prov) {
	if(state.world.province_get_nation_from_province_control(prov) != source)
		return false;
	if(state.world.province_get_nation_from_province_ownership(prov) != source)
		return false;
	return true;
}
void execute_toggle_immigrator_province(sys::state& state, dcon::nation_id source, dcon::province_id prov) {
	sys::toggle_modifier_from_province(state, prov, state.economy_definitions.immigrator_modifier, sys::date{});
}

void release_subject(sys::state& state, dcon::nation_id source, dcon::nation_id target) {

	command_data p{ command_type::release_subject, state.local_player_id };
	auto data = diplo_action_data{ target };
	p << data;
	add_to_command_queue(state, p);

}
bool can_release_subject(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	return state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(target)) == source;
}
void execute_release_subject(sys::state& state, dcon::nation_id source, dcon::nation_id target) {
	if(!can_release_subject(state, source, target))
		return;
	nations::release_vassal(state, state.world.nation_get_overlord_as_subject(target));
}

void notify_console_command(sys::state& state) {

	command_data p{ command_type::console_command, state.local_player_id };
	add_to_command_queue(state, p);
}

void execute_console_command(sys::state& state) {
	std::string command;
	{
		std::lock_guard lg{ state.lock_console_strings };
		command = state.console_command_pending;
		state.console_command_pending.clear();
	}
	if(command.length() > 0) {
		ui::initialize_console_fif_environment(state);
		fif::interpreter_stack stack;
		fif::run_fif_interpreter(*state.fif_environment, command, stack);
		if(state.fif_environment->mode == fif::fif_mode::error) {
			state.fif_environment->mode = fif::fif_mode::interpreting;
			std::lock_guard lg{ state.lock_console_strings };
			state.console_command_result += state.console_command_error;
			state.console_command_error.clear();
		} else {
			std::string temp_result;
			for(uint32_t i = 0; i < stack.main_size(); ++i) {
				temp_result += ui::format_fif_value(state, stack.main_data(i), stack.main_type(i));
				temp_result += " ";
			}
			if(stack.main_size() > 0) {
				state.fif_environment->source_stack.push_back("drop");
				state.fif_environment->compiler_stack.emplace_back(std::make_unique<fif::outer_interpreter>(*state.fif_environment));
				fif::outer_interpreter* o = static_cast<fif::outer_interpreter*>(state.fif_environment->compiler_stack.back().get());
				static_cast<fif::interpreter_stack*>(o->interpreter_state.get())->move_into(std::move(stack));

				switch_compiler_stack_mode(*state.fif_environment, fif::fif_mode::interpreting);
				fif::mode_switch_scope* m = static_cast<fif::mode_switch_scope*>(state.fif_environment->compiler_stack.back().get());
				m->interpreted_link = o;

				while(o->interpreter_state->main_size() > 0) {
					fif::execute_fif_word(fif::parse_result{ "drop", false }, *state.fif_environment, false);
				}

				state.fif_environment->source_stack.pop_back();
				restore_compiler_stack_mode(*state.fif_environment);

				state.fif_environment->compiler_stack.pop_back();
			}
			temp_result += "?Gok.?W\\n";
			{
				std::lock_guard lg{ state.lock_console_strings };
				state.console_command_result += temp_result;
			}
			state.fif_environment->mode = fif::fif_mode::interpreting;
		}
	}
}

void evenly_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {


	command_data p{ command_type::even_split_army, state.local_player_id };
	auto data = army_movement_data{ };
	data.a = a;
	p << data;
	add_to_command_queue(state, p);
}
bool can_evenly_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	return can_split_army(state, source, a);
}
void execute_evenly_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	bool inf_split = false;
	bool cav_split = false;
	bool art_split = false;

	static std::vector<dcon::regiment_id> to_transfer;
	to_transfer.clear();

	for(auto t : state.world.army_get_army_membership(a)) {
		auto type = state.military_definitions.unit_base_definitions[t.get_regiment().get_type()].type;
		if(type == military::unit_type::infantry) {
			if(inf_split) {
				to_transfer.push_back(t.get_regiment());
			}
			inf_split = !inf_split;
		} else if(type == military::unit_type::cavalry) {
			if(cav_split) {
				to_transfer.push_back(t.get_regiment());
			}
			cav_split = !cav_split;
		} else if(type == military::unit_type::support || type == military::unit_type::special) {
			if(art_split) {
				to_transfer.push_back(t.get_regiment());
			}
			art_split = !art_split;
		}
	}

	if(to_transfer.size() > 0) {
		auto new_u = fatten(state.world, state.world.create_army());
		new_u.set_controller_from_army_control(state.world.army_get_controller_from_army_control(a));
		new_u.set_location_from_army_location(state.world.army_get_location_from_army_location(a));
		new_u.set_black_flag(state.world.army_get_black_flag(a));
		new_u.set_dig_in(state.world.army_get_dig_in(a));

		for(auto t : to_transfer) {
			state.world.regiment_set_army_from_army_membership(t, new_u);
		}

		if(source == state.local_player_nation && state.is_selected(a)) {
			state.deselect(a);
			state.select(new_u);
		}
	}
}

void evenly_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {


	command_data p{ command_type::even_split_navy, state.local_player_id };
	auto data = navy_movement_data{ };
	data.n = a;
	p << data;
	add_to_command_queue(state, p);

}
bool can_evenly_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	return can_split_navy(state, source, a);
}
void execute_evenly_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	static std::vector<dcon::ship_id> to_transfer;
	to_transfer.clear();

	bool big_split = false;
	bool sm_split = false;
	bool tra_split = false;

	for(auto t : state.world.navy_get_navy_membership(a)) {
		auto type = state.military_definitions.unit_base_definitions[t.get_ship().get_type()].type;
		if(type == military::unit_type::big_ship) {
			if(big_split) {
				to_transfer.push_back(t.get_ship());
			}
			big_split = !big_split;
		} else if(type == military::unit_type::light_ship) {
			if(sm_split) {
				to_transfer.push_back(t.get_ship());
			}
			sm_split = !sm_split;
		} else if(type == military::unit_type::transport) {
			if(tra_split) {
				to_transfer.push_back(t.get_ship());
			}
			tra_split = !tra_split;
		}
	}

	if(to_transfer.size() > 0) {
		auto new_u = fatten(state.world, state.world.create_navy());
		new_u.set_controller_from_navy_control(state.world.navy_get_controller_from_navy_control(a));
		new_u.set_location_from_navy_location(state.world.navy_get_location_from_navy_location(a));
		new_u.set_months_outside_naval_range(state.world.navy_get_months_outside_naval_range(a));

		for(auto t : to_transfer) {
			state.world.ship_set_navy_from_navy_membership(t, new_u);
		}

		if(source == state.local_player_nation && state.is_selected(a)) {
			state.deselect(a);
			state.select(new_u);
		}
	}
}

void split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {

	command_data p{ command_type::split_army, state.local_player_id };
	auto data = army_movement_data{ };
	data.a = a;
	p << data;
	add_to_command_queue(state, p);
}
bool can_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	return military::get_effective_unit_commander(state, a) == source && !state.world.army_get_is_retreating(a) && !state.world.army_get_navy_from_army_transport(a) &&
		!bool(state.world.army_get_battle_from_army_battle_participation(a));
}
void execute_split_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
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
		new_u.set_controller_from_army_control(state.world.army_get_controller_from_army_control(a));
		new_u.set_location_from_army_location(state.world.army_get_location_from_army_location(a));
		new_u.set_black_flag(state.world.army_get_black_flag(a));
		new_u.set_dig_in(state.world.army_get_dig_in(a));

		for(auto t : to_transfer) {
			state.world.regiment_set_army_from_army_membership(t, new_u);
		}

		if(source == state.local_player_nation && state.is_selected(a))
			state.select(new_u);

		auto old_regs = state.world.army_get_army_membership(a);
		if(old_regs.begin() == old_regs.end()) {
			state.world.leader_set_army_from_army_leadership(state.world.army_get_general_from_army_leadership(a), new_u);

			if(source == state.local_player_nation) {
				state.deselect(a);
			}
			military::cleanup_army(state, a);
		}
	}
}

void split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {

	command_data p{ command_type::split_navy, state.local_player_id };
	auto data = navy_movement_data{ };
	data.n = a;
	p << data;
	add_to_command_queue(state, p);

}
bool can_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	auto embarked = state.world.navy_get_army_transport(a);
	return military::get_effective_unit_commander(state, a) == source && !state.world.navy_get_is_retreating(a) &&
		!bool(state.world.navy_get_battle_from_navy_battle_participation(a)) && embarked.begin() == embarked.end();
}
void execute_split_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
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
		new_u.set_controller_from_navy_control(state.world.navy_get_controller_from_navy_control(a));
		new_u.set_location_from_navy_location(state.world.navy_get_location_from_navy_location(a));
		new_u.set_months_outside_naval_range(state.world.navy_get_months_outside_naval_range(a));

		for(auto t : to_transfer) {
			state.world.ship_set_navy_from_navy_membership(t, new_u);
		}

		if(source == state.local_player_nation && state.is_selected(a))
			state.select(new_u);

		auto old_regs = state.world.navy_get_navy_membership(a);
		if(old_regs.begin() == old_regs.end()) {
			state.world.leader_set_navy_from_navy_leadership(state.world.navy_get_admiral_from_navy_leadership(a), new_u);
			if(source == state.local_player_nation) {
				state.deselect(a);
			}
			military::cleanup_navy(state, a);
		}
	}
}

void delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	command_data p{ command_type::delete_army, state.local_player_id };
	auto data = army_movement_data{ };
	data.a = a;
	p << data;
	add_to_command_queue(state, p);

}
bool can_delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	return state.world.army_get_controller_from_army_control(a) == source
		&& !state.world.army_get_is_retreating(a)
		&& !bool(state.world.army_get_battle_from_army_battle_participation(a))
		&& (
			province::has_safe_access_to_province(
				state,
				source,
				state.world.army_get_location_from_army_location(a)
			)
			|| state.world.army_get_black_flag(a)
		);
}
void execute_delete_army(sys::state& state, dcon::nation_id source, dcon::army_id a) {
	if(source == state.local_player_nation) {
		state.deselect(a);
	}
	military::cleanup_army(state, a);
}

void delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {

	command_data p{ command_type::delete_navy, state.local_player_id };
	auto data = navy_movement_data{  };
	data.n = a;
	p << data;
	add_to_command_queue(state, p);

}

bool can_delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	auto embarked = state.world.navy_get_army_transport(a);
	return state.world.navy_get_controller_from_navy_control(a) == source && !state.world.navy_get_is_retreating(a) &&
		embarked.begin() == embarked.end() && !bool(state.world.navy_get_battle_from_navy_battle_participation(a)) &&
		province::has_naval_access_to_province(state, source, state.world.navy_get_location_from_navy_location(a));
}
void execute_delete_navy(sys::state& state, dcon::nation_id source, dcon::navy_id a) {
	if(source == state.local_player_nation) {
		state.deselect(a);
	}
	military::cleanup_navy(state, a);
}

void change_general(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::leader_id l) {


	command_data p{ command_type::change_general, state.local_player_id };
	auto data = new_general_data{ a, l };
	p << data;
	add_to_command_queue(state, p);
}
bool can_change_general(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::leader_id l) {
	return state.world.army_get_controller_from_army_control(a) == source && !state.world.army_get_is_retreating(a) &&
		!bool(state.world.army_get_battle_from_army_battle_participation(a)) &&
		province::has_naval_access_to_province(state, source, state.world.army_get_location_from_army_location(a)) &&
		(!l || state.world.leader_get_nation_from_leader_loyalty(l) == source);
}
void execute_change_general(sys::state& state, dcon::nation_id source, dcon::army_id a, dcon::leader_id l) {
	state.world.army_set_general_from_army_leadership(a, l);
}

void change_admiral(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::leader_id l) {


	command_data p{ command_type::change_admiral, state.local_player_id };
	auto data = new_admiral_data{ a, l };
	p << data;
	add_to_command_queue(state, p);
}
bool can_change_admiral(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::leader_id l) {
	return state.world.navy_get_controller_from_navy_control(a) == source && !state.world.navy_get_is_retreating(a) &&
		!bool(state.world.navy_get_battle_from_navy_battle_participation(a)) &&
		province::has_naval_access_to_province(state, source, state.world.navy_get_location_from_navy_location(a)) &&
		(!l || state.world.leader_get_nation_from_leader_loyalty(l) == source);
}
void execute_change_admiral(sys::state& state, dcon::nation_id source, dcon::navy_id a, dcon::leader_id l) {
	state.world.navy_set_admiral_from_navy_leadership(a, l);
}

void mark_regiments_to_split(sys::state& state, dcon::nation_id source,
		std::array<dcon::regiment_id, num_packed_units> const& list) {

	command_data p{ command_type::designate_split_regiments, state.local_player_id };
	auto data = split_regiments_data{ };
	std::copy_n(list.data(), num_packed_units, data.regs);
	p << data;
	add_to_command_queue(state, p);
}
void execute_mark_regiments_to_split(sys::state& state, dcon::nation_id source, dcon::regiment_id const* regs) {
	for(uint32_t i = 0; i < num_packed_units; ++i) {
		if(regs[i]) {
			if(source ==
					military::get_effective_unit_commander(state ,state.world.regiment_get_army_from_army_membership(regs[i]))) {
				state.world.regiment_set_pending_split(regs[i], !state.world.regiment_get_pending_split(regs[i]));
			}
		}
	}
}

void mark_ships_to_split(sys::state& state, dcon::nation_id source, std::array<dcon::ship_id, num_packed_units> const& list) {

	command_data p{ command_type::designate_split_ships, state.local_player_id };
	auto data = split_ships_data{ };
	std::copy_n(list.data(), num_packed_units, data.ships);
	p << data;
	add_to_command_queue(state, p);


}
void execute_mark_ships_to_split(sys::state& state, dcon::nation_id source, dcon::ship_id const* regs) {
	for(uint32_t i = 0; i < num_packed_units; ++i) {
		if(regs[i]) {
			if(source == military::get_effective_unit_commander(state,state.world.ship_get_navy_from_navy_membership(regs[i]))) {
				state.world.ship_set_pending_split(regs[i], !state.world.ship_get_pending_split(regs[i]));
			}
		}
	}
}

void retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::navy_id navy, dcon::province_id dest) {
	command_data p{ command_type::naval_retreat, state.local_player_id };
	auto data = retreat_from_naval_battle_data{ navy, dest };
	p << data;
	add_to_command_queue(state, p);
}
std::vector<dcon::province_id> can_retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::navy_id navy, military::retreat_type retreat_type, dcon::province_id dest) {
	auto battle = state.world.navy_get_battle_from_navy_battle_participation(navy);
	if(!bool(battle)) {
		return std::vector<dcon::province_id>{};
	}
	if(!military::is_battle_retreatable(state, battle, retreat_type))
		return std::vector<dcon::province_id>{};


	if(source != military::get_effective_unit_commander(state, navy)) {
		return std::vector<dcon::province_id>{};
	}
	if(state.world.navy_get_is_retreating(navy))
		return std::vector<dcon::province_id>{};
	if(retreat_type == military::retreat_type::manual) {
		if(dest == state.world.navy_get_location_from_navy_location(navy)) {
			return std::vector<dcon::province_id>{};
		}
	}

	return province::make_naval_retreat_path(state, source, state.world.navy_get_location_from_navy_location(navy));
}
void execute_retreat_from_naval_battle(sys::state& state, dcon::nation_id source, dcon::navy_id navy, dcon::province_id dest = dcon::province_id{ }) {
	// This can be extended with diffrent retreat rules later, but to start this is compliant with Vic2 naval retreat behaviour
	auto battle = state.world.navy_get_battle_from_navy_battle_participation(navy);
	if(!military::is_battle_retreatable(state, battle, military::retreat_type::manual))
		return;
	// For navies, a retreat command will start retreating the navy from the battle.
	// In Vic2, navies do not retreat from the battle instantly unlike land units. Each ship which is part of the navy will start to retreat, and when all ships of the navy are retreated, the navy will finally exit the battle.
	// The navy retreats to the closest port province, regardless of which province the user clicked on when retreating
	assert(battle);
	bool can_retreat = military::retreat<military::battle_is_ending::no>(state, navy, military::retreat_type::manual);
	// navy must be able to retreat, otherwise it shouldnt have passed the "can_retreat_from_naval_battle" check
	assert(can_retreat);
	for(auto shp : state.world.navy_get_navy_membership(navy)) {
		for(auto& s : state.world.naval_battle_get_slots(battle)) {
			if(s.ship == shp.get_ship() && (s.flags & s.mode_mask) != s.mode_sunk && (s.flags & s.mode_mask) != s.mode_retreated) {
				military::single_ship_start_retreat(state, s, battle);
			}
		}
	}
	state.world.navy_set_moving_to_merge(navy, false);

}

void retreat_from_land_battle(sys::state& state, dcon::nation_id source, dcon::land_battle_id b) {

	command_data p{ command_type::land_retreat, state.local_player_id };
	auto data = land_battle_data{ b };
	p << data;
	add_to_command_queue(state, p);
}

bool can_retreat_from_land_battle(sys::state& state, dcon::nation_id source, dcon::land_battle_id b) {
	if(!military::can_retreat_from_battle(state, b))
		return false;
	if(source != military::get_land_battle_lead_attacker(state, b) && source != military::get_land_battle_lead_defender(state, b))
		return false;

	return true;
}
void execute_retreat_from_land_battle(sys::state& state, dcon::nation_id source, dcon::land_battle_id b) {
	if(!military::can_retreat_from_battle(state, b))
		return;

	if(source == military::get_land_battle_lead_attacker(state, b)) {
		military::end_battle(state, b, military::battle_result::defender_won);
	} else if(source == military::get_land_battle_lead_defender(state, b)) {
		military::end_battle(state, b, military::battle_result::attacker_won);
	}
}

void invite_to_crisis(sys::state& state, dcon::nation_id source, dcon::nation_id invitation_to, dcon::nation_id target,
		dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag,
		dcon::nation_id cb_secondary_nation) {

	command_data p{ command_type::invite_to_crisis, state.local_player_id };
	auto data = crisis_invitation_data{ };
	data.invited = invitation_to;
	data.target = target;
	data.cb_type = primary_cb;
	data.cb_state = cb_state;
	data.cb_tag = cb_tag;
	data.cb_secondary_nation = cb_secondary_nation;

	p << data;
	add_to_command_queue(state, p);
}
bool can_invite_to_crisis(sys::state& state, dcon::nation_id source, dcon::nation_id invitation_to, dcon::nation_id target,
		dcon::cb_type_id primary_cb, dcon::state_definition_id cb_state, dcon::national_identity_id cb_tag,
		dcon::nation_id cb_secondary_nation) {

	if(state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < 1.0f)
		return false;

	if(state.current_crisis_state != sys::crisis_state::heating_up)
		return false;
	if(source != state.primary_crisis_attacker && source != state.primary_crisis_defender)
		return false;

	if(invitation_to == target || source == target)
		return false;

	// to must be merely interested participant
	for(auto& par : state.crisis_participants) {
		if(!par.id) {
			return false; // not in crisis
		}
		if(par.id == invitation_to) {
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
		if(par.id == target) {
			if(par.merely_interested)
				return false;
			if(par.supports_attacker && source == state.primary_crisis_attacker)
				return false;
			if(!par.supports_attacker && source == state.primary_crisis_defender)
				return false;

			break;
		}
	}

	// no duplicates
	auto wargoalslist = (source == state.primary_crisis_attacker) ? state.crisis_attacker_wargoals : state.crisis_defender_wargoals;
	for(auto ewg : wargoalslist) {
		// Different added_by, similar everything else
		if (ewg.state == cb_state && ewg.wg_tag == cb_tag &&
						ewg.secondary_nation == cb_secondary_nation && ewg.target_nation == target)
			return false;

		if(!ewg.cb) {
			break;
		}
	}

	auto cb_type = state.world.cb_type_get_type_bits(primary_cb);
	if((cb_type & military::cb_flag::always) == 0 && (cb_type & military::cb_flag::is_not_constructing_cb) != 0)
		return false;

	if(!military::cb_instance_conditions_satisfied(state, invitation_to, target, primary_cb, cb_state, cb_tag, cb_secondary_nation)) {
		return false;
	}

	return true;
}
void execute_invite_to_crisis(sys::state& state, dcon::nation_id source, crisis_invitation_data const& data) {
	if(state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < 1.0f)
		return;

	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - 1.0f);

	diplomatic_message::message m;
	memset(&m, 0, sizeof(diplomatic_message::message));
	m.to = data.invited;
	m.from = source;
	m.data.crisis_offer.added_by = m.to;
	m.data.crisis_offer.target_nation = data.target;
	m.data.crisis_offer.secondary_nation = data.cb_secondary_nation;
	m.data.crisis_offer.state = data.cb_state;
	m.data.crisis_offer.wg_tag = data.cb_tag;
	m.data.crisis_offer.cb = data.cb_type;

	m.type = diplomatic_message::type::take_crisis_side_offer;

	diplomatic_message::post(state, m);
}

void queue_crisis_add_wargoal(sys::state& state, dcon::nation_id source, sys::full_wg wg) {

	command_data p{ command_type::crisis_add_wargoal, state.local_player_id };
	auto data = new_war_goal_data{ };
	data.target = wg.target_nation;
	data.cb_type = wg.cb;
	data.cb_state = wg.state;
	data.cb_tag = wg.wg_tag;
	data.cb_secondary_nation = wg.secondary_nation;
	p << data;
	add_to_command_queue(state, p);
}

void execute_crisis_add_wargoal(sys::state& state, dcon::nation_id source, new_war_goal_data const& data) {
	if(state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < 1.0f)
		return;

	auto& wargoalslist = (source == state.primary_crisis_attacker) ? state.crisis_attacker_wargoals : state.crisis_defender_wargoals;
	nations::crisis_add_wargoal(wargoalslist, sys::full_wg{
		source, // added_by;
		data.target, // target_nation;
		data.cb_secondary_nation, //  secondary_nation;
		data.cb_tag, // wg_tag;
		data.cb_state, // state;
		data.cb_type // cb
	});
	auto infamy = military::crisis_cb_addition_infamy_cost(state, data.cb_type, source, data.target, data.cb_state);
	state.world.nation_set_infamy(source, state.world.nation_get_infamy(source) + infamy);

	auto& current_diplo = state.world.nation_get_diplomatic_points(source);
	state.world.nation_set_diplomatic_points(source, current_diplo - 1.0f);
}

bool crisis_can_add_wargoal(sys::state& state, dcon::nation_id source, sys::full_wg wg) {

	if(state.world.nation_get_is_player_controlled(source) && state.world.nation_get_diplomatic_points(source) < 1.0f)
		return false;

	if(state.current_crisis_state != sys::crisis_state::heating_up)
		return false;
	if(source != state.primary_crisis_attacker && source != state.primary_crisis_defender)
		return false;

	// target must be in crisis
	for(auto& par : state.crisis_participants) {
		if(!par.id) {
			return false; // not in crisis
		}
		if(par.id == wg.target_nation) {
			if(par.merely_interested)
				return false;
			if(par.supports_attacker && source == state.primary_crisis_attacker)
				return false;
			if(!par.supports_attacker && source == state.primary_crisis_defender)
				return false;

			break;
		}
	}

	auto cb_type = state.world.cb_type_get_type_bits(wg.cb);
	if((cb_type & military::cb_flag::always) == 0 && (cb_type & military::cb_flag::is_not_constructing_cb) != 0)
		return false;

	if(!military::cb_instance_conditions_satisfied(state, source, wg.target_nation, wg.cb, wg.state, wg.wg_tag, wg.secondary_nation)) {
		return false;
	}

	// no duplicates
	auto wargoalslist = (source == state.primary_crisis_attacker) ? state.crisis_attacker_wargoals : state.crisis_defender_wargoals;
	for(auto ewg : wargoalslist) {
		if(ewg.added_by == wg.added_by && ewg.state == wg.state && ewg.wg_tag == wg.wg_tag &&
						ewg.secondary_nation == wg.secondary_nation && ewg.target_nation == wg.target_nation &&
						ewg.cb == wg.cb)
			return false;
		if(!ewg.cb) {
			break;
		}
	}

	return true;
}

void toggle_mobilization(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::toggle_mobilization, state.local_player_id };
	add_to_command_queue(state, p);

}

void execute_toggle_mobilization(sys::state& state, dcon::nation_id source) {
	if(state.world.nation_get_is_mobilized(source)) {
		military::end_mobilization(state, source);
	} else {
		military::start_mobilization(state, source);
	}
}

void enable_debt(sys::state& state, dcon::nation_id source, bool debt_is_enabled) {

	command_data p{ command_type::enable_debt, state.local_player_id };
	auto data = make_leader_data{ debt_is_enabled };
	p << data;
	add_to_command_queue(state, p);
}

void execute_enable_debt(sys::state& state, dcon::nation_id source, bool debt_is_enabled) {
	state.world.nation_set_is_debt_spending(source, debt_is_enabled);
}

void move_capital(sys::state& state, dcon::nation_id source, dcon::province_id prov) {

	command_data p{ command_type::move_capital, state.local_player_id };
	auto data = generic_location_data{ prov };
	p << data;
	add_to_command_queue(state, p);

}

bool can_move_capital(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	if(state.current_crisis_state != sys::crisis_state::inactive)
		return false;
	if(state.world.nation_get_is_at_war(source))
		return false;
	if(state.world.nation_get_capital(source) == p)
		return false;
	if(state.world.province_get_is_colonial(p))
		return false;
	if(state.world.province_get_continent(state.world.nation_get_capital(source)) != state.world.province_get_continent(p))
		return false;
	if(nations::nation_accepts_culture(state, source, state.world.province_get_dominant_culture(p)) == false)
		return false;
	if(state.world.province_get_siege_progress(p) > 0.f)
		return false;
	if(state.world.province_get_siege_progress(state.world.nation_get_capital(source)) > 0.f)
		return false;
	if(state.world.province_get_nation_from_province_ownership(p) != source)
		return false;
	if(state.world.province_get_nation_from_province_control(p) != source)
		return false;
	if(state.world.province_get_is_owner_core(p) == false)
		return false;
	return true;
}

void execute_move_capital(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	state.world.nation_set_capital(source, p);
}

void toggle_local_administration(sys::state& state, dcon::nation_id source, dcon::province_id prov) {

	command_data p{ command_type::toggle_local_administration, state.local_player_id };
	auto data = generic_location_data{ prov };
	p << data;
	add_to_command_queue(state, p);

}
bool can_toggle_local_administration(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	// allow planning future capitals
	// capitals outside of your actual control are not updated
	// also allows to not handle deletion of administations
	auto rebels = state.world.national_identity_get_nation_from_identity_holder(state.national_definitions.rebel_id);
	if(source == rebels || !source) {
		return false;
	}
	return true;
}
void execute_toggle_local_administration(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	dcon::administration_id existing_admin{ };
	state.world.nation_for_each_nation_administration(source, [&](auto naid) {
		auto aid = state.world.nation_administration_get_administration(naid);
		auto capital = state.world.administration_get_capital(aid);
		if(p == capital) {
			existing_admin = aid;
		}
	});
	if(existing_admin) {
		state.world.delete_administration(existing_admin);
	} else {
		auto admin = state.world.create_administration();
		state.world.administration_set_capital(admin, p);
		state.world.force_create_nation_administration(admin, source);
	}
}

void toggle_production_directive(sys::state& state, dcon::nation_id source, dcon::state_instance_id for_state, dcon::production_directive_id directive) {
	command_data p{ command_type::toggle_production_directive, state.local_player_id };
	auto data = production_directive_data{ for_state, directive };
	p << data;
	add_to_command_queue(state, p);
}
void execute_toggle_production_directive(sys::state& state, dcon::nation_id source, dcon::state_instance_id for_state, dcon::production_directive_id directive) {
	if(!source || !directive)
		return;

	if(for_state) {
		if(source == state.world.state_instance_get_nation_from_state_ownership(for_state)) {
			state.world.state_instance_set_production_directive(for_state, directive, !(state.world.state_instance_get_production_directive(for_state, directive)));
		}
	} else {
		state.world.nation_set_production_directive(source, directive, !(state.world.nation_get_production_directive(source, directive)));
	}
}

void take_province(sys::state& state, dcon::nation_id source, dcon::province_id prov) {


	command_data p{ command_type::take_province, state.local_player_id };
	auto data = generic_location_data{ prov };
	p << data;
	add_to_command_queue(state, p);

}

bool can_take_province(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	auto fid = dcon::fatten(state.world, p);
	auto owner = fid.get_nation_from_province_ownership();
	auto rel = state.world.nation_get_overlord_as_subject(owner);
	auto overlord = state.world.overlord_get_ruler(rel);

	if(state.defines.alice_allow_revoke_subject_states == 0.0f)
		return false;

	if(overlord != source)
		return false;
	if(state.current_crisis_state != sys::crisis_state::inactive)
		return false;
	if(state.world.nation_get_is_at_war(source))
		return false;
	if(state.world.nation_get_is_at_war(owner))
		return false;
	if(state.world.province_get_siege_progress(p) > 0.f)
		return false;
	if(state.world.province_get_siege_progress(state.world.nation_get_capital(source)) > 0.f)
		return false;
	// Occupied
	if(state.world.province_get_nation_from_province_control(p) != owner)
		return false;
	return true;
}

void execute_take_province(sys::state& state, dcon::nation_id source, dcon::province_id p) {
	auto fid = dcon::fatten(state.world, p);
	auto owner = fid.get_province_ownership_as_province().get_nation();

	/*
	- The province gets nationalism equal to define:YEARS_OF_NATIONALISM
	*/
	state.world.province_set_nationalism(p, state.defines.years_of_nationalism);

	for(auto n : state.world.in_nation) {
		if(n == owner) {
			// All provinces in that subject get extra militancy
			demographics::modify_militancy(state, n, state.defines.alice_take_province_militancy_subject);
		}

		auto rel = state.world.nation_get_overlord_as_subject(owner);
		auto overlord = state.world.overlord_get_ruler(rel);

		if(overlord == source) {
			// All other subjects get extra militancy
			demographics::modify_militancy(state, n, state.defines.alice_take_province_militancy_all_subjects);
		}
	}

	fid.get_province_ownership_as_province().set_nation(source);
	fid.get_province_control_as_province().set_nation(source);
}

void use_province_button(sys::state& state, dcon::nation_id source, dcon::gui_def_id d, dcon::province_id i) {

	command_data p{ command_type::pbutton_script, state.local_player_id };
	auto data = pbutton_data{ d, i };
	p << data;
	add_to_command_queue(state, p);

}
bool can_use_province_button(sys::state& state, dcon::nation_id source, dcon::gui_def_id d, dcon::province_id p) {
	auto& def = state.ui_defs.gui[d];
	if(def.get_element_type() != ui::element_type::button)
		return false;
	if(def.data.button.get_button_scripting() != ui::button_scripting::province)
		return false;
	if(!def.data.button.scriptable_enable)
		return true;
	return trigger::evaluate(state, def.data.button.scriptable_enable, trigger::to_generic(p), trigger::to_generic(p), trigger::to_generic(source));
}
void execute_use_province_button(sys::state& state, dcon::nation_id source, dcon::gui_def_id d, dcon::province_id p) {
	auto & def = state.ui_defs.gui[d];
	if(def.data.button.scriptable_effect)
		effect::execute(state, def.data.button.scriptable_effect, trigger::to_generic(p), trigger::to_generic(p), trigger::to_generic(source), uint32_t(state.current_date.value), uint32_t(p.index() ^ (d.index() << 4)));
}

void use_nation_button(sys::state& state, dcon::nation_id source, dcon::gui_def_id d, dcon::nation_id n) {

	command_data p{ command_type::nbutton_script, state.local_player_id };
	auto data = nbutton_data{ d, n };
	p << data;
	add_to_command_queue(state, p);

}
bool can_use_nation_button(sys::state& state, dcon::nation_id source, dcon::gui_def_id d, dcon::nation_id n) {
	auto& def = state.ui_defs.gui[d];
	if(def.get_element_type() != ui::element_type::button)
		return false;
	if(def.data.button.get_button_scripting() != ui::button_scripting::nation)
		return false;
	if(!def.data.button.scriptable_enable)
		return true;
	return trigger::evaluate(state, def.data.button.scriptable_enable, trigger::to_generic(n), trigger::to_generic(n), trigger::to_generic(source));
}
void execute_use_nation_button(sys::state& state, dcon::nation_id source, dcon::gui_def_id d, dcon::nation_id n) {
	auto& def = state.ui_defs.gui[d];
	if(def.data.button.scriptable_effect)
		effect::execute(state, def.data.button.scriptable_effect, trigger::to_generic(n), trigger::to_generic(n), trigger::to_generic(source), uint32_t(state.current_date.value), uint32_t(n.index() ^ (d.index() << 4)));
}

void post_chat_message(sys::state& state, ui::chat_message& m) {
	// Private message
	bool can_see = true;
	if(bool(m.target)) {
		can_see = state.local_player_nation == m.source || state.local_player_nation == m.target;
	}
	if(can_see) {
		state.ui_state.chat_messages[state.ui_state.chat_messages_index++] = m;
		if(state.ui_state.chat_messages_index >= state.ui_state.chat_messages.size())
			state.ui_state.chat_messages_index = 0;
		notification::post(state, notification::message{
			[m](sys::state& state, text::layout_base& contents) {
				text::add_line(state, contents, "msg_chat_message_1", text::variable_type::x, m.source);
				text::add_line(state, contents, "msg_chat_message_2", text::variable_type::x, m.body);
			},
			"msg_chat_message_title",
			m.source, dcon::nation_id{}, dcon::nation_id{},
			sys::message_base_type::chat_message
		});
	}
}

void chat_message(sys::state& state, dcon::nation_id source, std::string_view body, dcon::nation_id target) {


	command_data p{ command_type::chat_message, state.local_player_id };
	auto data = chat_message_data{ };
	data.target = target;

	data.msg_len = std::min<uint16_t>(uint16_t(body.length()), ui::max_chat_message_len);

	p << data;
	p.push_ptr(std::string(body).c_str(), std::min<uint16_t>(uint16_t(body.length()), ui::max_chat_message_len));
	add_to_command_queue(state, p);
}
bool can_chat_message(sys::state& state, command_data& command) {
	// TODO: bans, kicks, mutes?

	auto& payload = command.get_payload<chat_message_data>();

	// check that the message length is correct before reading from it
	if(!command.check_variable_size_payload<chat_message_data>(payload.msg_len)) {
		assert(false && "Variable command with a inconsistent size recieved!");
		return false;
	}

	return true;
}
void execute_chat_message(sys::state& state, dcon::nation_id source, std::string_view body, dcon::nation_id target, dcon::mp_player_id sender) {
	ui::chat_message m{};
	m.source = source;
	m.target = target;
	m.body = std::string(body);
	m.set_sender_name(state.world.mp_player_get_nickname(sender));
	post_chat_message(state, m);
}

void notify_player_joins(sys::state& state, dcon::nation_id source, const sys::player_name& name, const sys::player_password_raw& password, bool needs_loading, dcon::nation_id player_nation) {
	assert(state.network_mode == sys::network_mode_type::host);
	command_data p{ command_type::notify_player_joins, state.local_player_id};
	auto data = notify_joins_data{  };
	data.needs_loading = needs_loading;
	data.player_name = name;
	data.player_nation = player_nation;
	p << data;
	add_to_command_queue(state, p);
}
bool can_notify_player_joins(sys::state& state, dcon::nation_id source, const sys::player_name& name, bool needs_loading, dcon::nation_id player_nation) {
	// TODO: bans, kicks, mutes?
	return true;
}

bool can_change_gamerule_setting(sys::state& state, dcon::nation_id source, dcon::gamerule_id gamerule, uint8_t new_setting) {
	return state.world.gamerule_get_settings_count(gamerule) >= new_setting + 1 && new_setting < sys::max_gamerule_settings;
}

void execute_change_gamerule_setting(sys::state& state, dcon::nation_id source, dcon::gamerule_id gamerule, uint8_t new_setting) {
	gamerule::set_gamerule(state, gamerule, new_setting);
	auto sub = text::substitution_map{ };
	text::add_to_substitution_map(sub, text::variable_type::x, state.world.gamerule_get_name(gamerule));
	auto setting_name = state.world.gamerule_get_options(gamerule)[new_setting].name;
	text::add_to_substitution_map(sub, text::variable_type::y, setting_name);
	auto str = text::resolve_string_substitution(state, "alice_gamerules_change_chat_msg", sub);
	if(state.network_mode == sys::network_mode_type::single_player) {
		execute_chat_message(state, state.local_player_nation, str, dcon::nation_id{ }, state.local_player_id);
	}
	else {
		auto host = network::get_host_player(state);
		auto host_nation = state.world.mp_player_get_nation_from_player_nation(host);
		execute_chat_message(state, host_nation, str, dcon::nation_id{ }, host);
	}
	// if you are in control of the gamerules (either host, or single player), then save the current gamerules as your default preferances in file
	if(state.network_mode == sys::network_mode_type::single_player || state.network_mode == sys::network_mode_type::host) {

		state.save_gamerule_settings();
	}
	
}

void change_gamerule_setting(sys::state& state, dcon::nation_id source, dcon::gamerule_id gamerule, uint8_t new_setting) {

	command_data p{ command_type::change_game_rule_setting, state.local_player_id };
	auto data = change_gamerule_setting_data{ gamerule, new_setting };
	p << data;
	add_to_command_queue(state, p);

}

dcon::mp_player_id execute_notify_player_joins(sys::state& state, dcon::nation_id source, const sys::player_name& name, const sys::player_password_raw& password, bool needs_loading, dcon::nation_id player_nation) {
#ifndef NDEBUG
	state.console_log("client:receive:cmd | type:notify_player_joins | nation: " + std::to_string(player_nation.index()) + " | name: " + name.to_string());
#endif

	auto player_id = network::create_mp_player(state, name, password, !needs_loading, false, player_nation);

	if(needs_loading) {

		command::command_data cmd{ command::command_type::notify_player_is_loading, player_id };

		execute_command(state, cmd);
	}
	// make the chat message appear as if it is sent by the host
	auto host = network::get_host_player(state);
	auto host_nation = state.world.mp_player_get_nation_from_player_nation(host);
	ui::chat_message m{};
	m.source = host_nation;
	text::substitution_map sub{};
	text::add_to_substitution_map(sub, text::variable_type::playername, name.to_string_view());
	m.body = text::resolve_string_substitution(state, "chat_player_joins", sub);
	m.set_sender_name(state.world.mp_player_get_nickname(host));
	post_chat_message(state, m);

	/* Hotjoin */
	if(state.current_scene.game_in_progress)
		ai::remove_ai_data(state, player_nation);

	network::log_player_nations(state);
	// update UI immediately incase alot of long commands (ie loading save) is queued up
	state.game_state_updated.store(true, std::memory_order::release);
	return player_id;
}

void notify_player_leaves(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id leaving_player) {

	// only the host can use this command, so we use the leaving players ID here in the header
	command_data p{ command_type::notify_player_leaves, leaving_player };
	auto data = notify_leaves_data{ make_ai };
	p << data;
	add_to_command_queue(state, p);
}
bool can_notify_player_leaves(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id leaving_player) {
	return state.world.mp_player_is_valid(leaving_player);
}
void execute_notify_player_leaves(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id leaving_player) {
	assert(leaving_player);


	// send message and make it appear as if it is coming from the host
	auto host = network::get_host_player(state);
	auto host_nation = state.world.mp_player_get_nation_from_player_nation(host);
	ui::chat_message m{};
	m.source = host_nation;
	text::substitution_map sub{};
	const auto& nickname = state.world.mp_player_get_nickname(leaving_player);
	text::add_to_substitution_map(sub, text::variable_type::playername, nickname.to_string_view());
	m.body = text::resolve_string_substitution(state, "chat_player_leaves", sub);
	m.set_sender_name(state.world.mp_player_get_nickname(host));
	post_chat_message(state, m);

	network::delete_mp_player(state, leaving_player, make_ai);
}



void notify_player_timeout(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id disconnected_player) {

	// only the host can use this command, so we use the disconnected players ID here in the header
	command_data p{ command_type::notify_player_timeout, disconnected_player };
	auto data = notify_player_timeout_data{ make_ai };
	p << data;
	add_to_command_queue(state, p);
}
bool can_notify_player_timeout(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id disconnected_player) {
	return state.world.mp_player_is_valid(disconnected_player);
}
void execute_notify_player_timeout(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id disconnected_player) {
	assert(disconnected_player);

	if(disconnected_player == state.local_player_id) {
		auto discard = state.error_windows.try_push(ui::error_window{ text::produce_simple_string(state, "disconnected_message_header"), text::produce_simple_string(state, "disconnected_message_timed_out") });
		return;
	}

	// send message and make it appear as if it is coming from the host
	auto host = network::get_host_player(state);
	auto host_nation = state.world.mp_player_get_nation_from_player_nation(host);
	ui::chat_message m{};
	m.source = host_nation;
	text::substitution_map sub{};
	const auto& nickname = state.world.mp_player_get_nickname(disconnected_player);
	text::add_to_substitution_map(sub, text::variable_type::playername, nickname.to_string_view());
	m.body = text::resolve_string_substitution(state, "chat_timed_out", sub);
	m.set_sender_name(state.world.mp_player_get_nickname(host));
	post_chat_message(state, m);

	network::delete_mp_player(state, disconnected_player, make_ai);
}

void execute_change_ai_nation_state(sys::state& state, dcon::nation_id source, bool no_ai) 	{
	state.world.nation_set_is_player_controlled(source, no_ai);
}
void notify_player_ban(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id banned_player) {
	// only the host can use this command, so we use the banned players ID here in the header
	command_data p{ command_type::notify_player_ban, banned_player };
	auto data = notify_player_ban_data{ make_ai };
	p << data;
	add_to_command_queue(state, p);
}
bool can_notify_player_ban(sys::state& state, dcon::nation_id source, dcon::mp_player_id banned_player) {
	return state.world.mp_player_is_valid(banned_player);
}
void execute_notify_player_ban(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id banned_player) {
	assert(banned_player);
	// if we are banned, then display it to the user
	if(banned_player == state.local_player_id) {
		auto discard = state.error_windows.try_push(ui::error_window{ text::produce_simple_string(state, "disconnected_message_header"), text::produce_simple_string(state, "disconnected_message_banned") });
		return;
	}
	const auto& nickname = state.world.mp_player_get_nickname(banned_player);
	if(state.network_mode == sys::network_mode_type::host) {
		network::add_player_to_ban_list(state, banned_player);
	}

	// it should look like a message sent by the host

	auto host = network::get_host_player(state);
	auto host_nation = state.world.mp_player_get_nation_from_player_nation(host);
	ui::chat_message m{};
	m.source = host_nation;
	text::substitution_map sub{};
	text::add_to_substitution_map(sub, text::variable_type::playername, nickname.to_string_view());
	m.body = text::resolve_string_substitution(state, "chat_player_ban", sub);
	m.set_sender_name(state.world.mp_player_get_nickname(host));
	post_chat_message(state, m);

	network::delete_mp_player(state, banned_player, make_ai);
}

void notify_player_kick(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id kicked_player) {
	// only the host can use this command, so we use the kicked players ID here in the header
	command_data p{ command_type::notify_player_kick, kicked_player };
	auto data = notify_player_kick_data{ make_ai };
	p << data;
	add_to_command_queue(state, p);

}
bool can_notify_player_kick(sys::state& state, dcon::nation_id source, dcon::mp_player_id kicked_player) {
	return state.world.mp_player_is_valid(kicked_player);
}

void execute_notify_player_kick(sys::state& state, dcon::nation_id source, bool make_ai, dcon::mp_player_id kicked_player) {
	assert(kicked_player);
	// if we are kicked, then display it to the user
	if(kicked_player == state.local_player_id) {
		auto discard = state.error_windows.try_push(ui::error_window{ text::produce_simple_string(state, "disconnected_message_header"), text::produce_simple_string(state, "disconnected_message_kicked") });
		return;
	}
	const auto& nickname = state.world.mp_player_get_nickname(kicked_player);
	

	// send message and make it appear as if it is coming from the host
	auto host = network::get_host_player(state);
	auto host_nation = state.world.mp_player_get_nation_from_player_nation(host);

	ui::chat_message m{};
	m.source = host_nation;
	text::substitution_map sub{};

	text::add_to_substitution_map(sub, text::variable_type::playername, nickname.to_string_view());
	m.body = text::resolve_string_substitution(state, "chat_player_kick", sub);
	m.set_sender_name(state.world.mp_player_get_nickname(host));
	post_chat_message(state, m);

	network::delete_mp_player(state, kicked_player, make_ai);
}

void notify_player_picks_nation(sys::state& state, dcon::nation_id source, dcon::nation_id target) {

	command_data p{ command_type::notify_player_picks_nation, state.local_player_id };
	auto data = nation_pick_data{ target };
	p << data;
	add_to_command_queue(state, p);
}
bool can_notify_player_picks_nation(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::mp_player_id player) {
	// cannot pick a new nation whilst not in the looby
	if(!state.current_scene.is_lobby) {
		return false;
	}
	if(source == target || !player) //redundant
		return false;
	if(!bool(target)) //Invalid nation
		return false;
	// Should support co-op now.
	return true;
}
void execute_notify_player_picks_nation(sys::state& state, dcon::nation_id source, dcon::nation_id target, dcon::mp_player_id player) {
	network::switch_one_player(state, target, source, player);
}


bool can_notify_player_oos(sys::state& state, command_data& command) {
	auto player = command.header.player_id;
	// can't notify oos if there already are oos
	if(state.world.mp_player_get_is_oos(player)) {
		return false;
	}
	return true;
}

void notify_player_oos(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::notify_player_oos, state.local_player_id };
	add_to_command_queue(state, p);

#ifndef NDEBUG
	state.console_log("client:send:cmd | type=notify_player_oos | from nation: " + std::to_string(source.index()));
#endif
	// have this commented out as currently there isnt any way to determine the cause of an OOS when it happens
	//ui::chat_message m{};
	//m.source = source;
	//text::substitution_map sub{};
	//text::add_to_substitution_map(sub, text::variable_type::playername, state.network_state.nickname.to_string_view());
	//m.body = text::resolve_string_substitution(state, "chat_player_oos_source", sub);
	//m.set_sender_name(name.data);
	//post_chat_message(state, m);

	network::log_player_nations(state);
}
void execute_notify_player_oos(sys::state& state, dcon::nation_id source, dcon::mp_player_id oos_player) {
	state.actual_game_speed = 0; //pause host immediately
	state.debug_save_oos_dump();

	network::log_player_nations(state);

	assert(oos_player);
	state.world.mp_player_set_is_oos(oos_player, true);
	
	// send message and make it appear as if it is coming from the host
	auto host = network::get_host_player(state);
	auto host_nation = state.world.mp_player_get_nation_from_player_nation(host);
	ui::chat_message m{};
	m.source = host_nation;
	text::substitution_map sub{};
	const auto& nickname = state.world.mp_player_get_nickname(oos_player);
	text::add_to_substitution_map(sub, text::variable_type::playername, nickname.to_string_view());
	m.body = text::resolve_string_substitution(state, "chat_player_oos", sub);
	m.set_sender_name(state.world.mp_player_get_nickname(host));
	post_chat_message(state, m);

#ifndef NDEBUG
	state.console_log("client:rcv:cmd | type=notify_player_oos | from:" + std::to_string(source.index()));
#endif

}



void notify_oos_gamestate(sys::state& state, dcon::nation_id source) {
	uint32_t size = 0;
	auto mp_state_data = network::write_network_entire_mp_state(state, size);

	command_data p{ command_type::notify_oos_gamestate, state.local_player_id };
	auto data = notify_oos_gamestate_data{ };
	data.size = size;
	p << data;
	p.push_ptr(mp_state_data.get(), size);
	add_to_command_queue(state, p);
}

bool can_notify_oos_gamestate(sys::state& state, command_data& command) {
	const auto& payload = command.get_payload<notify_oos_gamestate_data>();
	// check that the data length is correct before reading from it
	if(!command.check_variable_size_payload<notify_oos_gamestate_data>(payload.size)) {
		assert(false && "Variable command with a inconsistent size recieved!");
		return false;
	}

	auto player = command.header.player_id;
	// can't send oos gamestate if the player sending it isn't oos
	return state.world.mp_player_get_is_oos(player);
}


void execute_notify_oos_gamestate(sys::state& state, dcon::nation_id source, dcon::mp_player_id oos_player, const uint8_t* oos_gamestate_data, uint32_t data_size) {

	std::unique_ptr<sys::state> oos_gamestate = std::make_unique<sys::state>();
	read_entire_mp_state(oos_gamestate_data, oos_gamestate_data + data_size, *oos_gamestate);
	network::dump_oos_report(state, *oos_gamestate);
	
}



void advance_tick(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::advance_tick, state.local_player_id };
	auto data = advance_tick_data{ };
	data.speed = state.actual_game_speed.load(std::memory_order::acquire);
	data.date = state.current_date;
	p << data;
	add_to_command_queue(state, p);
}

void execute_advance_tick(sys::state& state, dcon::nation_id source, sys::checksum_key& k, int32_t speed, sys::date new_date) {
	
	state.single_game_tick();

	// We check OOS AFTER the tick. That way the oos notification will be sent after without processing a whole other tick. If it was done before, it would process the tick first, then send the oos notification after.
	// TODO: Maybe add the OOS notification command here directly, and skip the queue so no actions are performed?
	if(state.network_mode == sys::network_mode_type::client) {
		if(!state.network_state.out_of_sync) {
			
			if(network::should_do_oos_check(state)) {
#ifndef NDEBUG
				state.console_log("client:checkingOOS | advance_tick | from:" + std::to_string(source.index()) +
					"|dt_local:" + state.current_date.to_string(state.start_date) + " | dt_incoming:" + new_date.to_string(state.start_date));
#endif
				sys::checksum_key current = state.get_mp_state_checksum();
				if(!current.is_equal(k)) {
#ifndef NDEBUG
					network::SHA512 sha512;
					std::string local = sha512.hash(current.to_char());
					std::string incoming = sha512.hash(k.to_char());
					state.console_log("client:desyncfound | Local checksum:" + local + " | " + "Incoming: " + incoming);
#endif
					state.network_state.out_of_sync = true;
					state.debug_save_oos_dump();
				}
			}
		}
		state.actual_game_speed = speed;
	}



	// Notify server that we're still here
	if(state.current_date.value % 7 == 0 && state.network_mode == sys::network_mode_type::client) {
		network_inactivity_ping(state, state.local_player_nation, state.current_date);
	}
}

void notify_save_loaded(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::notify_save_loaded, state.local_player_id };
	auto data = notify_save_loaded_data{ };
	data.target = dcon::nation_id{};
	p << data;
	add_to_command_queue(state, p);
}
void execute_notify_save_loaded(sys::state& state, dcon::nation_id source, sys::checksum_key& k) {
	state.session_host_checksum = k;
	/* Reset OOS state, and for host, advise new clients with a save stream so they can hotjoin!
	   Additionally we will clear the new client sending queue, since the state is no longer
	   "replayable" without heavy bandwidth costs */
	state.network_state.is_new_game = false;
	state.network_state.out_of_sync = false;
	state.network_state.reported_oos = false;
	// if client, enable save stream mode and set up other variables
	if(state.network_mode == sys::network_mode_type::client) {
		auto& payload = state.network_state.recv_buffer.get_payload<command::notify_save_loaded_data>();
		uint32_t save_size = payload.length;
		state.network_state.save_stream = true;
		assert(save_size > 0);
		if(save_size >= 32 * 1000 * 1000) { // 32 MB
			auto discard = state.error_windows.try_push(ui::error_window{ "Network Error", "Network client save stream too big: " + network::get_last_error_msg() });
			network::finish(state, false);
			return;
		}
		state.network_state.save_data.resize(static_cast<size_t>(save_size));
	}
}

void notify_reload(sys::state& state, dcon::nation_id source, sys::checksum_key& mp_state_checksum) {
	command_data p{ command_type::notify_reload, state.local_player_id };
	auto data = notify_reload_data{ mp_state_checksum };
	p << data;
	add_to_command_queue(state, p);

}
void execute_notify_reload(sys::state& state, dcon::nation_id source, sys::checksum_key& k) {
	state.session_host_checksum = k;
	// reload the save *locally* to ensure synch with the rest of the lobby. Primarily to update unsaved data.
	// this only happens when a new player joins, or when a manual resync is initiated, and only for the clients which are already "in sync". If a new or oos'd client needs a fresh save, it will be provided as a save stream elsewhere.
	state.network_state.is_new_game = false;
	state.network_state.out_of_sync = false;
	state.network_state.reported_oos = false;

	window::change_cursor(state, window::cursor_type::busy);
	{
		state.yield_ui_lock = true;
		std::unique_lock lock(state.ui_lock);

		std::vector<dcon::nation_id> no_ai_nations;
		for(const auto n : state.world.in_nation)
			if(state.world.nation_get_is_player_controlled(n))
				no_ai_nations.push_back(n);
		dcon::nation_id old_local_player_nation = state.local_player_nation;
		/* Save the buffer before we fill the unsaved data */
		size_t length = sizeof_save_section(state);
		auto save_buffer = std::unique_ptr<uint8_t[]>(new uint8_t[length]);
		sys::write_save_section(save_buffer.get(), state);
		state.local_player_nation = dcon::nation_id{ };
		/* Then reload as if we loaded the save data */
		state.reset_state();
		sys::read_save_section(save_buffer.get(), save_buffer.get() + length, state);
		network::set_no_ai_nations_after_reload(state, no_ai_nations, old_local_player_nation);
		state.fill_unsaved_data();

		state.yield_ui_lock = false;
		lock.unlock();
		state.ui_lock_cv.notify_one();
	}
	window::change_cursor(state, window::cursor_type::normal);

	assert(state.world.nation_get_is_player_controlled(state.local_player_nation));
	assert(state.session_host_checksum.is_equal(state.get_mp_state_checksum()));
	command::notify_player_fully_loaded(state, state.local_player_nation); // notify we are done reloading

#ifndef NDEBUG
	network::SHA512 sha512;
	std::string encodedchecksum = sha512.hash(state.session_host_checksum.to_char());
	state.console_log("client:exec:cmd | type=notify_reload from:" + std::to_string(source.index()) + "| checksum: " + encodedchecksum);
#endif

	network::log_player_nations(state);
}

bool can_notify_start_game(sys::state& state, dcon::nation_id source) {
	if(state.network_mode == sys::network_mode_type::single_player) {
		return bool(state.local_player_nation);
	}
	else if(state.network_mode == sys::network_mode_type::client) {
		return true;
	}
	else {
		if(network::any_player_on_invalid_nation(state)) {
			return false;
		}
		return !network::check_any_players_loading(state);
	}
}

void execute_notify_start_game(sys::state& state, dcon::nation_id source) {
	assert(state.world.nation_get_is_player_controlled(state.local_player_nation));
	state.selected_armies.clear();
	state.selected_navies.clear();
	for(auto& v : state.ctrl_armies) v.clear();
	for(auto& v : state.ctrl_navies) v.clear();
	/* And clear the save stuff */
	state.network_state.current_save_buffer.reset();
	state.network_state.current_save_length = 0;
	state.network_state.last_save_checksum = sys::checksum_key{ };
	/* Clear AI data */
	for(const auto n : state.world.in_nation) {
		if(state.world.nation_get_is_player_controlled(n)) {
			ai::remove_ai_data(state, n);
			// give back units if puppet becomes player controlled
			if(bool(state.world.nation_get_overlord_as_subject(n)) && state.world.nation_get_overlord_commanding_units(n)) {
				military::give_back_units(state, n);
			}
		}
	}
	{
		state.yield_ui_lock = true;
		std::unique_lock lock(state.ui_lock);

		game_scene::switch_scene(state, game_scene::scene_id::in_game_basic);
		state.set_selected_province(dcon::province_id{});
		state.map_state.unhandled_province_selection = true;

		auto cache = sys::player_data{};
		cache.nation = state.local_player_nation;
		state.player_data_cache.push_back(cache);

		state.yield_ui_lock = false;
		lock.unlock();
		state.ui_lock_cv.notify_one();
	}
}

void notify_start_game(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::notify_start_game, state.local_player_id };
	add_to_command_queue(state, p);
}

void notify_player_is_loading(sys::state& state, dcon::nation_id source, dcon::mp_player_id loading_player) {

	command_data p{ command_type::notify_player_is_loading, loading_player };
	add_to_command_queue(state, p);
}

void execute_notify_player_is_loading(sys::state& state, dcon::nation_id source, dcon::mp_player_id loading_player) {
	assert(loading_player);
	network::mp_player_set_fully_loaded(state, loading_player, false);
	// update UI immediately incase alot of long commands (ie loading save) is queued up
	state.game_state_updated.store(true, std::memory_order::release);

	
}



void notify_player_fully_loaded(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::notify_player_fully_loaded, state.local_player_id };
	add_to_command_queue(state, p);
}

void execute_notify_player_fully_loaded(sys::state& state, dcon::nation_id source, dcon::mp_player_id loaded_player) {
	assert(loaded_player);
	network::mp_player_set_fully_loaded(state, loaded_player, true);
	state.world.mp_player_set_is_oos(loaded_player, false);
	// update UI immediately incase alot of long commands (ie loading save) is queued up
	state.game_state_updated.store(true, std::memory_order::release);
	
}

bool can_notify_stop_game(sys::state& state, dcon::nation_id source) {
	if(state.current_scene.is_lobby) {
		return false;
	}
	if(state.network_mode == sys::network_mode_type::single_player) {
		return true;
	} else if(state.network_mode == sys::network_mode_type::client) {
		return true;
	} else {
		return !network::check_any_players_loading(state);
	}
}

void execute_notify_stop_game(sys::state& state, dcon::nation_id source) {
	{
		state.yield_ui_lock = true;
		std::unique_lock lock(state.ui_lock);

		game_scene::switch_scene(state, game_scene::scene_id::pick_nation);
		state.set_selected_province(dcon::province_id{});
		state.map_state.unhandled_province_selection = true;

		state.yield_ui_lock = false;
		lock.unlock();
		state.ui_lock_cv.notify_one();
	}
}

void notify_stop_game(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::notify_stop_game, state.local_player_id };
	add_to_command_queue(state, p);
}

void execute_notify_pause_game(sys::state& state, dcon::nation_id source) {
	state.actual_game_speed = 0;
}

void notify_pause_game(sys::state& state, dcon::nation_id source) {

	command_data p{ command_type::notify_pause_game, state.local_player_id };
	add_to_command_queue(state, p);

}

void network_inactivity_ping(sys::state& state, dcon::nation_id source, sys::date date) {

	command_data p{ command_type::network_inactivity_ping, state.local_player_id };
	auto data = advance_tick_data{ };
	data.date = date;
	p << data;
	add_to_command_queue(state, p);
}
void execute_network_inactivity_ping(sys::state& state, dcon::nation_id source, sys::date date, dcon::mp_player_id player) {
	// Update last seen of the client
	if(state.network_mode == sys::network_mode_type::host) {
		for(auto& client : state.network_state.clients) {
			if(client.player_id == player) {
				client.last_seen = date;
			}
		}
	}
	return;
}

// host only. Sends commands to other clients to handle the resync
void resync_lobby(sys::state& state, dcon::nation_id source) {
	command_data p{ command_type::resync_lobby, state.local_player_id };
	add_to_command_queue(state, p);
}


void execute_resync_lobby(sys::state& state, dcon::nation_id source) {
	network::full_reset_after_oos(state);
	state.ui_state.recently_pressed_resync = false;
}

void execute_notify_mp_data(sys::state& state, const notify_mp_data_data& data) {
	// size boundary is checked in can_notify_mp_data so we can safely do this
	sys::read_mp_data(data.mp_data(), data.mp_data() + data.data_len, state);
	// update UI immediately incase alot of long commands (ie loading save) is queued up
	state.game_state_updated.store(true, std::memory_order::release);
}

bool can_notify_mp_data(sys::state& state, command_data& command) {

		auto& payload = command.get_payload<notify_mp_data_data>();

	// check that the data length is correct before reading from it
	if(!command.check_variable_size_payload<notify_mp_data_data>(payload.data_len)) {
		assert(false && "Variable command with a inconsistent size recieved!");
		return false;
	}

	return true;
}








bool notify_oos_gamestate_is_host_receive_command(const sys::state& state) {
	return state.host_settings.oos_debug_mode;
}





bool can_perform_command(sys::state& state, command_data& c) {
	auto source = state.world.mp_player_get_nation_from_player_nation(c.header.player_id);
	switch(c.header.type) {
	case command_type::invalid:
		assert(false && "Invalid command received"); // invalid command
		break;
	case command_type::change_nat_focus:
	{
		auto& data = c.get_payload<command::national_focus_data>();
		return can_set_national_focus(state, source, data.target_state, data.focus);
	}

	case command_type::start_research:
	{
		auto& data = c.get_payload<command::start_research_data>();
		return can_start_research(state, source, data.tech);
	}

	case command_type::make_leader:
	{
		auto& data = c.get_payload<command::make_leader_data>();
		return can_make_leader(state, source, data.is_general);
	}

	case command_type::set_factory_type_priority:
	{
		auto& data = c.get_payload<command::set_factory_priority_data>();
		return can_set_factory_type_priority(state, source, data.factory, data.value);
	}

	case command_type::begin_province_building_construction:
	{
		auto& data = c.get_payload<command::province_building_data>();
		return can_begin_province_building_construction(state, source, data.location,
				data.type);
	}

	case command_type::war_subsidies:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_give_war_subsidies(state, source, data.target);
	}

	case command_type::cancel_war_subsidies:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_cancel_war_subsidies(state, source, data.target);
	}

	case command_type::increase_relations:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_increase_relations(state, source, data.target);
	}

	case command_type::decrease_relations:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_decrease_relations(state, source, data.target);
	}

	case command_type::begin_factory_building_construction:
	{
		auto& data = c.get_payload<command::factory_building_data>();
		return can_begin_factory_building_construction(state, source, data.location,
				data.type, data.is_upgrade, data.refit_target);
	}

	case command_type::begin_naval_unit_construction:
	{
		auto& data = c.get_payload<command::naval_unit_construction_data>();
		return can_start_naval_unit_construction(state, source, data.location,
				data.type, data.template_province);
	}

	case command_type::cancel_naval_unit_construction:
	{
		auto& data = c.get_payload<command::naval_unit_construction_data>();
		return can_cancel_naval_unit_construction(state, source, data.location,
				data.type);
	}

	case command_type::begin_land_unit_construction:
	{
		auto& data = c.get_payload<command::land_unit_construction_data>();
		return can_start_land_unit_construction(state, source, data.location,
				data.pop_culture, data.type, data.template_province);
	}

	case command_type::cancel_land_unit_construction:
	{
		auto& data = c.get_payload<command::land_unit_construction_data>();
		return can_cancel_land_unit_construction(state, source, data.location,
				data.pop_culture, data.type);
	}

	case command_type::delete_factory:
	{
		auto& data = c.get_payload<command::factory_data>();
		return can_delete_factory(state, source, data.id);
	}

	case command_type::change_factory_settings:
	{
		auto& data = c.get_payload<command::factory_data>();
		return can_change_factory_settings(state, source, data.id, data.priority, data.subsidize);
	}

	case command_type::make_vassal:
	{
		auto& data = c.get_payload<command::tag_target_data>();
		return can_make_vassal(state, source, data.ident);
	}

	case command_type::release_and_play_nation:
	{
		auto& data = c.get_payload<command::tag_target_data>();
		return can_release_and_play_as(state, source, data.ident, c.header.player_id);
	}

	case command_type::change_budget:
	{
		auto& data = c.get_payload<command::budget_settings_data>();
		return can_change_budget_settings(state, source, data);
	}

	case command_type::start_election:
	{
		return can_start_election(state, source);
	}

	case command_type::change_influence_priority:
	{
		auto& data = c.get_payload<command::influence_priority_data>();
		return can_change_influence_priority(state, source, data.influence_target,
				data.priority);
	}

	case command_type::expel_advisors:
	{
		auto& data = c.get_payload<command::influence_action_data>();
		return can_expel_advisors(state, source, data.influence_target, data.gp_target);
	}

	case command_type::ban_embassy:
	{
		auto& data = c.get_payload<command::influence_action_data>();
		return can_ban_embassy(state, source, data.influence_target, data.gp_target);
	}

	case command_type::discredit_advisors:
	{
		auto& data = c.get_payload<command::influence_action_data>();
		return can_discredit_advisors(state, source, data.influence_target, data.gp_target);
	}

	case command_type::decrease_opinion:
	{
		auto& data = c.get_payload<command::influence_action_data>();
		return can_decrease_opinion(state, source, data.influence_target, data.gp_target);
	}

	case command_type::remove_from_sphere:
	{
		auto& data = c.get_payload<command::influence_action_data>();
		return can_remove_from_sphere(state, source, data.influence_target, data.gp_target);
	}

	case command_type::increase_opinion:
	{
		auto& data = c.get_payload<command::influence_action_data>();
		return can_increase_opinion(state, source, data.influence_target);
	}

	case command_type::add_to_sphere:
	{
		auto& data = c.get_payload<command::influence_action_data>();
		return can_add_to_sphere(state, source, data.influence_target);
	}

	case command_type::upgrade_colony_to_state:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_upgrade_colony_to_state(state, source, state.world.province_get_state_membership(data.prov));
	}

	case command_type::invest_in_colony:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_invest_in_colony(state, source, data.prov);
	}

	case command_type::abandon_colony:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_abandon_colony(state, source, data.prov);
	}

	case command_type::finish_colonization:
	{
		auto& data = c.get_payload<command::generic_state_definition_data>();
		return can_finish_colonization(state, source, data.state_def);
	}

	case command_type::intervene_in_war:
	{
		auto& data = c.get_payload<command::war_target_data>();
		return can_intervene_in_war(state, source, data.war, data.for_attacker);
	}

	case command_type::suppress_movement:
	{
		return true; //can_suppress_movement(state, c.source, c.data.movement.iopt);
	}

	case command_type::civilize_nation:
	{
		return can_civilize_nation(state, source);
	}

	case command_type::appoint_ruling_party:
	{
		auto& data = c.get_payload<command::political_party_data>();
		return can_appoint_ruling_party(state, source, data.p);
	}

	case command_type::change_issue_option:
	{
		auto& data = c.get_payload<command::issue_selection_data>();
		return can_enact_issue(state, source, data.r);
	}

	case command_type::change_reform_option:
	{
		auto& data = c.get_payload<command::reform_selection_data>();
		return can_enact_reform(state, source, data.r);
	}

	case command_type::become_interested_in_crisis:
	{
		return can_become_interested_in_crisis(state, source);
	}

	case command_type::take_sides_in_crisis:
	{
		auto& data = c.get_payload<command::crisis_join_data>();
		return can_take_sides_in_crisis(state, source, data.join_attackers);
	}

	case command_type::change_stockpile_settings:
	{
		auto& data = c.get_payload<command::stockpile_settings_data>();
		return can_change_stockpile_settings(state, source, data.c, data.amount,
				data.draw_on_stockpiles);
	}

	case command_type::take_decision:
	{
		auto& data = c.get_payload<command::decision_data>();
		return can_take_decision(state, source, data.d);
	}

	case command_type::make_n_event_choice:
	{
		auto& data = c.get_payload<command::pending_human_n_event_data>();
		return can_make_event_choice(state, source, data);
	}
	case command_type::make_f_n_event_choice:
	{
		auto& data = c.get_payload<command::pending_human_f_n_event_data>();
		return can_make_event_choice(state, source, data);
	}
	case command_type::make_p_event_choice:
	{
		auto& data = c.get_payload<command::pending_human_p_event_data>();
		return can_make_event_choice(state, source, data);
	}

	case command_type::make_f_p_event_choice:
	{
		auto& data = c.get_payload<command::pending_human_f_p_event_data>();
		return can_make_event_choice(state, source, data);
	}

	case command_type::cancel_cb_fabrication:
	{
		return can_cancel_cb_fabrication(state, source);
	}

	case command_type::fabricate_cb:
	{
		auto& data = c.get_payload<command::cb_fabrication_data>();
		return can_fabricate_cb(state, source, data.target, data.type);
	}

	case command_type::ask_for_military_access:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_ask_for_access(state, source, data.target);
	}

	case command_type::ask_for_alliance:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_ask_for_alliance(state, source, data.target);
	}

	case command_type::ask_for_free_trade_agreement:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_ask_for_free_trade_agreement(state, source, data.target);
	}

	case command_type::switch_embargo_status:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_switch_embargo_status(state, source, data.target);
	}

	case command_type::revoke_trade_rights:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_revoke_trade_rights(state, source, data.target);
	}

	case command_type::call_to_arms:
	{
		auto& data = c.get_payload<command::call_to_arms_data>();
		return can_call_to_arms(state, source, data.target, data.war, false, data.automatic_call);
	}

	case command_type::respond_to_diplomatic_message:
	{
		return true; //can_respond_to_diplomatic_message(state, c.source, c.data.message.from, c.data.message.type, c.data.message.accept);
	}

	case command_type::cancel_military_access:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_cancel_military_access(state, source, data.target);
	}

	case command_type::cancel_alliance:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_cancel_alliance(state, source, data.target);
	}

	case command_type::cancel_given_military_access:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_cancel_given_military_access(state, source, data.target);
	}

	case command_type::declare_war:
	{
		auto& data = c.get_payload<command::new_war_data>();
		return can_declare_war(state, source, data.target, data.primary_cb, data.cb_state,
				data.cb_tag, data.cb_secondary_nation);
	}

	case command_type::add_war_goal:
	{
		auto& data = c.get_payload<command::new_war_goal_data>();
		return can_add_war_goal(state, source, data.war, data.target, data.cb_type,
				data.cb_state, data.cb_tag, data.cb_secondary_nation);
	}

	case command_type::start_peace_offer:
	{
		auto& data = c.get_payload<command::new_offer_data>();
		return can_start_peace_offer(state, source, data.target, data.war,
				data.is_concession);
	}

	case command_type::add_peace_offer_term:
	{
		auto& data = c.get_payload<command::offer_wargoal_data>();
		return can_add_to_peace_offer(state, source, data.wg);
	}

	case command_type::send_peace_offer:
	{
		return can_send_peace_offer(state, source);
	}

	case command_type::move_army:
	{
		auto& data = c.get_payload<command::army_movement_data>();
		return can_move_army(state, source, data.a, data.dest, data.reset).size() != 0;
	}

	case command_type::move_navy:
	{
		auto& data = c.get_payload<command::navy_movement_data>();
		return can_move_navy(state, source, data.n, data.dest, data.reset).size() != 0;
	}

	case command_type::embark_army:
	{
		auto& data = c.get_payload<command::army_movement_data>();
		return can_embark_army(state, source, data.a);
	}

	case command_type::merge_armies:
	{
		auto& data = c.get_payload<command::merge_army_data>();
		return can_merge_armies(state, source, data.a, data.b);
	}

	case command_type::merge_navies:
	{
		auto& data = c.get_payload<command::merge_navy_data>();
		return can_merge_navies(state, source, data.a, data.b);
	}

	case command_type::split_army:
	{
		auto& data = c.get_payload<command::army_movement_data>();
		return can_split_army(state, source, data.a);
	}

	case command_type::split_navy:
	{
		auto& data = c.get_payload<command::navy_movement_data>();
		return can_split_navy(state, source, data.n);
	}

	case command_type::change_unit_type:
	{
		auto& data = c.get_payload<command::change_unit_type_data>();
		return can_change_unit_type(state, source, data.regs, data.ships, data.new_type);
	}

	case command_type::delete_army:
	{
		auto& data = c.get_payload<command::army_movement_data>();
		return can_delete_army(state, source, data.a);
	}

	case command_type::delete_navy:
	{
		auto& data = c.get_payload<command::navy_movement_data>();
		return can_delete_navy(state, source, data.n);
	}

	case command_type::designate_split_regiments:
	{
		return true; //can_mark_regiments_to_split(state, c.source, c.data.split_regiments.regs);
	}

	case command_type::designate_split_ships:
	{
		return true; //can_mark_ships_to_split(state, c.source, c.data.split_ships.ships);
	}

	case command_type::naval_retreat:
	{
		auto& data = c.get_payload<command::retreat_from_naval_battle_data>();
		return can_retreat_from_naval_battle(state, source, data.navy, military::retreat_type::manual, data.dest).size() != 0;
	}

	case command_type::land_retreat:
	{
		auto& data = c.get_payload<command::land_battle_data>();
		return can_retreat_from_land_battle(state, source, data.b);
	}

	case command_type::start_crisis_peace_offer:
	{
		auto& data = c.get_payload<command::new_offer_data>();
		return can_start_crisis_peace_offer(state, source, data.is_concession);
	}

	case command_type::invite_to_crisis:
	{
		return true; // can_invite_to_crisis(state, c.source, c.data.crisis_invitation);
	}

	case command_type::add_wargoal_to_crisis_offer:
	{
		return true; // can_add_to_crisis_peace_offer(state, c.source, c.data.crisis_invitation);
	}

	case command_type::crisis_add_wargoal:
	{
		auto& data = c.get_payload<command::new_war_goal_data>();
		return crisis_can_add_wargoal(state, source, sys::full_wg{
		source, // added_by;
		data.target, // target_nation;
		data.cb_secondary_nation, //  secondary_nation;
		data.cb_tag, // wg_tag;
		data.cb_state, // state;
		data.cb_type // cb
		});

	}

	case command_type::send_crisis_peace_offer:
	{
		return can_send_crisis_peace_offer(state, source);
	}

	case command_type::change_admiral:
	{
		auto& data = c.get_payload<command::new_admiral_data>();
		return can_change_admiral(state, source, data.a, data.l);
	}

	case command_type::change_general:
	{
		auto& data = c.get_payload<command::new_general_data>();
		return can_change_general(state, source, data.a, data.l);
	}

	case command_type::toggle_mobilization:
	{
		return true; //can_toggle_mobilization(state, c.source);
	}

	case command_type::give_military_access:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_give_military_access(state, source, data.target);
	}

	case command_type::set_rally_point:
	{
		return true; //can_set_rally_point(state, c.source, c.data.rally_point.location, c.data.rally_point.naval, c.data.rally_point.enable);
	}

	case command_type::save_game:
	{
		return can_save_game(state, c);
	}

	case command_type::cancel_factory_building_construction:
	{
		auto& data = c.get_payload<command::factory_building_data>();
		return can_cancel_factory_building_construction(state, source, data.location, data.type);
	}

	case command_type::disband_undermanned:
	{
		auto& data = c.get_payload<command::army_movement_data>();
		return can_disband_undermanned_regiments(state, source, data.a);
	}

	case command_type::even_split_army:
	{
		auto& data = c.get_payload<command::army_movement_data>();
		return can_evenly_split_army(state, source, data.a);
	}

	case command_type::even_split_navy:
	{
		auto& data = c.get_payload<command::navy_movement_data>();
		return can_evenly_split_navy(state, source, data.n);
	}

	case command_type::toggle_hunt_rebels:
	{
		return true; //can_toggle_rebel_hunting(state, c.source, c.data.army_movement.a);
	}

	case command_type::toggle_select_province:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_toggle_select_province(state, source, data.prov);
	}

	case command_type::toggle_immigrator_province:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_toggle_immigrator_province(state, source, data.prov);
	}

	case command_type::state_transfer:
	{
		auto& data = c.get_payload<command::state_transfer_data>();
		return can_state_transfer(state, source, data.target, data.state);
	}

	case command_type::enable_debt:
	{
		return true;
	}

	case command_type::move_capital:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_move_capital(state, source, data.prov);
	}

	case command_type::toggle_local_administration:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_toggle_local_administration(state, source, data.prov);
	}

	case command_type::take_province:
	{
		auto& data = c.get_payload<command::generic_location_data>();
		return can_take_province(state, source, data.prov);
	}

	case command_type::toggle_unit_ai_control:
	{
		return true;
	}
	case command_type::toggle_mobilized_is_ai_controlled:
	{
		return true;
	}
	case command_type::toggle_interested_in_alliance:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_toggle_interested_in_alliance(state, source, data.target);
	}
	case command_type::toggle_production_directive:
	{
		return true;
	}
	case command_type::pbutton_script:
	{
		auto& data = c.get_payload<command::pbutton_data>();
		return can_use_province_button(state, source, data.button, data.id);
	}
	case command_type::nbutton_script:
	{
		auto& data = c.get_payload<command::nbutton_data>();
		return can_use_nation_button(state, source, data.button, data.id);
	}

		// common mp commands
	case command_type::chat_message:
	{
		//auto& data = c.get_payload<command::chat_message_data>();
		{
			/*size_t count = 0;
			for(count = 0; count < sizeof(data.body); count++)
				if(data.body[count] == '\0')

					std::string_view sv(data.body, data.body + count);*/
			return can_chat_message(state, c);

		}
	}
	case command_type::notify_player_ban:
	{
		auto& data = c.get_payload<command::notify_player_ban_data>();
		return can_notify_player_ban(state, source, c.header.player_id);
	}

	case command_type::notify_player_kick:
	{
		auto& data = c.get_payload<command::notify_player_kick_data>();
		return can_notify_player_kick(state, source, c.header.player_id);
	}

	case command_type::notify_player_joins:
	{
		auto& data = c.get_payload<command::notify_joins_data>();
		return can_notify_player_joins(state, source, data.player_name, data.needs_loading, data.player_nation);
	}

	case command_type::notify_player_leaves:
	{
		auto& data = c.get_payload<command::notify_leaves_data>();
		return can_notify_player_leaves(state, source, data.make_ai, c.header.player_id);
	}

	case command_type::notify_player_picks_nation:
	{
		auto& data = c.get_payload<command::nation_pick_data>();
		return can_notify_player_picks_nation(state, source, data.target, c.header.player_id);
	}

	case command_type::notify_player_oos:
	{
		return can_notify_player_oos(state, c);
	}
	case command_type::notify_oos_gamestate:
	{
		return can_notify_oos_gamestate(state, c);
	}
	case command_type::advance_tick:
	{
		return true; //return can_advance_tick(state, c.source, c.data.advance_tick.checksum, c.data.advance_tick.speed);
	}
	case command_type::notify_save_loaded:
	{
		return true; //return can_notify_save_loaded(state, c.source, c.data.notify_save_loaded.seed, c.data.notify_save_loaded.checksum);
	}
	case command_type::notify_reload:
	{
		return true;
	}
	case command_type::notify_start_game:
	{
		return can_notify_start_game(state, source);
	}
	case command_type::notify_stop_game:
	{
		return can_notify_stop_game(state, source);
	}
	case command_type::notify_pause_game:
	{
		return true; //return can_notify_pause_game(state, c.source);
	}
	case command_type::release_subject:
	{
		auto& data = c.get_payload<command::diplo_action_data>();
		return can_release_subject(state, source, data.target);
	}
	case command_type::network_inactivity_ping:
	{
		return true;
	}
	case command_type::console_command:
	{
		return true;
	}
	case command_type::grant_province:
	{
		return false;
	}
	case command_type::network_populate:
	{
		return false;
	}
	case command_type::notify_player_fully_loaded:
	{
		return true;
	}
	case command_type::notify_player_is_loading:
	{
		return true;
	}
	case command_type::change_ai_nation_state:
	{
		return true;
	}
	case command_type::stop_army_movement:
	{
		auto& data = c.get_payload<command::stop_army_movement_data>();
		return can_stop_army_movement(state, source, data.army);
	}
	case command_type::stop_navy_movement:
	{
		auto& data = c.get_payload<command::stop_navy_movement_data>();
		return can_stop_navy_movement(state, source, data.navy);
	}
	case command_type::command_units:
	{
		auto& data = c.get_payload<command::command_units_data>();
		return can_command_units(state, source, data.target);
	}
	case command_type::give_back_units:
	{
		auto& data = c.get_payload<command::command_units_data>();
		return can_give_back_units(state, source, data.target);
	}
	case command_type::change_game_rule_setting:
	{
		auto& data = c.get_payload<command::change_gamerule_setting_data>();
		return can_change_gamerule_setting(state, source, data.gamerule, data.setting);
	}
	case command_type::resync_lobby:
	{
		return true;
	}
	case command_type::notify_mp_data:
	{
		return can_notify_mp_data(state, c);
	}
	case command_type::notify_player_timeout:
	{
		const auto& data = c.get_payload<notify_player_timeout_data>();
		return can_notify_player_timeout(state, source, data.make_ai, c.header.player_id);
	}
	}
	return false;
}

bool execute_command(sys::state& state, command_data& c) {
	if(!can_perform_command(state, c))
		return false;
	state.tick_start_counter.fetch_add(1, std::memory_order::seq_cst);
	auto source_nation = state.world.mp_player_get_nation_from_player_nation(c.header.player_id);
	switch(c.header.type) {
	case command_type::invalid:
	{
		assert(false && "Invalid command received"); // invalid command
		break;
	}
	case command_type::change_nat_focus:
	{
		auto& data = c.get_payload<command::national_focus_data>();
		execute_set_national_focus(state, source_nation, data.target_state, data.focus);
		break;
	}
	case command_type::start_research:
	{
		auto& data = c.get_payload<start_research_data>();
		execute_start_research(state, source_nation, data.tech);
		break;
	}
	case command_type::make_leader:
	{
		auto& data = c.get_payload<make_leader_data>();
		execute_make_leader(state, source_nation, data.is_general);
		break;
	}
	case command_type::set_factory_type_priority:
	{
		auto& data = c.get_payload<set_factory_priority_data>();
		execute_set_factory_type_priority(state, source_nation, data.factory, data.value);
		break;
	}
	case command_type::begin_province_building_construction:
	{
		auto& data = c.get_payload<province_building_data>();
		execute_begin_province_building_construction(state, source_nation, data.location,
				data.type);
		break;
	}
	case command_type::war_subsidies:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_give_war_subsidies(state, source_nation, data.target);
		break;
	}
	case command_type::cancel_war_subsidies:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_cancel_war_subsidies(state, source_nation, data.target);
		break;
	}
	case command_type::increase_relations:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_increase_relations(state, source_nation, data.target);
		break;
	}
	case command_type::decrease_relations:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_decrease_relations(state, source_nation, data.target);
		break;
	}
	case command_type::begin_factory_building_construction:
	{
		auto& data = c.get_payload<factory_building_data>();
		execute_begin_factory_building_construction(state, source_nation, data.location,
				data.type, data.is_upgrade, data.refit_target);
		break;
	}
	case command_type::begin_naval_unit_construction:
	{
		auto& data = c.get_payload<naval_unit_construction_data>();
		execute_start_naval_unit_construction(state, source_nation, data.location,
				data.type, data.template_province);
		break;
	}
	case command_type::cancel_naval_unit_construction:
	{
		auto& data = c.get_payload<naval_unit_construction_data>();
		execute_cancel_naval_unit_construction(state, source_nation, data.location,
				data.type);
		break;
	}
	case command_type::begin_land_unit_construction:
	{
		auto& data = c.get_payload<land_unit_construction_data>();
		execute_start_land_unit_construction(state, source_nation, data.location,
				data.pop_culture, data.type, data.template_province);
		break;
	}
	case command_type::cancel_land_unit_construction:
	{
		auto& data = c.get_payload<land_unit_construction_data>();
		execute_cancel_land_unit_construction(state, source_nation, data.location,
				data.pop_culture, data.type);
		break;
	}
	case command_type::delete_factory:
	{
		auto& data = c.get_payload<factory_data>();
		execute_delete_factory(state, source_nation, data.id);
		break;
	}
	case command_type::change_factory_settings:
	{
		auto& data = c.get_payload<factory_data>();
		execute_change_factory_settings(state, source_nation, data.id, data.priority,
				data.subsidize);
		break;
	}
	case command_type::make_vassal:
	{
		auto& data = c.get_payload<tag_target_data>();
		execute_make_vassal(state, source_nation, data.ident);
		break;
	}
	case command_type::release_and_play_nation:
	{
		auto& data = c.get_payload<tag_target_data>();
		execute_release_and_play_as(state, source_nation, data.ident, c.header.player_id);
		break;
	}
	case command_type::change_budget:
	{
		auto& data = c.get_payload<budget_settings_data>();
		execute_change_budget_settings(state, source_nation, data);
		break;
	}
	case command_type::start_election:;
	{
		execute_start_election(state, source_nation);
		break;
	}
	case command_type::change_influence_priority:
	{
		auto& data = c.get_payload<influence_priority_data>();
		execute_change_influence_priority(state, source_nation, data.influence_target,
				data.priority);
		break;
	}
	case command_type::expel_advisors:
	{
		auto& data = c.get_payload<influence_action_data>();
		execute_expel_advisors(state, source_nation, data.influence_target, data.gp_target);
		break;
	}
	case command_type::ban_embassy:
	{
		auto& data = c.get_payload<influence_action_data>();
		execute_ban_embassy(state, source_nation, data.influence_target, data.gp_target);
		break;
	}
	case command_type::discredit_advisors:
	{
		auto& data = c.get_payload<influence_action_data>();
		execute_discredit_advisors(state, source_nation, data.influence_target, data.gp_target);
		break;
	}
	case command_type::decrease_opinion:
	{
		auto& data = c.get_payload<influence_action_data>();
		execute_decrease_opinion(state, source_nation, data.influence_target, data.gp_target);
		break;
	}
	case command_type::remove_from_sphere:
	{
		auto& data = c.get_payload<influence_action_data>();
		execute_remove_from_sphere(state, source_nation, data.influence_target, data.gp_target);
		break;
	}
	case command_type::increase_opinion:
	{
		auto& data = c.get_payload<influence_action_data>();
		execute_increase_opinion(state, source_nation, data.influence_target);
		break;
	}
	case command_type::add_to_sphere:
	{
		auto& data = c.get_payload<influence_action_data>();
		execute_add_to_sphere(state, source_nation, data.influence_target);
		break;
	}
	case command_type::upgrade_colony_to_state:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_upgrade_colony_to_state(state, source_nation, state.world.province_get_state_membership(data.prov));
		break;
	}
	case command_type::invest_in_colony:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_invest_in_colony(state, source_nation, data.prov);
		break;
	}
	case command_type::abandon_colony:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_abandon_colony(state, source_nation, data.prov);
		break;
	}
	case command_type::finish_colonization:
	{
		auto& data = c.get_payload<generic_state_definition_data>();
		execute_finish_colonization(state, source_nation, data.state_def);
		break;
	}
	case command_type::intervene_in_war:
	{
		auto& data = c.get_payload<war_target_data>();
		execute_intervene_in_war(state, source_nation, data.war, data.for_attacker);
		break;
	}
	case command_type::suppress_movement:
	{
		auto& data = c.get_payload<movement_data>();
		execute_suppress_movement(state, source_nation, data.iopt, data.tag);
		break;
	}
	case command_type::civilize_nation:
	{
		execute_civilize_nation(state, source_nation);
		break;
	}
	case command_type::appoint_ruling_party:
	{
		auto& data = c.get_payload<political_party_data>();
		execute_appoint_ruling_party(state, source_nation, data.p);
		break;
	}
	case command_type::change_issue_option:
	{
		auto& data = c.get_payload<issue_selection_data>();
		execute_enact_issue(state, source_nation, data.r);
		break;
	}
	case command_type::change_reform_option:
	{
		auto& data = c.get_payload<reform_selection_data>();
		execute_enact_reform(state, source_nation, data.r);
		break;
	}
	case command_type::become_interested_in_crisis:
	{
		execute_become_interested_in_crisis(state, source_nation);
		break;
	}
	case command_type::take_sides_in_crisis:
	{
		auto& data = c.get_payload<crisis_join_data>();
		execute_take_sides_in_crisis(state, source_nation, data.join_attackers);
		break;
	}
	case command_type::change_stockpile_settings:
	{
		auto& data = c.get_payload<stockpile_settings_data>();
		execute_change_stockpile_settings(state, source_nation, data.c, data.amount,
				data.draw_on_stockpiles);
		break;
	}
	case command_type::take_decision:
	{
		auto& data = c.get_payload<decision_data>();
		execute_take_decision(state, source_nation, data.d);
		break;
	}
	case command_type::make_n_event_choice:
	{
		auto& data = c.get_payload<pending_human_n_event_data>();
		execute_make_event_choice(state, source_nation, data);
		break;
	}
	case command_type::make_f_n_event_choice:
	{
		auto& data = c.get_payload<pending_human_f_n_event_data>();
		execute_make_event_choice(state, source_nation, data);
		break;
	}
	case command_type::make_p_event_choice:
	{
		auto& data = c.get_payload<pending_human_p_event_data>();
		execute_make_event_choice(state, source_nation, data);
		break;
	}
	case command_type::make_f_p_event_choice:
	{
		auto& data = c.get_payload<pending_human_f_p_event_data>();
		execute_make_event_choice(state, source_nation, data);
		break;
	}
	case command_type::cancel_cb_fabrication:
	{
		execute_cancel_cb_fabrication(state, source_nation);
		break;
	}
	case command_type::fabricate_cb:
	{
		auto& data = c.get_payload<cb_fabrication_data>();
		execute_fabricate_cb(state, source_nation, data.target, data.type, data.target_state);
		break;
	}
	case command_type::ask_for_military_access:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_ask_for_access(state, source_nation, data.target);
		break;
	}
	case command_type::ask_for_alliance:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_ask_for_alliance(state, source_nation, data.target);
		break;
	}
	case command_type::ask_for_free_trade_agreement:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_ask_for_free_trade_agreement(state, source_nation, data.target);
		break;
	}
	case command_type::switch_embargo_status:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_switch_embargo_status(state, source_nation, data.target);
		break;
	}
	case command_type::revoke_trade_rights:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_revoke_trade_rights(state, source_nation, data.target);
		break;
	}
	case command_type::call_to_arms:
	{
		auto& data = c.get_payload<call_to_arms_data>();
		execute_call_to_arms(state, source_nation, data.target, data.war, data.automatic_call);
		break;
	}
	case command_type::respond_to_diplomatic_message:
	{
		auto& data = c.get_payload<message_data>();
		execute_respond_to_diplomatic_message(state, source_nation, data.from, data.type, data.accept);
		break;
	}
	case command_type::cancel_military_access:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_cancel_military_access(state, source_nation, data.target);
		break;
	}
	case command_type::cancel_alliance:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_cancel_alliance(state, source_nation, data.target);
		break;
	}
	case command_type::cancel_given_military_access:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_cancel_military_access(state, source_nation, data.target);
		break;
	}
	case command_type::declare_war:
	{
		auto& data = c.get_payload<new_war_data>();
		execute_declare_war(state, source_nation, data.target, data.primary_cb, data.cb_state,
				data.cb_tag, data.cb_secondary_nation, data.call_attacker_allies, data.run_conference);
		break;
	}
	case command_type::add_war_goal:
	{
		auto& data = c.get_payload<new_war_goal_data>();
		execute_add_war_goal(state, source_nation, data.war, data.target, data.cb_type,
				data.cb_state, data.cb_tag, data.cb_secondary_nation);
		break;
	}
	case command_type::start_peace_offer:
	{
		auto& data = c.get_payload<new_offer_data>();
		execute_start_peace_offer(state, source_nation, data.target, data.war,
				data.is_concession);
		break;
	}
	case command_type::add_peace_offer_term:
	{
		auto& data = c.get_payload<offer_wargoal_data>();
		execute_add_to_peace_offer(state, source_nation, data.wg);
		break;
	}
	case command_type::send_peace_offer:
	{
		execute_send_peace_offer(state, source_nation);
		break;
	}
	case command_type::move_army:
	{
		auto& data = c.get_payload<army_movement_data>();
		execute_move_army(state, source_nation, data.a, data.dest, data.reset, data.special_order);
		break;
	}
	case command_type::move_navy:
	{
		auto& data = c.get_payload<navy_movement_data>();
		execute_move_navy(state, source_nation, data.n, data.dest, data.reset);
		break;
	}
	case command_type::embark_army:
	{
		auto& data = c.get_payload<army_movement_data>();
		execute_embark_army(state, source_nation, data.a);
		break;
	}
	case command_type::merge_armies:
	{
		auto& data = c.get_payload<merge_army_data>();
		execute_merge_armies(state, source_nation, data.a, data.b);
		break;
	}
	case command_type::merge_navies:
	{
		auto& data = c.get_payload<merge_navy_data>();
		execute_merge_navies(state, source_nation, data.a, data.b);
		break;
	}
	case command_type::split_army:
	{
		auto& data = c.get_payload<army_movement_data>();
		execute_split_army(state, source_nation, data.a);
		break;
	}
	case command_type::split_navy:
	{
		auto& data = c.get_payload<navy_movement_data>();
		execute_split_navy(state, source_nation, data.n);
		break;
	}
	case command_type::change_unit_type:
	{
		auto& data = c.get_payload<change_unit_type_data>();
		execute_change_unit_type(state, source_nation, data.regs, data.ships, data.new_type);
		break;
	}
	case command_type::delete_army:
	{
		auto& data = c.get_payload<army_movement_data>();
		execute_delete_army(state, source_nation, data.a);
		break;
	}
	case command_type::delete_navy:
	{
		auto& data = c.get_payload<navy_movement_data>();
		execute_delete_navy(state, source_nation, data.n);
		break;
	}
	case command_type::designate_split_regiments:
	{
		auto& data = c.get_payload<split_regiments_data>();
		execute_mark_regiments_to_split(state, source_nation, data.regs);
		break;
	}
	case command_type::designate_split_ships:
	{
		auto& data = c.get_payload<split_ships_data>();
		execute_mark_ships_to_split(state, source_nation, data.ships);
		break;
	}
	case command_type::naval_retreat:
	{
		auto& data = c.get_payload<retreat_from_naval_battle_data>();
		execute_retreat_from_naval_battle(state, source_nation, data.navy, data.dest);
		break;
	}
	case command_type::land_retreat:
	{
		auto& data = c.get_payload<land_battle_data>();
		execute_retreat_from_land_battle(state, source_nation, data.b);
		break;
	}
	case command_type::start_crisis_peace_offer:
	{
		auto& data = c.get_payload<new_offer_data>();
		execute_start_crisis_peace_offer(state, source_nation, data.is_concession);
		break;
	}
	case command_type::invite_to_crisis:
	{
		auto& data = c.get_payload<crisis_invitation_data>();
		execute_invite_to_crisis(state, source_nation, data);
		break;
	}
	case command_type::add_wargoal_to_crisis_offer:
	{
		auto& data = c.get_payload<crisis_invitation_data>();
		execute_add_to_crisis_peace_offer(state, source_nation, data);
		break;
	}
	case command_type::crisis_add_wargoal:
	{
		auto& data = c.get_payload<new_war_goal_data>();
		execute_crisis_add_wargoal(state, source_nation, data);
		break;
	}
	case command_type::send_crisis_peace_offer:
	{
		execute_send_crisis_peace_offer(state, source_nation);
		break;
	}
	case command_type::change_admiral:
	{
		auto& data = c.get_payload<new_admiral_data>();
		execute_change_admiral(state, source_nation, data.a, data.l);
		break;
	}
	case command_type::change_general:
	{
		auto& data = c.get_payload<new_general_data>();
		execute_change_general(state, source_nation, data.a, data.l);
		break;
	}
	case command_type::toggle_mobilization:
	{
		execute_toggle_mobilization(state, source_nation);
		break;
	}
	case command_type::give_military_access:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_give_military_access(state, source_nation, data.target);
		break;
	}
	case command_type::set_rally_point:
	{
		auto& data = c.get_payload<rally_point_data>();
		execute_set_rally_point(state, source_nation, data.location, data.naval, data.enable);
		break;
	}
	case command_type::save_game:
	{
		auto& data = c.get_payload<save_game_data>();
		std::string str(data.filename(), data.filename_len);
		execute_save_game(state, source_nation, data.and_quit, str);
		break;
	}
	case command_type::cancel_factory_building_construction:
	{
		auto& data = c.get_payload<factory_building_data>();
		execute_cancel_factory_building_construction(state, source_nation, data.location, data.type);
		break;
	}
	case command_type::disband_undermanned:
	{
		auto& data = c.get_payload < army_movement_data>();
		execute_disband_undermanned_regiments(state, source_nation, data.a);
		break;
	}
	case command_type::even_split_army:
	{
		auto& data = c.get_payload<army_movement_data>();
		execute_evenly_split_army(state, source_nation, data.a);
		break;
	}
	case command_type::even_split_navy:
	{
		auto& data = c.get_payload<navy_movement_data>();
		execute_evenly_split_navy(state, source_nation, data.n);
		break;
	}
	case command_type::toggle_hunt_rebels:
	{
		auto& data = c.get_payload<army_movement_data>();
		execute_toggle_rebel_hunting(state, source_nation, data.a);
		break;
	}
	case command_type::toggle_select_province:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_toggle_select_province(state, source_nation, data.prov);
		break;
	}
	case command_type::toggle_immigrator_province:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_toggle_immigrator_province(state, source_nation, data.prov);
		break;
	}
	case command_type::state_transfer:
	{
		auto& data = c.get_payload<state_transfer_data>();
		execute_state_transfer(state, source_nation, data.target, data.state);
		break;
	}
	case command_type::release_subject:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_release_subject(state, source_nation, data.target);
		break;
	}
	case command_type::enable_debt:
	{
		auto& data = c.get_payload<make_leader_data>();
		execute_enable_debt(state, source_nation, data.is_general);
		break;
	}
	case command_type::move_capital:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_move_capital(state, source_nation, data.prov);
		break;
	}
	case command_type::toggle_local_administration:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_toggle_local_administration(state, source_nation, data.prov);
		break;
	}
	case command_type::take_province:
	{
		auto& data = c.get_payload<generic_location_data>();
		execute_take_province(state, source_nation, data.prov);
		break;
	}
	case command_type::toggle_unit_ai_control:
	{
		auto& data = c.get_payload<army_movement_data>();
		execute_toggle_unit_ai_control(state, source_nation, data.a);
		break;
	}
	case command_type::toggle_mobilized_is_ai_controlled:
	{
		execute_toggle_mobilized_is_ai_controlled(state, source_nation);
		break;
	}
	case command_type::toggle_interested_in_alliance:
	{
		auto& data = c.get_payload<diplo_action_data>();
		execute_toggle_interested_in_alliance(state, source_nation, data.target);
		break;
	}
	case command_type::toggle_production_directive:
	{
		auto& data = c.get_payload<production_directive_data>();
		execute_toggle_production_directive(state, source_nation, data.for_state, data.id);
		break;
	}
	case command_type::pbutton_script:
	{
		auto& data = c.get_payload<pbutton_data>();
		execute_use_province_button(state, source_nation, data.button, data.id);
		break;
	}
	case command_type::nbutton_script:
	{
		auto& data = c.get_payload<nbutton_data>();
		execute_use_nation_button(state, source_nation, data.button, data.id);
		break;
	}
		// common mp commands
	case command_type::chat_message:
	{
		auto& data = c.get_payload<chat_message_data>();
		std::string_view sv(data.body(), data.msg_len);
		execute_chat_message(state, source_nation, sv, data.target, c.header.player_id);
		break;
	}
	case command_type::notify_player_ban:
	{
		auto& data = c.get_payload<notify_player_ban_data>();
		execute_notify_player_ban(state, source_nation, data.make_ai, c.header.player_id);
		break;
	}
	case command_type::notify_player_kick:
	{
		auto& data = c.get_payload<notify_player_kick_data>();
		execute_notify_player_kick(state, source_nation, data.make_ai, c.header.player_id);
		break;
	}
	case command_type::notify_player_joins:
	{
		auto& data = c.get_payload<notify_joins_data>();
		execute_notify_player_joins(state, source_nation, data.player_name, data.player_password, data.needs_loading, data.player_nation);
		break;
	}
	case command_type::notify_player_leaves:
	{
		auto& data = c.get_payload<notify_leaves_data>();
		execute_notify_player_leaves(state, source_nation, data.make_ai, c.header.player_id);
		break;
	}
	case command_type::notify_player_picks_nation:
	{
		auto& data = c.get_payload<nation_pick_data>();
		execute_notify_player_picks_nation(state, source_nation, data.target, c.header.player_id);
		break;
	}
	case command_type::notify_player_oos:
	{
		execute_notify_player_oos(state, source_nation, c.header.player_id);
		break;
	}
	case command_type::notify_oos_gamestate:
	{
		const auto& data = c.get_payload<notify_oos_gamestate_data>();
		execute_notify_oos_gamestate(state, source_nation, c.header.player_id, data.gamestate_data(), data.size);
		break;
	}
	case command_type::advance_tick:
	{
		auto& data = c.get_payload<advance_tick_data>();
		execute_advance_tick(state, source_nation, data.checksum, data.speed, data.date);
		break;
	}
	case command_type::notify_save_loaded:
	{
		auto& data = c.get_payload<notify_save_loaded_data>();
		execute_notify_save_loaded(state, source_nation, data.checksum);
		break;
	}
	case command_type::notify_reload:
	{
		auto& data = c.get_payload<notify_reload_data>();
		execute_notify_reload(state, source_nation, data.checksum);
		break;
	}
	case command_type::notify_start_game:
	{
		execute_notify_start_game(state, source_nation);
		break;
	}
	case command_type::notify_stop_game:
	{
		execute_notify_stop_game(state, source_nation);
		break;
	}
	case command_type::notify_pause_game:
	{
		execute_notify_pause_game(state, source_nation);
		break;
	}
	case command_type::network_inactivity_ping:
	{
		auto& data = c.get_payload<advance_tick_data>();
		execute_network_inactivity_ping(state, source_nation, data.date, c.header.player_id);
		break;
	}
	case command_type::console_command:
	{
		execute_console_command(state);
		break;
	}
	case command_type::grant_province:
		break;
	case command_type::network_populate:
		break;
	case command_type::notify_player_fully_loaded:
	{
		execute_notify_player_fully_loaded(state, source_nation, c.header.player_id);
		break;
	}
	case command_type::notify_player_is_loading:
	{
		execute_notify_player_is_loading(state, source_nation, c.header.player_id);
		break;
	}
	case command_type::change_ai_nation_state:
	{
		auto& data = c.get_payload<change_ai_nation_state_data>();
		execute_change_ai_nation_state(state, source_nation, data.no_ai);
		break;
	}
	case command_type::stop_army_movement:
	{
		auto& data = c.get_payload<stop_army_movement_data>();
		execute_stop_army_movement(state, source_nation, data.army);
		break;
	}
	case command_type::stop_navy_movement:
	{
		auto& data = c.get_payload<stop_navy_movement_data>();
		execute_stop_navy_movement(state, source_nation, data.navy);
		break;
	}
	case command_type::command_units:
	{
		auto& data = c.get_payload<command_units_data>();
		execute_command_units(state, source_nation, data.target);
		break;
	}
	case command_type::give_back_units:
	{
		auto& data = c.get_payload<command_units_data>();
		execute_give_back_units(state, source_nation, data.target);
		break;
	}
	case command_type::change_game_rule_setting:
	{
		auto& data = c.get_payload<change_gamerule_setting_data>();
		execute_change_gamerule_setting(state, source_nation, data.gamerule, data.setting);
		break;
	}
	case command_type::resync_lobby:
	{
		execute_resync_lobby(state, source_nation);
		break;
	}
	case command_type::notify_mp_data:
	{
		auto& data = c.get_payload<notify_mp_data_data>();
		execute_notify_mp_data(state, data);
		break;
	}
	case command_type::notify_player_timeout:
	{
		const auto& data = c.get_payload<notify_player_timeout_data>();
		execute_notify_player_timeout(state, source_nation, data.make_ai, c.header.player_id);
		break;
	}
	}
	state.tick_end_counter.fetch_add(1, std::memory_order::seq_cst);
	return true;
}

bool is_host_receive_command(command_type type, const sys::state& state) {
	auto iterator = command_type_handlers.find(type);
	// have bounds checking to guard against invalid commands
	if(iterator == command_type_handlers.end()) {
		return false;
	} else {
		return iterator->second.is_host_receive_command(state);
	}
}

bool is_host_broadcast_command(const sys::state& state, const command_data& command) {
	auto iterator = command_type_handlers.find(command.header.type);
	// have bounds checking to guard against invalid commands
	if(iterator == command_type_handlers.end()) {
		return false;
	}
	else {
		return iterator->second.is_host_broadcast_command(state);
	}
}



void execute_pending_commands(sys::state& state) {
	auto* c = state.incoming_commands.front();
	bool command_executed = false;
	while(c) {
		command_executed = true;
		execute_command(state, *c);
		state.incoming_commands.pop();
		c = state.incoming_commands.front();
	}

	if(command_executed) {
		province::update_connected_regions(state);
		province::update_cached_values(state);
		nations::update_cached_values(state);
		state.game_state_updated.store(true, std::memory_order::release);
	}
}

} // namespace command
