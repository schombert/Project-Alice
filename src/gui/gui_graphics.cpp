#include "gui_graphics.hpp"
#include "simple_fs.hpp"
#include "gui_element_base.hpp"

namespace ui {

void load_text_gui_definitions(sys::state& state, parsers::building_gfx_context& context, parsers::error_handler& err) {
	// preload special background textures
	assert(context.ui_defs.textures.size() == size_t(0));
	{
		auto stripped_name = simple_fs::remove_double_backslashes(std::string_view("gfx\\interface\\small_tiles_dialog.tga"));
		context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(stripped_name));
		context.map_of_texture_names.insert_or_assign(stripped_name, definitions::small_tiles_dialog);
	}
	{
		auto stripped_name = simple_fs::remove_double_backslashes(std::string_view("gfx\\interface\\tiles_dialog.tga"));
		context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(stripped_name));
		context.map_of_texture_names.insert_or_assign(stripped_name, definitions::tiles_dialog);
	}
	{
		auto stripped_name = simple_fs::remove_double_backslashes(std::string_view("gfx\\interface\\transparency.tga"));
		context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(stripped_name));
		context.map_of_texture_names.insert_or_assign(stripped_name, definitions::transparency);
	}

	auto rt = get_root(state.common_fs);
	auto interfc = open_directory(rt, NATIVE("interface"));

	{
		// first, load in special mod gfx
		// TODO put this in a better location
		auto alice_gfx = open_file(rt, NATIVE("assets/alice.gfx"));
		if(alice_gfx) {
			auto content = view_contents(*alice_gfx);
			err.file_name = "assets/alice.gfx";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_gfx_files(gen, err, context);
		}

		auto all_files = list_files(interfc, NATIVE(".gfx"));

		for(auto& file : all_files) {
			auto ofile = open_file(file);
			if(ofile) {
				auto content = view_contents(*ofile);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_gfx_files(gen, err, context);
			}
		}
	}

	{
		// first, load in special mod gui
		// TODO put this in a better location
		auto alice_gui = open_file(rt, NATIVE("assets/alice.gui"));
		if(alice_gui) {
			auto content = view_contents(*alice_gui);
			err.file_name = "assets/alice.gui";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_gui_files(gen, err, context);
		}

		// load normal .gui files
		auto all_gui_files = list_files(interfc, NATIVE(".gui"));

		for(auto& file : all_gui_files) {
			auto file_name = get_full_name(file);
			if(!parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("confirmbuild.gui")) && !parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("convoys.gui")) && !parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("brigadeview.gui"))) {
				auto ofile = open_file(file);
				if(ofile) {
					auto content = view_contents(*ofile);
					err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
					parsers::token_generator gen(content.data, content.data + content.file_size);
					parsers::parse_gui_files(gen, err, context);
				}
			}
		}
	}
}

xy_pair child_relative_location(element_base const & parent, element_base const & child) {
	switch(child.base_data.get_orientation()) {
	case orientation::upper_left:
		return xy_pair{int16_t(child.base_data.position.x), int16_t(child.base_data.position.y)};
	case orientation::upper_right:
		return xy_pair{int16_t(parent.base_data.size.x + child.base_data.position.x), int16_t(child.base_data.position.y)};
	case orientation::lower_left:
		return xy_pair{int16_t(child.base_data.position.x), int16_t(parent.base_data.size.y + child.base_data.position.y)};
	case orientation::lower_right:
		return xy_pair{int16_t(parent.base_data.size.x + child.base_data.position.x), int16_t(parent.base_data.size.y + child.base_data.position.y)};
	case orientation::upper_center:
		return xy_pair{int16_t(parent.base_data.size.x / 2 + child.base_data.position.x), int16_t(child.base_data.position.y)};
	case orientation::lower_center:
		return xy_pair{int16_t(parent.base_data.size.x / 2 + child.base_data.position.x), int16_t(parent.base_data.size.y + child.base_data.position.y)};
	case orientation::center:
		return xy_pair{int16_t(parent.base_data.size.x / 2 + child.base_data.position.x), int16_t(parent.base_data.size.y / 2 + child.base_data.position.y)};
	default:
		return xy_pair{int16_t(child.base_data.position.x), int16_t(child.base_data.position.y)};
	}
}

uint8_t element_base::get_pixel_opacity(sys::state& state, int32_t x, int32_t y, dcon::texture_id tid) {
	uint8_t* pixels = state.open_gl.asset_textures[tid].data;
	int32_t width = state.open_gl.asset_textures[tid].size_x;
	int32_t stride = state.open_gl.asset_textures[tid].channels;
	if(pixels && 0 <= x && x < width && 0 <= y && y < state.open_gl.asset_textures[tid].size_y)
		return pixels[(y * width * stride) + (x * stride) + stride - 1];
	else
		return 0;
}

