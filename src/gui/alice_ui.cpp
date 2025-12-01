#include <numbers>
#include <cmath>

#include "system_state.hpp"
#include "money.hpp"
#include "gui_modifier_tooltips.hpp"
#include "economy_trade_routes.hpp"
#include "color_templates.hpp"
#include "economy_templates.hpp"
#include "demographics_templates.hpp"
#include "economy_tooltips.hpp"
#include "gui_common_elements.hpp"
#include "gui_templates.hpp"
#include "gui_element_base.hpp"
#include "gui_element_types.hpp"
#include "gui_trade_window.hpp"
#include "gui_deserialize.hpp"
#include "alice_ui.hpp"
#include "economy_commands_data.hpp"
#include "lua_alice_api.hpp"
#include "demographics.hpp"
#include "economy_pops.hpp"
#include "advanced_province_buildings.hpp"
#include "gamerules.cpp"
#include "macrobuilder2.cpp"
#include "budgetwindow.cpp"
#include "demographicswindow.cpp"
#include "province_economy_overview.cpp"
#include "pop_details.cpp"
#include "pop_budget_details.cpp"
#include "market_trade_report.cpp"
#include "rgo_report.cpp"
#include "market_prices_report.cpp"
#include "trade_dashboard.cpp"
#include "main_menu.cpp"

namespace alice_ui {

bool state_is_rtl(sys::state& state) {
	return state.world.locale_get_native_rtl(state.font_collection.get_current_locale());
}

ogl::animation::type to_ogl_type(animation_type type, bool forwards) {
	switch(type) {
	case animation_type::page_left:
		return forwards ? ogl::animation::type::page_flip_left : ogl::animation::type::page_flip_left_rev;
	case animation_type::page_right:
		return forwards ? ogl::animation::type::page_flip_right : ogl::animation::type::page_flip_right_rev;
	case animation_type::page_up:
		return forwards ? ogl::animation::type::page_flip_up : ogl::animation::type::page_flip_up_rev;
	case animation_type::none:
		return ogl::animation::type::page_flip_left;
	case animation_type::page_middle:
		return forwards ? ogl::animation::type::page_flip_mid : ogl::animation::type::page_flip_mid_rev;
	default:
		return ogl::animation::type::page_flip_left;
	}
}

void layout_level::change_page(sys::state& state, layout_window_element& container, int32_t new_page) {
	if(page_animation != animation_type::none) {
		bool lflip = new_page < current_page && current_page > 0;
		bool rflip = new_page > current_page && current_page < int32_t(page_starts.size() - 1);
		if(rflip) {
			auto pos = ui::get_absolute_location(state, container);
			state.ui_animation.start_animation(state, pos.x + resolved_x_pos, pos.y + resolved_y_pos, resolved_x_size, resolved_y_size, to_ogl_type(page_animation, true), 220);
		} else if(lflip) {
			auto pos = ui::get_absolute_location(state, container);
			state.ui_animation.start_animation(state, pos.x + resolved_x_pos, pos.y + resolved_y_pos, resolved_x_size, resolved_y_size, to_ogl_type(page_animation, false), 220);
		}

		current_page = int16_t(std::clamp(new_page, 0, int32_t(page_starts.size() - 1)));

		if(rflip || lflip) {
			container.remake_layout(state, false);
			state.ui_animation.post_update_frame(state);
		}
	} else if(new_page != current_page) {
		current_page = int16_t(std::clamp(new_page, 0, int32_t(page_starts.size() - 1)));
		container.remake_layout(state, false);
	}
}

bool pop_passes_filter(sys::state& state, dcon::pop_id p) {
	if(popwindow::excluded_cultures.contains(state.world.pop_get_culture(p).id.index()))
		return false;
	if(popwindow::excluded_types.contains(state.world.pop_get_poptype(p).id.index()))
		return false;
	if(popwindow::excluded_religions.contains(state.world.pop_get_religion(p).id.index()))
		return false;
	if(popwindow::excluded_states.contains(state.world.province_get_state_membership(state.world.pop_get_province_from_pop_location(p)).id.index()))
		return false;
	if(!popwindow::show_colonial && state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)))
		return false;
	if(!popwindow::show_non_colonial && !state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)))
		return false;
	return true;
}


void template_icon_graphic::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, default_tooltip);
}
void template_icon_graphic::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(template_id == -1)
		return;

	ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
		state.ui_templates.icons[template_id].renders.get_render(state, int32_t(base_data.size.x), int32_t(base_data.size.y), state.user_settings.ui_scale, color.r, color.g, color.b));
}


void template_bg_graphic::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, default_tooltip);
}
void template_bg_graphic::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(template_id == -1)
		return;

	grid_size_window* par = static_cast<grid_size_window*>(parent);
	ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
		state.ui_templates.backgrounds[template_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}

void template_icon_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, default_tooltip);
}
void template_icon_button::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(template_id == -1)
		return;

	grid_size_window* par = static_cast<grid_size_window*>(parent);
	auto ms_after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_activated);
	if(disabled) {
		auto bg_id = state.ui_templates.iconic_button_t[template_id].disabled.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
		if(icon != -1) {
			auto ico_color = state.ui_templates.iconic_button_t[template_id].disabled.icon_color;
			auto l = state.ui_templates.iconic_button_t[template_id].disabled.icon_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto t = state.ui_templates.iconic_button_t[template_id].disabled.icon_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			auto r = state.ui_templates.iconic_button_t[template_id].disabled.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto b = state.ui_templates.iconic_button_t[template_id].disabled.icon_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
				state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
		}
	} else if(ms_after.count() < mouse_over_animation_ms && state.ui_templates.iconic_button_t[template_id].animate_active_transition) {
		float percentage = float(ms_after.count()) / float(mouse_over_animation_ms);
		if(this == state.ui_state.under_mouse) {
			auto active_id = state.ui_templates.iconic_button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
			auto bg_id = state.ui_templates.iconic_button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			if(icon != -1) {
				auto ico_color = state.ui_templates.iconic_button_t[template_id].active.icon_color;
				auto l = state.ui_templates.iconic_button_t[template_id].active.icon_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
				auto t = state.ui_templates.iconic_button_t[template_id].active.icon_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
				auto r = state.ui_templates.iconic_button_t[template_id].active.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
				auto b = state.ui_templates.iconic_button_t[template_id].active.icon_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
				ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
					state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
			}
		} else {
			auto active_id = state.ui_templates.iconic_button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			auto bg_id = state.ui_templates.iconic_button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}

			if(icon != -1) {
				auto ico_color = state.ui_templates.iconic_button_t[template_id].primary.icon_color;
				auto l = state.ui_templates.iconic_button_t[template_id].primary.icon_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
				auto t = state.ui_templates.iconic_button_t[template_id].primary.icon_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
				auto r = state.ui_templates.iconic_button_t[template_id].primary.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
				auto b = state.ui_templates.iconic_button_t[template_id].primary.icon_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
				ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
					state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
			}
		}
	} else if(this == state.ui_state.under_mouse) {
		auto active_id = state.ui_templates.iconic_button_t[template_id].active.bg;
		if(active_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
		if(icon != -1) {
			auto ico_color = state.ui_templates.iconic_button_t[template_id].active.icon_color;
			auto l = state.ui_templates.iconic_button_t[template_id].active.icon_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto t = state.ui_templates.iconic_button_t[template_id].active.icon_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			auto r = state.ui_templates.iconic_button_t[template_id].active.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto b = state.ui_templates.iconic_button_t[template_id].active.icon_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
				state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
		}
	} else {
		auto bg_id = state.ui_templates.iconic_button_t[template_id].primary.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
		if(icon != -1) {
			auto ico_color = state.ui_templates.iconic_button_t[template_id].primary.icon_color;
			auto l = state.ui_templates.iconic_button_t[template_id].primary.icon_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto t = state.ui_templates.iconic_button_t[template_id].primary.icon_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			auto r = state.ui_templates.iconic_button_t[template_id].primary.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto b = state.ui_templates.iconic_button_t[template_id].primary.icon_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
				state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
		}
	}
}

void template_icon_button::on_hover(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover(state);
}
void template_icon_button::on_hover_end(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover_end(state);
}


bool auto_close_button::button_action(sys::state& state) noexcept {
	state.ui_state.set_focus_target(state, nullptr);
	parent->set_visible(state, false);
	return true;
}
ui::message_result auto_close_button::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(key == sys::virtual_key::ESCAPE) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		state.ui_state.set_focus_target(state, nullptr);
		parent->set_visible(state, false);
		return ui::message_result::consumed;
	}
	return ui::message_result::unseen;
}

static text::alignment convert_align(template_project::aui_text_alignment a) {
	switch(a) {
	case template_project::aui_text_alignment::center: return text::alignment::center;
	case template_project::aui_text_alignment::left: return text::alignment::left;
	case template_project::aui_text_alignment::right: return text::alignment::right;
	}
	return text::alignment::left;
}

void template_label::set_text(sys::state& state, std::string_view new_text) {
	if(new_text != cached_text) {
		template_project::text_region_template region;
		grid_size_window* par = static_cast<grid_size_window*>(parent);

		if(template_id != -1) {
			region = state.ui_templates.label_t[template_id].primary;
		}


		cached_text = new_text;
		{
			internal_layout.contents.clear();
			internal_layout.number_of_lines = 0;

			text::single_line_layout sl{ internal_layout,
				text::layout_parameters{
					0, 0, static_cast<int16_t>(base_data.size.x - region.h_text_margins * par->grid_size * 2), static_cast<int16_t>(base_data.size.y - region.v_text_margins * 2),
					text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2), 0,
					convert_align(region.h_text_alignment), text::text_color::black, true, true
				},
				state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
			sl.add_text(state, cached_text);
		}
	}
}

void template_label::on_reset_text(sys::state& state) noexcept {
	cached_text.clear();
	if(default_text)
		set_text(state, text::produce_simple_string(state, default_text));
}
void template_label::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, default_tooltip);
}
void template_label::on_create(sys::state& state) noexcept {
	on_reset_text(state);
}

void template_label::render(sys::state& state, int32_t x, int32_t y) noexcept {
	template_project::text_region_template region;
	grid_size_window* par = static_cast<grid_size_window*>(parent);

	if(template_id != -1) {
		region = state.ui_templates.label_t[template_id].primary;
	}
	auto color = state.ui_templates.colors[region.text_color];

	auto bg_id = region.bg;
	if(bg_id != -1) {
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
			state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
	}

	auto fh = text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2);
	auto linesz = state.font_collection.line_height(state, fh);
	if(linesz == 0.f)
		return;

	int32_t yoff = 0;
	switch(region.v_text_alignment) {
	case template_project::aui_text_alignment::center: yoff = int32_t((base_data.size.y - linesz) / 2); break;
	case template_project::aui_text_alignment::left: yoff = int32_t(region.v_text_margins * par->grid_size); break;
	case template_project::aui_text_alignment::right: yoff = int32_t(base_data.size.y - linesz - region.v_text_margins * par->grid_size); break;
	}

	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(
			state,
			t,
			float(x) + t.x + region.h_text_margins * par->grid_size,
			float(y + t.y + yoff),
			fh,
			ogl::color3f{ color.r, color.g, color.b },
			ogl::color_modification::none
		);
	}
}



void template_mixed_button::set_text(sys::state& state, std::string_view new_text) {
	if(new_text != cached_text) {
		template_project::mixed_region_template region;
		grid_size_window* par = static_cast<grid_size_window*>(parent);

		if(template_id != -1) {
			region = state.ui_templates.mixed_button_t[template_id].primary;
		}

		cached_text = new_text;
		{
			internal_layout.contents.clear();
			internal_layout.number_of_lines = 0;
			auto icon_space = region.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));

			text::single_line_layout sl{ internal_layout,
				text::layout_parameters{
					0, 0, static_cast<int16_t>(base_data.size.x - icon_space - region.h_text_margins * par->grid_size * 2), static_cast<int16_t>(base_data.size.y - region.v_text_margins * 2),
					text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2), 0,
					convert_align(region.h_text_alignment), text::text_color::black, true, true
				},
				state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
			sl.add_text(state, cached_text);
		}
	}
}
void template_mixed_button::on_hover(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover(state);
}
void template_mixed_button::on_hover_end(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover_end(state);
}
void template_mixed_button::on_reset_text(sys::state& state) noexcept {
	cached_text.clear();
	if(default_text)
		set_text(state, text::produce_simple_string(state, default_text));
}
void template_mixed_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, default_tooltip);
}
void template_mixed_button::on_create(sys::state& state) noexcept {
	on_reset_text(state);
}

void template_mixed_button::render(sys::state& state, int32_t x, int32_t y) noexcept {
	template_project::mixed_region_template region;
	grid_size_window* par = static_cast<grid_size_window*>(parent);

	if(template_id == -1)
		return;

	auto ms_after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_activated);
	if(disabled) {
		region = state.ui_templates.mixed_button_t[template_id].disabled;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	} else if(ms_after.count() < mouse_over_animation_ms && state.ui_templates.mixed_button_t[template_id].animate_active_transition) {
		float percentage = float(ms_after.count()) / float(mouse_over_animation_ms);
		if(this == state.ui_state.under_mouse) {
			region = state.ui_templates.mixed_button_t[template_id].active;
			auto active_id = state.ui_templates.mixed_button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
			auto bg_id = state.ui_templates.mixed_button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			
		} else {
			region = state.ui_templates.mixed_button_t[template_id].primary;
			auto active_id = state.ui_templates.mixed_button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			auto bg_id = state.ui_templates.mixed_button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
		}
	} else if(this == state.ui_state.under_mouse) {
		region = state.ui_templates.mixed_button_t[template_id].active;
		auto active_id = region.bg;
		if(active_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
		
	} else {
		region = state.ui_templates.mixed_button_t[template_id].primary;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	}

	auto color = state.ui_templates.colors[region.shared_color];
	auto icon_space = region.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));

	{ // icon
		if(icon_id != -1) {
			auto l = region.icon_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto t = region.icon_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			auto r = region.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto b = region.icon_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
				state.ui_templates.icons[icon_id].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, color.r, color.g, color.b));
		}
	}
	{ // text
		auto fh = text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2);
		auto linesz = state.font_collection.line_height(state, fh);
		if(linesz == 0.f)
			return;

		int32_t yoff = 0;
		switch(region.v_text_alignment) {
		case template_project::aui_text_alignment::center: yoff = int32_t((base_data.size.y - linesz) / 2); break;
		case template_project::aui_text_alignment::left: yoff = int32_t(region.v_text_margins * par->grid_size); break;
		case template_project::aui_text_alignment::right: yoff = int32_t(base_data.size.y - linesz - region.v_text_margins * par->grid_size); break;
		}

		for(auto& t : internal_layout.contents) {
			ui::render_text_chunk(
				state,
				t,
				float(x) + t.x + region.h_text_margins * par->grid_size + icon_space,
				float(y + t.y + yoff),
				fh,
				ogl::color3f{ color.r, color.g, color.b },
				ogl::color_modification::none
			);
		}
	}
}


