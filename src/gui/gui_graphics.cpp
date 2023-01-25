#include "gui_graphics.hpp"

namespace ui {

void load_text_gui_definitions(sys::state& state, parsers::error_handler& err) {
	parsers::building_gfx_context context{ state, state.ui_defs };

	// preload special background textures
	assert(context.ui_defs.textures.size() == size_t(0));

	context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(std::string_view("gfx\\interface\\small_tiles_dialog.tga")));
	context.map_of_texture_names.insert_or_assign(std::string("gfx\\interface\\small_tiles_dialog.tga"), defintions::small_tiles_dialog);

	context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(std::string_view("gfx\\interface\\tiles_dialog.tga")));
	context.map_of_texture_names.insert_or_assign(std::string("gfx\\interface\\tiles_dialog.tga"), defintions::tiles_dialog);

	context.ui_defs.textures.emplace_back(context.full_state.add_to_pool(std::string_view("gfx\\interface\\transparency.tga")));
	context.map_of_texture_names.insert_or_assign(std::string("gfx\\interface\\transparency.tga"), defintions::transparency);

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

}
