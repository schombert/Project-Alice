#include "gui_graphics.hpp"

namespace ui {

void load_text_gui_definitions(sys::state& state, parsers::error_handler& err) {
	parsers::building_gfx_context context{ state, state.ui_defs };

	// preload special background textures
	assert(context.ui_defs.textures.size() == size_t(0));

	context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(std::string_view("gfx\\interface\\small_tiles_dialog.tga")));
	context.map_of_texture_names.insert_or_assign(std::string("gfx\\interface\\small_tiles_dialog.tga"), definitions::small_tiles_dialog);

	context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(std::string_view("gfx\\interface\\tiles_dialog.tga")));
	context.map_of_texture_names.insert_or_assign(std::string("gfx\\interface\\tiles_dialog.tga"), definitions::tiles_dialog);

	context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(std::string_view("gfx\\interface\\transparency.tga")));
	context.map_of_texture_names.insert_or_assign(std::string("gfx\\interface\\transparency.tga"), definitions::transparency);

	auto rt = get_root(state.common_fs);
	auto interfc = open_directory(rt, NATIVE("interface"));

	{
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
		auto all_gui_files = list_files(interfc, NATIVE(".gui"));

		for(auto& file : all_gui_files) {
			auto file_name = get_full_name(file);
			if(!parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("confirmbuild.gui"))
				&& !parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("convoys.gui"))
				&& !parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("brigadeview.gui"))
				&& !parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("eu3dialog.gui"))
				) {
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

element_base* element_base::impl_probe_mouse(sys::state& state, int32_t x, int32_t y) noexcept {
	if(0 <= x && x <= base_data.size.x && 0 <= y && y <= base_data.size.y && test_mouse(state, x, y) == message_result::consumed)
		return this;
	else
		return nullptr;
}
message_result element_base::impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(0 <= x && x <= base_data.size.x && 0 <= y && y <= base_data.size.y)
		return on_lbutton_down(state, x, y, mods);
	else
		return message_result::unseen;
}
message_result element_base::impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(0 <= x && x <= base_data.size.x && 0 <= y && y <= base_data.size.y)
		return on_rbutton_down(state, x, y, mods);
	else
		return message_result::unseen;
}
message_result element_base::impl_on_drag(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(0 <= x && x <= base_data.size.x && 0 <= y && y <= base_data.size.y)
		return on_drag(state, x, y, mods);
	else
		return message_result::unseen;
}
message_result element_base::impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	return on_key_down(state, key, mods);
}
message_result element_base::impl_on_text(sys::state& state, char ch) noexcept {
	return on_text(state, ch);
}
message_result element_base::impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(0 <= x && x <= base_data.size.x && 0 <= y && y <= base_data.size.y)
		return on_scroll(state, x, y, amount, mods);
	else
		return message_result::unseen;
}
void element_base::impl_on_update(sys::state& state) noexcept {
	on_update(state);
}
message_result element_base::impl_get(Cyto::Any& payload) noexcept {
	if(auto res = get(payload); res != message_result::consumed) {
		if(parent)
			return parent->impl_get(payload);
		return message_result::unseen;
	}
	return message_result::consumed;
}
message_result element_base::impl_set(Cyto::Any& payload) noexcept {
	return set(payload);
}


message_result element_base::test_mouse(sys::state& state, int32_t x, int32_t y) noexcept {
	return message_result::unseen;
}
message_result element_base::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_drag(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
message_result element_base::on_text(sys::state& state, char ch) noexcept {
	return message_result::unseen;
}
message_result element_base::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	return message_result::unseen;
}
void element_base::on_update(sys::state& state) noexcept {
}
message_result element_base::get(Cyto::Any& payload) noexcept {
	return message_result::unseen;
}
message_result element_base::set(Cyto::Any& payload) noexcept {
	return message_result::unseen;
}

void element_base::impl_render(sys::state& state, int32_t x, int32_t y) noexcept {
	render(state, x, y);
}

xy_pair get_absolute_location(element_base const& node) {
	if(node.parent) {
		auto parent_loc = get_absolute_location(*node.parent);
		return xy_pair{int16_t(parent_loc.x + node.base_data.position.x), int16_t(parent_loc.y + node.base_data.position.y) };
	} else {
		return node.base_data.position;
	}
}

}