void template_mixed_button_ci::render(sys::state& state, int32_t x, int32_t y) noexcept {
	template_project::mixed_region_template region;
	grid_size_window* par = static_cast<grid_size_window*>(parent);

	if(template_id == -1)
		return;

	auto ms_after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_activated);
	if(disabled) {
		region = state.ui_templates.mixed_button_t[template_id].disabled;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	} else if(ms_after.count() < mouse_over_animation_ms && state.ui_templates.mixed_button_t[template_id].animate_active_transition) {
		float percentage = float(ms_after.count()) / float(mouse_over_animation_ms);
		if(this == state.ui_state.under_mouse) {
			region = state.ui_templates.mixed_button_t[template_id].active;
			auto active_id = state.ui_templates.mixed_button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
			auto bg_id = state.ui_templates.mixed_button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}

		} else {
			region = state.ui_templates.mixed_button_t[template_id].primary;
			auto active_id = state.ui_templates.mixed_button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			auto bg_id = state.ui_templates.mixed_button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
		}
	} else if(this == state.ui_state.under_mouse) {
		region = state.ui_templates.mixed_button_t[template_id].active;
		auto active_id = region.bg;
		if(active_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}

	} else {
		region = state.ui_templates.mixed_button_t[template_id].primary;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	}

	auto color = state.ui_templates.colors[region.shared_color];
	auto icon_space = region.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));

	{ // icon
		if(icon_id != -1) {
			auto l = region.icon_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto t = region.icon_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			auto r = region.icon_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + x;
			auto b = region.icon_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size)) + y;
			ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
				state.ui_templates.icons[icon_id].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, icon_color.r, icon_color.g, icon_color.b));
		}
	}
	{ // text
		auto fh = text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2);
		auto linesz = state.font_collection.line_height(state, fh);
		if(linesz == 0.f)
			return;

		int32_t yoff = 0;
		switch(region.v_text_alignment) {
		case template_project::aui_text_alignment::center: yoff = int32_t((base_data.size.y - linesz) / 2); break;
		case template_project::aui_text_alignment::left: yoff = int32_t(region.v_text_margins * par->grid_size); break;
		case template_project::aui_text_alignment::right: yoff = int32_t(base_data.size.y - linesz - region.v_text_margins * par->grid_size); break;
		}

		for(auto& t : internal_layout.contents) {
			ui::render_text_chunk(
				state,
				t,
				float(x) + t.x + region.h_text_margins * par->grid_size + icon_space,
				float(y + t.y + yoff),
				fh,
				ogl::color3f{ color.r, color.g, color.b },
				ogl::color_modification::none
			);
		}
	}
}


void template_text_button::set_text(sys::state& state, std::string_view new_text) {
	if(new_text != cached_text) {
		template_project::text_region_template region;
		grid_size_window* par = static_cast<grid_size_window*>(parent);

		if(template_id != -1) {
			region = state.ui_templates.button_t[template_id].primary;
		}

		cached_text = new_text;
		{
			internal_layout.contents.clear();
			internal_layout.number_of_lines = 0;

			text::single_line_layout sl{ internal_layout,
				text::layout_parameters{
					0, 0, static_cast<int16_t>(base_data.size.x - region.h_text_margins * par->grid_size* 2), static_cast<int16_t>(base_data.size.y - region.v_text_margins * 2),
					text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2), 0,
					convert_align(region.h_text_alignment), text::text_color::black, true, true
				},
				state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
			sl.add_text(state, cached_text);
		}
	}
}
void template_text_button::on_hover(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover(state);
}
void template_text_button::on_hover_end(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover_end(state);
}
void template_text_button::on_reset_text(sys::state& state) noexcept {
	cached_text.clear();
	if(default_text)
		set_text(state, text::produce_simple_string(state, default_text));
}
void template_text_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, default_tooltip);
}
void template_text_button::on_create(sys::state& state) noexcept {
	on_reset_text(state);
}

void template_text_button::render(sys::state& state, int32_t x, int32_t y) noexcept {
	template_project::text_region_template region;
	grid_size_window* par = static_cast<grid_size_window*>(parent);

	if(template_id == -1)
		return;

	auto ms_after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_activated);
	if(disabled) {
		region = state.ui_templates.button_t[template_id].disabled;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	} else if(ms_after.count() < mouse_over_animation_ms && state.ui_templates.button_t[template_id].animate_active_transition) {
		float percentage = float(ms_after.count()) / float(mouse_over_animation_ms);
		if(this == state.ui_state.under_mouse) {
			region = state.ui_templates.button_t[template_id].active;
			auto active_id = state.ui_templates.button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
			auto bg_id = state.ui_templates.button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}

		} else {
			region = state.ui_templates.button_t[template_id].primary;
			auto active_id = state.ui_templates.button_t[template_id].active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			auto bg_id = state.ui_templates.button_t[template_id].primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
		}
	} else if(this == state.ui_state.under_mouse) {
		region = state.ui_templates.button_t[template_id].active;
		auto active_id = region.bg;
		if(active_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}

	} else {
		region = state.ui_templates.button_t[template_id].primary;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	}

	auto color = state.ui_templates.colors[region.text_color];

	auto fh = text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2);
	auto linesz = state.font_collection.line_height(state, fh);
	if(linesz == 0.f)
		return;

	int32_t yoff = 0;
	switch(region.v_text_alignment) {
	case template_project::aui_text_alignment::center: yoff = int32_t((base_data.size.y - linesz) / 2); break;
	case template_project::aui_text_alignment::left: yoff = int32_t(region.v_text_margins * par->grid_size);  break;
	case template_project::aui_text_alignment::right: yoff = int32_t(base_data.size.y - linesz - region.v_text_margins * par->grid_size); break;
	}

	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(
			state,
			t,
			float(x) + t.x + region.h_text_margins * par->grid_size,
			float(y + t.y + yoff),
			fh,
			ogl::color3f{ color.r, color.g, color.b },
			ogl::color_modification::none
		);
	}
}


void template_toggle_button::set_text(sys::state& state, std::string_view new_text) {
	if(new_text != cached_text) {
		template_project::toggle_region region;
		grid_size_window* par = static_cast<grid_size_window*>(parent);

		if(template_id != -1) {
			region = is_active ? state.ui_templates.toggle_button_t[template_id].on_region : state.ui_templates.toggle_button_t[template_id].off_region;
		}

		auto l = region.text_margin_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));
		auto r = region.text_margin_right.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));

		cached_text = new_text;
		{
			internal_layout.contents.clear();
			internal_layout.number_of_lines = 0;

			text::single_line_layout sl{ internal_layout,
				text::layout_parameters{
					0, 0, static_cast<int16_t>(base_data.size.x - (l + r)), static_cast<int16_t>(base_data.size.y),
					text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2), 0,
					convert_align(region.h_text_alignment), text::text_color::black, true, true
				},
				state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
			sl.add_text(state, cached_text);
		}
	}
}
void template_toggle_button::on_hover(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover(state);
}
void template_toggle_button::on_hover_end(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
	button_on_hover_end(state);
}
void template_toggle_button::on_reset_text(sys::state& state) noexcept {
	cached_text.clear();
	if(default_text)
		set_text(state, text::produce_simple_string(state, default_text));
}
void template_toggle_button::set_active(sys::state& state, bool active) {
	if(active != is_active) {
		is_active = active;
		auto temp = std::move(cached_text);
		cached_text.clear();
		set_text(state, temp);
	}
}
void template_toggle_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, default_tooltip);
}
void template_toggle_button::on_create(sys::state& state) noexcept {
	on_reset_text(state);
}

void template_toggle_button::render(sys::state& state, int32_t x, int32_t y) noexcept {
	template_project::toggle_region mainregion;
	template_project::color_region region;
	grid_size_window* par = static_cast<grid_size_window*>(parent);

	if(template_id == -1)
		return;

	mainregion = is_active ? state.ui_templates.toggle_button_t[template_id].on_region : state.ui_templates.toggle_button_t[template_id].off_region;

	auto ms_after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_activated);
	if(disabled) {
		region = mainregion.disabled;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	} else if(ms_after.count() < mouse_over_animation_ms && state.ui_templates.toggle_button_t[template_id].animate_active_transition) {
		float percentage = float(ms_after.count()) / float(mouse_over_animation_ms);
		if(this == state.ui_state.under_mouse) {
			region = mainregion.active;
			auto active_id = region.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
			auto bg_id = mainregion.primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}

		} else {
			region = mainregion.primary;
			auto active_id = mainregion.active.bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			auto bg_id = mainregion.primary.bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
		}
	} else if(this == state.ui_state.under_mouse) {
		region = mainregion.active;
		auto active_id = region.bg;
		if(active_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}

	} else {
		region = mainregion.primary;
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	}

	auto color = state.ui_templates.colors[region.color];

	auto l = mainregion.text_margin_left.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));
	auto top = mainregion.text_margin_top.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));
	auto b = mainregion.text_margin_bottom.resolve(float(base_data.size.x), float(base_data.size.y), float(par->grid_size));

	auto fh = text::make_font_id(state, mainregion.font_choice == 1, mainregion.font_scale * par->grid_size * 2);
	auto linesz = state.font_collection.line_height(state, fh);
	if(linesz == 0.f)
		return;

	int32_t yoff = 0;
	switch(mainregion.v_text_alignment) {
	case template_project::aui_text_alignment::center: yoff = int32_t((base_data.size.y - linesz - (top + b)) / 2); break;
	case template_project::aui_text_alignment::left: yoff = int32_t(top);  break;
	case template_project::aui_text_alignment::right: yoff = int32_t(base_data.size.y - linesz - b); break;
	}

	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(
			state,
			t,
			float(x) + t.x + l,
			float(y + t.y + yoff),
			fh,
			ogl::color3f{ color.r, color.g, color.b },
			ogl::color_modification::none
		);
	}
}


void page_buttons::render(sys::state& state, int32_t x, int32_t y) noexcept {
	int16_t p = for_layout->current_page;
	int16_t z = int16_t(for_layout->page_starts.size());

	if(p != last_page || z != last_size) {
		text_layout.contents.clear();
		text_layout.number_of_lines = 0;
		last_page = p;
		last_size = z;
		if(z > 1) {
			std::string display = std::to_string(p + 1) + "/" + std::to_string(z);
			text::single_line_layout sl{ text_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, base_data.size.y), 0, text::alignment::center, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
			sl.add_text(state, display);
		}
	}

	if(last_size <= 1)
		return;

	if(for_layout->template_id == -1) {
		auto fh = text::make_font_id(state, false, base_data.size.y);
		auto linesz = state.font_collection.line_height(state, fh);
		if(linesz == 0.0f) return;
		auto ycentered = (base_data.size.y - linesz) / 2;
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false);
		for(auto& t : text_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)), fh, ui::get_text_color(state, ((layout_window_element*)parent)->page_text_color), cmod);
		}

		int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;

		// buttons, left
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && rel_mouse_x <= base_data.size.y, for_layout->current_page == 0, true), float(x), float(y), float(base_data.size.y), float(base_data.size.y), ogl::get_late_load_texture_handle(state, ((layout_window_element*)parent)->page_left_texture_id, ((layout_window_element*)parent)->page_left_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));

		// right
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && (base_data.size.x - base_data.size.y) <= rel_mouse_x, for_layout->current_page >= int32_t(for_layout->page_starts.size()) - 1, true), float(x + base_data.size.x - base_data.size.y), float(y), float(base_data.size.y), float(base_data.size.y), ogl::get_late_load_texture_handle(state, ((layout_window_element*)parent)->page_right_texture_id, ((layout_window_element*)parent)->page_right_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	} else {
		int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
		grid_size_window* par = static_cast<grid_size_window*>(parent);

		if(auto button_template = state.ui_templates.layout_region_t[for_layout->template_id].left_button; button_template != -1) { // left button
			auto icon = state.ui_templates.layout_region_t[for_layout->template_id].left_button_icon;
			auto x_pos = x;
			auto y_pos = y;
			template_project::icon_region_template region;
			if(for_layout->current_page == 0) {
				region = state.ui_templates.iconic_button_t[button_template].disabled;
			} else if(this == state.ui_state.under_mouse && rel_mouse_x <= base_data.size.y) {
				region = state.ui_templates.iconic_button_t[button_template].active;
			} else {
				region = state.ui_templates.iconic_button_t[button_template].primary;
			}
			auto bg_id = region.bg;
			if(bg_id != -1) {
				ogl::render_textured_rect_direct(state, float(x_pos), float(y_pos), float(base_data.size.y), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.y) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
			}
			if(icon != -1) {
				auto ico_color = region.icon_color;
				auto l = region.icon_left.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
				auto t = region.icon_top.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
				auto r = region.icon_right.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
				auto b = region.icon_bottom.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
				ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
					state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
			}
		}
		if(auto button_template = state.ui_templates.layout_region_t[for_layout->template_id].right_button; button_template != -1) { // right button
			auto icon = state.ui_templates.layout_region_t[for_layout->template_id].right_button_icon;
			auto x_pos = x + base_data.size.x - base_data.size.y;
			auto y_pos = y;
			template_project::icon_region_template region;
			if(for_layout->current_page >= int32_t(for_layout->page_starts.size()) - 1) {
				region = state.ui_templates.iconic_button_t[button_template].disabled;
			} else if(this == state.ui_state.under_mouse && (base_data.size.x - base_data.size.y) <= rel_mouse_x) {
				region = state.ui_templates.iconic_button_t[button_template].active;
			} else {
				region = state.ui_templates.iconic_button_t[button_template].primary;
			}
			auto bg_id = region.bg;
			if(bg_id != -1) {
				ogl::render_textured_rect_direct(state, float(x_pos), float(y_pos), float(base_data.size.y), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.y) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
			}
			if(icon != -1) {
				auto ico_color = region.icon_color;
				auto l = region.icon_left.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
				auto t = region.icon_top.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
				auto r = region.icon_right.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
				auto b = region.icon_bottom.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
				ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
					state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
			}
		}
		{ // text
			auto region = state.ui_templates.layout_region_t[for_layout->template_id].page_number_text;

			auto bg_id = region.bg;
			if(bg_id != -1) {
				ogl::render_textured_rect_direct(state, float(base_data.size.y), float(y), float(base_data.size.x - 2 * base_data.size.y), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x -  2 * base_data.size.y) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
			}

			auto fh = text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2);
			auto linesz = state.font_collection.line_height(state, fh);
			if(linesz == 0.0f) return;

			auto ycentered = (base_data.size.y - linesz) / 2;
			auto color = state.ui_templates.colors[region.text_color];

			for(auto& t : text_layout.contents) {
				ui::render_text_chunk(
					state,
					t,
					float(x) + t.x,
					float(y + t.y + ycentered),
					fh,
					ogl::color3f{ color.r, color.g, color.b },
					ogl::color_modification::none
				);
			}
		}
	}
}
ui::message_result page_buttons::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(last_size <= 1)
		return ui::message_result::unseen;

	if(x <= base_data.size.y) { // left button
		if(for_layout->current_page > 0) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			for_layout->change_page(state, *((layout_window_element*)parent), for_layout->current_page - 1);
		}
	} else if(base_data.size.x - base_data.size.y <= x) { // right button
		if(for_layout->current_page < int32_t(for_layout->page_starts.size()) - 1) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			for_layout->change_page(state, *((layout_window_element*)parent), for_layout->current_page + 1);
		}
	}

	return ui::message_result::consumed;
}
void page_buttons::on_update(sys::state& state) noexcept {

}

