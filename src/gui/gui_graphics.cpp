#include "gui_graphics.hpp"
#include "simple_fs.hpp"
#include "gui_element_base.hpp"

namespace ui {

void load_text_gui_definitions(sys::state& state, parsers::building_gfx_context& context, parsers::error_handler& err) {
	// preload special background textures
	assert(context.ui_defs.textures.size() == size_t(0));
	{
		auto stripped_name = simple_fs::remove_double_backslashes(std::string_view("gfx\\interface\\small_tiles_dialog.tga"));
		context.ui_defs.textures.emplace_back(context.full_state.add_key_win1252(stripped_name));
		context.map_of_texture_names.insert_or_assign(stripped_name, definitions::small_tiles_dialog);
	}
	{
		auto stripped_name = simple_fs::remove_double_backslashes(std::string_view("gfx\\interface\\tiles_dialog.tga"));
		context.ui_defs.textures.emplace_back(context.full_state.add_key_win1252(stripped_name));
		context.map_of_texture_names.insert_or_assign(stripped_name, definitions::tiles_dialog);
	}
	{
		auto stripped_name = simple_fs::remove_double_backslashes(std::string_view("gfx\\interface\\transparency.tga"));
		context.ui_defs.textures.emplace_back(context.full_state.add_key_win1252(stripped_name));
		context.map_of_texture_names.insert_or_assign(stripped_name, definitions::transparency);
	}

	auto rt = get_root(state.common_fs);
	auto interfc = open_directory(rt, NATIVE("interface"));
	auto assets = open_directory(rt, NATIVE("assets"));

	{
		// first, load in special mod gfx
		// TODO put this in a better location
		auto all_alice_files = list_files(assets, NATIVE(".gfx"));
		for(auto& file : all_alice_files) {
			if(auto ofile = open_file(file); ofile) {
				auto content = view_contents(*ofile);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_gfx_files(gen, err, context);
			}
		}

		auto all_files = list_files(interfc, NATIVE(".gfx"));
		for(auto& file : all_files) {
			if(auto ofile = open_file(file); ofile) {
				auto content = view_contents(*ofile);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_gfx_files(gen, err, context);
			}
		}

		auto all_sfx_files = list_files(interfc, NATIVE(".sfx"));
		for(auto& file : all_sfx_files) {
			if(auto ofile = open_file(file); ofile) {
				auto content = view_contents(*ofile);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_sfx_file(gen, err, context);
			}
		}
	}

	{
		// first, load in special mod gui
		// TODO put this in a better location
		auto all_alice_gui_files = list_files(assets, NATIVE(".gui"));
		for(auto& file : all_alice_gui_files) {
			if(auto ofile = open_file(file); ofile) {
				auto content = view_contents(*ofile);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_gui_files(gen, err, context);
				context.gui_files.emplace_back(std::move(*ofile));
			}
		}

		// load normal .gui files
		auto all_gui_files = list_files(interfc, NATIVE(".gui"));

		for(auto& file : all_gui_files) {
			auto file_name = get_full_name(file);
			if(!parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(),
						 NATIVE("confirmbuild.gui")) &&
					!parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("convoys.gui")) &&
					!parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(),
							NATIVE("brigadeview.gui"))) {
				auto ofile = open_file(file);
				if(ofile) {
					auto content = view_contents(*ofile);
					err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
					parsers::token_generator gen(content.data, content.data + content.file_size);
					parsers::parse_gui_files(gen, err, context);
					context.gui_files.emplace_back(std::move(*ofile));
				}
			}
		}
	}
}

int16_t child_relative_location_y_component(element_base const& parent, element_base const& child) {
	auto orientation = child.base_data.get_orientation();
	switch(orientation) {
	case orientation::upper_left:
		return int16_t(child.base_data.position.y);
	case orientation::upper_right:
		return int16_t(child.base_data.position.y);
	case orientation::lower_left:
		return int16_t(parent.base_data.size.y + child.base_data.position.y);
	case orientation::lower_right:
		return int16_t(parent.base_data.size.y + child.base_data.position.y);
	case orientation::upper_center:
		return int16_t(child.base_data.position.y);
	case orientation::lower_center:
		return int16_t(parent.base_data.size.y + child.base_data.position.y);
	case orientation::center:
		return int16_t(parent.base_data.size.y / 2 + child.base_data.position.y);
	default:
		return int16_t(child.base_data.position.y);
	}
}

xy_pair child_relative_non_mirror_location(sys::state& state, element_base const& parent, element_base const& child) {
	auto orientation = child.base_data.get_orientation();
	int16_t y = child_relative_location_y_component(parent, child);
	switch(orientation) {
	case orientation::upper_left:
	case orientation::lower_left:
	default:
		return xy_pair{ int16_t(child.base_data.position.x), y };
	case orientation::upper_right:
	case orientation::lower_right:
		return xy_pair{ int16_t(parent.base_data.size.x + child.base_data.position.x), y };
	case orientation::upper_center:
	case orientation::lower_center:
	case orientation::center:
		return xy_pair{ int16_t(parent.base_data.size.x / 2 + child.base_data.position.x), y };
	}
}

