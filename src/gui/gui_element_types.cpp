#include <algorithm>
#include <string_view>
#include <variant>
#include "culture.hpp"
#include "dcon_generated.hpp"
#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "fonts.hpp"
#include "gui_graphics.hpp"
#include "opengl_wrapper.hpp"
#include "text.hpp"

namespace ui {

inline message_result greater_result(message_result a, message_result b) {
	if(a == message_result::consumed || b == message_result::consumed)
		return message_result::consumed;
	if(a == message_result::seen || b == message_result::seen)
		return message_result::seen;
	return message_result::unseen;
}


mouse_probe container_base::impl_probe_mouse(sys::state& state, int32_t x, int32_t y) noexcept {
	for(auto& c : children) {
		if(c->is_visible()) {
			auto relative_location = child_relative_location(*this, *c);
			auto res = c->impl_probe_mouse(state, x - relative_location.x, y - relative_location.y);
			if(res.under_mouse)
				return res;
		}
	}
	return element_base::impl_probe_mouse(state, x, y);
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
	for(auto& c : children) {
		if(c->is_visible()) {
			c->impl_on_update(state);
		}
	}
	on_update(state);
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

	for(size_t i = children.size(); i-- > 0; ) {
		if(children[i]->is_visible()) {
			auto relative_location = child_relative_location(*this, *(children[i]));
			children[i]->impl_render(state, x + relative_location.x, y + relative_location.y);
		}
	}
}


std::unique_ptr<element_base> container_base::remove_child(element_base* child) noexcept {
	if(auto it = std::find_if(children.begin(), children.end(),
		[child](std::unique_ptr<element_base>& p) { return p.get() == child; }); it != children.end()) {
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
	if(auto it = std::find_if(children.begin(), children.end(),
		[child](std::unique_ptr<element_base>& p) { return p.get() == child; }); it != children.end()) {
		if(it != children.begin())
			std::rotate(children.begin(), it, it + 1);
	}
}
void container_base::move_child_to_back(element_base* child) noexcept {
	if(auto it = std::find_if(children.begin(), children.end(),
		[child](std::unique_ptr<element_base>& p) { return p.get() == child; }); it != children.end()) {
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
		[&state, name](std::unique_ptr<element_base>& p) { return state.to_string_view(p->base_data.name) == name; }); it != children.end()) {
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
				ogl::render_bordered_rect(
					state,
					get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					gfx_def.type_dependant,
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(
					state,
					get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					frame,
					gfx_def.number_of_frames,
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			} else {
				ogl::render_textured_rect(
					state,
					get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			}
		}
	}
}

void button_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	image_element_base::render(state, x, y);
	if(stored_text.length() > 0) {

		auto linesz = state.font_collection.fonts[font_id - 1].line_height(font_size);
		auto ycentered = (base_data.size.y - linesz) / 2;

		ogl::render_text(
			state, stored_text.c_str(), uint32_t(stored_text.length()),
			get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
			float(x + text_offset), float(y + ycentered + state.font_collection.fonts[font_id - 1].top_adjustment(font_size)), float(font_size),
			black_text ? ogl::color3f{ 0.0f,0.0f,0.0f } : ogl::color3f{ 1.0f,1.0f,1.0f },
			state.font_collection.fonts[font_id - 1]);
	}
}

void button_element_base::set_button_text(sys::state& state, std::string const& new_text) {
	stored_text = new_text;
	text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size)) / 2.0f;
}

void button_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::button) {
		auto base_text_handle = base_data.data.button.txt;
		stored_text = text::produce_simple_string(state, base_text_handle);
		font_id = text::font_index_from_font_id(base_data.data.button.font_handle);
		font_size = text::size_from_font_id(base_data.data.button.font_handle);
		black_text = text::is_black_from_font_id(base_data.data.button.font_handle);
		text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size)) / 2.0f;
	}
}

message_result edit_box_element_base::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	// Set edit control so we get on_text events
	state.in_edit_control = true;
	state.ui_state.edit_target = this;
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
	return message_result::consumed;
}

void edit_box_element_base::on_text(sys::state& state, char ch) noexcept {
	if(ch >= 32) {
		auto s = std::string(get_text(state)).insert(edit_index, 1, ch);
		edit_index++;
		set_text(state, s);
		edit_box_update(state, s);
	}
}

