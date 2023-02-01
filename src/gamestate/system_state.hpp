#pragma once

#include <memory>
#include <stdint.h>

#include "constants.hpp"
#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "simple_fs.hpp"
#include "text.hpp"
#include "opengl_wrapper.hpp"
#include "fonts.hpp"
#include "sound.hpp"
#include "map.hpp"
#include "economy.hpp"
#include "culture.hpp"
#include "military.hpp"

// this header will eventually contain the highest-level objects
// that represent the overall state of the program
// it will also include the game state itself eventually as a member

namespace window {
	class window_data_impl;
}

namespace sys {

	enum class window_state : uint8_t {
		normal, maximized, minimized
	};


	struct user_settings_s {
		float ui_scale = 1.0f;
		float master_volume = 0.5f;
		float music_volume = 1.0f;
		float effects_volume = 1.0f;
		float interface_volume = 1.0f;
		bool prefer_fullscreen = false;

	};

	struct global_scenario_data_s { // this struct holds miscellaneous global properties of the scenario

	};

	struct alignas(64) state {
		// the state struct will eventually include (at least pointers to)
		// the state of the sound system, the state of the windowing system,
		// and the game data / state itself

		// utlimately it is the system struct that gets passed around everywhere
		// so that bits of the ui, for example, can control the overall state of
		// the game

		// implementation dependent data that needs to be stored for the window
		// subsystem

		dcon::data_container world;

		// scenario data

		economy::global_economy_state economy;
		culture::global_cultural_state culture;
		military::global_military_state military;

		std::vector<char> text_data; // stores string data in the win1250 codepage
		std::vector<text::text_component> text_components;
		tagged_vector<text::text_sequence, dcon::text_sequence_id> text_sequences;
		ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq> key_to_text_sequence;

		ui::definitions ui_defs; // definitions for graphics and ui

		// persistent user settings

		user_settings_s user_settings;

		// current program / ui state

		simple_fs::file_system common_fs; // file system for looking up graphics assets, etc
		std::unique_ptr<window::window_data_impl> win_ptr = nullptr; // platfom-dependent window information
		std::unique_ptr<sound::sound_impl> sound_ptr = nullptr; // platfom-dependent sound information
		ui::state ui_state; // transient information for the state of the ui
		text::font_manager font_collection;

		// common data for the window
		int32_t x_size = 0;
		int32_t y_size = 0;
		int32_t mouse_x_position = 0;
		int32_t mouse_y_position = 0;
		bool in_edit_control = false;
		bool is_dragging = false;

		// map data
		map::display_data map_display;

		// graphics data
		ogl::data open_gl;

		// the following functions will be invoked by the window subsystem

		void on_rbutton_down(int32_t x, int32_t y, key_modifiers mod);
		void on_mbutton_down(int32_t x, int32_t y, key_modifiers mod);
		void on_lbutton_down(int32_t x, int32_t y, key_modifiers mod);
		void on_rbutton_up(int32_t x, int32_t y, key_modifiers mod);
		void on_mbutton_up(int32_t x, int32_t y, key_modifiers mod);
		void on_lbutton_up(int32_t x, int32_t y, key_modifiers mod);
		void on_mouse_move(int32_t x, int32_t y, key_modifiers mod);
		void on_mouse_drag(int32_t x, int32_t y, key_modifiers mod); // called when the left button is held down
		void on_drag_finished(int32_t x, int32_t y, key_modifiers mod); // called when the left button is released after one or more drag events
		void on_resize(int32_t x, int32_t y, window_state win_state);
		void on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod, float amount); // an amount of 1.0 is one "click" of the wheel
		void on_key_down(virtual_key keycode, key_modifiers mod);
		void on_key_up(virtual_key keycode, key_modifiers mod);
		void on_text(char c); // c is win1250 codepage value
		void render(); // called to render the frame may (and should) delay returning until the frame is rendered, including waiting for vsync

		// the following function are for interacting with the string pool

		std::string_view to_string_view(dcon::text_key tag) const; // takes a stored tag and give you the text

		dcon::text_key add_to_pool(std::string const& text); // returns the newly added text
		dcon::text_key add_to_pool(std::string_view text);
		dcon::text_key add_to_pool_lowercase(std::string const& text); // these functions are as above, but force the text into lower case
		dcon::text_key add_to_pool_lowercase(std::string_view text);

		// searches the string pool for any existing string, appends if it is new
		// use this function sparingly; i.e. only when you think it is likely that
		// the text has already been added. Searching *all* the text may not be cheap
		dcon::text_key add_unique_to_pool(std::string const& text);

		state() : key_to_text_sequence(0, text::vector_backed_hash(text_data), text::vector_backed_eq(text_data)) {}
		~state();

		void save_user_settings() const;
		void load_user_settings();
		void update_ui_scale(float new_scale);

		void load_scenario_data(); // loads all scenario files other than map data
	};
}
