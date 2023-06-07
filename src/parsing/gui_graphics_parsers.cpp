#include "gui_graphics.hpp"
#include "parsers_declarations.hpp"
#include "system_state.hpp"
#include "fonts.hpp"

namespace parsers {
struct obj_and_horizontal {
	ui::gfx_object* obj = nullptr;
	bool horizontal = false;
};
obj_and_horizontal common_create_object(gfx_object const& obj_in, building_gfx_context& context) {
	auto gfxindex = context.ui_defs.gfx.size();
	context.ui_defs.gfx.emplace_back();
	ui::gfx_object& new_obj = context.ui_defs.gfx.back();

	context.map_of_names.insert_or_assign(std::string(obj_in.name), dcon::gfx_object_id(uint16_t(gfxindex)));

	if(obj_in.allwaystransparent) {
		new_obj.flags |= ui::gfx_object::always_transparent;
	}
	if(obj_in.clicksound_set) {
		new_obj.flags |= ui::gfx_object::has_click_sound;
	}
	if(obj_in.flipv) {
		new_obj.flags |= ui::gfx_object::flip_v;
	}
	if(obj_in.transparencecheck) {
		new_obj.flags |= ui::gfx_object::do_transparency_check;
	}

	new_obj.number_of_frames = uint8_t(obj_in.noofframes);

	if(obj_in.primary_texture.length() > 0) {
		auto stripped = simple_fs::remove_double_backslashes(obj_in.primary_texture);
		if(auto it = context.map_of_texture_names.find(std::string(stripped)); it != context.map_of_texture_names.end()) {
			new_obj.primary_texture_handle = it->second;
		} else {
			auto index = context.ui_defs.textures.size();
			context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(stripped));
			new_obj.primary_texture_handle = dcon::texture_id(uint16_t(index));
			context.map_of_texture_names.insert_or_assign(stripped, dcon::texture_id(uint16_t(index)));
		}
	}
	if(obj_in.secondary_texture.length() > 0) {
		auto stripped = simple_fs::remove_double_backslashes(obj_in.secondary_texture);
		if(auto it = context.map_of_texture_names.find(stripped); it != context.map_of_texture_names.end()) {
			new_obj.type_dependent = uint16_t(it->second.index() + 1);
		} else {
			auto index = context.ui_defs.textures.size();
			context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(stripped));
			new_obj.type_dependent = uint16_t(index + 1);
			context.map_of_texture_names.insert_or_assign(stripped, dcon::texture_id(uint16_t(index)));
		}
	}

	if(obj_in.size) {
		new_obj.size.x = int16_t(*obj_in.size);
		new_obj.size.y = int16_t(*obj_in.size);
	}
	if(obj_in.size_obj) {
		new_obj.size.x = int16_t(obj_in.size_obj->x);
		new_obj.size.y = int16_t(obj_in.size_obj->y);
	}
	if(obj_in.bordersize) {
		new_obj.type_dependent = uint16_t(obj_in.bordersize->x);
	}

	return obj_and_horizontal{&new_obj, obj_in.horizontal};
}

void gfx_object_outer::spritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::generic_sprite);
}
void gfx_object_outer::corneredtilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::bordered_rect);
}
void gfx_object_outer::maskedshieldtype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::flag_mask);
}
void gfx_object_outer::textspritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::text_sprite);
}
void gfx_object_outer::tilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::tile_sprite);
}
void gfx_object_outer::progressbartype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	if(res.horizontal)
		res.obj->flags |= uint8_t(ui::object_type::horizontal_progress_bar);
	else
		res.obj->flags |= uint8_t(ui::object_type::vertical_progress_bar);
}
void gfx_object_outer::barcharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::barchart);
}
void gfx_object_outer::piecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::piechart);
}
void gfx_object_outer::linecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
	auto res = common_create_object(obj, context);
	res.obj->flags |= uint8_t(ui::object_type::linegraph);
}

void gfx_add_obj(parsers::token_generator& gen, parsers::error_handler& err, building_gfx_context& context) { parsers::parse_gfx_object_outer(gen, err, context); }

void assign(ui::xy_pair& a, parsers::gfx_xy_pair const& b) {
	a.x = int16_t(b.x);
	a.y = int16_t(b.y);
}

