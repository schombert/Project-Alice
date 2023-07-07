#include "system_state.hpp"
#include "dcon_generated.hpp"
#include "map_modes.hpp"
#include "opengl_wrapper.hpp"
#include "window.hpp"
#include "gui_element_base.hpp"
#include <algorithm>
#include <functional>
#include "parsers_declarations.hpp"
#include "gui_console.hpp"
#include "gui_minimap.hpp"
#include "gui_unit_panel.hpp"
#include "gui_topbar.hpp"
#include "gui_province_window.hpp"
#include "gui_outliner_window.hpp"
#include "gui_event.hpp"
#include "gui_map_icons.hpp"
#include "gui_election_window.hpp"
#include "gui_diplomacy_request_window.hpp"
#include "gui_message_window.hpp"
#include "gui_naval_combat.hpp"
#include "gui_land_combat.hpp"
#include "map_tooltip.hpp"
#include "unit_tooltip.hpp"
#include "main_menu/gui_country_selection_window.hpp"
#include "demographics.hpp"
#include <algorithm>
#include <thread>
#include "rebels.hpp"

namespace sys {
//
// window event functions
//

void state::on_rbutton_down(int32_t x, int32_t y, key_modifiers mod) {
	// Lose focus on text
	ui_state.edit_target = nullptr;

	if(ui_state.under_mouse != nullptr) {
		// TODO: look at return value
		auto r =ui_state.under_mouse->impl_on_rbutton_down(*this, ui_state.relative_mouse_location.x, ui_state.relative_mouse_location.y, mod);
			if(r != ui::message_result::consumed) {
				map_state.on_rbutton_down(*this, x, y, x_size, y_size, mod);
			}
	} else {
		map_state.on_rbutton_down(*this, x, y, x_size, y_size, mod);
	}
}
void state::on_mbutton_down(int32_t x, int32_t y, key_modifiers mod) {
	// Lose focus on text
	ui_state.edit_target = nullptr;

	map_state.on_mbuttom_down(x, y, x_size, y_size, mod);
}
void state::on_lbutton_down(int32_t x, int32_t y, key_modifiers mod) {
	// Lose focus on text
	ui_state.edit_target = nullptr;

	if(ui_state.under_mouse != nullptr) {
		auto r = ui_state.under_mouse->impl_on_lbutton_down(*this, ui_state.relative_mouse_location.x,
				ui_state.relative_mouse_location.y, mod);
		if(r != ui::message_result::consumed) {
			map_state.on_lbutton_down(*this, x, y, x_size, y_size, mod);
			if(ui_state.province_window) {
				static_cast<ui::province_view_window*>(ui_state.province_window)->set_active_province(*this, map_state.selected_province);
			}
		}
	} else {
		map_state.on_lbutton_down(*this, x, y, x_size, y_size, mod);
		if(ui_state.province_window) {
			static_cast<ui::province_view_window*>(ui_state.province_window)->set_active_province(*this, map_state.selected_province);
		}
	}
}
void state::on_rbutton_up(int32_t x, int32_t y, key_modifiers mod) { }
void state::on_mbutton_up(int32_t x, int32_t y, key_modifiers mod) {
	map_state.on_mbuttom_up(x, y, mod);
}
void state::on_lbutton_up(int32_t x, int32_t y, key_modifiers mod) {
	is_dragging = false;
	if(ui_state.drag_target) {
		on_drag_finished(x, y, mod);
	}
}
void state::on_mouse_move(int32_t x, int32_t y, key_modifiers mod) {
	if(ui_state.under_mouse != nullptr) {
		auto r = ui_state.under_mouse->impl_on_mouse_move(*this, ui_state.relative_mouse_location.x,
				ui_state.relative_mouse_location.y, mod);
		if(r != ui::message_result::consumed) {
			map_state.on_mouse_move(x, y, x_size, y_size, mod);
		}
	} else {
		map_state.on_mouse_move(x, y, x_size, y_size, mod);
		if(map_state.is_dragging) {
			if(ui_state.mouse_sensitive_target) {
				ui_state.mouse_sensitive_target->set_visible(*this, false);
				ui_state.mouse_sensitive_target = nullptr;
			}
		}
	}
	if(ui_state.mouse_sensitive_target) {
		auto mx = int32_t(x / user_settings.ui_scale);
		auto my = int32_t(y / user_settings.ui_scale);
		if(mx < ui_state.target_ul_bounds.x || mx > ui_state.target_lr_bounds.x || my < ui_state.target_ul_bounds.y || my > ui_state.target_lr_bounds.y) {

			ui_state.mouse_sensitive_target->set_visible(*this, false);
			ui_state.mouse_sensitive_target = nullptr;
		}
	}
}
void state::on_mouse_drag(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is held down
	is_dragging = true;

	if(ui_state.drag_target)
		ui_state.drag_target->on_drag(*this, int32_t(mouse_x_position / user_settings.ui_scale),
				int32_t(mouse_y_position / user_settings.ui_scale), int32_t(x / user_settings.ui_scale),
				int32_t(y / user_settings.ui_scale), mod);
}
void state::on_drag_finished(int32_t x, int32_t y,
		key_modifiers mod) { // called when the left button is released after one or more drag events
	if(ui_state.drag_target) {
		ui_state.drag_target->on_drag_finish(*this);
		ui_state.drag_target = nullptr;
	}
}
void state::on_resize(int32_t x, int32_t y, window::window_state win_state) {
	if(win_state != window::window_state::minimized) {
		ui_state.root->base_data.size.x = int16_t(x / user_settings.ui_scale);
		ui_state.root->base_data.size.y = int16_t(y / user_settings.ui_scale);
	}
}
void state::on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod,
		float amount) { // an amount of 1.0 is one "click" of the wheel
	if(ui_state.scroll_target != nullptr) {
		auto r = ui_state.scroll_target->impl_on_scroll(*this, ui_state.relative_mouse_location.x, ui_state.relative_mouse_location.y,
				amount, mod);
		if(r != ui::message_result::consumed) {
			// TODO Settings for making zooming the map faster
			map_state.on_mouse_wheel(x, y, x_size, y_size, mod, amount);

			if(ui_state.mouse_sensitive_target) {
				ui_state.mouse_sensitive_target->set_visible(*this, false);
				ui_state.mouse_sensitive_target = nullptr;
			}
		}
	} else {
		map_state.on_mouse_wheel(x, y, x_size, y_size, mod, amount);

		if(ui_state.mouse_sensitive_target) {
			ui_state.mouse_sensitive_target->set_visible(*this, false);
			ui_state.mouse_sensitive_target = nullptr;
		}
	}
}
void state::on_key_down(virtual_key keycode, key_modifiers mod) {
	if(ui_state.edit_target) {
		ui_state.edit_target->impl_on_key_down(*this, keycode, mod);
	} else {
		if(ui_state.root->impl_on_key_down(*this, keycode, mod) != ui::message_result::consumed) {
			if(keycode == virtual_key::ESCAPE) {
				if(ui_state.console_window->is_visible())
					ui::console_window::show_toggle(*this);
				else
					ui::show_main_menu(*this);
			} else if(keycode == virtual_key::TILDA || keycode == virtual_key::BACK_SLASH) {
				ui::console_window::show_toggle(*this);
			}
			map_state.on_key_down(keycode, mod);

			if(keycode == sys::virtual_key::LEFT || keycode == sys::virtual_key::RIGHT || keycode == sys::virtual_key::UP || keycode == sys::virtual_key::DOWN) {
				if(ui_state.mouse_sensitive_target) {
					ui_state.mouse_sensitive_target->set_visible(*this, false);
					ui_state.mouse_sensitive_target = nullptr;
				}
			}
		}
	}
}
void state::on_key_up(virtual_key keycode, key_modifiers mod) {
	map_state.on_key_up(keycode, mod);
}
void state::on_text(char c) { // c is win1250 codepage value
	if(ui_state.edit_target)
		ui_state.edit_target->on_text(*this, c);
}

inline constexpr int32_t tooltip_width = 400;