message_result edit_box_element_base::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(state.ui_state.edit_target == this) {
		// Typable keys are handled by on_text callback, we only handle control keys
		auto s = std::string(get_text(state));
		switch(key) {
		case sys::virtual_key::RETURN:
			edit_box_enter(state, s);
			s.clear();
			set_text(state, s);
			edit_index = 0;
			break;
		case sys::virtual_key::LEFT:
			edit_index = std::max<int32_t>(edit_index - 1, 0);
			break;
		case sys::virtual_key::RIGHT:
			edit_index = std::min<int32_t>(edit_index + 1, int32_t(s.length()));
			break;
		case sys::virtual_key::DELETE_KEY:
			if(edit_index < int32_t(s.length()))
				s.erase(edit_index, 1);
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
			ogl::render_bordered_rect(
				state,
				get_color_modification(false, false, false),
				16.f,
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				ogl::get_texture_handle(state, background_texture_id, false),
				base_data.get_rotation(),
				false
			);
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

void simple_text_element_base::set_text(sys::state& state, std::string const& new_text) {
	stored_text = new_text;
	if(base_data.get_element_type() == element_type::button) {
		switch(base_data.data.button.get_alignment()) {
			case alignment::centered:
			case alignment::justified:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size)) / 2.0f;
				break;
			case alignment::right:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size));
				break;
			case alignment::left:
				text_offset = 0.0f;
				break;

		}
	} else if(base_data.get_element_type() == element_type::text) {
		switch(base_data.data.button.get_alignment()) {
			case alignment::centered:
			case alignment::justified:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size) - base_data.data.text.border_size.x) / 2.0f;
				break;
			case alignment::right:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size) - base_data.data.text.border_size.x);
				break;
			case alignment::left:
				text_offset = base_data.data.text.border_size.x;
				break;

		}
	}
}
void simple_text_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::button) {
		auto base_text_handle = base_data.data.button.txt;
		stored_text = text::produce_simple_string(state, base_text_handle);
		font_id = text::font_index_from_font_id(base_data.data.button.font_handle);
		font_size = text::size_from_font_id(base_data.data.button.font_handle);
		black_text = text::is_black_from_font_id(base_data.data.button.font_handle);

		switch(base_data.data.button.get_alignment()) {
			case alignment::centered:
			case alignment::justified:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size)) / 2.0f;
				break;
			case alignment::right:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size));
				break;
			case alignment::left:
				text_offset = 0.0f;
				break;

		}

	} else if(base_data.get_element_type() == element_type::text) {
		auto base_text_handle = base_data.data.text.txt;
		stored_text = text::produce_simple_string(state, base_text_handle);
		font_id = text::font_index_from_font_id(base_data.data.text.font_handle);
		font_size = text::size_from_font_id(base_data.data.text.font_handle);
		black_text = text::is_black_from_font_id(base_data.data.text.font_handle);

		switch(base_data.data.button.get_alignment()) {
			case alignment::centered:
			case alignment::justified:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size) - base_data.data.text.border_size.x) / 2.0f;
				break;
			case alignment::right:
				text_offset = (base_data.size.x - state.font_collection.fonts[font_id - 1].text_extent(stored_text.c_str(), uint32_t(stored_text.length()), font_size) - base_data.data.text.border_size.x);
				break;
			case alignment::left:
				text_offset = base_data.data.text.border_size.x;
				break;

		}
	}
}
void simple_text_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	if(stored_text.length() > 0) {
		if(base_data.get_element_type() == element_type::text) {
			//auto linesz = state.font_collection.fonts[font_id - 1].line_height(font_size);
			//auto ycentered = (base_data.size.y - base_data.data.text.border_size.y - linesz) / 2;
			//ycentered = std::max(ycentered + state.font_collection.fonts[font_id - 1].top_adjustment(font_size), float(base_data.data.text.border_size.y));
			ogl::render_text(
				state, stored_text.c_str(), uint32_t(stored_text.length()),
				ogl::color_modification::none,
				float(x + text_offset), float(y + base_data.data.text.border_size.y), float(font_size),
				black_text ? ogl::color3f{ 0.0f,0.0f,0.0f } : ogl::color3f{ 1.0f,1.0f,1.0f },
				state.font_collection.fonts[font_id - 1]);
		} else {
			auto linesz = state.font_collection.fonts[font_id - 1].line_height(font_size);
			auto ycentered = (base_data.size.y - linesz) / 2;

			ogl::render_text(
				state, stored_text.c_str(), uint32_t(stored_text.length()),
				ogl::color_modification::none,
				float(x + text_offset), float(y + ycentered + state.font_collection.fonts[font_id - 1].top_adjustment(font_size)), float(font_size),
				black_text ? ogl::color3f{ 0.0f,0.0f,0.0f } : ogl::color3f{ 1.0f,1.0f,1.0f },
				state.font_collection.fonts[font_id - 1]);
		}
	}
}

