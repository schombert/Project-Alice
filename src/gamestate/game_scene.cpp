#include "gui_unit_grid_box.hpp"
#include "gui_unit_panel.hpp"
#include "gui_province_window.hpp"
#include "gui_land_combat.hpp"
#include "gui_console.hpp"
#include "gui_chat_window.hpp"
#include "gui_event.hpp"
#include "gui_map_icons.hpp"
#include "simple_fs.hpp"

namespace game_scene {

void switch_scene(sys::state& state, scene_id ui_scene) {
	/*
	if (state.ui_state.end_screen)
		state.ui_state.end_screen->set_visible(state, false);
	if(state.ui_state.nation_picker)
		state.ui_state.nation_picker->set_visible(state, false);
	if(state.ui_state.root)
		state.ui_state.root->set_visible(state, false);
	if(state.ui_state.select_states_legend)
		state.ui_state.select_states_legend->set_visible(state, false);
	if(state.ui_state.military_root)
		state.ui_state.military_root->set_visible(state, false);

	state.get_root_element()->set_visible(state, true);
	*/

	switch(ui_scene) {
	case scene_id::in_game_state_selector:
		state.current_scene = state_wargoal_selector();

		state.stored_map_mode = state.map_state.active_map_mode;
		map_mode::set_map_mode(state, map_mode::mode::state_select);
		state.set_selected_province(dcon::province_id{});

		return;

	case scene_id::in_game_basic:
		if(state.current_scene.id == scene_id::in_game_state_selector) {
			state.state_selection.reset();
			map_mode::set_map_mode(state, state.stored_map_mode);
		}

		state.current_scene = basic_game();

		return;

	case scene_id::in_game_economy_viewer:
		state.current_scene = economy_viewer_scene();

		return;

	case scene_id::in_game_military:
		state.current_scene = battleplan_editor();

		return;

	case scene_id::end_screen:
		state.current_scene = end_screen();

		return;

	case scene_id::pick_nation:
		state.current_scene = nation_picker();

		return;

	case scene_id::in_game_military_selector:
		state.current_scene = battleplan_editor_add_army();

		return;
	case scene_id::count: // this should never happen
		assert(false);
		return;
	}
	
	state.game_state_updated.store(true, std::memory_order_release);
}

void do_nothing_province_target(sys::state& state,
		dcon::nation_id nation,
		dcon::province_id target,
		sys::key_modifiers mod) { }
void do_nothing(sys::state& state) { }
void do_nothing_screen(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) { }

bool belongs_on_map(sys::state& state, ui::element_base* checked_element) {
	while(checked_element != nullptr) {
		if(checked_element == state.ui_state.units_root.get())
			return true;
		if(checked_element == state.ui_state.unit_details_box.get())
			return true;
		checked_element = checked_element->parent;
	}
	return false;
}

map::map_view get_view(sys::state& state) {
	auto current_view = map::map_view::globe;
	if(state.user_settings.map_is_globe == sys::projection_mode::flat) {
		current_view = map::map_view::flat;
	} else if(state.user_settings.map_is_globe == sys::projection_mode::globe_perpect) {
		current_view = map::map_view::globe_perspect;
	}

	return current_view;
}

float get_effects_volume(sys::state& state) {
	return state.user_settings.effects_volume * state.user_settings.master_volume;
}

void selected_units_control(
	sys::state& state,
	dcon::nation_id nation,
	dcon::province_id target,
	sys::key_modifiers mod
) {
	bool fail = false;
	bool army_play = false;
	//as opposed to queueing
	bool reset_orders = (uint8_t(mod) & uint8_t(sys::key_modifiers::modifiers_shift)) == 0;

	float volume = get_effects_volume(state);
	if(reset_orders) {
		for(auto a : state.selected_armies) {
			if(command::can_move_or_stop_army(state, state.local_player_nation, a, target)) {
				command::move_or_stop_army(state, state.local_player_nation, a, target, state.ui_state.selected_army_order);
				army_play = true;
			}
			else {
				fail = true;
			}
		}
		for(auto a : state.selected_navies) {
			if(command::can_move_retreat_or_stop_navy(state, state.local_player_nation, a, target)) {
				command::move_retreat_or_stop_navy(state, state.local_player_nation, a, target);
			} else {
				fail = true;
			}
		}
	}
	else {
		for(auto a : state.selected_armies) {
			if(command::can_move_army(state, state.local_player_nation, a, target, false).empty()) {
				fail = true;
			} else {
				command::move_army(state, state.local_player_nation, a, target, false, state.ui_state.selected_army_order);
				army_play = true;
			}
		}
		for(auto a : state.selected_navies) {
			if(command::can_move_navy(state, state.local_player_nation, a, target, false).empty()) {
				fail = true;
			} else {
				command::move_navy(state, state.local_player_nation, a, target, false);
			}
		}
	}

	if(!fail) {
		if(army_play) {
			sound::play_effect(state, sound::get_army_move_sound(state), volume);
		} else {
			sound::play_effect(state, sound::get_navy_move_sound(state), volume);
		}
	} else {
		sound::play_effect(state, sound::get_error_sound(state), volume);
	}
}

void open_diplomacy(
	sys::state& state,
	dcon::nation_id nation,
	dcon::province_id target,
	sys::key_modifiers mod
) {
	auto owner = state.world.province_get_nation_from_province_ownership(target);
	if(owner) {
		state.open_diplomacy(owner);
	} else {
		state.open_diplomacy(nation);
	}
}

void select_player_nation_from_selected_province(sys::state& state) {
	auto owner = state.world.province_get_nation_from_province_ownership(state.map_state.selected_province);
	if(owner) {
		// On single player we simply set the local player nation
		// on multiplayer we wait until we get a confirmation that we are
		// allowed to pick the specified nation as no two players can get on
		// a nation, at the moment
		// Co-op should work
		if(state.network_mode == sys::network_mode_type::single_player) {
			if(state.local_player_nation) {
				state.world.nation_set_is_player_controlled(state.local_player_nation, false);
			}
			state.local_player_nation = owner;
			state.world.nation_set_is_player_controlled(state.local_player_nation, true);
			if(state.ui_state.nation_picker) {
				state.ui_state.nation_picker->impl_on_update(state);
			}
		} else if(command::can_notify_player_picks_nation(state, state.local_player_nation, owner, state.network_state.nickname)) {
			command::notify_player_picks_nation(state, state.local_player_nation, owner, state.network_state.nickname);
		}
	}
}

void select_wargoal_state_from_selected_province(sys::state& state) {
	auto prov = state.map_state.selected_province;
	auto sdef = state.world.province_get_state_from_abstract_state_membership(prov);
	state.state_select(sdef);
}

void send_selected_province_to_province_window(sys::state& state) {
	if(state.ui_state.province_window) {
		static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, state.map_state.selected_province);
	}
}

