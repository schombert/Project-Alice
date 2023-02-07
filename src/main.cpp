#ifdef LOCAL_USER_SETTINGS
#include "local_user_settings.hpp"
#endif
#include "parsers.cpp"
#include "defines.cpp"
#include "float_from_chars.cpp"
#include "system_state.cpp"
#include "gui_graphics_parsers.cpp"
#include "text.cpp"
#include "fonts.cpp"
#include "texture.cpp"
#include "gui_graphics.cpp"
#include "gui_element_types.cpp"
#include "gui_main_menu.cpp"
#include "gui_console.cpp"
#include "nations_parsing.cpp"
#include "cultures_parsing.cpp"
#include "econ_parsing.cpp"
#include "military_parsing.cpp"
#include "date_interface.cpp"
#include "provinces_parsing.cpp"

#ifdef _WIN64
// WINDOWS implementations go here

#include "simple_fs_win.cpp"
#include "window_win.cpp"
#include "sound_win.cpp"
#include "opengl_wrapper_win.cpp"

#ifndef ALICE_NO_ENTRY_POINT
#include "entry_point_win.cpp"
#endif

#else
// LINUX implementations go here

#include "simple_fs_nix.cpp"
#include "window_nix.cpp"
#include "sound_nix.cpp"
#include "opengl_wrapper_nix.cpp"

#ifndef ALICE_NO_ENTRY_POINT
#include "entry_point_nix.cpp"
#endif

#endif

#include "opengl_wrapper.cpp"
#include "map.cpp"
#include "map_modes.cpp"

namespace sys {
	state::~state() {
		// why does this exist ? So that the destructor of the unique pointer doesn't have to be known before it is implemented
	}
}
