#include <algorithm>
#include <cmath>
#include <stddef.h>
#include <stdint.h>
#include <unordered_map>
#include <variant>
#include "color.hpp"
#include "culture.hpp"
#include "cyto_any.hpp"
#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_element_base.hpp"
#include "gui_element_types.hpp"
#include "fonts.hpp"
#include "gui_graphics.hpp"
#include "nations.hpp"
#include "opengl_wrapper.hpp"
#include "text.hpp"
#include "sound.hpp"
#include "unit_tooltip.hpp"

namespace ui {

inline message_result greater_result(message_result a, message_result b) {
	if(a == message_result::consumed || b == message_result::consumed)
		return message_result::consumed;
	if(a == message_result::seen || b == message_result::seen)
		return message_result::seen;
	return message_result::unseen;
}

mouse_probe container_base::impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept {
	for(auto& c : children) {
		if(c->is_visible()) {
			auto relative_location = child_relative_location(*this, *c);
			auto res = c->impl_probe_mouse(state, x - relative_location.x, y - relative_location.y, type);
			if(res.under_mouse)
				return res;
		}
	}
	return element_base::impl_probe_mouse(state, x, y, type);
}

message_result container_base::impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	message_result res = message_result::unseen;
	for(auto& c : children) {
		if(c->is_visible()) {
			res = greater_result(res, c->impl_on_key_down(state, key, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
	}
	return greater_result(res, element_base::impl_on_key_down(state, key, mods));
}
void container_base::impl_on_update(sys::state& state) noexcept {
	on_update(state);
	for(auto& c : children) {
		if(c->is_visible()) {
			c->impl_on_update(state);
		}
	}
}
void container_base::impl_on_reset_text(sys::state& state) noexcept {
	for(auto& c : children) {
		c->impl_on_reset_text(state);
	}
	on_reset_text(state);
}
message_result container_base::impl_set(sys::state& state, Cyto::Any& payload) noexcept {
	message_result res = message_result::unseen;
	for(auto& c : children) {
		res = greater_result(res, c->impl_set(state, payload));
	}
	return greater_result(res, set(state, payload));
}

void container_base::impl_render(sys::state& state, int32_t x, int32_t y) noexcept {
	element_base::impl_render(state, x, y);

	for(size_t i = children.size(); i-- > 0;) {
		if(children[i]->is_visible()) {
			auto relative_location = child_relative_location(*this, *(children[i]));
			children[i]->impl_render(state, x + relative_location.x, y + relative_location.y);
		}
	}
}

std::unique_ptr<element_base> container_base::remove_child(element_base* child) noexcept {
	if(auto it =
					std::find_if(children.begin(), children.end(), [child](std::unique_ptr<element_base>& p) { return p.get() == child; });
			it != children.end()) {
		if(it + 1 != children.end())
			std::rotate(it, it + 1, children.end());
		auto temp = std::move(children.back());
		children.pop_back();
		temp->parent = nullptr;
		return temp;
	}
	return std::unique_ptr<element_base>{};
}
void container_base::move_child_to_front(element_base* child) noexcept {
	if(auto it =
					std::find_if(children.begin(), children.end(), [child](std::unique_ptr<element_base>& p) { return p.get() == child; });
			it != children.end()) {
		if(it != children.begin())
			std::rotate(children.begin(), it, it + 1);
	}
}
void container_base::move_child_to_back(element_base* child) noexcept {
	if(auto it =
					std::find_if(children.begin(), children.end(), [child](std::unique_ptr<element_base>& p) { return p.get() == child; });
			it != children.end()) {
		if(it + 1 != children.end())
			std::rotate(it, it + 1, children.end());
	}
}
void container_base::add_child_to_front(std::unique_ptr<element_base> child) noexcept {
	child->parent = this;
	children.emplace_back(std::move(child));
	if(children.size() > 1) {
		std::rotate(children.begin(), children.end() - 1, children.end());
	}
}
void container_base::add_child_to_back(std::unique_ptr<element_base> child) noexcept {
	child->parent = this;
	children.emplace_back(std::move(child));
}
element_base* container_base::get_child_by_name(sys::state const& state, std::string_view name) noexcept {
	if(auto it = std::find_if(children.begin(), children.end(),
				 [&state, name](std::unique_ptr<element_base>& p) { return state.to_string_view(p->base_data.name) == name; });
			it != children.end()) {
		return it->get();
	}
	return nullptr;
}
element_base* container_base::get_child_by_index(sys::state const& state, int32_t index) noexcept {
	if(0 <= index && index < int32_t(children.size()))
		return children[index].get();
	return nullptr;
}

ogl::color_modification get_color_modification(bool is_under_mouse, bool is_disabled, bool is_interactable) {
	if(!is_under_mouse || !is_interactable) {
		if(is_disabled) {
			return ogl::color_modification::disabled;
		} else {
			return ogl::color_modification::none;
		}
	} else {
		if(is_disabled) {
			return ogl::color_modification::interactable_disabled;
		} else {
			return ogl::color_modification::interactable;
		}
	}
}

void image_element_base::on_create(sys::state& state) noexcept {
	element_base::on_create(state);
	if(base_data.get_element_type() == element_type::image) {
		frame = base_data.data.image.frame();
	}
}

void image_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid;
	if(base_data.get_element_type() == element_type::image) {
		gid = base_data.data.image.gfx_object;
	} else if(base_data.get_element_type() == element_type::button) {
		gid = base_data.data.button.button_image;
	}
	// TODO: More elements defaults?
	if(gid) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
						gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), frame,
						gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			} else {
				ogl::render_textured_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
						float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			}
		}
	}
}

void tinted_image_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid;
	if(base_data.get_element_type() == element_type::image) {
		gid = base_data.data.image.gfx_object;
	} else if(base_data.get_element_type() == element_type::button) {
		gid = base_data.data.button.button_image;
	}
	if(gid) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.number_of_frames > 1) {
				ogl::render_tinted_subsprite(state, frame,
					gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					sys::red_from_int(color), sys::green_from_int(color), sys::blue_from_int(color),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped());
			} else {
				ogl::render_tinted_textured_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					sys::red_from_int(color), sys::green_from_int(color), sys::blue_from_int(color),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped());
			}
		}
	}
}

void progress_bar::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(base_data.get_element_type() == element_type::image) {
		dcon::gfx_object_id gid = base_data.data.image.gfx_object;
		if(gid) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			auto secondary_texture_handle = dcon::texture_id(gfx_def.type_dependent - 1);
			if(gfx_def.primary_texture_handle) {
				ogl::render_progress_bar(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
						progress, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						ogl::get_texture_handle(state, secondary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			}
		}
	}
}

void button_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	image_element_base::render(state, x, y);
	if(stored_text.length() > 0) {

		auto linesz = state.font_collection.line_height(state, base_data.data.button.font_handle);
		auto ycentered = (base_data.size.y - linesz) / 2;

		ogl::render_text(state, stored_text.c_str(), uint32_t(stored_text.length()),
				get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x + int32_t(text_offset)),
				float(y + int32_t(ycentered)), black_text ? ogl::color3f{0.0f, 0.0f, 0.0f} : ogl::color3f{1.0f, 1.0f, 1.0f},
				base_data.data.button.font_handle);
	}
}

// From ui_f_shader.glsl
uint32_t internal_get_interactable_disabled_color(float r, float g, float b) {
	float amount = (r + g + b) / 4.f;
	return sys::pack_color(std::min(1.f, amount + 0.1f), std::min(1.f, amount + 0.1f), std::min(1.f, amount + 0.1f));
}
uint32_t internal_get_interactable_color(float r, float g, float b) {
	return sys::pack_color(std::min(1.f, r + 0.1f), std::min(1.f, g + 0.1f), std::min(1.f, b + 0.1f));
}
uint32_t internal_get_disabled_color(float r, float g, float b) {
	float amount = (r + g + b) / 4.f;
	return sys::pack_color(amount, amount, amount);
}

void tinted_button_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid;
	if(base_data.get_element_type() == element_type::image) {
		gid = base_data.data.image.gfx_object;
	} else if(base_data.get_element_type() == element_type::button) {
		gid = base_data.data.button.button_image;
	}
	if(gid) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			auto tcolor = color;
			float r = sys::red_from_int(color);
			float g = sys::green_from_int(color);
			float b = sys::blue_from_int(color);
			ogl::color_modification cmod = get_color_modification(this == state.ui_state.under_mouse, disabled, interactable);
			if(cmod == ogl::color_modification::interactable) {
				tcolor = internal_get_interactable_color(r, g, b);
			} else if(cmod == ogl::color_modification::interactable_disabled) {
				tcolor = internal_get_interactable_disabled_color(r, g, b);
			} else if(cmod == ogl::color_modification::disabled) {
				tcolor = internal_get_disabled_color(r, g, b);
			}

			if(gfx_def.number_of_frames > 1) {
				ogl::render_tinted_subsprite(state, frame,
					gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					sys::red_from_int(tcolor), sys::green_from_int(tcolor), sys::blue_from_int(tcolor),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped());
			} else {
				ogl::render_tinted_textured_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					sys::red_from_int(tcolor), sys::green_from_int(tcolor), sys::blue_from_int(tcolor),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped());
			}
		}
	}
	if(stored_text.length() > 0) {
		auto linesz = state.font_collection.line_height(state, base_data.data.button.font_handle);
		auto ycentered = (base_data.size.y - linesz) / 2;
		ogl::render_text(state, stored_text.c_str(), uint32_t(stored_text.length()),
				get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x + text_offset),
				float(y + ycentered), black_text ? ogl::color3f{ 0.0f, 0.0f, 0.0f } : ogl::color3f{ 1.0f, 1.0f, 1.0f },
				base_data.data.button.font_handle);
	}
}

ogl::color3f get_text_color(text::text_color text_color) {
	switch(text_color) {
	case text::text_color::black:
	case text::text_color::unspecified:
		return ogl::color3f{0.0f, 0.0f, 0.0f};

	case text::text_color::white:
		return ogl::color3f{1.0f, 1.0f, 1.0f};

	case text::text_color::red:
		return ogl::color3f{0.9f, 0.2f, 0.1f};

	case text::text_color::green:
		return ogl::color3f{0.2f, 0.95f, 0.2f};

	case text::text_color::yellow:
		return ogl::color3f{0.9f, 0.9f, 0.1f};

	case text::text_color::light_blue:
		return ogl::color3f{0.5f, 0.5f, 1.0f};

	case text::text_color::dark_blue:
		return ogl::color3f{0.2f, 0.2f, 0.8f};

	case text::text_color::orange:
		return ogl::color3f{1.f, 0.7f, 0.1f};

	case text::text_color::lilac:
		return ogl::color3f{0.8f, 0.7f, 0.3f};

	case text::text_color::light_grey:
		return ogl::color3f{0.5f, 0.5f, 0.5f};

	case text::text_color::dark_red:
		return ogl::color3f{0.5f, 0.f, 0.f};

	case text::text_color::dark_green:
		return ogl::color3f{0.f, 0.5f, 0.f};
	case text::text_color::gold:
		return ogl::color3f{232.0f / 255.0f, 210.0f / 255.0f, 124.0f / 255.0f};
	case text::text_color::brown:
		return ogl::color3f{ 150.0f / 255.0f, 75.0f / 255.0f, 0.f };
	default:
		return ogl::color3f{0.f, 0.f, 0.f};
	}
}

