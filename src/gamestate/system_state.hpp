#pragma once

#include <memory>
#include <stdint.h>
#include <atomic>
#include <chrono>

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
#include "nations.hpp"
#include "date_interface.hpp"
#include "defines.hpp"
#include "province.hpp"

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
		bool dummy1 = false;
		bool dummy2 = false;
		bool dummy3 = false;
		bool use_classic_fonts = false;

	};

	struct global_scenario_data_s { // this struct holds miscellaneous global properties of the scenario

	};

	struct crisis_member_def {
		dcon::nation_id id;
		bool supports_attacker = false;
	};
	enum class crisis_type : uint32_t {
		none = 0, claim = 1, liberation = 2, colonial = 3
	};

	struct alignas(64) state {
		// the state struct will eventually include (at least pointers to)
		// the state of the sound system, the state of the windowing system,
		// and the game data / state itself

		// ultimately it is the system struct that gets passed around everywhere
		// so that bits of the ui, for example, can control the overall state of
		// the game

		// implementation dependent data that needs to be stored for the window
		// subsystem

		dcon::data_container world;

		// scenario data

		parsing::defines defines;

		economy::global_economy_state economy_definitions;
		culture::global_cultural_state culture_definitions;
		military::global_military_state military_definitions;
		nations::global_national_state national_definitions;
		province::global_provincial_state province_definitions;

		absolute_time_point start_date;
		absolute_time_point end_date;

		std::vector<uint16_t> trigger_data;
		std::vector<uint16_t> effect_data;
		std::vector<value_modifier_segment> value_modifier_segments;
		tagged_vector<value_modifier_description, dcon::value_modifier_key> value_modifiers;

		std::vector<char> text_data; // stores string data in the win1250 codepage
		std::vector<text::text_component> text_components;
		tagged_vector<text::text_sequence, dcon::text_sequence_id> text_sequences;
		ankerl::unordered_dense::map<dcon::text_key, dcon::text_sequence_id, text::vector_backed_hash, text::vector_backed_eq> key_to_text_sequence;

		bool adjacency_data_out_of_date = true;
		std::vector<dcon::nation_id> nations_by_rank;
		std::vector<dcon::nation_id> nations_by_industrial_score;
		std::vector<dcon::nation_id> nations_by_military_score;
		std::vector<dcon::nation_id> nations_by_prestige_score;

		dcon::state_instance_id crisis_state;
		std::vector<crisis_member_def> crisis_participants;
		crisis_type current_crisis = crisis_type::none;
		float crisis_temperature = 0;
		dcon::nation_id primary_crisis_attacker;
		dcon::nation_id primary_crisis_defender;

		std::vector<char> unit_names; // a second text buffer, this time for just the unit names
		                              // why a second text buffer? Partly because unit names don't need the extra redirection possibilities of
									  // ordinary game text, partly because I envision the possibility that we may stick dynamic names into this
		                              // We also may push this into the save game if we handle unit renaming using this

		ui::definitions ui_defs; // definitions for graphics and ui

		std::vector<uint8_t> flag_type_map; // flag_type remapper for saving space while also allowing
		                                    // mods to add flags not present in vanilla
		std::vector<culture::flag_type> flag_types; // List of unique flag types

		// persistent user settings

		user_settings_s user_settings;

		// current program / ui state

		dcon::nation_id local_player_nation;
		sys::date current_date = sys::date{0};

		simple_fs::file_system common_fs; // file system for looking up graphics assets, etc
		std::unique_ptr<window::window_data_impl> win_ptr = nullptr; // platform-dependent window information
		std::unique_ptr<sound::sound_impl> sound_ptr = nullptr; // platform-dependent sound information
		ui::state ui_state; // transient information for the state of the ui
		text::font_manager font_collection;

		// synchronization data (between main update logic and ui thread)
		std::atomic<bool> game_state_updated = false; // game state -> ui signal
		std::atomic<bool> quit_signaled = false; // ui -> game state signal
		std::atomic<int32_t> actual_game_speed = 0; // ui -> game state message

		// internal game timer / update logic
		std::chrono::time_point<std::chrono::steady_clock> last_update = std::chrono::steady_clock::now();
		bool internally_paused = false; // should NOT be set from the ui context (but may be read)

		// common data for the window
		int32_t x_size = 0;
		int32_t y_size = 0;
		int32_t mouse_x_position = 0;
		int32_t mouse_y_position = 0;
		bool is_dragging = false;

		// map data
		map::display_data map_display;

		// graphics data
		ogl::data open_gl;

		// the following functions will be invoked by the window subsystem

		void on_create(); // called once after the window is created and opengl is ready
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

		// this function runs the internal logic of the game. It will return *only* after a quit notification is sent to it

		void game_loop();

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

		dcon::unit_name_id add_unit_name(std::string_view text); // returns the newly added text
		std::string_view to_string_view(dcon::unit_name_id tag) const; // takes a stored tag and give you the text

		dcon::trigger_key commit_trigger_data(std::vector<uint16_t> data);
		dcon::effect_key commit_effect_data(std::vector<uint16_t> data);

		state() : key_to_text_sequence(0, text::vector_backed_hash(text_data), text::vector_backed_eq(text_data)) {}
		~state();

		void save_user_settings() const;
		void load_user_settings();
		void update_ui_scale(float new_scale);

		void load_scenario_data(); // loads all scenario files other than map data
		void fill_unsaved_data(); // reconstructs derived values that are not directly saved after a save has been loaded

		void open_diplomacy(dcon::nation_id target);  // Open the diplomacy window with target selected
	};
}
