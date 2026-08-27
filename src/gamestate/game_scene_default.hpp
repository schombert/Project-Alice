#pragma once

#include "game_scene_constants.hpp"
#include "constants_ui.hpp"
#include "constants.hpp"
#include "container_types_ui.hpp"

namespace game_scene {

void do_nothing(sys::state& state);
ui::mouse_probe recalculate_mouse_probe_identity(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe);
void render_map_generic(sys::state& state);
void generic_map_scene_update(sys::state& state);
void highlight_given_province(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province);
void open_chat_during_game(sys::state& state);

struct scene_properties {
	scene_id id;

	std::function<ui::element_base* (sys::state& state)> get_root;

	bool starting_scene = false;
	bool final_scene = false;
	bool enforced_pause = false;
	bool based_on_map = true;
	bool overwrite_map_tooltip = false;
	bool accept_events = false;
	bool is_lobby = false;
	bool game_in_progress = true;

	borders_granularity borders = borders_granularity::province;

	std::function<void(sys::state& state, dcon::nation_id nation, dcon::province_id target, sys::key_modifiers mod)> rbutton_selected_units;
	std::function<void(sys::state& state, dcon::nation_id nation, dcon::province_id target, sys::key_modifiers mod)> rbutton_province;

	//drag related
	bool allow_drag_selection;
	std::function<void(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod)> on_drag_start;
	std::function<void(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod)> drag_selection;

	std::function<void(sys::state& state)> lbutton_up;
	std::function<void(sys::state& state)> on_province_selected;

	//key presses related
	std::function <sys::virtual_key(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod)> keycode_mapping;
	std::function <void(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod)> handle_hotkeys;

	//logger
	std::function <void(sys::state& state, std::string_view message)> console_log;

	//render
	std::function <void(sys::state& state)> render_ui = do_nothing;
	std::function <void(sys::state& state)> render_map = render_map_generic;

	//mouse probing
	std::function <ui::mouse_probe(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe)> recalculate_mouse_probe = recalculate_mouse_probe_identity;
	std::function <ui::mouse_probe(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe)> recalculate_tooltip_probe = recalculate_mouse_probe_identity;

	//clean up function which is supposed to keep ui "view" of the game state consistent
	std::function <void(sys::state& state)> clean_up = do_nothing;

	std::function <void(sys::state& state)> on_game_state_update = generic_map_scene_update;
	std::function <void(sys::state& state)> on_game_state_update_update_ui = do_nothing;

	//other functions:

	std::function <void(sys::state& state)> open_chat = open_chat_during_game;
	std::function <void(sys::state& state)> on_map_movement_stopped = do_nothing;

	// graphics
	std::function <void(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province)> update_highlight_texture = highlight_given_province;
};
}