void start_dragging(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	state.x_drag_start = x;
	state.y_drag_start = y;
	state.drag_selecting = true;
	window::change_cursor(state, window::cursor_type::drag_select);
}

void stop_dragging(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	state.is_dragging = false;
	if(state.ui_state.drag_target) {
		state.on_drag_finished(x, y, mod);
	}
}

void render_map_generic(sys::state& state) {
	state.map_state.render(state, state.x_size, state.y_size);
}

void ui_rbutton(sys::state& state, sys::key_modifiers mod) {
	state.ui_state.under_mouse->impl_on_rbutton_down(
		state,
		state.ui_state.relative_mouse_location.x,
		state.ui_state.relative_mouse_location.y,
		mod
	);
}
void ui_lbutton(sys::state& state, sys::key_modifiers mod) {
	auto result = state.ui_state.under_mouse->impl_on_lbutton_down(
		state,
		state.ui_state.relative_mouse_location.x,
		state.ui_state.relative_mouse_location.y,
		mod
	);
	if(result != ui::message_result::unseen) {
		state.ui_state.set_focus_target(state, state.ui_state.under_mouse);
	}
	state.ui_state.left_mouse_hold_target = state.ui_state.under_mouse;
}

void on_rbutton_down_map(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	state.map_state.on_rbutton_down(state, x, y, state.x_size, state.y_size, mod);
}

void on_lbutton_down_map(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	state.map_state.on_lbutton_down(state, x, y, state.x_size, state.y_size, mod);
}

void on_lbutton_up_map(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	state.map_state.on_lbutton_up(state, x, y, state.x_size, state.y_size, mod);
}

void handle_rbutton_down_map_interaction(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	auto current_view = get_view(state);
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(state.x_size, state.y_size);
	glm::vec2 map_pos;
	if(!state.map_state.screen_to_map(mouse_pos, screen_size, current_view, map_pos)) {
		return;
	}
	map_pos *= glm::vec2(float(state.map_state.map_data.size_x), float(state.map_state.map_data.size_y));
	auto idx = int32_t(state.map_state.map_data.size_y - map_pos.y) * int32_t(state.map_state.map_data.size_x) + int32_t(map_pos.x);

	if(0 <= idx && size_t(idx) < state.map_state.map_data.province_id_map.size()) {
		sound::play_interface_sound(state, sound::get_random_province_select_sound(state), get_effects_volume(state));
		auto id = province::from_map_id(state.map_state.map_data.province_id_map[idx]);

		if(state.selected_armies.size() > 0 || state.selected_navies.size() > 0) {
			state.current_scene.rbutton_selected_units(state, state.local_player_nation, id, mod);
		} else {
			state.current_scene.rbutton_province(state, state.local_player_nation, id, mod);
		}
	}

	on_rbutton_down_map(state, x, y, mod);
}

void handle_lbutton_down_map_interaction(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	on_lbutton_down_map(state, x, y, mod);
	state.current_scene.on_drag_start(state, x, y, mod);
}

void on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	// Lose focus on text
	state.ui_state.set_focus_target(state, nullptr);

	if(state.iui_state.over_ui) {
		return;
	}

	bool under_mouse_belongs_on_map = belongs_on_map(state, state.ui_state.under_mouse);

	//if we clicked on UI element, handle it
	if(state.ui_state.under_mouse != nullptr && !under_mouse_belongs_on_map) {
		ui_rbutton(state, mod);
		return;
	}
	// otherwise, we clicked on the map and need to handle map clicking logic;
	handle_rbutton_down_map_interaction(state, x, y, mod);
}

void on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	// Lose focus on text

	if(state.iui_state.over_ui) {
		return;
	}

	if(state.ui_state.under_mouse != nullptr) {
		ui_lbutton(state, mod);
		return;
	}

	state.ui_state.set_focus_target(state, nullptr);
	handle_lbutton_down_map_interaction(state, x, y, mod);
}