void button_element_base::set_button_text(sys::state& state, std::string const& new_text) {
	stored_text = new_text;
	text_offset = (base_data.size.x - state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()),
		base_data.data.button.font_handle)) / 2.0f;
}

void button_element_base::on_reset_text(sys::state& state) noexcept {
	if(stored_text.length() > 0) {
		text_offset = (base_data.size.x - state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), base_data.data.button.font_handle)) / 2.0f;
	}
}

void button_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::button) {
		auto base_text_handle = base_data.data.button.txt;
		black_text = text::is_black_from_font_id(base_data.data.button.font_handle);
		if(base_text_handle) {
			stored_text = text::produce_simple_string(state, base_text_handle);
			text_offset = (base_data.size.x - state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), base_data.data.button.font_handle)) / 2.0f;
		}
	}
}

message_result edit_box_element_base::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	// Set edit control so we get on_text events
	state.ui_state.edit_target = this;
	sound::play_interface_sound(state, sound::get_click_sound(state),
			state.user_settings.interface_volume * state.user_settings.master_volume);
	return message_result::consumed;
}

void edit_box_element_base::on_text(sys::state& state, char ch) noexcept {
	if(state.ui_state.edit_target == this && state.ui_state.edit_target->is_visible()) {
		if(ch >= 32 && ch != '`') {
			auto s = std::string(get_text(state)).insert(edit_index, 1, ch);
			edit_index++;
			set_text(state, s);
			edit_box_update(state, s);
		}
	}
}

message_result edit_box_element_base::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(state.ui_state.edit_target == this && state.ui_state.edit_target->is_visible()) {
		// Typable keys are handled by on_text callback, we only handle control keys
		auto s = std::string(get_text(state));
		switch(key) {
		case sys::virtual_key::RETURN:
			edit_box_enter(state, s);
			s.clear();
			set_text(state, s);
			edit_index = 0;
			break;
		case sys::virtual_key::ESCAPE:
			edit_box_esc(state);
			break;
		case sys::virtual_key::TAB:
			edit_box_tab(state, s);
			break;
		case sys::virtual_key::UP:
			edit_box_up(state);
			break;
		case sys::virtual_key::DOWN:
			edit_box_down(state);
			break;
		case sys::virtual_key::TILDA:
			edit_box_backtick(state);
			break;
		case sys::virtual_key::LEFT:
			edit_index = std::max<int32_t>(edit_index - 1, 0);
			break;
		case sys::virtual_key::RIGHT:
			edit_index = std::min<int32_t>(edit_index + 1, int32_t(s.length()));
			break;
		case sys::virtual_key::DELETE_KEY:
			if(edit_index < int32_t(s.length())) {
				s.erase(edit_index, 1);
				set_text(state, s);
				edit_box_update(state, s);
			}
			break;
		case sys::virtual_key::BACK:
			if(edit_index > 0 && edit_index <= int32_t(s.length())) {
				s.erase(edit_index - 1, 1);
				set_text(state, s);
				edit_index--;
				edit_box_update(state, s);
			}
			break;
		default:
			break;
		}
		return message_result::consumed;
	}
	return message_result::unseen;
}

void edit_box_element_base::on_reset_text(sys::state& state) noexcept { }

void edit_box_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::button) {
		simple_text_element_base::black_text = text::is_black_from_font_id(base_data.data.button.font_handle);
		simple_text_element_base::text_offset = 0.0f;
	} else if(base_data.get_element_type() == element_type::text) {
		;
		simple_text_element_base::black_text = text::is_black_from_font_id(base_data.data.text.font_handle);
		simple_text_element_base::text_offset = base_data.data.text.border_size.x;
	}
}

void edit_box_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(base_data.get_element_type() == element_type::text) {
		dcon::texture_id background_texture_id;
		if((base_data.data.text.flags & uint8_t(ui::text_background::tiles_dialog)) != 0) {
			background_texture_id = definitions::tiles_dialog;
		} else if((base_data.data.text.flags & uint8_t(ui::text_background::transparency)) != 0) {
			background_texture_id = definitions::transparency;
		} else if((base_data.data.text.flags & uint8_t(ui::text_background::small_tiles_dialog)) != 0) {
			background_texture_id = definitions::small_tiles_dialog;
		}
		// TODO: Partial transparency is ignored, might cause issues with "transparency"?
		// is-vertically-flipped is also ignored, also the border size **might** be
		// variable/stored somewhere, but I don't know where?
		if(bool(background_texture_id)) {
			ogl::render_bordered_rect(state, ogl::color_modification::none, 16.0f, float(x), float(y), float(base_data.size.x),
					float(base_data.size.y), ogl::get_texture_handle(state, background_texture_id, true), base_data.get_rotation(), false);
		}
	}

	// TODO: A better way to show the cursor!
	auto old_s = std::string(get_text(state));
	auto blink_s = std::string(get_text(state));
	blink_s.insert(size_t(edit_index), 1, '|');
	set_text(state, blink_s);
	simple_text_element_base::render(state, x, y);
	set_text(state, old_s);
}

void tool_tip::render(sys::state& state, int32_t x, int32_t y) noexcept {
	ogl::render_bordered_rect(state, ogl::color_modification::none, 16.0f, float(x), float(y), float(base_data.size.x),
			float(base_data.size.y), ogl::get_texture_handle(state, definitions::tiles_dialog, true), ui::rotation::upright, false);
	auto black_text = text::is_black_from_font_id(state.ui_state.tooltip_font);
	for(auto& t : internal_layout.contents) {
		ogl::render_text(state, t.win1250chars.c_str(), uint32_t(t.win1250chars.length()), ogl::color_modification::none,
				float(x) + t.x, float(y + t.y), get_text_color(t.color), state.ui_state.tooltip_font);
	}
}

void line_graph::set_data_points(sys::state& state, std::vector<float> const& datapoints) noexcept {
	assert(datapoints.size() == count);
	float min = *std::min_element(datapoints.begin(), datapoints.end());
	float max = *std::max_element(datapoints.begin(), datapoints.end());
	float y_height = max - min;
	std::vector<float> scaled_datapoints = std::vector<float>(count);
	if(y_height == 0.f) {
		for(size_t i = 0; i < count; i++) {
			scaled_datapoints[i] = .5f;
		}
	} else {
		for(size_t i = 0; i < count; i++) {
			scaled_datapoints[i] = (datapoints[i] - min) / y_height;
		}
	}


	lines.set_y(scaled_datapoints.data());
}

void line_graph::set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max) noexcept {
	assert(datapoints.size() == count);
	float y_height = max - min;
	std::vector<float> scaled_datapoints = std::vector<float>(count);
	if(y_height == 0.f) {
		for(size_t i = 0; i < count; i++) {
			scaled_datapoints[i] = .5f;
		}
	} else {
		for(size_t i = 0; i < count; i++) {
			scaled_datapoints[i] = (datapoints[i] - min) / y_height;
		}
	}


	lines.set_y(scaled_datapoints.data());
}

void line_graph::on_create(sys::state& state) noexcept {
	element_base::on_create(state);
}

void line_graph::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(!is_coloured) {
		ogl::render_linegraph(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, lines);
	} else {
		ogl::render_linegraph(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, r, g, b, lines);
	}	
}

void simple_text_element_base::set_text(sys::state& state, std::string const& new_text) {
	stored_text = new_text;
	on_reset_text(state);
}

void simple_text_element_base::on_reset_text(sys::state& state) noexcept {
	if(stored_text.length() == 0)
		return;
	float extent = 0.f;
	uint16_t font_handle = 0;
	if(base_data.get_element_type() == element_type::button)
		font_handle = base_data.data.button.font_handle;
	else if(base_data.get_element_type() == element_type::text)
		font_handle = base_data.data.text.font_handle;

	extent = state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), font_handle);

	if(stored_text.back() != '\x85' && int16_t(extent) > base_data.size.x) {
		auto width_of_ellipsis = 0.5f * state.font_collection.text_extent(state, "\x85", uint32_t(1), font_handle);

		uint32_t m = 1;
		for(; m < stored_text.length(); ++m) {
			if(state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(m), font_handle) + width_of_ellipsis > base_data.size.x)
				break;
		}

		stored_text = stored_text.substr(0, m - 1) + "\x85";
	}
	if(base_data.get_element_type() == element_type::button) {
		switch(base_data.data.button.get_alignment()) {
		case alignment::centered:
		case alignment::justified:
			text_offset = (base_data.size.x - state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), font_handle)) / 2.0f;
			break;
		case alignment::right:
			text_offset = (base_data.size.x - state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), font_handle));
			break;
		case alignment::left:
			text_offset = 0.0f;
			break;
		}
	} else if(base_data.get_element_type() == element_type::text) {
		switch(base_data.data.button.get_alignment()) {
		case alignment::centered:
		case alignment::justified:
			text_offset = (base_data.size.x - state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), font_handle) - base_data.data.text.border_size.x) / 2.0f;
			break;
		case alignment::right:
			text_offset = (base_data.size.x - state.font_collection.text_extent(state, stored_text.c_str(), uint32_t(stored_text.length()), font_handle) -  base_data.data.text.border_size.x);
			break;
		case alignment::left:
			text_offset = base_data.data.text.border_size.x;
			break;
		}
	}
}
void simple_text_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::button) {
		set_text(state, text::produce_simple_string(state, base_data.data.button.txt));
		black_text = text::is_black_from_font_id(base_data.data.button.font_handle);
	} else if(base_data.get_element_type() == element_type::text) {
		set_text(state, text::produce_simple_string(state, base_data.data.text.txt));
		black_text = text::is_black_from_font_id(base_data.data.text.font_handle);
	}
}
void simple_text_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(stored_text.length() > 0) {
		if(base_data.get_element_type() == element_type::text) {
			// auto linesz = state.font_collection.fonts[font_id - 1].line_height(font_size);
			// auto ycentered = (base_data.size.y - base_data.data.text.border_size.y - linesz) / 2;
			// ycentered = std::max(ycentered + state.font_collection.fonts[font_id - 1].top_adjustment(font_size),
			// float(base_data.data.text.border_size.y));
			ogl::render_text(state, stored_text.c_str(), uint32_t(stored_text.length()), ogl::color_modification::none,
					float(x + int32_t(text_offset)), float(y + base_data.data.text.border_size.y),
					black_text ? ogl::color3f{0.0f, 0.0f, 0.0f} : ogl::color3f{1.0f, 1.0f, 1.0f}, base_data.data.button.font_handle);
		} else {
			auto linesz = state.font_collection.line_height(state, base_data.data.button.font_handle);
			auto ycentered = (base_data.size.y - linesz) / 2;

			ogl::render_text(state, stored_text.c_str(), uint32_t(stored_text.length()), ogl::color_modification::none,
					float(x + int32_t(text_offset)), float(y + ycentered),
					black_text ? ogl::color3f{0.0f, 0.0f, 0.0f} : ogl::color3f{1.0f, 1.0f, 1.0f}, base_data.data.text.font_handle);
		}
	}
}

