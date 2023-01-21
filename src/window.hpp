#pragma once

#include "system_state.hpp"

namespace window {
	struct creation_parameters {
		int32_t size_x = 600;
		int32_t size_y = 400;
		sys::window_state intitial_state = sys::window_state::maximized;
		bool borderless_fullscreen = false;
	};

	void create_window(sys::state& game_state, creation_parameters const& params); // this function will not return until the window is closed or otherwise destroyed
	void close_window(sys::state& game_state); // close the main window
	void set_borderless_full_screen(sys::state& game_state, bool fullscreen);
	bool is_in_fullscreen(sys::state const& game_state);
	bool is_key_depressed(sys::state const& game_state, sys::virtual_key key); // why not cheer it up then?
}