ogl::color3f get_text_color(text::text_color text_color) {
	switch(text_color) {
	case text::text_color::black:
	case text::text_color::unspecified:
		return ogl::color3f{ 0.f, 0.f, 0.f };
	case text::text_color::white:
		return ogl::color3f{ 1.f, 1.f, 1.f };
	case text::text_color::red:
		return ogl::color3f{ 1.f, 0.f, 0.f };
	case text::text_color::green:
		return ogl::color3f{ 0.f, 1.f, 0.f };
	case text::text_color::yellow:
		return ogl::color3f{ 1.f, 1.f, 0.f };
	case text::text_color::light_blue:
		return ogl::color3f{ 0.f, 0.f, 1.f };
	case text::text_color::dark_blue:
		return ogl::color3f{ 0.f, 0.f, .5f };
	default:
		return ogl::color3f{ 0.f, 0.f, 0.f };
	}
}

bool is_hyperlink_substitution(text_substitution sub) {
        return std::holds_alternative<dcon::nation_id>(sub) ||
               std::holds_alternative<dcon::state_definition_id>(sub) ||
               std::holds_alternative<dcon::province_id>(sub);
}

void multiline_text_element_base::add_text_section(sys::state& state, std::string_view text, float& current_x, float& current_y, text::text_color color) noexcept {
	auto& font = state.font_collection.fonts[font_id - 1];
	size_t str_i = 0;
	size_t current_len = 0;
	while(str_i < text.size()) {
		// FIXME: this approach of finding word breaks does not apply to all languages
		auto next_wb = text.find_first_of(" \n\t", str_i + current_len);
		if(next_wb == std::string_view::npos) {
			next_wb = text.size();
		}
		next_wb = std::min(next_wb, text.size()) - str_i;
		if(next_wb == current_len) {
			current_len++;
		} else {
			auto seg_start = std::next(text.begin(), str_i);
			std::string_view segment{ seg_start, std::next(seg_start, next_wb) };
			if(current_len == 0 && current_x + font.text_extent(segment.data(), uint32_t(segment.size()), font_size) >= base_data.size.x) {
				// the current word is too long for the text box, just let it overflow
				sections.push_back(multiline_text_section{ segment, current_x, current_y, color });
				current_x = 0.f;
				current_y += line_height + vertical_spacing;
				str_i += next_wb;
				current_len = 0;
				line_count++;
			} else if(current_x + font.text_extent(segment.data(), uint32_t(segment.size()), font_size) >= base_data.size.x) {
				std::string_view section{ seg_start, std::next(seg_start, current_len) };
				sections.push_back(multiline_text_section{ section, current_x, current_y, color });
				current_x = 0.f;
				current_y += line_height + vertical_spacing;
				str_i += current_len;
				current_len = 0;
				line_count++;
			} else if(next_wb == text.size() - str_i) {
				// we've reached the end of the text
				std::string_view remaining{ seg_start, text.end() };
				sections.push_back(multiline_text_section{ remaining, current_x, current_y, color });
				current_x += font.text_extent(remaining.data(), uint32_t(remaining.size()), font_size);
				if(current_x >= base_data.size.x) {
					current_x = 0.f;
					current_y += line_height + vertical_spacing;
					line_count++;
				}
				break;
			} else {
				current_len = next_wb;
			}
		}
	}
}