mouse_probe element_base::impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept {
	mouse_probe probe_result = mouse_probe{nullptr, xy_pair{int16_t(x), int16_t(y)}};
	if(0 <= x && x < base_data.size.x && 0 <= y && y < base_data.size.y && test_mouse(state, x, y, type) == message_result::consumed) {
		auto elem_type = base_data.get_element_type();
		if(elem_type == element_type::button || elem_type == element_type::image || elem_type == element_type::listbox) {
			dcon::gfx_object_id gfx_id;
			if(elem_type == element_type::button) {
				gfx_id = base_data.data.button.button_image;
			} else if(elem_type == element_type::image) {
				gfx_id = base_data.data.image.gfx_object;
			} else if(elem_type == element_type::listbox) {
				gfx_id = base_data.data.list_box.background_image;
			}
			auto& gfx_def = state.ui_defs.gfx[gfx_id];
			auto mask_handle = gfx_def.type_dependent;
			if(gfx_def.is_partially_transparent() && gfx_def.primary_texture_handle && get_pixel_opacity(state, x, y, gfx_def.primary_texture_handle)) {
				probe_result.under_mouse = this;
			} else if(gfx_def.get_object_type() == ui::object_type::flag_mask && mask_handle && gfx_def.primary_texture_handle) {
				ogl::get_texture_handle(state, dcon::texture_id(mask_handle - 1), true);
				if(get_pixel_opacity(state, x, y, dcon::texture_id(mask_handle - 1))) {
					probe_result.under_mouse = this;
				}
			} else {
				probe_result.under_mouse = this;
			}
		} else {
			probe_result.under_mouse = this;
		}
	}
	return probe_result;
}
message_result element_base::impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return on_lbutton_down(state, x, y, mods);
}
message_result element_base::impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return on_rbutton_down(state, x, y, mods);
}
message_result element_base::impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	return on_key_down(state, key, mods);
}
message_result element_base::impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	return on_scroll(state, x, y, amount, mods);
}
message_result element_base::impl_on_mouse_move(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return on_mouse_move(state, x, y, mods);
}
void element_base::impl_on_update(sys::state& state) noexcept {
	on_update(state);
}
void element_base::impl_on_reset_text(sys::state& state) noexcept {
	on_reset_text(state);
}
message_result element_base::impl_get(sys::state& state, Cyto::Any& payload) noexcept {
	if(auto res = get(state, payload); res != message_result::consumed) {
		if(parent)
			return parent->impl_get(state, payload);
		return message_result::unseen;
	}
	return message_result::consumed;
}
message_result element_base::impl_set(sys::state& state, Cyto::Any& payload) noexcept {
	return set(state, payload);
}

message_result element_base::test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type t) noexcept {
	return message_result::unseen;
}
message_result element_base::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}

void element_base::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
}
message_result element_base::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_mouse_move(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
void element_base::on_update(sys::state& state) noexcept {
}
message_result element_base::get(sys::state& state, Cyto::Any& payload) noexcept {
	return message_result::unseen;
}
message_result element_base::set(sys::state& state, Cyto::Any& payload) noexcept {
	return message_result::unseen;
}

void element_base::impl_render(sys::state& state, int32_t x, int32_t y) noexcept {
	render(state, x, y);
}

xy_pair get_absolute_location(element_base const & node) {
	if(node.parent) {
		auto parent_loc = get_absolute_location(*node.parent);
		auto rel_loc = child_relative_location(*node.parent, node);
		return xy_pair{int16_t(parent_loc.x + rel_loc.x), int16_t(parent_loc.y + rel_loc.y)};
	} else {
		return node.base_data.position;
	}
}

int32_t ui_width(sys::state const & state) {
	return int32_t(state.x_size / state.user_settings.ui_scale);
}
int32_t ui_height(sys::state const & state) {
	return int32_t(state.y_size / state.user_settings.ui_scale);
}

void populate_definitions_map(sys::state& state) {
	for(size_t i = state.ui_defs.gui.size(); i-- > 0;) {
		if(state.ui_defs.gui[dcon::gui_def_id(dcon::gui_def_id::value_base_t(i))].is_top_level()) {
			auto key = state.to_string_view(state.ui_defs.gui[dcon::gui_def_id(dcon::gui_def_id::value_base_t(i))].name);
			auto value = element_target{nullptr, dcon::gui_def_id(dcon::gui_def_id::value_base_t(i))};
			state.ui_state.defs_by_name.insert_or_assign(key, value);
		}
	}
}

} // namespace ui