void on_lbutton_up_ui_click_hold_and_release(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	auto rel_x = state.ui_state.relative_mouse_location.x;
	auto rel_y = state.ui_state.relative_mouse_location.y;

	bool mouse_over_target = state.ui_state.under_mouse == state.ui_state.left_mouse_hold_target;

	if(!state.current_scene.allow_drag_selection) {
		if(mouse_over_target) {
			state.ui_state.under_mouse->impl_on_lbutton_up(state, rel_x, rel_y, mod, true);
		} else {
			state.ui_state.left_mouse_hold_target->impl_on_lbutton_up(state, rel_x, rel_y, mod, false);
		}
	} else {
		if(mouse_over_target) {
			state.ui_state.left_mouse_hold_target = nullptr;
			state.ui_state.under_mouse->impl_on_lbutton_up(state, rel_x, rel_y, mod, true);
		} else if(!mouse_over_target && !state.drag_selecting) {
			state.ui_state.left_mouse_hold_target->impl_on_lbutton_up(state, rel_x, rel_y, mod, false);
			ui::element_base* temp_hold_target = state.ui_state.left_mouse_hold_target;
			state.ui_state.left_mouse_hold_target = nullptr;
			if(state.ui_state.scrollbar_continuous_movement) {
				Cyto::Any payload = ui::scrollbar_settings{};
				temp_hold_target->impl_set(state, payload);
				state.ui_state.scrollbar_continuous_movement = false;
			}
		}
	}
}

// Should be called only from the UI thread
void deselect_units(sys::state& state) {
	state.selected_armies.clear();
	state.selected_navies.clear();
}

bool province_mid_point_is_in_selection(sys::state& state, int32_t x, int32_t y, dcon::province_id province) {
	auto mid_point = state.world.province_get_mid_point(province);
	auto map_pos = state.map_state.normalize_map_coord(mid_point);
	auto screen_size = glm::vec2{ float(state.x_size), float(state.y_size) };
	glm::vec2 screen_pos;
	if(state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos, { 5.f, 5.f })) {
		if(state.x_drag_start <= int32_t(screen_pos.x) && int32_t(screen_pos.x) <= x
			&& state.y_drag_start <= int32_t(screen_pos.y) && int32_t(screen_pos.y) <= y) {
			return true;
		}
	}
	return false;
}

bool province_port_is_in_selection(sys::state& state, int32_t x, int32_t y, dcon::province_id province) {
	auto port_to = state.world.province_get_port_to(province);
	auto adj = state.world.get_province_adjacency_by_province_pair(province, port_to);
	if(adj) {
		auto id = adj.index();
		auto& border = state.map_state.map_data.borders[id];
		auto& vertex = state.map_state.map_data.border_vertices[border.start_index + border.count / 4];

		auto map_x = vertex.position.x;
		auto map_y = vertex.position.y;

		glm::vec2 map_pos(map_x, 1.0f - map_y);
		auto screen_size = glm::vec2{ float(state.x_size), float(state.y_size) };
		glm::vec2 screen_pos;
		if(state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos, { 5.f, 5.f })) {
			if(state.x_drag_start <= int32_t(screen_pos.x)
				&& int32_t(screen_pos.x) <= x
				&& state.y_drag_start <= int32_t(screen_pos.y)
				&& int32_t(screen_pos.y) <= y
			) {
				return true;
			}
		}
	}
	return false;
}

bool army_is_in_selection(sys::state& state, int32_t x, int32_t y, dcon::army_id a) {
	auto province = state.world.army_get_location_from_army_location(a);
	return province_mid_point_is_in_selection(state, x, y, province);
}

bool navy_is_in_selection(sys::state& state, int32_t x, int32_t y, dcon::navy_id n) {
	auto loc = state.world.navy_get_location_from_navy_location(n);
	if(loc.index() >= state.province_definitions.first_sea_province.index()) {
		if(province_mid_point_is_in_selection(state, x, y, loc)) {
			return true;
		}
	} else {
		if(province_port_is_in_selection(state, x, y, loc)) {
			return true;
		}
	}
	return false;
}

void select_units(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	if((int32_t(sys::key_modifiers::modifiers_shift) & int32_t(mod)) == 0) {
		deselect_units(state);
	}
	// Hide selected province
	if(state.ui_state.province_window) {
		state.ui_state.province_window->set_visible(state, false);
		state.set_selected_province(dcon::province_id{}); //ensure we deselect from map too
	}
	if((int32_t(sys::key_modifiers::modifiers_ctrl) & int32_t(mod)) == 0) {
		for(auto a : state.world.in_army) {
			if(a.is_valid() && !a.get_navy_from_army_transport() && military::get_effective_unit_commander(state, a) == state.local_player_nation) {
				if(army_is_in_selection(state, x, y, a)) {
					state.select(a);
				}
			}
		}
	}
	for(auto a : state.world.in_navy) {
		if(a.is_valid() && military::get_effective_unit_commander(state, a) == state.local_player_nation) {
			if(navy_is_in_selection(state, x, y, a)) {
				state.select(a);
			}
		}
	}
	if(!state.selected_armies.empty() && !state.selected_navies.empty()) {
		state.selected_navies.clear();
	}
	// Play selection sound effect
	if(!state.selected_armies.empty()) {
		sound::play_effect(state, sound::get_army_select_sound(state), get_effects_volume(state));
	} else {
		sound::play_effect(state, sound::get_navy_select_sound(state), get_effects_volume(state));
	}
	state.game_state_updated.store(true, std::memory_order_release);
}

void handle_drag_stop(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	bool insignificant_movement =
		std::abs(x - state.x_drag_start) <= int32_t(std::ceil(state.x_size * 0.0025))
		&& std::abs(y - state.y_drag_start) <= int32_t(std::ceil(state.x_size * 0.0025));

	state.ui_state.scrollbar_timer = 0;
	if(state.ui_state.under_mouse != nullptr || !state.drag_selecting) {
		state.drag_selecting = false;
		if(state.ui_state.edit_target_internal == nullptr)
			window::change_cursor(state, window::cursor_type::normal);
	} else if(insignificant_movement) {
		// we assume that user wanted to click
		state.drag_selecting = false;
		window::change_cursor(state, window::cursor_type::normal);
		send_selected_province_to_province_window(state);
		deselect_units(state);
		state.game_state_updated.store(true, std::memory_order_release);
	} else {
		// stop dragging and select units
		state.drag_selecting = false;
		window::change_cursor(state, window::cursor_type::normal);
		if(x < state.x_drag_start)
			std::swap(x, state.x_drag_start);
		if(y < state.y_drag_start)
			std::swap(y, state.y_drag_start);

		state.current_scene.drag_selection(state, x, y, mod);
	}
}

