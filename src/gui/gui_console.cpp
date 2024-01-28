#include <string>
#include <string_view>
#include <variant>
#include "gui_console.hpp"
#include "gui_fps_counter.hpp"
#include "nations.hpp"

struct command_info {
	static constexpr uint32_t max_arg_slots = 4;

	std::string_view name;
	enum class type : uint8_t {
		none = 0,
		reload,
		abort,
		clear_log,
		fps,
		set_tag,
		help,
		show_stats,
		colour_guide,
		cheat,
		diplomacy_points,
		research_points,
		infamy,
		money,
		westernize,
		unwesternize,
		elecwin,
		mainmenu,
		cb_progress,
		crisis,
		end_game,
		event,
		militancy,
		dump_out_of_sync,
		fog_of_war,
		prestige,
		force_ally,
		win_wars,
		toggle_ai,
		always_allow_wargoals,
		always_allow_reforms,
		always_accept_deals,
		complete_constructions,
		instant_research,
		game_info,
		spectate,
		conquer_tag,
		change_owner,
		change_control,
		change_control_and_owner,
		province_id_tooltip,
		wasd,
		next_song,
		add_population,
		instant_army,
		instant_industry,
	} mode = type::none;
	std::string_view desc;
	struct argument_info {
		std::string_view name;
		enum class type : uint8_t {
			none = 0, numeric, tag, text
		} mode = type::none;
		bool optional = false;
	} args[max_arg_slots] = {};
};