void simple_body_text::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::button) {
		set_text(state, text::produce_simple_string(state, base_data.data.button.txt));
		black_text = text::is_black_from_font_id(base_data.data.button.font_handle);
	} else if(base_data.get_element_type() == element_type::text) {
		set_text(state, text::produce_simple_string(state, base_data.data.text.txt));
		black_text = text::is_black_from_font_id(base_data.data.text.font_handle);
	}
}
void simple_body_text::set_text(sys::state& state, std::string const& new_text) {
	if(!state.user_settings.use_classic_fonts) {
		auto old_handle = base_data.data.text_common.font_handle;
		base_data.data.text_common.font_handle &= ~(0x01 << 7);
		auto old_value = base_data.data.text_common.font_handle & 0x3F;
		base_data.data.text_common.font_handle &= ~(0x003F);
		base_data.data.text_common.font_handle |= (old_value - 2);

		simple_text_element_base::set_text(state, new_text);
		base_data.data.text_common.font_handle = old_handle;
	} else {
		simple_text_element_base::set_text(state, new_text);
	}
}
void simple_body_text::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(!state.user_settings.use_classic_fonts) {
		auto old_handle = base_data.data.text_common.font_handle;
		base_data.data.text_common.font_handle &= ~(0x01 << 7);
		auto old_value = base_data.data.text_common.font_handle & 0x3F;
		base_data.data.text_common.font_handle &= ~(0x003F);
		base_data.data.text_common.font_handle |= (old_value - 2);

		simple_text_element_base::render(state, x, y);
		base_data.data.text_common.font_handle = old_handle;
	} else {
		simple_text_element_base::render(state, x, y);
	}
}
void simple_body_text::on_reset_text(sys::state& state) noexcept {
	if(!state.user_settings.use_classic_fonts) {
		auto old_handle = base_data.data.text_common.font_handle;
		base_data.data.text_common.font_handle &= ~(0x01 << 7);
		auto old_value = base_data.data.text_common.font_handle & 0x3F;
		base_data.data.text_common.font_handle &= ~(0x003F);
		base_data.data.text_common.font_handle |= (old_value - 2);

		simple_text_element_base::on_reset_text(state);
		base_data.data.text_common.font_handle = old_handle;
	} else {
		simple_text_element_base::on_reset_text(state);
	}
}

void color_text_element::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(stored_text.length() > 0) {
		if(base_data.get_element_type() == element_type::text) {
			ogl::render_text(state, stored_text.c_str(), uint32_t(stored_text.length()), ogl::color_modification::none,
				float(x + text_offset), float(y + base_data.data.text.border_size.y), get_text_color(color), base_data.data.button.font_handle);
		} else {
			auto linesz = state.font_collection.line_height(state, base_data.data.button.font_handle);
			auto ycentered = (base_data.size.y - linesz) / 2;

			ogl::render_text(state, stored_text.c_str(), uint32_t(stored_text.length()), ogl::color_modification::none,
				float(x + text_offset), float(y + ycentered), get_text_color(color), base_data.data.text.font_handle);
		}
	}
}

void multiline_text_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::text) {
		black_text = text::is_black_from_font_id(base_data.data.text.font_handle);
		line_height = state.font_collection.line_height(state, base_data.data.text.font_handle);
		visible_lines = base_data.size.y / int32_t(line_height);
	}
}

void multiline_text_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(base_data.get_element_type() == element_type::text) {
		for(auto& t : internal_layout.contents) {
			float line_offset = t.y - line_height * float(current_line);
			if(0 <= line_offset && line_offset < base_data.size.y) {
				ogl::render_text(state, t.win1250chars.c_str(), uint32_t(t.win1250chars.length()), ogl::color_modification::none,
						float(x) + t.x, float(y + line_offset), get_text_color(t.color), base_data.data.text.font_handle);
			}
		}
	}
}

message_result multiline_text_element_base::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto const* chunk = internal_layout.get_chunk_from_position(x, y + int32_t(line_height * float(current_line)));
	if(chunk != nullptr) {
		if(std::holds_alternative<dcon::nation_id>(chunk->source)) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			state.open_diplomacy(std::get<dcon::nation_id>(chunk->source));
			auto cap = state.world.nation_get_capital(std::get<dcon::nation_id>(chunk->source));
			if(cap) {
				auto map_pos = state.world.province_get_mid_point(cap);
				map_pos.x /= float(state.map_state.map_data.size_x);
				map_pos.y /= float(state.map_state.map_data.size_y);
				map_pos.y = 1.0f - map_pos.y;
				state.map_state.set_pos(map_pos);
			}
		} else if(std::holds_alternative<dcon::province_id>(chunk->source)) {
			auto prov = std::get<dcon::province_id>(chunk->source);
			if(prov) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				state.map_state.set_selected_province(prov);
				static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov);

				if(state.map_state.get_zoom() < 8)
					state.map_state.zoom = 8.0f;

				auto map_pos = state.world.province_get_mid_point(prov);
				map_pos.x /= float(state.map_state.map_data.size_x);
				map_pos.y /= float(state.map_state.map_data.size_y);
				map_pos.y = 1.0f - map_pos.y;
				state.map_state.set_pos(map_pos);
			}
		} else if(std::holds_alternative<dcon::state_instance_id>(chunk->source)) {
			auto s = std::get<dcon::state_instance_id>(chunk->source);
			auto prov = state.world.state_instance_get_capital(s);
			if(prov) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				state.map_state.set_selected_province(prov);
				static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov);

				if(state.map_state.get_zoom() < 8)
					state.map_state.zoom = 8.0f;

				auto map_pos = state.world.province_get_mid_point(prov);
				map_pos.x /= float(state.map_state.map_data.size_x);
				map_pos.y /= float(state.map_state.map_data.size_y);
				map_pos.y = 1.0f - map_pos.y;
				state.map_state.set_pos(map_pos);
			}
		} else if(std::holds_alternative<dcon::national_identity_id>(chunk->source)) {
			auto id = std::get<dcon::national_identity_id>(chunk->source);
			auto nat = state.world.national_identity_get_nation_from_identity_holder(id);
			if(nat) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				state.open_diplomacy(nat);
				auto cap = state.world.nation_get_capital(nat);
				if(cap) {
					auto map_pos = state.world.province_get_mid_point(cap);
					map_pos.x /= float(state.map_state.map_data.size_x);
					map_pos.y /= float(state.map_state.map_data.size_y);
					map_pos.y = 1.0f - map_pos.y;
					state.map_state.set_pos(map_pos);
				}
			}
		} else if(std::holds_alternative<dcon::state_definition_id>(chunk->source)) {
			auto s = std::get<dcon::state_definition_id>(chunk->source);
			auto prov_rng = state.world.state_definition_get_abstract_state_membership(s);
			dcon::province_id prov = prov_rng.begin() != prov_rng.end() ? (*prov_rng.begin()).get_province().id : dcon::province_id{ };
			if(prov) {
				sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
				state.map_state.set_selected_province(prov);
				static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, prov);

				if(state.map_state.get_zoom() < 8)
					state.map_state.zoom = 8.0f;

				auto map_pos = state.world.province_get_mid_point(prov);
				map_pos.x /= float(state.map_state.map_data.size_x);
				map_pos.y /= float(state.map_state.map_data.size_y);
				map_pos.y = 1.0f - map_pos.y;
				state.map_state.set_pos(map_pos);
			}
		}
		return message_result::consumed;
	}
	return message_result::unseen;
}

message_result multiline_text_element_base::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::consumed;
}

message_result multiline_text_element_base::test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept {
	switch(type) {
	case mouse_probe_type::click:
	{
		auto chunk = internal_layout.get_chunk_from_position(x, y + int32_t(line_height * float(current_line)));
		if(!chunk)
			return message_result::unseen;
		if(std::holds_alternative<dcon::nation_id>(chunk->source)
			|| std::holds_alternative<dcon::province_id>(chunk->source)
			|| std::holds_alternative<dcon::state_instance_id>(chunk->source)
			|| std::holds_alternative<dcon::national_identity_id>(chunk->source)
			|| std::holds_alternative<dcon::state_definition_id>(chunk->source)) {

			return message_result::consumed;
		}
		return message_result::unseen;
	}
	case mouse_probe_type::tooltip:
		return message_result::unseen;
	case mouse_probe_type::scroll:
		return message_result::unseen;
	}
	return message_result::unseen;
}

void multiline_button_element_base::on_create(sys::state& state) noexcept {
	button_element_base::on_create(state);
	if(base_data.get_element_type() == element_type::button) {
		black_text = text::is_black_from_font_id(base_data.data.button.font_handle);
		line_height = state.font_collection.line_height(state, base_data.data.button.font_handle);
		visible_lines = base_data.size.y / int32_t(line_height);
	}
	set_button_text(state, "");
}

void multiline_button_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	button_element_base::render(state, x, y);
	if(base_data.get_element_type() == element_type::button) {
		for(auto& t : internal_layout.contents) {
			float line_offset = t.y - line_height * float(current_line);
			if(0 <= line_offset && line_offset < base_data.size.y) {
				ogl::render_text(state, t.win1250chars.c_str(), uint32_t(t.win1250chars.length()), ogl::color_modification::none,
						float(x) + t.x, float(y + line_offset), get_text_color(t.color), base_data.data.button.font_handle);
			}
		}
	}
}

void make_size_from_graphics(sys::state& state, ui::element_data& dat) {
	if(dat.size.x == 0 || dat.size.y == 0) {
		dcon::gfx_object_id gfx_handle;
		float scale = 1.0f;
		if(dat.get_element_type() == ui::element_type::image) {
			gfx_handle = dat.data.image.gfx_object;
			scale = dat.data.image.scale;
		} else if(dat.get_element_type() == ui::element_type::button) {
			gfx_handle = dat.data.button.button_image;
		}
		if(gfx_handle) {
			if(state.ui_defs.gfx[gfx_handle].size.x != 0) {
				dat.size = state.ui_defs.gfx[gfx_handle].size;
			} else {
				auto tex_handle = state.ui_defs.gfx[gfx_handle].primary_texture_handle;
				if(tex_handle) {
					ogl::get_texture_handle(state, tex_handle, state.ui_defs.gfx[gfx_handle].is_partially_transparent());
					dat.size.y = int16_t(state.open_gl.asset_textures[tex_handle].size_y);
					dat.size.x = int16_t(state.open_gl.asset_textures[tex_handle].size_x / state.ui_defs.gfx[gfx_handle].number_of_frames);
				}
			}
			if(scale != 1.0f) {
				dat.size.x = int16_t(dat.size.x * dat.data.image.scale);
				dat.size.y = int16_t(dat.size.y * dat.data.image.scale);
			}
		}
	}
}

