#include <string>
#include <string_view>
#include <variant>
#include "gui_console.hpp"
#include "gui_fps_counter.hpp"
#include "nations.hpp"

struct command_info {
	std::string_view name;
	enum class type : uint8_t {
		none = 0, reload, abort, clear_log, fps, set_tag, help, fire_national_event,fire_free_national_event, fire_provincial_event, fire_free_provincial_event,
		show_stats,
		search_tag
	} mode = type::none;
	std::string_view desc;
	struct argument_info {
		std::string_view name;
		enum class type : uint8_t {
			none = 0, numeric, tag, text
		} mode = type::none;
		bool optional = false;
	} args[4] = {};
};
static const std::vector<command_info> possible_commands = {
	command_info{ "none", command_info::type::none, "Dummy command",
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "reload", command_info::type::reload, "Reloads Alice",
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "abort", command_info::type::abort, "Abnormaly terminates execution",
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "clr_log", command_info::type::clear_log, "Clears console logs",
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "fps", command_info::type::fps, "Toggles FPS counter",
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "tag", command_info::type::set_tag, "Set the current player's country",
		command_info::argument_info{ "country", command_info::argument_info::type::tag, false },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "search", command_info::type::search_tag, "Set the current player's country",
		command_info::argument_info{ "country", command_info::argument_info::type::tag, false },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "help", command_info::type::help, "Display help",
		command_info::argument_info{ "cmd", command_info::argument_info::type::text, true },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "fne", command_info::type::fire_national_event, "Fire (trigger-based) national event",
		command_info::argument_info{ "event-id", command_info::argument_info::type::numeric, false },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "ffn", command_info::type::fire_free_national_event, "Fire free (random-chance based) national event",
		command_info::argument_info{ "event-id", command_info::argument_info::type::numeric, false },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "fpe", command_info::type::fire_provincial_event, "Fire (trigger-based) provincial event",
		command_info::argument_info{ "event-id", command_info::argument_info::type::numeric, false },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "ffp", command_info::type::fire_free_provincial_event, "Fire free (random-chance based) provincial event",
		command_info::argument_info{ "event-id", command_info::argument_info::type::numeric, false },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
	command_info{ "stats", command_info::type::show_stats, "Shows statistics of the current resources used",
		command_info::argument_info{ "type", command_info::argument_info::type::text, true },
		command_info::argument_info{},
		command_info::argument_info{},
		command_info::argument_info{}
	},
};

static uint32_t levenshtein_distance(std::string_view s1, std::string_view s2) {
	if(s1.empty() || s2.empty())
		return uint32_t(s1.empty() ? s2.size() : s1.size());
	std::vector<std::vector<uint32_t>> dist(s1.size(), std::vector<uint32_t>(s2.size(), 0));
	for(size_t i = 0; i < s1.size(); ++i)
		dist[i][0] = uint32_t(i);
	for(size_t j = 0; j < s2.size(); ++j)
		dist[0][j] = uint32_t(j);
	for(size_t j = 1; j < s2.size(); ++j)
		for(size_t i = 1; i < s1.size(); ++i) {
			auto cost = s1[i] == s2[j] ? 0 : 1;
			auto x = std::min(dist[i][j - 1], dist[i - 1][j - 1] + cost);
			dist[i][j] = std::min(dist[i - 1][j] + 1, x);
		}
	return dist[s1.size() - 1][s2.size() - 1];
}

static bool set_active_tag(sys::state& state, std::string_view tag) noexcept {
	bool found = false;
	state.world.for_each_national_identity([&](dcon::national_identity_id id) {
		std::string curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
		if(curr == tag) {
			dcon::national_identity_fat_id fat_id = dcon::fatten(state.world, id);
			state.local_player_nation = fat_id.get_nation_from_identity_holder().id;
			found = true;
		}
	});
	return found;
}

static void log_to_console(sys::state& state, ui::element_base* parent, std::string_view s) noexcept {
	Cyto::Any output = std::string(s);
	parent->impl_get(state, output);
}

struct parser_state {
	command_info cmd{};
	std::variant<
		std::monostate, // none
		std::string_view, // tag/string
		uint32_t // numeric
	> arg_slots[4] = {};
};
static parser_state parse_command(sys::state& state, std::string_view s) {
	// Parse command
	parser_state pstate{};
	pstate.cmd = possible_commands[0];
	for(const auto& cmd : possible_commands)
		if(s.starts_with(cmd.name)) {
			pstate.cmd = cmd;
			break;
		}
	if(pstate.cmd.mode == command_info::type::none)
		return parser_state{};
	char const* start = s.data();
	char const* end = s.data() + s.length();
	char const* position = start + pstate.cmd.name.size();
	for(; position < end && isspace(*position); ++position)
		;
	for(auto i = 0; i < 4; ++i) {
		char const* ident_start = position;
		for(; position < end && !isspace(*position) ; ++position)
			;
		char const* ident_end = position;
		if(ident_start == ident_end)
			break;
		std::string_view ident(ident_start, ident_end);
		if(pstate.cmd.args[i].mode == command_info::argument_info::type::text
		|| pstate.cmd.args[i].mode == command_info::argument_info::type::tag)
			pstate.arg_slots[i] = ident;
		else if(pstate.cmd.args[i].mode == command_info::argument_info::type::numeric)
			pstate.arg_slots[i] = uint32_t(std::stoi(std::string(ident)));
		else
			pstate.arg_slots[i] = std::monostate{};
		// Skip spaces
		for(; position < end && isspace(*position); ++position)
			;
		if(position >= end)
			break;
	}
	return pstate;
}

void ui::console_edit::edit_box_enter(sys::state& state, std::string_view s) noexcept {
	if(s.empty())
		return;
	
	auto pstate = parse_command(state, s);
	if(pstate.cmd.mode == command_info::type::none)
		return;
	
	log_to_console(state, parent, s);
	switch(pstate.cmd.mode) {
	case command_info::type::reload:
		log_to_console(state, parent, "Reloading...");
		state.map_display.load_map(state);
		break;
	case command_info::type::abort:
		log_to_console(state, parent, "Aborting...");
		std::abort();
		break;
	case command_info::type::clear_log:
		static_cast<console_window*>(parent)->clear_list(state);
		break;
	case command_info::type::fps:
		if(!state.ui_state.fps_counter) {
			auto fps_counter = make_element_by_type<fps_counter_text_box>(state, "fps_counter");
			state.ui_state.fps_counter = fps_counter.get();
			state.ui_state.root->add_child_to_front(std::move(fps_counter));
		} else {
			state.ui_state.fps_counter->set_visible(state, !state.ui_state.fps_counter->is_visible());
			state.ui_state.root->move_child_to_front(state.ui_state.fps_counter);
		}
		break;
	case command_info::type::set_tag: {
		auto tag = std::get<std::string_view>(pstate.arg_slots[0]);
		if(set_active_tag(state, tag) == false) {
			std::pair<uint32_t, dcon::national_identity_id> closest_match{};
			bool name_instead_of_tag = false;
			closest_match.first = std::numeric_limits<uint32_t>::max();
			// Search for matches on tags
			state.world.for_each_national_identity([&](dcon::national_identity_id id) {
				std::string name = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
				auto dist = levenshtein_distance(tag, name);
				if(dist < closest_match.first) {
					closest_match.first = dist;
					closest_match.second = id;
				}
			});
			// And on country names themselves
			state.world.for_each_national_identity([&](dcon::national_identity_id id) {
				auto fat_id = dcon::fatten(state.world, id);
				auto name = text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name());
				if(name == tag)
					name_instead_of_tag = true;
				auto dist = levenshtein_distance(tag, name);
				if(dist < closest_match.first) {
					closest_match.first = dist;
					closest_match.second = id;
				}
			});
			// Print results of search
			auto fat_id = dcon::fatten(state.world, closest_match.second);
			std::string text = "Closest match might be \"" + nations::int_to_tag(fat_id.get_identifying_int()) + "\" (" + text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + ") Id #" + std::to_string(closest_match.second.value);
			log_to_console(state, parent, text);
			if(name_instead_of_tag)
				log_to_console(state, parent, "You need to use tags (3-letters) instead of the full name!");
			else
				log_to_console(state, parent, "Is this what you meant?");
		} else {
			log_to_console(state, parent, "Switching to " + std::string(tag));
		}
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::search_tag: {
		auto tag = std::get<std::string_view>(pstate.arg_slots[0]);
		std::pair<uint32_t, dcon::national_identity_id> closest_match{};
		closest_match.first = std::numeric_limits<uint32_t>::max();
		// Search for matches on tags
		state.world.for_each_national_identity([&](dcon::national_identity_id id) {
			std::string name = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
			auto dist = levenshtein_distance(tag, name);
			if(dist < closest_match.first) {
				closest_match.first = dist;
				closest_match.second = id;
			}
		});
		// And on country names themselves
		state.world.for_each_national_identity([&](dcon::national_identity_id id) {
			auto fat_id = dcon::fatten(state.world, id);
			auto name = fat_id.get_nation_from_identity_holder().get_name();
			auto dist = levenshtein_distance(tag, text::produce_simple_string(state, name));
			if(dist < closest_match.first) {
				closest_match.first = dist;
				closest_match.second = id;
			}
		});
		// Print results of search
		auto fat_id = dcon::fatten(state.world, closest_match.second);
		log_to_console(state, parent, "Closest match: \"" + nations::int_to_tag(fat_id.get_identifying_int()) + "\" (" + text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + ") Id #" + std::to_string(closest_match.second.value));
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::help: {
		auto log_command_info = [&](auto cmd) {
			std::string text = "* " + std::string(cmd.name) + " ";
			for(const auto& arg : cmd.args)
				if(arg.mode != command_info::argument_info::type::none) {
					if(arg.optional)
						text += "[(optional)" + std::string(arg.name) + "] ";
					else
						text += "(" + std::string(arg.name) + ") ";
				}
			text += "- " + std::string(cmd.desc);
			log_to_console(state, parent, text);
		};
		if(std::holds_alternative<std::string_view>(pstate.arg_slots[0])) {
			auto cmd_name = std::get<std::string_view>(pstate.arg_slots[0]);
			bool found = false;
			for(const auto& cmd : possible_commands)
				if(cmd.name == cmd_name) {
					log_command_info(cmd);
					found = true;
					break;
				}
			if(!found) {
				log_to_console(state, parent, "Command " + std::string(cmd_name) + " not found :<");
				// Give the user a command they might've mispelt!
				std::pair<uint32_t, command_info> closest_match{};
				closest_match.first = std::numeric_limits<uint32_t>::max();
				for(const auto& cmd : possible_commands) {
					const auto distance = levenshtein_distance(cmd_name, cmd.name);
					if(distance < closest_match.first) {
						closest_match.first = distance;
						closest_match.second = cmd;
					}
				}
				log_to_console(state, parent, "Did you mean " + std::string(closest_match.second.name) + "(" + std::string(closest_match.second.desc) + ")" "?");
			}
		} else {
			log_to_console(state, parent, "Here's some helpful commands ^-^");
			for(const auto& cmd : possible_commands)
				log_command_info(cmd);
		}
	} break;
	case command_info::type::fire_national_event: {
		auto num = std::get<uint32_t>(pstate.arg_slots[0]);
		auto id = dcon::national_event_id(dcon::national_event_id::value_base_t(num));
		ui::fire_event(state, id);
		log_to_console(state, parent, std::string("Firing national event ") + std::to_string(num));
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::fire_free_national_event: {
		auto num = std::get<uint32_t>(pstate.arg_slots[0]);
		auto id = dcon::free_national_event_id(dcon::free_national_event_id::value_base_t(num));
		ui::fire_event(state, id);
		log_to_console(state, parent, std::string("Firing free national event ") + std::to_string(num));
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::fire_provincial_event: {
		auto num = std::get<uint32_t>(pstate.arg_slots[0]);
		auto id = dcon::provincial_event_id(dcon::provincial_event_id::value_base_t(num));
		ui::fire_event(state, id);
		log_to_console(state, parent, std::string("Firing provincial event ") + std::to_string(num));
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::fire_free_provincial_event: {
		auto num = std::get<uint32_t>(pstate.arg_slots[0]);
		auto id = dcon::free_provincial_event_id(dcon::free_provincial_event_id::value_base_t(num));
		ui::fire_event(state, id);
		log_to_console(state, parent, std::string("Firing free provincial event ") + std::to_string(num));
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::show_stats: {
		if(!std::holds_alternative<std::string_view>(pstate.arg_slots[0])) {
			log_to_console(state, parent, "Valid options: demo(graphics), diplo(macy), eco(nomy), event(s), mil(itary), tech(nology), pol(itics), a(ll)/all");
			log_to_console(state, parent, "Ex: \"stats pol\"");
			break;
		}
		enum class flags : uint8_t {
			none = 0x00,
			demographics = 0x01,
			diplomacy = 0x02,
			economy = 0x04,
			events = 0x08,
			military = 0x10,
			technology = 0x20,
			politics = 0x40,
			all = 0x7F,
			count
		};
		uint8_t v = 0;
		const auto k = std::get<std::string_view>(pstate.arg_slots[0]);
		if(k[0] == 'd' && k[1] == 'e') { // de(mo)
			v |= uint8_t(flags::demographics);
		} else if(k[0] == 'd') { // d(iplo)
			v |= uint8_t(flags::diplomacy);
		} else if(k[0] == 'e' && k[1] == 'c') { // ec(on)
			v |= uint8_t(flags::economy);
		} else if(k[0] == 'e') { // e(vent)
			v |= uint8_t(flags::events);
		} else if(k[0] == 'm') { // m(il)
			v |= uint8_t(flags::military);
		} else if(k[0] == 't') { // t(ech)
			v |= uint8_t(flags::technology);
		} else if(k[0] == 'p') { // p(ol)
			v |= uint8_t(flags::politics);
		} else if(k[0] == 'a') { // a(ll)
			v |= uint8_t(flags::all);
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent, std::string("* National Identities - ") + std::to_string(state.world.national_identity_size()));
			log_to_console(state, parent, std::string("* Political Parties - ") + std::to_string(state.world.political_party_size()));
			log_to_console(state, parent, std::string("* Religions - ") + std::to_string(state.world.religion_size()));
			log_to_console(state, parent, std::string("* Culture Groups - ") + std::to_string(state.world.culture_group_size()));
			log_to_console(state, parent, std::string("* Cultures - ") + std::to_string(state.world.culture_size()));
			log_to_console(state, parent, std::string("* Culture Group Memberships - ") + std::to_string(state.world.culture_group_membership_size()));
			log_to_console(state, parent, std::string("* Cultural Unions (Of) - ") + std::to_string(state.world.cultural_union_of_size()));
		}
		if((v & uint8_t(flags::economy)) != 0) {
			log_to_console(state, parent, std::string("* Commodities - ") + std::to_string(state.world.commodity_size()));
			log_to_console(state, parent, std::string("* Modifiers - ") + std::to_string(state.world.modifier_size()));
			log_to_console(state, parent, std::string("* Factory Types - ") + std::to_string(state.world.factory_type_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent, std::string("* Ideology Groups - ") + std::to_string(state.world.ideology_group_size()));
			log_to_console(state, parent, std::string("* Ideologies - ") + std::to_string(state.world.ideology_size()));
			log_to_console(state, parent, std::string("* Ideology Group Memberships - ") + std::to_string(state.world.ideology_group_membership_size()));
			log_to_console(state, parent, std::string("* Issues - ") + std::to_string(state.world.issue_size()));
			log_to_console(state, parent, std::string("* Issue Options - ") + std::to_string(state.world.issue_option_size()));
			log_to_console(state, parent, std::string("* Reforms - ") + std::to_string(state.world.reform_size()));
			log_to_console(state, parent, std::string("* Reform Options - ") + std::to_string(state.world.reform_option_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent, std::string("* CB Types - ") + std::to_string(state.world.cb_type_size()));
		}
		if((v & uint8_t(flags::military)) != 0) {
			log_to_console(state, parent, std::string("* Leader Traits - ") + std::to_string(state.world.leader_trait_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent, std::string("* Pop Types - ") + std::to_string(state.world.pop_type_size()));
			log_to_console(state, parent, std::string("* Rebel Types - ") + std::to_string(state.world.rebel_type_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent, std::string("* Provinces - ") + std::to_string(state.world.province_size()));
			log_to_console(state, parent, std::string("* Province Adjacenciess - ") + std::to_string(state.world.province_adjacency_size()));
			log_to_console(state, parent, std::string("* Nation Adjacencies - ") + std::to_string(state.world.nation_adjacency_size()));
		}
		if((v & uint8_t(flags::military)) != 0) {
			log_to_console(state, parent, std::string("* Regiments - ") + std::to_string(state.world.regiment_size()));
			log_to_console(state, parent, std::string("* Ships - ") + std::to_string(state.world.ship_size()));
			log_to_console(state, parent, std::string("* Armies - ") + std::to_string(state.world.army_size()));
			log_to_console(state, parent, std::string("* Navies - ") + std::to_string(state.world.navy_size()));
			log_to_console(state, parent, std::string("* Army Controls - ") + std::to_string(state.world.army_control_size()));
			log_to_console(state, parent, std::string("* Army Locations - ") + std::to_string(state.world.army_location_size()));
			log_to_console(state, parent, std::string("* Army Memberships - ") + std::to_string(state.world.army_membership_size()));
			log_to_console(state, parent, std::string("* Regiment Sources - ") + std::to_string(state.world.regiment_source_size()));
			log_to_console(state, parent, std::string("* Navy Controls - ") + std::to_string(state.world.navy_control_size()));
			log_to_console(state, parent, std::string("* Navy Locations - ") + std::to_string(state.world.navy_location_size()));
			log_to_console(state, parent, std::string("* Navy Memberships - ") + std::to_string(state.world.navy_membership_size()));
			log_to_console(state, parent, std::string("* Leaders - ") + std::to_string(state.world.leader_size()));
			log_to_console(state, parent, std::string("* Army Leadership (leader<->army) - ") + std::to_string(state.world.army_leadership_size()));
			log_to_console(state, parent, std::string("* Navy Leadership (leader<->navy) - ") + std::to_string(state.world.navy_leadership_size()));
			log_to_console(state, parent, std::string("* Leader Loyalties (leader<->nation membership) - ") + std::to_string(state.world.leader_loyalty_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent, std::string("* Wars - ") + std::to_string(state.world.war_size()));
			log_to_console(state, parent, std::string("* Wargoals - ") + std::to_string(state.world.wargoal_size()));
			log_to_console(state, parent, std::string("* War Participants - ") + std::to_string(state.world.war_participant_size()));
			log_to_console(state, parent, std::string("* Wargoals Attached (wargoal<->war) - ") + std::to_string(state.world.wargoals_attached_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent, std::string("* State Definitions - ") + std::to_string(state.world.state_definition_size()));
			log_to_console(state, parent, std::string("* State Instances - ") + std::to_string(state.world.state_instance_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent, std::string("* Colonizations - ") + std::to_string(state.world.colonization_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent, std::string("* State Ownerships - ") + std::to_string(state.world.state_ownership_size()));
			log_to_console(state, parent, std::string("* Abstract State Memberships - ") + std::to_string(state.world.abstract_state_membership_size()));
			log_to_console(state, parent, std::string("* Cores - ") + std::to_string(state.world.core_size()));
			log_to_console(state, parent, std::string("* Identity Holders - ") + std::to_string(state.world.identity_holder_size()));
		}
		if((v & uint8_t(flags::technology)) != 0) {
			log_to_console(state, parent, std::string("* Technologies - ") + std::to_string(state.world.technology_size()));
			log_to_console(state, parent, std::string("* Inventions - ") + std::to_string(state.world.invention_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent, std::string("* Nations - ") + std::to_string(state.world.nation_size()));
			log_to_console(state, parent, std::string("* Diplomatic Relations - ") + std::to_string(state.world.diplomatic_relation_size()));
			log_to_console(state, parent, std::string("* Unilateral Relationships - ") + std::to_string(state.world.unilateral_relationship_size()));
			log_to_console(state, parent, std::string("* GP Relationships - ") + std::to_string(state.world.gp_relationship_size()));
		}
		if((v & uint8_t(flags::economy)) != 0) {
			log_to_console(state, parent, std::string("* Factories - ") + std::to_string(state.world.factory_size()));
			log_to_console(state, parent, std::string("* Factory Locations - ") + std::to_string(state.world.factory_location_size()));
		}
		if((v & uint8_t(flags::politics)) != 0) {
			log_to_console(state, parent, std::string("* Province Ownerships - ") + std::to_string(state.world.province_ownership_size()));
			log_to_console(state, parent, std::string("* Province Controls - ") + std::to_string(state.world.province_control_size()));
			log_to_console(state, parent, std::string("* Province Rebel Controls - ") + std::to_string(state.world.province_rebel_control_size()));
			log_to_console(state, parent, std::string("* Overlords - ") + std::to_string(state.world.overlord_size()));
			log_to_console(state, parent, std::string("* Rebel Factions - ") + std::to_string(state.world.rebel_faction_size()));
			log_to_console(state, parent, std::string("* Rebellions Within - ") + std::to_string(state.world.rebellion_within_size()));
			log_to_console(state, parent, std::string("* Movements - ") + std::to_string(state.world.movement_size()));
			log_to_console(state, parent, std::string("* Movements Within - ") + std::to_string(state.world.movement_within_size()));
			log_to_console(state, parent, std::string("* Pop Movement Memberships - ") + std::to_string(state.world.pop_movement_membership_size()));
			log_to_console(state, parent, std::string("* Pop Rebellion Memberships - ") + std::to_string(state.world.pop_rebellion_membership_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent, std::string("* Pops - ") + std::to_string(state.world.pop_size()));
			log_to_console(state, parent, std::string("* Pop Locations - ") + std::to_string(state.world.pop_location_size()));
		}
		if((v & uint8_t(flags::events)) != 0) {
			log_to_console(state, parent, std::string("* National Events - ") + std::to_string(state.world.national_event_size()));
			log_to_console(state, parent, std::string("* Provincial Events - ") + std::to_string(state.world.provincial_event_size()));
			log_to_console(state, parent, std::string("* Free National Events - ") + std::to_string(state.world.free_national_event_size()));
			log_to_console(state, parent, std::string("* Free Provincial Events - ") + std::to_string(state.world.free_provincial_event_size()));
			log_to_console(state, parent, std::string("* National Focuses - ") + std::to_string(state.world.national_focus_size()));
			log_to_console(state, parent, std::string("* Decisions - ") + std::to_string(state.world.decision_size()));
		}
	} break;
	case command_info::type::none:
		log_to_console(state, parent, "Command \"" + std::string(s) + "\" not found.");
		break;
	default:
		break;
	}
	log_to_console(state, parent, ""); // space after command
}

void ui::console_window::show_toggle(sys::state& state) {
	assert(state.ui_state.console_window);
	state.ui_state.console_window->set_visible(state, !state.ui_state.console_window->is_visible());
	if(state.ui_state.console_window->is_visible()) state.ui_state.root->move_child_to_front(state.ui_state.console_window);
}