void on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mod) {
	stop_dragging(state, x, y, mod);
	if(state.user_settings.left_mouse_click_hold_and_release && state.ui_state.left_mouse_hold_target) {
		on_lbutton_up_ui_click_hold_and_release(state, x, y, mod);
	}

	on_lbutton_up_map(state, x, y, mod);

	if(!state.ui_state.under_mouse && !state.iui_state.over_ui) {
		state.current_scene.lbutton_up(state);
	}

	// if we were holding some "button" and this scene doesn't allow drag selection, then we can safely return
	if(state.user_settings.left_mouse_click_hold_and_release
		&& state.ui_state.left_mouse_hold_target
		&& !state.current_scene.allow_drag_selection
	) {
		return;
	}

	state.ui_state.scrollbar_timer = 0;
	handle_drag_stop(state, x, y, mod);
}

sys::virtual_key replace_keycodes_map_movement(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	//Emulating autohotkey
	if(!state.ui_state.edit_target_internal && state.user_settings.wasd_for_map_movement) {
		if(keycode == sys::virtual_key::W)
			return sys::virtual_key::UP;
		else if(keycode == sys::virtual_key::A)
			return sys::virtual_key::LEFT;
		else if(keycode == sys::virtual_key::S)
			return sys::virtual_key::DOWN;
		else if(keycode == sys::virtual_key::D)
			return sys::virtual_key::RIGHT;
	}
	return keycode;
}

sys::virtual_key replace_keycodes_identity(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	return keycode;
}

sys::virtual_key replace_keycodes(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::MINUS)
		return sys::virtual_key::SUBTRACT;
	else if(keycode == sys::virtual_key::PLUS)
		return sys::virtual_key::ADD;

	return state.current_scene.keycode_mapping(state, keycode, mod);
}


void nation_picker_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(state.ui_state.nation_picker->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		if(keycode == sys::virtual_key::ESCAPE) {
			ui::show_main_menu_nation_picker(state);
		} else if(keycode == sys::virtual_key::TAB) {
			ui::open_chat_window(state);
		}
		state.map_state.on_key_down(keycode, mod);
	}
}

void state_selector_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(state.ui_state.select_states_legend->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		state.map_state.on_key_down(keycode, mod);
		if(keycode == sys::virtual_key::ESCAPE) {
			state.state_selection->on_cancel(state);
			switch_scene(state, scene_id::in_game_basic);
			state.ui_state.root->impl_on_update(state);
		}
	}
}

void military_screen_on_lbutton_up(sys::state& state) {
	auto selected_group = state.selected_army_group;
	auto selected_province = state.map_state.selected_province;

	switch(state.selected_army_group_order) {
	case sys::army_group_order::none :
		return;
	case sys::army_group_order::designate_port:
		state.toggle_designated_port(selected_group, selected_province);
		return;
	case sys::army_group_order::siege:
		state.toggle_enforce_control_position(selected_group, selected_province);
		return;
	case sys::army_group_order::defend:
		state.toggle_defensive_position(selected_group, selected_province);
		return;
	default:
		break;
	}
}

void military_screen_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	auto selected_group = state.selected_army_group;
	auto selected_province = state.map_state.selected_province;

	if(state.ui_state.root->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		if(keycode == sys::virtual_key::ESCAPE) {
			if(state.selected_army_group_order != sys::army_group_order::none) {
				state.selected_army_group_order = sys::army_group_order::none;
			} else {
				if(!selected_group) {
					game_scene::switch_scene(state, scene_id::in_game_basic);
				} else {
					state.deselect_army_group();
				}
			}
		}
	} if(state.map_state.selected_province) {
		if(keycode == sys::virtual_key::Z) {
			//create HQ
			if(!selected_group) {
				state.new_army_group(state.map_state.selected_province);
			}
		} else if(selected_group) {
			if(keycode == sys::virtual_key::X) {
				state.toggle_designated_port(selected_group, selected_province);
			} else if(keycode == sys::virtual_key::V) {
				state.toggle_defensive_position(selected_group, selected_province);
			} else if(keycode == sys::virtual_key::N) {
				switch_scene(state, scene_id::in_game_military_selector);
			}
		}
	}
}

void economy_screen_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(state.ui_state.select_states_legend->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		state.map_state.on_key_down(keycode, mod);
		if(keycode == sys::virtual_key::ESCAPE) {
			state.iui_state.over_ui = false;
			switch_scene(state, scene_id::in_game_basic);
			state.ui_state.root->impl_on_update(state);
		}
	}
}

void handle_escape_basic(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(state.ui_state.console_window->is_visible()) {
		ui::console_window::show_toggle(state);
	} else if(!state.selected_armies.empty() || !state.selected_navies.empty()) {
		deselect_units(state);
		state.game_state_updated.store(true, std::memory_order::release);
	} else {
		ui::show_main_menu_nation_basic(state);
	}
}

void center_on_capital(sys::state& state, dcon::nation_id nation) {
	if(auto cap = state.world.nation_get_capital(nation); cap) {
		if(state.map_state.get_zoom() < map::zoom_very_close)
			state.map_state.zoom = map::zoom_very_close;
		state.map_state.center_map_on_province(state, cap);
	}
}