void state::render() { // called to render the frame may (and should) delay returning until the frame is rendered, including
                       // waiting for vsync
	auto game_state_was_updated = game_state_updated.exchange(false, std::memory_order::acq_rel);

	auto mouse_probe = ui_state.root->impl_probe_mouse(*this, int32_t(mouse_x_position / user_settings.ui_scale),
			int32_t(mouse_y_position / user_settings.ui_scale), ui::mouse_probe_type::click);
	auto tooltip_probe = ui_state.root->impl_probe_mouse(*this, int32_t(mouse_x_position / user_settings.ui_scale),
			int32_t(mouse_y_position / user_settings.ui_scale), ui::mouse_probe_type::tooltip);

	if(!mouse_probe.under_mouse && map_state.get_zoom() > 5 && ui_state.unit_details_box->is_visible()) {
		mouse_probe = ui_state.unit_details_box->impl_probe_mouse(*this,
			int32_t(mouse_x_position / user_settings.ui_scale - ui_state.unit_details_box->base_data.position.x),
			int32_t(mouse_y_position / user_settings.ui_scale - ui_state.unit_details_box->base_data.position.y),
			ui::mouse_probe_type::click);
		if(!tooltip_probe.under_mouse) {
			mouse_probe = ui_state.unit_details_box->impl_probe_mouse(*this,
				int32_t(mouse_x_position / user_settings.ui_scale - ui_state.unit_details_box->base_data.position.x),
				int32_t(mouse_y_position / user_settings.ui_scale - ui_state.unit_details_box->base_data.position.y),
				ui::mouse_probe_type::tooltip);
		}
	}

	if(!mouse_probe.under_mouse && map_state.get_zoom() > 5) {
		if(map_state.active_map_mode == map_mode::mode::rgo_output) {
			// RGO doesn't need clicks... yet
		} else {
			mouse_probe = ui_state.units_root->impl_probe_mouse(*this, int32_t(mouse_x_position / user_settings.ui_scale),
					int32_t(mouse_y_position / user_settings.ui_scale), ui::mouse_probe_type::click);
		}
		if(!tooltip_probe.under_mouse) {
			tooltip_probe = ui_state.units_root->impl_probe_mouse(*this, int32_t(mouse_x_position / user_settings.ui_scale),
				int32_t(mouse_y_position / user_settings.ui_scale), ui::mouse_probe_type::tooltip);
		}
	}

	if(game_state_was_updated) {
		this->map_state.map_data.update_borders(*this);
		nations::update_ui_rankings(*this);
		// Processing of (gamestate <=> ui) queues
		{
			// National events
			auto* c1 = new_n_event.front();
			while(c1) {
				if(world.national_event_get_is_major(c1->e)) {
					static_cast<ui::national_event_window<true>*>(ui_state.major_event_window)
							->events.push_back(ui::event_data_wrapper{*c1});
				} else {
					static_cast<ui::national_event_window<false>*>(ui_state.national_event_window)
							->events.push_back(ui::event_data_wrapper{*c1});
				}
				new_n_event.pop();
				c1 = new_n_event.front();
			}
			// Free national events
			auto* c2 = new_f_n_event.front();
			while(c2) {
				if(world.free_national_event_get_is_major(c2->e)) {
					static_cast<ui::national_event_window<true>*>(ui_state.major_event_window)
							->events.push_back(ui::event_data_wrapper{*c2});
				} else {
					static_cast<ui::national_event_window<false>*>(ui_state.national_event_window)
							->events.push_back(ui::event_data_wrapper{*c2});
				}
				new_f_n_event.pop();
				c2 = new_f_n_event.front();
			}
			// Provincial events
			auto* c3 = new_p_event.front();
			while(c3) {
				static_cast<ui::provincial_event_window*>(ui_state.provincial_event_window)
						->events.push_back(ui::event_data_wrapper{*c3});
				new_p_event.pop();
				c3 = new_p_event.front();
			}
			// Free provincial events
			auto* c4 = new_f_p_event.front();
			while(c4) {
				static_cast<ui::provincial_event_window*>(ui_state.provincial_event_window)
						->events.push_back(ui::event_data_wrapper{*c4});
				new_f_p_event.pop();
				c4 = new_f_p_event.front();
			}
			// Diplomatic messages
			auto* c5 = new_requests.front();
			while(c5) {
				static_cast<ui::diplomacy_request_window*>(ui_state.request_window)->messages.push_back(*c5);
				new_requests.pop();
				c5 = new_requests.front();
			}
			// Log messages
			auto* c6 = new_messages.front();
			while(c6) {
				if(c6->about == local_player_nation) {
					if(user_settings.self_message_settings[int32_t(c6->type)] & message_response::log) {
						static_cast<ui::message_log_window*>(ui_state.msg_log_window)->messages.push_back(*c6);
					}
					if(user_settings.self_message_settings[int32_t(c6->type)] & message_response::popup) {
						static_cast<ui::message_window*>(ui_state.msg_window)->messages.push_back(*c6);
						if(user_settings.self_message_settings[int32_t(c6->type)] & message_response::pause) {
							ui_pause.store(true, std::memory_order_release);
						}
					}
				} else if(notification::nation_is_interesting(*this, c6->about)) {
					if(user_settings.interesting_message_settings[int32_t(c6->type)] & message_response::log) {
						static_cast<ui::message_log_window*>(ui_state.msg_log_window)->messages.push_back(*c6);
					}
					if(user_settings.interesting_message_settings[int32_t(c6->type)] & message_response::popup) {
						static_cast<ui::message_window*>(ui_state.msg_window)->messages.push_back(*c6);
						if(user_settings.interesting_message_settings[int32_t(c6->type)] & message_response::pause) {
							ui_pause.store(true, std::memory_order_release);
						}
					}
				} else {
					if(user_settings.other_message_settings[int32_t(c6->type)] & message_response::log) {
						static_cast<ui::message_log_window*>(ui_state.msg_log_window)->messages.push_back(*c6);
					}
					if(user_settings.other_message_settings[int32_t(c6->type)] & message_response::popup) {
						static_cast<ui::message_window*>(ui_state.msg_window)->messages.push_back(*c6);
						if(user_settings.other_message_settings[int32_t(c6->type)] & message_response::pause) {
							ui_pause.store(true, std::memory_order_release);
						}
					}
					
				}
				new_messages.pop();
				c6 = new_messages.front();
			}
			// Naval Combat Reports
			auto* c7 = naval_battle_reports.front();
			while(c7) {
				if(ui_state.endof_navalcombat_windows.size() == 0) {
					ui_state.endof_navalcombat_windows.push_back(ui::make_element_by_type<ui::naval_combat_end_popup>(*this,
								ui_state.defs_by_name.find("endofnavalcombatpopup")->second.definition));
				}
				//static_cast<ui::naval_combat_window*>(ui_state.navalcombat_windows.back().get())->messages.push_back(*c7);
				static_cast<ui::naval_combat_end_popup*>(ui_state.endof_navalcombat_windows.back().get())->report = *c7;
				ui_state.root->add_child_to_front(std::move(ui_state.endof_navalcombat_windows.back()));
				ui_state.endof_navalcombat_windows.pop_back();
				naval_battle_reports.pop();
				c7 = naval_battle_reports.front();
			}
		}
		if(!static_cast<ui::national_event_window<true>*>(ui_state.major_event_window)->events.empty()) {
			ui_state.major_event_window->set_visible(*this, true);
			ui_state.root->move_child_to_front(ui_state.major_event_window);
		}
		if(!static_cast<ui::national_event_window<false>*>(ui_state.national_event_window)->events.empty()) {
			ui_state.national_event_window->set_visible(*this, true);
			ui_state.root->move_child_to_front(ui_state.national_event_window);
		}
		if(!static_cast<ui::provincial_event_window*>(ui_state.provincial_event_window)->events.empty()) {
			ui_state.provincial_event_window->set_visible(*this, true);
			ui_state.root->move_child_to_front(ui_state.provincial_event_window);
		}
		if(!static_cast<ui::diplomacy_request_window*>(ui_state.request_window)->messages.empty()) {
			ui_state.request_window->set_visible(*this, true);
			ui_state.root->move_child_to_front(ui_state.request_window);
		}
		if(!static_cast<ui::message_window*>(ui_state.msg_window)->messages.empty()) {
			ui_state.msg_window->set_visible(*this, true);
			ui_state.root->move_child_to_front(ui_state.msg_window);
		}

		ui_state.root->impl_on_update(*this);
		map_mode::update_map_mode(*this);

		
		ui_state.rgos_root->impl_on_update(*this);
		ui_state.units_root->impl_on_update(*this);

		ui_state.multi_unit_selection_window->set_visible(*this, selected_armies.size() + selected_navies.size() > 1);


		if(ui_state.last_tooltip && ui_state.tooltip->is_visible()) {
			auto type = ui_state.last_tooltip->has_tooltip(*this);
			if(type == ui::tooltip_behavior::variable_tooltip || type == ui::tooltip_behavior::position_sensitive_tooltip) {
				auto container = text::create_columnar_layout(ui_state.tooltip->internal_layout,
						text::layout_parameters{16, 16, tooltip_width, int16_t(ui_state.root->base_data.size.y - 20), ui_state.tooltip_font, 0,
								text::alignment::left,
								text::text_color::white, true},
						 10);
				ui_state.last_tooltip->update_tooltip(*this, tooltip_probe.relative_location.x, tooltip_probe.relative_location.y,
						container);
				ui_state.tooltip->base_data.size.x = int16_t(container.used_width + 16);
				ui_state.tooltip->base_data.size.y = int16_t(container.used_height + 16);
				if(container.used_width > 0)
					ui_state.tooltip->set_visible(*this, true);
				else
					ui_state.tooltip->set_visible(*this, false);
			}
		}
	}



	if(ui_state.last_tooltip != tooltip_probe.under_mouse) {
		ui_state.last_tooltip = tooltip_probe.under_mouse;
		if(tooltip_probe.under_mouse) {
			auto type = ui_state.last_tooltip->has_tooltip(*this);
			if(type != ui::tooltip_behavior::no_tooltip) {

				auto container = text::create_columnar_layout(ui_state.tooltip->internal_layout,
						text::layout_parameters{16, 16, tooltip_width,int16_t( ui_state.root->base_data.size.y - 20), ui_state.tooltip_font, 0,
								text::alignment::left,
								text::text_color::white, true},
						 10);
				ui_state.last_tooltip->update_tooltip(*this, tooltip_probe.relative_location.x, tooltip_probe.relative_location.y,
						container);
				ui_state.tooltip->base_data.size.x = int16_t(container.used_width + 16);
				ui_state.tooltip->base_data.size.y = int16_t(container.used_height + 16);
				if(container.used_width > 0)
					ui_state.tooltip->set_visible(*this, true);
				else
					ui_state.tooltip->set_visible(*this, false);
			} else {
				ui_state.tooltip->set_visible(*this, false);
			}
		} else {
			ui_state.tooltip->set_visible(*this, false);
		}
	} else if(ui_state.last_tooltip &&
						ui_state.last_tooltip->has_tooltip(*this) == ui::tooltip_behavior::position_sensitive_tooltip) {
		auto container = text::create_columnar_layout(ui_state.tooltip->internal_layout,
				text::layout_parameters{16, 16, tooltip_width, int16_t(ui_state.root->base_data.size.y - 20), ui_state.tooltip_font, 0,
						text::alignment::left,
						text::text_color::white, true},
				 10);
		ui_state.last_tooltip->update_tooltip(*this, tooltip_probe.relative_location.x, tooltip_probe.relative_location.y, container);
		ui_state.tooltip->base_data.size.x = int16_t(container.used_width + 16);
		ui_state.tooltip->base_data.size.y = int16_t(container.used_height + 16);
		if(container.used_width > 0)
			ui_state.tooltip->set_visible(*this, true);
		else
			ui_state.tooltip->set_visible(*this, false);
	}

	if(ui_state.last_tooltip && ui_state.tooltip->is_visible()) {
		// reposition tooltip
		auto target_location = ui::get_absolute_location(*ui_state.last_tooltip);
		if(ui_state.tooltip->base_data.size.y <=
				ui_state.root->base_data.size.y - (target_location.y + ui_state.last_tooltip->base_data.size.y)) {
			ui_state.tooltip->base_data.position.y = int16_t(target_location.y + ui_state.last_tooltip->base_data.size.y);
			ui_state.tooltip->base_data.position.x = std::clamp(
					int16_t(target_location.x + (ui_state.last_tooltip->base_data.size.x / 2) - (ui_state.tooltip->base_data.size.x / 2)),
					int16_t(0), int16_t(ui_state.root->base_data.size.x - ui_state.tooltip->base_data.size.x));
		} else if(ui_state.tooltip->base_data.size.x <=
							ui_state.root->base_data.size.x - (target_location.x + ui_state.last_tooltip->base_data.size.x)) {
			ui_state.tooltip->base_data.position.x = int16_t(target_location.x + ui_state.last_tooltip->base_data.size.x);
			ui_state.tooltip->base_data.position.y = std::clamp(
					int16_t(target_location.y + (ui_state.last_tooltip->base_data.size.y / 2) - (ui_state.tooltip->base_data.size.y / 2)),
					int16_t(0),
					int16_t(ui_state.root->base_data.size.y - ui_state.tooltip->base_data.size.y));
		} else if(ui_state.tooltip->base_data.size.x <= target_location.x) {
			ui_state.tooltip->base_data.position.x = int16_t(target_location.x - ui_state.tooltip->base_data.size.x);
			ui_state.tooltip->base_data.position.y = std::clamp(
					int16_t(target_location.y + (ui_state.last_tooltip->base_data.size.y / 2) - (ui_state.tooltip->base_data.size.y / 2)),
					int16_t(0), int16_t(ui_state.root->base_data.size.y - ui_state.tooltip->base_data.size.y));
		} else if(ui_state.tooltip->base_data.size.y <= target_location.y) {
			ui_state.tooltip->base_data.position.y = int16_t(target_location.y - ui_state.tooltip->base_data.size.y);
			ui_state.tooltip->base_data.position.x = std::clamp(
					int16_t(target_location.x + (ui_state.last_tooltip->base_data.size.x / 2) - (ui_state.tooltip->base_data.size.x / 2)),
					int16_t(0), int16_t(ui_state.root->base_data.size.x - ui_state.tooltip->base_data.size.x));
		} else {
			ui_state.tooltip->base_data.position.x = std::clamp(
					int16_t(target_location.x + (ui_state.last_tooltip->base_data.size.x / 2) - (ui_state.tooltip->base_data.size.x / 2)),
					int16_t(0), int16_t(ui_state.root->base_data.size.x - ui_state.tooltip->base_data.size.x));
			ui_state.tooltip->base_data.position.y = std::clamp(
					int16_t(target_location.y + (ui_state.last_tooltip->base_data.size.y / 2) - (ui_state.tooltip->base_data.size.y / 2)),
					int16_t(0), int16_t(ui_state.root->base_data.size.y - ui_state.tooltip->base_data.size.y));
		}
	}

	// Have to have the map tooltip down here, and we must check both of the probes
	// Not doing this causes the map tooltip to override some of the regular tooltips (namely the score tooltips)
	if(!mouse_probe.under_mouse && !tooltip_probe.under_mouse) {
		dcon::province_id prov = map_state.get_province_under_mouse(*this, int32_t(mouse_x_position), int32_t(mouse_y_position), x_size, y_size);

		if(map_state.get_zoom() <= 5)
			prov = dcon::province_id{};

		if(prov) {
			auto container = text::create_columnar_layout(ui_state.tooltip->internal_layout,
					text::layout_parameters{16, 16, tooltip_width, int16_t(ui_state.root->base_data.size.y - 20), ui_state.tooltip_font, 0, text::alignment::left, text::text_color::white, true},
					20);

			// Enable this and tooltip will follow the cursor
			// ui_state.tooltip->base_data.position.x = int16_t(mouse_x_position / user_settings.ui_scale);
			// ui_state.tooltip->base_data.position.y = int16_t(mouse_y_position / user_settings.ui_scale);
			

			ui::populate_map_tooltip(*this, container, prov);

			ui_state.tooltip->base_data.size.x = int16_t(container.used_width + 16);
			ui_state.tooltip->base_data.size.y = int16_t(container.used_height + 16);
			if(container.used_width > 0) {
				// This block positions the tooltip somewhat under the province centroid

					auto mid_point = world.province_get_mid_point(prov);
					auto map_pos = map_state.normalize_map_coord(mid_point);
					auto screen_size =
							glm::vec2{float(x_size / user_settings.ui_scale), float(y_size / user_settings.ui_scale)};
					glm::vec2 screen_pos;
					if(!map_state.map_to_screen(*this, map_pos, screen_size, screen_pos)) {
						ui_state.tooltip->set_visible(*this, false);
					} else {
						ui_state.tooltip->base_data.position =
								ui::xy_pair{int16_t(screen_pos.x - container.used_width / 2 - 8), int16_t(screen_pos.y + 3.5f * map_state.get_zoom())};
						ui_state.tooltip->set_visible(*this, true);
					}

				// Alternatively: just make it visible
				// ui_state.tooltip->set_visible(*this, true);
				//
			} else {
				ui_state.tooltip->set_visible(*this, false);
			}
		} else {
			ui_state.tooltip->set_visible(*this, false);
		}
	}

	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if(bg_gfx_id) {
		// Render default background
		glUseProgram(open_gl.ui_shader_program);
		glUniform1f(ogl::parameters::screen_width, float(x_size) / user_settings.ui_scale);
		glUniform1f(ogl::parameters::screen_height, float(y_size) / user_settings.ui_scale);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(0, 0, x_size, y_size);
		glDepthRange(-1.0, 1.0);
		auto& gfx_def = ui_defs.gfx[bg_gfx_id];
		if(gfx_def.primary_texture_handle) {
			ogl::render_textured_rect(*this, ui::get_color_modification(false, false, false), 0.f, 0.f, float(x_size), float(y_size),
					ogl::get_texture_handle(*this, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					ui::rotation::upright, gfx_def.is_vertically_flipped());
		}
	}

	map_state.render(*this, x_size, y_size);

	// UI rendering
	glUseProgram(open_gl.ui_shader_program);
	glUniform1f(ogl::parameters::screen_width, float(x_size) / user_settings.ui_scale);
	glUniform1f(ogl::parameters::screen_height, float(y_size) / user_settings.ui_scale);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, x_size, y_size);
	glDepthRange(-1.0, 1.0);

	ui_state.under_mouse = mouse_probe.under_mouse;
	ui_state.scroll_target = ui_state.root->impl_probe_mouse(*this,
		int32_t(mouse_x_position / user_settings.ui_scale),
		int32_t(mouse_y_position / user_settings.ui_scale),
		ui::mouse_probe_type::scroll).under_mouse;

	ui_state.relative_mouse_location = mouse_probe.relative_location;

	if(map_state.get_zoom() > 5) {
		if(map_state.active_map_mode == map_mode::mode::rgo_output) {
			ui_state.rgos_root->impl_render(*this, 0, 0);
		} else {
			ui_state.units_root->impl_render(*this, 0, 0);
		}
		if(ui_state.unit_details_box->is_visible()) {
			ui_state.unit_details_box->impl_render(*this, ui_state.unit_details_box->base_data.position.x, ui_state.unit_details_box->base_data.position.y);
		}
	}
	ui_state.root->impl_render(*this, 0, 0);
	if(ui_state.tooltip->is_visible()) {
		ui_state.tooltip->impl_render(*this, ui_state.tooltip->base_data.position.x, ui_state.tooltip->base_data.position.y);
	}
}
void state::on_create() {
	// Clear "center" property so they don't look messed up!
	ui_defs.gui[ui_state.defs_by_name.find("state_info")->second.definition].flags &= ~ui::element_data::orientation_mask;
	ui_defs.gui[ui_state.defs_by_name.find("production_goods_name")->second.definition].flags &=
			~ui::element_data::orientation_mask;
	ui_defs.gui[ui_state.defs_by_name.find("factory_info")->second.definition].flags &= ~ui::element_data::orientation_mask;
	ui_defs.gui[ui_state.defs_by_name.find("new_factory_option")->second.definition].flags &= ~ui::element_data::orientation_mask;
	ui_defs.gui[ui_state.defs_by_name.find("ledger_legend_entry")->second.definition].flags &= ~ui::element_data::orientation_mask;
	ui_defs.gui[ui_state.defs_by_name.find("project_info")->second.definition].flags &= ~ui::element_data::orientation_mask;
	// Allow mobility of those windows who can be moved, and shall be moved
	ui_defs.gui[ui_state.defs_by_name.find("pop_details_win")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("trade_flow")->second.definition].data.window.flags |= ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("event_election_window")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("invest_project_window")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	// if(!user_settings.use_new_ui) {	TODO - this should only trigger if youre not on faithful mode, in Vic2, none of these
	// windows are moveable
	ui_defs.gui[ui_state.defs_by_name.find("ledger")->second.definition].data.window.flags |= ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("province_view")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("releaseconfirm")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("build_factory")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("defaultdiplomacydialog")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("gpselectdiplomacydialog")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("makecbdialog")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("declarewardialog")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("setuppeacedialog")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("setupcrisisbackdowndialog")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("endofnavalcombatpopup")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	ui_defs.gui[ui_state.defs_by_name.find("endoflandcombatpopup")->second.definition].data.window.flags |=
			ui::window_data::is_moveable_mask;
	//}
	// Find the object id for the main_bg displayed (so we display it before the map)
	bg_gfx_id = ui_defs.gui[ui_state.defs_by_name.find("bg_main_menus")->second.definition].data.image.gfx_object;

	ui_state.unit_details_box = ui::make_element_by_type<ui::grid_box>(*this, ui_state.defs_by_name.find("alice_grid_panel")->second.definition);
	ui_state.unit_details_box->set_visible(*this, false);

	world.for_each_province([&](dcon::province_id id) {
		if(world.province_get_port_to(id)) {
			auto ptr = ui::make_element_by_type<ui::port_window>(*this, "alice_port_icon");
			static_cast<ui::port_window*>(ptr.get())->set_province(*this, id);
			ui_state.units_root->add_child_to_front(std::move(ptr));
		}
	});
	world.for_each_province([&](dcon::province_id id) {
		auto ptr = ui::make_element_by_type<ui::unit_counter_window>(*this, "alice_map_unit");
		static_cast<ui::unit_counter_window*>(ptr.get())->prov = id;
		ui_state.units_root->add_child_to_front(std::move(ptr));	
	});
	world.for_each_province([&](dcon::province_id id) {
		auto ptr = ui::make_element_by_type<ui::rgo_icon>(*this, "alice_rgo_mapicon");
		static_cast<ui::rgo_icon*>(ptr.get())->content = id;
		ui_state.rgos_root->add_child_to_front(std::move(ptr));
	});

	{
		auto window = ui::make_element_by_type<ui::console_window>(*this, "console_wnd");
		ui_state.console_window = window.get();
		window->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(window));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::outliner_window>(*this, "outliner");
		ui_state.outliner_window = new_elm.get();
		new_elm->impl_on_update(*this);
		ui_state.root->add_child_to_front(std::move(new_elm));
		// Has to be created AFTER the outliner window
		// The topbar has this button within, however since the button isn't properly displayed, it is better to make
		// it into an independent element of it's own, living freely on the UI root so it can be flexibly moved around when
		// the window is resized for example.
		for(size_t i = ui_defs.gui.size(); i-- > 0;) {
			auto gdef = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i));
			if(to_string_view(ui_defs.gui[gdef].name) == "topbar_outlinerbutton_bg") {
				auto new_bg = ui::make_element_by_type<ui::outliner_button>(*this, gdef);
				ui_state.root->add_child_to_front(std::move(new_bg));
				break;
			}
		}
		// Then create button atop
		for(size_t i = ui_defs.gui.size(); i-- > 0;) {
			auto gdef = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i));
			if(to_string_view(ui_defs.gui[gdef].name) == "topbar_outlinerbutton") {
				auto new_btn = ui::make_element_by_type<ui::outliner_button>(*this, gdef);
				new_btn->impl_on_update(*this);
				ui_state.root->add_child_to_front(std::move(new_btn));
				break;
			}
		}
	}
	{
		auto new_elm = ui::make_element_by_type<ui::minimap_container_window>(*this, "menubar");
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::minimap_picture_window>(*this, "minimap_pic");
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::province_view_window>(*this, "province_view");
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm_army = ui::make_element_by_type<ui::unit_details_window<dcon::army_id>>(*this, "sup_unit_status");
		ui_state.army_status_window = new_elm_army.get();
		new_elm_army->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(new_elm_army));

		auto new_elm_navy = ui::make_element_by_type<ui::unit_details_window<dcon::navy_id>>(*this, "sup_unit_status");
		ui_state.navy_status_window = new_elm_navy.get();
		new_elm_navy->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(new_elm_navy));
	}
	{
		auto mselection = ui::make_element_by_type<ui::mulit_unit_selection_panel>(*this, "alice_multi_unitpanel");
		ui_state.multi_unit_selection_window = mselection.get();
		mselection->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(mselection));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::election_event_window>(*this, "event_election_window");
		ui_state.election_window = new_elm.get();
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::diplomacy_request_window>(*this, "defaultdialog");
		ui_state.request_window = new_elm.get();
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::message_window>(*this, "defaultpopup");
		ui_state.msg_window = new_elm.get();
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::national_event_window<true>>(*this, "event_major_window");
		ui_state.major_event_window = new_elm.get();
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::national_event_window<false>>(*this, "event_country_window");
		ui_state.national_event_window = new_elm.get();
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::provincial_event_window>(*this, "event_province_window");
		ui_state.provincial_event_window = new_elm.get();
		ui_state.root->add_child_to_front(std::move(new_elm));
	}

	{
		auto new_elm = ui::make_element_by_type<ui::topbar_window>(*this, "topbar");
		new_elm->impl_on_update(*this);
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::country_selection_window>(*this, "country_selection_panel");
		new_elm->impl_on_update(*this);
		new_elm->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::naval_combat_end_popup>(*this, "endofnavalcombatpopup");
		new_elm->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::land_combat_end_popup>(*this, "endoflandcombatpopup");
		new_elm->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::naval_combat_window>(*this, "naval_combat");
		new_elm->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::land_combat_window>(*this, "land_combat");
		new_elm->set_visible(*this, false);
		ui_state.root->add_child_to_front(std::move(new_elm));
	}

	map_mode::set_map_mode(*this, map_mode::mode::political);

	if(user_settings.use_classic_fonts) {
		ui_state.tooltip_font = text::name_into_font_id(*this, "vic_18_black");
	} else {
		ui_state.tooltip_font = text::name_into_font_id(*this, "ToolTip_Font");
	}

	ui_state.rgos_root->impl_on_update(*this);
	ui_state.units_root->impl_on_update(*this);
}
//
// string pool functions
//