void drop_down_list_page_buttons::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(!owner_control)
		return;

	auto tid = owner_control->template_id;
	if(tid == -1)
		return;
	auto lt = state.ui_templates.drop_down_t[tid].layout_region_base;

	auto total_pages = (owner_control->total_items + owner_control->items_per_page  -1) / owner_control->items_per_page;

	if(owner_control->page_text_out_of_date) {
		text_layout.contents.clear();
		text_layout.number_of_lines = 0;
		if(total_pages > 1) {
			std::string display = std::to_string(owner_control->list_page + 1) + "/" + std::to_string(total_pages);
			text::single_line_layout sl{ text_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, base_data.size.y), 0, text::alignment::center, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
			sl.add_text(state, display);
		}
		owner_control->page_text_out_of_date = false;
	}

	if(total_pages <= 1)
		return;


	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	grid_size_window* par = static_cast<grid_size_window*>(parent);

	if(auto button_template = state.ui_templates.layout_region_t[lt].left_button; button_template != -1) { // left button
		auto icon = state.ui_templates.layout_region_t[lt].left_button_icon;
		auto x_pos = x;
		auto y_pos = y;
		template_project::icon_region_template region;
		if(owner_control->list_page == 0) {
			region = state.ui_templates.iconic_button_t[button_template].disabled;
		} else if(this == state.ui_state.under_mouse && rel_mouse_x <= base_data.size.y) {
			region = state.ui_templates.iconic_button_t[button_template].active;
		} else {
			region = state.ui_templates.iconic_button_t[button_template].primary;
		}
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x_pos), float(y_pos), float(base_data.size.y), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.y) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
		if(icon != -1) {
			auto ico_color = region.icon_color;
			auto l = region.icon_left.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
			auto t = region.icon_top.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
			auto r = region.icon_right.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
			auto b = region.icon_bottom.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
			ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
				state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
		}
	}
	if(auto button_template = state.ui_templates.layout_region_t[lt].right_button; button_template != -1) { // right button
		auto icon = state.ui_templates.layout_region_t[lt].right_button_icon;
		auto x_pos = x + base_data.size.x - base_data.size.y;
		auto y_pos = y;
		template_project::icon_region_template region;
		if(owner_control->list_page + 1 >= total_pages) {
			region = state.ui_templates.iconic_button_t[button_template].disabled;
		} else if(this == state.ui_state.under_mouse && (base_data.size.x - base_data.size.y) <= rel_mouse_x) {
			region = state.ui_templates.iconic_button_t[button_template].active;
		} else {
			region = state.ui_templates.iconic_button_t[button_template].primary;
		}
		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x_pos), float(y_pos), float(base_data.size.y), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.y) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
		if(icon != -1) {
			auto ico_color = region.icon_color;
			auto l = region.icon_left.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
			auto t = region.icon_top.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
			auto r = region.icon_right.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + x_pos;
			auto b = region.icon_bottom.resolve(float(base_data.size.y), float(base_data.size.y), float(par->grid_size)) + y_pos;
			ogl::render_textured_rect_direct(state, l, t, r - l, b - t,
				state.ui_templates.icons[icon].renders.get_render(state, int32_t(r - l), int32_t(b - t), state.user_settings.ui_scale, state.ui_templates.colors[ico_color].r, state.ui_templates.colors[ico_color].g, state.ui_templates.colors[ico_color].b));
		}
	}
	{ // text
		auto region = state.ui_templates.layout_region_t[lt].page_number_text;

		auto bg_id = region.bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(base_data.size.y), float(y), float(base_data.size.x - 2 * base_data.size.y), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x - 2 * base_data.size.y) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}

		auto fh = text::make_font_id(state, region.font_choice == 1, region.font_scale * par->grid_size * 2);
		auto linesz = state.font_collection.line_height(state, fh);
		if(linesz == 0.0f) return;

		auto ycentered = (base_data.size.y - linesz) / 2;
		auto color = state.ui_templates.colors[region.text_color];

		for(auto& t : text_layout.contents) {
			ui::render_text_chunk(
				state,
				t,
				float(x) + t.x,
				float(y + t.y + ycentered),
				fh,
				ogl::color3f{ color.r, color.g, color.b },
				ogl::color_modification::none
			);
		}
	}
}

ui::message_result drop_down_list_page_buttons::test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept {
	if(owner_control && type == ui::mouse_probe_type::click && owner_control->total_items > owner_control->items_per_page) {
		return ui::message_result::consumed;
	} else {
		return ui::message_result::unseen;
	}
}

ui::message_result drop_down_list_page_buttons::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(!owner_control)
		return ui::message_result::unseen;

	if(x <= base_data.size.y) { // left button
		if(owner_control->list_page > 0) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			owner_control->change_page(state, owner_control->list_page - 1);
		}
	} else if(base_data.size.x - base_data.size.y <= x) { // right button
		if(owner_control->list_page < (owner_control->total_items - 1) / owner_control->items_per_page) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			owner_control->change_page(state, owner_control->list_page + 1);
		}
	}

	return ui::message_result::consumed;
}
void drop_down_list_page_buttons::on_update(sys::state& state) noexcept {

}

void template_drop_down_control::on_hover(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
}
void template_drop_down_control::on_hover_end(sys::state& state) noexcept {
	last_activated = std::chrono::steady_clock::now();
}

void drop_down_list_button::on_update(sys::state& state) noexcept {
	if(owner_control) {
		if(owner_control->selected_item == list_id)
			icon_id = int16_t(state.ui_templates.drop_down_t[owner_control->template_id].selection_icon);
		else
			icon_id = -1;
	}
}
bool drop_down_list_button::button_action(sys::state& state) noexcept {
	if(owner_control) {
		owner_control->on_selection(state, list_id);
		owner_control->hide_list(state);
	}
	return true;
}

void template_drop_down_control::open_list(sys::state& state) {
	if(template_id == -1)
		return;
	if(state.ui_templates.drop_down_t[template_id].list_button == -1)
		return;
	if(total_items <= 0)
		return;

	auto current_root = state.current_scene.get_root(state);
	if(!state.ui_state.popup_menu) {
		auto new_menu = std::make_unique<alice_ui::pop_up_menu_container>();
		state.ui_state.popup_menu = new_menu.get();
		current_root->add_child_to_back(std::move(new_menu));
	}

	// resize and position list window TODO
	grid_size_window* par = static_cast<grid_size_window*>(parent);
	auto self_pos = ui::get_absolute_location(state, *this);

	auto total_vert_margin = state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size * 2;
	auto max_vert_elm_count = int32_t((state.ui_state.root->base_data.size.y - (self_pos.y + base_data.size.y + total_vert_margin + par->grid_size * 2)) / element_y_size);
	bool position_below = true;
	if(max_vert_elm_count < target_page_height || (target_page_height <= 0 && max_vert_elm_count < 3)) {
		max_vert_elm_count = int32_t((self_pos.y - (total_vert_margin + par->grid_size * 2)) / element_y_size);
		position_below = false;
	}
	if(max_vert_elm_count <= 0)
		return;

	bool one_page = false;
	if(target_page_height <= 0) {
		items_per_page = max_vert_elm_count;
	} else {
		items_per_page = std::min(max_vert_elm_count, target_page_height);
	}

	if(two_columns && items_per_page * 2 >= total_items) {
		items_per_page = (total_items + 1) / 2;
		one_page = true;
	}
	if(!two_columns && items_per_page >= total_items) {
		items_per_page = total_items;
		one_page = true;
	}
	

	auto lb = state.ui_templates.drop_down_t[template_id].list_button;
	auto elm_h_size = int32_t(state.ui_templates.mixed_button_t[lb].primary.h_text_margins * par->grid_size + state.ui_templates.mixed_button_t[lb].primary.icon_right.resolve(float(element_x_size), float(element_y_size), float(par->grid_size)) + element_x_size);

	state.ui_state.popup_menu->base_data.size.x = int16_t(state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size * 2 + elm_h_size + (two_columns ? elm_h_size : 0));
	state.ui_state.popup_menu->base_data.size.y = int16_t(state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size * 2 + items_per_page * element_y_size + (one_page ? 0 : par->grid_size * 2));

	state.ui_state.popup_menu->base_data.position.x = int16_t(self_pos.x + base_data.size.x / 2 - state.ui_state.popup_menu->base_data.size.x / 2);
	if(state.ui_state.popup_menu->base_data.position.x < 0) {
		state.ui_state.popup_menu->base_data.position.x = 0;
	}
	if(state.ui_state.popup_menu->base_data.position.x + state.ui_state.popup_menu->base_data.size.x > state.ui_state.root->base_data.size.x) {
		state.ui_state.popup_menu->base_data.position.x = int16_t(state.ui_state.root->base_data.size.x - state.ui_state.popup_menu->base_data.size.x);
	}
	if(position_below) {
		state.ui_state.popup_menu->base_data.position.y = int16_t(self_pos.y + base_data.size.y);
	} else {
		state.ui_state.popup_menu->base_data.position.y = int16_t(self_pos.y - state.ui_state.popup_menu->base_data.size.y);
	}

	if(two_columns)
		items_per_page *= 2;

	state.ui_state.popup_menu->scroll_redirect = this;
	change_page(state, 0);

	state.ui_state.popup_menu->grid_size = par->grid_size;
	state.ui_state.popup_menu->bg_template = state.ui_templates.drop_down_t[template_id].dropdown_window_bg;

	state.ui_state.popup_menu->flags = uint8_t(state.ui_state.popup_menu->flags & ~is_invisible_mask);
	if(state.ui_state.popup_menu->parent != current_root) {
		auto take_child = state.ui_state.popup_menu->parent->remove_child(state.ui_state.popup_menu);
		current_root->add_child_to_front(std::move(take_child));
	} else {
		current_root->move_child_to_front(state.ui_state.popup_menu);
	}
	state.ui_state.set_mouse_sensitive_target(state, state.ui_state.popup_menu);
}
void template_drop_down_control::hide_list(sys::state& state) {
	if(state.ui_state.popup_menu) {
		state.ui_state.popup_menu->set_visible(state, false);
	}
}
void template_drop_down_control::change_page(sys::state& state, int32_t to_page) {
	if(template_id == -1)
		return;
	if(!state.ui_state.popup_menu || state.ui_state.popup_menu->scroll_redirect != this) // menu not open
		return;

	state.ui_state.popup_menu->children.clear();
	page_text_out_of_date = true;

	grid_size_window* par = static_cast<grid_size_window*>(parent);
	auto lb = state.ui_templates.drop_down_t[template_id].list_button;
	auto elm_h_size = int32_t(state.ui_templates.mixed_button_t[lb].primary.h_text_margins * par->grid_size + state.ui_templates.mixed_button_t[lb].primary.icon_right.resolve(float(element_x_size), float(element_y_size), float(par->grid_size)) + element_x_size);

	list_page = to_page;

	if(total_items > items_per_page) { // add list buttons
		// create if non existing
		if(!page_controls) {
			page_controls = std::make_unique<drop_down_list_page_buttons>();
			page_controls->owner_control = this;
			page_controls->base_data.size.x = int16_t(par->grid_size * 8);
			page_controls->base_data.size.y = int16_t(par->grid_size * 2);
		}

		state.ui_state.popup_menu->children.push_back(page_controls.get());
		page_controls->base_data.position.x = int16_t(state.ui_state.popup_menu->base_data.size.x / 2 - page_controls->base_data.size.x / 2);
		page_controls->base_data.position.y = int16_t(state.ui_state.popup_menu->base_data.size.y - page_controls->base_data.size.y - (state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size));
		page_controls->parent = state.ui_state.popup_menu;
	}

	auto index = 0;
	while(index < items_per_page) { // place elements
		auto effective_index = to_page * items_per_page + index;
		if(effective_index >= total_items)
			break;

		auto x_offset = (two_columns && index >= items_per_page / 2 ? elm_h_size : 0) + (elm_h_size - element_x_size)  + state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size;
		auto y_offset = (two_columns && index >= items_per_page / 2 ? ((index - items_per_page / 2) * element_y_size) : index * element_y_size) + state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size;

		auto child = get_nth_item(state, effective_index, index);
		state.ui_state.popup_menu->children.push_back(child);
		child->base_data.position.x = int16_t(x_offset);
		child->base_data.position.y = int16_t(y_offset);
		child->parent = state.ui_state.popup_menu;
		child->impl_on_update(state);
		++index;
	}

	index = 0;
	bool alt = false;
	while(index < items_per_page) { // place buttons
		if(two_columns && index == items_per_page / 2)
			alt = false;

		auto effective_index = to_page * items_per_page + index;
		if(effective_index >= total_items)
			break;

		auto x_offset = (two_columns && index >= items_per_page / 2 ? elm_h_size : 0) + state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size;
		auto y_offset = (two_columns && index >= items_per_page / 2 ? ((index - items_per_page / 2) * element_y_size) : index * element_y_size) + state.ui_templates.drop_down_t[template_id].dropdown_window_margin * par->grid_size;
		if(index >= int32_t(list_buttons_pool.size())) {
			list_buttons_pool.emplace_back(std::make_unique<drop_down_list_button>());
			list_buttons_pool.back()->owner_control = this;
		}
		state.ui_state.popup_menu->children.push_back(list_buttons_pool[index].get());
		list_buttons_pool[index]->list_id = effective_index;
		list_buttons_pool[index]->template_id = (alt && state.ui_templates.drop_down_t[template_id].list_button_alt != -1) ? state.ui_templates.drop_down_t[template_id].list_button_alt : state.ui_templates.drop_down_t[template_id].list_button;
		list_buttons_pool[index]->base_data.position.x = int16_t(x_offset);
		list_buttons_pool[index]->base_data.position.y = int16_t(y_offset);
		list_buttons_pool[index]->base_data.size.x = int16_t(elm_h_size);
		list_buttons_pool[index]->base_data.size.y = int16_t(element_y_size);
		list_buttons_pool[index]->parent = state.ui_state.popup_menu;
		list_buttons_pool[index]->impl_on_update(state);
		alt = !alt;
		++index;
	}

}

void pop_up_menu_container::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(bg_template != -1) {
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_template].renders.get_render(state, float(base_data.size.x) / float(grid_size), float(base_data.size.y) / float(grid_size), grid_size, state.user_settings.ui_scale));
	}
}

void template_drop_down_control::on_create(sys::state& state) noexcept {
	grid_size_window* par = static_cast<grid_size_window*>(parent);
	label_window->base_data.position.y = int16_t(base_data.size.y / 2 - label_window->base_data.size.y / 2);
	label_window->base_data.position.x = int16_t((base_data.size.x - base_data.size.y) / 2 - label_window->base_data.size.x / 2);
	if(template_id != -1) {
		label_window->base_data.position.y += int16_t(state.ui_templates.drop_down_t[template_id].vertical_nudge);
	}
}

void template_drop_down_control::render(sys::state& state, int32_t x, int32_t y) noexcept {
	grid_size_window* par = static_cast<grid_size_window*>(parent);

	if(template_id == -1)
		return;

	auto ms_after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_activated);
	if(disabled) {
		auto bg_id = state.ui_templates.drop_down_t[template_id].disabled_bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	} else if(ms_after.count() < mouse_over_animation_ms && state.ui_templates.drop_down_t[template_id].animate_active_transition) {
		float percentage = float(ms_after.count()) / float(mouse_over_animation_ms);
		if(this == state.ui_state.under_mouse) {
			auto active_id = state.ui_templates.drop_down_t[template_id].active_bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
			auto bg_id = state.ui_templates.drop_down_t[template_id].primary_bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}

		} else {
			auto active_id = state.ui_templates.drop_down_t[template_id].active_bg;
			if(active_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					percentage, 1.0f);
			}
			auto bg_id = state.ui_templates.drop_down_t[template_id].primary_bg;
			if(bg_id != -1) {
				ogl::render_rect_slice(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale),
					0.0f, percentage);
			}
		}
	} else if(this == state.ui_state.under_mouse) {
		auto active_id = state.ui_templates.drop_down_t[template_id].active_bg;
		if(active_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[active_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}

	} else {
		auto bg_id = state.ui_templates.drop_down_t[template_id].primary_bg;
		if(bg_id != -1) {
			ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
		}
	}
}