std::string_view multiline_text_element_base::get_substitute(sys::state& state, text::variable_type var_type) noexcept {
	if(std::holds_alternative<std::string_view>(substitutions[size_t(var_type)])) {
		return std::get<std::string_view>(substitutions[size_t(var_type)]);
	} else if(std::holds_alternative<dcon::text_key>(substitutions[size_t(var_type)])) {
		auto tkey = std::get<dcon::text_key>(substitutions[size_t(var_type)]);
		return state.to_string_view(tkey);
	} else if(std::holds_alternative<dcon::nation_id>(substitutions[size_t(var_type)])) {
		dcon::nation_id nation_id = std::get<dcon::nation_id>(substitutions[size_t(var_type)]);
		dcon::nation_fat_id fat_id = dcon::fatten(state.world, nation_id);
		auto name_id = fat_id.get_identity_from_identity_holder().get_name();
		std::string_view name{ text::produce_simple_string(state, name_id) };
		return name; 
	} else if(std::holds_alternative<dcon::state_definition_id>(substitutions[size_t(var_type)])) {
		dcon::state_definition_id state_id = std::get<dcon::state_definition_id>(substitutions[size_t(var_type)]);
		dcon::state_definition_fat_id fat_id = dcon::fatten(state.world, state_id);
		auto name_id = fat_id.get_name();
		std::string_view name{ text::produce_simple_string(state, name_id) };
		return name;
	} else if(std::holds_alternative<dcon::province_id>(substitutions[size_t(var_type)])) {
		auto province_id = std::get<dcon::province_id>(substitutions[size_t(var_type)]);
		dcon::province_fat_id fat_id = dcon::fatten(state.world, province_id);
		auto name_id = fat_id.get_name();
		std::string_view name{ text::produce_simple_string(state, name_id) };
		return name;
	} else {
		std::string_view unknown{ "?" };
		return unknown;
	}
}

void multiline_text_element_base::generate_sections(sys::state& state) noexcept {
	auto& seq = state.text_sequences[base_data.data.text.txt];
	font_id = text::font_index_from_font_id(base_data.data.text.font_handle);
	font_size = text::size_from_font_id(base_data.data.text.font_handle);
	auto& font = state.font_collection.fonts[font_id - 1];
	line_height = font.line_height(font_size);
	text::text_color current_color = text::text_color::black;
	float current_x = 0.f;
	float current_y = 0.f;

	for(size_t i = seq.starting_component; i < size_t(seq.starting_component + seq.component_count); i++) {
		if(std::holds_alternative<dcon::text_key>(state.text_components[i])) {
			auto tkey = std::get<dcon::text_key>(state.text_components[i]);
			std::string_view text = state.to_string_view(tkey);
			add_text_section(state, text, current_x, current_y, current_color);
		} else if(std::holds_alternative<text::line_break>(state.text_components[i])) {
			current_x = 0.f;
			current_y += line_height + vertical_spacing;
			line_count++;
		} else if(std::holds_alternative<text::text_color>(state.text_components[i])) {
			current_color = std::get<text::text_color>(state.text_components[i]);
		} else if(std::holds_alternative<text::variable_type>(state.text_components[i])) {
			auto var_type = std::get<text::variable_type>(state.text_components[i]);
			if(size_t(var_type) < substitutions.size()) {
				if(is_hyperlink_substitution(substitutions[size_t(var_type)])) {
					int32_t last_x = int32_t(current_x);
					int32_t last_y = int32_t(current_y);
					auto substitute = get_substitute(state, var_type);
					add_text_section(state, substitute, current_x, current_y, current_color);
					for(int32_t line = last_y; line <= int32_t(current_y); line += int32_t(line_height + vertical_spacing)) {
						int32_t x = line == last_y ? last_x : 0;
						int32_t width = line == int32_t(current_y) ? int32_t(current_x) : base_data.size.x;
						if(x < width) {
							hyperlinks.push_back(hyperlink{
								substitutions[size_t(var_type)], 
								x, 
								line, 
								width, 
								line + int32_t(line_height + vertical_spacing)
							});
						}
					}
				} else {
					auto substitute = get_substitute(state, var_type);
					add_text_section(state, substitute, current_x, current_y, current_color);
				}
			}
		}
	}
	if(current_x) {
		line_count++;
	}
	visible_lines = std::min(line_count, base_data.size.y / int32_t(line_height));
}

void multiline_text_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::text) {
		generate_sections(state);
	}
}

void multiline_text_element_base::update_text(sys::state& state, dcon::text_sequence_id seq_id) {
	if(base_data.get_element_type() == element_type::text) {
		base_data.data.text.txt = seq_id;
		generate_sections(state);
		set_scroll_pos(0);
	}
}

void multiline_text_element_base::update_substitutions(sys::state& state, std::vector<text_substitution> subs) {
	substitutions = subs;
	if(base_data.get_element_type() == element_type::text) {
		generate_sections(state);
		set_scroll_pos(current_line);
	}
}