std::string_view state::to_string_view(dcon::text_key tag) const {
	if(!tag)
		return std::string_view();
	auto start_position = text_data.data() + tag.index();
	auto data_size = text_data.size();
	auto end_position = start_position;
	for(; end_position < text_data.data() + data_size; ++end_position) {
		if(*end_position == 0)
			break;
	}
	return std::string_view(text_data.data() + tag.index(), size_t(end_position - start_position));
}

dcon::text_key state::add_to_pool_lowercase(std::string const& new_text) {
	auto res = add_to_pool(new_text);
	for(auto i = 0; i < int32_t(new_text.length()); ++i) {
		text_data[res.index() + i] = char(tolower(text_data[res.index() + i]));
	}
	return res;
}
dcon::text_key state::add_to_pool_lowercase(std::string_view new_text) {
	auto res = add_to_pool(new_text);
	for(auto i = 0; i < int32_t(new_text.length()); ++i) {
		text_data[res.index() + i] = char(tolower(text_data[res.index() + i]));
	}
	return res;
}
dcon::text_key state::add_to_pool(std::string const& new_text) {
	auto start = text_data.size();
	auto size = new_text.length();
	if(size == 0)
		return dcon::text_key();
	text_data.resize(start + size + 1, char(0));
	std::copy_n(new_text.c_str(), size + 1, text_data.data() + start);
	return dcon::text_key(uint32_t(start));
}
dcon::text_key state::add_to_pool(std::string_view new_text) {
	auto start = text_data.size();
	auto length = new_text.length();
	if(length == 0)
		return dcon::text_key();
	text_data.resize(start + length + 1, char(0));
	std::copy_n(new_text.data(), length, text_data.data() + start);
	text_data.back() = 0;
	return dcon::text_key(uint32_t(start));
}