std::unique_ptr<element_base> make_element(sys::state& state, std::string_view name) {
	auto it = state.ui_state.defs_by_name.find(name);
	if(it != state.ui_state.defs_by_name.end()) {
		if(it->second.generator) {
			auto res = it->second.generator(state, it->second.definition);
			if(res) {
				std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.definition]), sizeof(ui::element_data));
				make_size_from_graphics(state, res->base_data);
				res->on_create(state);
				return res;
			}
		}
		return make_element_immediate(state, it->second.definition);
	}
	return std::unique_ptr<element_base>{};
}

state::state() {
	units_root = std::make_unique<container_base>();
	rgos_root = std::make_unique<container_base>();
	province_details_root = std::make_unique<container_base>();
	root = std::make_unique<container_base>();
	tooltip = std::make_unique<tool_tip>();
	tooltip->flags |= element_base::is_invisible_mask;
}

state::~state() = default;

void window_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::window) {
		auto first_child = base_data.data.window.first_child;
		auto num_children = base_data.data.window.num_children;
		for(uint32_t i = num_children; i-- > 0;) {
			auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i + first_child.index()));
			auto ch_res = make_child(state, state.to_string_view(state.ui_defs.gui[child_tag].name), child_tag);
			if(!ch_res) {
				ch_res = ui::make_element_immediate(state, child_tag);
			}
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
			}
		}
	}
}

void window_element_base::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y,
		sys::key_modifiers mods) noexcept {
	auto location_abs = get_absolute_location(*this);
	if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy &&
			oldy < base_data.size.y + location_abs.y) {
		xy_pair new_abs_pos = location_abs;
		new_abs_pos.x += int16_t(x - oldx);
		new_abs_pos.y += int16_t(y - oldy);

		if(ui_width(state) > base_data.size.x)
			new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui_width(state) - base_data.size.x));
		if(ui_height(state) > base_data.size.y)
			new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui_height(state) - base_data.size.y));

		base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
	}
}

template<class T>
void piechart<T>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(distribution.size() > 0)
		ogl::render_piechart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), data_texture);
}

template<class T>
void piechart<T>::on_create(sys::state& state) noexcept {
	base_data.position.x -= base_data.size.x;
	radius = float(base_data.size.x);
	base_data.size.x *= 2;
	base_data.size.y *= 2;
}

template<class T>
void piechart<T>::update_chart(sys::state& state) {
	std::sort(distribution.begin(), distribution.end(), [](auto const& a, auto const& b) { return a.value > b.value; });
	float total = 0.0f;
	for(auto& e : distribution) {
		total += e.value;
	}
	int32_t int_total = 0;

	if(total != 0.0f) {
		for(auto& e : distribution) {
			auto ivalue = int32_t(e.value * float(resolution) / total);
			e.slices = uint8_t(ivalue);
			e.value /= total;
			int_total += ivalue;
		}
	} else {
		distribution.clear();
	}

	if(int_total < resolution && distribution.size() > 0) {
		auto rem = resolution - int_total;
		while(rem > 0) {
			for(auto& e : distribution) {
				e.slices += uint8_t(1);
				rem -= 1;
				if(rem == 0)
					break;
			}
		}
	} else if(int_total > resolution) {
		assert(false);
	}

	size_t i = 0;
	for(auto& e : distribution) {
		uint32_t color = ogl::get_ui_color<T>(state, e.key);
		auto slice_count = size_t(e.slices);

		for(size_t j = 0; j < slice_count; j++) {
			data_texture.data[(i + j) * channels] = uint8_t(color & 0xFF);
			data_texture.data[(i + j) * channels + 1] = uint8_t(color >> 8 & 0xFF);
			data_texture.data[(i + j) * channels + 2] = uint8_t(color >> 16 & 0xFF);
		}

		i += slice_count;
	}
	for(; i < resolution; i++) {
		data_texture.data[i * channels] = uint8_t(0);
		data_texture.data[i * channels + 1] = uint8_t(0);
		data_texture.data[i * channels + 2] = uint8_t(0);
	}

	data_texture.data_updated = true;
}

template<class T>
void piechart<T>::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	float const PI = 3.141592653589793238463f;
	float dx = float(x) - radius;
	float dy = float(y) - radius;
	size_t index = 0;
	if(dx != 0.0f || dy != 0.0f) {
		float dist = std::sqrt(dx * dx + dy * dy);
		float angle = std::acos(-dx / dist);
		if(dy > 0.f) {
			angle = PI + (PI - angle);
		}
		index = size_t(angle / (2.f * PI) * float(resolution));
	}
	for(auto const& e : distribution) {
		if(index < size_t(e.slices)) {
			populate_tooltip(state, e.key, e.value, contents);
			return;
		}
		index -= size_t(e.slices);
	}
}

template<class T>
void piechart<T>::populate_tooltip(sys::state& state, T t, float percentage, text::columnar_layout& contents) noexcept {
	auto fat_t = dcon::fatten(state.world, t);
	auto box = text::open_layout_box(contents, 0);
	text::add_to_layout_box(state, contents, box, fat_t.get_name(), text::substitution_map{});
	text::add_to_layout_box(state, contents, box, std::string(":"), text::text_color::white);
	text::add_space_to_layout_box(state, contents, box);
	text::add_to_layout_box(state, contents, box, text::format_percentage(percentage, 1), text::text_color::white);
	text::close_layout_box(contents, box);
}

template<class SrcT, class DemoT>
void demographic_piechart<SrcT, DemoT>::on_update(sys::state& state) noexcept {
	this->distribution.clear();

	Cyto::Any obj_id_payload = SrcT{};
	size_t i = 0;
	if(this->parent) {
		this->parent->impl_get(state, obj_id_payload);
		float total_pops = 0.f;

		for_each_demo(state, [&](DemoT demo_id) {
			float volume = 0.f;
			if(obj_id_payload.holds_type<dcon::province_id>()) {
				auto demo_key = demographics::to_key(state, demo_id);
				auto prov_id = any_cast<dcon::province_id>(obj_id_payload);
				volume = state.world.province_get_demographics(prov_id, demo_key);
			} else if(obj_id_payload.holds_type<dcon::nation_id>()) {
				auto demo_key = demographics::to_key(state, demo_id);
				auto nat_id = any_cast<dcon::nation_id>(obj_id_payload);
				volume = state.world.nation_get_demographics(nat_id, demo_key);
			}

			if constexpr(std::is_same_v<SrcT, dcon::pop_id>) {
				if(obj_id_payload.holds_type<dcon::pop_id>()) {
					auto demo_key = pop_demographics::to_key(state, demo_id);
					auto pop_id = any_cast<dcon::pop_id>(obj_id_payload);
					volume = state.world.pop_get_demographics(pop_id, demo_key);
				}
			}
			if(volume > 0)
				this->distribution.emplace_back(demo_id, volume);
		});
	}

	this->update_chart(state);
}

void autoscaling_scrollbar::on_create(sys::state& state) noexcept {
	base_data.data.scrollbar.flags &= ~ui::scrollbar_data::step_size_mask;
	scrollbar::on_create(state);
}

void autoscaling_scrollbar::scale_to_parent() {
	base_data.size.y = parent->base_data.size.y;
	base_data.data.scrollbar.border_size = base_data.size;
	base_data.position.x = parent->base_data.size.x; // base_data.size.x / 3;

	left->base_data.position.y = parent->base_data.size.y - left->base_data.size.y;
	right->base_data.position.y = 0;
	track->base_data.size.y = parent->base_data.size.y - 2 * right->base_data.size.y;
	track->base_data.position.y = right->base_data.size.y;
	track->base_data.position.x = 5;
	slider->base_data.position.x = 0;
	settings.track_size = track->base_data.size.y;

	left->step_size = -settings.scaling_factor;
	right->step_size = -settings.scaling_factor;
}

void multiline_text_scrollbar::on_value_change(sys::state& state, int32_t v) noexcept {
	Cyto::Any payload = multiline_text_scroll_event{int32_t(scaled_value())};
	impl_get(state, payload);
}

void scrollable_text::on_create(sys::state& state) noexcept {
	auto res = std::make_unique<multiline_text_element_base>();
	std::memcpy(&(res->base_data), &(base_data), sizeof(ui::element_data));
	make_size_from_graphics(state, res->base_data);
	res->base_data.position.x = 0;
	res->base_data.position.y = 0;
	res->on_create(state);
	delegate = res.get();
	delegate->base_data.flags &= ~uint8_t(ui::element_data::orientation_mask);
	add_child_to_front(std::move(res));

	auto ptr = make_element_by_type<multiline_text_scrollbar>(state, "standardlistbox_slider");
	text_scrollbar = static_cast<multiline_text_scrollbar*>(ptr.get());
	add_child_to_front(std::move(ptr));
	text_scrollbar->scale_to_parent();
}

void scrollable_text::calibrate_scrollbar(sys::state& state) noexcept {
	if(delegate->internal_layout.number_of_lines > delegate->visible_lines) {
		text_scrollbar->set_visible(state, true);
		text_scrollbar->change_settings(state,
				mutable_scrollbar_settings{0, delegate->internal_layout.number_of_lines - delegate->visible_lines, 0, 0, false});
	} else {
		text_scrollbar->set_visible(state, false);
		delegate->current_line = 0;
	}
}