void layout_window_element::initialize_template(sys::state& state, int32_t id, int32_t gs, bool ac) {
	window_template = id;
	grid_size = gs;
	if(ac) {
		auto_close = std::make_unique<auto_close_button>();
		auto_close->base_data.size.x = int16_t(grid_size * 3);
		auto_close->base_data.size.y = int16_t(grid_size * 3);
		auto_close->base_data.flags = uint8_t(ui::orientation::upper_right);
		if(window_template != -1) {
			auto_close->base_data.position.y = int16_t(state.ui_templates.window_t[window_template].v_close_button_margin * gs);
			auto_close->base_data.position.x = int16_t(-grid_size * 3 - state.ui_templates.window_t[window_template].h_close_button_margin * gs);
			auto_close->template_id = state.ui_templates.window_t[window_template].close_button_definition;
			auto_close->icon = state.ui_templates.window_t[window_template].close_button_icon;
		} else {
			auto_close->base_data.position.y = int16_t(0);
			auto_close->base_data.position.x = int16_t(-grid_size * 3);
		}
		auto_close->parent = this;
	}
}
void layout_window_element::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(window_template != -1 && state.ui_templates.window_t[window_template].bg != -1) {
		auto bgid = state.ui_templates.window_t[window_template].bg;
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				state.ui_templates.backgrounds[bgid].renders.get_render(state, float(base_data.size.x) / float(grid_size), float(base_data.size.y) / float(grid_size), grid_size, state.user_settings.ui_scale));
	}
	render_layout_internal(layout, state, x, y);
}

layout_level* innermost_scroll_level(layout_level& current, int32_t x, int32_t y) {
	if(x < current.resolved_x_pos || y < current.resolved_y_pos || current.resolved_x_pos + current.resolved_x_size < x || current.resolved_y_pos + current.resolved_y_size < y)
		return nullptr;

	for(auto& m : current.contents) {
		if(std::holds_alternative< sub_layout>(m)) {
			auto r = innermost_scroll_level(*std::get<sub_layout>(m).layout, x, y);
			if(r)
				return r;
		}
	}

	if(current.paged)
		return &current;

	return nullptr;
}

ui::message_result layout_window_element::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	auto sub_layout = innermost_scroll_level(layout, x, y);
	if(sub_layout) {
		sub_layout->change_page(state, *this, sub_layout->current_page + ((amount < 0) ? 1 : -1));
	}
	return ui::message_result::consumed;
}

struct layout_item_position {
	int32_t index = 0;
	int32_t sub_index = 0;

	bool operator==(layout_item_position const& o) const noexcept {
		return index == o.index && sub_index == o.sub_index;
	}
	bool operator!=(layout_item_position const& o) const noexcept {
		return !(*this == o);
	}
	bool operator<=(layout_item_position const& o) const noexcept {
		return (index < o.index) || (index == o.index && sub_index <= o.sub_index);
	}
	bool operator>=(layout_item_position const& o) const noexcept {
		return (index > o.index) || (index == o.index && sub_index >= o.sub_index);
	}
	bool operator<(layout_item_position const& o) const noexcept {
		return !(*this >= o);
	}
	bool operator>(layout_item_position const& o) const noexcept {
		return !(*this <= o);
	}
};

struct layout_iterator {
	std::vector<layout_item>& backing;
	layout_item_position position;

	layout_iterator(std::vector<layout_item>& backing) : backing(backing) {
	}

	bool current_is_glue() {
		return has_more() && std::holds_alternative<layout_glue>(backing[position.index]);
	}
	measure_result measure_current(sys::state& state, bool glue_horizontal, int32_t max_crosswise, bool first_in_section) {
		if(!has_more())
			return measure_result{ 0, 0, measure_result::special::none };
		auto& m = backing[position.index];

		if(std::holds_alternative<layout_control>(m)) {
			auto& i = std::get<layout_control>(m);
			if(i.absolute_position) {
				return  measure_result{ 0, 0, measure_result::special::none };
			}
			return  measure_result{ i.ptr->base_data.size.x, i.ptr->base_data.size.y, measure_result::special::none };
		} else if(std::holds_alternative<layout_window>(m)) {
			auto& i = std::get<layout_window>(m);
			if(i.absolute_position) {
				return  measure_result{ 0, 0, measure_result::special::none };
			}
			return  measure_result{ i.ptr->base_data.size.x, i.ptr->base_data.size.y, measure_result::special::none };
		} else if(std::holds_alternative<layout_glue>(m)) {
			auto& i = std::get<layout_glue>(m);
			if(glue_horizontal) {
				switch(i.type) {
				case glue_type::standard: return measure_result{ i.amount, 0, measure_result::special::none };
				case glue_type::at_least: return measure_result{ i.amount, 0, measure_result::special::space_consumer };
				case glue_type::line_break: return measure_result{ 0, 0, measure_result::special::end_line };
				case glue_type::page_break: return measure_result{ 0, 0, measure_result::special::end_page };
				case glue_type::glue_don_t_break: return measure_result{ i.amount, 0, measure_result::special::no_break };
				}
			} else {
				switch(i.type) {
				case glue_type::standard: return measure_result{ 0, i.amount, measure_result::special::none };
				case glue_type::at_least: return measure_result{ 0, i.amount, measure_result::special::space_consumer };
				case glue_type::line_break: return measure_result{ 0, 0, measure_result::special::end_line };
				case glue_type::page_break: return measure_result{ 0, 0, measure_result::special::end_page };
				case glue_type::glue_don_t_break: return measure_result{ 0, i.amount, measure_result::special::no_break };
				}
			}
		} else if(std::holds_alternative<generator_instance>(m)) {
			auto& i = std::get<generator_instance>(m);
			bool alt = false;
			return i.generator->place_item(state, nullptr, size_t(position.sub_index), 0, 0, first_in_section, alt);
		} else if(std::holds_alternative<sub_layout>(m)) {
			auto& i = std::get<sub_layout>(m);
			int32_t x = 0;
			int32_t y = 0;
			bool consume_fill = false;
			if(i.layout->size_x != -1)
				x = i.layout->size_x;
			else {
				if(glue_horizontal)
					consume_fill = true;
				else
					x = max_crosswise;
			}
			if(i.layout->size_y != -1)
				y = i.layout->size_y;
			else {
				if(!glue_horizontal)
					consume_fill = true;
				else
					y = max_crosswise;
			}

			return measure_result{ x, y, consume_fill ? measure_result::special::space_consumer : measure_result::special::none };
		}
		return measure_result{ 0, 0, measure_result::special::none };
	}
	void place_current(sys::state& state, layout_window_element* destination, int32_t x, int32_t y, int32_t width, int32_t height, bool first_in_section, bool& alternate, int32_t layout_x, int32_t layout_y, bool remake_pages) {
		if(!has_more())
			return;
		auto& m = backing[position.index];

		if(std::holds_alternative<layout_control>(m)) {
			auto& i = std::get<layout_control>(m);
			if(i.absolute_position) {
				i.ptr->base_data.position.x = int16_t(layout_x + i.abs_x);
				i.ptr->base_data.position.y = int16_t(layout_y + i.abs_y);
			} else {
				i.ptr->base_data.position.x = int16_t(x);
				i.ptr->base_data.position.y = int16_t(y);
			}
			destination->children.push_back(i.ptr);
			i.ptr->impl_on_update(state);
		} else if(std::holds_alternative<layout_window>(m)) {
			auto& i = std::get<layout_window>(m);
			if(i.absolute_position) {
				i.ptr->base_data.position.x = int16_t(layout_x + i.abs_x);
				i.ptr->base_data.position.y = int16_t(layout_y + i.abs_y);
			} else {
				i.ptr->base_data.position.x = int16_t(x);
				i.ptr->base_data.position.y = int16_t(y);
			}
			destination->children.push_back(i.ptr.get());
			i.ptr->impl_on_update(state);
		} else if(std::holds_alternative<layout_glue>(m)) {

		} else if(std::holds_alternative<generator_instance>(m)) {
			auto& i = std::get<generator_instance>(m);
			i.generator->place_item(state, destination, size_t(position.sub_index), x, y, first_in_section, alternate);
		} else if(std::holds_alternative<sub_layout>(m)) {
			auto& i = std::get<sub_layout>(m);
			destination->remake_layout_internal(*i.layout, state, x, y, width, height, remake_pages);
		}
	}
	void move_position(int32_t n) {
		while(n > 0 && has_more()) {
			if(std::holds_alternative<generator_instance>(backing[position.index])) {
				auto& g = std::get<generator_instance>(backing[position.index]);
				auto sub_count = g.generator->item_count();
				if(n >= int32_t(sub_count - position.sub_index)) {
					n -= int32_t(sub_count - position.sub_index);
					position.sub_index = 0;
					++position.index;
				} else {
					position.sub_index += n;
					n = 0;
				}
			} else {
				++position.index;
				--n;
			}
		}
		while(n < 0 && position.index >= 0) {
			if(position.index >= int32_t(backing.size())) {
				position.index = int32_t(backing.size()) - 1;
				if(backing.size() > 0 && std::holds_alternative<generator_instance>(backing[position.index])) {
					auto& g = std::get<generator_instance>(backing[position.index]);
					position.sub_index = std::max(int32_t(g.generator->item_count()) - 1, 0);
				}
				++n;
			} else if(std::holds_alternative<generator_instance>(backing[position.index])) {
				auto& g = std::get<generator_instance>(backing[position.index]);
				if(-n > position.sub_index) {
					n += (position.sub_index + 1);
					--position.index;
				} else {
					position.sub_index += n;
					n = 0;
					break; // don't reset sub index
				}
			} else {
				--position.index;
				++n;
			}

			if(position.index < 0) {
				position.sub_index = 0;
				position.index = 0; return;
			}
			if(std::holds_alternative<generator_instance>(backing[position.index])) {
				auto& g = std::get<generator_instance>(backing[position.index]);
				position.sub_index = std::max(int32_t(g.generator->item_count()) - 1, 0);
			}
		}
	}
	bool has_more() {
		return position.index < int32_t(backing.size());
	}
	void reset() {
		position.index = 0;
		position.sub_index = 0;
	}
};

void layout_window_element::impl_on_update(sys::state& state) noexcept {
	on_update(state);
}

void layout_window_element::clear_pages_internal(layout_level& lvl) {
	lvl.page_starts.clear();
	for(auto& m : lvl.contents) {
		if(holds_alternative<sub_layout>(m)) {
			auto& i = std::get<sub_layout>(m);
			clear_pages_internal(*i.layout);
		}
	}
}
void layout_window_element::render_layout_internal(layout_level& lvl, sys::state& state, int32_t x, int32_t y) {
	if(lvl.template_id != -1) {
		auto bg = state.ui_templates.layout_region_t[lvl.template_id].bg;
		if(bg != -1) {
			auto x_pos = lvl.resolved_x_pos;
			auto y_pos = lvl.resolved_y_pos;
			auto width = lvl.resolved_x_size;
			auto height = lvl.resolved_y_size;

			ogl::render_textured_rect_direct(state, float(x + x_pos), float(y + y_pos), float(width), float(height),
				state.ui_templates.backgrounds[bg].renders.get_render(state, float(width) / float(grid_size), float(height) / float(grid_size), int32_t(grid_size), state.user_settings.ui_scale));
		}
	}

	int32_t index_start = 0;
	if(lvl.current_page > 0 && lvl.page_starts.size() > 0) {
		index_start = lvl.page_starts[lvl.current_page - 1].last_index;
	}
	int32_t index_end = int32_t(lvl.contents.size());
	if(lvl.current_page < int32_t(lvl.page_starts.size())) {
		index_end = lvl.page_starts[lvl.current_page].last_index;
	}

	layout_iterator it(lvl.contents);
	it.move_position(index_start);
	for(int32_t j = index_start; j < index_end; ++j) {
		//sub_layout, texture_layer
		auto& c = lvl.contents[it.position.index];
		if(std::holds_alternative<texture_layer>(c)) {
			auto& i = std::get<texture_layer>(c);
			auto cmod = ui::get_color_modification(false, false, false);
			ogl::render_textured_rect(state, cmod, float(x + lvl.resolved_x_pos), float(y + lvl.resolved_y_pos), float(lvl.resolved_x_size), float(lvl.resolved_y_size), ogl::get_late_load_texture_handle(state, i.texture_id, i.texture), base_data.get_rotation(), false, state_is_rtl(state));
		} else if(std::holds_alternative<sub_layout>(c)) {
			auto& i = std::get<sub_layout>(c);
			render_layout_internal(*i.layout, state, x, y);
		}
		it.move_position(1);
	}
}

struct layout_box {
	uint16_t x_dim = 0;
	uint16_t y_dim = 0;
	uint16_t item_count = 0;
	uint16_t space_conumer_count = 0;
	uint16_t non_glue_count = 0;
	bool end_page = false;
};

layout_box measure_horizontal_box(sys::state& state, layout_iterator& source, int32_t max_x, int32_t max_y) {
	layout_box result{};

	auto initial_pos = source.position;

	while(source.has_more()) {
		auto m_result = source.measure_current(state, true, max_y, source.position == initial_pos);
		bool is_glue = source.current_is_glue();
		int32_t xdtemp = result.x_dim;
		bool fits = ((m_result.x_space + result.x_dim) <= max_x) || (source.position == initial_pos) || is_glue;

		if(fits) {
			result.x_dim = std::min(uint16_t(m_result.x_space + result.x_dim), uint16_t(max_x));
			int32_t xdtemp2 = result.x_dim;
			result.y_dim = std::max(result.y_dim, uint16_t(m_result.y_space));
			if(m_result.other == measure_result::special::space_consumer) {
				++result.space_conumer_count;
			}
			++result.item_count;
			if(!is_glue)
				++result.non_glue_count;
			if(m_result.other == measure_result::special::end_page) {
				result.end_page = true;
				source.move_position(1);
				break;
			}
			if(m_result.other == measure_result::special::end_line) {
				source.move_position(1);
				break;
			}
		} else {
			break;
		}

		source.move_position(1);
	}

	int32_t rollback_count = 0;
	auto rollback_end_pos = source.position;

	// rollback loop -- drop any items that were glued to the preivous item
	while(source.position > initial_pos) {
		source.move_position(-1);
		auto m_result = source.measure_current(state, true, max_y, source.position == initial_pos);
		if(m_result.other != measure_result::special::no_break) {
			source.move_position(1);
			break;
		}
		if(source.current_is_glue()) // don't break just before no break glue
			source.move_position(-1);
	}
	
	if(source.position > initial_pos && rollback_end_pos != (source.position)) { // non trivial rollback
		result = layout_box{};
		auto new_end = source.position;
		source.position = initial_pos;

		// final measurement loop if rollback was non zero
		while(source.position < new_end) {
			auto m_result = source.measure_current(state, true, max_y, source.position == initial_pos);
			bool is_glue = source.current_is_glue();

			result.x_dim = std::min(uint16_t(m_result.x_space + result.x_dim), uint16_t(max_x));
			result.y_dim = std::max(result.y_dim, uint16_t(m_result.y_space));
			if(m_result.other == measure_result::special::space_consumer) {
				++result.space_conumer_count;
			}
			if(!is_glue)
				++result.non_glue_count;
			++result.item_count;

			if(m_result.other == measure_result::special::end_page) {
				result.end_page = true;
			}

			source.move_position(1);
		}
	}

	return result;
}
layout_box measure_vertical_box(sys::state& state, layout_iterator& source, int32_t max_x, int32_t max_y) {
	layout_box result{};

	auto initial_pos = source.position;

	while(source.has_more()) {
		auto m_result = source.measure_current(state, false, max_x, source.position == initial_pos);
		bool is_glue = source.current_is_glue();
		bool fits = ((m_result.y_space + result.y_dim) <= max_y) || (source.position == initial_pos) || is_glue;

		if(fits) {
			result.y_dim = std::min(uint16_t(m_result.y_space + result.y_dim), uint16_t(max_y));
			result.x_dim = std::max(result.x_dim, uint16_t(m_result.x_space));
			if(m_result.other == measure_result::special::space_consumer) {
				++result.space_conumer_count;
			}
			++result.item_count;
			if(!is_glue)
				++result.non_glue_count;
			if(m_result.other == measure_result::special::end_page) {
				result.end_page = true;
				source.move_position(1);
				break;
			}
			if(m_result.other == measure_result::special::end_line) {
				source.move_position(1);
				break;
			}
		} else {
			break;
		}

		source.move_position(1);
	}

	int32_t rollback_count = 0;
	auto rollback_end_pos = source.position;

	// rollback loop -- drop any items that were glued to the preivous item
	while(source.position > initial_pos) {
		source.move_position(-1);
		auto m_result = source.measure_current(state, false, max_x, source.position == initial_pos);
		if(m_result.other != measure_result::special::no_break) {
			source.move_position(1);
			break;
		}
		if(source.current_is_glue()) // don't break just before no break glue
			source.move_position(-1);
	}

	if(source.position > initial_pos && rollback_end_pos != (source.position)) { // non trivial rollback
		result = layout_box{};
		auto new_end = source.position;
		source.position = initial_pos;

		// final measurement loop if rollback was non zero
		while(source.position < new_end) {
			auto m_result = source.measure_current(state, false, max_x, source.position == initial_pos);
			bool is_glue = source.current_is_glue();

			result.y_dim = std::min(uint16_t(m_result.y_space + result.y_dim), uint16_t(max_y));
			result.x_dim = std::max(result.x_dim, uint16_t(m_result.x_space));
			if(m_result.other == measure_result::special::space_consumer) {
				++result.space_conumer_count;
			}
			if(!is_glue)
				++result.non_glue_count;
			++result.item_count;

			if(m_result.other == measure_result::special::end_page) {
				result.end_page = true;
			}

			source.move_position(1);
		}
	}

	return result;
}