dcon::text_key state::add_unique_to_pool(std::string const& new_text) {
	if(new_text.length() > 0) {
		auto search_result = std::search(text_data.data(), text_data.data() + text_data.size(),
				std::boyer_moore_horspool_searcher(new_text.c_str(), new_text.c_str() + new_text.length() + 1));
		if(search_result != text_data.data() + text_data.size()) {
			return dcon::text_key(uint32_t(search_result - text_data.data()));
		} else {
			return add_to_pool(new_text);
		}
	} else {
		return dcon::text_key();
	}
}

dcon::unit_name_id state::add_unit_name(std::string_view text) {
	auto start = unit_names.size();
	auto length = text.length();
	if(length == 0)
		return dcon::unit_name_id();

	unit_names.resize(start + length + 1, char(0));
	std::copy_n(text.data(), length, unit_names.data() + start);
	unit_names.back() = 0;
	unit_names_indices.push_back(int32_t(start));
	return dcon::unit_name_id(dcon::unit_name_id::value_base_t(unit_names_indices.size() - 1));
}
std::string_view state::to_string_view(dcon::unit_name_id tag) const {
	if(!tag)
		return std::string_view();
	auto start_position = unit_names.data() + unit_names_indices[tag.index()];
	auto data_size = unit_names.size();
	auto end_position = start_position;
	for(; end_position < unit_names.data() + data_size; ++end_position) {
		if(*end_position == 0)
			break;
	}
	return std::string_view(unit_names.data() + unit_names_indices[tag.index()], size_t(end_position - start_position));
}

dcon::trigger_key state::commit_trigger_data(std::vector<uint16_t> data) {
	if(data.size() == 0) {
		if(trigger_data_indices.empty())
			trigger_data_indices.push_back(int32_t(0));
		return dcon::trigger_key();
	}

	auto search_result = std::search(trigger_data.data(), trigger_data.data() + trigger_data.size(),
			std::boyer_moore_horspool_searcher(data.data(), data.data() + data.size()));
	if(search_result != trigger_data.data() + trigger_data.size()) {
		auto const start = search_result - trigger_data.data();
		auto it = std::find(trigger_data_indices.begin(), trigger_data_indices.end(), int32_t(start));
		if(it != trigger_data_indices.end()) {
			auto d = std::distance(trigger_data_indices.begin(), it);
			return dcon::trigger_key(dcon::trigger_key::value_base_t(d));
		} else {
			trigger_data_indices.push_back(int32_t(start));
			assert(trigger_data_indices.size() <= std::numeric_limits<uint16_t>::max());
			return dcon::trigger_key(dcon::trigger_key::value_base_t(trigger_data_indices.size() - 1));
		}
	} else {
		auto start = trigger_data.size();
		auto size = data.size();
		trigger_data.resize(start + size, uint16_t(0));
		std::copy_n(data.data(), size, trigger_data.data() + start);
		trigger_data_indices.push_back(int32_t(start));
		assert(trigger_data_indices.size() <= std::numeric_limits<uint16_t>::max());
		return dcon::trigger_key(dcon::trigger_key::value_base_t(trigger_data_indices.size() - 1));
	}
}

dcon::effect_key state::commit_effect_data(std::vector<uint16_t> data) {
	if(data.size() == 0)
		return dcon::effect_key();

	auto start = effect_data.size();
	auto size = data.size();

	effect_data.resize(start + size, uint16_t(0));
	std::copy_n(data.data(), size, effect_data.data() + start);
	return dcon::effect_key(uint16_t(start));
}

void state::save_user_settings() const {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	simple_fs::write_file(settings_location, NATIVE("user_settings.dat"), reinterpret_cast<char const*>(&user_settings),
			uint32_t(sizeof(user_settings_s)));
}
void state::load_user_settings() {
	auto settings_location = simple_fs::get_or_create_settings_directory();
	auto settings_file = open_file(settings_location, NATIVE("user_settings.dat"));
	if(settings_file) {
		auto content = view_contents(*settings_file);
		std::memcpy(&user_settings, content.data, std::min(uint32_t(sizeof(user_settings_s)), content.file_size));

		user_settings.interface_volume = std::clamp(user_settings.interface_volume, 0.0f, 1.0f);
		user_settings.music_volume = std::clamp(user_settings.music_volume, 0.0f, 1.0f);
		user_settings.effects_volume = std::clamp(user_settings.effects_volume, 0.0f, 1.0f);
		user_settings.master_volume = std::clamp(user_settings.master_volume, 0.0f, 1.0f);
	}
}

void state::update_ui_scale(float new_scale) {
	user_settings.ui_scale = new_scale;
	ui_state.root->base_data.size.x = int16_t(x_size / user_settings.ui_scale);
	ui_state.root->base_data.size.y = int16_t(y_size / user_settings.ui_scale);
	// TODO move windows
}

void list_pop_types(sys::state& state, parsers::scenario_building_context& context) {
	auto root = get_root(state.common_fs);
	auto poptypes = open_directory(root, NATIVE("poptypes"));

	for(auto& file : simple_fs::list_files(poptypes, NATIVE(".txt"))) {
		auto full_name = get_full_name(file);
		auto last = full_name.c_str() + full_name.length();
		auto first = full_name.c_str();
		for(; last > first; --last) {
			if(*last == NATIVE('.'))
				break;
		}
		auto start_of_name = last;
		for(; start_of_name >= first; --start_of_name) {
			if(*start_of_name == NATIVE('\\') || *start_of_name == NATIVE('/')) {
				++start_of_name;
				break;
			}
		}
		auto utf8typename = simple_fs::native_to_utf8(native_string_view(start_of_name, last - start_of_name));

		auto name_id = text::find_or_add_key(context.state, utf8typename);
		auto type_id = state.world.create_pop_type();
		state.world.pop_type_set_name(type_id, name_id);
		context.map_of_poptypes.insert_or_assign(std::string(utf8typename), type_id);
	}
}

void state::open_diplomacy(dcon::nation_id target) {
	Cyto::Any payload = ui::element_selection_wrapper<dcon::nation_id>{target};
	if(ui_state.diplomacy_subwindow != nullptr) {
		if(ui_state.topbar_subwindow != nullptr) {
			ui_state.topbar_subwindow->set_visible(*this, false);
		}
		ui_state.topbar_subwindow = ui_state.diplomacy_subwindow;
		ui_state.diplomacy_subwindow->set_visible(*this, true);
		ui_state.root->move_child_to_front(ui_state.diplomacy_subwindow);
		ui_state.diplomacy_subwindow->impl_get(*this, payload);
	}
}