void multiline_text_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	for(auto& section : sections) {
		float line_offset = section.y_offset - line_height * float(current_line);
		if(section.stored_text.size() && 0 <= line_offset && line_offset < base_data.size.y) {
			ogl::render_text(
				state, section.stored_text.data(), uint32_t(section.stored_text.size()),
				ogl::color_modification::none,
				float(x + section.x_offset), float(y + base_data.size.y + line_offset), float(font_size),
				get_text_color(section.color),
				state.font_collection.fonts[font_id - 1]
			);
		}
	}
}

message_result multiline_text_element_base::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	for(auto& link : hyperlinks) {
		if(link.x <= x && x < link.width && link.y <= y && y < link.height) {
			// TODO implement hyperlink actions
			return message_result::consumed;
		}
	}
	return message_result::unseen;
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
			auto res = it->second.generator(state, it->second.defintion);
			if(res) {
				std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.defintion]), sizeof(ui::element_data));
				make_size_from_graphics(state, res->base_data);
				res->on_create(state);
				return res;
			}
		}
		return make_element_immediate(state, it->second.defintion);
	}
	return std::unique_ptr<element_base>{};
}

state::state() {
	root = std::make_unique<container_base>();
}


void window_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::window) {
		auto first_child = base_data.data.window.first_child;
		auto num_children = base_data.data.window.num_children;
		for(uint32_t i = num_children; i-- > 0; ) {
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

void window_element_base::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto location_abs = get_absolute_location(*this);
	if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
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

void piechart_element_base::generate_data_texture(sys::state& state) {
	memcpy(data_texture.data, get_colors(state).data(), resolution * channels);
	data_texture.data_updated = true;
}

void piechart_element_base::on_create(sys::state& state) noexcept {
	generate_data_texture(state);
}

void piechart_element_base::on_update(sys::state& state) noexcept {
	generate_data_texture(state);
}

void piechart_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	ogl::render_piechart(
		state,
		ogl::color_modification::none, 
		float(x - base_data.size.x), float(y), float(base_data.size.x * 2),
		data_texture
	);
}

template<class SrcT, class DemoT>
std::vector<uint8_t> demographic_piechart<SrcT, DemoT>::get_colors(sys::state& state) noexcept {
	std::vector<uint8_t> colors(resolution * channels);
	Cyto::Any obj_id = SrcT{};
	size_t i = 0;
	if(parent) {
		parent->impl_get(state, obj_id);
		if(obj_id.holds_type<dcon::province_id>()) {
			auto prov_id = any_cast<dcon::province_id>(obj_id);
			dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
			auto total_pops = state.world.province_get_demographics(prov_id, demographics::total);
			DemoT last_demo{};
			for_each_demo(state, [&](DemoT demo_id) {
				last_demo = demo_id;
				auto demo_fat_id = dcon::fatten(state.world, demo_id);
				auto demo_key = demographics::to_key(state, demo_fat_id.id);
				auto volume = state.world.province_get_demographics(prov_id, demo_key);
				auto slice_count = size_t(volume / total_pops * resolution);
				auto color = demo_fat_id.get_color();
				for(size_t j = 0; j < slice_count * channels; j += channels) {
					colors[j + i] = uint8_t(color & 0xFF);
					colors[j + i + 1] = uint8_t(color >> 8 & 0xFF);
					colors[j + i + 2] = uint8_t(color >> 16 & 0xFF);
				}
				i += slice_count * channels;
			});
			auto fat_last_culture = dcon::fatten(state.world, last_demo);
			auto last_cult_color = fat_last_culture.get_color();
			for(; i < colors.size(); i += channels) {
				colors[i] = uint8_t(last_cult_color & 0xFF);
				colors[i + 1] = uint8_t(last_cult_color >> 8 & 0xFF);
				colors[i + 2] = uint8_t(last_cult_color >> 16 & 0xFF);
			}
		}
	}
	return colors;
}

template<class RowWinT, class RowConT>
void standard_listbox_scrollbar<RowWinT, RowConT>::scale_to_parent() {
	base_data.size.y = parent->base_data.size.y;
	base_data.data.scrollbar.border_size = base_data.size;
	base_data.position.x = parent->base_data.size.x; // base_data.size.x / 3;

	left->base_data.position.y = parent->base_data.size.y - left->base_data.size.y;
	right->base_data.position.y = 0;
	track->base_data.size.y = parent->base_data.size.y - 2 * right->base_data.size.y;
	track->base_data.position.y = right->base_data.size.y;
	track->base_data.position.x = 5;
	slider->base_data.position.x = 0;
	settings.track_size = track->base_data.size.y - left->base_data.size.y;

	left->step_size = -settings.scaling_factor;
	right->step_size = -settings.scaling_factor;
}

template<class RowWinT, class RowConT>
void standard_listbox_scrollbar<RowWinT, RowConT>::on_value_change(sys::state& state, int32_t v) noexcept {
	static_cast<listbox_element_base<RowWinT, RowConT>*>(parent)->update(state);
}

template<class RowConT>
class wrapped_row_content {
public:
	RowConT content;
	wrapped_row_content() {
		content = RowConT{};
	}
	wrapped_row_content(RowConT con) {
		content = con;
	}
};

template<class RowConT>
message_result listbox_row_element_base<RowConT>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<wrapped_row_content<RowConT>>()) {
		content = any_cast<wrapped_row_content<RowConT>>(payload).content;
		update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowConT>
message_result listbox_row_element_base<RowConT>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	return parent->impl_on_scroll(state, x, y, amount, mods);
}