void layout_window_element::remake_layout_internal(layout_level& lvl, sys::state& state, int32_t x, int32_t y, int32_t w, int32_t h, bool remake_pages) {
	auto base_x_size = lvl.size_x != -1 ? int32_t(lvl.size_x) : w;
	auto base_y_size = lvl.size_y != -1 ? int32_t(lvl.size_y) : h;
	auto top_margin = int32_t(lvl.margin_top);
	auto bottom_margin = lvl.margin_bottom != -1 ? int32_t(lvl.margin_bottom) : top_margin;
	auto left_margin = lvl.margin_left != -1 ? int32_t(lvl.margin_left) : bottom_margin;
	auto right_margin = lvl.margin_right != -1 ? int32_t(lvl.margin_right) : left_margin;
	auto effective_x_size = base_x_size - (left_margin + right_margin);
	auto effective_y_size = base_y_size - (top_margin + bottom_margin);
	if(lvl.paged) {
		effective_y_size -= lvl.page_controls->base_data.size.y;
		lvl.page_controls->base_data.position.x = int16_t(x + left_margin + (effective_x_size / 2) - (lvl.page_controls->base_data.size.x / 2));
		lvl.page_controls->base_data.position.y = int16_t(y + top_margin + effective_y_size);
		children.push_back(lvl.page_controls.get());
	}

	lvl.resolved_x_pos = int16_t(x);
	lvl.resolved_y_pos = int16_t(y );
	lvl.resolved_x_size = int16_t(base_x_size);
	lvl.resolved_y_size = int16_t(base_y_size);

	if(lvl.page_starts.empty())
		remake_pages = true;

	if(remake_pages) {
		switch(lvl.type) {
		case layout_type::single_horizontal:
		{
			layout_iterator it(lvl.contents);
			// measure loop
			int32_t running_count = 0;
			while(it.has_more()) {
				auto box = measure_horizontal_box(state, it, effective_x_size, effective_y_size);
				lvl.page_starts.push_back(page_info{ uint16_t(running_count + box.item_count) });
				running_count += box.item_count;
				assert(box.item_count > 0);
			}
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ uint16_t(0) });
			}
		} break;
		case layout_type::single_vertical:
		{
			layout_iterator it(lvl.contents);
			// measure loop
			int32_t running_count = 0;
			while(it.has_more()) {
				auto box = measure_vertical_box(state, it, effective_x_size, effective_y_size);
				lvl.page_starts.push_back(page_info{ uint16_t(running_count + box.item_count) });
				running_count += box.item_count;
				assert(box.item_count > 0);
			}
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ uint16_t(0) });
			}
		} break;
		case layout_type::overlapped_horizontal:
		{
			layout_iterator it(lvl.contents);
			auto box = measure_horizontal_box(state, it, std::numeric_limits<int32_t>::max(), effective_y_size);
			lvl.page_starts.push_back(page_info{ uint16_t(box.item_count) });
		} break;
		case layout_type::overlapped_vertical:
		{
			layout_iterator it(lvl.contents);
			auto box = measure_vertical_box(state, it, effective_x_size, std::numeric_limits<int32_t>::max());
			lvl.page_starts.push_back(page_info{ uint16_t(box.item_count) });
		} break;
		case layout_type::mulitline_horizontal:
		{
			layout_iterator it(lvl.contents);
			int32_t running_count = 0;

			while(it.has_more()) {
				int32_t y_remaining = effective_y_size;
				bool first = true;
				while(it.has_more()) {
					auto pre_pos = it.position;

					auto box = measure_horizontal_box(state, it, effective_x_size, y_remaining);
					assert(box.item_count > 0);
					if(box.y_dim > y_remaining && !first) { // end
						it.position = pre_pos;
						break;
					}
					running_count += box.item_count;
					y_remaining -= int32_t(box.y_dim + lvl.interline_spacing);
					if(y_remaining <= 0)
						break;
					if(box.end_page)
						break;
					first = false;
				}
				lvl.page_starts.push_back(page_info{ uint16_t(running_count)  });
			}
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ uint16_t(0) });
			}
		} break;
		case layout_type::multiline_vertical:
		{
			layout_iterator it(lvl.contents);
			int32_t running_count = 0;

			while(it.has_more()) {
				int32_t x_remaining = effective_x_size;
				bool first = true;
				while(it.has_more()) {
					auto pre_pos = it.position;

					auto box = measure_vertical_box(state, it, effective_x_size, x_remaining);
					assert(box.item_count > 0);
					if(box.x_dim > x_remaining && !first) { // end
						it.position = pre_pos;
						break;
					}
					running_count += box.item_count;
					x_remaining -= int32_t(box.x_dim + lvl.interline_spacing);
					if(x_remaining <= 0)
						break;
					if(box.end_page)
						break;
					first = false;
				}
				lvl.page_starts.push_back(page_info{ uint16_t(running_count) });
			}
			if(lvl.page_starts.empty()) {
				lvl.page_starts.push_back(page_info{ uint16_t(0) });
			}
		} break;
		}
	} // end: remake lists

	for(auto& m : lvl.contents) {
		if(holds_alternative<generator_instance>(m)) {
			auto& i = std::get<generator_instance>(m);
			i.generator->reset_pools();
		}
	}

	// handle texture layers here: they do not depend on layout type

	if(window_template == -1) { // temporary until all ui windows are handled by templates
		layout_iterator place_it(lvl.contents);
		while(place_it.has_more()) {
			if(std::holds_alternative<texture_layer>(lvl.contents[place_it.position.index])) {
				auto& i = std::get<texture_layer>(lvl.contents[place_it.position.index]);
				positioned_texture temp{
					(int16_t)lvl.resolved_x_pos, (int16_t)lvl.resolved_y_pos, (int16_t)effective_x_size, (int16_t)effective_y_size,
					i.texture,
					dcon::texture_id{},
					i.texture_type
				};
				textures_to_render.push_back(temp);
			}
			place_it.move_position(1);
		}
	}

	switch(lvl.type) {
	case layout_type::single_horizontal:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		int32_t index_start = 0;
		if(lvl.current_page > 0) {
			index_start = lvl.page_starts[lvl.current_page - 1].last_index;
		}

		layout_iterator it(lvl.contents);
		it.move_position(index_start);

		auto start_pos = it.position;
		auto box = measure_horizontal_box(state, it, effective_x_size, effective_y_size);
		it.position = start_pos;

		int32_t space_used = box.x_dim;
		int32_t fill_consumer_count = box.space_conumer_count;
		// place / render

		int32_t extra_runlength = int32_t(effective_x_size - space_used);
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}

		space_used = x + extra_lead + left_margin;
		bool alternate = true;
		for(uint16_t i = 0; i < box.item_count; ++i) {
			auto mr =  it.measure_current(state, true, effective_y_size, i == 0);
			int32_t yoff = 0;
			int32_t xoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: yoff = y + top_margin; break;
			case layout_line_alignment::trailing: yoff = y + top_margin + effective_y_size - mr.y_space; break;
			case layout_line_alignment::centered: yoff = y + top_margin + (effective_y_size - mr.y_space) / 2;  break;
			}

			it.place_current(state, this, xoff, yoff, mr.x_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), mr.y_space, i == 0, alternate, x, y, remake_pages);
			it.move_position(1);

			space_used += mr.x_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
		}
	} break;
	case layout_type::single_vertical:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		int32_t index_start = 0;
		if(lvl.current_page > 0) {
			index_start = lvl.page_starts[lvl.current_page - 1].last_index;
		}

		layout_iterator it(lvl.contents);
		it.move_position(index_start);

		auto start_pos = it.position;
		auto box = measure_vertical_box(state, it, effective_x_size, effective_y_size);
		it.position = start_pos;

		int32_t space_used = box.y_dim;
		int32_t fill_consumer_count = box.space_conumer_count;
		// place / render

		int32_t extra_runlength = int32_t(effective_y_size - space_used);
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}

		space_used = y + extra_lead + top_margin;
		bool alternate = true;
		for(uint16_t i = 0; i < box.item_count; ++i) {
			auto mr = it.measure_current(state, false, effective_x_size, i == 0);

			int32_t xoff = 0;
			int32_t yoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: xoff = x + left_margin; break;
			case layout_line_alignment::trailing: xoff = x + left_margin + effective_x_size - mr.x_space; break;
			case layout_line_alignment::centered: xoff = x + left_margin + (effective_x_size - mr.x_space) / 2;  break;
			}

			it.place_current(state, this, xoff, yoff, mr.x_space, mr.y_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), i == 0, alternate, x, y, remake_pages);
			it.move_position(1);

			space_used += mr.y_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
		}
	} break;
	case layout_type::overlapped_horizontal:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		int32_t index_start = 0;
		if(lvl.current_page > 0) {
			index_start = lvl.page_starts[lvl.current_page - 1].last_index;
		}
		place_it.move_position(index_start);

		auto pre_pos = place_it.position;
		auto box = measure_horizontal_box(state, place_it, std::numeric_limits<int32_t>::max(), effective_y_size);
		place_it.position = pre_pos;

		int32_t space_used = box.x_dim;
		int32_t fill_consumer_count = box.space_conumer_count;
		int32_t non_glue_count = box.non_glue_count;

		int32_t extra_runlength = std::max(0, int32_t(effective_x_size - space_used));
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}
		int32_t overlap_subtraction = (non_glue_count > 1 && space_used > effective_x_size) ? int32_t(space_used - effective_x_size) / (non_glue_count - 1) : 0;
		space_used = x + extra_lead + left_margin;

		bool page_first = true;
		bool alternate = true;
		while(place_it.has_more()) {
			auto mr = place_it.measure_current(state, true, effective_y_size, page_first);
			int32_t yoff = 0;
			int32_t xoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: yoff = y + top_margin; break;
			case layout_line_alignment::trailing: yoff = y + top_margin + effective_y_size - mr.y_space; break;
			case layout_line_alignment::centered: yoff = y + top_margin + (effective_y_size - mr.y_space) / 2;  break;
			}
			bool was_abs = false;
			if(std::holds_alternative< layout_control>(lvl.contents[place_it.position.index])) {
				auto& i = std::get<layout_control>(lvl.contents[place_it.position.index]);
				was_abs = i.absolute_position;
			} else if(std::holds_alternative< layout_window>(lvl.contents[place_it.position.index])) {
				auto& i = std::get<layout_window>(lvl.contents[place_it.position.index]);
				was_abs = i.absolute_position;
			}
			place_it.place_current(state, this, xoff, yoff, mr.x_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), mr.y_space, page_first, alternate, x, y, remake_pages);

			if(!place_it.current_is_glue()) {
				page_first = false;
			}

			space_used += mr.x_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
			if(!place_it.current_is_glue() && !was_abs)
				space_used -= overlap_subtraction;

			place_it.move_position(1);
		}
	} break;
	case layout_type::overlapped_vertical:
	{
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));

		layout_iterator place_it(lvl.contents);
		int32_t index_start = 0;
		if(lvl.current_page > 0) {
			index_start = lvl.page_starts[lvl.current_page - 1].last_index;
		}
		place_it.move_position(index_start);

		place_it.move_position(index_start);
		auto pre_pos = place_it.position;
		auto box = measure_horizontal_box(state, place_it, effective_x_size, std::numeric_limits<int32_t>::max());
		place_it.position = pre_pos;

		int32_t space_used = box.y_dim;
		int32_t fill_consumer_count = box.space_conumer_count;
		int32_t non_glue_count = box.non_glue_count;

		int32_t extra_runlength = std::max(0, int32_t(effective_y_size - space_used));
		int32_t per_fill_consumer = fill_consumer_count != 0 ? (extra_runlength / fill_consumer_count) : 0;
		int32_t extra_lead = 0;
		switch(lvl.line_alignment) {
		case layout_line_alignment::leading: break;
		case layout_line_alignment::trailing: extra_lead = extra_runlength - fill_consumer_count * per_fill_consumer; break;
		case layout_line_alignment::centered: extra_lead = (extra_runlength - fill_consumer_count * per_fill_consumer) / 2;  break;
		}
		int32_t overlap_subtraction = (non_glue_count > 1 && space_used > effective_y_size) ? int32_t(space_used - effective_y_size) / (non_glue_count - 1) : 0;
		space_used = y + extra_lead + top_margin;

		bool page_first = true;
		bool alternate = true;
		while(place_it.has_more()) {
			auto mr = place_it.measure_current(state, false, effective_x_size, page_first);
			int32_t xoff = 0;
			int32_t yoff = space_used;
			switch(lvl.line_internal_alignment) {
			case layout_line_alignment::leading: xoff = x + left_margin; break;
			case layout_line_alignment::trailing: xoff = x + left_margin + effective_x_size - mr.x_space; break;
			case layout_line_alignment::centered: xoff = x + left_margin + (effective_x_size - mr.x_space) / 2;  break;
			}
			bool was_abs = false;
			if(std::holds_alternative< layout_control>(lvl.contents[place_it.position.index])) {
				auto& i = std::get<layout_control>(lvl.contents[place_it.position.index]);
				was_abs = i.absolute_position;
			} else if(std::holds_alternative< layout_window>(lvl.contents[place_it.position.index])) {
				auto& i = std::get<layout_window>(lvl.contents[place_it.position.index]);
				was_abs = i.absolute_position;
			}

			place_it.place_current(state, this, xoff, yoff, mr.x_space, mr.y_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), page_first, alternate, x, y, remake_pages);

			if(!place_it.current_is_glue()) {
				page_first = false;
			}

			space_used += mr.y_space;
			if(mr.other == measure_result::special::space_consumer) {
				space_used += per_fill_consumer;
			}
			if(!place_it.current_is_glue() && !was_abs)
				space_used -= overlap_subtraction;

			place_it.move_position(1);
		}
	} break;
	case layout_type::mulitline_horizontal:
	{
		layout_iterator place_it(lvl.contents);
		int32_t index_start = 0;
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));
		if(lvl.current_page > 0) {
			index_start = lvl.page_starts[lvl.current_page - 1].last_index;
		}
		place_it.move_position(index_start);

		int32_t y_remaining = effective_y_size;
		bool first = true;
		while(place_it.has_more()) {
			auto pre_pos = place_it.position;

			auto box = measure_horizontal_box(state, place_it, effective_x_size, y_remaining);
			assert(box.item_count > 0);
			if(box.y_dim > y_remaining && !first) { // end
				break;
			}

			place_it.position = pre_pos;
			bool alternate = true;

			int32_t extra_runlength = int32_t(effective_x_size - box.x_dim);
			int32_t per_fill_consumer = box.space_conumer_count != 0 ? (extra_runlength / box.space_conumer_count) : 0;
			int32_t extra_lead = 0;
			switch(lvl.line_alignment) {
			case layout_line_alignment::leading: break;
			case layout_line_alignment::trailing: extra_lead = extra_runlength - box.space_conumer_count * per_fill_consumer; break;
			case layout_line_alignment::centered: extra_lead = (extra_runlength - box.space_conumer_count * per_fill_consumer) / 2;  break;
			}
			auto space_used = x + extra_lead + left_margin;

			for(uint16_t i = 0; i < box.item_count; ++i) {
				auto mr = place_it.measure_current(state, false, effective_x_size, i == 0);

				int32_t yoff = 0;
				int32_t xoff = space_used;
				switch(lvl.line_internal_alignment) {
				case layout_line_alignment::leading: yoff = y + top_margin + (effective_y_size - y_remaining); break;
				case layout_line_alignment::trailing: yoff = y + top_margin + (effective_y_size - y_remaining) + box.y_dim - mr.y_space; break;
				case layout_line_alignment::centered: yoff = y + top_margin + (effective_y_size - y_remaining) + (box.y_dim - mr.y_space) / 2;  break;
				}
				place_it.place_current(state, this, xoff, yoff, mr.x_space, mr.y_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), i == 0, alternate, x, y, remake_pages);
				place_it.move_position(1);

				space_used += mr.x_space;
				if(mr.other == measure_result::special::space_consumer) {
					space_used += per_fill_consumer;
				}
			}

			y_remaining -= int32_t(box.y_dim + lvl.interline_spacing);
			if(y_remaining <= 0) {
				break;
			}
			if(box.end_page) {
				break;
			}
			first = false;
		}
		
	} break;
	case layout_type::multiline_vertical:
	{
		layout_iterator place_it(lvl.contents);
		int32_t index_start = 0;
		lvl.current_page = std::clamp(lvl.current_page, int16_t(0), int16_t(lvl.page_starts.size() - 1));
		if(lvl.current_page > 0) {
			index_start = lvl.page_starts[lvl.current_page - 1].last_index;
		}
		place_it.move_position(index_start);


		int32_t x_remaining = effective_x_size;
		bool first = true;
		while(place_it.has_more()) {
			auto pre_pos = place_it.position;

			auto box = measure_vertical_box(state, place_it, x_remaining, effective_y_size);
			assert(box.item_count > 0);
			if(box.x_dim > x_remaining && !first) { // end
				break;
			}

			place_it.position = pre_pos;
			bool alternate = true;

			int32_t extra_runlength = int32_t(effective_y_size - box.y_dim);
			int32_t per_fill_consumer = box.space_conumer_count != 0 ? (extra_runlength / box.space_conumer_count) : 0;
			int32_t extra_lead = 0;
			switch(lvl.line_alignment) {
			case layout_line_alignment::leading: break;
			case layout_line_alignment::trailing: extra_lead = extra_runlength - box.space_conumer_count * per_fill_consumer; break;
			case layout_line_alignment::centered: extra_lead = (extra_runlength - box.space_conumer_count * per_fill_consumer) / 2;  break;
			}
			auto space_used = y + extra_lead + top_margin;

			for(uint16_t i = 0; i < box.item_count; ++i) {
				auto mr = place_it.measure_current(state, false, effective_x_size, i == 0);

				int32_t xoff = 0;
				int32_t yoff = space_used;
				switch(lvl.line_internal_alignment) {
				case layout_line_alignment::leading: xoff = x + left_margin + (effective_x_size - x_remaining); break;
				case layout_line_alignment::trailing: xoff = x + left_margin + (effective_x_size - x_remaining) + box.x_dim - mr.x_space; break;
				case layout_line_alignment::centered: xoff = x + left_margin + (effective_x_size - x_remaining) + (box.x_dim - mr.x_space) / 2;  break;
				}
				place_it.place_current(state, this, xoff, yoff, mr.x_space + (mr.other == measure_result::special::space_consumer ? per_fill_consumer : 0), mr.y_space, i == 0, alternate, x, y, remake_pages);
				place_it.move_position(1);

				space_used += mr.y_space;
				if(mr.other == measure_result::special::space_consumer) {
					space_used += per_fill_consumer;
				}
			}

			x_remaining -= int32_t(box.x_dim + lvl.interline_spacing);
			if(x_remaining <= 0)
				break;
			if(box.end_page)
				break;
			first = false;
		}

	} break;
	}
}


