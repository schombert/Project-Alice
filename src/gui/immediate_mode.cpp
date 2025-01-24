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

rect subrect(rect& rectangle, float w, float h, alignment_horizontal align_hor, alignment_vertical align_vert) {
	rect result{ 0.f, 0.f, w, h };

	switch(align_hor) {
	case iui::alignment_horizontal::left:
		break;
	case iui::alignment_horizontal::center:
		result.x = rectangle.x + rectangle.w / 2.f - w / 2.f;
		break;
	case iui::alignment_horizontal::right:
		result.x = rectangle.x + rectangle.w  - w;
		break;
	default:
		break;
	}

	switch(align_vert) {
	case iui::alignment_vertical::top:
		break;
	case iui::alignment_vertical::center:
		result.y = rectangle.y + rectangle.h / 2.f - h / 2.f;
		break;
	case iui::alignment_vertical::horizontal:
		result.y = rectangle.y + rectangle.h - h;
		break;
	default:
		break;
	}

	return result;
};

void iui_state::panel_colored(
	sys::state& state,
	rect& r,
	float red, float green, float blue
) {
	if(state.mouse_x_position / state.user_settings.ui_scale >= r.x && state.mouse_x_position / state.user_settings.ui_scale <= r.x + r.w) {
		if(state.mouse_y_position / state.user_settings.ui_scale >= r.y && state.mouse_y_position / state.user_settings.ui_scale <= r.y + r.h) {
			over_ui = true;
		}
	}

	ogl::render_colored_rect(
		state,
		r.x , r.y , r.w , r.h , red, green, blue,
		ui::rotation::upright, false, false
	);
}

bool iui_state::check_hover(sys::state& state, rect& r) {
	if(state.mouse_x_position / state.user_settings.ui_scale >= r.x && state.mouse_x_position / state.user_settings.ui_scale <= r.x + r.w) {
		if(state.mouse_y_position / state.user_settings.ui_scale >= r.y && state.mouse_y_position / state.user_settings.ui_scale <= r.y + r.h) {
			return true;
		}
	}
	return false;
}

void iui_state::panel(
	sys::state& state,
	rect& r
) {
	if(state.mouse_x_position / state.user_settings.ui_scale >= r.x && state.mouse_x_position / state.user_settings.ui_scale <= r.x + r.w) {
		if(state.mouse_y_position / state.user_settings.ui_scale >= r.y && state.mouse_y_position / state.user_settings.ui_scale <= r.y + r.h) {
			over_ui = true;
		}
	}

	ogl::render_colored_rect(
		state,
		r.x , r.y , r.w , r.h , 0.f, 0.f, 0.f,
		ui::rotation::upright, false, false
	);

	ogl::render_simple_rect(
		state,
		r.x + 1.f, r.y + 1.f, r.w - 2.f, r.h - 2.f,
		ui::rotation::upright, false, false
	);
}

void iui_state::panel_subsprite(
	sys::state& state,
	rect& r,
	int frame, int total_frames,
	GLuint texture_handle
) {
	if(state.mouse_x_position / state.user_settings.ui_scale >= r.x && state.mouse_x_position / state.user_settings.ui_scale <= r.x + r.w) {
		if(state.mouse_y_position / state.user_settings.ui_scale >= r.y && state.mouse_y_position / state.user_settings.ui_scale <= r.y + r.h) {
			over_ui = true;
		}
	}

	ogl::render_subsprite(
		state, ogl::color_modification::none, frame, total_frames,
		r.x , r.y , r.w , r.h ,
		texture_handle,
		ui::rotation::upright, false, false
	);
}

