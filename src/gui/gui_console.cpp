#include <string>
#include <string_view>
#include <variant>
#include "gui_console.hpp"
#include "gui_fps_counter.hpp"
#include "nations.hpp"

struct command_info {
	static constexpr uint32_t max_arg_slots = 4;

	std::string_view name;
	enum class type : uint8_t { none = 0, reload, abort, clear_log, fps, set_tag, help, show_stats, colour_guide, cheat } mode = type::none;
	std::string_view desc;
	struct argument_info {
		std::string_view name;
		enum class type : uint8_t { none = 0, numeric, tag, text } mode = type::none;
		bool optional = false;
	} args[max_arg_slots] = {};
};

static const std::vector<command_info> possible_commands = {command_info{"none", command_info::type::none, "Dummy command", {command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"reload", command_info::type::reload, "Reloads Alice", {command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"abort", command_info::type::abort, "Abnormaly terminates execution", {command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"clr_log", command_info::type::clear_log, "Clears console logs", {command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"fps", command_info::type::fps, "Toggles FPS counter", {command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"tag", command_info::type::set_tag, "Set the current player's country", {command_info::argument_info{"country", command_info::argument_info::type::tag, false}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"help", command_info::type::help, "Display help", {command_info::argument_info{"cmd", command_info::argument_info::type::text, true}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"stats", command_info::type::show_stats, "Shows statistics of the current resources used", {command_info::argument_info{"type", command_info::argument_info::type::text, true}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"cheat", command_info::type::cheat, "Finishes all cassus bellis, adds 99 diplo points, instant research and westernizes (if not already)", {command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}},
	command_info{"colour", command_info::type::colour_guide, "An overview of available colours for complex text", {command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}, command_info::argument_info{}}}};

static uint32_t levenshtein_distance(std::string_view s1, std::string_view s2) {
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
static uint32_t levenshtein_tokenized_distance(std::string_view needle, std::string_view haystack) {
	assert(needle.find(" ") == std::string::npos);
	uint32_t dist = std::numeric_limits<uint32_t>::max();
	std::string str{haystack};
	size_t pos = 0;
	while((pos = str.find(" ")) != std::string::npos) {
		auto token = str.substr(0, pos);
		dist = std::min<uint32_t>(dist, levenshtein_distance(needle, token));
		str.erase(0, pos + 1);
	}
	return std::min<uint32_t>(dist, levenshtein_distance(needle, str));
}

static bool set_active_tag(sys::state& state, std::string_view tag) noexcept {
	bool found = false;
	state.world.for_each_national_identity([&](dcon::national_identity_id id) {
		auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
		if(curr == tag) {
			command::switch_nation(state, state.local_player_nation, id);
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
	std::variant< std::monostate, // none
		std::string,			  // tag/string
		int32_t					  // numeric
		>
		arg_slots[command_info::max_arg_slots] = {};
};
static parser_state parse_command(sys::state& state, std::string_view text) {
	std::string s{text};
	// Makes all text lowercase for proper processing
	std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return char(tolower(char(c))); });

	// Parse command
	parser_state pstate{};
	pstate.cmd = possible_commands[0];
	for(auto const& cmd : possible_commands)
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
		case command_info::argument_info::type::tag: {
			std::string tag{ident};
			std::transform(tag.begin(), tag.end(), tag.begin(), [](auto c) { return char(toupper(char(c))); });
			pstate.arg_slots[i] = tag;
			break;
		}
		case command_info::argument_info::type::numeric:
			pstate.arg_slots[i] = int32_t(std::stoi(std::string(ident)));
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
	float x_offs = state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), base_data.data.text.font_handle);
	if(lhs_suggestion.length() > 0) {
		char const* start_text = lhs_suggestion.data();
		char const* end_text = lhs_suggestion.data() + lhs_suggestion.length();
		std::string text(std::string_view(start_text, end_text));
		if(!text.empty()) {
			ogl::render_text(state, text.c_str(), uint32_t(text.length()), ogl::color_modification::none, float(x + text_offset) + x_offs, float(y + base_data.data.text.border_size.y), get_text_color(text::text_color::light_grey), base_data.data.button.font_handle);
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
			ogl::render_text(state, text.c_str(), uint32_t(text.length()), ogl::color_modification::none, float(x + text_offset) + x_offs, float(y + base_data.data.text.border_size.y), get_text_color(text::text_color::light_grey), base_data.data.button.font_handle);
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
		std::pair<uint32_t, command_info const*> closest_match = std::make_pair<uint32_t, command_info const*>(std::numeric_limits<uint32_t>::max(), &possible_commands[0]);
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
			// Now type in a suggestion...
			dcon::nation_id nid = state.world.identity_holder_get_nation(state.world.national_identity_get_identity_holder(closest_match.second));
			std::string name = nations::int_to_tag(state.world.national_identity_get_identifying_int(closest_match.second));
			if(tag.size() >= name.size()) {
				lhs_suggestion = std::string{};
			} else {
				lhs_suggestion = name.substr(tag.size());
			}
			rhs_suggestion = text::produce_simple_string(state, state.world.nation_get_name(nid)) + " - " + name;
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
				return;
			}
		} else if(pstate.cmd.args[i].mode == command_info::argument_info::type::tag) {
			if(!std::holds_alternative<std::string>(pstate.arg_slots[i])) {
				log_to_console(state, parent, "Command requires a \xA7Ytag\xA7W argument at " + std::to_string(i));
				return;
			}
		} else if(pstate.cmd.args[i].mode == command_info::argument_info::type::numeric) {
			if(!std::holds_alternative<int32_t>(pstate.arg_slots[i])) {
				log_to_console(state, parent, "Command requires a \xA7Ynumeric\xA7W argument at " + std::to_string(i));
				return;
			}
		}
	}
	switch(pstate.cmd.mode) {
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
	case command_info::type::set_tag: {
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
					"Tag could refer to \"\xA7Y" + nations::int_to_tag(fat_id.get_identifying_int()) + "\xA7W\" (\xA7Y" + text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + "\xA7W) Id #" + std::to_string(closest_tag_match.second.value));
			} else {
				auto fat_id = dcon::fatten(state.world, closest_name_match.second);
				log_to_console(state, parent,
					"Name could refer to \"\xA7Y" + nations::int_to_tag(fat_id.get_identifying_int()) + "\xA7W\" (\xA7Y" + text::produce_simple_string(state, fat_id.get_nation_from_identity_holder().get_name()) + "\xA7W) Id #" + std::to_string(closest_name_match.second.value));
			}

			if(tag.size() != 3)
				log_to_console(state, parent, "You need to use \xA7Ytags\xA7W (3-letters) instead of the full name");
			else
				log_to_console(state, parent, "Is this what you meant?");
		} else {
			log_to_console(state, parent, "Switching to \xA7Y" + std::string(tag) + "\xA7W");
		}
		state.game_state_updated.store(true, std::memory_order::release);
	} break;
	case command_info::type::help: {
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
	case command_info::type::show_stats: {
		if(!std::holds_alternative<std::string>(pstate.arg_slots[0])) {
			log_to_console(state, parent, "Valid options: demo(graphics), diplo(macy), eco(nomy), event(s), mil(itary)");
			log_to_console(state, parent, "tech(nology), pol(itics), a(ll)/all");
			log_to_console(state, parent, "Ex: \"stats pol\"");
			break;
		}
		enum class flags : uint8_t { none = 0x00, demographics = 0x01, diplomacy = 0x02, economy = 0x04, events = 0x08, military = 0x10, technology = 0x20, politics = 0x40, all = 0x7F, count };
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
	case command_info::type::cheat:
		log_to_console(state, parent, "You cheater >:(");
		
		break;
	// State changing events
	case command_info::type::none:
		log_to_console(state, parent, "Command \"" + std::string(s) + "\" not found.");
		break;
	default:
		break;
	}
	log_to_console(state, parent, ""); // space after command
	add_to_history(state, std::string(s));
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
				std::string tmp_text{text};
				ogl::render_text(state, tmp_text.c_str(), uint32_t(tmp_text.length()), ogl::color_modification::none, float(x + text_offset) + x_offs, float(y + base_data.data.text.border_size.y), get_text_color(text_color), base_data.data.button.font_handle);
				x_offs += state.font_collection.text_extent(state, tmp_text.c_str(), uint32_t(tmp_text.length()), base_data.data.text.font_handle);
			}
			if(uint8_t(*end_text) == 0xA7) {
				text_color = text::char_to_color(*++end_text); // Skip escape, then read colour
				++end_text;									   // Skip colour
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