std::string_view get_setting_text_key(int32_t type) {
	static char const* key_str[] = {
		"amsg_revolt",								  // revolt
		"amsg_war_on_nation",						  // war_on_nation
		"amsg_war_by_nation",						  // war_by_nation
		"amsg_wargoal_added",						  // wargoal_added
		"amsg_siegeover_by_nation",					  // siegeover_by_nation
		"amsg_siegeover_on_nation",					  // siegeover_on_nation
		"amsg_colony_finished",						  // colony_finished
		"amsg_reform_gained",						  // reform_gained
		"amsg_reform_lost",							  // reform_lost
		"amsg_ruling_party_change",					  // ruling_party_change
		"amsg_upperhouse",							  // upperhouse
		"amsg_electionstart",						  // electionstart
		"amsg_electiondone",							  // electiondone
		"amsg_breakcountry",							  // breakcountry
		"amsg_peace_accepted_from_nation",			  // peace_accepted_from_nation
		"amsg_peace_rejected_from_nation",			  // peace_rejected_from_nation
		"amsg_peace_accepted_by_nation",				  // peace_accepted_by_nation
		"amsg_peace_rejected_by_nation",				  // peace_rejected_by_nation
		"amsg_mobilization_start",					  // mobilization_start
		"amsg_mobilization_end",						  // mobilization_end
		"amsg_factory_complete",						  // factory_complete
		"amsg_rr_complete",							  // rr_complete
		"amsg_fort_complete",						  // fort_complete
		"amsg_naval_base_complete",					  // naval_base_complete
		"amsg_province_event",						  // province_event
		"amsg_national_event",						  // national_event
		"amsg_major_event",							  // major_event
		"amsg_invention",							  // invention
		"amsg_tech",									  // tech
		"amsg_leader_dies",							  // leader_dies
		"amsg_land_combat_starts_on_nation",			  // land_combat_starts_on_nation
		"amsg_naval_combat_starts_on_nation",		  // naval_combat_starts_on_nation
		"amsg_land_combat_starts_by_nation",			  // land_combat_starts_by_nation
		"amsg_naval_combat_starts_by_nation",		  // naval_combat_starts_by_nation
		"amsg_movement_finishes",					  // movement_finishes
		"amsg_decision",								  // decision
		"amsg_lose_great_power",						  // lose_great_power
		"amsg_become_great_power",					  // become_great_power
		"amsg_war_subsidies_start_by_nation",		  // war_subsidies_start_by_nation
		"amsg_war_subsidies_start_on_nation",		  // war_subsidies_start_on_nation
		"amsg_war_subsidies_end_by_nation",			  // war_subsidies_end_by_nation
		"amsg_war_subsidies_end_on_nation",			  // war_subsidies_end_on_nation
		"amsg_reparations_start_by_nation",			  // reparations_start_by_nation
		"amsg_reparations_start_on_nation",			  // reparations_start_on_nation
		"amsg_reparations_end_by_nation",			  // reparations_end_by_nation
		"amsg_reparations_end_on_nation",			  // reparations_end_on_nation
		"amsg_mil_access_start_by_nation",			  // mil_access_start_by_nation
		"amsg_mil_access_start_on_nation",			  // mil_access_start_on_nation
		"amsg_mil_access_end_by_nation",				  // mil_access_end_by_nation
		"amsg_mil_access_end_on_nation",				  // mil_access_end_on_nation
		"amsg_mil_access_declined_by_nation",		  // mil_access_declined_by_nation
		"amsg_mil_access_declined_on_nation",		  // mil_access_declined_on_nation
		"amsg_alliance_starts",						  // alliance_starts
		"amsg_alliance_ends",						  // alliance_ends
		"amsg_alliance_declined_by_nation",			  // alliance_declined_by_nation
		"amsg_alliance_declined_on_nation",			  // alliance_declined_on_nation
		"amsg_ally_called_accepted_by_nation",		  // ally_called_accepted_by_nation
		"amsg_ally_called_declined_by_nation",		  // ally_called_declined_by_nation
		"amsg_discredit_by_nation",					  // discredit_by_nation
		"amsg_ban_by_nation",						  // ban_by_nation
		"amsg_expell_by_nation",						  // expell_by_nation
		"amsg_discredit_on_nation",					  // discredit_on_nation
		"amsg_ban_on_nation",						  // ban_on_nation
		"amsg_expell_on_nation",						  // expell_on_nation
		"amsg_increase_opinion",						  // increase_opinion
		"amsg_decrease_opinion_by_nation",			  // decrease_opinion_by_nation
		"amsg_decrease_opinion_on_nation",			  // decrease_opinion_on_nation
		"amsg_rem_sphere_by_nation",					  // rem_sphere_by_nation
		"amsg_rem_sphere_on_nation",					  // rem_sphere_on_nation
		"amsg_removed_from_sphere",					  // removed_from_sphere
		"amsg_add_sphere",							  // add_sphere
		"amsg_added_to_sphere",						  // added_to_sphere
		"amsg_increase_relation_by_nation",			  // increase_relation_by_nation
		"amsg_increase_relation_on_nation",			  // increase_relation_on_nation
		"amsg_decrease_relation_by_nation",			  // decrease_relation_by_nation
		"amsg_decrease_relation_on_nation",			  // decrease_relation_on_nation
		"amsg_join_war_by_nation",					  // join_war_by_nation
		"amsg_join_war_on_nation",					  // join_war_on_nation
		"amsg_gw_unlocked",							  // gw_unlocked
		"amsg_war_becomes_great",					  // war_becomes_great
		"amsg_cb_detected_on_nation",				  // cb_detected_on_nation
		"amsg_cb_detected_by_nation",				  // cb_detected_by_nation
		"amsg_crisis_join_offer_accepted_by_nation",	  // crisis_join_offer_accepted_by_nation
		"amsg_crisis_join_offer_declined_by_nation",	  // crisis_join_offer_declined_by_nation
		"amsg_crisis_join_offer_accepted_from_nation", // crisis_join_offer_accepted_from_nation
		"amsg_crisis_join_offer_declined_from_nation", // crisis_join_offer_declined_from_nation
		"amsg_crisis_resolution_accepted",			  // crisis_resolution_accepted
		"amsg_crisis_becomes_war",					  // crisis_becomes_war
		"amsg_crisis_resolution_declined_from_nation", // crisis_resolution_declined_from_nation
		"amsg_crisis_starts",						  // crisis_starts
		"amsg_crisis_attacker_backer",				  // crisis_attacker_backer
		"amsg_crisis_defender_backer",				  // crisis_defender_backer
		"amsg_crisis_fizzle",						  // crisis_fizzle
		"amsg_war_join_by",							  // war_join_by
		"amsg_war_join_on",							  // war_join_on
		"amsg_cb_fab_finished",						  // cb_fab_finished
		"amsg_cb_fab_cancelled",						  // cb_fab_cancelled
		"amsg_crisis_voluntary_join",				  // crisis_voluntary_join
		"amsg_army_built", // army_built
		"amsg_navy_built", // navy_built
		"amsg_bankruptcy", //bankruptcy
		"amsg_entered_automatic_alliance",//entered_automatic_alliance
		"amsg_chat_message",//chat_message
		"amsg_embargo",
		"amsg_embargod"
	};
	return std::string_view{ key_str[type] };
}


void describe_conversion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto location = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto conversion_chances = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.conversion_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

	float base_amount =
		state.defines.conversion_scale *
		(state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::conversion_rate) + 1.0f) *
		(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_conversion_rate) + 1.0f) *
		conversion_chances;

	auto pr = state.world.pop_get_religion(ids);
	auto state_religion = state.world.nation_get_religion(owner);

	// pops of the state religion do not convert
	if(state_religion == pr)
		base_amount = 0.0f;

	// need at least 1 pop following the religion in the province
	if(state.world.province_get_demographics(location, demographics::to_key(state, state_religion.id)) < 1.f)
		base_amount = 0.0f;

	text::add_line(state, contents, "pop_conver_1", text::variable_type::x, int64_t(std::max(0.0f, state.world.pop_get_size(ids) * base_amount)));
	text::add_line_break_to_layout(state, contents);

	if(state_religion == pr) {
		text::add_line(state, contents, "pop_conver_2");
		return;
	}
	// need at least 1 pop following the religion in the province
	if(state.world.province_get_demographics(location, demographics::to_key(state, state_religion.id)) < 1.f) {
		text::add_line(state, contents, "pop_conver_3");
		return; // early exit
	}
	text::add_line(state, contents, "pop_conver_4");
	text::add_line(state, contents, "pop_conver_5", text::variable_type::x, text::fp_three_places{ state.defines.conversion_scale });
	text::add_line(state, contents, "pop_conver_6", text::variable_type::x, text::fp_two_places{ std::max(0.0f,state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_conversion_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_conversion_rate, true);
	text::add_line(state, contents, "pop_conver_7", text::variable_type::x, text::fp_two_places{ std::max(0.0f, state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::conversion_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, location, 15, sys::provincial_mod_offsets::conversion_rate, true);

	text::add_line(state, contents, "pop_conver_8", text::variable_type::x, text::fp_four_places{ conversion_chances });
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.conversion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

void describe_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);

	auto owners = state.world.province_get_nation_from_province_ownership(loc);
	float base = 0.f;
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.bureaucrat
		&& (
			state.world.nation_get_accepted_cultures(owners, state.world.pop_get_culture(ids))
			|| state.world.nation_get_primary_culture(owners) == state.world.pop_get_culture(ids)
			)
	) {
		base = demographics::administration_base_push;
		text::add_line(state, contents, "pop_migration_bureaucracy", text::variable_type::x, text::fp_two_places{ base });
	}

	if(state.world.province_get_is_colonial(loc) && base == 0.f) {
		text::add_line(state, contents, "pop_mig_1");
		return;
	}
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_mig_2");
		return;
	}

	auto migration_chance = base + std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.migration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto prov_mod = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_mig_3", text::variable_type::x, text::fp_three_places{ scale }, text::variable_type::y,
			text::fp_percentage{ prov_mod }, text::variable_type::val, text::fp_two_places{ migration_chance });

	ui::active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);

	text::add_line(state, contents, "pop_mig_4");
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.migration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0);
}

void describe_colonial_migration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);


	if(state.world.nation_get_is_colonial_nation(owner) == false) {
		text::add_line(state, contents, "pop_cmig_1");
		return;
	}
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_cmig_2");
		return;
	}
	auto pt = state.world.pop_get_poptype(ids);
	if(pt == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_cmig_3");
		return;
	}
	if(state.world.pop_type_get_strata(pt) == uint8_t(culture::pop_strata::rich)) {
		text::add_line(state, contents, "pop_cmig_4");
		return;
	}
	if(pt == state.culture_definitions.primary_factory_worker || pt == state.culture_definitions.secondary_factory_worker) {
		text::add_line(state, contents, "pop_cmig_5");
		return;
	}

	float base = 0.f;
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.bureaucrat
		&& (
			state.world.nation_get_accepted_cultures(owner, state.world.pop_get_culture(ids))
			|| state.world.nation_get_primary_culture(owner) == state.world.pop_get_culture(ids)
			)
	) {
		base = demographics::administration_base_push;
		text::add_line(state, contents, "pop_migration_bureaucracy", text::variable_type::x, text::fp_two_places{ base });
	}

	auto mig_chance = base + std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.colonialmigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto im_push = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto cmig = (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::colonial_migration) + 1.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_cmig_6", text::variable_type::x, text::fp_three_places{ scale }, text::variable_type::y,
			text::fp_percentage{ im_push }, text::variable_type::num, text::fp_percentage{ cmig }, text::variable_type::val,
			text::fp_two_places{ mig_chance });

	ui::active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);
	ui::active_modifiers_description(state, contents, owner, 0, sys::national_mod_offsets::colonial_migration, true);

	text::add_line(state, contents, "pop_cmig_7");
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.colonialmigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0);
}