message_result scrollable_text::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(delegate->internal_layout.number_of_lines > delegate->visible_lines) {
		text_scrollbar->update_scaled_value(state, text_scrollbar->scaled_value() + std::clamp(-amount, -1.f, 1.f));
		delegate->current_line = int32_t(text_scrollbar->scaled_value());
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

message_result scrollable_text::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<multiline_text_scroll_event>()) {
		auto event = any_cast<multiline_text_scroll_event>(payload);
		delegate->current_line = event.new_value;
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

template<class RowWinT, class RowConT>
void standard_listbox_scrollbar<RowWinT, RowConT>::on_value_change(sys::state& state, int32_t v) noexcept {
	static_cast<listbox_element_base<RowWinT, RowConT>*>(parent)->update(state);
}

void listbox2_scrollbar::on_value_change(sys::state& state, int32_t v) noexcept {
	send(state, parent, listbox2_scroll_event{ });
}

message_result listbox2_row_element::get(sys::state& state, Cyto::Any& payload) noexcept {
	send(state, parent, listbox2_row_view{ this });
	return message_result::unseen;
}


template<class RowConT>
message_result listbox_row_element_base<RowConT>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<RowConT>()) {
		payload.emplace<RowConT>(content);
		return message_result::consumed;
	} else if(payload.holds_type<wrapped_listbox_row_content<RowConT>>()) {
		content = any_cast<wrapped_listbox_row_content<RowConT>>(payload).content;
		impl_on_update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowConT>
message_result listbox_row_button_base<RowConT>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<RowConT>()) {
		payload.emplace<RowConT>(content);
		return message_result::consumed;
	} else if(payload.holds_type<wrapped_listbox_row_content<RowConT>>()) {
		content = any_cast<wrapped_listbox_row_content<RowConT>>(payload).content;
		update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::update(sys::state& state) {
	auto content_off_screen = int32_t(row_contents.size() - row_windows.size());
	int32_t scroll_pos = list_scrollbar->raw_value();
	if(content_off_screen <= 0) {
		list_scrollbar->set_visible(state, false);
		scroll_pos = 0;
	} else {
		list_scrollbar->change_settings(state, mutable_scrollbar_settings{0, content_off_screen, 0, 0, false});
		list_scrollbar->set_visible(state, true);
		scroll_pos = std::min(scroll_pos, content_off_screen);
	}

	if(is_reversed()) {
		auto i = int32_t(row_contents.size()) - scroll_pos - 1;
		for(int32_t rw_i = int32_t(row_windows.size()) - 1; rw_i >= 0; rw_i--) {
			RowWinT* row_window = row_windows[size_t(rw_i)];
			if(i >= 0) {
				row_window->set_visible(state, true);
				auto prior_content = retrieve<RowConT>(state, row_window);
				auto new_content = row_contents[i--];

				if(prior_content != new_content) {
					send(state, row_window, wrapped_listbox_row_content<RowConT>{ new_content });
					if(!row_window->is_visible()) {
						row_window->set_visible(state, true);
					} else {
						row_window->impl_on_update(state);
					}
				} else {
					row_window->set_visible(state, true);
				}
			} else {
				row_window->set_visible(state, false);
			}
		}
	} else {
		auto i = size_t(scroll_pos);
		for(RowWinT* row_window : row_windows) {
			if(i < row_contents.size()) {
				auto prior_content = retrieve<RowConT>(state, row_window);
				auto new_content = row_contents[i++];

				if(prior_content != new_content) {
					send(state, row_window, wrapped_listbox_row_content<RowConT>{ new_content });
					if(!row_window->is_visible()) {
						row_window->set_visible(state, true);
					} else {
						row_window->impl_on_update(state);
					}
				} else {
					row_window->set_visible(state, true);
				}
			} else {
				row_window->set_visible(state, false);
			}
		}
	}
}

template<class RowWinT, class RowConT>
message_result listbox_element_base<RowWinT, RowConT>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(row_contents.size() > row_windows.size()) {
		amount = is_reversed() ? -amount : amount;
		list_scrollbar->update_raw_value(state, list_scrollbar->raw_value() + (amount < 0 ? 1 : -1));
		update(state);
	}
	return message_result::consumed;
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::scroll_to_bottom(sys::state& state) {
	uint32_t list_size = 0;
	for(auto rc : row_contents) {
		list_size++;
	}
	list_scrollbar->update_raw_value(state, list_size);
	update(state);
}

template<typename contents_type>
message_result listbox2_base<contents_type>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(int32_t(row_contents.size()) > visible_row_count) {
		//amount = is_reversed() ? -amount : amount;
		list_scrollbar->update_raw_value(state, list_scrollbar->raw_value() + (amount < 0 ? 1 : -1));
		impl_on_update(state);
	}
	return message_result::consumed;
}

template<typename contents_type>
void listbox2_base<contents_type>::on_update(sys::state& state) noexcept {
	auto content_off_screen = int32_t(row_contents.size()) - visible_row_count;
	int32_t scroll_pos = list_scrollbar->raw_value();

	if(content_off_screen <= 0) {
		list_scrollbar->set_visible(state, false);
		list_scrollbar->update_raw_value(state, 0);

		int32_t i = 0;
		for(; i < int32_t(row_contents.size()); ++i) {
			row_windows[i]->set_visible(state, true);
		}
		for(; i < int32_t(row_windows.size()); ++i) {
			row_windows[i]->set_visible(state, false);
		}
	} else {
		list_scrollbar->change_settings(state, mutable_scrollbar_settings{ 0, content_off_screen, 0, 0, false });
		list_scrollbar->set_visible(state, true);
		scroll_pos = std::min(scroll_pos, content_off_screen);

		int32_t i = 0;
		for(; i < visible_row_count; ++i) {
			row_windows[i]->set_visible(state, true);
		}
		for(; i < int32_t(row_windows.size()); ++i) {
			row_windows[i]->set_visible(state, false);
		}
	}
}

template<typename contents_type>
message_result listbox2_base<contents_type>::get(sys::state& state, Cyto::Any& payload) noexcept  {
	if(payload.holds_type<listbox2_scroll_event>()) {
		impl_on_update(state);
		return message_result::consumed;
	} else if(payload.holds_type<listbox2_row_view>()) {
		listbox2_row_view ptr = any_cast<listbox2_row_view>(payload);
		for(int32_t index = 0; index < int32_t(row_windows.size()); ++index) {
			if(row_windows[index] == ptr.row) {
				stored_index = index + list_scrollbar->raw_value();
				return message_result::consumed;
			}
		}
		stored_index = -1;
		return message_result::consumed;
	} else if(payload.holds_type<contents_type>()) {
		if(0 <= stored_index && stored_index < int32_t(row_contents.size())) {
			payload = row_contents[stored_index];
		}
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

template<typename contents_type>
void listbox2_base<contents_type>::resize(sys::state& state, int32_t height) {
	int32_t row_height = row_windows[0]->base_data.position.y + row_windows[0]->base_data.size.y;
	int32_t height_covered = int32_t(row_windows.size()) * row_height;
	int32_t required_rows = (height - height_covered) / row_height;

	while(required_rows > 0) {
		auto new_row = make_row(state);
		row_windows.push_back(new_row.get());
		new_row->base_data.position.y += int16_t(height_covered);
		add_child_to_back(std::move(new_row));

		height_covered += row_height;
		--required_rows;
	}

	visible_row_count = height / row_height;
	base_data.size.y = int16_t(row_height * visible_row_count);

	if(visible_row_count != 0) {
		if(scrollbar_is_internal)
			base_data.size.x -= 16;
		list_scrollbar->scale_to_parent();
		if(scrollbar_is_internal)
			base_data.size.x += 16;
	}
}

template<typename contents_type>
void listbox2_base<contents_type>::on_create(sys::state& state) noexcept {
	auto ptr = make_element_by_type<listbox2_scrollbar>(state, "standardlistbox_slider");
	list_scrollbar = static_cast<listbox2_scrollbar*>(ptr.get());
	add_child_to_back(std::move(ptr));

	auto base_row = make_row(state);
	row_windows.push_back(base_row.get());
	add_child_to_back(std::move(base_row));

	resize(state, base_data.size.y);
}
template<typename contents_type>
void listbox2_base<contents_type>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid = base_data.data.list_box.background_image;
	if(gid) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, get_color_modification(false, false, true), gfx_def.type_dependent, float(x), float(y),
						float(base_data.size.x), float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			} else {
				ogl::render_textured_rect(state, get_color_modification(false, false, true), float(x), float(y), float(base_data.size.x),
						float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			}
		}
	}
	container_base::render(state, x, y);
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::on_create(sys::state& state) noexcept {
	int16_t current_y = 0;
	int16_t subwindow_y_size = 0;
	while(current_y + subwindow_y_size <= base_data.size.y) {
		auto ptr = make_element_by_type<RowWinT>(state, get_row_element_name());
		row_windows.push_back(static_cast<RowWinT*>(ptr.get()));
		int16_t offset = ptr->base_data.position.y;
		ptr->base_data.position.y += current_y;
		subwindow_y_size = ptr->base_data.size.y;
		current_y += ptr->base_data.size.y + offset;
		add_child_to_front(std::move(ptr));
	}
	auto ptr = make_element_by_type<standard_listbox_scrollbar<RowWinT, RowConT>>(state, "standardlistbox_slider");
	list_scrollbar = static_cast<standard_listbox_scrollbar<RowWinT, RowConT>*>(ptr.get());
	add_child_to_front(std::move(ptr));
	list_scrollbar->scale_to_parent();

	update(state);
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid = base_data.data.list_box.background_image;
	if(gid) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, get_color_modification(false, false, true), gfx_def.type_dependent, float(x), float(y),
						float(base_data.size.x), float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			} else {
				ogl::render_textured_rect(state, get_color_modification(false, false, true), float(x), float(y), float(base_data.size.x),
						float(base_data.size.y),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped());
			}
		}
	}
	container_base::render(state, x, y);
}
template<class ItemWinT, class ItemConT>
void overlapping_listbox_element_base<ItemWinT, ItemConT>::update(sys::state& state) {
	auto spacing = int16_t(base_data.data.overlapping.spacing);
	if(base_data.get_element_type() == element_type::overlapping) {
		while(row_contents.size() > windows.size()) {
			auto ptr = make_element_by_type<ItemWinT>(state, get_row_element_name());
			if(subwindow_width <= 0) {
				subwindow_width = ptr->base_data.size.x;
			}
			windows.push_back(static_cast<ItemWinT*>(ptr.get()));
			add_child_to_front(std::move(ptr));
		}

		float size_ratio = float(row_contents.size() * (subwindow_width + spacing)) / float(base_data.size.x);
		int16_t offset = spacing + subwindow_width;
		if(size_ratio > 1.f) {
			offset = int16_t(float(subwindow_width) / size_ratio);
		}
		int16_t current_x = 0;
		if(base_data.data.overlapping.image_alignment == alignment::right) {
			current_x = base_data.size.x - subwindow_width - offset * int16_t(row_contents.size() - 1);
		}
		for(size_t i = 0; i < windows.size(); i++) {
			if(i < row_contents.size()) {
				update_subwindow(state, *windows[i], row_contents[i]);
				windows[i]->base_data.position.x = current_x;
				current_x += offset;
				windows[i]->set_visible(state, true);
			} else {
				windows[i]->set_visible(state, false);
			}
		}
	}
}

std::string_view overlapping_flags_box::get_row_element_name() {
	return "flag_list_flag";
}

void overlapping_flags_box::update_subwindow(sys::state& state, overlapping_flags_flag_button& subwindow,
		dcon::national_identity_id content) {
	subwindow.set_current_nation(state, content);
}

void overlapping_flags_box::on_update(sys::state& state) noexcept {
	current_nation = retrieve<dcon::nation_id>(state, parent);
	populate_flags(state);
}

void overlapping_friendly_flags::populate_flags(sys::state& state) {
	if(bool(current_nation)) {
		row_contents.clear();
		for(auto it : state.world.nation_get_gp_relationship_as_great_power(current_nation)) {
			if((it.get_status() & nations::influence::level_mask) == nations::influence::level_friendly) {
				row_contents.push_back(it.get_influence_target().get_identity_from_identity_holder());
			}
		}
		update(state);
	}
}