void state::load_scenario_data() {
	parsers::error_handler err("");

	parsers::scenario_building_context context(*this);

	text::load_text_data(*this, 2); // 2 = English
	ui::load_text_gui_definitions(*this, context.gfx_context, err);

	auto root = get_root(common_fs);
	auto common = open_directory(root, NATIVE("common"));

	auto map = open_directory(root, NATIVE("map"));
	// parse default.map
	{
		auto def_map_file = open_file(map, NATIVE("default.map"));
		if(def_map_file) {
			auto content = view_contents(*def_map_file);
			err.file_name = "default.map";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_default_map_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File map/default.map could not be opened\n";
		}
	}
	// parse definition.csv
	{
		auto def_csv_file = open_file(map, NATIVE("definition.csv"));
		if(def_csv_file) {
			auto content = view_contents(*def_csv_file);
			err.file_name = "definition.csv";
			parsers::read_map_colors(content.data, content.data + content.file_size, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File map/definition.csv could not be opened\n";
		}
	}

	{
		err.file_name = "adjacencies.csv";
		auto adj_csv_file = open_file(map, NATIVE("adjacencies.csv"));
		if(adj_csv_file) {
			auto adj_content = view_contents(*adj_csv_file);
			parsers::read_map_adjacency(adj_content.data, adj_content.data + adj_content.file_size, err, context);
		}
	}

	/*
	240,208,1 Tsushima --> 240,208,0 Nagasaki
	128,65,97 Fehmarn--> 128,65,96 Kiel
	*/

	if(auto it = context.map_color_to_province_id.find(sys::pack_color(240, 208, 0));
			it != context.map_color_to_province_id.end() &&
			context.map_color_to_province_id.find(sys::pack_color(240, 208, 1)) == context.map_color_to_province_id.end()) {
		context.map_color_to_province_id.insert_or_assign(sys::pack_color(240, 208, 1), it->second);
	}
	if(auto it = context.map_color_to_province_id.find(sys::pack_color(128, 65, 96));
			it != context.map_color_to_province_id.end() &&
			context.map_color_to_province_id.find(sys::pack_color(128, 65, 97)) == context.map_color_to_province_id.end()) {
		context.map_color_to_province_id.insert_or_assign(sys::pack_color(128, 65, 97), it->second);
	}

	/*
	// DO NOT RESTORE
	// 1, 222, 200 --> 51, 221, 251 -- randomly misplaced sea
	// Apparently this color is not just found in the corner of the map, but scattered around it as well
	// thus substituting a valid province color for it causes the sea tile to connect to all sorts of place
	// (a very undesirable behavior
	if(auto it = context.map_color_to_province_id.find(sys::pack_color(51, 221, 251));
			it != context.map_color_to_province_id.end() &&
			context.map_color_to_province_id.find(sys::pack_color(1, 222, 200)) == context.map_color_to_province_id.end()) {
		context.map_color_to_province_id.insert_or_assign(sys::pack_color(1, 222, 200), it->second);
	}
	*/

	// 94, 53, 41 --> 89, 202, 202 -- random dots in the sea tiles
	// 247, 248, 245 -- > 89, 202, 202


	if(auto it = context.map_color_to_province_id.find(sys::pack_color(89, 202, 202));
			it != context.map_color_to_province_id.end() &&
			context.map_color_to_province_id.find(sys::pack_color(94, 53, 41)) == context.map_color_to_province_id.end()) {
		context.map_color_to_province_id.insert_or_assign(sys::pack_color(94, 53, 41), it->second);
	}
	if(auto it = context.map_color_to_province_id.find(sys::pack_color(89, 202, 202));
			it != context.map_color_to_province_id.end() &&
			context.map_color_to_province_id.find(sys::pack_color(247, 248, 245)) == context.map_color_to_province_id.end()) {
		context.map_color_to_province_id.insert_or_assign(sys::pack_color(247, 248, 245), it->second);
	}


	std::thread map_loader([&]() { map_state.load_map_data(context); });

	// Read national tags from countries.txt
	{
		auto countries = open_file(common, NATIVE("countries.txt"));
		if(countries) {
			auto content = view_contents(*countries);
			err.file_name = "countries.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_identity_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/countries.txt could not be opened\n";
		}
	}
	// read religions from religion.txt
	{
		auto religion = open_file(common, NATIVE("religion.txt"));
		if(religion) {
			auto content = view_contents(*religion);
			err.file_name = "religion.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_religion_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/religion.txt could not be opened\n";
		}
	}
	// read cultures from cultures.txt
	{
		auto cultures = open_file(common, NATIVE("cultures.txt"));
		if(cultures) {
			auto content = view_contents(*cultures);
			err.file_name = "cultures.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_culture_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/cultures.txt could not be opened\n";
		}
	}
	// read commodities from goods.txt
	{
		// FIRST: make sure that we have a money good
		if(world.commodity_size() == 0) {
			// create money
			auto money_id = world.create_commodity();
			assert(money_id.index() == 0);
		}
		auto goods = open_file(common, NATIVE("goods.txt"));
		if(goods) {
			auto content = view_contents(*goods);
			err.file_name = "goods.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_goods_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/goods.txt could not be opened\n";
		}
	}
	// read buildings.text
	// world.factory_type_resize_construction_costs(world.commodity_size());
	{
		auto buildings = open_file(common, NATIVE("buildings.txt"));
		if(buildings) {
			auto content = view_contents(*buildings);
			err.file_name = "buildings.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_building_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/buildings.txt could not be opened\n";
		}
	}
	// pre parse ideologies.txt
	{
		context.ideologies_file = open_file(common, NATIVE("ideologies.txt"));
		if(context.ideologies_file) {
			auto content = view_contents(*context.ideologies_file);
			err.file_name = "ideologies.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_ideology_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/ideologies.txt could not be opened\n";
		}
	}
	// pre parse issues.txt
	{
		context.issues_file = open_file(common, NATIVE("issues.txt"));
		if(context.issues_file) {
			auto content = view_contents(*context.issues_file);
			err.file_name = "issues.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_issues_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/ideologies.txt could not be opened\n";
		}
	}
	// parse governments.txt
	{
		auto governments = open_file(common, NATIVE("governments.txt"));
		if(governments) {
			auto content = view_contents(*governments);
			err.file_name = "governments.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_governments_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/governments.txt could not be opened\n";
		}
	}
	// pre parse cb_types.txt
	{
		context.cb_types_file = open_file(common, NATIVE("cb_types.txt"));
		if(context.cb_types_file) {
			auto content = view_contents(*context.cb_types_file);
			err.file_name = "cb_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_cb_types_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/cb_types.txt could not be opened\n";
		}
	}
	// parse traits.txt
	{
		auto traits = open_file(common, NATIVE("traits.txt"));
		if(traits) {
			auto content = view_contents(*traits);
			err.file_name = "traits.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_traits_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/traits.txt could not be opened\n";
		}
	}
	// pre parse crimes.txt
	{
		context.crimes_file = open_file(common, NATIVE("crime.txt"));
		if(context.crimes_file) {
			auto content = view_contents(*context.crimes_file);
			err.file_name = "crime.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_crimes_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/crime.txt could not be opened\n";
		}
	}
	// pre parse triggered_modifiers.txt
	{
		context.triggered_modifiers_file = open_file(common, NATIVE("triggered_modifiers.txt"));
		if(context.triggered_modifiers_file) {
			auto content = view_contents(*context.triggered_modifiers_file);
			err.file_name = "triggered_modifiers.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_triggered_modifiers_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/triggered_modifiers.txt could not be opened\n";
		}
	}
	// parse nationalvalues.txt
	{
		auto nv_file = open_file(common, NATIVE("nationalvalues.txt"));
		if(nv_file) {
			auto content = view_contents(*nv_file);
			err.file_name = "nationalvalues.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_values_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/nationalvalues.txt could not be opened\n";
		}
	}
	// parse static_modifiers.txt
	{
		auto sm_file = open_file(common, NATIVE("static_modifiers.txt"));
		if(sm_file) {
			auto content = view_contents(*sm_file);
			err.file_name = "static_modifiers.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_static_modifiers_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/static_modifiers.txt could not be opened\n";
		}
	}
	// parse event_modifiers.txt
	{
		auto em_file = open_file(common, NATIVE("event_modifiers.txt"));
		if(em_file) {
			auto content = view_contents(*em_file);
			err.file_name = "event_modifiers.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_event_modifiers_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/event_modifiers.txt could not be opened\n";
		}
	}
	// read defines.lua
	{
		auto defines_file = open_file(common, NATIVE("defines.lua"));
		if(defines_file) {
			auto content = view_contents(*defines_file);
			err.file_name = "defines.lua";
			defines.parse_file(*this, std::string_view(content.data, content.data + content.file_size), err);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/defines.lua could not be opened\n";
		}
	}
	// gather names of poptypes
	list_pop_types(*this, context);
	// pre parse rebel_types.txt
	{
		context.rebel_types_file = open_file(common, NATIVE("rebel_types.txt"));
		if(context.rebel_types_file) {
			auto content = view_contents(*context.rebel_types_file);
			err.file_name = "rebel_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_rebel_types_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/rebel_types.txt could not be opened\n";
		}
	}

	// parse terrain.txt
	{
		auto terrain_file = open_file(map, NATIVE("terrain.txt"));
		if(terrain_file) {
			auto content = view_contents(*terrain_file);
			err.file_name = "terrain.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_terrain_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File map/terrain.txt could not be opened\n";
		}
	}
	// parse region.txt
	{
		auto region_file = open_file(map, NATIVE("region.txt"));
		if(region_file) {
			auto content = view_contents(*region_file);
			err.file_name = "region.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_region_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File map/region.txt could not be opened\n";
		}
	}
	// parse continent.txt
	{
		auto continent_file = open_file(map, NATIVE("continent.txt"));
		if(continent_file) {
			auto content = view_contents(*continent_file);
			err.file_name = "continent.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_continent_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File map/continent.txt could not be opened\n";
		}
	}
	// parse climate.txt
	{
		auto climate_file = open_file(map, NATIVE("climate.txt"));
		if(climate_file) {
			auto content = view_contents(*climate_file);
			err.file_name = "climate.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_climate_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File map/climate.txt could not be opened\n";
		}
	}
	// parse technology.txt
	{
		auto tech_file = open_file(common, NATIVE("technology.txt"));
		if(tech_file) {
			auto content = view_contents(*tech_file);
			err.file_name = "technology.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_technology_main_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/technology.txt could not be opened\n";
		}
	}
	// pre parse inventions
	{
		auto inventions = open_directory(root, NATIVE("inventions"));
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::army};
			auto i_file = open_file(inventions, NATIVE("army_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "army_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			} else {
				err.fatal = true;
				err.accumulated_errors += "File common/army_inventions.txt could not be opened\n";
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::navy};
			auto i_file = open_file(inventions, NATIVE("navy_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "navy_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			} else {
				err.fatal = true;
				err.accumulated_errors += "File common/navy_inventions.txt could not be opened\n";
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::commerce};
			auto i_file = open_file(inventions, NATIVE("commerce_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "commerce_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			} else {
				err.fatal = true;
				err.accumulated_errors += "File common/commerce_inventions.txt could not be opened\n";
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::culture};
			auto i_file = open_file(inventions, NATIVE("culture_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "culture_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			} else {
				err.fatal = true;
				err.accumulated_errors += "File common/culture_inventions.txt could not be opened\n";
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::industry};
			auto i_file = open_file(inventions, NATIVE("industry_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "industry_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			} else {
				err.fatal = true;
				err.accumulated_errors += "File common/industry_inventions.txt could not be opened\n";
			}
		}
	}
	// load unit type definitions
	{
		parsers::make_base_units(context);

		auto units = open_directory(root, NATIVE("units"));
		for(auto unit_file : simple_fs::list_files(units, NATIVE(".txt"))) {
			auto opened_file = open_file(unit_file);
			if(opened_file) {
				auto content = view_contents(*opened_file);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_unit_file(gen, err, context);
			}
		}
	}
	// make space in arrays

	world.national_identity_resize_unit_names_count(uint32_t(military_definitions.unit_base_definitions.size()));
	world.national_identity_resize_unit_names_first(uint32_t(military_definitions.unit_base_definitions.size()));

	world.political_party_resize_party_issues(uint32_t(culture_definitions.party_issues.size()));

	world.province_resize_party_loyalty(world.ideology_size());

	world.pop_type_resize_everyday_needs(world.commodity_size());
	world.pop_type_resize_luxury_needs(world.commodity_size());
	world.pop_type_resize_life_needs(world.commodity_size());
	world.pop_type_resize_ideology(world.ideology_size());
	world.pop_type_resize_issues(world.issue_option_size());
	world.pop_type_resize_promotion(world.pop_type_size());

	world.national_focus_resize_production_focus(world.commodity_size());

	world.technology_resize_activate_building(world.factory_type_size());
	world.technology_resize_activate_unit(uint32_t(military_definitions.unit_base_definitions.size()));

	world.invention_resize_activate_building(world.factory_type_size());
	world.invention_resize_activate_unit(uint32_t(military_definitions.unit_base_definitions.size()));
	world.invention_resize_activate_crime(uint32_t(culture_definitions.crimes.size()));

	world.rebel_type_resize_government_change(uint32_t(culture_definitions.governments.size()));

	world.nation_resize_active_inventions(world.invention_size());
	world.nation_resize_active_technologies(world.technology_size());
	world.nation_resize_upper_house(world.ideology_size());

	world.national_identity_resize_government_flag_type(uint32_t(culture_definitions.governments.size()));

	// load country files
	world.for_each_national_identity([&](dcon::national_identity_id i) {
		auto country_file = open_file(common, simple_fs::win1250_to_native(context.file_names_for_idents[i]));
		if(country_file) {
			parsers::country_file_context c_context{context, i};
			auto content = view_contents(*country_file);
			err.file_name = context.file_names_for_idents[i];
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_country_file(gen, err, c_context);
		}
	});

	// load province history files

	auto history = open_directory(root, NATIVE("history"));
	{
		auto prov_history = open_directory(history, NATIVE("provinces"));
		for(auto subdir : list_subdirectories(prov_history)) {
			for(auto prov_file : list_files(subdir, NATIVE(".txt"))) {
				auto file_name = simple_fs::native_to_utf8(get_full_name(prov_file));
				auto name_begin = file_name.c_str();
				auto name_end = name_begin + file_name.length();
				for(; --name_end > name_begin;) {
					if(isdigit(*name_end))
						break;
				}
				auto value_start = name_end;
				for(; value_start > name_begin; --value_start) {
					if(!isdigit(*value_start))
						break;
				}
				++value_start;
				++name_end;

				err.file_name = file_name;
				auto province_id = parsers::parse_int(std::string_view(value_start, name_end - value_start), 0, err);
				if(province_id > 0 && uint32_t(province_id) < context.original_id_to_prov_id_map.size()) {
					auto opened_file = open_file(prov_file);
					if(opened_file) {
						auto pid = context.original_id_to_prov_id_map[province_id];
						parsers::province_file_context pf_context{context, pid};
						auto content = view_contents(*opened_file);
						parsers::token_generator gen(content.data, content.data + content.file_size);
						parsers::parse_province_history_file(gen, err, pf_context);
					}
				}
			}
		}
	}

	culture::set_default_issue_and_reform_options(*this);

	// load pop history files
	{
		auto pop_history = open_directory(history, NATIVE("pops"));
		auto startdate = sys::date(0).to_ymd(start_date);
		auto start_dir_name =
				std::to_string(startdate.year) + "." + std::to_string(startdate.month) + "." + std::to_string(startdate.day);
		auto date_directory = open_directory(pop_history, simple_fs::utf8_to_native(start_dir_name));

		for(auto pop_file : list_files(date_directory, NATIVE(".txt"))) {
			auto opened_file = open_file(pop_file);
			if(opened_file) {
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_pop_history_file(gen, err, context);
			}
		}
	}
	// load poptype definitions
	{
		auto poptypes = open_directory(root, NATIVE("poptypes"));
		for(auto pr : context.map_of_poptypes) {
			auto opened_file = open_file(poptypes, simple_fs::utf8_to_native(pr.first + ".txt"));
			if(opened_file) {
				err.file_name = pr.first + ".txt";
				auto content = view_contents(*opened_file);
				parsers::poptype_context inner_context{context, pr.second};
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_poptype_file(gen, err, inner_context);
			}
		}
	}

	// load ideology contents
	{
		err.file_name = "ideologies.txt";
		for(auto& pr : context.map_of_ideologies) {
			parsers::individual_ideology_context new_context{context, pr.second.id};
			parsers::parse_individual_ideology(pr.second.generator_state, err, new_context);
		}
	}
	// triggered modifier contents
	{
		err.file_name = "triggered_modifiers.txt";
		for(auto& r : context.set_of_triggered_modifiers) {
			national_definitions.triggered_modifiers[r.index].trigger_condition =
					parsers::read_triggered_modifier_condition(r.generator_state, err, context);
		}
	}
	// cb contents
	{
		err.file_name = "cb_types.txt";
		for(auto& r : context.map_of_cb_types) {
			parsers::individual_cb_context new_context{context, r.second.id};
			parsers::parse_cb_body(r.second.generator_state, err, new_context);
		}
	}
	// pending crimes
	{
		err.file_name = "crime.txt";
		for(auto& r : context.map_of_crimes) {
			parsers::read_pending_crime(r.second.id, r.second.generator_state, err, context);
		}
	}
	// pending issue options
	{
		err.file_name = "issues.txt";
		for(auto& r : context.map_of_ioptions) {
			parsers::read_pending_option(r.second.id, r.second.generator_state, err, context);
		}
	}
	// pending reform options
	{
		err.file_name = "issues.txt";
		for(auto& r : context.map_of_roptions) {
			parsers::read_pending_reform(r.second.id, r.second.generator_state, err, context);
		}
	}
	// parse national_focus.txt
	{
		auto nat_focus = open_file(common, NATIVE("national_focus.txt"));
		if(nat_focus) {
			auto content = view_contents(*nat_focus);
			err.file_name = "national_focus.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_focus_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/national_focus.txt could not be opened\n";
		}
	}
	// load pop_types.txt
	{
		auto pop_types_file = open_file(common, NATIVE("pop_types.txt"));
		if(pop_types_file) {
			auto content = view_contents(*pop_types_file);
			err.file_name = "pop_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_main_pop_type_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/pop_types.txt could not be opened\n";
		}
	}
	// read pending techs
	{
		err.file_name = "technology file";
		for(auto& r : context.map_of_technologies) {
			parsers::read_pending_technology(r.second.id, r.second.generator_state, err, context);
		}
	}
	// read pending inventions
	{
		err.file_name = "inventions file";
		for(auto& r : context.map_of_inventions) {
			parsers::read_pending_invention(r.second.id, r.second.generator_state, err, context);
		}
	}
	// parse on_actions.txt
	{
		auto on_action = open_file(common, NATIVE("on_actions.txt"));
		if(on_action) {
			auto content = view_contents(*on_action);
			err.file_name = "on_actions.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_on_action_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/on_actions.txt could not be opened\n";
		}
	}
	// parse production_types.txt
	{
		auto prod_types = open_file(common, NATIVE("production_types.txt"));
		if(prod_types) {
			auto content = view_contents(*prod_types);
			err.file_name = "production_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);

			parsers::production_context new_context{context};
			parsers::parse_production_types_file(gen, err, new_context);

			if(!new_context.found_worker_types) {
				err.fatal = true;
				err.accumulated_errors += "Unable to identify factory worker types from production_types.txt\n";
			}
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/production_types.txt could not be opened\n";
		}
	}
	// read pending rebel types
	{
		err.file_name = "rebel_types.txt";
		for(auto& r : context.map_of_rebeltypes) {
			parsers::read_pending_rebel_type(r.second.id, r.second.generator_state, err, context);
		}
	}
	// load decisions
	{
		auto decisions = open_directory(root, NATIVE("decisions"));
		for(auto decision_file : list_files(decisions, NATIVE(".txt"))) {
			auto opened_file = open_file(decision_file);
			if(opened_file) {
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_decision_file(gen, err, context);
			}
		}
	}
	// load events
	{
		auto events = open_directory(root, NATIVE("events"));
		std::vector<simple_fs::file> held_open_files;
		for(auto event_file : list_files(events, NATIVE(".txt"))) {
			auto opened_file = open_file(event_file);
			if(opened_file) {
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_event_file(gen, err, context);
				held_open_files.emplace_back(std::move(*opened_file));
			}
		}
		err.file_name = "pending events";
		parsers::commit_pending_events(err, context);
	}
	// load oob
	{
		auto oob_dir = open_directory(history, NATIVE("units"));
		for(auto oob_file : list_files(oob_dir, NATIVE(".txt"))) {
			auto file_name = get_full_name(oob_file);

			auto last = file_name.c_str() + file_name.length();
			auto first = file_name.c_str();
			auto start_of_name = last;
			for(; start_of_name >= first; --start_of_name) {
				if(*start_of_name == NATIVE('\\') || *start_of_name == NATIVE('/')) {
					++start_of_name;
					break;
				}
			}
			if(last - start_of_name >= 6 && file_name.ends_with(NATIVE("_oob.txt"))) {
				auto utf8name = simple_fs::native_to_utf8(native_string_view(start_of_name, last - start_of_name));

				if(auto it = context.map_of_ident_names.find(nations::tag_to_int(utf8name[0], utf8name[1], utf8name[2]));
						it != context.map_of_ident_names.end()) {
					auto holder = context.state.world.national_identity_get_nation_from_identity_holder(it->second);
					if(holder) {
						parsers::oob_file_context new_context{context, holder};

						auto opened_file = open_file(oob_file);
						if(opened_file) {
							err.file_name = utf8name;
							auto content = view_contents(*opened_file);
							parsers::token_generator gen(content.data, content.data + content.file_size);
							parsers::parse_oob_file(gen, err, new_context);
						}
					} else {
						// dead tag
					}
				} else {
					err.accumulated_errors += "invalid tag " + utf8name.substr(0, 3) + " encountered while scanning oob files\n";
				}
			}
		}
	}
	// parse diplomacy history
	{
		auto diplomacy = open_directory(history, NATIVE("diplomacy"));
		{
			auto dip_file = open_file(diplomacy, NATIVE("Alliances.txt"));
			if(dip_file) {
				auto content = view_contents(*dip_file);
				err.file_name = "Alliances.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_alliance_file(gen, err, context);
			} else {
				err.accumulated_errors += "File history/diplomacy/Alliances.txt could not be opened\n";
			}
		}
		{
			auto dip_file = open_file(diplomacy, NATIVE("PuppetStates.txt"));
			if(dip_file) {
				auto content = view_contents(*dip_file);
				err.file_name = "PuppetStates.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_puppets_file(gen, err, context);
			} else {
				err.accumulated_errors += "File history/diplomacy/PuppetStates.txt could not be opened\n";
			}
		}
		{
			auto dip_file = open_file(diplomacy, NATIVE("Unions.txt"));
			if(dip_file) {
				auto content = view_contents(*dip_file);
				err.file_name = "Unions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_union_file(gen, err, context);
			} else {
				err.accumulated_errors += "File history/diplomacy/Unions.txt could not be opened\n";
			}
		}
	}

	// !!!! yes, I know
	world.nation_resize_flag_variables(uint32_t(national_definitions.num_allocated_national_flags));

	// load country history
	{
		auto country_dir = open_directory(history, NATIVE("countries"));
		for(auto country_file : list_files(country_dir, NATIVE(".txt"))) {
			auto file_name = get_full_name(country_file);

			auto last = file_name.c_str() + file_name.length();
			auto first = file_name.c_str();
			auto start_of_name = last;
			for(; start_of_name >= first; --start_of_name) {
				if(*start_of_name == NATIVE('\\') || *start_of_name == NATIVE('/')) {
					++start_of_name;
					break;
				}
			}
			if(last - start_of_name >= 6) {
				auto utf8name = simple_fs::native_to_utf8(native_string_view(start_of_name, last - start_of_name));

				if(auto it = context.map_of_ident_names.find(nations::tag_to_int(utf8name[0], utf8name[1], utf8name[2]));
						it != context.map_of_ident_names.end()) {
					auto holder = context.state.world.national_identity_get_nation_from_identity_holder(it->second);

					parsers::country_history_context new_context{context, it->second, holder};

					auto opened_file = open_file(country_file);
					if(opened_file) {
						err.file_name = utf8name;
						auto content = view_contents(*opened_file);
						parsers::token_generator gen(content.data, content.data + content.file_size);
						parsers::parse_country_history_file(gen, err, new_context);
					}

				} else {
					err.accumulated_errors +=
							"invalid tag " + utf8name.substr(0, 3) + " encountered while scanning country history files\n";
				}
			}
		}
	}

	// load war history
	{
		auto country_dir = open_directory(history, NATIVE("wars"));
		for(auto war_file : list_files(country_dir, NATIVE(".txt"))) {
			auto opened_file = open_file(war_file);
			if(opened_file) {
				parsers::war_history_context new_context{context};

				err.file_name = simple_fs::native_to_utf8(simple_fs::get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_war_history_file(gen, err, new_context);
			}
		}
	}

	// misc touch ups
	nations::generate_initial_state_instances(*this);
	world.nation_resize_stockpiles(world.commodity_size());
	world.nation_resize_variables(uint32_t(national_definitions.num_allocated_national_variables));
	world.pop_resize_demographics(pop_demographics::size(*this));
	national_definitions.global_flag_variables.resize((national_definitions.num_allocated_global_flags + 7) / 8,
			dcon::bitfield_type{0});

	world.for_each_ideology([&](dcon::ideology_id id) {
		if(!bool(world.ideology_get_activation_date(id))) {
			world.ideology_set_enabled(id, true);
		}
	});

	for(auto n : world.in_nation) {
		n.set_diplomatic_points(1.0f);
	}

	// fix worker types
	province::for_each_land_province(*this, [&](dcon::province_id p) {
		bool is_mine = world.commodity_get_is_mine(world.province_get_rgo(p));

		// fix pop types
		for(auto pop : world.province_get_pop_location(p)) {
			if(is_mine && pop.get_pop().get_poptype() == culture_definitions.farmers) {
				pop.get_pop().set_poptype(culture_definitions.laborers);
			}
			if(!is_mine && pop.get_pop().get_poptype() == culture_definitions.laborers) {
				pop.get_pop().set_poptype(culture_definitions.farmers);
			}
		}
	});

	// add dummy nations for unheld tags
	world.for_each_national_identity([&](dcon::national_identity_id id) {
		if(!world.national_identity_get_nation_from_identity_holder(id)) {
			auto new_nation = world.create_nation();
			world.try_create_identity_holder(new_nation, id);
		}
	});

	map_loader.join();

	// touch up adjacencies
	world.for_each_province_adjacency([&](dcon::province_adjacency_id id) {
		auto frel = fatten(world, id);
		auto prov_a = frel.get_connected_provinces(0);
		auto prov_b = frel.get_connected_provinces(1);
		if(prov_a.id.index() < province_definitions.first_sea_province.index() &&
				prov_b.id.index() >= province_definitions.first_sea_province.index()) {
			frel.get_type() |= province::border::coastal_bit;
		} else if(prov_a.id.index() >= province_definitions.first_sea_province.index() &&
							prov_b.id.index() < province_definitions.first_sea_province.index()) {
			frel.get_type() |= province::border::coastal_bit;
		}
		if(prov_a.get_state_from_abstract_state_membership() != prov_b.get_state_from_abstract_state_membership()) {
			frel.get_type() |= province::border::state_bit;
		}
		if(prov_a.get_nation_from_province_ownership() != prov_b.get_nation_from_province_ownership()) {
			frel.get_type() |= province::border::national_bit;
		}
	});

	// fill in the terrain type

	for(int32_t i = 0; i < province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id id{dcon::province_id::value_base_t(i)};
		if(!world.province_get_terrain(id)) { // don't overwrite if set by the history file
			auto terrain_type = map_state.map_data.median_terrain_type[province::to_map_id(id)];
			if(terrain_type < 64) {
				auto modifier = context.modifier_by_terrain_index[terrain_type];
				world.province_set_terrain(id, modifier);
			}
		}
	}
	for(int32_t i = province_definitions.first_sea_province.index(); i < int32_t(world.province_size()); ++i) {
		dcon::province_id id{dcon::province_id ::value_base_t(i)};
		world.province_set_terrain(id, context.ocean_terrain);
	}

	// make ports
	province::for_each_land_province(*this, [&](dcon::province_id p) {
		for(auto adj : world.province_get_province_adjacency(p)) {
			auto other = adj.get_connected_provinces(0) != p ? adj.get_connected_provinces(0) : adj.get_connected_provinces(1);
			auto bits = adj.get_type();
			if((bits & province::border::coastal_bit) != 0 && (bits & province::border::impassible_bit) == 0) {
				world.province_set_port_to(p, other.id);
				return;
			}
		}
	});

	fill_unsaved_data(); // we need this to run triggers

	demographics::regenerate_from_pop_data(*this);
	economy::initialize(*this);

	culture::create_initial_ideology_and_issues_distribution(*this);
	demographics::regenerate_from_pop_data(*this);

	military::reinforce_regiments(*this);

	nations::update_administrative_efficiency(*this);
	military::regenerate_land_unit_average(*this);
	military::regenerate_ship_scores(*this);
	nations::update_industrial_scores(*this);
	military::update_naval_supply_points(*this);
	economy::update_rgo_employment(*this);
	economy::update_factory_employment(*this);
	nations::update_military_scores(*this); // depends on ship score, land unit average
	nations::update_rankings(*this);		// depends on industrial score, military scores

	assert(great_nations.size() == 0);
	for(uint32_t i = 0; i < nations_by_rank.size() && i < uint32_t(defines.great_nations_count); ++i) {
		if(nations_by_rank[i]) {
			great_nations.push_back(great_nation{ sys::date{0}, nations_by_rank[i] });
			world.nation_set_is_great_power(nations_by_rank[i], true);
		}
	}

	// run the economy for three days on scenario creation
	economy::update_rgo_employment(*this);
	economy::update_factory_employment(*this);
	economy::daily_update(*this);

	economy::update_rgo_employment(*this);
	economy::update_factory_employment(*this);
	economy::daily_update(*this);

	economy::update_rgo_employment(*this);
	economy::update_factory_employment(*this);
	economy::daily_update(*this);

	military::recover_org(*this);

	if(err.accumulated_errors.length() > 0)
		window::emit_error_message(err.accumulated_errors, err.fatal);
}

void state::fill_unsaved_data() { // reconstructs derived values that are not directly saved after a save has been loaded
	world.nation_resize_modifier_values(sys::national_mod_offsets::count);
	world.nation_resize_rgo_goods_output(world.commodity_size());
	world.nation_resize_factory_goods_output(world.commodity_size());
	world.nation_resize_factory_goods_throughput(world.commodity_size());
	world.nation_resize_rgo_size(world.commodity_size());
	world.nation_resize_rebel_org_modifier(world.rebel_type_size());
	world.nation_resize_active_unit(uint32_t(military_definitions.unit_base_definitions.size()));
	world.nation_resize_active_crime(uint32_t(culture_definitions.crimes.size()));
	world.nation_resize_active_building(world.factory_type_size());
	world.nation_resize_unit_stats(uint32_t(military_definitions.unit_base_definitions.size()));

	world.province_resize_modifier_values(provincial_mod_offsets::count);

	world.nation_resize_demographics(demographics::size(*this));
	world.state_instance_resize_demographics(demographics::size(*this));
	world.province_resize_demographics(demographics::size(*this));

	world.for_each_nation([&](dcon::nation_id id) { politics::update_displayed_identity(*this, id); });

	nations_by_rank.resize(2000); // TODO: take this value directly from the data container: max number of nations
	nations_by_industrial_score.resize(2000);
	nations_by_military_score.resize(2000);
	nations_by_prestige_score.resize(2000);
	crisis_participants.resize(2000);

	world.for_each_issue([&](dcon::issue_id id) {
		for(auto& opt : world.issue_get_options(id)) {
			if(opt) {
				world.issue_option_set_parent_issue(opt, id);
			}
		}
	});
	world.for_each_reform([&](dcon::reform_id id) {
		for(auto& opt : world.reform_get_options(id)) {
			if(opt) {
				world.reform_option_set_parent_reform(opt, id);
			}
		}
	});
	for(auto i : culture_definitions.party_issues) {
		world.issue_set_issue_type(i, uint8_t(culture::issue_type::party));
	}
	for(auto i : culture_definitions.military_issues) {
		world.reform_set_reform_type(i, uint8_t(culture::issue_type::military));
	}
	for(auto i : culture_definitions.economic_issues) {
		world.reform_set_reform_type(i, uint8_t(culture::issue_type::economic));
	}
	for(auto i : culture_definitions.social_issues) {
		world.issue_set_issue_type(i, uint8_t(culture::issue_type::social));
	}
	for(auto i : culture_definitions.political_issues) {
		world.issue_set_issue_type(i, uint8_t(culture::issue_type::political));
	}

	military::reset_unit_stats(*this);
	culture::repopulate_technology_effects(*this);
	culture::repopulate_invention_effects(*this);
	military::apply_base_unit_stat_modifiers(*this);

	province::update_connected_regions(*this);

	province::restore_unsaved_values(*this);

	culture::update_all_nations_issue_rules(*this);
	culture::restore_unsaved_values(*this);
	nations::restore_state_instances(*this);
	demographics::regenerate_from_pop_data(*this);

	sys::repopulate_modifier_effects(*this);
	military::restore_unsaved_values(*this);
	nations::restore_unsaved_values(*this);

	pop_demographics::regenerate_is_primary_or_accepted(*this);

	nations::update_administrative_efficiency(*this);
	rebel::update_movement_values(*this);

	economy::regenerate_unsaved_values(*this);

	military::regenerate_land_unit_average(*this);
	military::regenerate_ship_scores(*this);
	nations::update_industrial_scores(*this);
	nations::update_military_scores(*this);
	nations::update_rankings(*this);
	nations::update_ui_rankings(*this);

	nations::monthly_flashpoint_update(*this);

	if(local_player_nation) {
		world.nation_set_is_player_controlled(local_player_nation, true);
		// reshow pending events, messages, etc
		for(auto const& e : pending_n_event) {
			if(e.n == local_player_nation) {
				new_n_event.push(e);
			}
		}
		for(auto const& e : pending_f_n_event) {
			if(e.n == local_player_nation) {
				new_f_n_event.push(e);
			}
		}
		for(auto const& e : pending_p_event) {
			if(world.province_get_nation_from_province_ownership(e.p) == local_player_nation) {
				new_p_event.push(e);
			}
		}
		for(auto const& e : pending_f_p_event) {
			if(world.province_get_nation_from_province_ownership(e.p) == local_player_nation) {
				new_f_p_event.push(e);
			}
		}
		for(auto const& m : pending_messages) {
			if(m.to == local_player_nation) {
				new_requests.push(m);
			}
		}
	}
	ui_date = current_date;

	game_state_updated.store(true, std::memory_order::release);
}

constexpr inline int32_t game_speed[] = {
		0,		// speed 0
		2000, // speed 1 -- 2 seconds
		1000, // speed 2 -- 1 second
		500,	// speed 3 -- 0.5 seconds
		250,	// speed 4 -- 0.25 seconds
};

void state::game_loop() {
	while(quit_signaled.load(std::memory_order::acquire) == false) {
		auto speed = actual_game_speed.load(std::memory_order::acquire);
		auto upause = ui_pause.load(std::memory_order::acquire);
		if(speed <= 0 || upause || internally_paused) {
			command::execute_pending_commands(*this);
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
		} else {
			auto entry_time = std::chrono::steady_clock::now();
			auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(entry_time - last_update).count();

			command::execute_pending_commands(*this);
			if(speed >= 5 || ms_count >= game_speed[speed]) { /*enough time has passed*/
				last_update = entry_time;

				// do update logic
				province::update_connected_regions(*this);
				province::update_cached_values(*this);
				nations::update_cached_values(*this);

				current_date += 1;

				auto ymd_date = current_date.to_ymd(start_date);

				diplomatic_message::update_pending(*this);

				auto month_start = sys::year_month_day{ymd_date.year, ymd_date.month, uint16_t(1)};
				auto next_month_start = sys::year_month_day{ymd_date.year, uint16_t(ymd_date.month + 1), uint16_t(1)};
				auto const days_in_month = uint32_t(sys::days_difference(month_start, next_month_start));

				// pop update:
				static demographics::ideology_buffer idbuf(*this);
				static demographics::issues_buffer isbuf(*this);
				static demographics::promotion_buffer pbuf;
				static demographics::assimilation_buffer abuf;
				static demographics::migration_buffer mbuf;
				static demographics::migration_buffer cmbuf;
				static demographics::migration_buffer imbuf;

				// calculate complex changes in parallel where we can, but don't actually apply the results
				// instead, the changes are saved to be applied only after all triggers have been evaluated
				concurrency::parallel_for(0, 7, [&](int32_t index) {
					switch(index) {
					case 0: {
						auto o = uint32_t(ymd_date.day);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_ideologies(*this, o, days_in_month, idbuf);
						break;
					}
					case 1: {
						auto o = uint32_t(ymd_date.day + 1);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_issues(*this, o, days_in_month, isbuf);
						break;
					}
					case 2: {
						auto o = uint32_t(ymd_date.day + 6);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_type_changes(*this, o, days_in_month, pbuf);
						break;
					}
					case 3: {
						auto o = uint32_t(ymd_date.day + 7);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_assimilation(*this, o, days_in_month, abuf);
						break;
					}
					case 4: {
						auto o = uint32_t(ymd_date.day + 8);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_internal_migration(*this, o, days_in_month, mbuf);
						break;
					}
					case 5: {
						auto o = uint32_t(ymd_date.day + 9);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_colonial_migration(*this, o, days_in_month, cmbuf);
						break;
					}
					case 6: {
						auto o = uint32_t(ymd_date.day + 10);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_immigration(*this, o, days_in_month, imbuf);
						break;
					}
					}
				});

				// apply in parallel where we can
				concurrency::parallel_for(0, 8, [&](int32_t index) {
					switch(index) {
					case 0: {
						auto o = uint32_t(ymd_date.day + 0);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::apply_ideologies(*this, o, days_in_month, idbuf);
						break;
					}
					case 1: {
						auto o = uint32_t(ymd_date.day + 1);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::apply_issues(*this, o, days_in_month, isbuf);
						break;
					}
					case 2: {
						auto o = uint32_t(ymd_date.day + 2);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_militancy(*this, o, days_in_month);
						break;
					}
					case 3: {
						auto o = uint32_t(ymd_date.day + 3);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_consciousness(*this, o, days_in_month);
						break;
					}
					case 4: {
						auto o = uint32_t(ymd_date.day + 4);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_literacy(*this, o, days_in_month);
						break;
					}
					case 5: {
						auto o = uint32_t(ymd_date.day + 5);
						if(o >= days_in_month)
							o -= days_in_month;
						demographics::update_growth(*this, o, days_in_month);
						break;
					}
					case 6:
						province::ve_for_each_land_province(*this,
								[&](auto ids) { world.province_set_daily_net_migration(ids, ve::fp_vector{}); });
						break;
					case 7:
						province::ve_for_each_land_province(*this,
								[&](auto ids) { world.province_set_daily_net_immigration(ids, ve::fp_vector{}); });
						break;
					}
				});

				// because they may add pops, these changes must be applied sequentially
				{
					auto o = uint32_t(ymd_date.day + 6);
					if(o >= days_in_month)
						o -= days_in_month;
					demographics::apply_type_changes(*this, o, days_in_month, pbuf);
				}
				{
					auto o = uint32_t(ymd_date.day + 7);
					if(o >= days_in_month)
						o -= days_in_month;
					demographics::apply_assimilation(*this, o, days_in_month, abuf);
				}
				{
					auto o = uint32_t(ymd_date.day + 8);
					if(o >= days_in_month)
						o -= days_in_month;
					demographics::apply_internal_migration(*this, o, days_in_month, mbuf);
				}
				{
					auto o = uint32_t(ymd_date.day + 9);
					if(o >= days_in_month)
						o -= days_in_month;
					demographics::apply_colonial_migration(*this, o, days_in_month, cmbuf);
				}
				{
					auto o = uint32_t(ymd_date.day + 10);
					if(o >= days_in_month)
						o -= days_in_month;
					demographics::apply_immigration(*this, o, days_in_month, imbuf);
				}

				demographics::remove_size_zero_pops(*this);

				// basic repopulation of demographics derived values
				demographics::regenerate_from_pop_data(*this);

				// values updates pass 1 (mostly trivial things, can be done in parallel
				concurrency::parallel_for(0, 15, [&](int32_t index) {
					switch(index) {
					case 0:
						nations::update_administrative_efficiency(*this);
						break;
					case 1:
						nations::update_research_points(*this);
						break;
					case 2:
						military::regenerate_land_unit_average(*this);
						break;
					case 3:
						military::regenerate_ship_scores(*this);
						break;
					case 4:
						nations::update_industrial_scores(*this);
						break;
					case 5:
						military::update_naval_supply_points(*this);
						break;
					case 6:
						economy::update_rgo_employment(*this);
						break;
					case 7:
						economy::update_factory_employment(*this);
						break;
					case 8:
						rebel::daily_update_rebel_organization(*this);
						break;
					case 9:
						military::daily_leaders_update(*this);
						break;
					case 10:
						politics::daily_party_loyalty_update(*this);
						break;
					case 11:
						nations::daily_update_flashpoint_tension(*this);
						break;
					case 12:
						military::update_ticking_war_score(*this);
						break;
					case 13:
						military::increase_dig_in(*this);
						break;
					case 14:
						military::recover_org(*this);
						break;
					}
				});

				economy::daily_update(*this);

				military::update_movement(*this);
				military::update_siege_progress(*this);
				military::update_naval_battles(*this);
				military::update_land_battles(*this);
				military::advance_mobilizations(*this);

				event::update_events(*this);

				culture::update_research(*this, uint32_t(ymd_date.year));

				nations::update_military_scores(*this); // depends on ship score, land unit average
				nations::update_rankings(*this);				// depends on industrial score, military scores
				nations::update_great_powers(*this);		// depends on rankings
				nations::update_influence(*this);				// depends on rankings, great powers

				province::update_colonization(*this);
				military::update_cbs(*this); // may add/remove cbs to a nation

				nations::update_crisis(*this);
				politics::update_elections(*this);

				// Once per month updates, spread out over the month
				switch(ymd_date.day) {
				case 1:
					nations::update_monthly_points(*this);
					break;
				case 2:
					sys::update_modifier_effects(*this);
					break;
				case 3:
					military::monthly_leaders_update(*this);
					break;
				case 4:
					military::reinforce_regiments(*this);
					break;
				case 5:
					rebel::update_movements(*this);
					rebel::update_factions(*this);
					break;
				case 8:
					military::apply_attrition(*this);
					break;
				case 9:
					military::repair_ships(*this);
					break;
				case 10:
					province::update_crimes(*this);
					break;
				case 11:
					province::update_nationalism(*this);
					break;
				case 15:
					culture::discover_inventions(*this);
					break;
				case 20:
					nations::monthly_flashpoint_update(*this);
					break;
				case 24:
					rebel::execute_rebel_victories(*this);
					break;
				case 25:
					rebel::execute_province_defections(*this);
					break;
				case 28:
					rebel::rebel_risings_check(*this);
					break;
				default:
					break;
				}

				military::apply_regiment_damage(*this);

				// yearly update : redo the upper house
				if(ymd_date.day == 1 && ymd_date.month == 1) {
					for(auto n : world.in_nation) {
						politics::recalculate_upper_house(*this, n);
					}
				}

				/*
				 * END OF DAY: update cached data
				 */

				player_data_cache.treasury_record[current_date.value % 32] = nations::get_treasury(*this, local_player_nation);
				if((current_date.value % 16) == 0) {
					auto index = economy::most_recent_price_record_index(*this);
					for(auto c : world.in_commodity) {
						c.set_price_record(index, c.get_current_price());
					}
				}

				ui_date = current_date;

				game_state_updated.store(true, std::memory_order::release);
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}
}

void state::console_log(ui::element_base* base, std::string message, bool open_console) {
	if(ui_state.console_window != nullptr) {

		Cyto::Any payload = std::string(to_string_view(base->base_data.name)) + ": " + std::string(message);
		ui_state.console_window->impl_get(*this, payload);

		if(open_console && !(ui_state.console_window->is_visible())) {
			ui_state.root->move_child_to_front(ui_state.console_window);
			ui_state.console_window->set_visible(*this, true);
		}
	}
}

} // namespace sys