template<class RowConT>
message_result listbox_row_button_base<RowConT>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<wrapped_row_content<RowConT>>()) {
		content = any_cast<wrapped_row_content<RowConT>>(payload).content;
		update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowConT>
message_result listbox_row_button_base<RowConT>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	return parent->impl_on_scroll(state, x, y, amount, mods);
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::update(sys::state& state) {
	auto content_off_screen = int32_t(row_contents.size() - row_windows.size());
	int32_t scroll_pos = int32_t(list_scrollbar->scaled_value());
	if(content_off_screen <= 0) {
		list_scrollbar->set_visible(state, false);
		scroll_pos = 0;
	} else {
		list_scrollbar->change_settings(state, mutable_scrollbar_settings{
			0, content_off_screen, 0, 0, false
		});
		list_scrollbar->set_visible(state, true);
	}

	if(is_reversed()) {
		auto i = int32_t(row_contents.size()) - scroll_pos - 1;
		for(size_t rw_i = row_windows.size() - 1; rw_i > 0; rw_i--) {
			if(i >= 0) {
				Cyto::Any payload = wrapped_row_content<RowConT>{ row_contents[i--] };
				row_windows[rw_i]->impl_get(state, payload);
				row_windows[rw_i]->set_visible(state, true);
			} else {
				row_windows[rw_i]->set_visible(state, false);
			}
		}
	} else {
		auto i = size_t(scroll_pos);
		for(RowWinT* row_window : row_windows) {
			if(i < row_contents.size()) {
				Cyto::Any payload = wrapped_row_content<RowConT>{ row_contents[i++] };
				row_window->impl_get(state, payload);
				row_window->set_visible(state, true);
			} else {
				row_window->set_visible(state, false);
			}
		}
	}
}

template<class RowWinT, class RowConT>
message_result listbox_element_base<RowWinT, RowConT>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(row_contents.size() > row_windows.size()) {
		list_scrollbar->update_scaled_value(state, list_scrollbar->scaled_value() + std::clamp(-amount, -1.f, 1.f));
		update(state);
	}
	return message_result::consumed;
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
				ogl::render_bordered_rect(
					state,
					get_color_modification(false, false, true),
					gfx_def.type_dependant,
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			} else {
				ogl::render_textured_rect(
					state,
					get_color_modification(false, false, true),
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			}
		}
	}
	container_base::render(state, x, y);
}
template<class ItemWinT, class ItemConT>
void overlapping_listbox_element_base<ItemWinT, ItemConT>::update(sys::state& state) {
	auto spacing = int16_t(base_data.data.overlapping.spacing);
	if(base_data.get_element_type() == element_type::overlapping) {
		while(contents.size() > windows.size()) {
			auto ptr = make_element_by_type<ItemWinT>(state, get_row_element_name());
			if(subwindow_width <= 0) {
				subwindow_width = ptr->base_data.size.x;
			}
			windows.push_back(static_cast<ItemWinT*>(ptr.get()));
			add_child_to_front(std::move(ptr));
		}

		float size_ratio = float(contents.size() * (subwindow_width + spacing)) / float(base_data.size.x);
		int16_t offset = spacing + subwindow_width;
		if(size_ratio > 1.f) {
			offset = int16_t(float(subwindow_width) / size_ratio);
		}
		int16_t current_x = 0;
		if(base_data.data.overlapping.image_alignment == alignment::right) {
			current_x = base_data.size.x - subwindow_width - offset * int16_t(contents.size() - 1);
		}
		for(size_t i = 0; i < windows.size(); i++) {
			if(i < contents.size()) {
				update_subwindow(state, windows[i], contents[i]);
				windows[i]->base_data.position.x = current_x;
				current_x += offset;
				windows[i]->set_visible(state, true);
			} else {
				windows[i]->set_visible(state, false);
			}
		}
	}
}

