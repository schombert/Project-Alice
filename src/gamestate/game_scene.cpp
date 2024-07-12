#include "gui_unit_grid_box.hpp"
#include "gui_unit_panel.hpp"
#include "gui_province_window.hpp"
#include "gui_land_combat.hpp"
#include "gui_console.hpp"
#include "gui_chat_window.hpp"
#include "gui_event.hpp"
#include "gui_map_icons.hpp"


namespace game_scene {

void switch_scene(sys::state& state, scene_id ui_scene) {

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

	switch(ui_scene) {
	case scene_id::in_game_state_selector:
		state.current_scene = state_wargoal_selector;

		state.stored_map_mode = state.map_state.active_map_mode;
		map_mode::set_map_mode(state, map_mode::mode::state_select);
		state.map_state.set_selected_province(dcon::province_id{});


		return;

	case scene_id::in_game_basic:
		if(state.current_scene.id == scene_id::in_game_state_selector) {
			state.state_selection.reset();
			map_mode::set_map_mode(state, state.stored_map_mode);
		}

		state.current_scene = basic_game;		

		state.ui_state.military_root->set_visible(state, false);
		state.ui_state.root->set_visible(state, true);


		return;

	case scene_id::in_game_military:
		state.current_scene = battleplan_editor;

		state.ui_state.military_root->set_visible(state, true);
		state.ui_state.root->set_visible(state, false);

		return;

	case scene_id::end_screen:
		state.current_scene = end_screen;

		return;

	case scene_id::pick_nation:
		state.current_scene = nation_picker;

		return;
	}
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

	for(auto a : state.selected_armies) {
		if(command::can_move_army(state, nation, a, target).empty()) {
			fail = true;
		} else {
			command::move_army(state, nation, a, target, reset_orders);
			army_play = true;
		}
	}
	for(auto a : state.selected_navies) {
		if(command::can_move_navy(state, nation, a, target).empty()) {
			fail = true;
		} else {
			command::move_navy(state, nation, a, target, reset_orders);
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
		// TODO: Allow Co-op
		if(state.network_mode == sys::network_mode_type::single_player) {
			state.world.nation_set_is_player_controlled(state.local_player_nation, false);
			state.local_player_nation = owner;
			state.world.nation_set_is_player_controlled(state.local_player_nation, true);
			if(state.ui_state.nation_picker) {
				state.ui_state.nation_picker->impl_on_update(state);
			}
		} else if(command::can_notify_player_picks_nation(state, state.local_player_nation, owner)) {
			command::notify_player_picks_nation(state, state.local_player_nation, owner);
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
	state.ui_state.under_mouse->impl_on_lbutton_down(
		state,
		state.ui_state.relative_mouse_location.x,
		state.ui_state.relative_mouse_location.y,
		mod
	);
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
	state.ui_state.edit_target = nullptr;

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
	state.ui_state.edit_target = nullptr;

	if(state.ui_state.under_mouse != nullptr) {
		ui_lbutton(state, mod);
		return;
	}

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

void deselect_units(sys::state& state) {
	state.selected_armies.clear();
	state.selected_navies.clear();
}

bool province_mid_point_is_in_selection(sys::state& state, int32_t x, int32_t y, dcon::province_id province) {
	auto mid_point = state.world.province_get_mid_point(province);
	auto map_pos = state.map_state.normalize_map_coord(mid_point);
	auto screen_size = glm::vec2{ float(state.x_size), float(state.y_size) };
	glm::vec2 screen_pos;
	if(state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
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
		auto& vertex = state.map_state.map_data.border_vertices[border.start_index + border.count / 2];

		auto map_x = vertex.position.x;
		auto map_y = vertex.position.y;

		glm::vec2 map_pos(map_x, 1.0f - map_y);
		auto screen_size = glm::vec2{ float(state.x_size), float(state.y_size) };
		glm::vec2 screen_pos;
		if(state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
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
	if((int32_t(sys::key_modifiers::modifiers_ctrl) & int32_t(mod)) == 0) {
		for(auto a : state.world.nation_get_army_control(state.local_player_nation)) {
			if(!a.get_army().get_navy_from_army_transport() && !a.get_army().get_battle_from_army_battle_participation() && !a.get_army().get_is_retreating()) {
				if(army_is_in_selection(state, x, y, a.get_army())) {
					state.select(a.get_army());
				}
			}
		}
	}
	for(auto a : state.world.nation_get_navy_control(state.local_player_nation)) {
		if(!a.get_navy().get_battle_from_navy_battle_participation() && !a.get_navy().get_is_retreating()) {
			if(navy_is_in_selection(state, x, y, a.get_navy())) {
				state.select(a.get_navy());
			}
		}
	}
	if(!state.selected_armies.empty() && !state.selected_navies.empty()) {
		state.selected_navies.clear();
	}
	// Hide province upon selecting multiple armies / navies :)
	if(!state.selected_armies.empty() || !state.selected_navies.empty()) {
		if(state.ui_state.province_window) {
			state.ui_state.province_window->set_visible(state, false);
			state.map_state.set_selected_province(dcon::province_id{}); //ensure we deselect from map too
		}
		// Play selection sound effect
		if(!state.selected_armies.empty()) {
			sound::play_effect(state, sound::get_army_select_sound(state), get_effects_volume(state));
		} else {
			sound::play_effect(state, sound::get_navy_select_sound(state), get_effects_volume(state));
		}
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
	state.current_scene.lbutton_up(state);

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
	if(!state.ui_state.edit_target && state.user_settings.wasd_for_map_movement) {
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
	bool consumed = true;

	if(state.ui_state.nation_picker->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		if(keycode == sys::virtual_key::ESCAPE) {
			ui::show_main_menu_nation_picker(state);
			consumed = true;
		} else if(keycode == sys::virtual_key::TAB) {
			ui::open_chat_window(state);
			consumed = true;
		}
		state.map_state.on_key_down(keycode, mod);
	}
}

void state_selector_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	if(state.ui_state.select_states_legend->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		state.map_state.on_key_down(keycode, mod);
		if(keycode == sys::virtual_key::ESCAPE) {
			switch_scene(state, scene_id::in_game_basic);
			state.state_selection->on_cancel(state);
			state.finish_state_selection();
			state.ui_state.root->impl_on_update(state);
		}
	}
}

void military_screen_hotkeys(sys::state& state, sys::virtual_key keycode, sys::key_modifiers mod) {
	auto selected_group = state.selected_army_group;
	auto selected_province = state.map_state.selected_province;

	if(state.ui_state.root->impl_on_key_down(state, keycode, mod) != ui::message_result::consumed) {
		if(keycode == sys::virtual_key::ESCAPE) {
			if(selected_group == nullptr) {
				game_scene::switch_scene(state, scene_id::in_game_basic);
			} else {
				state.deselect_army_group();
			}
		}
	} if(state.map_state.selected_province) {
		if(keycode == sys::virtual_key::Z) {
			//create HQ
			if(selected_group == nullptr) {
				state.new_army_group(state.map_state.selected_province);
			}
		} else if(selected_group != nullptr) {
			if(keycode == sys::virtual_key::X) {
				if(state.ui_state.ctrl_held_down) {
					selected_group->naval_travel_origin.clear();
				} else {
					selected_group->naval_travel_origin.push_back(selected_province);
				}
			} else if(keycode == sys::virtual_key::C) {
				if(state.ui_state.ctrl_held_down) {
					selected_group->naval_travel_target.clear();
				} else {
					selected_group->naval_travel_target.push_back(selected_province);
				}
			} else if(keycode == sys::virtual_key::V) {
				if(state.ui_state.ctrl_held_down) {
					// TODO: removal of defensive position
				} else {
					state.new_defensive_position(selected_group, selected_province);
				}
			} else if(keycode == sys::virtual_key::B) {
				for(auto item : state.selected_armies) {
					state.remove_army_from_all_army_groups_clean(item);
					state.add_army_to_army_group(state.selected_army_group, item);
				}
				for(auto item : state.selected_navies) {
					state.remove_navy_from_all_army_groups_clean(item);
					state.add_navy_to_army_group(state.selected_army_group, item);
				}
				state.update_regiments_and_ships(state.selected_army_group);
			}
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
	keycode = replace_keycodes(state, keycode, mod);
	if(state.ui_state.edit_target) {
		state.ui_state.edit_target->impl_on_key_down(state, keycode, mod);
	} else {
		state.current_scene.handle_hotkeys(state, keycode, mod);
	}
}

void console_log_pick_nation(sys::state& state, std::string_view message) {
	if(state.ui_state.console_window_r != nullptr) {
		Cyto::Any payload = std::string(message);
		state.ui_state.console_window_r->impl_get(state, payload);
		if(true && !(state.ui_state.console_window_r->is_visible())) {
			state.ui_state.nation_picker->move_child_to_front(state.ui_state.console_window_r);
			state.ui_state.console_window_r->set_visible(state, true);
		}
	}
}

void console_log_other(sys::state& state, std::string_view message) {
	if(state.ui_state.console_window != nullptr) {
		Cyto::Any payload = std::string(message);
		state.ui_state.console_window->impl_get(state, payload);
		if(true && !(state.ui_state.console_window->is_visible())) {
			state.ui_state.root->move_child_to_front(state.ui_state.console_window);
			state.ui_state.console_window->set_visible(state, true);
		}
	}
}

void render_ui_ingame(sys::state& state) {
	if(state.ui_state.tl_chat_list) {
		state.ui_state.root->move_child_to_front(state.ui_state.tl_chat_list);
	}
	if(state.map_state.get_zoom() > map::zoom_close) {
		if(!state.ui_state.ctrl_held_down) {
			if(state.ui_state.rgos_root && state.map_state.active_map_mode == map_mode::mode::rgo_output) {
				state.ui_state.rgos_root->impl_render(state, 0, 0);
			} else {
				if(state.ui_state.units_root) {
					state.ui_state.units_root->impl_render(state, 0, 0);
				}
				if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
					state.ui_state.unit_details_box->impl_render(state, state.ui_state.unit_details_box->base_data.position.x, state.ui_state.unit_details_box->base_data.position.y);
				}
			}
		} else if(state.map_state.get_zoom() >= ui::big_counter_cutoff && state.ui_state.province_details_root) {
			state.ui_state.province_details_root->impl_render(state, 0, 0);
		}
	}
}

ui::mouse_probe recalculate_mouse_probe_identity(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	return mouse_probe;
}

ui::mouse_probe recalculate_mouse_probe_basic(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	if(!state.ui_state.units_root || state.ui_state.ctrl_held_down) {
		return mouse_probe;
	}
	if(state.map_state.active_map_mode == map_mode::mode::rgo_output) {
		// RGO doesn't need clicks... yet
		return mouse_probe;
	}

	float scaled_mouse_x = state.mouse_x_position / state.user_settings.ui_scale;
	float scaled_mouse_y = state.mouse_y_position / state.user_settings.ui_scale;
	float pos_x = state.ui_state.unit_details_box->base_data.position.x;
	float pos_y = state.ui_state.unit_details_box->base_data.position.y;

	if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
		mouse_probe = state.ui_state.unit_details_box->impl_probe_mouse(
			state,
			int32_t(scaled_mouse_x - pos_x),
			int32_t(scaled_mouse_y - pos_y),
			ui::mouse_probe_type::click
		);
	}
	if(!mouse_probe.under_mouse) {
		mouse_probe = state.ui_state.units_root->impl_probe_mouse(
			state,
			int32_t(scaled_mouse_x),
			int32_t(scaled_mouse_y),
			ui::mouse_probe_type::click
		);
	}

	return mouse_probe;
}

ui::mouse_probe recalculate_tooltip_probe_basic(sys::state& state, ui::mouse_probe mouse_probe, ui::mouse_probe tooltip_probe) {
	if(!state.ui_state.units_root || state.ui_state.ctrl_held_down) {
		return tooltip_probe;
	}
	if(state.map_state.active_map_mode == map_mode::mode::rgo_output) {
		// RGO doesn't need clicks... yet
		return tooltip_probe;
	}

	if(tooltip_probe.under_mouse) {
		return tooltip_probe;
	}

	float scaled_mouse_x = state.mouse_x_position / state.user_settings.ui_scale;
	float scaled_mouse_y = state.mouse_y_position / state.user_settings.ui_scale;
	float pos_x = state.ui_state.unit_details_box->base_data.position.x;
	float pos_y = state.ui_state.unit_details_box->base_data.position.y;

	if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
		tooltip_probe = state.ui_state.unit_details_box->impl_probe_mouse(state,
			int32_t(scaled_mouse_x - pos_x),
			int32_t(scaled_mouse_y - pos_y),
			ui::mouse_probe_type::tooltip);
		mouse_probe = state.ui_state.unit_details_box->impl_probe_mouse(
			state,
			int32_t(scaled_mouse_x - pos_x),
			int32_t(scaled_mouse_y - pos_y),
			ui::mouse_probe_type::click
		);
	}
	if(!mouse_probe.under_mouse) {
		tooltip_probe = state.ui_state.units_root->impl_probe_mouse(
			state,
			int32_t(scaled_mouse_x),
			int32_t(scaled_mouse_y),
			ui::mouse_probe_type::tooltip);
	}

	return tooltip_probe;
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
	for(auto i = state.selected_armies.size(); i-- > 0; ) {
		if(!state.world.army_is_valid(state.selected_armies[i]) || state.world.army_get_controller_from_army_control(state.selected_armies[i]) != state.local_player_nation) {
			state.selected_armies[i] = state.selected_armies.back();
			state.selected_armies.pop_back();
		}
	}
	for(auto i = state.selected_navies.size(); i-- > 0; ) {
		if(!state.world.navy_is_valid(state.selected_navies[i]) || state.world.navy_get_controller_from_navy_control(state.selected_navies[i]) != state.local_player_nation) {
			state.selected_navies[i] = state.selected_navies.back();
			state.selected_navies.pop_back();
		}
	}
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
	if(state.selected_army_group != nullptr) {
		state.ui_state.army_group_window->set_visible(state, true);
	} else {
		state.ui_state.army_group_window->set_visible(state, false);
	}
}

void update_unit_selection_ui(sys::state& state) {
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

void update_ui_state_basic(sys::state& state) {
	map_mode::update_map_mode(state);
	if(state.ui_state.unit_details_box && state.ui_state.unit_details_box->is_visible()) {
		state.ui_state.unit_details_box->impl_on_update(state);
	}
	ui::close_expired_event_windows(state);
	if(state.ui_state.rgos_root) {
		state.ui_state.rgos_root->impl_on_update(state);
	}
	if(state.ui_state.units_root) {
		state.ui_state.units_root->impl_on_update(state);
	}
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

void generic_map_scene_update(sys::state& state) {
	state.map_state.map_data.update_borders(state);
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

}