void overlapping_cordial_flags::populate_flags(sys::state& state) {
	if(bool(current_nation)) {
		row_contents.clear();
		for(auto it : state.world.nation_get_gp_relationship_as_great_power(current_nation)) {
			if((it.get_status() & nations::influence::level_mask) == nations::influence::level_cordial) {
				row_contents.push_back(it.get_influence_target().get_identity_from_identity_holder());
			}
		}
		update(state);
	}
}

void overlapping_sphere_flags::populate_flags(sys::state& state) {
	if(bool(current_nation)) {
		row_contents.clear();
		if(state.world.nation_get_in_sphere_of(current_nation)) {
			row_contents.push_back(state.world.nation_get_in_sphere_of(current_nation).get_identity_from_identity_holder());
		}
		for(auto it : state.world.nation_get_gp_relationship_as_great_power(current_nation)) {
			if((it.get_status() & nations::influence::level_mask) == nations::influence::level_in_sphere) {
				row_contents.push_back(it.get_influence_target().get_identity_from_identity_holder());
			}
		}
		update(state);
	}
}

void overlapping_puppet_flags::populate_flags(sys::state& state) {
	if(bool(current_nation)) {
		row_contents.clear();
		auto fat_id = dcon::fatten(state.world, current_nation);
		auto overlord = state.world.nation_get_overlord_as_subject(current_nation);
		auto overlord_nation = dcon::fatten(state.world, overlord).get_ruler();
		if(bool(overlord_nation)) {
			row_contents.push_back(overlord_nation.get_identity_from_identity_holder().id);
		} else {
			for(auto puppet : state.world.nation_get_overlord_as_ruler(current_nation)) {
				row_contents.push_back(puppet.get_subject().get_identity_from_identity_holder().id);
			}
		}
		update(state);
	}
}

void overlapping_ally_flags::populate_flags(sys::state& state) {
	if(bool(current_nation)) {
		row_contents.clear();
		for(auto rel : state.world.nation_get_diplomatic_relation(current_nation)) {
			if(rel.get_are_allied()) {
				auto ally = nations::get_relationship_partner(state, rel.id, current_nation);
				auto fat_ally = dcon::fatten(state.world, ally);
				row_contents.push_back(fat_ally.get_identity_from_identity_holder().id);
			}
		}
		update(state);
	}
}

void overlapping_enemy_flags::populate_flags(sys::state& state) {
	if(bool(current_nation)) {
		row_contents.clear();
		for(auto wa : state.world.nation_get_war_participant(current_nation)) {
			bool is_attacker = wa.get_is_attacker();
			for(auto o : wa.get_war().get_war_participant()) {
				if(o.get_is_attacker() != is_attacker) {
					row_contents.push_back(o.get_nation().get_identity_from_identity_holder().id);
				}
			}
		}
		update(state);
	}
}


std::string_view overlapping_truce_flags::get_row_element_name() {
	return "flag_list_flag";
}

void overlapping_truce_flag_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, "trucewith", text::variable_type::list, n);
	text::add_line(state, contents, "until_date", text::variable_type::x, until);
}

void overlapping_truce_flags::update_subwindow(sys::state& state, overlapping_truce_flag_button& subwindow, truce_pair content) {
	subwindow.upate_truce(state, content.n, content.until);
}

void overlapping_truce_flags::on_update(sys::state& state) noexcept {
	current_nation = retrieve<dcon::nation_id>(state, parent);

	row_contents.clear();
	for(auto rel : state.world.nation_get_diplomatic_relation(current_nation)) {
		if(rel.get_truce_until() && state.current_date < rel.get_truce_until()) {
			auto other = rel.get_related_nations(0) != current_nation ? rel.get_related_nations(0) : rel.get_related_nations(1);
			if(!military::are_at_war(state, current_nation, other)) {
				row_contents.push_back(truce_pair{ other, rel.get_truce_until() });
			}
		}
	}
	update(state);
}


dcon::national_identity_id flag_button::get_current_nation(sys::state& state) noexcept {
	auto nation = retrieve<dcon::national_identity_id>(state, parent);
	if(bool(nation)) {
		return nation;
	} else {
		return state.world.nation_get_identity_from_identity_holder(retrieve<dcon::nation_id>(state, parent));
	}
}

dcon::rebel_faction_id flag_button::get_current_rebel_faction(sys::state& state) noexcept {
	return retrieve<dcon::rebel_faction_id>(state, parent);
}

void flag_button::button_action(sys::state& state) noexcept {
	auto ident = get_current_nation(state);
	if(!ident)
		return;

	auto fat_id = dcon::fatten(state.world, ident);
	auto nation = fat_id.get_nation_from_identity_holder();
	if(bool(nation.id) && nation.get_owned_province_count() != 0) {
		state.open_diplomacy(nation.id);
	}
}

void flag_button2::button_action(sys::state& state) noexcept {
	auto nid = retrieve<dcon::nation_id>(state, this);
	auto tid = retrieve<dcon::national_identity_id>(state, this);
	if(!nid && tid) {
		nid = state.world.national_identity_get_nation_from_identity_holder(tid);
	}
	auto rid = retrieve<dcon::rebel_faction_id>(state, this);
	if(!nid && rid) {
		nid = state.world.rebel_faction_get_ruler_from_rebellion_within(rid);
	}
	if(nid && state.world.nation_get_owned_province_count(nid) != 0)
		state.open_diplomacy(nid);
}

void flag_button2::on_update(sys::state& state) noexcept {
	auto nid = retrieve<dcon::nation_id>(state, this);
	if(nid) {
		flag_texture_handle = ogl::get_flag_handle(state, state.world.nation_get_identity_from_identity_holder(nid), culture::get_current_flag_type(state, nid));
		return;
	}

	auto tid = retrieve<dcon::national_identity_id>(state, this);
	if(!nid && tid) {
		flag_texture_handle = ogl::get_flag_handle(state, tid, culture::get_current_flag_type(state, tid));
		return;
	}

	auto reb_tag = state.world.nation_get_identity_from_identity_holder(state.national_definitions.rebel_id);
	flag_texture_handle = ogl::get_flag_handle(state, reb_tag, culture::flag_type::default_flag);
}

void flag_button2::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto nid = retrieve<dcon::nation_id>(state, this);
	if(nid) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, nid);
		text::close_layout_box(contents, box);
		return;
	}
	auto tid = retrieve<dcon::national_identity_id>(state, this);
	if(tid) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, tid);
		text::close_layout_box(contents, box);
		return;
	}
	auto rid = retrieve<dcon::rebel_faction_id>(state, this);
	if(rid) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, rebel::rebel_name(state, rid));
		text::close_layout_box(contents, box);
		return;
	}
}

void flag_button2::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid;
	if(base_data.get_element_type() == element_type::image) {
		gid = base_data.data.image.gfx_object;
	} else if(base_data.get_element_type() == element_type::button) {
		gid = base_data.data.button.button_image;
	}
	if(gid && flag_texture_handle > 0) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.type_dependent) {
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
			auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
				float(x) + float(base_data.size.x - mask_tex.size_x) * 0.5f,
				float(y) + float(base_data.size.y - mask_tex.size_y) * 0.5f,
				float(mask_tex.size_x),
				float(mask_tex.size_y),
				flag_texture_handle, mask_handle, base_data.get_rotation(), gfx_def.is_vertically_flipped());
		} else {
			ogl::render_textured_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, base_data.get_rotation(),
					gfx_def.is_vertically_flipped());
		}
	}
	image_element_base::render(state, x, y);
}

void flag_button::set_current_nation(sys::state& state, dcon::national_identity_id ident) noexcept {
	if(!bool(ident))
		ident = state.world.nation_get_identity_from_identity_holder(state.national_definitions.rebel_id);

	auto fat_id = dcon::fatten(state.world, ident);
	auto nation = fat_id.get_nation_from_identity_holder();
	culture::flag_type flag_type = culture::flag_type{};
	if(bool(nation.id) && nation.get_owned_province_count() != 0) {
		flag_type = culture::get_current_flag_type(state, nation.id);
	} else {
		flag_type = culture::get_current_flag_type(state, ident);
	}
	flag_texture_handle = ogl::get_flag_handle(state, ident, flag_type);
}

void flag_button::on_update(sys::state& state) noexcept {
	set_current_nation(state, get_current_nation(state));
}

void flag_button::on_create(sys::state& state) noexcept {
	button_element_base::on_create(state);
}

void flag_button::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid;
	if(base_data.get_element_type() == element_type::image) {
		gid = base_data.data.image.gfx_object;
	} else if(base_data.get_element_type() == element_type::button) {
		gid = base_data.data.button.button_image;
	}
	if(gid && flag_texture_handle > 0) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.type_dependent) {
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
			auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
				float(x) + float(base_data.size.x - mask_tex.size_x) * 0.5f,
				float(y) + float(base_data.size.y - mask_tex.size_y) * 0.5f,
				float(mask_tex.size_x),
				float(mask_tex.size_y),
				flag_texture_handle, mask_handle, base_data.get_rotation(), gfx_def.is_vertically_flipped());
		} else {
			ogl::render_textured_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, base_data.get_rotation(),
					gfx_def.is_vertically_flipped());
		}
	}
	image_element_base::render(state, x, y);
}
void flag_button::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto ident = get_current_nation(state);
	if(ident) {
		auto name = nations::name_from_tag(state, ident);
		if(name) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, name);
			text::close_layout_box(contents, box);
		}
	} else {
		if(auto reb = get_current_rebel_faction(state); reb) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, rebel::rebel_name(state, reb));
			text::close_layout_box(contents, box);
			return;
		}
	}
}

message_result draggable_target::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	for(auto tmp = parent; tmp != nullptr; tmp = tmp->parent) {
		if(tmp->base_data.get_element_type() == element_type::window && tmp->base_data.data.window.is_moveable()) {
			state.ui_state.drag_target = tmp;
			return message_result::consumed;
		}
	}
	return message_result::consumed;
}

std::unique_ptr<element_base> make_element_immediate(sys::state& state, dcon::gui_def_id id) {
	auto& def = state.ui_defs.gui[id];
	if(def.get_element_type() == ui::element_type::image) {
		auto res = std::make_unique<image_element_base>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	} else if(def.get_element_type() == ui::element_type::button) {
		auto res = std::make_unique<button_element_base>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	} else if(def.get_element_type() == ui::element_type::window) {
		auto res = std::make_unique<window_element_base>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		res->on_create(state);
		return res;
	} else if(def.get_element_type() == ui::element_type::scrollbar) {
		auto res = std::make_unique<scrollbar>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		res->on_create(state);
		return res;
	} else if(def.get_element_type() == ui::element_type::text) {
		auto res = std::make_unique<simple_text_element_base>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		res->on_create(state);
		return res;
	}
	// TODO: other defaults

	return nullptr;
}