void iui_state::panel_textured(
	sys::state& state,
	rect& r,
	GLuint texture_handle
) {
	if(state.mouse_x_position / state.user_settings.ui_scale >= r.x && state.mouse_x_position / state.user_settings.ui_scale <= r.x + r.w) {
		if(state.mouse_y_position / state.user_settings.ui_scale >= r.y && state.mouse_y_position / state.user_settings.ui_scale <= r.y + r.h) {
			over_ui = true;
		}
	}

	ogl::render_subsprite(
		state, ogl::color_modification::none, 0, 1,
		r.x, r.y, r.w, r.h,
		texture_handle,
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

	if(!(state.mouse_x_position / state.user_settings.ui_scale > r.x && state.mouse_x_position / state.user_settings.ui_scale < r.x + r.w)) {		
		return false;
	}
	if(!(state.mouse_y_position / state.user_settings.ui_scale > r.y && state.mouse_y_position / state.user_settings.ui_scale < r.y + r.h)) {
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

// currently assumes that total frames == 3
bool iui_state::button_textured(
	sys::state& state, int32_t identifier,
	rect& r,
	int total_frames,
	GLuint texture_handle,
	bool active
) {
	bool result = state.iui_state.active_element == identifier;

	if(active) {
		panel_subsprite(state, r, 2, 3, texture_handle);
	} else {
		panel_subsprite(state, r, 0, 3, texture_handle);
	}

	if(!(state.mouse_x_position / state.user_settings.ui_scale > r.x && state.mouse_x_position / state.user_settings.ui_scale < r.x + r.w)) {
		return false;
	}
	if(!(state.mouse_y_position / state.user_settings.ui_scale > r.y && state.mouse_y_position / state.user_settings.ui_scale < r.y + r.h)) {
		return false;
	}

	panel_subsprite(state, r, 1, 3, texture_handle);
	state.iui_state.hovered_element = identifier;

	if(state.iui_state.mouse_pressed) {
		if(state.iui_state.active_element == identifier) {
			panel_subsprite(state, r, 2, 3, texture_handle);
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
	rect& r, ogl::color3f color
) {
	auto font_size = state.font_collection.line_height(state, current_font);
	auto text_height = 0.f;

	for(auto& txt : content) {
		text_height = std::max(text_height, txt.y + font_size);
	}

	for(auto& txt : content) {
		ui::render_text_chunk(
			state,
			txt,
			(r.x + txt.x) ,
			(r.y + txt.y + r.h / 2.f - text_height / 2.f) ,
			current_font,
			color,
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
			render_text(state, gui_text_chunks_map[identifier], r, { 0.f, 0.f, 0.f });
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
	render_text(state, contents.base_layout.contents, r, { 0.f, 0.f, 0.f });

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
			render_text(state, gui_text_chunks_map[identifier], r, { 0.f, 0.f, 0.f });
			return;
		}
	}

	auto layout = text::layout{};
	auto contents = init_layout(state, layout, current_font, text::alignment::center, r.w, r.h);
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(
		state,
		contents,
		box,
		text::int_wholenum{ value }
	);
	text::close_layout_box(contents, box);
	render_text(state, contents.base_layout.contents, r, { 0.f, 0.f, 0.f });

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
			render_text(state, gui_text_chunks_map[identifier], r, { 0.f, 0.f, 0.f });
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
	render_text(state, contents.base_layout.contents, r, { 0.f, 0.f, 0.f });

	gui_text_chunks_map.insert_or_assign(identifier, std::move(layout.contents));
	gui_float_map[identifier] = value;
}

void iui_state::localized_string(
	sys::state& state, int32_t identifier,
	rect& r,
	std::string_view key,
	ogl::color3f color
) {
	if(auto it = gui_text_map.find(identifier); it != gui_text_map.end()) {
		if(gui_text_map[identifier] == key) {
			render_text(state, gui_text_chunks_map[identifier], r, color);
			return;
		}
	}

	auto layout = text::layout{};
	auto contents = init_layout(state, layout, current_font, text::alignment::left, r.w, r.h);
	auto box = text::open_layout_box(contents);
	text::localised_format_box(state, contents, box, key);
	text::close_layout_box(contents, box);
	render_text(state, contents.base_layout.contents, r, color);

	gui_text_chunks_map.insert_or_assign(identifier, std::move(layout.contents));
	gui_text_map[identifier] = key;
}

void iui_state::localized_string_r(
	sys::state& state, int32_t identifier,
	rect r,
	std::string_view key,
	ogl::color3f color
) {
	iui_state::localized_string(state, identifier, r, key, color);
}


void iui_state::load_description(sys::state& state, std::string_view name, element_description& local_desc) {
	auto key = state.lookup_key(name);
	auto def = state.ui_state.defs_by_name.find(key)->second.definition;
	auto& desc = state.ui_defs.gui[def];
	local_desc.w = desc.size.x;
	local_desc.h = desc.size.y;
	auto& gfx_def = state.ui_defs.gfx[desc.data.button.button_image];
	auto handle = ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent());
	local_desc.texture_handle = handle;
}
}