void overlapping_sphere_flags::populate_flags(sys::state& state) {
	if(bool(current_nation)) {
		contents.clear();
		int32_t sphereling_count = 0;  // this is a hack that's only getting used because checking if a nation is a GP doesn't work yet.
		state.world.for_each_nation([&](dcon::nation_id other) {
			auto other_fat = dcon::fatten(state.world, other);
			if(other_fat.get_in_sphere_of().id == current_nation) {
				contents.push_back(other_fat.get_identity_from_identity_holder().id);
				sphereling_count++;
			}
		});
		if(sphereling_count <= 0) {
			auto fat_id = dcon::fatten(state.world, current_nation);
			auto sphere_lord = fat_id.get_in_sphere_of();
			if(sphere_lord.id) {
				contents.push_back(sphere_lord.get_identity_from_identity_holder().id);
			}
		}
		update(state);
	}
}

void overlapping_sphere_flags::on_update(sys::state& state) noexcept {
	populate_flags(state);
}

message_result overlapping_sphere_flags::set(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<dcon::nation_id>()) {
		current_nation = any_cast<dcon::nation_id>(payload);
		populate_flags(state);
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

dcon::national_identity_id flag_button::get_current_nation(sys::state& state) noexcept {
	Cyto::Any payload = dcon::nation_id{};
	if(parent != nullptr) {
		parent->impl_get(state, payload);
		auto nation = any_cast<dcon::nation_id>(payload);
		auto fat_nation = dcon::fatten(state.world, nation);
		return fat_nation.get_identity_from_identity_holder().id;
	} else {
		return dcon::national_identity_id{};
	}
}

void flag_button::button_action(sys::state& state) noexcept {
	auto fat_id = dcon::fatten(state.world, get_current_nation(state));
	auto nation = fat_id.get_nation_from_identity_holder();
	if(bool(nation.id)) {
		state.open_diplomacy(nation.id);
	}
}

void flag_button::set_current_nation(sys::state& state, dcon::national_identity_id identity) noexcept {
	if(bool(identity)) {
		auto fat_id = dcon::fatten(state.world, identity);
		auto nation = fat_id.get_nation_from_identity_holder();
		culture::flag_type flag_type = culture::flag_type{};
		if(bool(nation.id)) {
			flag_type = culture::get_current_flag_type(state, nation.id);
		} else {
			flag_type = culture::get_current_flag_type(state, identity);
		}
		flag_texture_handle = ogl::get_flag_handle(state, identity, flag_type);
	}
}

void flag_button::on_update(sys::state& state) noexcept {
	set_current_nation(state, get_current_nation(state));
}

void flag_button::on_create(sys::state& state) noexcept {
	button_element_base::on_create(state);
	on_update(state);
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
		if(gfx_def.type_dependant) {
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependant - 1), true);
			ogl::render_masked_rect(
				state,
				get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				flag_texture_handle,
				mask_handle,
				base_data.get_rotation(),
				gfx_def.is_vertically_flipped()
			);
		} else {
			ogl::render_textured_rect(
				state,
				get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
				float(x), float(y), float(base_data.size.x), float(base_data.size.y),
				flag_texture_handle,
				base_data.get_rotation(),
				gfx_def.is_vertically_flipped()
			);
		}
	}
	button_element_base::render(state, x, y);
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
	if(parent) {
		Cyto::Any payload = value_change{ -step_size, true, true };
		parent->impl_get(state, payload);
	}
}
void scrollbar_right::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = value_change{ step_size, true, true };
		parent->impl_get(state, payload);
	}
}