void scrollbar_left::button_action(sys::state& state) noexcept {
	send(state, parent, value_change{ -step_size, true, true });
}
void scrollbar_left::button_shift_action(sys::state& state) noexcept {
	send(state, parent, value_change{ -step_size * 5, true, true });
}
void scrollbar_left::button_shift_right_action(sys::state& state) noexcept {
	send(state, parent, value_change{ -step_size * 10000, true, true });
}
void scrollbar_right::button_action(sys::state& state) noexcept {
	send(state, parent, value_change{ step_size, true, true });
}
void scrollbar_right::button_shift_action(sys::state& state) noexcept {
	send(state, parent, value_change{ step_size * 5, true, true });
}
void scrollbar_right::button_shift_right_action(sys::state& state) noexcept {
	send(state, parent, value_change{ step_size * 10000, true, true });
}

message_result scrollbar_right::set(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<scrollbar_settings>()) {
		if(hold_continous) {
			button_action(state);
		}

		return message_result::consumed;
	}
	return message_result::unseen;
}
message_result scrollbar_left::set(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<scrollbar_settings>()) {
		if(hold_continous) {
			button_action(state);
		}

		return message_result::consumed;
	}
	return message_result::unseen;
}

message_result scrollbar_track::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	scrollbar_settings parent_state = retrieve<scrollbar_settings>(state, parent);
	int32_t pos_in_track = parent_state.vertical ? y : x;
	int32_t clamped_pos = std::clamp(pos_in_track, parent_state.buttons_size / 2, parent_state.track_size - parent_state.buttons_size / 2);
	float fp_pos = float(clamped_pos - parent_state.buttons_size / 2) / float(parent_state.track_size - parent_state.buttons_size);
	send(state, parent, value_change{ int32_t(parent_state.lower_value + fp_pos * (parent_state.upper_value - parent_state.lower_value)), true, false });
	return message_result::consumed;
}

tooltip_behavior scrollbar_track::has_tooltip(sys::state& state) noexcept {
	if(parent)
		return parent->has_tooltip(state);
	return opaque_element_base::has_tooltip(state);
}
void scrollbar_track::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(parent)
		return parent->update_tooltip(state, x, y, contents);
	return opaque_element_base::update_tooltip(state, x, y, contents);
}

message_result scrollbar_slider::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return message_result::consumed;
}
void scrollbar_slider::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {

	if(!parent)
		return;

	auto location_abs = get_absolute_location(*this);

	scrollbar_settings parent_settings = retrieve<scrollbar_settings>(state, parent);
	if(parent_settings.vertical) {
		if(!(location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y)) {
			return; // drag has left slider behind
		}
	} else {
		if(!(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x)) {
			return; // drag has left slider behind
		}
	}

	int32_t pos_in_track = 0;

	// TODO: take care of case where there are partial range limits

	float min_percentage = float(parent_settings.lower_limit - parent_settings.lower_value) / float(parent_settings.upper_value - parent_settings.lower_value);
	auto min_offest =
			parent_settings.buttons_size + int32_t((parent_settings.track_size - parent_settings.buttons_size) * min_percentage);

	float max_percentage = float(parent_settings.upper_limit - parent_settings.lower_value) /  float(parent_settings.upper_value - parent_settings.lower_value);
	auto max_offest =
			parent_settings.buttons_size + int32_t((parent_settings.track_size - parent_settings.buttons_size) * max_percentage);

	if(parent_settings.vertical) {
		base_data.position.y += int16_t(y - oldy);
		base_data.position.y = int16_t(
				std::clamp(int32_t(base_data.position.y), parent_settings.using_limits ? min_offest : parent_settings.buttons_size, parent_settings.using_limits ? max_offest : parent_settings.track_size));
		pos_in_track = base_data.position.y - parent_settings.buttons_size / 2;
	} else {
		base_data.position.x += int16_t(x - oldx);
		base_data.position.x = int16_t(
				std::clamp(int32_t(base_data.position.x), parent_settings.using_limits ? min_offest : parent_settings.buttons_size, parent_settings.using_limits ? max_offest : parent_settings.track_size));
		pos_in_track = base_data.position.x - parent_settings.buttons_size / 2;
	}
	float fp_pos =
			float(pos_in_track - parent_settings.buttons_size / 2) / float(parent_settings.track_size - parent_settings.buttons_size);

	Cyto::Any adjustment_payload = value_change{
			int32_t(parent_settings.lower_value + fp_pos * (parent_settings.upper_value - parent_settings.lower_value)), true, false};
	parent->impl_get(state, adjustment_payload);
}

void scrollbar::update_raw_value(sys::state& state, int32_t v) {
	stored_value = settings.using_limits ? std::clamp(v, settings.lower_limit, settings.upper_limit) : std::clamp(v, settings.lower_value, settings.upper_value);

	float percentage = float(stored_value - settings.lower_value) / float(settings.upper_value - settings.lower_value);
	auto offset = settings.buttons_size + int32_t((settings.track_size - settings.buttons_size) * percentage);
	if(slider && state.ui_state.drag_target != slider) {
		if(settings.vertical)
			slider->base_data.position.y = int16_t(offset);
		else
			slider->base_data.position.x = int16_t(offset);
	}
}
void scrollbar::update_scaled_value(sys::state& state, float v) {
	float scaling_factor = float(settings.scaling_factor);
	// Non-vanilla special accomodation
	if(base_data.data.scrollbar.get_step_size() == ui::step_size::twenty_five) {
		scaling_factor = 0.25f;
	}
	int32_t rv = std::clamp(int32_t(v * scaling_factor), settings.lower_value, settings.upper_value);
	update_raw_value(state, rv);
}
float scrollbar::scaled_value() const {
	return float(stored_value) / float(settings.scaling_factor);
}
int32_t scrollbar::raw_value() const {
	return stored_value;
}

void scrollbar::change_settings(sys::state& state, mutable_scrollbar_settings const& settings_s) {
	settings.lower_limit = settings_s.lower_limit * settings.scaling_factor;
	settings.lower_value = settings_s.lower_value * settings.scaling_factor;
	settings.upper_limit = settings_s.upper_limit * settings.scaling_factor;
	settings.upper_value = settings_s.upper_value * settings.scaling_factor;
	settings.using_limits = settings_s.using_limits;

	settings.upper_value = std::max(settings.upper_value, settings.lower_value + 1); // ensure the scrollbar is never of range zero

	if(settings.using_limits) {
		if(right_limit) {
			right_limit->set_visible(state, settings.lower_value < settings.lower_limit);

			float percentage = float(settings.lower_limit - settings.lower_value) / float(settings.upper_value - settings.lower_value);
			auto offset = settings.buttons_size + int32_t((settings.track_size - settings.buttons_size) * percentage);
			if(settings.vertical)
				right_limit->base_data.position.y = int16_t(offset);
			else
				right_limit->base_data.position.x = int16_t(offset);
		}
		if(left_limit) {
			left_limit->set_visible(state, settings.upper_value > settings.upper_limit);

			float percentage = float(settings.upper_limit - settings.lower_value) / float(settings.upper_value - settings.lower_value);
			auto offset = settings.buttons_size + int32_t((settings.track_size - settings.buttons_size) * percentage);
			if(settings.vertical)
				left_limit->base_data.position.y = int16_t(offset + settings.buttons_size - 10);
			else
				left_limit->base_data.position.x = int16_t(offset + settings.buttons_size - 10);
		}
		if(stored_value < settings.lower_limit || stored_value > settings.upper_limit) {
			update_raw_value(state, stored_value);
			//on_value_change(state, stored_value);
		}
	} else {
		if(right_limit)
			right_limit->set_visible(state, false);
		if(left_limit)
			left_limit->set_visible(state, false);
		if(stored_value < settings.lower_value || stored_value > settings.upper_value) {
			update_raw_value(state, stored_value);
			//on_value_change(state, stored_value);
		}
	}
}

void scrollbar::on_create(sys::state& state) noexcept {
	// create & position sub elements
	// fill out settings data
	// set initial slider pos

	if(base_data.get_element_type() == element_type::scrollbar) {
		auto step = base_data.data.scrollbar.get_step_size();
		settings.scaling_factor = 1;
		switch(step) {
		case step_size::twenty_five:
			break;
		case step_size::two:
			break;
		case step_size::one:
			break;
		case step_size::one_tenth:
			settings.scaling_factor = 10;
			break;
		case step_size::one_hundredth:
			settings.scaling_factor = 100;
			break;
		case step_size::one_thousandth:
			settings.scaling_factor = 1000;
			break;
		}
		settings.lower_value = 0;
		settings.upper_value = base_data.data.scrollbar.max_value * settings.scaling_factor;
		settings.lower_limit = 0;
		settings.upper_limit = settings.upper_value;

		settings.vertical = !base_data.data.scrollbar.is_horizontal();
		stored_value = settings.lower_value;

		auto first_child = base_data.data.scrollbar.first_child;
		auto num_children = base_data.data.scrollbar.num_children;

		if(num_children >= 6) {
			auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(5 + first_child.index()));
			auto ch_res = make_element_by_type<image_element_base>(state, child_tag);
			right_limit = ch_res.get();
			right_limit->set_visible(state, false);
			add_child_to_back(std::move(ch_res));
		}
		if(num_children >= 5) {
			auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(4 + first_child.index()));
			auto ch_res = make_element_by_type<image_element_base>(state, child_tag);
			left_limit = ch_res.get();
			left_limit->set_visible(state, false);
			add_child_to_back(std::move(ch_res));
		}

		if(num_children >= 4) {
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(2 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_slider>(state, child_tag);
				slider = ch_res.get();
				add_child_to_back(std::move(ch_res));
			}
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(0 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_left>(state, child_tag);
				left = ch_res.get();
				add_child_to_back(std::move(ch_res));

				settings.buttons_size = settings.vertical ? left->base_data.size.y : left->base_data.size.x;
				if(step_size::twenty_five == step)
					left->step_size = 25;
				else if(step_size::two == step)
					left->step_size = 2;
				else
					left->step_size = 1;
			}
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(1 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_right>(state, child_tag);
				right = ch_res.get();
				add_child_to_back(std::move(ch_res));

				if(step_size::twenty_five == step)
					right->step_size = 25;
				else if(step_size::two == step)
					right->step_size = 2;
				else
					right->step_size = 1;
			}
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(3 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_track>(state, child_tag);
				track = ch_res.get();
				add_child_to_back(std::move(ch_res));

				settings.track_size = settings.vertical ? track->base_data.size.y : track->base_data.size.x;
			}
			left->base_data.position.x = 0;
			left->base_data.position.y = 0;
			if(settings.vertical) {
				track->base_data.position.y = int16_t(settings.buttons_size);
				slider->base_data.position.y = int16_t(settings.buttons_size);
				right->base_data.position.y = int16_t(settings.track_size + settings.buttons_size);
				// track->base_data.position.x = 0;
				slider->base_data.position.x = 0;
				right->base_data.position.x = 0;
			} else {
				track->base_data.position.x = int16_t(settings.buttons_size);
				slider->base_data.position.x = int16_t(settings.buttons_size);
				right->base_data.position.x = int16_t(settings.track_size + settings.buttons_size);
				// track->base_data.position.y = 0;
				slider->base_data.position.y = 0;
				right->base_data.position.y = 0;
			}
		}
	}
}
void scrollbar_slider::on_drag_finish(sys::state& state) noexcept {
	if(parent) {
		parent->impl_on_drag_finish(state);
	}
}
message_result scrollbar::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<scrollbar_settings>()) {
		settings.track_size = track ? int32_t(settings.vertical ? track->base_data.size.y : track->base_data.size.x) : 1;
		payload = settings;
		return message_result::consumed;
	} else if(payload.holds_type<value_change>()) {
		auto adjustments = any_cast<value_change>(payload);

		if(adjustments.is_relative)
			stored_value += adjustments.new_value;
		else
			stored_value = adjustments.new_value;

		if(adjustments.move_slider) {
			update_raw_value(state, stored_value);
		} else {
			stored_value = settings.using_limits ? std::clamp(stored_value, settings.lower_limit, settings.upper_limit)
																					 : std::clamp(stored_value, settings.lower_value, settings.upper_value);
		}

		if(adjustments.move_slider == true && adjustments.is_relative == false && !state.ui_state.drag_target) { // track click
			state.ui_state.drag_target = slider;
		}

		on_value_change(state, stored_value);
		return message_result::unseen;
	} else {
		return message_result::unseen;
	}
}