void in_game_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(state.ui_state.root->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		uint32_t ctrl_group = 0;
		if(keycode == sys::virtual_key::ESCAPE) {
			handle_escape_basic(state, keycode, mod);
		} else if(keycode == sys::virtual_key::TILDA || keycode == sys::virtual_key::BACK_SLASH) {
			ui::console_window::show_toggle(state);
		} else if(keycode == sys::virtual_key::HOME) {
			center_on_capital(state, state.local_player_nation);
		} else if(keycode == sys::virtual_key::TAB) {
			ui::open_chat_window(state);
		} else if(keycode == sys::virtual_key::Z && state.ui_state.ctrl_held_down) {
			// Battleplanner scene hotkey
			switch_scene(state, scene_id::in_game_military);
		} else if(keycode == sys::virtual_key::N && state.ui_state.ctrl_held_down) {
			// Economy scene hotkey
			switch_scene(state, scene_id::in_game_economy_viewer);
		} else if(keycode == sys::virtual_key::NUMPAD1 || keycode == sys::virtual_key::NUM_1) {
			ctrl_group = 1;
		} else if(keycode == sys::virtual_key::NUMPAD2 || keycode == sys::virtual_key::NUM_2) {
			ctrl_group = 2;
		} else if(keycode == sys::virtual_key::NUMPAD3 || keycode == sys::virtual_key::NUM_3) {
			ctrl_group = 3;
		} else if(keycode == sys::virtual_key::NUMPAD4 || keycode == sys::virtual_key::NUM_4) {
			ctrl_group = 4;
		} else if(keycode == sys::virtual_key::NUMPAD5 || keycode == sys::virtual_key::NUM_5) {
			ctrl_group = 5;
		} else if(keycode == sys::virtual_key::NUMPAD6 || keycode == sys::virtual_key::NUM_6) {
			ctrl_group = 6;
		} else if(keycode == sys::virtual_key::NUMPAD7 || keycode == sys::virtual_key::NUM_7) {
			ctrl_group = 7;
		} else if(keycode == sys::virtual_key::NUMPAD8 || keycode == sys::virtual_key::NUM_8) {
			ctrl_group = 8;
		} else if(keycode == sys::virtual_key::NUMPAD9 || keycode == sys::virtual_key::NUM_9) {
			ctrl_group = 9;
		}
		if(ctrl_group != 0) {
			if(mod == sys::key_modifiers::modifiers_ctrl) {
				for(const auto a : state.selected_armies) {
					auto& v = state.ctrl_armies[ctrl_group];
					auto it = std::find(v.begin(), v.end(), a);
					if(it != v.end()) {
						*it = v.back();
						v.pop_back();
					} else {
						v.push_back(a);
					}
				}
				for(const auto n : state.selected_navies) {
					auto& v = state.ctrl_navies[ctrl_group];
					auto it = std::find(v.begin(), v.end(), n);
					if(it != v.end()) {
						*it = v.back();
						v.pop_back();
					} else {
						v.push_back(n);
					}
				}
				state.game_state_updated.store(true, std::memory_order_release);
			} else { //shift to append
				for(const auto a : state.ctrl_armies[ctrl_group]) {
					state.select(a);
				}
				for(const auto n : state.ctrl_navies[ctrl_group]) {
					state.select(n);
				}
				state.game_state_updated.store(true, std::memory_order_release);
			}
		}

		if(!state.ui_state.topbar_subwindow->is_visible()) {
			state.map_state.on_key_down(keycode, mod);
		}

		if(keycode == sys::virtual_key::LEFT || keycode == sys::virtual_key::RIGHT || keycode == sys::virtual_key::UP || keycode == sys::virtual_key::DOWN) {
			if(state.ui_state.mouse_sensitive_target) {
				state.ui_state.mouse_sensitive_target->set_visible(state, false);
				state.ui_state.mouse_sensitive_target = nullptr;
			}
		}
	}
}

void do_nothing_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	return;
}


void on_key_down(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(state.ui_state.edit_target_internal && keycode != sys::virtual_key::ESCAPE) {
		state.ui_state.edit_target_internal->impl_on_key_down(state, keycode, mod);
	} else {
		keycode = replace_keycodes(state, keycode, mod);
		state.current_scene.handle_hotkeys(state, keycode, mod);
	}
}

void console_log_other(sys::state& state, std::string_view message) {
	auto msg = std::string(message);
	auto dt = state.current_date.to_ymd(state.start_date);
	auto dtstr = std::to_string(dt.year) + "." + std::to_string(dt.month) + "." + std::to_string(dt.day);
	msg = "" + std::string(state.network_state.nickname.to_string_view()) + "|" + dtstr + "| " + msg;
#ifdef _WIN32
	OutputDebugStringA(msg.c_str());
	OutputDebugStringA("\n");
#endif

	auto folder = simple_fs::get_or_create_data_dumps_directory();

	msg += "\n";

	simple_fs::append_file(
			folder,
			NATIVE("console_log.txt"),
			msg.c_str(),
			uint32_t(msg.size())
	);
	/*if(state.ui_state.console_window) {
		Cyto::Any payload = std::string(message);
		state.ui_state.console_window->impl_get(state, payload);
		if(true && !(state.ui_state.console_window->is_visible())) {
			state.ui_state.root->move_child_to_front(state.ui_state.console_window);
			state.ui_state.console_window->set_visible(state, true);
		}
	}*/
}


void render_units(sys::state& state) {
	if(state.ui_state.units_root) {
		state.ui_state.units_root->impl_render(state, 0, 0);
	}
}

void render_units_info_box(sys::state& state) {
	if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
		state.ui_state.unit_details_box->impl_render(state, state.ui_state.unit_details_box->base_data.position.x, state.ui_state.unit_details_box->base_data.position.y);
	}
}

