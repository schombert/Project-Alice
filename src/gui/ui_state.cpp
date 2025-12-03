#include "ui_state.hpp"
#include "gui_element_types.hpp"
#include "system_state.hpp"
#include "map_tooltip.hpp"

namespace ui {

inline constexpr int32_t tooltip_width = 400;

void state::update_timing() {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	if(last_render_time == std::chrono::time_point<std::chrono::steady_clock>{}) {
		last_render_time = now;
	}
	time_since_last_render = std::chrono::duration_cast<std::chrono::microseconds>(now - last_render_time);
	auto frames_per_second = 1.f / float(time_since_last_render.count() / 1e6);
	last_render_time = now;
	if(fps_timer > 20) {
		last_fps = frames_per_second;
		fps_timer = 0;
	}
	fps_timer += 1;
}

void state::update_scroll(sys::state& game_state) {
	if(scrollbar_timer > 500 * (last_fps / 60)) {
		scrollbar_continuous_movement = true;
		if(left_mouse_hold_target != nullptr) {
			Cyto::Any payload = ui::scrollbar_settings{};
			left_mouse_hold_target->impl_set(game_state, payload);
		}
	}
	if(left_mouse_hold_target != nullptr) {
		scrollbar_timer += 1;
	}
}

void state::set_focus_target(sys::state& state, element_base* target) {
	if(edit_target_internal && !target) {
		state.win_ptr->text_services.suspend_keystroke_handling();
		state.win_ptr->text_services.set_focus(state, nullptr);
	}
	if(!edit_target_internal && target)
		state.win_ptr->text_services.resume_keystroke_handling();
	if(edit_target_internal && edit_target_internal != target) {
		edit_target_internal->on_lose_focus(state);
	}
	if(edit_target_internal != target) {
		if(target && target->on_get_focus(state) != ui::focus_result::ignored)
			edit_target_internal = target;
		else
			edit_target_internal = nullptr;
	}
}
void state::set_mouse_sensitive_target(sys::state& state, element_base* target) {
	if(mouse_sensitive_target) {
		mouse_sensitive_target->set_visible(state, false);
	}
	mouse_sensitive_target = target;
	if(target) {
		auto size = target->base_data.size;
		target_ul_bounds = ui::get_absolute_location(state, *target);
		target_lr_bounds = ui::xy_pair{ int16_t(target_ul_bounds.x + size.x), int16_t(target_ul_bounds.y + size.y) };

		auto mx = int32_t(state.mouse_x_position / state.user_settings.ui_scale);
		auto my = int32_t(state.mouse_y_position / state.user_settings.ui_scale);

		auto x_distance = std::max(std::max(target_ul_bounds.x - mx, 0), std::max(mx - target_lr_bounds.x, 0));
		auto y_distance = std::max(std::max(target_ul_bounds.y - my, 0), std::max(my - target_lr_bounds.y, 0));
		target_distance = std::max(x_distance, y_distance);

		target->set_visible(state, true);
	}
}

void state::update_tooltip(sys::state& state, ui::mouse_probe tooltip_probe, int32_t tooltip_sub_index, int16_t max_height) {
	if(last_tooltip != tooltip_probe.under_mouse) return;
	if(last_tooltip_sub_index != tooltip_sub_index) return;
	if(!last_tooltip) return;
	if(!tooltip->is_visible()) return;

	auto type = last_tooltip->has_tooltip(state);
	if(type != ui::tooltip_behavior::position_sensitive_tooltip) {
		auto container = text::create_columnar_layout(
			state,
			tooltip->internal_layout,
			text::layout_parameters{
				0, 0,
				tooltip_width,
				max_height,
				tooltip_font,
				0,
				text::alignment::left,
				text::text_color::white,
				true
			},
			10
		);
		last_tooltip->update_tooltip(state, tooltip_probe.relative_location.x, tooltip_probe.relative_location.y,
				container);
		populate_shortcut_tooltip(state, *last_tooltip, container);
		if(container.native_rtl == text::layout_base::rtl_status::rtl) {
			container.used_width = -container.used_width;
			for(auto& t : container.base_layout.contents) {
				t.x += 16 + container.used_width;
				t.y += 16;
			}
		} else {
			for(auto& t : container.base_layout.contents) {
				t.x += 16;
				t.y += 16;
			}
		}
		tooltip->base_data.size.x = int16_t(container.used_width + 32);
		tooltip->base_data.size.y = int16_t(container.used_height + 32);
		if(container.used_width > 0)
			tooltip->set_visible(state, true);
		else
			tooltip->set_visible(state, false);
	}
}



void state::populate_tooltip(sys::state& state, ui::mouse_probe tooltip_probe, int32_t tooltip_sub_index, int16_t max_height) {
	if(last_tooltip != tooltip_probe.under_mouse || last_tooltip_sub_index != tooltip_sub_index) {
		last_tooltip = tooltip_probe.under_mouse;
		last_tooltip_sub_index = tooltip_sub_index;

		if(tooltip_probe.under_mouse) {
			auto type = last_tooltip->has_tooltip(state);
			if(type != ui::tooltip_behavior::no_tooltip) {
				auto container = text::create_columnar_layout(state, tooltip->internal_layout,
					text::layout_parameters{
						0, 0, tooltip_width, max_height,
						tooltip_font, 0,
						text::alignment::left,
						text::text_color::white,
						true
					},
					10
				);
				last_tooltip->update_tooltip(
					state,
					tooltip_probe.relative_location.x,
					tooltip_probe.relative_location.y,
					container
				);
				populate_shortcut_tooltip(state, *last_tooltip, container);
				if(container.native_rtl == text::layout_base::rtl_status::rtl) {
					container.used_width = -container.used_width;
					for(auto& t : container.base_layout.contents) {
						t.x += 16 + container.used_width;
						t.y += 16;
					}
				} else {
					for(auto& t : container.base_layout.contents) {
						t.x += 16;
						t.y += 16;
					}
				}
				tooltip->base_data.size.x = int16_t(container.used_width + 32);
				tooltip->base_data.size.y = int16_t(container.used_height + 32);
				if(container.used_width > 0)
					tooltip->set_visible(state, true);
				else
					tooltip->set_visible(state, false);
			} else {
				tooltip->set_visible(state, false);
			}
		} else {
			tooltip->set_visible(state, false);
		}
	} else if(last_tooltip && last_tooltip->has_tooltip(state) == ui::tooltip_behavior::position_sensitive_tooltip) {
		auto container = text::create_columnar_layout(
			state,
			tooltip->internal_layout,
			text::layout_parameters{
				0, 0, tooltip_width, max_height,
				tooltip_font, 0,
				text::alignment::left,
				text::text_color::white,
				true
			},
			10
		);
		last_tooltip->update_tooltip(state, tooltip_probe.relative_location.x, tooltip_probe.relative_location.y, container);
		populate_shortcut_tooltip(state, *last_tooltip, container);
		if(container.native_rtl == text::layout_base::rtl_status::rtl) {
			container.used_width = -container.used_width;
			for(auto& t : container.base_layout.contents) {
				t.x += 16 + container.used_width;
				t.y += 16;
			}
		} else {
			for(auto& t : container.base_layout.contents) {
				t.x += 16;
				t.y += 16;
			}
		}
		tooltip->base_data.size.x = int16_t(container.used_width + 32);
		tooltip->base_data.size.y = int16_t(container.used_height + 32);
		if(container.used_width > 0)
			tooltip->set_visible(state, true);
		else
			tooltip->set_visible(state, false);
	}
}

void state::reposition_tooltip(ui::urect tooltip_bounds, int16_t root_height, int16_t root_width) {
	if(!last_tooltip) return;
	if(!tooltip->is_visible()) return;

	// reposition tooltip
	if(tooltip->base_data.size.y <= root_height - (tooltip_bounds.top_left.y + tooltip_bounds.size.y)) {
		tooltip->base_data.position.y = int16_t(tooltip_bounds.top_left.y + tooltip_bounds.size.y);
		tooltip->base_data.position.x = std::clamp(
			int16_t(tooltip_bounds.top_left.x + (tooltip_bounds.size.x / 2) - (tooltip->base_data.size.x / 2)),
			int16_t(0),
			int16_t(std::max(root_width - tooltip->base_data.size.x, 0))
		);
	} else if(tooltip->base_data.size.x <= root_width - (tooltip_bounds.top_left.x + tooltip_bounds.size.x)) {
		tooltip->base_data.position.x = int16_t(tooltip_bounds.top_left.x + tooltip_bounds.size.x);
		tooltip->base_data.position.y = std::clamp(
			int16_t(tooltip_bounds.top_left.y + (tooltip_bounds.size.y / 2) - (tooltip->base_data.size.y / 2)),
			int16_t(0),
			int16_t(std::max(root_height - tooltip->base_data.size.y, 0))
		);
	} else if(tooltip->base_data.size.x <= tooltip_bounds.top_left.x) {
		tooltip->base_data.position.x = int16_t(tooltip_bounds.top_left.x - tooltip->base_data.size.x);
		tooltip->base_data.position.y = std::clamp(
			int16_t(tooltip_bounds.top_left.y + (tooltip_bounds.size.y / 2) - (tooltip->base_data.size.y / 2)),
			int16_t(0),
			int16_t(std::max(root_height - tooltip->base_data.size.y, 0))
		);
	} else if(tooltip->base_data.size.y <= tooltip_bounds.top_left.y) {
		tooltip->base_data.position.y = int16_t(tooltip_bounds.top_left.y - tooltip->base_data.size.y);
		tooltip->base_data.position.x = std::clamp(
			int16_t(tooltip_bounds.top_left.x + (tooltip_bounds.size.x / 2) - (tooltip->base_data.size.x / 2)),
			int16_t(0),
			int16_t(std::max(root_width - tooltip->base_data.size.x, 0))
		);
	} else {
		tooltip->base_data.position.x = std::clamp(
			int16_t(tooltip_bounds.top_left.x + (tooltip_bounds.size.x / 2) - (tooltip->base_data.size.x / 2)),
			int16_t(0),
			int16_t(std::max(root_width - tooltip->base_data.size.x, 0))
		);
		tooltip->base_data.position.y = std::clamp(
			int16_t(tooltip_bounds.top_left.y + (tooltip_bounds.size.y / 2) - (tooltip->base_data.size.y / 2)),
			int16_t(0),
			int16_t(std::max(root_height - tooltip->base_data.size.y, 0))
		);
	}
}

void state::handle_map_tooltip(sys::state& state, int16_t max_height) {
	dcon::province_id prov = state.map_state.get_province_under_mouse(state, int32_t(state.mouse_x_position), int32_t(state.mouse_y_position), state.x_size, state.y_size);

	bool tooltip_failed = false;;

	if(state.map_state.active_map_mode == map_mode::mode::terrain)
		tooltip_failed = true;
	if(state.map_state.active_map_mode == map_mode::mode::political)
		if(!state.current_scene.overwrite_map_tooltip)
			tooltip_failed = true;

	if(!prov) {
		tooltip_failed = true;
	}

	if(tooltip_failed) {
		tooltip->set_visible(state, false);
	}

	auto container = text::create_columnar_layout(
		state,
		tooltip->internal_layout,
		text::layout_parameters{
			0, 0,
			tooltip_width, max_height,
			tooltip_font, 0,
			text::alignment::left,
			text::text_color::white,
			true
		},
		20
	);
	ui::populate_map_tooltip(state, container, prov);
	if(container.native_rtl == text::layout_base::rtl_status::rtl) {
		container.used_width = -container.used_width;
		for(auto& t : container.base_layout.contents) {
			t.x += 16 + container.used_width;
			t.y += 16;
		}
	} else {
		for(auto& t : container.base_layout.contents) {
			t.x += 16;
			t.y += 16;
		}
	}
	tooltip->base_data.size.x = int16_t(container.used_width + 32);
	tooltip->base_data.size.y = int16_t(container.used_height + 32);
	if(container.used_width > 0) {
		// This block positions the tooltip somewhat under the province centroid
		auto mid_point = state.world.province_get_mid_point(prov);
		auto map_pos = state.map_state.normalize_map_coord(mid_point);
		auto screen_size =
			glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
		glm::vec2 screen_pos;
		if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos, { 200.f, 200.f })) {
			tooltip->set_visible(state, false);
		} else {
			tooltip->base_data.position =
				ui::xy_pair{ int16_t(screen_pos.x - container.used_width / 2 - 8), int16_t(screen_pos.y + 3.5f * state.map_state.get_zoom()) };
			tooltip->set_visible(state, true);
		}
		// Alternatively: just make it visible
		// tooltip->set_visible(state, true);
	} else {
		tooltip->set_visible(state, false);
	}
}

void state::render_tooltip(sys::state& state, bool follow_mouse, int32_t mouse_x, int32_t mouse_y, int32_t window_size_x, int32_t window_size_y, float ui_scale) {
	if(!tooltip->is_visible()) {
		return;
	}	

	if( follow_mouse) {
		//floating by mouse

		int32_t aim_x = int32_t(mouse_x / ui_scale) + cursor_size;
		int32_t aim_y = int32_t(mouse_y / ui_scale) + cursor_size;
		int32_t wsize_x = int32_t(window_size_x / ui_scale);
		int32_t wsize_y = int32_t(window_size_y / ui_scale);
		//this only works if the tooltip isnt bigger than the entire window, wont crash though
		if(aim_x + tooltip->base_data.size.x > wsize_x) {
			aim_x = wsize_x - tooltip->base_data.size.x;
		}
		if(aim_y + tooltip->base_data.size.y > wsize_y) {
			aim_y = wsize_y - tooltip->base_data.size.y;
		}
		tooltip->impl_render(state, aim_x, aim_y);
	} else {
		//tooltip centered over ui element

		tooltip->impl_render(state, tooltip->base_data.position.x, tooltip->base_data.position.y);
	}
}

}
