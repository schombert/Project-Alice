#include "immediate_mode.hpp"
#include "system_state.hpp"

namespace iui {

void shrink(rect& rectangle, float value) {
	rectangle.x += value;
	rectangle.y += value;
	rectangle.w -= value * 2.f;
	rectangle.h -= value * 2.f;
};

void move_to(rect& rectangle, float x, float y) {
	rectangle.x = x;
	rectangle.y = y;
};

void iui_state::panel_colored(
	sys::state& state,
	rect& r,
	float red, float green, float blue
) {
	if(state.mouse_x_position >= r.x && state.mouse_x_position <= r.x + r.w) {
		if(state.mouse_y_position >= r.y && state.mouse_y_position <= r.y + r.h) {
			over_ui = true;
		}
	}

	ogl::render_colored_rect(
		state,
		r.x, r.y, r.w, r.h, red, green, blue,
		ui::rotation::upright, false, false
	);
}

void iui_state::panel(
	sys::state& state,
	rect& r
) {
	if(state.mouse_x_position >= r.x && state.mouse_x_position <= r.x + r.w) {
		if(state.mouse_y_position >= r.y && state.mouse_y_position <= r.y + r.h) {
			over_ui = true;
		}
	}

	ogl::render_colored_rect(
		state,
		r.x, r.y, r.w, r.h, 0.f, 0.f, 0.f,
		ui::rotation::upright, false, false
	);

	ogl::render_simple_rect(
		state,
		r.x + 1.f, r.y + 1.f, r.w - 2.f, r.h - 2.f,
		ui::rotation::upright, false, false
	);
}

bool iui_state::button(
	sys::state& state, int32_t identifier,
	rect& r,
	bool active
) {
	bool result = state.iui_state.active_element == identifier;

	if(active) {
		panel_colored(state, r, 1.0f, 0.7f, 0.7f);
	} else {
		panel(state, r);
	}

	if(!(state.mouse_x_position > r.x && state.mouse_x_position < r.x + r.w)) {		
		return false;
	}
	if(!(state.mouse_y_position > r.y && state.mouse_y_position < r.y + r.h)) {
		return false;
	}

	panel_colored(state, r, 0.8f, 0.8f, 0.8f);
	state.iui_state.hovered_element = identifier;

	if(state.iui_state.mouse_pressed) {
		if(state.iui_state.active_element == identifier) {
			panel_colored(state, r, 0.9f, 0.9f, 0.9f);
			state.iui_state.mouse_pressed = false;
			return true;
		}
		state.iui_state.active_element = identifier;
	}

	return false;
}

text::endless_layout iui_state::init_layout(
	sys::state& state,
	text::layout& layout,
	uint16_t font_handle,
	text::alignment align, float w, float h
) {
	return text::create_endless_layout(
		state,
		layout,
		text::layout_parameters{
			.left = 0,
			.top = 0,
			.right = (int16_t)w,
			.bottom = (int16_t)h,
			.font_id = font_handle,
			.leading = 0,
			.align = align,
			.color = text::text_color::black
		}
	);
}

void iui_state::render_text(
	sys::state& state,
	std::vector<text::text_chunk>& content,
	float x, float y
) {
	for(auto& txt : content) {
		ui::render_text_chunk(
			state,
			txt,
			x + txt.x,
			y + txt.y,
			current_font,
			ogl::color3f{ 0.0f, 0.0f, 0.0f },
			ogl::color_modification::none
		);
	}
}

void iui_state::float_2(
	sys::state& state, int32_t identifier,
	rect& r,
	float value
) {
	if(auto it = gui_float_map.find(identifier); it != gui_float_map.end()) {
		if(gui_float_map[identifier] == value) {
			render_text(state, gui_text_chunks_map[identifier], r.x, r.y);
			return;
		}
	}

	auto layout = text::layout{};
	auto contents = init_layout(state, layout, current_font, text::alignment::right, r.w, r.h);
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(
		state,
		contents,
		box,
		text::fp_two_places{ value }
	);
	text::close_layout_box(contents, box);
	render_text(state, contents.base_layout.contents, r.x, r.y);

	gui_text_chunks_map.insert_or_assign(identifier, std::move(layout.contents));
	gui_float_map[identifier] = value;
}

void iui_state::int_whole(
	sys::state& state, int32_t identifier,
	rect& r,
	int32_t value
) {
	if(auto it = gui_int_map.find(identifier); it != gui_int_map.end()) {
		if(gui_int_map[identifier] == value) {
			render_text(state, gui_text_chunks_map[identifier], r.x, r.y);
			return;
		}
	}

	auto layout = text::layout{};
	auto contents = init_layout(state, layout, current_font, text::alignment::right, r.w, r.h);
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(
		state,
		contents,
		box,
		text::int_wholenum{ value }
	);
	text::close_layout_box(contents, box);
	render_text(state, contents.base_layout.contents, r.x, r.y);

	gui_text_chunks_map.insert_or_assign(identifier, std::move(layout.contents));
	gui_int_map[identifier] = value;
}

void iui_state::price(
	sys::state& state, int32_t identifier,
	rect& r,
	float value
) {
	if(auto it = gui_float_map.find(identifier); it != gui_float_map.end()) {
		if(gui_float_map[identifier] == value) {
			render_text(state, gui_text_chunks_map[identifier], r.x, r.y);
			return;
		}
	}

	auto layout = text::layout{};
	auto contents = init_layout(state, layout, current_font, text::alignment::right, r.w, r.h);
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(
		state,
		contents,
		box,
		text::fp_currency{ value }
	);
	text::close_layout_box(contents, box);
	render_text(state, contents.base_layout.contents, r.x, r.y);

	gui_text_chunks_map.insert_or_assign(identifier, std::move(layout.contents));
	gui_float_map[identifier] = value;
}

void iui_state::localized_string(
	sys::state& state, int32_t identifier,
	rect& r,
	std::string_view key
) {
	if(auto it = gui_text_map.find(identifier); it != gui_text_map.end()) {
		if(gui_text_map[identifier] == key) {
			render_text(state, gui_text_chunks_map[identifier], r.x, r.y);
			return;
		}
	}

	auto layout = text::layout{};
	auto contents = init_layout(state, layout, current_font, text::alignment::left, r.w, r.h);
	auto box = text::open_layout_box(contents);
	text::localised_format_box(state, contents, box, key);
	text::close_layout_box(contents, box);
	render_text(state, contents.base_layout.contents, r.x, r.y);

	gui_text_chunks_map.insert_or_assign(identifier, std::move(layout.contents));
	gui_text_map[identifier] = key;
}

void iui_state::localized_string_r(
	sys::state& state, int32_t identifier,
	rect r,
	std::string_view key
) {
	iui_state::localized_string(state, identifier, r, key);
}

}