void render_ui_military(sys::state& state) {
	render_units(state);
}

void render_ui_selection_screen(sys::state& state) {
	render_units(state);
	render_units_info_box(state);
}

void render_ui_ingame(sys::state& state) {
	state.iui_state.frame_start(state);
	if(state.ui_state.tl_chat_list) {
		state.ui_state.root->move_child_to_back(state.ui_state.tl_chat_list);
	}
	if(state.map_state.get_zoom() > map::zoom_close) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(state.open_gl.ui_shader_program);
		glUniform1i(state.open_gl.ui_shader_texture_sampler_uniform, 0);
		glUniform1i(state.open_gl.ui_shader_secondary_texture_sampler_uniform, 1);
		glUniform1f(state.open_gl.ui_shader_screen_width_uniform, float(state.x_size) / state.user_settings.ui_scale);
		glUniform1f(state.open_gl.ui_shader_screen_height_uniform, float(state.y_size) / state.user_settings.ui_scale);
		glUniform1f(state.open_gl.ui_shader_gamma_uniform, state.user_settings.gamma);
		glViewport(0, 0, state.x_size, state.y_size);
		glDepthRange(-1.0f, 1.0f);

		iui::rect label_rect{ 0.f, 0.f, state.iui_state.map_label.w * 2.f, state.iui_state.map_label.h * 1.5f };
		iui::rect label_text_rect = label_rect;
		iui::shrink(label_text_rect, 2.f);
		auto screen_size = glm::vec2(state.x_size, state.y_size) / state.user_settings.ui_scale;

		if(state.map_state.active_map_mode == map_mode::mode::admin) {
			state.world.for_each_nation([&](auto nid) {
				auto capital = state.world.nation_get_capital(nid);
				auto& midpoint = state.world.province_get_mid_point(capital);
				auto map_pos = state.map_state.normalize_map_coord(midpoint);
				glm::vec2 screen_pos{};
				if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos, { 200.f, 200.f })) {
					return;
				}
				screen_pos.y += 40.f;
				iui::move_to(
					label_rect,
					screen_pos.x - label_rect.w / 2.f, screen_pos.y - label_rect.h / 2.f
				);
				iui::move_to(
					label_text_rect,
					screen_pos.x - label_text_rect.w / 2.f, screen_pos.y - label_text_rect.h / 2.f + 2.f
				);
				state.iui_state.panel_textured(state, label_rect, state.iui_state.map_label.texture_handle);
				state.iui_state.localized_string(
					state, capital.id.index(),
					label_text_rect,
					text::produce_simple_string(state, state.world.province_get_name(capital))
				);
			});

			state.world.for_each_administration([&](auto aid) {
				auto capital = state.world.administration_get_capital(aid);
				auto& midpoint = state.world.province_get_mid_point(capital);
				auto map_pos = state.map_state.normalize_map_coord(midpoint);
				glm::vec2 screen_pos{};
				if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos, { 200.f, 200.f })) {
					return;
				}
				screen_pos.y += 40.f;
				iui::move_to(
					label_rect,
					screen_pos.x - label_rect.w / 2.f, screen_pos.y - label_rect.h / 2.f
				);
				iui::move_to(
					label_text_rect,
					screen_pos.x - label_text_rect.w / 2.f, screen_pos.y - label_text_rect.h / 2.f + 2.f
				);
				state.iui_state.panel_textured(state, label_rect, state.iui_state.map_label.texture_handle);
				state.iui_state.localized_string(
					state, capital.id.index(),
					label_text_rect,
					text::produce_simple_string(state, state.world.province_get_name(capital))
				);
			});
		}

		if(!state.ui_state.ctrl_held_down) {
			if(state.ui_state.rgos_root
			&& (state.map_state.active_map_mode == map_mode::mode::rgo_output
			|| state.map_state.active_map_mode == map_mode::mode::infrastructure
			|| state.map_state.active_map_mode == map_mode::mode::naval)) {
				state.ui_state.rgos_root->impl_render(state, 0, 0);
			} else {
				render_units(state);
				render_units_info_box(state);
			}
		} else if(state.map_state.get_zoom() >= ui::big_counter_cutoff && state.ui_state.province_details_root) {
			state.ui_state.province_details_root->impl_render(state, 0, 0);
		}
	}

	state.iui_state.frame_end();
}

ui::mouse_probe recalculate_mouse_probe_identity(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	return mouse_probe;
}

ui::mouse_probe recalculate_mouse_probe_units_details(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	float scaled_mouse_x = state.mouse_x_position / state.user_settings.ui_scale;
	float scaled_mouse_y = state.mouse_y_position / state.user_settings.ui_scale;
	float pos_x = state.ui_state.unit_details_box->base_data.position.x;
	float pos_y = state.ui_state.unit_details_box->base_data.position.y;

	return state.ui_state.unit_details_box->impl_probe_mouse(
			state,
			int32_t(scaled_mouse_x - pos_x),
			int32_t(scaled_mouse_y - pos_y),
			ui::mouse_probe_type::click
	);
}

ui::mouse_probe recalculate_mouse_probe_units(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	float scaled_mouse_x = state.mouse_x_position / state.user_settings.ui_scale;
	float scaled_mouse_y = state.mouse_y_position / state.user_settings.ui_scale;

	return state.ui_state.units_root->impl_probe_mouse(
			state,
			int32_t(scaled_mouse_x),
			int32_t(scaled_mouse_y),
			ui::mouse_probe_type::click
	);
}

ui::mouse_probe recalculate_mouse_probe_units_and_details(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
		mouse_probe = recalculate_mouse_probe_units_details(state, mouse_probe, tooltip_probe);
	}
	if(!mouse_probe.under_mouse) {
		mouse_probe = recalculate_mouse_probe_units(state, mouse_probe, tooltip_probe);
	}

	return mouse_probe;
}