void gui_element_common::size(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context) { assign(target.size, pr); }
void gui_element_common::position(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context) { assign(target.position, pr); }

void gui_element_common::orientation(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) {
	if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "center") || is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "centre")) {
		target.flags |= uint8_t(ui::orientation::center);
	} else if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "lower_left")) {
		target.flags |= uint8_t(ui::orientation::lower_left);
	} else if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "lower_right")) {
		target.flags |= uint8_t(ui::orientation::lower_right);
	} else if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "upper_left") || is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "upperl_left")) {
		target.flags |= uint8_t(ui::orientation::upper_left);
	} else if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "upper_right")) {
		target.flags |= uint8_t(ui::orientation::upper_right);
	} else if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "center_up")) {
		target.flags |= uint8_t(ui::orientation::upper_center);
	} else if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "center_down")) {
		target.flags |= uint8_t(ui::orientation::lower_center);
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(txt) + " as an orientation on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void gui_element_common::name(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { target.name = context.full_state.add_to_pool_lowercase(txt); }
void gui_element_common::rotation(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) {
	if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "-1.5708")) {
		target.flags |= uint8_t(ui::rotation::r90_right);
	} else if(is_fixed_token_ci(txt.data(), txt.data() + txt.length(), "1.5708")) {
		target.flags |= uint8_t(ui::rotation::r90_left);
	} else if(parse_float(txt, line, err) == 0.0f) {
		target.flags |= uint8_t(ui::rotation::upright);
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(txt) + " as a rotation on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void gui_element_common::maxwidth(association_type, int32_t v, error_handler& err, int32_t line, building_gfx_context& context) { target.size.x = int16_t(v); }
void gui_element_common::maxheight(association_type, int32_t v, error_handler& err, int32_t line, building_gfx_context& context) { target.size.y = int16_t(v); }

button::button() {
	target.data.button = ui::button_data{};
	target.flags = uint8_t(ui::element_type::button);
}
void button::spritetype(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) {
	if(txt.length() == 0)
		return;
	auto it = context.map_of_names.find(std::string(txt));
	if(it != context.map_of_names.end()) {
		target.data.button.button_image = it->second;
	} else {
		err.accumulated_errors += "referenced unknown gfx object  " + std::string(txt) + " on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}

void button::shortcut(association_type, std::string_view t, error_handler& err, int32_t line, building_gfx_context& context) {
	if(t.length() == size_t(0))
		target.data.button.shortcut = sys::virtual_key::NONE;
	else if(t.length() == size_t(1)) {
		if((t[0] >= 'a') && (t[0] <= 'z')) {
			target.data.button.shortcut = sys::virtual_key((uint8_t)sys::virtual_key::A + (t[0] - 'a'));
		} else if((t[0] >= 'A') && (t[0] <= 'Z')) {
			target.data.button.shortcut = sys::virtual_key((uint8_t)sys::virtual_key::A + (t[0] - 'A'));
		} else if((t[0] >= '0') && (t[0] <= '9')) {
			target.data.button.shortcut = sys::virtual_key((uint8_t)sys::virtual_key::NUM_0 + (t[0] - '0'));
		} else if(t[0] == ':') {
			target.data.button.shortcut = sys::virtual_key::SEMICOLON;
		} else if(t[0] == ';') {
			target.data.button.shortcut = sys::virtual_key::SEMICOLON;
		} else if(t[0] == '+') {
			target.data.button.shortcut = sys::virtual_key::PLUS;
		} else if(t[0] == '=') {
			target.data.button.shortcut = sys::virtual_key::PLUS;
		} else if(t[0] == '<') {
			target.data.button.shortcut = sys::virtual_key::COMMA;
		} else if(t[0] == ',') {
			target.data.button.shortcut = sys::virtual_key::COMMA;
		} else if(t[0] == '-') {
			target.data.button.shortcut = sys::virtual_key::MINUS;
		} else if(t[0] == '_') {
			target.data.button.shortcut = sys::virtual_key::MINUS;
		} else if(t[0] == '.') {
			target.data.button.shortcut = sys::virtual_key::PERIOD;
		} else if(t[0] == '>') {
			target.data.button.shortcut = sys::virtual_key::PERIOD;
		} else if(t[0] == '?') {
			target.data.button.shortcut = sys::virtual_key::FORWARD_SLASH;
		} else if(t[0] == '/') {
			target.data.button.shortcut = sys::virtual_key::FORWARD_SLASH;
		} else if(t[0] == '~') {
			target.data.button.shortcut = sys::virtual_key::TILDA;
		} else if(t[0] == '`') {
			target.data.button.shortcut = sys::virtual_key::TILDA;
		} else if(t[0] == '{') {
			target.data.button.shortcut = sys::virtual_key::OPEN_BRACKET;
		} else if(t[0] == '[') {
			target.data.button.shortcut = sys::virtual_key::OPEN_BRACKET;
		} else if(t[0] == '|') {
			target.data.button.shortcut = sys::virtual_key::BACK_SLASH;
		} else if(t[0] == '\\') {
			target.data.button.shortcut = sys::virtual_key::BACK_SLASH;
		} else if(t[0] == '}') {
			target.data.button.shortcut = sys::virtual_key::CLOSED_BRACKET;
		} else if(t[0] == ']') {
			target.data.button.shortcut = sys::virtual_key::CLOSED_BRACKET;
		} else if(t[0] == '\'') {
			target.data.button.shortcut = sys::virtual_key::QUOTE;
		} else if(t[0] == '\"') {
			target.data.button.shortcut = sys::virtual_key::QUOTE;
		} else {
			err.accumulated_errors += "tried to parse  " + std::string(t) + " as a key name on line " + std::to_string(line) + " of file " + err.file_name + "\n";
		}
	} else {
		if(is_fixed_token_ci(t.data(), t.data() + t.length(), "none")) {
			target.data.button.shortcut = sys::virtual_key::NONE;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "backspace")) {
			target.data.button.shortcut = sys::virtual_key::BACK;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "back")) {
			target.data.button.shortcut = sys::virtual_key::BACK;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "tab")) {
			target.data.button.shortcut = sys::virtual_key::TAB;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "return")) {
			target.data.button.shortcut = sys::virtual_key::RETURN;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "enter")) {
			target.data.button.shortcut = sys::virtual_key::RETURN;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "esc")) {
			target.data.button.shortcut = sys::virtual_key::ESCAPE;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "escape")) {
			target.data.button.shortcut = sys::virtual_key::ESCAPE;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "space")) {
			target.data.button.shortcut = sys::virtual_key::SPACE;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "page_up")) {
			target.data.button.shortcut = sys::virtual_key::PRIOR;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "page_down")) {
			target.data.button.shortcut = sys::virtual_key::NEXT;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "end")) {
			target.data.button.shortcut = sys::virtual_key::END;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "home")) {
			target.data.button.shortcut = sys::virtual_key::HOME;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "left")) {
			target.data.button.shortcut = sys::virtual_key::LEFT;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "up")) {
			target.data.button.shortcut = sys::virtual_key::UP;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "right")) {
			target.data.button.shortcut = sys::virtual_key::RIGHT;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "down")) {
			target.data.button.shortcut = sys::virtual_key::DOWN;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "select")) {
			target.data.button.shortcut = sys::virtual_key::SELECT;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "insert")) {
			target.data.button.shortcut = sys::virtual_key::INSERT;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "delete")) {
			target.data.button.shortcut = sys::virtual_key::DELETE_KEY;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "del")) {
			target.data.button.shortcut = sys::virtual_key::DELETE_KEY;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f1")) {
			target.data.button.shortcut = sys::virtual_key::F1;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f2")) {
			target.data.button.shortcut = sys::virtual_key::F2;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f3")) {
			target.data.button.shortcut = sys::virtual_key::F3;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f4")) {
			target.data.button.shortcut = sys::virtual_key::F4;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f5")) {
			target.data.button.shortcut = sys::virtual_key::F5;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f6")) {
			target.data.button.shortcut = sys::virtual_key::F6;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f7")) {
			target.data.button.shortcut = sys::virtual_key::F7;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f8")) {
			target.data.button.shortcut = sys::virtual_key::F8;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f9")) {
			target.data.button.shortcut = sys::virtual_key::F9;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f10")) {
			target.data.button.shortcut = sys::virtual_key::F10;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f11")) {
			target.data.button.shortcut = sys::virtual_key::F11;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f12")) {
			target.data.button.shortcut = sys::virtual_key::F12;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f13")) {
			target.data.button.shortcut = sys::virtual_key::F13;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f14")) {
			target.data.button.shortcut = sys::virtual_key::F14;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f15")) {
			target.data.button.shortcut = sys::virtual_key::F15;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f16")) {
			target.data.button.shortcut = sys::virtual_key::F16;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f17")) {
			target.data.button.shortcut = sys::virtual_key::F17;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f18")) {
			target.data.button.shortcut = sys::virtual_key::F18;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f19")) {
			target.data.button.shortcut = sys::virtual_key::F19;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f20")) {
			target.data.button.shortcut = sys::virtual_key::F20;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f21")) {
			target.data.button.shortcut = sys::virtual_key::F21;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f22")) {
			target.data.button.shortcut = sys::virtual_key::F22;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f23")) {
			target.data.button.shortcut = sys::virtual_key::F23;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "f24")) {
			target.data.button.shortcut = sys::virtual_key::F24;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad0")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD0;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad1")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD1;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad2")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD2;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad3")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD3;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad4")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD4;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad5")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD5;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad6")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD6;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad7")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD7;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad8")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD8;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "numpad9")) {
			target.data.button.shortcut = sys::virtual_key::NUMPAD9;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "multiply")) {
			target.data.button.shortcut = sys::virtual_key::MULTIPLY;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "add")) {
			target.data.button.shortcut = sys::virtual_key::ADD;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "separator")) {
			target.data.button.shortcut = sys::virtual_key::SEPARATOR;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "subtract")) {
			target.data.button.shortcut = sys::virtual_key::SUBTRACT;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "decimal")) {
			target.data.button.shortcut = sys::virtual_key::DECIMAL;
		} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "divide")) {
			target.data.button.shortcut = sys::virtual_key::DIVIDE;
		} else {
			err.accumulated_errors += "tried to parse  " + std::string(t) + " as a key name on line " + std::to_string(line) + " of file " + err.file_name + "\n";
		}
	}
}