inline constexpr command_info possible_commands[] = {
		command_info{"none", command_info::type::none, "Dummy command",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"reload", command_info::type::reload, "Reloads Alice",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"abort", command_info::type::abort, "Abnormally terminates execution",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"clr_log", command_info::type::clear_log, "Clears console logs",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"fps", command_info::type::fps, "Toggles FPS counter",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"tag", command_info::type::set_tag, "Set the current player's country",
				{command_info::argument_info{"country", command_info::argument_info::type::tag, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"help", command_info::type::help, "Display help",
				{command_info::argument_info{"cmd", command_info::argument_info::type::text, true}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"stats", command_info::type::show_stats, "Shows statistics of the current resources used",
				{command_info::argument_info{"type", command_info::argument_info::type::text, true}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"cheat", command_info::type::cheat,
				"Finishes all casus belli, adds 99 diplo points, instant research and westernizes (if not already)",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"dp", command_info::type::diplomacy_points, "Adds the specified number of diplo points",
				{command_info::argument_info{"amount", command_info::argument_info::type::numeric, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"rp", command_info::type::research_points, "Adds the specified number of research points",
				{command_info::argument_info{"amount", command_info::argument_info::type::numeric, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"inf", command_info::type::infamy, "Adds the specified number of infamy",
				{command_info::argument_info{"amount", command_info::argument_info::type::numeric, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"cbp", command_info::type::cb_progress, "Adds the specified % of progress towards CB fabrication",
				{command_info::argument_info{"amount", command_info::argument_info::type::numeric, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"money", command_info::type::money, "Adds the specified amount of money to the national treasury",
				{command_info::argument_info{"amount", command_info::argument_info::type::numeric, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"west", command_info::type::westernize, "Westernizes",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"unwest", command_info::type::unwesternize, "Unwesternizes",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"elecwin", command_info::type::elecwin, "Shows/Hides Election Window",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"mainmenu", command_info::type::mainmenu, "Shows/Hides Main Menu",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"colour", command_info::type::colour_guide, "An overview of available colors for complex text",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"crisis", command_info::type::crisis, "Force a crisis to occur",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"end_game", command_info::type::end_game, "Ends the game",
				{command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"event", command_info::type::event, "Triggers a random country event by its legacy id",
				{command_info::argument_info{"id", command_info::argument_info::type::numeric, false}, command_info::argument_info{"target", command_info::argument_info::type::tag, true}}},
		command_info{"angry", command_info::type::militancy, "Makes everyone in your nation very militant",
				{command_info::argument_info{"amount", command_info::argument_info::type::numeric, false}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"pr", command_info::type::prestige, "Increases prestige by amount",
				{command_info::argument_info{"amount", command_info::argument_info::type::numeric, false}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"oos", command_info::type::dump_out_of_sync, "Dump an OOS save",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"fow", command_info::type::fog_of_war, "Toggles fog of war ON/OFF",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}}},
		command_info{"fa", command_info::type::force_ally, "Force an alliance between you and a country",
				{command_info::argument_info{"country", command_info::argument_info::type::tag, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"ww", command_info::type::win_wars, "Win all current wars youre involved at",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"tai", command_info::type::toggle_ai, "Toggles ON/OFF AI for countries",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		/* doesn't work, removed until someone fixes it
		command_info{ "aw", command_info::type::always_allow_wargoals, "Always allow adding wargoals",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}}, */
		command_info{"ar", command_info::type::always_allow_reforms, "Always allow enacting reforms",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"cc", command_info::type::complete_constructions, "Complete all current constructions",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"ir", command_info::type::instant_research, "Instant research",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"ym", command_info::type::always_accept_deals, "AI always accepts our deals",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{"gi", command_info::type::game_info, "Shows general game information",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}}},
		command_info{ "spectate", command_info::type::spectate, "Become spectator nation",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "conquer", command_info::type::conquer_tag, "Annexes an entire nation (use 'all' for the entire world)",
				{command_info::argument_info{"tag", command_info::argument_info::type::tag, false}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "chow", command_info::type::change_owner, "Change province owner to country",
				{command_info::argument_info{"province", command_info::argument_info::type::numeric, false}, command_info::argument_info{"country", command_info::argument_info::type::tag, true},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "chcon", command_info::type::change_control, "Give province control to country",
				{command_info::argument_info{"province", command_info::argument_info::type::numeric, false}, command_info::argument_info{"country", command_info::argument_info::type::tag, true},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "chcow", command_info::type::change_control_and_owner, "Give province to country",
				{command_info::argument_info{"province", command_info::argument_info::type::numeric, false}, command_info::argument_info{"country", command_info::argument_info::type::tag, true},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "provid", command_info::type::province_id_tooltip, "show province id in mouse tooltip",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "wasd", command_info::type::wasd, "move camera with wasd",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "nextsong", command_info::type::next_song, "Skips to the next track",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "addpop", command_info::type::add_population, "Add a certain ammount of population to your nation",
				{command_info::argument_info{"ammount", command_info::argument_info::type::numeric, false }, command_info::argument_info{ },
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "instant_army", command_info::type::instant_army, "Instantly builds all armies",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}} },
		command_info{ "instant_industry", command_info::type::instant_industry, "Instantly builds all industries",
				{command_info::argument_info{}, command_info::argument_info{},
						command_info::argument_info{}, command_info::argument_info{}} },
						
};

uint32_t levenshtein_distance(std::string_view s1, std::string_view s2) {
	// NOTE: Change parameters as you wish - but these work fine for the majority of mods
	constexpr uint32_t insertion_cost = 1;
	constexpr uint32_t deletion_cost = 1;
	constexpr uint32_t subst_cost = 1;

	if(s1.empty() || s2.empty())
		return uint32_t(s1.empty() ? s2.size() : s1.size());
	std::vector<std::vector<uint32_t>> dist(s1.size(), std::vector<uint32_t>(s2.size(), 0));
	for(size_t i = 1; i < s1.size(); ++i)
		dist[i][0] = uint32_t(i);
	for(size_t j = 1; j < s2.size(); ++j)
		dist[0][j] = uint32_t(j);
	for(size_t j = 1; j < s2.size(); ++j)
		for(size_t i = 1; i < s1.size(); ++i) {
			auto cost = s1[i] == s2[j] ? 0 : subst_cost;
			auto x = std::min(dist[i - 1][j] + deletion_cost, dist[i][j - 1] + insertion_cost);
			dist[i][j] = std::min(x, dist[i - 1][j - 1] + cost);
		}
	return dist[s1.size() - 1][s2.size() - 1];
}

// Splits the strings into tokens and treats them as words individually, it is important that
// the needle DOES NOT have spaces or else the algorithm will not work properly.
// This is for being able to match country names which might be matchable iff treated as individual
// words instead of a big giant text.
uint32_t levenshtein_tokenized_distance(std::string_view needle, std::string_view haystack) {
	assert(needle.find(" ") == std::string::npos);
	uint32_t dist = std::numeric_limits<uint32_t>::max();
	std::string str{ haystack };
	size_t pos = 0;
	while((pos = str.find(" ")) != std::string::npos) {
		auto token = str.substr(0, pos);
		dist = std::min<uint32_t>(dist, levenshtein_distance(needle, token));
		str.erase(0, pos + 1);
	}
	return std::min<uint32_t>(dist, levenshtein_distance(needle, str));
}

bool set_active_tag(sys::state& state, std::string_view tag) noexcept {
	bool found = false;
	state.world.for_each_national_identity([&](dcon::national_identity_id id) {
		auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
		if(curr == tag) {
			command::c_switch_nation(state, state.local_player_nation, id);
			found = true;
		}
	});
	return found;
}

void log_to_console(sys::state& state, ui::element_base* parent, std::string_view s) noexcept {
	Cyto::Any output = std::string(s);
	parent->impl_get(state, output);
}

struct parser_state {
	command_info cmd{};
	std::variant< std::monostate, // none
		std::string, // tag/string
		int32_t // numeric
	> arg_slots[command_info::max_arg_slots] = {};
};

dcon::national_identity_id smart_get_national_identity_from_tag(sys::state& state, ui::element_base* parent, std::string_view tag) noexcept {
	dcon::national_identity_id nid{};
	for(auto id : state.world.in_national_identity) {
		if(nations::int_to_tag(state.world.national_identity_get_identifying_int(id)) == tag) {
			nid = id;
			break;
		}
	}

	// print entire schema arguing for the tag and the autosuggester
	if(!bool(nid)) {
		std::pair<uint32_t, dcon::national_identity_id> closest_tag_match{}; // schombert notes: using pair in this way is an abomination
		closest_tag_match.first = std::numeric_limits<uint32_t>::max();
		std::pair<uint32_t, dcon::national_identity_id> closest_name_match{};
		closest_name_match.first = std::numeric_limits<uint32_t>::max();
		for(auto fat_id : state.world.in_national_identity) {
			{ // Tags
				auto name = nations::int_to_tag(state.world.national_identity_get_identifying_int(fat_id));
				uint32_t dist = levenshtein_distance(tag, name);
				if(dist < closest_tag_match.first) {
					closest_tag_match.first = dist;
					closest_tag_match.second = fat_id;
				}
			}
			{ // Names
				auto name = text::produce_simple_string(state, fat_id.get_name());
				std::transform(name.begin(), name.end(), name.begin(), [](auto c) { return char(toupper(char(c))); });
				uint32_t dist = levenshtein_tokenized_distance(tag, name);
				if(dist < closest_name_match.first) {
					closest_name_match.first = dist;
					closest_name_match.second = fat_id;
				}
			}
		}
		// Print results of search
		if(tag.size() == 3) {
			auto fat_id = dcon::fatten(state.world, closest_tag_match.second);
			log_to_console(state, parent,
			"Tag could refer to @" + nations::int_to_tag(fat_id.get_identifying_int()) + " \"\xA7Y" + nations::int_to_tag(fat_id.get_identifying_int()) + "\xA7W\" (\xA7Y" + text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + "\xA7W) Id #" + std::to_string(closest_tag_match.second.value));
		} else {
			auto fat_id = dcon::fatten(state.world, closest_name_match.second);
			log_to_console(state, parent, "Name could refer to @" + nations::int_to_tag(fat_id.get_identifying_int()) + " \"\xA7Y" + nations::int_to_tag(fat_id.get_identifying_int()) + "\xA7W\" (\xA7Y" + text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + "\xA7W) Id #" + std::to_string(closest_name_match.second.value));
		}
		if(tag.size() != 3)
			log_to_console(state, parent, "You need to use \xA7Ytags\xA7W (3-letters) instead of the full name");
		else
			log_to_console(state, parent, "Is this what you meant?");
	}
	return nid;
}

parser_state parse_command(sys::state& state, std::string_view text) {
	std::string s{ text };
	// Makes all text lowercase for proper processing
	std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return char(tolower(char(c))); });

	// Parse command
	parser_state pstate{};
	pstate.cmd = possible_commands[0];
	size_t first_space = 0;
	for(size_t i = 0; i < s.size(); ++i) {
		if(isspace(s.at(i))) break;
		first_space = i;
	}
	for(auto const& cmd : possible_commands)
		if(s.compare(0, first_space + 1, cmd.name) == 0) {
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
	for(uint32_t i = 0; i < command_info::max_arg_slots; ++i) {
		char const* ident_start = position;
		for(; position < end && !isspace(*position); ++position)
			;
		char const* ident_end = position;
		if(ident_start == ident_end)
			break;

		std::string_view ident(ident_start, ident_end);
		switch(pstate.cmd.args[i].mode) {
		case command_info::argument_info::type::text:
			pstate.arg_slots[i] = std::string(ident);
			break;
		case command_info::argument_info::type::tag:
		{
			std::string tag{ ident };
			std::transform(tag.begin(), tag.end(), tag.begin(), [](auto c) { return char(toupper(char(c))); });
			pstate.arg_slots[i] = tag;
			break;
		}
		case command_info::argument_info::type::numeric:
			if(isdigit(ident[0]) || ident[0] == '-') {
				pstate.arg_slots[i] = int32_t(std::stoi(std::string(ident)));
			} else {
				pstate.arg_slots[i] = int32_t(0);
			}
			break;
		default:
			pstate.arg_slots[i] = std::monostate{};
			break;
		}
		// Skip spaces
		for(; position < end && isspace(*position); ++position)
			;
		if(position >= end)
			break;
	}
	return pstate;
}

void ui::console_edit::render(sys::state& state, int32_t x, int32_t y) noexcept {
	ui::edit_box_element_base::render(state, x, y);

	// Render the suggestions given (after the inputted text obv)
	float x_offs = state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()),
			base_data.data.text.font_handle);
	if(lhs_suggestion.length() > 0) {
		char const* start_text = lhs_suggestion.data();
		char const* end_text = lhs_suggestion.data() + lhs_suggestion.length();
		std::string text(std::string_view(start_text, end_text));
		if(!text.empty()) {
			ogl::render_text(state, text.c_str(), uint32_t(text.length()), ogl::color_modification::none,
					float(x + text_offset) + x_offs, float(y + base_data.data.text.border_size.y),
					get_text_color(text::text_color::light_grey), base_data.data.button.font_handle);
			x_offs += state.font_collection.text_extent(state, text.c_str(), uint32_t(text.length()), base_data.data.text.font_handle);
		}
	}

	if(rhs_suggestion.length() > 0) {
		char const* start_text = rhs_suggestion.data();
		char const* end_text = rhs_suggestion.data() + rhs_suggestion.length();
		std::string text(std::string_view(start_text, end_text));
		if(!text.empty()) {
			// Place text right before it ends (centered right)
			x_offs = float(base_data.size.x);
			x_offs -= 24;
			x_offs -= state.font_collection.text_extent(state, text.c_str(), uint32_t(text.length()), base_data.data.text.font_handle);
			ogl::render_text(state, text.c_str(), uint32_t(text.length()), ogl::color_modification::none,
					float(x + text_offset) + x_offs, float(y + base_data.data.text.border_size.y),
					get_text_color(text::text_color::light_grey), base_data.data.button.font_handle);
		}
	}
}

void ui::console_edit::edit_box_update(sys::state& state, std::string_view s) noexcept {
	lhs_suggestion = std::string{};
	rhs_suggestion = std::string{};
	if(s.empty())
		return;

	std::size_t pos = s.find_last_of(' ');
	if(pos == std::string::npos) {
		// Still typing command - so suggest commands
		std::pair<uint32_t, command_info const*> closest_match =
			std::make_pair<uint32_t, command_info const*>(std::numeric_limits<uint32_t>::max(), &possible_commands[0]);
		for(auto const& cmd : possible_commands) {
			std::string_view name = cmd.name;
			if(name.starts_with(s)) {
				if(name == s)
					return; // No suggestions given...
				uint32_t dist = levenshtein_distance(s, name);
				if(dist < closest_match.first) {
					closest_match.first = dist;
					closest_match.second = &cmd;
				}
			}
		}
		// Only suggest the "unfinished" part of the command and provide a brief description of it
		if(closest_match.second->name.length() > s.length())
			lhs_suggestion = closest_match.second->name.substr(s.length());
		rhs_suggestion = std::string(closest_match.second->desc);
	} else {
		// Specific suggestions for each command
		if(s.starts_with("tag") && pos + 1 < s.size()) {
			std::string tag = std::string(s.substr(pos + 1));
			if(tag.empty())
				return; // Can't give suggestion if nothing was inputted
			std::transform(tag.begin(), tag.end(), tag.begin(), [](auto c) { return char(toupper(char(c))); });
			// Tag will autofill a country name + indicate it's full name
			std::pair<uint32_t, dcon::national_identity_id> closest_match{};
			closest_match.first = std::numeric_limits<uint32_t>::max();
			state.world.for_each_national_identity([&](dcon::national_identity_id id) {
				dcon::national_identity_fat_id fat_id = dcon::fatten(state.world, id);
				std::string name = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
				if(name.starts_with(tag)) {
					uint32_t dist = levenshtein_distance(tag, name);
					if(dist < closest_match.first) {
						closest_match.first = dist;
						closest_match.second = id;
					}
				}
			});
			if(closest_match.second) {
				// Now type in a suggestion...
				dcon::nation_id nid = state.world.identity_holder_get_nation(state.world.national_identity_get_identity_holder(closest_match.second));
				std::string name = nations::int_to_tag(state.world.national_identity_get_identifying_int(closest_match.second));
				if(tag.size() >= name.size()) {
					lhs_suggestion = std::string{};
				} else {
					lhs_suggestion = name.substr(tag.size());
				}
				rhs_suggestion = text::produce_simple_string(state, state.world.nation_get_name(nid)) + " - " + name;
			} else {
				lhs_suggestion = std::string{};
				rhs_suggestion = std::string{};
				if(tag.size() == 1)
					rhs_suggestion = tag + "?? - ???";
				else if(tag.size() == 2)
					rhs_suggestion = tag + "? - ???";
			}
		}
	}
}

void ui::console_edit::edit_box_tab(sys::state& state, std::string_view s) noexcept {
	if(s.empty())
		return;

	std::pair<uint32_t, std::string_view> closest_match{};
	closest_match.first = std::numeric_limits<uint32_t>::max();

	// Loop through possible_commands
	for(auto const& cmd : possible_commands) {
		std::string_view name = cmd.name;
		if(name.starts_with(s)) {
			uint32_t dist = levenshtein_distance(s, name);
			if(dist < closest_match.first) {
				closest_match.first = dist;
				closest_match.second = name;
			}
		}
	}
	auto closest_name = closest_match.second;
	if(closest_name.empty())
		return;
	set_text(state, std::string(closest_name) + " ");
	auto index = int32_t(closest_name.size() + 1);
	edit_index_position(state, index);
	edit_box_update(state, s);
}

void ui::console_edit::edit_box_up(sys::state& state) noexcept {
	std::string up = up_history();
	if(!up.empty()) {
		this->set_text(state, up);
		auto index = int32_t(up.size());
		this->edit_index_position(state, index);
	}
}
void ui::console_edit::edit_box_down(sys::state& state) noexcept {
	std::string down = down_history();
	if(!down.empty()) {
		this->set_text(state, down);
		auto index = int32_t(down.size());
		this->edit_index_position(state, index);
	}
}

template<typename F>
void write_single_component(sys::state& state, native_string_view filename, F&& func) {
	auto sdir = simple_fs::get_or_create_oos_directory();
	auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[sys::sizeof_scenario_section(state)]);
	auto buffer_position = func(buffer.get(), state);
	size_t total_size_used = reinterpret_cast<uint8_t*>(buffer_position) - buffer.get();
	simple_fs::write_file(sdir, filename, reinterpret_cast<char*>(buffer.get()), uint32_t(total_size_used));
}

void ui::console_edit::edit_box_enter(sys::state& state, std::string_view s) noexcept {
	if(s.empty())
		return;

	auto pstate = parse_command(state, s);
	if(pstate.cmd.mode == command_info::type::none)
		return;

	log_to_console(state, parent, s);
	for(uint32_t i = 0; i < command_info::max_arg_slots; ++i) {
		if(pstate.cmd.args[i].optional)
			continue;
		if(pstate.cmd.args[i].mode == command_info::argument_info::type::text) {
			if(!std::holds_alternative<std::string>(pstate.arg_slots[i])) {
				log_to_console(state, parent, "Command requires a \xA7Ytext\xA7W argument at " + std::to_string(i));
				Cyto::Any payload = this;
				impl_get(state, payload);
				return;
			}
		} else if(pstate.cmd.args[i].mode == command_info::argument_info::type::tag) {
			if(!std::holds_alternative<std::string>(pstate.arg_slots[i])) {
				log_to_console(state, parent, "Command requires a \xA7Ytag\xA7W argument at " + std::to_string(i));
				Cyto::Any payload = this;
				impl_get(state, payload);
				return;
			}
		} else if(pstate.cmd.args[i].mode == command_info::argument_info::type::numeric) {
			if(!std::holds_alternative<int32_t>(pstate.arg_slots[i])) {
				log_to_console(state, parent, "Command requires a \xA7Ynumeric\xA7W argument at " + std::to_string(i));
				Cyto::Any payload = this;
				impl_get(state, payload);
				return;
			}
		}
	}
	switch(pstate.cmd.mode) {
	case command_info::type::mainmenu:
		if(!state.ui_state.main_menu) {
			show_main_menu(state);
		} else {
			state.ui_state.main_menu->is_visible() ? state.ui_state.main_menu->set_visible(state, false)
				: state.ui_state.main_menu->set_visible(state, true);
			state.ui_state.main_menu->impl_on_update(state);
		}
		break;
	case command_info::type::elecwin:
		state.ui_state.election_window->is_visible() ? state.ui_state.election_window->set_visible(state, false)
			: state.ui_state.election_window->set_visible(state, true);
		state.ui_state.election_window->impl_on_update(state);
		break;
	case command_info::type::reload:
		log_to_console(state, parent, "Reloading...");
		state.map_state.load_map(state);
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
	case command_info::type::set_tag:
	{
		auto tag = std::get<std::string>(pstate.arg_slots[0]);
		if(set_active_tag(state, tag) == false) {
			std::pair<uint32_t, dcon::national_identity_id> closest_tag_match{};
			closest_tag_match.first = std::numeric_limits<uint32_t>::max();
			std::pair<uint32_t, dcon::national_identity_id> closest_name_match{};
			closest_name_match.first = std::numeric_limits<uint32_t>::max();
			state.world.for_each_national_identity([&](dcon::national_identity_id id) {
				auto fat_id = dcon::fatten(state.world, id);
				{ // Tags
					auto name = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
					uint32_t dist = levenshtein_distance(tag, name);
					if(dist < closest_tag_match.first) {
						closest_tag_match.first = dist;
						closest_tag_match.second = id;
					}
				}
				{ // Names
					auto name = text::produce_simple_string(state, fat_id.get_name());
					std::transform(name.begin(), name.end(), name.begin(), [](auto c) { return char(toupper(char(c))); });
					uint32_t dist = levenshtein_tokenized_distance(tag, name);
					if(dist < closest_name_match.first) {
						closest_name_match.first = dist;
						closest_name_match.second = id;
					}
				}
			});
			// Print results of search
			if(tag.size() == 3) {
				auto fat_id = dcon::fatten(state.world, closest_tag_match.second);
				log_to_console(state, parent,
						"Tag could refer to @" + nations::int_to_tag(fat_id.get_identifying_int()) + " \"\xA7Y" + nations::int_to_tag(fat_id.get_identifying_int()) + "\xA7W\" (\xA7Y" +
								text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + "\xA7W) Id #" +
								std::to_string(closest_tag_match.second.value));
			} else {
				auto fat_id = dcon::fatten(state.world, closest_name_match.second);
				log_to_console(state, parent,
						"Name could refer to @" + nations::int_to_tag(fat_id.get_identifying_int()) + " \"\xA7Y" + nations::int_to_tag(fat_id.get_identifying_int()) + "\xA7W\" (\xA7Y" +
								text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + "\xA7W) Id #" +
								std::to_string(closest_name_match.second.value));
			}

			if(tag.size() != 3)
				log_to_console(state, parent, "You need to use \xA7Ytags\xA7W (3-letters) instead of the full name");
			else
				log_to_console(state, parent, "Is this what you meant?");
		} else {
			auto nid = smart_get_national_identity_from_tag(state, parent, tag);
			if(bool(nid)) {
				command::c_switch_nation(state, state.local_player_nation, nid);
				log_to_console(state, parent, "Switching to @" + std::string(tag) + " \xA7Y" + std::string(tag) + "\xA7W");
				state.game_state_updated.store(true, std::memory_order::release);
			}
		}
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::help:
	{
		auto log_command_info = [&](auto cmd) {
			std::string text = "\x95"
				"\xA7Y" +
				std::string(cmd.name) + "\xA7W ";
			for(const auto& arg : cmd.args)
				if(arg.mode != command_info::argument_info::type::none) {
					if(arg.optional)
						text += "\xA7Y[(optional)\xA7W" + std::string(arg.name) + "] ";
					else
						text += "\xA7G(" + std::string(arg.name) + ")\xA7W ";
				}
			text += "- " + std::string(cmd.desc);
			log_to_console(state, parent, text);
			};
		if(std::holds_alternative<std::string>(pstate.arg_slots[0])) {
			auto cmd_name = std::get<std::string>(pstate.arg_slots[0]);
			bool found = false;
			for(auto const& cmd : possible_commands)
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
				for(auto const& cmd : possible_commands) {
					const uint32_t distance = levenshtein_distance(cmd_name, cmd.name);
					if(distance < closest_match.first) {
						closest_match.first = distance;
						closest_match.second = cmd;
					}
				}
				log_to_console(state, parent,
						"Did you mean \xA7Y" + std::string(closest_match.second.name) + "\xA7W (" + std::string(closest_match.second.desc) +
								")"
								"?");
			}
		} else {
			log_to_console(state, parent, "Here's some helpful commands ^-^");
			for(auto const& cmd : possible_commands)
				log_command_info(cmd);
		}
	} break;
	case command_info::type::show_stats:
	{
		if(!std::holds_alternative<std::string>(pstate.arg_slots[0])) {
			log_to_console(state, parent, "Valid options: demo(graphics), diplo(macy), eco(nomy), event(s), mil(itary)");
			log_to_console(state, parent, "tech(nology), pol(itics), a(ll)/all");
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
		auto const k = std::get<std::string>(pstate.arg_slots[0]);
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
			log_to_console(state, parent,
					"\x95\xA7Y"
					"National Identities\xA7W: " +
							std::to_string(state.world.national_identity_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Political Parties\xA7W: " +
							std::to_string(state.world.political_party_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Religions\xA7W: " +
							std::to_string(state.world.religion_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Culture Groups\xA7W: " +
							std::to_string(state.world.culture_group_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Cultures\xA7W: " +
							std::to_string(state.world.culture_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Culture Group Memberships\xA7W: " +
							std::to_string(state.world.culture_group_membership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Cultural Unions (Of)\xA7W: " +
							std::to_string(state.world.cultural_union_of_size()));
		}
		if((v & uint8_t(flags::economy)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Commodities\xA7W: " +
							std::to_string(state.world.commodity_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Modifiers\xA7W: " +
							std::to_string(state.world.modifier_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Factory Types\xA7W: " +
							std::to_string(state.world.factory_type_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Ideology Groups\xA7W: " +
							std::to_string(state.world.ideology_group_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Ideologies\xA7W: " +
							std::to_string(state.world.ideology_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Ideology Group Memberships\xA7W: " +
							std::to_string(state.world.ideology_group_membership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Issues\xA7W: " +
							std::to_string(state.world.issue_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Issue Options\xA7W: " +
							std::to_string(state.world.issue_option_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Reforms\xA7W: " +
							std::to_string(state.world.reform_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Reform Options\xA7W: " +
							std::to_string(state.world.reform_option_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"CB Types\xA7W: " +
							std::to_string(state.world.cb_type_size()));
		}
		if((v & uint8_t(flags::military)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Leader Traits\xA7W: " +
							std::to_string(state.world.leader_trait_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Pop Types\xA7W: " +
							std::to_string(state.world.pop_type_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Rebel Types\xA7W: " +
							std::to_string(state.world.rebel_type_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Provinces\xA7W: " +
							std::to_string(state.world.province_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Province Adjacenciess\xA7W: " +
							std::to_string(state.world.province_adjacency_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Nation Adjacencies\xA7W: " +
							std::to_string(state.world.nation_adjacency_size()));
		}
		if((v & uint8_t(flags::military)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Regiments\xA7W: " +
							std::to_string(state.world.regiment_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Ships\xA7W: " +
							std::to_string(state.world.ship_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Armies\xA7W: " +
							std::to_string(state.world.army_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Navies\xA7W: " +
							std::to_string(state.world.navy_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Army Controls\xA7W: " +
							std::to_string(state.world.army_control_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Army Locations\xA7W: " +
							std::to_string(state.world.army_location_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Army Memberships\xA7W: " +
							std::to_string(state.world.army_membership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Regiment Sources\xA7W: " +
							std::to_string(state.world.regiment_source_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Navy Controls\xA7W: " +
							std::to_string(state.world.navy_control_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Navy Locations\xA7W: " +
							std::to_string(state.world.navy_location_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Navy Memberships\xA7W: " +
							std::to_string(state.world.navy_membership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Leaders\xA7W: " +
							std::to_string(state.world.leader_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Army Leadership (leader<->army)\xA7W: " +
							std::to_string(state.world.army_leadership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Navy Leadership (leader<->navy)\xA7W: " +
							std::to_string(state.world.navy_leadership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Leader Loyalties (leader<->nation membership)\xA7W: " +
							std::to_string(state.world.leader_loyalty_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Wars\xA7W: " +
							std::to_string(state.world.war_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Wargoals\xA7W: " +
							std::to_string(state.world.wargoal_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"War Participants\xA7W: " +
							std::to_string(state.world.war_participant_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Wargoals Attached (wargoal<->war)\xA7W: " +
							std::to_string(state.world.wargoals_attached_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"State Definitions\xA7W: " +
							std::to_string(state.world.state_definition_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"State Instances\xA7W: " +
							std::to_string(state.world.state_instance_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Colonizations\xA7W: " +
							std::to_string(state.world.colonization_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"State Ownerships\xA7W: " +
							std::to_string(state.world.state_ownership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Abstract State Memberships\xA7W: " +
							std::to_string(state.world.abstract_state_membership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Cores\xA7W: " +
							std::to_string(state.world.core_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Identity Holders\xA7W: " +
							std::to_string(state.world.identity_holder_size()));
		}
		if((v & uint8_t(flags::technology)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Technologies\xA7W: " +
							std::to_string(state.world.technology_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Inventions\xA7W: " +
							std::to_string(state.world.invention_size()));
		}
		if((v & uint8_t(flags::diplomacy)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Nations\xA7W: " +
							std::to_string(state.world.nation_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Diplomatic Relations\xA7W: " +
							std::to_string(state.world.diplomatic_relation_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Unilateral Relationships\xA7W: " +
							std::to_string(state.world.unilateral_relationship_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"GP Relationships\xA7W: " +
							std::to_string(state.world.gp_relationship_size()));
		}
		if((v & uint8_t(flags::economy)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Factories\xA7W: " +
							std::to_string(state.world.factory_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Factory Locations\xA7W: " +
							std::to_string(state.world.factory_location_size()));
		}
		if((v & uint8_t(flags::politics)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Province Ownerships\xA7W: " +
							std::to_string(state.world.province_ownership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Province Controls\xA7W: " +
							std::to_string(state.world.province_control_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Province Rebel Controls\xA7W: " +
							std::to_string(state.world.province_rebel_control_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Overlords\xA7W: " +
							std::to_string(state.world.overlord_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Rebel Factions\xA7W: " +
							std::to_string(state.world.rebel_faction_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Rebellions Within\xA7W: " +
							std::to_string(state.world.rebellion_within_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Movements\xA7W: " +
							std::to_string(state.world.movement_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Movements Within\xA7W: " +
							std::to_string(state.world.movement_within_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Pop Movement Memberships\xA7W: " +
							std::to_string(state.world.pop_movement_membership_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Pop Rebellion Memberships\xA7W: " +
							std::to_string(state.world.pop_rebellion_membership_size()));
		}
		if((v & uint8_t(flags::demographics)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Pops\xA7W: " +
							std::to_string(state.world.pop_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Pop Locations\xA7W: " +
							std::to_string(state.world.pop_location_size()));
		}
		if((v & uint8_t(flags::events)) != 0) {
			log_to_console(state, parent,
					"\x95\xA7Y"
					"National Events\xA7W: " +
							std::to_string(state.world.national_event_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Provincial Events\xA7W: " +
							std::to_string(state.world.provincial_event_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Free National Events\xA7W: " +
							std::to_string(state.world.free_national_event_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Free Provincial Events\xA7W: " +
							std::to_string(state.world.free_provincial_event_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"National Focuses\xA7W: " +
							std::to_string(state.world.national_focus_size()));
			log_to_console(state, parent,
					"\x95\xA7Y"
					"Decisions\xA7W: " +
							std::to_string(state.world.decision_size()));
		}
	} break;
	case command_info::type::colour_guide:
		log_to_console(state, parent,
				"\xA7G"
				"\\xA7Y for Green.");
		log_to_console(state, parent,
				"\xA7R"
				"\\xA7Y for Red.");
		log_to_console(state, parent,
				"\xA7Y"
				"\\xA7Y for Yellow.");
		log_to_console(state, parent,
				"\xA7O"
				"\\xA7Y for Orange.");
		log_to_console(state, parent,
				"\xA7"
				"B"
				"\\xA7B for Blue.");
		log_to_console(state, parent,
				"\xA7g"
				"\\xA7g for Dark blue (*originally grey).");
		log_to_console(state, parent,
				"\xA7!"
				"\\xA7! for reseting to default.");
		log_to_console(state, parent,
				"\xA7W"
				"\\xA7b for Black.");
		log_to_console(state, parent,
				"\xA7W"
				"\\xA7W for White.");
		log_to_console(state, parent,
				"\xA7L"
				"\\xA7L for Lilac.");
		log_to_console(state, parent,
				"\xA7RRed\xA7GGreen\xA7"
				"B"
				"Blue");
		break;
	case command_info::type::diplomacy_points:
		command::c_change_diplo_points(state, state.local_player_nation, float(std::get<int32_t>(pstate.arg_slots[0])));
		break;
	case command_info::type::research_points:
		command::c_change_research_points(state, state.local_player_nation, float(std::get<int32_t>(pstate.arg_slots[0])));
		break;
	case command_info::type::money:
		command::c_change_money(state, state.local_player_nation, float(std::get<int32_t>(pstate.arg_slots[0])));
		break;
	case command_info::type::infamy:
		command::c_change_infamy(state, state.local_player_nation, float(std::get<int32_t>(pstate.arg_slots[0])));
		break;
	case command_info::type::cb_progress:
		command::c_change_cb_progress(state, state.local_player_nation, float(std::get<int32_t>(pstate.arg_slots[0])));
		break;
	case command_info::type::westernize:
		command::c_westernize(state, state.local_player_nation);
		break;
	case command_info::type::unwesternize:
		command::c_unwesternize(state, state.local_player_nation);
		break;
	case command_info::type::cheat:
		command::c_westernize(state, state.local_player_nation);
		command::c_change_cb_progress(state, state.local_player_nation, float(1000.f));
		command::c_change_research_points(state, state.local_player_nation, float(100000.f));
		command::c_change_diplo_points(state, state.local_player_nation, float(99.f));
		break;
	case command_info::type::crisis:
		command::c_force_crisis(state, state.local_player_nation);
		break;
	case command_info::type::end_game:
		command::c_end_game(state, state.local_player_nation);
		break;
	case command_info::type::event:
	{
		dcon::national_identity_id nid = state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
		if(std::holds_alternative<std::string>(pstate.arg_slots[1])) {
			auto tag = std::get<std::string>(pstate.arg_slots[1]);
			nid = smart_get_national_identity_from_tag(state, parent, tag);
		}
		if(nid)
			command::c_event_as(state, state.local_player_nation, state.world.national_identity_get_nation_from_identity_holder(nid), std::get<int32_t>(pstate.arg_slots[0]));
		else
			command::c_event(state, state.local_player_nation, std::get<int32_t>(pstate.arg_slots[0]));
	}
	break;
	case command_info::type::militancy:
		command::c_change_national_militancy(state, state.local_player_nation, float(std::get<int32_t>(pstate.arg_slots[0])));
		break;
	case command_info::type::prestige:
		command::c_change_prestige(state, state.local_player_nation, float(std::get<int32_t>(pstate.arg_slots[0])));
		break;
	case command_info::type::force_ally:
	{
		dcon::national_identity_id nid = state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
		if(std::holds_alternative<std::string>(pstate.arg_slots[1])) {
			auto tag = std::get<std::string>(pstate.arg_slots[1]);
			nid = smart_get_national_identity_from_tag(state, parent, tag);
			command::c_force_ally(state, state.local_player_nation, state.world.national_identity_get_nation_from_identity_holder(nid));
		}
	}
	break;
	case command_info::type::dump_out_of_sync:
		state.debug_save_oos_dump();
		state.debug_scenario_oos_dump();
		// Extneded data NOT included in normal dumps
		write_single_component(state, NATIVE("map_data.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::memcpy_serialize(ptr_in, state.map_state.map_data.size_x);
			ptr_in = sys::memcpy_serialize(ptr_in, state.map_state.map_data.size_y);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.river_vertices);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.river_starts);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.river_counts);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.coastal_vertices);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.coastal_starts);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.coastal_counts);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.border_vertices);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.borders);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.terrain_id_map);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.province_id_map);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.province_area);
			ptr_in = sys::serialize(ptr_in, state.map_state.map_data.diagonal_borders);
			return ptr_in;
		});
		write_single_component(state, NATIVE("defines.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			memcpy(ptr_in, &(state.defines), sizeof(parsing::defines));
			ptr_in += sizeof(parsing::defines);
			return ptr_in;
		});
		write_single_component(state, NATIVE("economy_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			memcpy(ptr_in, &(state.economy_definitions), sizeof(economy::global_economy_state));
			ptr_in += sizeof(economy::global_economy_state);
			return ptr_in;
		});
		write_single_component(state, NATIVE("party_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.culture_definitions.party_issues);
			return ptr_in;
		});
		write_single_component(state, NATIVE("political_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.culture_definitions.political_issues);
			return ptr_in;
		});
		write_single_component(state, NATIVE("social_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.culture_definitions.social_issues);
			return ptr_in;
		});
		write_single_component(state, NATIVE("military_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.culture_definitions.military_issues);
			return ptr_in;
		});
		write_single_component(state, NATIVE("economic_issues.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.culture_definitions.economic_issues);
			return ptr_in;
		});
		write_single_component(state, NATIVE("tech_folders.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.culture_definitions.tech_folders);
			return ptr_in;
		});
		write_single_component(state, NATIVE("crimes.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.culture_definitions.crimes);
			return ptr_in;
		});
		write_single_component(state, NATIVE("culture_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.artisans);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.capitalists);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.farmers);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.laborers);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.clergy);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.soldiers);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.officers);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.slaves);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.aristocrat);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.primary_factory_worker);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.secondary_factory_worker);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.officer_leadership_points);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.bureaucrat_tax_efficiency);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.conservative);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.jingoism);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.promotion_chance);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.demotion_chance);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.migration_chance);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.colonialmigration_chance);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.emigration_chance);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.assimilation_chance);
			ptr_in = sys::memcpy_serialize(ptr_in, state.culture_definitions.conversion_chance);
			return ptr_in;
		});
		write_single_component(state, NATIVE("unit_base_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.military_definitions.unit_base_definitions);
			return ptr_in;
		});
		write_single_component(state, NATIVE("military_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.first_background_trait);
			//ptr_in = sys::serialize(ptr_in, state.military_definitions.unit_base_definitions);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.base_army_unit);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.base_naval_unit);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.standard_civil_war);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.standard_great_war);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.standard_status_quo);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.liberate);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.uninstall_communist_gov);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.crisis_colony);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.crisis_liberate);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.irregular);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.infantry);
			ptr_in = sys::memcpy_serialize(ptr_in, state.military_definitions.artillery);
			return ptr_in;
		});
		write_single_component(state, NATIVE("national_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.national_definitions.flag_variable_names);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.global_flag_variable_names);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.variable_names);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.triggered_modifiers);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.rebel_id);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_easy_player);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.easy_player);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.hard_player);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_hard_player);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_easy_ai);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.easy_ai);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.hard_ai);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.very_hard_ai);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.overseas);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.coastal);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.non_coastal);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.coastal_sea);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.sea_zone);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.land_province);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.blockaded);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.no_adjacent_controlled);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.core);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.has_siege);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.occupied);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.nationalism);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.infrastructure);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.base_values);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.war);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.peace);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.disarming);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.war_exhaustion);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.badboy);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.debt_default_to);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.bad_debter);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.great_power);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.second_power);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.civ_nation);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.unciv_nation);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.average_literacy);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.plurality);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.generalised_debt_default);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.total_occupation);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.total_blockaded);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.in_bankrupcy);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_variables);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.num_allocated_national_flags);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.num_allocated_global_flags);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.flashpoint_focus);
			ptr_in = sys::memcpy_serialize(ptr_in, state.national_definitions.flashpoint_amount);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_yearly_pulse);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_quarterly_pulse);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_battle_won);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_battle_lost);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_surrender);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_new_great_nation);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_lost_great_nation);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_election_tick);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_colony_to_state);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_state_conquest);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_colony_to_state_free_slaves);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_debtor_default);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_debtor_default_small);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_debtor_default_second);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_civilize);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_my_factories_nationalized);
			ptr_in = sys::serialize(ptr_in, state.national_definitions.on_crisis_declare_interest);
			return ptr_in;
		});
		write_single_component(state, NATIVE("province_definitions.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.province_definitions.canals);
			ptr_in = sys::serialize(ptr_in, state.province_definitions.terrain_to_gfx_map);
			ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.first_sea_province);
			ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.europe);
			ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.asia);
			ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.africa);
			ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.north_america);
			ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.south_america);
			ptr_in = sys::memcpy_serialize(ptr_in, state.province_definitions.oceania);
			return ptr_in;
		});
		write_single_component(state, NATIVE("dates.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::memcpy_serialize(ptr_in, state.start_date);
			ptr_in = sys::memcpy_serialize(ptr_in, state.end_date);
			return ptr_in;
		});
		write_single_component(state, NATIVE("trigger_data.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.trigger_data);
			return ptr_in;
		});
		write_single_component(state, NATIVE("trigger_data_indices.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.trigger_data_indices);
			return ptr_in;
		});
		write_single_component(state, NATIVE("effect_data.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.effect_data);
			return ptr_in;
		});
		write_single_component(state, NATIVE("effect_data_indices.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.effect_data_indices);
			return ptr_in;
		});
		write_single_component(state, NATIVE("value_modifier_segments.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.value_modifier_segments);
			return ptr_in;
		});
		write_single_component(state, NATIVE("value_modifiers.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.value_modifiers);
			return ptr_in;
		});
		write_single_component(state, NATIVE("text_data.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.text_data);
			return ptr_in;
		});
		write_single_component(state, NATIVE("text_components.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.text_components);
			return ptr_in;
		});
		write_single_component(state, NATIVE("text_sequences.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.text_sequences);
			return ptr_in;
		});
		write_single_component(state, NATIVE("key_to_text_sequence.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.key_to_text_sequence);
			return ptr_in;
		});
		write_single_component(state, NATIVE("ui_defs_gfx.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.ui_defs.gfx);
			return ptr_in;
		});
		write_single_component(state, NATIVE("ui_defs_textures.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.ui_defs.textures);
			return ptr_in;
		});
		write_single_component(state, NATIVE("ui_defs_textures.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.ui_defs.textures);
			return ptr_in;
		});
		write_single_component(state, NATIVE("ui_defs_gui.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.ui_defs.gui);
			return ptr_in;
		});
		write_single_component(state, NATIVE("font_collection_font_names.bin"), [&](uint8_t* ptr_in, sys::state& state) -> uint8_t* {
			ptr_in = sys::serialize(ptr_in, state.font_collection.font_names);
			return ptr_in;
		});
		break;
	case command_info::type::fog_of_war:
		state.user_settings.fow_enabled = !state.user_settings.fow_enabled;
		state.map_state.map_data.update_fog_of_war(state);
		break;
	case command_info::type::win_wars:
		break;
	case command_info::type::toggle_ai:
		for(auto n : state.world.in_nation)
			command::c_toggle_ai(state, state.local_player_nation, n);
		break;
	case command_info::type::always_allow_wargoals:
		state.cheat_data.always_allow_wargoals = !state.cheat_data.always_allow_wargoals;
		break;
	case command_info::type::always_allow_reforms:
		state.cheat_data.always_allow_reforms = !state.cheat_data.always_allow_reforms;
		break;
	case command_info::type::complete_constructions:
		command::c_complete_constructions(state, state.local_player_nation);
		break;
	case command_info::type::instant_research:
		command::c_instant_research(state, state.local_player_nation);
		break;
	case command_info::type::always_accept_deals:
		state.cheat_data.always_accept_deals = !state.cheat_data.always_accept_deals;
		break;
	case command_info::type::game_info:
		log_to_console(state, parent, "Seed: " + std::to_string(state.game_seed));
		log_to_console(state, parent, std::string("Great Wars: ") + (state.military_definitions.great_wars_enabled ? "\x02" : "\x01"));
		log_to_console(state, parent, std::string("World Wars: ") + (state.military_definitions.world_wars_enabled ? "\x02" : "\x01"));
		break;
	case command_info::type::spectate:
		command::c_switch_nation(state, state.local_player_nation, state.world.nation_get_identity_from_identity_holder(state.national_definitions.rebel_id));
		break;
	case command_info::type::conquer_tag:
	{
		auto tag = std::get<std::string>(pstate.arg_slots[0]);
		if(tag == "ALL") {
			for(const auto p : state.world.in_province) {
				command::c_change_owner(state, state.local_player_nation, p, state.local_player_nation);
			}
		} else {
			auto nid = smart_get_national_identity_from_tag(state, parent, tag);
			if(nid) {
				auto n = state.world.national_identity_get_nation_from_identity_holder(nid);
				for(const auto po : state.world.in_province_ownership) {
					if(po.get_nation() == n)
						command::c_change_owner(state, state.local_player_nation, po.get_province(), state.local_player_nation);
				}
			}
		}
		break;
	}
	case command_info::type::change_control_and_owner:
	{
		auto province_id = dcon::province_id((uint16_t)std::get<std::int32_t>(pstate.arg_slots[0]));
		auto nid = state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
		if(std::holds_alternative<std::string>(pstate.arg_slots[1])) {
			auto tag = std::get<std::string>(pstate.arg_slots[1]);
			nid = smart_get_national_identity_from_tag(state, parent, tag);
		}
		command::c_change_owner(state, state.local_player_nation, province_id, state.world.national_identity_get_nation_from_identity_holder(nid));
		break;
	}
	case command_info::type::change_owner:
	{
		auto province_id = dcon::province_id((uint16_t)std::get<std::int32_t>(pstate.arg_slots[0]));
		auto nid = state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
		if(std::holds_alternative<std::string>(pstate.arg_slots[1])) {
			auto tag = std::get<std::string>(pstate.arg_slots[1]);
			nid = smart_get_national_identity_from_tag(state, parent, tag);
		}
		command::c_change_owner(state, state.local_player_nation, province_id, state.world.national_identity_get_nation_from_identity_holder(nid));
		break;
	}
	case command_info::type::change_control:
	{
		auto province_id = dcon::province_id((uint16_t)std::get<std::int32_t>(pstate.arg_slots[0]));
		auto nid = state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
		if(std::holds_alternative<std::string>(pstate.arg_slots[1])) {
			auto tag = std::get<std::string>(pstate.arg_slots[1]);
			nid = smart_get_national_identity_from_tag(state, parent, tag);
		}
		command::c_change_controller(state, state.local_player_nation, province_id, state.world.national_identity_get_nation_from_identity_holder(nid));
		break;
	}
	case command_info::type::province_id_tooltip:
	{
		state.cheat_data.show_province_id_tooltip = not state.cheat_data.show_province_id_tooltip;
		break;
	}
	case command_info::type::wasd:
	{
		state.cheat_data.wasd_move_cam = not state.cheat_data.wasd_move_cam;
		break;
	}
	case command_info::type::next_song:
	{
		sound::play_new_track(state);
		break;
	}
	case command_info::type::add_population:
	{
		auto ammount = std::get<std::int32_t>(pstate.arg_slots[0]);
		command::c_add_population(state, state.local_player_nation, ammount);
		break;
	}
	case command_info::type::instant_army:
	{
		command::c_instant_army(state, state.local_player_nation);
		break;
	}
	case command_info::type::instant_industry:
	{
		command::c_instant_industry(state, state.local_player_nation);
		break;
	}
	case command_info::type::none:
		log_to_console(state, parent, "Command \"" + std::string(s) + "\" not found.");
		break;
	default:
		break;
	}
	log_to_console(state, parent, ""); // space after command
	add_to_history(state, std::string(s));

	Cyto::Any payload = this;
	impl_get(state, payload);
}

void ui::console_text::render(sys::state& state, int32_t x, int32_t y) noexcept {
	float x_offs = 0.f;
	if(stored_text.length() > 0) {
		auto text_color = text::text_color::white;
		for(char const* start_text = stored_text.data(); start_text < stored_text.data() + stored_text.length();) {
			char const* end_text = start_text;
			for(; *end_text && *end_text != '\xA7'; ++end_text)
				;
			std::string_view text(start_text, end_text);
			if(!text.empty()) {
				std::string tmp_text{ text };
				ogl::render_text(state, tmp_text.c_str(), uint32_t(tmp_text.length()), ogl::color_modification::none,
						float(x + text_offset) + x_offs, float(y + base_data.data.text.border_size.y), get_text_color(text_color),
						base_data.data.button.font_handle);
				x_offs += state.font_collection.text_extent(state, tmp_text.c_str(), uint32_t(tmp_text.length()),
						base_data.data.text.font_handle);
			}
			if(uint8_t(*end_text) == 0xA7) {
				text_color = text::char_to_color(*++end_text); // Skip escape, then read colour
				++end_text;																		 // Skip colour
			}
			start_text = end_text;
		}
	}
}

void ui::console_window::show_toggle(sys::state& state) {
	assert(state.ui_state.console_window);
	state.ui_state.console_window->set_visible(state, !state.ui_state.console_window->is_visible());
	if(state.ui_state.console_window->is_visible())
		state.ui_state.root->move_child_to_front(state.ui_state.console_window);
}