void unit_frame_bg::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto display_unit = retrieve< unit_var>(state, parent);
	if(std::holds_alternative<dcon::army_id>(display_unit))
		single_unit_tooltip(state, contents, std::get<dcon::army_id>(display_unit));
	else if(std::holds_alternative<dcon::navy_id>(display_unit))
		single_unit_tooltip(state, contents, std::get<dcon::navy_id>(display_unit));
	text::add_line(state, contents, "alice_utt_controls_1");
	text::add_line(state, contents, "alice_utt_controls_2");
	if(state.network_mode != sys::network_mode_type::single_player)
		text::add_line(state, contents, "alice_utt_controls_3");
}

void populate_shortcut_tooltip(sys::state& state, ui::element_base& elm, text::columnar_layout& contents) noexcept {
	if(elm.base_data.get_element_type() != ui::element_type::button)
		return;
	if(elm.base_data.data.button.shortcut == sys::virtual_key::NONE)
		return;
	static const std::string_view key_names[] = { //enum class virtual_key : uint8_t {
		"", //NONE = 0x00,
		"Left-Button", //LBUTTON = 0x01,
		"Right-Button", //RBUTTON = 0x02,
		"Cancel", //CANCEL = 0x03,
		"Multimedia button", //MBUTTON = 0x04,
		"XButton1", //XBUTTON_1 = 0x05,
		"XButton2", //XBUTTON_2 = 0x06,
		"", //0x07
		"Backspace", //BACK = 0x08,
		"TAB", //TAB = 0x09,
		"", // 0x0A
		"", // 0x0B
		"Clear", //CLEAR = 0x0C,
		"Return", //RETURN = 0x0D,
		"", // 0x0E
		"", // 0x0F
		"Shift", //SHIFT = 0x10,
		"Control", //CONTROL = 0x11,
		"Menu", //MENU = 0x12,
		"Pause", //PAUSE = 0x13,
		"Capital", //CAPITAL = 0x14,
		"Kana", //KANA = 0x15,
		"", // 0x16
		"Junja", //JUNJA = 0x17,
		"Final", //FINAL = 0x18,
		"Kanji", //KANJI = 0x19,
		"", // 0x1A
		"Escape", //ESCAPE = 0x1B,
		"Convert", //CONVERT = 0x1C,
		"Nonconvert", //NONCONVERT = 0x1D,
		"Accept", //ACCEPT = 0x1E,
		"Modechange", //MODECHANGE = 0x1F,
		"Spacebar", //SPACE = 0x20,
		"Prior", //PRIOR = 0x21,
		"Next", //NEXT = 0x22,
		"End", //END = 0x23,
		"Home", //HOME = 0x24,
		"Left", //LEFT = 0x25,
		"Up", //UP = 0x26,
		"Right", //RIGHT = 0x27,
		"Down", //DOWN = 0x28,
		"Select", //SELECT = 0x29,
		"Print", //PRINT = 0x2A,
		"Execute", //EXECUTE = 0x2B,
		"Snapshot", //SNAPSHOT = 0x2C,
		"Insert", //INSERT = 0x2D,
		"Delete", //DELETE_KEY = 0x2E,
		"Help", //HELP = 0x2F,
		"0", //NUM_0 = 0x30,
		"1", //NUM_1 = 0x31,
		"2", //NUM_2 = 0x32,
		"3", //NUM_3 = 0x33,
		"4", //NUM_4 = 0x34,
		"5", //NUM_5 = 0x35,
		"6", //NUM_6 = 0x36,
		"7", //NUM_7 = 0x37,
		"8", //NUM_8 = 0x38,
		"9", //NUM_9 = 0x39,
		"", // 0x3A
		"", // 0x3B
		"", // 0x3C
		"", // 0x3D
		"", // 0x3E
		"", // 0x3F
		"", // 0x40
		"A", //A = 0x41,
		"B", //B = 0x42,
		"C", //C = 0x43,
		"D", //D = 0x44,
		"E", //E = 0x45,
		"F", //F = 0x46,
		"G", //G = 0x47,
		"H", //H = 0x48,
		"I", //I = 0x49,
		"J", //J = 0x4A,
		"K", //K = 0x4B,
		"L", //L = 0x4C,
		"M", //M = 0x4D,
		"N", //N = 0x4E,
		"O", //O = 0x4F,
		"P", //P = 0x50,
		"Q", //Q = 0x51,
		"R", //R = 0x52,
		"S", //S = 0x53,
		"T", //T = 0x54,
		"U", //U = 0x55,
		"V", //V = 0x56,
		"W", //W = 0x57,
		"X", //X = 0x58,
		"Y", //Y = 0x59,
		"Z", //Z = 0x5A,
		"Left Windows", //LWIN = 0x5B,
		"Right Windows", //RWIN = 0x5C,
		"Apps", //APPS = 0x5D,
		"", // 0x5E
		"Sleep", //SLEEP = 0x5F,
		"Numpad 0", //NUMPAD0 = 0x60,
		"Numpad 1", //NUMPAD1 = 0x61,
		"Numpad 2", //NUMPAD2 = 0x62,
		"Numpad 3", //NUMPAD3 = 0x63,
		"Numpad 4", //NUMPAD4 = 0x64,
		"Numpad 5", //NUMPAD5 = 0x65,
		"Numpad 6", //NUMPAD6 = 0x66,
		"Numpad 7", //NUMPAD7 = 0x67,
		"Numpad 8", //NUMPAD8 = 0x68,
		"Numpad 9", //NUMPAD9 = 0x69,
		"Numpad *", //MULTIPLY = 0x6A,
		"Numpad +", //ADD = 0x6B,
		"Numpad .", //SEPARATOR = 0x6C,
		"Numpad -", //SUBTRACT = 0x6D,
		"Numpad .", //DECIMAL = 0x6E,
		"Numpad /", //DIVIDE = 0x6F,
		"F1", //F1 = 0x70,
		"F2", //F2 = 0x71,
		"F3", //F3 = 0x72,
		"F4", //F4 = 0x73,
		"F5", //F5 = 0x74,
		"F6", //F6 = 0x75,
		"F7", //F7 = 0x76,
		"F8", //F8 = 0x77,
		"F9", //F9 = 0x78,
		"F10", //F10 = 0x79,
		"F11", //F11 = 0x7A,
		"F12", //F12 = 0x7B,
		"F13", //F13 = 0x7C,
		"F14", //F14 = 0x7D,
		"F15", //F15 = 0x7E,
		"F16", //F16 = 0x7F,
		"F17", //F17 = 0x80,
		"F18", //F18 = 0x81,
		"F19", //F19 = 0x82,
		"F20", //F20 = 0x83,
		"F21", //F21 = 0x84,
		"F22", //F22 = 0x85,
		"F23", //F23 = 0x86,
		"F24", //F24 = 0x87,
		"Navigation View", //NAVIGATION_VIEW = 0x88,
		"Navigation Menu", //NAVIGATION_MENU = 0x89,
		"Navigation Up", //NAVIGATION_UP = 0x8A,
		"Navigation Down", //NAVIGATION_DOWN = 0x8B,
		"Navigation Left", //NAVIGATION_LEFT = 0x8C,
		"Navigation Right", //NAVIGATION_RIGHT = 0x8D,
		"Navigation Accept", //NAVIGATION_ACCEPT = 0x8E,
		"Navigation Cancel", //NAVIGATION_CANCEL = 0x8F,
		"Numlock", //NUMLOCK = 0x90,
		"Scroll lock", //SCROLL = 0x91,
		"=", //OEM_NEC_EQUAL = 0x92,
		"", // 0x93
		"", // 0x94
		"", // 0x95
		"", // 0x96
		"", // 0x97
		"", // 0x98
		"", // 0x99
		"", // 0x9A
		"", // 0x9B
		"", // 0x9C
		"", // 0x9D
		"", // 0x9E
		"", // 0x9F
		"Left Shift", //LSHIFT = 0xA0,
		"Right Shift", //RSHIFT = 0xA1,
		"Left Control", //LCONTROL = 0xA2,
		"Right Control", //RCONTROL = 0xA3,
		"Left Menu", //LMENU = 0xA4,
		"Right Menu", //RMENU = 0xA5,
		"", // 0xA6
		"", // 0xA7
		"", // 0xA8
		"", // 0xA9
		"", // 0xAA
		"", // 0xAB
		"", // 0xAC
		"", // 0xAD
		"", // 0xAE
		"", // 0xAF
		"", // 0xB0
		"", // 0xB1
		"", // 0xB2
		"", // 0xB3
		"", // 0xB4
		"", // 0xB5
		"", // 0xB6
		"", // 0xB7
		"", // 0xB8
		"", // 0xB9
		";", //SEMICOLON = 0xBA,
		"+", //PLUS = 0xBB,
		",", //COMMA = 0xBC,
		"-", //MINUS = 0xBD,
		".", //PERIOD = 0xBE,
		"\\", //FORWARD_SLASH = 0xBF,
		"~", //TILDA = 0xC0,
		"", // 0xC1
		"", // 0xC2
		"", // 0xC3
		"", // 0xC4
		"", // 0xC5
		"", // 0xC6
		"", // 0xC7
		"", // 0xC8
		"", // 0xC9
		"", // 0xCA
		"", // 0xCB
		"", // 0xCC
		"", // 0xCD
		"", // 0xCE
		"", // 0xCF
		"[", //OPEN_BRACKET = 0xDB,
		"/", //BACK_SLASH = 0xDC,
		"]", //CLOSED_BRACKET = 0xDD,
		"\"", //QUOTE = 0xDE
	};
	text::add_line(state, contents, "alice_shortcut_tooltip", text::variable_type::x, key_names[uint8_t(elm.base_data.data.button.shortcut)]);
}

} // namespace ui