std::string lowercase_str(std::string_view sv) {
	std::string result;
	result.reserve(sv.length());
	for(auto ch : sv) {
		result += char(tolower(ch));
	}
	return result;
}

void button::buttontext(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) {
	auto it = context.full_state.key_to_text_sequence.find(lowercase_str(txt));
	if(it != context.full_state.key_to_text_sequence.end()) {
		target.data.button.txt = it->second;
	} else {
		auto new_key = context.full_state.add_to_pool_lowercase(txt);
		auto component_sz = context.full_state.text_components.size();
		context.full_state.text_components.push_back(new_key);
		auto seq_size = context.full_state.text_sequences.size();
		context.full_state.text_sequences.push_back(text::text_sequence{uint32_t(component_sz), uint16_t(1)});
		auto new_id = dcon::text_sequence_id(dcon::text_sequence_id::value_base_t(seq_size));
		target.data.button.txt = new_id;
		context.full_state.key_to_text_sequence.insert_or_assign(new_key, new_id);
	}
}

void button::buttonfont(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { target.data.button.font_handle = text::name_into_font_id(context.full_state, txt); }

void button::format(association_type, std::string_view t, error_handler& err, int32_t line, building_gfx_context& context) {
	if(is_fixed_token_ci(t.data(), t.data() + t.length(), "centre") || is_fixed_token_ci(t.data(), t.data() + t.length(), "center")) {
		target.data.button.flags |= uint8_t(ui::alignment::centered);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "left")) {
		target.data.button.flags |= uint8_t(ui::alignment::left);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "right")) {
		target.data.button.flags |= uint8_t(ui::alignment::right);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "justified")) {
		target.data.button.flags |= uint8_t(ui::alignment::justified);
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(t) + " as an alignment on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}

void button::clicksound(association_type, std::string_view t, error_handler& err, int32_t line, building_gfx_context& context) {
	if(is_fixed_token(t.data(), t.data() + t.length(), "click")) {
		target.data.button.flags |= uint8_t(ui::clicksound::click);
	} else if(is_fixed_token(t.data(), t.data() + t.length(), "close_window")) {
		target.data.button.flags |= uint8_t(ui::clicksound::close_window);
	} else if(is_fixed_token(t.data(), t.data() + t.length(), "start_game")) {
		target.data.button.flags |= uint8_t(ui::clicksound::start_game);
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(t) + " as a click sound on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}

image::image() {
	target.data.image = ui::image_data{};
	target.flags = uint8_t(ui::element_type::image);
}
void image::frame(association_type, uint32_t v, error_handler& err, int32_t line, building_gfx_context& context) {
	if(v <= 127) {
		target.data.image.flags |= uint8_t(v);
	} else {
		err.accumulated_errors += "asked for more than 127 frames on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void image::spritetype(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) {
	if(txt.length() == 0)
		return;
	auto it = context.map_of_names.find(std::string(txt));
	if(it != context.map_of_names.end()) {
		target.data.image.gfx_object = it->second;
	} else {
		err.accumulated_errors += "referenced unknown gfx object  " + std::string(txt) + " on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void image::scale(association_type, float v, error_handler& err, int32_t line, building_gfx_context& context) { target.data.image.scale = v; }

textbox::textbox() {
	target.data.text = ui::text_data{};
	target.flags = uint8_t(ui::element_type::text);
}

void textbox::bordersize(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context) { assign(target.data.text.border_size, pr); }
void textbox::fixedsize(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context) {
	if(v) {
		target.data.text.flags |= ui::text_data::is_fixed_size_mask;
	}
}
void textbox::font(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { target.data.text.font_handle = text::name_into_font_id(context.full_state, txt); }
void textbox::format(association_type, std::string_view t, error_handler& err, int32_t line, building_gfx_context& context) {
	if(is_fixed_token_ci(t.data(), t.data() + t.length(), "centre") || is_fixed_token_ci(t.data(), t.data() + t.length(), "center")) {
		target.data.text.flags |= uint8_t(ui::alignment::centered);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "left")) {
		target.data.text.flags |= uint8_t(ui::alignment::left);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "right")) {
		target.data.text.flags |= uint8_t(ui::alignment::right);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "justified")) {
		target.data.text.flags |= uint8_t(ui::alignment::justified);
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(t) + " as an alignment on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void textbox::text(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) {
	auto it = context.full_state.key_to_text_sequence.find(lowercase_str(txt));
	if(it != context.full_state.key_to_text_sequence.end()) {
		target.data.text.txt = it->second;
	} else {
		auto new_key = context.full_state.add_to_pool_lowercase(txt);
		auto component_sz = context.full_state.text_components.size();
		context.full_state.text_components.push_back(new_key);
		auto seq_size = context.full_state.text_sequences.size();
		context.full_state.text_sequences.push_back(text::text_sequence{uint32_t(component_sz), uint16_t(1)});
		auto new_id = dcon::text_sequence_id(dcon::text_sequence_id::value_base_t(seq_size));
		target.data.text.txt = new_id;
		context.full_state.key_to_text_sequence.insert_or_assign(new_key, new_id);
	}
}
void textbox::texturefile(association_type, std::string_view t, error_handler& err, int32_t line, building_gfx_context& context) {
	if(t.length() == 0) {
		target.data.text.flags |= uint8_t(ui::text_background::none);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "gfx\\\\interface\\\\tiles_dialog.tga")) {
		target.data.text.flags |= uint8_t(ui::text_background::tiles_dialog);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "gfx\\\\interface\\\\transparency.tga")) {
		target.data.text.flags |= uint8_t(ui::text_background::transparency);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "gfx\\\\interface\\\\small_tiles_dialog.dds")) {
		target.data.text.flags |= uint8_t(ui::text_background::small_tiles_dialog);
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(t) + " as a text box background on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}

listbox::listbox() {
	target.data.list_box = ui::list_box_data{};
	target.flags = uint8_t(ui::element_type::listbox);
}

void listbox::background(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) {
	if(txt.length() == 0)
		return;
	auto it = context.map_of_names.find(std::string(txt));
	if(it != context.map_of_names.end()) {
		target.data.list_box.background_image = it->second;
	} else {
		err.accumulated_errors += "referenced unknown gfx object  " + std::string(txt) + " on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void listbox::bordersize(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context) { assign(target.data.list_box.border_size, pr); }
void listbox::offset(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context) { assign(target.data.list_box.offset, pr); }
void listbox::spacing(association_type, uint32_t v, error_handler& err, int32_t line, building_gfx_context& context) { target.data.list_box.spacing = uint8_t(v); }

overlapping::overlapping() {
	target.data.overlapping = ui::overlapping_data{};
	target.flags = uint8_t(ui::element_type::overlapping);
}
void overlapping::format(association_type, std::string_view t, error_handler& err, int32_t line, building_gfx_context& context) {
	if(is_fixed_token_ci(t.data(), t.data() + t.length(), "centre") || is_fixed_token_ci(t.data(), t.data() + t.length(), "center")) {
		target.data.overlapping.image_alignment = ui::alignment::centered;
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "left")) {
		target.data.overlapping.image_alignment = ui::alignment::left;
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "right")) {
		target.data.overlapping.image_alignment = ui::alignment::right;
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "justified")) {
		target.data.overlapping.image_alignment = ui::alignment::justified;
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(t) + " as an alignment on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void overlapping::spacing(association_type, float v, error_handler& err, int32_t line, building_gfx_context& context) { target.data.overlapping.spacing = v; }

scrollbar::scrollbar() {
	target.data.scrollbar = ui::scrollbar_data{};
	target.flags = uint8_t(ui::element_type::scrollbar);
}
void scrollbar::horizontal(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context) {
	if(v)
		target.data.scrollbar.flags |= ui::scrollbar_data::is_horizontal_mask;
}
void scrollbar::leftbutton(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { leftbutton_ = txt; }
void scrollbar::rightbutton(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { rightbutton_ = txt; }
void scrollbar::rangelimitmaxicon(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { rangelimitmaxicon_ = txt; }
void scrollbar::rangelimitminicon(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { rangelimitminicon_ = txt; }
void scrollbar::slider(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { slider_ = txt; }
void scrollbar::track(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context) { track_ = txt; }
void scrollbar::lockable(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context) {
	if(v)
		target.data.scrollbar.flags |= ui::scrollbar_data::is_lockable_mask;
}
void scrollbar::userangelimit(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context) {
	if(v)
		target.data.scrollbar.flags |= ui::scrollbar_data::is_range_limited_mask;
}
void scrollbar::maxvalue(association_type, uint32_t v, error_handler& err, int32_t line, building_gfx_context& context) { target.data.scrollbar.max_value = uint16_t(v); }
void scrollbar::stepsize(association_type, std::string_view t, error_handler& err, int32_t line, building_gfx_context& context) {
	if(is_fixed_token_ci(t.data(), t.data() + t.length(), "1")) {
		target.data.scrollbar.flags |= uint8_t(ui::step_size::one);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "2")) {
		target.data.scrollbar.flags |= uint8_t(ui::step_size::two);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "0.1")) {
		target.data.scrollbar.flags |= uint8_t(ui::step_size::one_tenth);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "0.01")) {
		target.data.scrollbar.flags |= uint8_t(ui::step_size::one_hundredth);
	} else if(is_fixed_token_ci(t.data(), t.data() + t.length(), "0.001")) {
		target.data.scrollbar.flags |= uint8_t(ui::step_size::one_thousandth);
	} else {
		err.accumulated_errors += "tried to parse  " + std::string(t) + " as a step size on line " + std::to_string(line) + " of file " + err.file_name + "\n";
	}
}
void scrollbar::bordersize(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context) { assign(target.data.scrollbar.border_size, pr); }
void scrollbar::guibuttontype(button const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void scrollbar::icontype(image const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
auto find_in_children(std::string_view name, std::vector<ui::element_data> const& v, sys::state const& state) {
	auto lname = lowercase_str(name);
	for(size_t i = v.size(); i-- > 0;) {
		if(state.to_string_view(v[i].name) == lname) {
			return i;
		}
	}
	return size_t(-1);
}
void scrollbar::finish(building_gfx_context& context) {
	if(auto fr = find_in_children(leftbutton_, children, context.full_state); fr != size_t(-1) && leftbutton_.length() > 0 && children.size() > 0) {
		if(fr != 0) {
			std::swap(children[0], children[fr]);
		}
	}
	if(auto fr = find_in_children(rightbutton_, children, context.full_state); fr != size_t(-1) && rightbutton_.length() > 0 && children.size() > 1) {
		if(fr != 1) {
			std::swap(children[1], children[fr]);
		}
	}
	if(auto fr = find_in_children(slider_, children, context.full_state); fr != size_t(-1) && slider_.length() > 0 && children.size() > 2) {
		if(fr != 2) {
			std::swap(children[2], children[fr]);
		}
	}
	if(auto fr = find_in_children(track_, children, context.full_state); fr != size_t(-1) && track_.length() > 0 && children.size() > 3) {
		if(fr != 3) {
			std::swap(children[3], children[fr]);
		}
	}
	if(auto fr = find_in_children(rangelimitmaxicon_, children, context.full_state); fr != size_t(-1) && rangelimitmaxicon_.length() > 0 && children.size() > 4) {
		if(fr != 4) {
			std::swap(children[4], children[fr]);
		}
	}
	if(auto fr = find_in_children(rangelimitminicon_, children, context.full_state); fr != size_t(-1) && rangelimitminicon_.length() > 0 && children.size() > 5) {
		if(fr != 5) {
			std::swap(children[5], children[fr]);
		}
	}
	auto first_child = context.full_state.ui_defs.gui.size();
	for(auto& ch : children) {
		context.full_state.ui_defs.gui.push_back(ch);
	}
	target.data.scrollbar.num_children = uint8_t(children.size());
	target.data.scrollbar.first_child = dcon::gui_def_id(dcon::gui_def_id::value_base_t(first_child));
}

window::window() {
	target.data.window = ui::window_data{};
	target.flags = uint8_t(ui::element_type::window);
}
void window::fullscreen(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context) {
	if(v)
		target.data.window.flags |= ui::window_data::is_fullscreen_mask;
}
void window::moveable(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context) {
	if(v)
		target.data.window.flags |= ui::window_data::is_moveable_mask;
}

void window::guibuttontype(button const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::icontype(image const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::eu3dialogtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	children.push_back(v.target);
	children.back().data.window.flags |= ui::window_data::is_dialog_mask;
}
void window::instanttextboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	children.push_back(v.target);
	children.back().data.text.flags |= ui::text_data::is_instant_mask;
}
void window::listboxtype(listbox const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::positiontype(gui_element_common const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::scrollbartype(scrollbar const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::windowtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::checkboxtype(button const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	children.push_back(v.target);
	children.back().data.button.flags |= ui::button_data::is_checkbox_mask;
}
void window::shieldtype(image const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	children.push_back(v.target);
	children.back().data.image.flags |= ui::image_data::is_mask_mask;
}
void window::overlappingelementsboxtype(overlapping const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::editboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	children.push_back(v.target);
	children.back().data.text.flags |= ui::text_data::is_edit_mask;
}
void window::textboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context) { children.push_back(v.target); }
void window::finish(building_gfx_context& context) {
	auto first_child = context.full_state.ui_defs.gui.size();
	for(auto& ch : children) {
		context.full_state.ui_defs.gui.push_back(ch);
	}
	target.data.window.num_children = uint8_t(children.size());
	target.data.window.first_child = dcon::gui_def_id(dcon::gui_def_id::value_base_t(first_child));
}

void guitypes::guibuttontype(button const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::icontype(image const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::eu3dialogtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().data.window.flags |= ui::window_data::is_dialog_mask;
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::instanttextboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().data.text.flags |= ui::text_data::is_instant_mask;
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::listboxtype(listbox const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::positiontype(gui_element_common const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::scrollbartype(scrollbar const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::windowtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::checkboxtype(button const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().data.button.flags |= ui::button_data::is_checkbox_mask;
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::shieldtype(image const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().data.image.flags |= ui::image_data::is_mask_mask;
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::overlappingelementsboxtype(overlapping const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::editboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().data.text.flags |= ui::text_data::is_edit_mask;
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}
void guitypes::textboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context) {
	context.full_state.ui_defs.gui.push_back(v.target);
	context.full_state.ui_defs.gui.back().ex_flags |= ui::element_data::ex_is_top_level;
}

} // namespace parsers