void describe_emigration(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owners = state.world.province_get_nation_from_province_ownership(loc);

	if(state.world.nation_get_is_civilized(owners) == false) {
		text::add_line(state, contents, "pop_emg_1");
		return;
	}
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_emg_2");
		return;
	}
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_emg_3");
		return;
	}
	if(state.world.culture_group_get_is_overseas(
		state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(ids))) == false) {
		text::add_line(state, contents, "pop_emg_4");
		return;
	}

	auto impush = (state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::immigrant_push) + 1.0f);
	auto emig = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.emigration_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);
	auto scale = state.defines.immigration_scale;

	text::add_line(state, contents, "pop_emg_5", text::variable_type::x, text::fp_three_places{ scale }, text::variable_type::y,
			text::fp_percentage{ impush }, text::variable_type::val, text::fp_two_places{ emig });

	ui::active_modifiers_description(state, contents, loc, 0, sys::provincial_mod_offsets::immigrant_push, true);

	text::add_line(state, contents, "pop_emg_6");
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.emigration_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}

void describe_promotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {

	auto owner = nations::owner_of_pop(state, ids);
	auto promotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.promotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	text::add_line(state, contents, "pop_prom_1");
	if(promoted_type) {
		if(promoted_type == ptype) {
			text::add_line(state, contents, "pop_prom_3");
		} else if(state.world.pop_type_get_strata(promoted_type) >= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{ promotion_bonus });
			promotion_chance += promotion_bonus;
		} 
	}
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.promotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	ui::additive_value_modifier_description(state, contents, state.culture_definitions.demotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	if(promotion_chance <= 0.0f) {
		text::add_line(state, contents, "pop_prom_7");
		return;
	}
	text::add_line(state, contents, "pop_prom_8", text::variable_type::x, text::fp_three_places{ state.defines.promotion_scale },
				text::variable_type::val, text::fp_two_places{ promotion_chance });
	
}

void describe_demotion(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {

	auto owner = nations::owner_of_pop(state, ids);
	auto demotion_chance = trigger::evaluate_additive_modifier(state, state.culture_definitions.demotion_chance,
			trigger::to_generic(ids), trigger::to_generic(ids), 0);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto si = state.world.province_get_state_membership(loc);
	auto nf = state.world.state_instance_get_owner_focus(si);
	auto promoted_type = state.world.national_focus_get_promotion_type(nf);
	auto promotion_bonus = state.world.national_focus_get_promotion_amount(nf);
	auto ptype = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(ptype);

	text::add_line(state, contents, "pop_prom_4");
	if(promoted_type) {
		if(promoted_type == ptype) {
			text::add_line(state, contents, "pop_prom_3");
		} else if(state.world.pop_type_get_strata(promoted_type) <= strata) {
			text::add_line(state, contents, "pop_prom_2", text::variable_type::val, text::fp_two_places{ promotion_bonus });
			demotion_chance += promotion_bonus;
		}
	}
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.promotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	ui::additive_value_modifier_description(state, contents, state.culture_definitions.demotion_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);

	text::add_line_break_to_layout(state, contents);

	if(demotion_chance <= 0.0f) {
		text::add_line(state, contents, "pop_prom_5");
		return;
	}
	text::add_line(state, contents, "pop_prom_9", text::variable_type::x, text::fp_three_places{ state.defines.promotion_scale },
				text::variable_type::val, text::fp_two_places{ demotion_chance });
	
}

void describe_con(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const clergy_key = demographics::to_key(state, state.culture_definitions.clergy);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	float cfrac =
		state.world.province_get_demographics(loc, clergy_key) / state.world.province_get_demographics(loc, demographics::total);
	auto types = state.world.pop_get_poptype(ids);

	float lx_mod = pop_demographics::get_luxury_needs(state, ids) * state.defines.con_luxury_goods;
	float cl_mod = cfrac * (state.world.pop_type_get_strata(types) == int32_t(culture::pop_strata::poor) ?
														state.defines.con_poor_clergy : state.defines.con_midrich_clergy);
	float lit_mod = ((state.world.nation_get_plurality(owner) / 10.0f) *
								 (state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f) *
								 state.defines.con_literacy * pop_demographics::get_literacy(state, ids) *
								 (state.world.province_get_is_colonial(loc) ? state.defines.con_colonial_factor : 1.0f)) / 10.f;

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_consciousness_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_consciousness_modifier);
	float cmod = (state.world.province_get_is_colonial(loc) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_consciousness_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = (state.world.pop_get_is_primary_or_accepted_culture(ids) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier));
	auto old_con = pop_demographics::get_consciousness(state, ids) * 0.01f;
	auto total = (lx_mod + (cl_mod + lit_mod - old_con)) + (local_mod + sep_mod);

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_con_2");
	text::add_line(state, contents, "pop_con_3", text::variable_type::x, text::fp_two_places{ lx_mod });
	text::add_line(state, contents, "pop_con_4", text::variable_type::val, text::fp_percentage{ cfrac }, text::variable_type::x,
			text::fp_two_places{ cl_mod });
	text::add_line(state, contents, "pop_con_5", text::variable_type::x, text::fp_two_places{ lit_mod });

	text::add_line(state, contents, "pop_con_6", text::variable_type::x,
			text::fp_two_places{ state.world.nation_get_plurality(owner) }, 15);
	text::add_line(state, contents, "pop_con_7", text::variable_type::x,
			text::fp_percentage{ state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::literacy_con_impact) + 1.0f },
			15);
	ui::active_modifiers_description(state, contents, owner, 30, sys::national_mod_offsets::literacy_con_impact, false);
	text::add_line(state, contents, "pop_con_8", text::variable_type::x, text::fp_two_places{ state.defines.con_literacy }, 15);
	text::add_line(state, contents, "pop_con_9", text::variable_type::x, text::fp_percentage{ pop_demographics::get_literacy(state, ids) },
			15);
	if(state.world.province_get_is_colonial(loc)) {
		text::add_line(state, contents, "pop_con_10", text::variable_type::x, text::fp_two_places{ state.defines.con_colonial_factor },
				15);
	}
	text::add_line(state, contents, "pop_con_11", 15);
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_12");
		if(pmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::pop_consciousness_modifier, false);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_13");
		if(omod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_pop_consciousness_modifier, false);
	}
	if(!state.world.province_get_is_colonial(loc)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_14");
		if(cmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::core_pop_consciousness_modifier, false);
	}
	if(!state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_con_15");
		if(sep_mod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ sep_mod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ sep_mod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::non_accepted_pop_consciousness_modifier,
				false);
	}
	text::add_line(state, contents, "alice_con_decay_description", text::variable_type::x, text::fp_three_places{ pop_demographics::get_consciousness(state, ids) * 0.01f });
}

void describe_mil(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto const conservatism_key = pop_demographics::to_key(state, state.culture_definitions.conservative);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);
	auto ruling_party = state.world.nation_get_ruling_party(owner);
	auto ruling_ideology = state.world.political_party_get_ideology(ruling_party);

	float lx_mod = std::max(pop_demographics::get_luxury_needs(state, ids) - 0.5f, 0.0f) * state.defines.mil_has_luxury_need;
	float con_sup = (pop_demographics::get_demo(state, ids, conservatism_key) * state.defines.mil_ideology);
	float ruling_sup = ruling_ideology ? pop_demographics::get_demo(state, ids, pop_demographics::to_key(state, ruling_ideology)) * state.defines.mil_ruling_party : 0.0f;
	float ref_mod = state.world.province_get_is_colonial(loc)
		? 0.0f
		: (pop_demographics::get_social_reform_desire(state, ids) + pop_demographics::get_political_reform_desire(state, ids)) *
		(state.defines.mil_require_reform * 0.25f);

	float sub_t = (lx_mod + ruling_sup) + (con_sup + ref_mod);

	auto o_spending = state.world.nation_get_overseas_penalty(owner);
	auto spending_level = state.world.nation_get_spending_level(owner);
	auto overseas_mil = 0.f;
	if(province::is_overseas(state, loc)) {
		overseas_mil = 2.f * state.defines.alice_overseas_mil * (0.5f - (o_spending * spending_level));
	}

	float pmod = state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::pop_militancy_modifier);
	float omod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_pop_militancy_modifier);
	float cmod = (state.world.province_get_is_colonial(loc) ? 0.0f :
			state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::core_pop_militancy_modifier));

	float local_mod = (pmod + omod) + cmod;

	float sep_mod = (state.world.pop_get_is_primary_or_accepted_culture(ids) ? 0.0f :
			(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_militancy_modifier) + 1.0f) *
					state.defines.mil_non_accepted);
	float ln_mod = std::min((pop_demographics::get_life_needs(state, ids) - 0.5f), 0.0f) * state.defines.mil_no_life_need;
	float en_mod_a =
		std::min(0.0f, (pop_demographics::get_everyday_needs(state, ids) - 0.5f)) * state.defines.mil_lack_everyday_need;
	float en_mod_b =
		std::max(0.0f, (pop_demographics::get_everyday_needs(state, ids) - 0.5f)) * state.defines.mil_has_everyday_need;
	//Ranges from +0.00 - +0.50 militancy monthly, 0 - 100 war exhaustion
	float war_exhaustion =
		state.world.nation_get_war_exhaustion(owner) * state.defines.mil_war_exhaustion;
	auto old_mil = pop_demographics::get_militancy(state, ids) * 0.01f;
	float total = (sub_t + local_mod) + ((sep_mod - ln_mod - old_mil) + (en_mod_b - en_mod_a) + (war_exhaustion + overseas_mil));

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::red);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::red);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ total }, text::text_color::green);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "pop_mil_2");
	text::add_line(state, contents, "pop_mil_3", text::variable_type::x, text::fp_two_places{ ln_mod < 0.0f ? -ln_mod : 0.0f });
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_4");
		if(en_mod_b - en_mod_a >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::red);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ en_mod_b - en_mod_a }, text::text_color::red);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ en_mod_b - en_mod_a }, text::text_color::green);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line(state, contents, "pop_mil_5", text::variable_type::x, text::fp_two_places{ lx_mod });
	text::add_line(state, contents, "pop_mil_6", text::variable_type::x, text::fp_two_places{ con_sup });
	text::add_line(state, contents, "pop_mil_7", text::variable_type::x, text::fp_two_places{ ruling_sup });
	text::add_line(state, contents, "pop_mil_8", text::variable_type::x, text::fp_two_places{ ref_mod });
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_9");
		if(pmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ pmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, loc, 15, sys::provincial_mod_offsets::pop_militancy_modifier, false);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_10");
		if(omod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ omod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_pop_militancy_modifier, false);
	}
	if(!state.world.province_get_is_colonial(loc)) {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_mil_11");
		if(cmod >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, text::fp_two_places{ cmod }, text::text_color::red);
		}
		text::close_layout_box(contents, box);
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::core_pop_militancy_modifier, false);
	}
	if(!state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		text::add_line(state, contents, "pop_mil_12",
			text::variable_type::val, text::fp_two_places{ sep_mod },
			text::variable_type::x, text::fp_two_places{ state.defines.mil_non_accepted },
			text::variable_type::y, text::fp_percentage{ state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::non_accepted_pop_militancy_modifier) + 1.0f });
		ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::non_accepted_pop_militancy_modifier, false);
	}
	if(war_exhaustion > 0) {
		text::add_line(state, contents, "pop_mil_13", text::variable_type::val, text::fp_three_places{ war_exhaustion });
	}

	auto box = text::open_layout_box(contents);
	text::localised_format_box(state, contents, box, "alice_overseas_mil_description");
	text::add_space_to_layout_box(state, contents, box);
	if(overseas_mil <= 0) {
		text::add_to_layout_box(state, contents, box, text::fp_two_places{ overseas_mil }, text::text_color::green);
	} else {
		text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::red);
		text::add_to_layout_box(state, contents, box, text::fp_two_places{ overseas_mil }, text::text_color::red);
	}
	text::close_layout_box(contents, box);

	text::add_line(state, contents, "alice_mil_decay_description", text::variable_type::x, text::fp_three_places{ pop_demographics::get_militancy(state, ids) * state.defines.alice_militancy_decay });
}

void describe_lit(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto pop_budget = economy::pops::prepare_pop_budget(state, ids);

	auto box = text::open_layout_box(contents);

	text::add_line(state, contents, "pop_literacy_spending",
		text::variable_type::val, text::format_money(pop_budget.education.spent)
	);


	text::add_line(state, contents, "pop_literacy_spending_required",
		text::variable_type::val, text::format_money(pop_budget.education.required)
	);

	text::add_line(state, contents, "pop_literacy_spending_public",
		text::variable_type::val, text::format_percentage(pop_budget.education.satisfied_for_free_ratio)
	);

	auto result = pop_budget.education.satisfied_for_free_ratio + pop_budget.education.satisfied_with_money_ratio;

	text::add_line(state, contents, "pop_literacy_spending_ratio",
		text::variable_type::x, text::format_percentage(result)
	);

	if(result > 0.9f) {
		text::add_line(state, contents, "pop_literacy_spending_result_success",
			text::variable_type::x, text::format_percentage(result),
			text::variable_type::y, text::format_percentage(0.9f),
			text::variable_type::val, text::format_float(pop_demographics::pop_u16_scaling, 10)
		);
	}
	if(result < 0.7f) {
		text::add_line(state, contents, "pop_literacy_spending_result_failure",
			text::variable_type::x, text::format_percentage(result),
			text::variable_type::y, text::format_percentage(0.7f),
			text::variable_type::val, text::format_float(pop_demographics::pop_u16_scaling, 10)
		);
	}
}

void describe_money(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto savings = state.world.pop_get_savings(ids);
	auto pop_budget = economy::pops::prepare_pop_budget(state, ids);

	auto box = text::open_layout_box(contents);

	text::add_line(state, contents, "pop_budget_explanation_header",
		text::variable_type::val, text::format_money(savings)
	);

	// needs

	text::add_line(state, contents, "pop_budget_explanation_life_needs",
		text::variable_type::x, text::format_money(pop_budget.life_needs.spent),
		text::variable_type::y, text::format_money(pop_budget.life_needs.required),
		text::variable_type::val, text::format_percentage(pop_budget.life_needs.satisfied_with_money_ratio)
	);

	text::add_line(state, contents, "pop_budget_explanation_subsistence",
		text::variable_type::x, text::format_percentage(pop_budget.life_needs.satisfied_for_free_ratio),
		text::variable_type::y, text::format_percentage(pop_budget.life_needs.satisfied_with_money_ratio + pop_budget.life_needs.satisfied_for_free_ratio)
	);

	text::add_line(state, contents, "pop_budget_explanation_everyday_needs",
		text::variable_type::x, text::format_money(pop_budget.everyday_needs.spent),
		text::variable_type::y, text::format_money(pop_budget.everyday_needs.required),
		text::variable_type::val, text::format_percentage(pop_budget.everyday_needs.satisfied_with_money_ratio)
	);

	text::add_line(state, contents, "pop_budget_explanation_luxury_needs",
		text::variable_type::x, text::format_money(pop_budget.luxury_needs.spent),
		text::variable_type::y, text::format_money(pop_budget.luxury_needs.required),
		text::variable_type::val, text::format_percentage(pop_budget.luxury_needs.satisfied_with_money_ratio)
	);

	text::add_line(state, contents, "pop_budget_explanation_education",
		text::variable_type::x, text::format_money(pop_budget.education.spent)
	);

	text::add_line(state, contents, "pop_budget_explanation_investments",
		text::variable_type::x, text::format_money(pop_budget.investments.spent)
	);
	text::add_line(state, contents, "pop_budget_explanation_banks",
		text::variable_type::x, text::format_money(pop_budget.bank_savings.spent)
	);
}

