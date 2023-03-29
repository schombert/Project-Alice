#include "gui_console.hpp"
#include "gui_fps_counter.hpp"
#include "nations.hpp"

enum class Command {
	NoMessage,
	Reload,
	Abort,
	ClearLog,
	FPS,
	SetTag,
	Unknown
};


void set_active_tag(sys::state& state, std::string_view tag) noexcept {
    state.world.for_each_national_identity([&](dcon::national_identity_id id) {
        std::string curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
        if(curr == tag) {
            dcon::national_identity_fat_id fat_id = dcon::fatten(state.world, id);
            auto nat_id = fat_id.get_nation_from_identity_holder().id;
            state.local_player_nation = nat_id;
        }
    });
}

Command parse_command(std::string_view s) noexcept {
	if(s.empty()) {
		return Command::NoMessage;
	} else if(s == "reload") {
		return Command::Reload;
	} else if(s == "abort") {
		return Command::Abort;
	} else if(s == "clr_log") {
		return Command::ClearLog;
	} else if(s == "fps") {
		return Command::FPS;
	} else if(s.starts_with("tag ") && s.size() == 7) {
		return Command::SetTag;
	} else {
		return Command::Unknown;
	}
}

void log_to_console(sys::state& state, ui::element_base* parent, std::string_view s) noexcept {
	Cyto::Any output = std::string(s);
	parent->impl_get(state, output);
}
void clear_console(sys::state& state, ui::element_base* parent) noexcept {
	// TODO: Implement this. Idk how to remove all children at once.
}


void ui::console_edit::edit_box_enter(sys::state& state, std::string_view s) noexcept {
	Command command = parse_command(s);
	if(command == Command::NoMessage) {
		return;
	}

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
			clear_console(state, parent);
			break;
		case Command::FPS:
			if(!state.ui_state.fps_counter) {
				auto fps_counter = make_element_by_type<fps_counter_text_box>(state, "fps_counter");
				state.ui_state.fps_counter = fps_counter.get();
				state.ui_state.root->add_child_to_front(std::move(fps_counter));
			} else if(state.ui_state.fps_counter->is_visible()) {
				state.ui_state.fps_counter->set_visible(state, false);
			} else {
				state.ui_state.fps_counter->set_visible(state, true);
				state.ui_state.root->move_child_to_front(state.ui_state.fps_counter);
			}
			break;
		case Command::SetTag:
			// TODO: Verify tag exists. if it doesn't, log error to console using log_to_console.
			log_to_console(state, parent, std::string("Switching to ")+std::string(s.substr(4)));
			set_active_tag(state, s.substr(4));
			state.game_state_updated.store(true, std::memory_order::release);
			break;
		case Command::Unknown:
			log_to_console(state, parent, "Command not found.");
			// TODO: Make this red.
			break;
	}
	// Break-line to add space between commands
	log_to_console(state, parent, "");
}

void ui::console_window::show_toggle(sys::state& state) {
	assert(state.ui_state.console_window);
	if(state.ui_state.console_window->is_visible()) {
		state.ui_state.console_window->set_visible(state, false);
		// TODO: Remove focus on console once toggled off--not sure how to do this yet.
    } else {
        state.ui_state.console_window->set_visible(state, true);
        state.ui_state.root->move_child_to_front(state.ui_state.console_window);
    }
}