ui::mouse_probe recalculate_mouse_probe_basic(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	if(!state.ui_state.units_root || state.ui_state.ctrl_held_down) {
		return mouse_probe;
	}
	if(state.map_state.active_map_mode == map_mode::mode::rgo_output
	|| state.map_state.active_map_mode == map_mode::mode::infrastructure
	|| state.map_state.active_map_mode == map_mode::mode::naval) {
		// RGO doesn't need clicks... yet
		return mouse_probe;
	}
	return recalculate_mouse_probe_units_and_details(state, mouse_probe, tooltip_probe);
}

ui::mouse_probe recalculate_mouse_probe_military(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	if(!state.ui_state.military_root) {
		return mouse_probe;
	}

	float scaled_mouse_x = state.mouse_x_position / state.user_settings.ui_scale;
	float scaled_mouse_y = state.mouse_y_position / state.user_settings.ui_scale;

	return state.ui_state.military_root->impl_probe_mouse(
		state,
		int32_t(scaled_mouse_x),
		int32_t(scaled_mouse_y),
		ui::mouse_probe_type::click
	);
}

ui::mouse_probe recalculate_tooltip_probe_units_and_details(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	float scaled_mouse_x = state.mouse_x_position / state.user_settings.ui_scale;
	float scaled_mouse_y = state.mouse_y_position / state.user_settings.ui_scale;
	
	if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
		float pos_x = state.ui_state.unit_details_box->base_data.position.x;
		float pos_y = state.ui_state.unit_details_box->base_data.position.y;

		tooltip_probe = state.ui_state.unit_details_box->impl_probe_mouse(state,
			int32_t(scaled_mouse_x - pos_x),
			int32_t(scaled_mouse_y - pos_y),
			ui::mouse_probe_type::tooltip);
	}
	if(!tooltip_probe.under_mouse) {
		tooltip_probe = state.ui_state.units_root->impl_probe_mouse(
			state,
			int32_t(scaled_mouse_x),
			int32_t(scaled_mouse_y),
			ui::mouse_probe_type::tooltip);
	}

	return tooltip_probe;
}

ui::mouse_probe recalculate_tooltip_probe_basic(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	if(!state.ui_state.units_root || state.ui_state.ctrl_held_down) {
		return tooltip_probe;
	}
	if(state.map_state.active_map_mode == map_mode::mode::rgo_output
	|| state.map_state.active_map_mode == map_mode::mode::infrastructure
	|| state.map_state.active_map_mode == map_mode::mode::naval) {
		// RGO doesn't need clicks... yet
		return tooltip_probe;
	}
	if(tooltip_probe.under_mouse) {
		return tooltip_probe;
	}
	return recalculate_tooltip_probe_units_and_details(state, mouse_probe, tooltip_probe);
}

void clean_up_selected_armies_and_navies(sys::state& state) {
	// Allow player to select single enemy army or enemy navy to view them
	if(state.selected_armies.size() == 1) {
		if(!state.world.army_is_valid(state.selected_armies[0])) {
			state.selected_armies.clear();
		}
	} else if(state.selected_navies.size() == 1) {
		if(!state.world.navy_is_valid(state.selected_navies[0])) {
			state.selected_navies.clear();
		}
	} else {
		for(auto i = state.selected_armies.size(); i-- > 0; ) {
			if(!state.world.army_is_valid(state.selected_armies[i]) || military::get_effective_unit_commander(state, state.selected_armies[i]) != state.local_player_nation) {
				state.selected_armies[i] = state.selected_armies.back();
				state.selected_armies.pop_back();
			}
		}
		for(auto i = state.selected_navies.size(); i-- > 0; ) {
			if(!state.world.navy_is_valid(state.selected_navies[i]) || military::get_effective_unit_commander(state, state.selected_navies[i]) != state.local_player_nation) {
				state.selected_navies[i] = state.selected_navies.back();
				state.selected_navies.pop_back();
			}
		}
	}
}

void clean_up_basic_game_scene(sys::state& state) {
	if(state.ui_state.change_leader_window && state.ui_state.change_leader_window->is_visible()) {
		ui::leader_selection_window* win = static_cast<ui::leader_selection_window*>(state.ui_state.change_leader_window);
		if(state.ui_state.military_subwindow->is_visible() == false
			&& std::find(state.selected_armies.begin(), state.selected_armies.end(), win->a) == state.selected_armies.end()
			&& std::find(state.selected_navies.begin(), state.selected_navies.end(), win->v) == state.selected_navies.end()
		) {
			state.ui_state.change_leader_window->set_visible(state, false);
		}
	}
	clean_up_selected_armies_and_navies(state);
	// clear up control groups too
	for(auto& v : state.ctrl_armies) {
		for(auto i = v.size(); i-- > 0; ) {
			if(!state.world.army_is_valid(v[i]) || state.world.army_get_controller_from_army_control(v[i]) != state.local_player_nation) {
				v[i] = v.back();
				v.pop_back();
			}
		}
	}
	for(auto& v : state.ctrl_navies) {
		for(auto i = v.size(); i-- > 0; ) {
			if(!state.world.navy_is_valid(v[i]) || state.world.navy_get_controller_from_navy_control(v[i]) != state.local_player_nation) {
				v[i] = v.back();
				v.pop_back();
			}
		}
	}
}

void update_army_group_selection_ui(sys::state& state) {
	state.ui_state.army_group_window_land->set_visible(state, bool(state.selected_army_group));
}