void describe_growth(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto type = state.world.pop_get_poptype(ids);

	auto loc = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(loc);

	auto mod_life_rating = province::effective_life_rating_growth(state, loc);
	auto lr_factor = demographics::popgrowth_from_life_rating(state, mod_life_rating);

	auto modifiers = demographics::get_pop_growth_modifiers(state, ids);

	auto ln_factor = demographics::get_pop_starvation_factor(state, ids);


	auto total_factor = demographics::get_monthly_pop_growth_factor(state, ids);

	auto ln_penalty_scale = demographics::get_pop_starvation_penalty_scale(state, ids, modifiers);

	auto ln_penalty = ln_penalty_scale * ln_factor;

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_growth_1");
		if(total_factor >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, int64_t(total_factor * state.world.pop_get_size(ids)), text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, int64_t(total_factor * state.world.pop_get_size(ids)),
					text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);

	if(type == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_growth_2", text::variable_type::val, text::fp_one_place {state.defines.slave_growth_divisor });
	}
	text::add_line(state, contents, "pop_growth_3");
	text::add_line(state, contents, "pop_growth_4", text::variable_type::x, text::fp_percentage_two_places{ total_factor });
	//text::add_line(state, contents, "pop_growth_5", text::variable_type::x, text::fp_four_places{ state.defines.base_popgrowth }, 15);
	text::add_line(state, contents, "pop_growth_5", text::variable_type::x, text::fp_one_place{ mod_life_rating },
			text::variable_type::y, text::fp_one_place{ state.defines.min_life_rating_for_growth }, text::variable_type::val,
			text::fp_percentage_two_places{ state.defines.life_rating_growth_bonus }, text::variable_type::value, text::fp_percentage_two_places{ lr_factor },
			30);
	text::add_line(state, contents, "pop_growth_6", text::variable_type::x, text::fp_percentage_two_places{ state.defines.base_popgrowth },
			30);

	text::add_line(state, contents, "pop_growth_7", text::variable_type::x,
			text::fp_percentage_two_places{ state.world.province_get_modifier_values(loc, sys::provincial_mod_offsets::population_growth) }, 30);
	ui::active_modifiers_description(state, contents, loc, 45, sys::provincial_mod_offsets::population_growth, false);
	text::add_line(state, contents, "pop_growth_8", text::variable_type::x,
			text::fp_percentage_two_places{ state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pop_growth) }, 30);
	ui::active_modifiers_description(state, contents, owner, 45, sys::national_mod_offsets::pop_growth, false);

	text::add_line(state, contents, "pop_growth_9", text::variable_type::x, text::fp_percentage_two_places{ ln_penalty },
			text::variable_type::y, text::fp_percentage{ pop_demographics::get_life_needs(state, ids) }, text::variable_type::val,
			text::fp_percentage{ state.defines.life_need_starvation_limit });

	text::add_line(state, contents, "pop_growth_10", text::variable_type::x, text::fp_percentage_two_places{ state.defines.alice_max_starvation_degrowth }, text::variable_type::y, text::fp_percentage_two_places{ ln_penalty_scale }, text::variable_type::val, text::fp_percentage_two_places{modifiers });
	text::add_line(state, contents, "pop_growth_11", text::variable_type::x, text::fp_percentage{ state.defines.life_need_starvation_limit }, text::variable_type::y, text::fp_one_place{ ln_factor });
	text::add_line(state, contents, "pop_growth_12", text::variable_type::x, text::fp_one_place{ ln_factor }, text::variable_type::y, text::fp_percentage_two_places{ln_penalty_scale }, text::variable_type::val, text::fp_percentage_two_places{ln_penalty });
}

void describe_assimilation(sys::state& state, text::columnar_layout& contents, dcon::pop_id ids) {
	auto location = state.world.pop_get_province_from_pop_location(ids);
	auto owner = state.world.province_get_nation_from_province_ownership(location);
	auto assimilation_chances = std::max(trigger::evaluate_additive_modifier(state, state.culture_definitions.assimilation_chance, trigger::to_generic(ids), trigger::to_generic(ids), 0), 0.0f);

	auto pc = state.world.pop_get_culture(ids);

	float culture_size = state.world.province_get_demographics(location, demographics::to_key(state, pc));
	float total_size = state.world.province_get_demographics(location, demographics::total);
	auto anti_spam_measure = std::max(0.f, 0.04f - culture_size / (total_size + 1.f)) * 50.f;

	float base_amount =
		anti_spam_measure +
		state.defines.assimilation_scale *
		(state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f) *
		(state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f) *
		assimilation_chances;

	/*
	In a colonial province, assimilation numbers for pops with an *non* "overseas"-type culture group are reduced by a
	factor of 100. In a non-colonial province, assimilation numbers for pops with an *non* "overseas"-type culture
	group are reduced by a factor of 10.
	*/

	if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
		base_amount /= 10.0f;
	}


	/*
	All pops have their assimilation numbers reduced by a factor of 100 per core in the province sharing their primary
	culture.
	*/
	for(auto core : state.world.province_get_core(location)) {
		if(core.get_identity().get_primary_culture() == pc) {
			base_amount /= 100.0f;
		}
	}

	// slaves do not assimilate
	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves)
		base_amount = 0.0f;

	// pops of an accepted culture do not assimilate
	if(state.world.pop_get_is_primary_or_accepted_culture(ids))
		base_amount = 0.0f;

	// pops in an overseas and colonial province do not assimilate
	if(state.world.province_get_is_colonial(location) && province::is_overseas(state, location))
		base_amount = 0.0f;

	text::add_line(state, contents, "pop_assim_1", text::variable_type::x, int64_t(std::max(0.0f, state.world.pop_get_size(ids) * base_amount)));
	text::add_line_break_to_layout(state, contents);

	if(state.world.pop_get_poptype(ids) == state.culture_definitions.slaves) {
		text::add_line(state, contents, "pop_assim_2");
		return;
	}
	if(state.world.pop_get_is_primary_or_accepted_culture(ids)) {
		text::add_line(state, contents, "pop_assim_3");
		return;
	}
	if(state.world.province_get_is_colonial(location)) {
		text::add_line(state, contents, "pop_assim_4");
		return;
	}
	if(province::is_overseas(state, location)) {
		text::add_line(state, contents, "pop_assim_5");
		return;
	}
	text::add_line(state, contents, "pop_assim_6", text::variable_type::x, text::fp_three_places{ anti_spam_measure });
	text::add_line(state, contents, "pop_assim_7", text::variable_type::x, text::fp_three_places{ state.defines.assimilation_scale });
	text::add_line(state, contents, "pop_assim_8", text::variable_type::x, text::fp_two_places{ std::max(0.0f,state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::global_assimilation_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::global_assimilation_rate, true);
	text::add_line(state, contents, "pop_assim_9", text::variable_type::x, text::fp_two_places{ std::max(0.0f, state.world.province_get_modifier_values(location, sys::provincial_mod_offsets::assimilation_rate) + 1.0f) });
	ui::active_modifiers_description(state, contents, location, 15, sys::provincial_mod_offsets::assimilation_rate, true);

	if(!state.world.culture_group_get_is_overseas(state.world.culture_get_group_from_culture_group_membership(pc))) {
		text::add_line(state, contents, "pop_assim_10");
	}
	float core_factor = 1.0f;
	for(auto core : state.world.province_get_core(location)) {
		if(core.get_identity().get_primary_culture() == pc) {
			core_factor /= 100.0f;
		}
	}
	if(core_factor < 1.0f) {
		text::add_line(state, contents, "pop_assim_11", text::variable_type::x, text::fp_four_places{ core_factor });
	}
	text::add_line(state, contents, "pop_assim_12", text::variable_type::x, text::fp_four_places{ assimilation_chances });
	ui::additive_value_modifier_description(state, contents, state.culture_definitions.assimilation_chance, trigger::to_generic(ids),
			trigger::to_generic(ids), 0);
}


void pop_screen_sort_state_rows(sys::state& state, std::vector<dcon::state_instance_id>& state_instances, alice_ui::layout_window_element* parent) {
	bool work_to_do = false;
	auto table_source = (alice_ui::demographicswindow_main_t*)(parent);
	if(table_source->table_location_sort_direction != 0) work_to_do = true;
	if(table_source->table_size_sort_direction != 0) work_to_do = true;
	if(table_source->table_culture_sort_direction != 0) work_to_do = true;
	if(table_source->table_job_sort_direction != 0) work_to_do = true;
	if(table_source->table_religion_sort_direction != 0) work_to_do = true;
	if(table_source->table_militancy_sort_direction != 0) work_to_do = true;
	if(table_source->table_consciousness_sort_direction != 0) work_to_do = true;
	if(table_source->table_employment_sort_direction != 0) work_to_do = true;
	if(table_source->table_literacy_sort_direction != 0) work_to_do = true;
	if(table_source->table_money_sort_direction != 0) work_to_do = true;
	if(table_source->table_needs_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		if(table_source->table_location_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, std::string>>sort_values;
			for(auto si : state_instances) {
				sort_values.push_back(std::make_pair(si, text::get_dynamic_state_name(state, si)));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, std::string> a, std::pair<dcon::state_instance_id, std::string> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_location_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		}
		else if(table_source->table_size_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float total_size = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id prov) {
					for(auto p : state.world.province_get_pop_location(prov)) {
						if(alice_ui::pop_passes_filter(state, p.get_pop())) {
							total_size += p.get_pop().get_size();
						}
					}
				});
				sort_values.push_back(std::make_pair(si, total_size));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_size_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		}
		else if(table_source->table_culture_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float total_size = 0.0f;
				float accepted_size = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id prov) {
					for(auto p : state.world.province_get_pop_location(prov)) {
						if(alice_ui::pop_passes_filter(state, p.get_pop())) {
							total_size += p.get_pop().get_size();
							if(p.get_pop().get_is_primary_or_accepted_culture()) {
								accepted_size += p.get_pop().get_size();
							}
						}
					}
				});
				sort_values.push_back(std::make_pair(si, total_size > 0 ? accepted_size / total_size : 0.0f));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_culture_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		}
		else if(table_source->table_religion_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float total_size = 0.0f;
				float accepted_size = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id prov) {
					for(auto p : state.world.province_get_pop_location(prov)) {
						if(alice_ui::pop_passes_filter(state, p.get_pop())) {
							total_size += p.get_pop().get_size();
							if(p.get_pop().get_religion() == state.world.nation_get_religion(state.local_player_nation)) {
								accepted_size += p.get_pop().get_size();
							}
						}
					}
				});
				sort_values.push_back(std::make_pair(si, total_size > 0 ? accepted_size / total_size : 0.0f));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_religion_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		}
		// no sorting for state-level jobs yet
		else if(table_source->table_militancy_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float total = 0.0f;
				float sz = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
					for(auto o : state.world.province_get_pop_location(p)) {
						if(alice_ui::pop_passes_filter(state, o.get_pop())) {
							sz += o.get_pop().get_size();
							total += pop_demographics::from_pmc(o.get_pop().get_umilitancy()) * o.get_pop().get_size();
						}
					}
				});
				sort_values.push_back(std::make_pair(si, sz > 0 ? total / sz : 0.0f));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_militancy_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		} else if(table_source->table_consciousness_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float total = 0.0f;
				float sz = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
					for(auto o : state.world.province_get_pop_location(p)) {
						if(alice_ui::pop_passes_filter(state, o.get_pop())) {
							sz += o.get_pop().get_size();
							total += pop_demographics::from_pmc(o.get_pop().get_uconsciousness()) * o.get_pop().get_size();
						}
					}
				});
				sort_values.push_back(std::make_pair(si, sz > 0 ? total / sz : 0.0f));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_consciousness_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		} else if(table_source->table_employment_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float total = 0.0f;
				float sz = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id p) {
					for(auto o : state.world.province_get_pop_location(p)) {
						if(alice_ui::pop_passes_filter(state, o.get_pop())) {
							if(o.get_pop().get_poptype().get_has_unemployment()) {
								sz += o.get_pop().get_size();
								total += pop_demographics::get_employment(state, o.get_pop());
							} else {
								sz += o.get_pop().get_size();
								total += o.get_pop().get_size();
							}
						}
					}
				});
				auto employment_rate = sz > 0 ? total / sz : 0.0f;
				sort_values.push_back(std::make_pair(si, employment_rate));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_employment_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		} else if(table_source->table_literacy_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float literacy = 0.0f;
				float sz = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id prov) {
					for(auto p : state.world.province_get_pop_location(prov)) {
						if(alice_ui::pop_passes_filter(state, p.get_pop())) {
							sz += p.get_pop().get_size();
							literacy += pop_demographics::get_literacy(state, p.get_pop()) * p.get_pop().get_size();
						}
					}
				});
				sort_values.push_back(std::make_pair(si, sz > 0 ? literacy / sz : 0.0f));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_literacy_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		} else if(table_source->table_money_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float cash = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id prov) {
					for(auto p : state.world.province_get_pop_location(prov)) {
						if(alice_ui::pop_passes_filter(state, p.get_pop())) {
							cash += p.get_pop().get_savings();
						}
					}

				});
				sort_values.push_back(std::make_pair(si, cash));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_money_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}
		} else if(table_source->table_needs_sort_direction != 0) {
			std::vector<std::pair<dcon::state_instance_id, float>> sort_values;
			for(auto si : state_instances) {
				float needs = 0.0f;
				province::for_each_province_in_state_instance(state, si, [&](dcon::province_id prov) {
					for(auto p : state.world.province_get_pop_location(prov)) {
						if(alice_ui::pop_passes_filter(state, p.get_pop())) {
							needs += (pop_demographics::get_life_needs(state, p.get_pop()) + pop_demographics::get_everyday_needs(state, p.get_pop()) + pop_demographics::get_luxury_needs(state, p.get_pop())) * p.get_pop().get_size();
						}
					}

				});
				sort_values.push_back(std::make_pair(si, needs));
			}
			sys::merge_sort(sort_values.begin(), sort_values.end(), [&](std::pair<dcon::state_instance_id, float> a, std::pair<dcon::state_instance_id, float> b) {
				int8_t result = alice_ui::cmp3(a.second, b.second);
				return -result == table_source->table_needs_sort_direction;
			});
			state_instances.clear();
			for(auto pair : sort_values) {
				state_instances.push_back(pair.first);
			}

		}


	} else {
		sys::merge_sort(state_instances.begin(), state_instances.end(), [&](dcon::state_instance_id a, dcon::state_instance_id b) {
			if(!state.world.state_instance_get_capital(a).get_is_colonial() && state.world.state_instance_get_capital(b).get_is_colonial()) {
				return true;
			}
			if(state.world.state_instance_get_capital(a).get_is_colonial() && !state.world.state_instance_get_capital(b).get_is_colonial()) {
				return false;
			}
			// return state.world.state_instance_get_demographics(a, demographics::total) > state.world.state_instance_get_demographics(b, demographics::total);
			return text::get_short_state_name(state, a) < text::get_short_state_name(state, b);
		});
	}
}



}