xy_pair child_relative_location(sys::state& state, element_base const& parent, element_base const& child) {
	auto orientation = child.base_data.get_orientation();
	int16_t y = child_relative_location_y_component(parent, child);
	if(state.world.locale_get_native_rtl(state.font_collection.get_current_locale())) {
		switch(orientation) {
		case orientation::upper_left:
		case orientation::lower_left:
		default:
			return xy_pair{ int16_t(parent.base_data.size.x - child.base_data.position.x - child.base_data.size.x), y };
		case orientation::lower_right:
		case orientation::upper_right:
			return xy_pair{ int16_t(-child.base_data.position.x - child.base_data.size.x), y };
		case orientation::upper_center:
		case orientation::lower_center:
		case orientation::center:
			return xy_pair{ int16_t(parent.base_data.size.x / 2 - child.base_data.position.x - child.base_data.size.x), y };
		}
	}
	return child_relative_non_mirror_location(state, parent, child);
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
			dcon::gfx_object_id gid;
			if(elem_type == element_type::button) {
				gid = base_data.data.button.button_image;
			} else if(elem_type == element_type::image) {
				gid = base_data.data.image.gfx_object;
			} else if(elem_type == element_type::listbox) {
				gid = base_data.data.list_box.background_image;
			}
			if(gid) {
				auto& gfx_def = state.ui_defs.gfx[gid];
				auto mask_handle = gfx_def.type_dependent;
				if(gfx_def.is_partially_transparent() && gfx_def.primary_texture_handle && get_pixel_opacity(state, x, y, gfx_def.primary_texture_handle) > uint8_t(64)) {
					probe_result.under_mouse = this;
				} else if(gfx_def.get_object_type() == ui::object_type::flag_mask && mask_handle && gfx_def.primary_texture_handle) {
					ogl::get_texture_handle(state, dcon::texture_id(mask_handle - 1), true);

					auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
					auto x_off = float(base_data.size.x - mask_tex.size_x) * 0.5f;
					auto y_off = float(base_data.size.y - mask_tex.size_y) * 0.5f;

					if(get_pixel_opacity(state, int32_t(x - x_off), int32_t(y - y_off), dcon::texture_id(mask_handle - 1)) > uint8_t(64) || get_pixel_opacity(state, x, y, gfx_def.primary_texture_handle) > uint8_t(64)) {
						probe_result.under_mouse = this;
					}
				} else {
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
message_result element_base::impl_on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept {
	return on_lbutton_up(state, x, y, mods, under_mouse);
}
message_result element_base::impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return on_rbutton_down(state, x, y, mods);
}
message_result element_base::impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	return on_key_down(state, key, mods);
}
message_result element_base::impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount,
		sys::key_modifiers mods) noexcept {
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
message_result element_base::on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept {
	return message_result::unseen;
}
message_result element_base::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}

void element_base::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y,
		sys::key_modifiers mods) noexcept { }
message_result element_base::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_mouse_move(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
void element_base::on_update(sys::state& state) noexcept { }
message_result element_base::get(sys::state& state, Cyto::Any& payload) noexcept {
	return message_result::unseen;
}
message_result element_base::set(sys::state& state, Cyto::Any& payload) noexcept {
	return message_result::unseen;
}

void element_base::impl_render(sys::state& state, int32_t x, int32_t y) noexcept {
	render(state, x, y);
}

xy_pair get_absolute_location(sys::state& state, element_base const& node) {
	if(node.parent) {
		auto parent_loc = get_absolute_location(state, *node.parent);
		auto rel_loc = child_relative_location(state, *node.parent, node);
		return xy_pair{int16_t(parent_loc.x + rel_loc.x), int16_t(parent_loc.y + rel_loc.y)};
	} else {
		if(state.world.locale_get_native_rtl(state.font_collection.get_current_locale())) {
			auto pos = node.base_data.position;
			pos.x = int16_t(state.x_size) - node.base_data.position.x - node.base_data.size.x;
			return pos;
		}
		return node.base_data.position;
	}
}

xy_pair get_absolute_non_mirror_location(sys::state& state, element_base const& node) {
	if(node.parent) {
		auto parent_loc = get_absolute_non_mirror_location(state, *node.parent);
		auto rel_loc = child_relative_non_mirror_location(state, *node.parent, node);
		return xy_pair{ int16_t(parent_loc.x + rel_loc.x), int16_t(parent_loc.y + rel_loc.y) };
	} else {
		return node.base_data.position;
	}
}

int32_t ui_width(sys::state const& state) {
	return int32_t(state.x_size / state.user_settings.ui_scale);
}
int32_t ui_height(sys::state const& state) {
	return int32_t(state.y_size / state.user_settings.ui_scale);
}

void populate_definitions_map(sys::state& state) {
	for(size_t i = state.ui_defs.gui.size(); i-- > 0;) {
		if(state.ui_defs.gui[dcon::gui_def_id(dcon::gui_def_id::value_base_t(i))].is_top_level()) {
			auto value = element_target{nullptr, dcon::gui_def_id(dcon::gui_def_id::value_base_t(i))};
			state.ui_state.defs_by_name.insert_or_assign(state.ui_defs.gui[dcon::gui_def_id(dcon::gui_def_id::value_base_t(i))].name, value);
		}
	}
	for(size_t i = state.ui_defs.gfx.size(); i-- > 0;) {
		auto value = dcon::gfx_object_id(dcon::gfx_object_id::value_base_t(i));
		state.ui_state.gfx_by_name.insert_or_assign(state.ui_defs.gfx[value].name, value);
	}
}

} // namespace ui