void update_unit_selection_ui(sys::state& state) {
	// Change window visibility and pass unit ids down.
	if(state.selected_armies.size() + state.selected_navies.size() > 1) {
		state.ui_state.multi_unit_selection_window->set_visible(state, true);
		state.ui_state.army_status_window->set_visible(state, false);
		state.ui_state.navy_status_window->set_visible(state, false);
	} else if(state.selected_armies.size() == 1) {
		state.ui_state.multi_unit_selection_window->set_visible(state, false);
		if(state.ui_state.army_status_window->is_visible() && state.ui_state.army_status_window->unit_id != state.selected_armies[0]) {
			state.ui_state.army_status_window->unit_id = state.selected_armies[0];
			state.ui_state.army_status_window->impl_on_update(state);
		} else {
			state.ui_state.army_status_window->unit_id = state.selected_armies[0];
			state.ui_state.army_status_window->set_visible(state, true);
		}
		state.ui_state.navy_status_window->set_visible(state, false);
	} else if(state.selected_navies.size() == 1) {
		state.ui_state.multi_unit_selection_window->set_visible(state, false);
		state.ui_state.army_status_window->set_visible(state, false);
		if(state.ui_state.navy_status_window->is_visible() && state.ui_state.navy_status_window->unit_id != state.selected_navies[0]) {
			state.ui_state.navy_status_window->unit_id = state.selected_navies[0];
			state.ui_state.navy_status_window->impl_on_update(state);
		} else {
			state.ui_state.navy_status_window->unit_id = state.selected_navies[0];
			state.ui_state.navy_status_window->set_visible(state, true);
		}
	} else {
		state.ui_state.multi_unit_selection_window->set_visible(state, false);
		state.ui_state.army_status_window->set_visible(state, false);
		state.ui_state.navy_status_window->set_visible(state, false);
	}
}

void update_ui_unit_details(sys::state& state) {
	if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
		state.ui_state.unit_details_box->impl_on_update(state);
	}
	if(state.ui_state.units_root) {
		state.ui_state.units_root->impl_on_update(state);
	}
}

void update_ui_state_basic(sys::state& state) {
	map_mode::update_map_mode(state);
	ui::close_expired_event_windows(state);
	if(state.ui_state.rgos_root) {
		state.ui_state.rgos_root->impl_on_update(state);
	}
	update_ui_unit_details(state);
	
	if(state.ui_state.ctrl_held_down && state.map_state.get_zoom() >= ui::big_counter_cutoff && state.ui_state.province_details_root) {
		state.ui_state.province_details_root->impl_on_update(state);
	}	
}

void update_basic_game_scene(sys::state& state) {
	if(state.ui_state.army_combat_window && state.ui_state.army_combat_window->is_visible()) {
		ui::land_combat_window* win = static_cast<ui::land_combat_window*>(state.ui_state.army_combat_window);
		if(win->battle && !state.world.land_battle_is_valid(win->battle)) {
			state.ui_state.army_combat_window->set_visible(state, false);
		}
	}
	update_unit_selection_ui(state);
	state.map_state.map_data.update_borders(state);
}

void update_military_game_scene(sys::state& state) {
	update_army_group_selection_ui(state);
	state.map_state.map_data.update_borders(state);
}

void update_add_units_game_scene(sys::state& state) {
	update_unit_selection_ui(state);
	state.map_state.map_data.update_borders(state);
}

void generic_map_scene_update(sys::state& state) {
	state.map_state.map_data.update_borders(state);
}

void economy_scene_update(sys::state& state) {
	ui::close_expired_event_windows(state);
	economy_viewer::update(state);
}

void open_chat_during_game(sys::state& state) {
	ui::open_chat_during_game(state);
}
void open_chat_before_game(sys::state& state) {
	ui::open_chat_before_game(state);
}

void highlight_player_nation(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province) {
	for(const auto pc : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
		data[province::to_map_id(pc.get_province())] = 0x2B2B2B2B;
	}
}

void highlight_given_province(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province) {
	if(selected_province) {
		data[province::to_map_id(selected_province)] = 0x2B2B2B2B;
	}
}

void highlight_defensive_positions(sys::state& state, std::vector<uint32_t>& data, dcon::province_id selected_province) {
	if(state.selected_army_group) {

		for(auto position :	state.world.automated_army_group_get_provinces_defend(state.selected_army_group)) {
			data[province::to_map_id(position)] = 0x2B2B2B2B;
		}

		for(auto position : state.world.automated_army_group_get_provinces_ferry_origin(state.selected_army_group)) {
			data[province::to_map_id(position)] = 0x2B2B2B2B;
		}

		for(auto position : state.world.automated_army_group_get_provinces_enforce_control(state.selected_army_group)) {
			data[province::to_map_id(position)] = 0x2B2B2B2B;
		}
	}
}

ui::element_base* root_end_screen(sys::state& state){
	return state.ui_state.end_screen.get();
}

ui::element_base* root_pick_nation(sys::state& state) {
	return state.ui_state.nation_picker.get();
}

ui::element_base* root_game_basic(sys::state& state) {
	return state.ui_state.root.get();
}

ui::element_base* root_game_battleplanner(sys::state& state) {
	return state.ui_state.military_root.get();
}

ui::element_base* root_game_battleplanner_add_army(sys::state& state) {
	return state.ui_state.army_group_selector_root.get();
}

ui::element_base* root_game_battleplanner_unit_selection(sys::state& state) {
	return state.ui_state.army_group_selector_root.get();
}

ui::element_base* root_game_economy_viewer(sys::state& state) {
	return state.ui_state.economy_viewer_root.get();
}

ui::element_base* root_game_wargoal_state_selection(sys::state& state) {
	return state.ui_state.select_states_legend.get();
}

}
