#pragma once

#include "system_state.hpp"

namespace window {
struct creation_parameters {
	int32_t size_x = 1024;
	int32_t size_y = 768;
	sys::window_state initial_state = sys::window_state::maximized;
	bool borderless_fullscreen = false;
};

void create_window(sys::state& game_state,
                   creation_parameters const& params); // this function will not return until the window is closed or otherwise destroyed
void close_window(sys::state& game_state);             // close the main window
void set_borderless_full_screen(sys::state& game_state, bool fullscreen);
bool is_in_fullscreen(sys::state const& game_state);
bool is_key_depressed(sys::state const& game_state, sys::virtual_key key); // why not cheer it up then?

void emit_error_message(std::string const& content, bool fatal); // also terminates the program if fatal
} // namespace window