message_result scrollbar_track::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(parent) {
		Cyto::Any payload = scrollbar_settings{};
		if(parent->impl_get(state, payload) == message_result::consumed) {
			scrollbar_settings parent_state = any_cast<scrollbar_settings>(payload);
			int32_t pos_in_track = parent_state.vertical ? y : x;
			int32_t clamped_pos = std::clamp(pos_in_track, parent_state.buttons_size / 2, parent_state.track_size - parent_state.buttons_size / 2);
			float fp_pos = float(clamped_pos - parent_state.buttons_size / 2) / float(parent_state.track_size - parent_state.buttons_size);
			Cyto::Any adjustment_payload = value_change{
				int32_t(parent_state.lower_value + fp_pos * (parent_state.upper_value - parent_state.lower_value)),
				true, false };
			parent->impl_get(state, adjustment_payload);
		}
	}
	return message_result::consumed;
}

message_result scrollbar_slider::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return message_result::consumed;
}
void scrollbar_slider::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(!parent)
		return;

	auto location_abs = get_absolute_location(*this);

	scrollbar_settings parent_settings = [&]() {
		Cyto::Any payload = scrollbar_settings{};
		if(parent->impl_get(state, payload) == message_result::consumed) {
			return any_cast<scrollbar_settings>(payload);
		}
		return scrollbar_settings{};
	}();

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

	if(parent_settings.vertical) {
		base_data.position.y += int16_t(y - oldy);
		base_data.position.y = int16_t(std::clamp(int32_t(base_data.position.y), parent_settings.buttons_size, parent_settings.track_size));
		pos_in_track = base_data.position.y - parent_settings.buttons_size / 2;
	} else {
		base_data.position.x += int16_t(x - oldx);
		base_data.position.x = int16_t(std::clamp(int32_t(base_data.position.x), parent_settings.buttons_size, parent_settings.track_size));
		pos_in_track = base_data.position.x - parent_settings.buttons_size / 2;
	}
	float fp_pos = float(pos_in_track - parent_settings.buttons_size / 2) / float(parent_settings.track_size - parent_settings.buttons_size);

	Cyto::Any adjustment_payload = value_change{
		int32_t(parent_settings.lower_value + fp_pos * (parent_settings.upper_value - parent_settings.lower_value)),
		false, false };
	parent->impl_get(state, adjustment_payload);
}


void scrollbar::update_raw_value(sys::state& state, int32_t v) {
	// TODO: adjust to limits if using limits
	stored_value = std::clamp(v, settings.lower_value, settings.upper_value);
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
	int32_t rv = std::clamp(int32_t(v * settings.scaling_factor), settings.lower_value, settings.upper_value);
	update_raw_value(state, rv);
}
float scrollbar::scaled_value() const {
	return float(stored_value) / float(settings.scaling_factor);
}

void scrollbar::change_settings(sys::state& state, mutable_scrollbar_settings const& settings_s) {
	settings.lower_limit = settings_s.lower_limit * settings.scaling_factor;
	settings.lower_value = settings_s.lower_value * settings.scaling_factor;
	settings.upper_limit = settings_s.upper_limit * settings.scaling_factor;
	settings.upper_value = settings_s.upper_value * settings.scaling_factor;
	settings.using_limits = settings_s.using_limits;

	settings.upper_value = std::max(settings.upper_value, settings.lower_value + 1); // ensure the scrollbar is never of range zero

	// TODO: adjust to limits if using limits
	if(stored_value < settings.lower_value || stored_value > settings.upper_value) {
		update_raw_value(state, stored_value);
		on_value_change(state, stored_value);
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
			case step_size::one:
				break;
			case step_size::two:
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
				if(step_size::two == step)
					left->step_size = 2;
				else
					left->step_size = 1;
			}
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(1 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_right>(state, child_tag);
				right = ch_res.get();
				add_child_to_back(std::move(ch_res));

				if(step_size::two == step)
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
				//track->base_data.position.x = 0;
				slider->base_data.position.x = 0;
				right->base_data.position.x = 0;
			} else {
				track->base_data.position.x = int16_t(settings.buttons_size);
				slider->base_data.position.x = int16_t(settings.buttons_size);
				right->base_data.position.x = int16_t(settings.track_size + settings.buttons_size);
				//track->base_data.position.y = 0;
				slider->base_data.position.y = 0;
				right->base_data.position.y = 0;
			}
		}
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
			// TODO: adjust to limits if using limits
			stored_value = std::clamp(stored_value, settings.lower_value, settings.upper_value);
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

}
