#include "gui_console.hpp"
#include "gui_fps_counter.hpp"
#include "nations.hpp"

enum class Command {
	NoMessage, Reload, Abort, ClearLog, FPS, SetTag, Help, Unknown
};
std::vector<std::string> possibleCommands{ "reload", "abort", "clr_log", "fps", "tag", "help" };

void set_active_tag(sys::state& state, std::string_view tag) noexcept {
	state.world.for_each_national_identity([&](dcon::national_identity_id id) {
		std::string curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
		if(curr == tag) {
			dcon::national_identity_fat_id fat_id = dcon::fatten(state.world, id);
			state.local_player_nation = fat_id.get_nation_from_identity_holder().id;
		}
	});
}

Command parse_command(std::string_view s) noexcept {
	if(s.empty()) return Command::NoMessage;
	if(s == "reload") return Command::Reload;
	if(s == "abort") return Command::Abort;
	if(s == "clr_log") return Command::ClearLog;
	if(s == "fps") return Command::FPS;
	if(s.starts_with("tag ") && s.size() == 7) return Command::SetTag;
	if(s == "help") return Command::Help;
	return Command::Unknown;
}

void log_to_console(sys::state& state, ui::element_base* parent, std::string_view s) noexcept {
	Cyto::Any output = std::string(s);
	parent->impl_get(state, output);
}

void ui::console_edit::edit_box_enter(sys::state& state, std::string_view s) noexcept {
	Command command = parse_command(s);
	if(command == Command::NoMessage) return;
	log_to_console(state, parent, s);

	switch(command) {
		case Command::Reload:
			log_to_console(state, parent, "Reloading...");
			state.map_display.load_map(state);
			break;
		case Command::Abort:
			log_to_console(state, parent, "Aborting...");
			std::abort();
			break;
		case Command::ClearLog:
			static_cast<console_window*>(parent)->clear_list(state);
			break;
		case Command::FPS:
			if(!state.ui_state.fps_counter) {
				auto fps_counter = make_element_by_type<fps_counter_text_box>(state, "fps_counter");
				state.ui_state.fps_counter = fps_counter.get();
				state.ui_state.root->add_child_to_front(std::move(fps_counter));
			} else {
				state.ui_state.fps_counter->set_visible(state, !state.ui_state.fps_counter->is_visible());
				state.ui_state.root->move_child_to_front(state.ui_state.fps_counter);
			}
			break;
		case Command::SetTag:
			log_to_console(state, parent, std::string("Switching to ") + std::string(s.substr(4)));
			set_active_tag(state, s.substr(4));
			state.game_state_updated.store(true, std::memory_order::release);
			break;
		case Command::Help:
			// TODO: `help {cmd}` should spit out something along the lines of "tag {country tag} - Switch active country". 
			log_to_console(state, parent, "+-ALICE CONSOLE COMMANDS-+");
			for(const std::string& cmd : possibleCommands) log_to_console(state, parent, cmd);
			log_to_console(state, parent, "+-ALICE CONSOLE COMMANDS-+");
			break;
		case Command::Unknown:
			log_to_console(state, parent, "Command not found.");
			break;
		case Command::NoMessage:
			break;
	}
	log_to_console(state, parent, ""); // space after command
}

void ui::console_window::show_toggle(sys::state& state) {
	assert(state.ui_state.console_window);
	state.ui_state.console_window->set_visible(state, !state.ui_state.console_window->is_visible());
	if(state.ui_state.console_window->is_visible()) state.ui_state.root->move_child_to_front(state.ui_state.console_window);
}
