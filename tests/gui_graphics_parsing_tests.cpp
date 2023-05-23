#include "catch.hpp"

TEST_CASE("gfx parsers tests", "[parsers]") {
	SECTION("empty_file_with_types") {
		std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

		parsers::error_handler err("no file");
		const char tex[] = "spritetypes = {}\n"
			"bitmapfonts = {}\n"
			"fonts = {}\n"
			"lighttypes = {}\n"
			"objecttypes = {}\n";

		parsers::token_generator gen(RANGE(tex));
		parsers::building_gfx_context test_context{ *state, state->ui_defs };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->ui_defs.gfx.size() == size_t(0));
	}
	SECTION("unknown_spritetype") {
		std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

		parsers::error_handler err("no file");

		const char tex[] = "spritetypes = {\n"
			"badspritetype = {}\n"
			"}\n"
			"bitmapfonts = {}\n"
			"fonts = {}\n"
			"lighttypes = {}\n"
			"objecttypes = {}\n";

		parsers::token_generator gen(RANGE(tex));
		parsers::building_gfx_context test_context{ *state, state->ui_defs };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors.length() != size_t(0));
		REQUIRE(state->ui_defs.gfx.size() == size_t(0));
	}
	SECTION("sprite_names") {
		std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

		parsers::error_handler err("no file");

		const char tex[] = "spritetypes = {\n"
			"spritetype = {name = unknown}\n"
			"spritetype = {name = dummy}\n"
			"}\n"
			"bitmapfonts = {}\n"
			"fonts = {}\n"
			"lighttypes = {}\n"
			"objecttypes = {}\n";

		parsers::token_generator gen(RANGE(tex));
		parsers::building_gfx_context test_context{ *state, state->ui_defs };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->ui_defs.gfx.size() == size_t(2));

		REQUIRE(0 == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::always_transparent));
		REQUIRE(0 == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::flip_v));
		REQUIRE(0 == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::has_click_sound));
		REQUIRE(0 == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::do_transparency_check));
		REQUIRE(0 == state->ui_defs.gfx[dcon::gfx_object_id(0)].size.x);
		REQUIRE(0 == state->ui_defs.gfx[dcon::gfx_object_id(0)].size.y);
		REQUIRE(dcon::texture_id() == state->ui_defs.gfx[dcon::gfx_object_id(0)].primary_texture_handle);
		REQUIRE(0 == state->ui_defs.gfx[dcon::gfx_object_id(0)].type_dependent);
		REQUIRE(1 == state->ui_defs.gfx[dcon::gfx_object_id(0)].number_of_frames);
		REQUIRE(uint8_t(ui::object_type::generic_sprite) == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::type_mask));

		REQUIRE(test_context.map_of_names.find(std::string("unknown"))->second == dcon::gfx_object_id(0));
		REQUIRE(test_context.map_of_names.find(std::string("dummy"))->second == dcon::gfx_object_id(1));
	}
	SECTION("non_default_properties") {
		std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

		parsers::error_handler err("no file");

		const char tex[] = "spritetypes = {\n"
			"barcharttype = {\n"
			"name = dummy\n"
			"allwaystransparent = yes\n"
			"flipv = yes\n"
			"clicksound = click\n"
			"textureFile = blah\n"
			"borderSize = {x=4 y= 4}\n"
			"size = {x = 20 y = 40}\n"
			"noOfFrames = 6\n"
			"}\n"
			"}\n"
			"bitmapfonts = {}\n"
			"fonts = {}\n"
			"lighttypes = {}\n"
			"objecttypes = {}\n";

		parsers::token_generator gen(RANGE(tex));
		parsers::building_gfx_context test_context{ *state, state->ui_defs };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->ui_defs.gfx.size() == size_t(1));

		REQUIRE(ui::gfx_object::always_transparent == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::always_transparent));
		REQUIRE(ui::gfx_object::flip_v == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::flip_v));
		REQUIRE(ui::gfx_object::has_click_sound == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::has_click_sound));
		REQUIRE(0 == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::do_transparency_check));
		REQUIRE(20 == state->ui_defs.gfx[dcon::gfx_object_id(0)].size.x);
		REQUIRE(40 == state->ui_defs.gfx[dcon::gfx_object_id(0)].size.y);
		REQUIRE(dcon::texture_id(0) == state->ui_defs.gfx[dcon::gfx_object_id(0)].primary_texture_handle);
		REQUIRE(4 == state->ui_defs.gfx[dcon::gfx_object_id(0)].type_dependent);
		REQUIRE(6 == state->ui_defs.gfx[dcon::gfx_object_id(0)].number_of_frames);
		REQUIRE(uint8_t(ui::object_type::barchart) == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::type_mask));

		REQUIRE(test_context.map_of_names.find(std::string("dummy"))->second == dcon::gfx_object_id(0));
	}
	SECTION("non_default_properties_b") {
		std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

		parsers::error_handler err("no file");

		const char tex[] = "spritetypes = {\n"
			"progressbartype = {\n"
			"name = dummy\n"
			"allwaystransparent = yes\n"
			"flipv = yes\n"
			"clicksound = click\n"
			"transparencecheck = yes\n"
			"textureFile1 = blah\n"
			"textureFile2 = blah\n"
			"size = 30\n"
			"noOfFrames = 6\n"
			"horizontal = no\n"
			"}\n"
			"}\n"
			"bitmapfonts = {}\n"
			"fonts = {}\n"
			"lighttypes = {}\n"
			"objecttypes = {}\n";

		parsers::token_generator gen(RANGE(tex));
		parsers::building_gfx_context test_context{ *state, state->ui_defs };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->ui_defs.gfx.size() == size_t(1));

		REQUIRE(ui::gfx_object::always_transparent == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::always_transparent));
		REQUIRE(ui::gfx_object::flip_v == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::flip_v));
		REQUIRE(ui::gfx_object::has_click_sound == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::has_click_sound));
		REQUIRE(ui::gfx_object::do_transparency_check == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::do_transparency_check));
		REQUIRE(30 == state->ui_defs.gfx[dcon::gfx_object_id(0)].size.x);
		REQUIRE(30 == state->ui_defs.gfx[dcon::gfx_object_id(0)].size.y);
		REQUIRE(dcon::texture_id() != state->ui_defs.gfx[dcon::gfx_object_id(0)].primary_texture_handle);
		REQUIRE(0 != state->ui_defs.gfx[dcon::gfx_object_id(0)].type_dependent);
		REQUIRE(6 == state->ui_defs.gfx[dcon::gfx_object_id(0)].number_of_frames);
		REQUIRE(uint8_t(ui::object_type::vertical_progress_bar) == (state->ui_defs.gfx[dcon::gfx_object_id(0)].flags & ui::gfx_object::type_mask));

		REQUIRE(test_context.map_of_names.find(std::string("dummy"))->second == dcon::gfx_object_id(0));
	}
}

int32_t find_gui_by_name(sys::state const& state, std::string_view name) {
	for(int32_t i = 0; i < state.ui_defs.gui.ssize(); ++i) {
		if(state.to_string_view(state.ui_defs.gui[dcon::gui_def_id(uint16_t(i))].name) == name) {
			return i;
		}
	}
	return -1;
}

#ifndef IGNORE_REAL_FILES_TESTS

TEST_CASE("gfx game files parsing", "[parsers]") {
	SECTION("empty_file_with_types") {
		std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

		REQUIRE(std::string("NONE") != GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)
		REQUIRE(std::string("") != GAME_DIR);
		add_root(state->common_fs, NATIVE_M(GAME_DIR));
		auto rt = get_root(state->common_fs);
		auto interfc = open_directory(rt, NATIVE("interface"));
		auto all_files = list_files(interfc, NATIVE(".gfx"));

		parsers::error_handler err("");
		parsers::building_gfx_context test_context{ *state, state->ui_defs };
		for(auto& file : all_files) {
			auto ofile = open_file(file);
			if(ofile) {
				auto content = view_contents(*ofile);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_gfx_files(gen, err, test_context);
			}
		}

		REQUIRE(err.accumulated_errors == "");

		/*
		progressbartype = {
			name = "GFX_unciv_progress"
			color = { 0.0 0.5 0.0 }
			colortwo = { 1.0 0.0 0.0 }
			textureFile1 = "gfx\\interface\\unciv_progress2.tga"
			textureFile2 = "gfx\\interface\\unciv_progress1.tga"
			size = { x = 371 y = 25 }
			effectFile = "gfx\\FX\\progress.fx"
		}
		*/
		auto found_obj_it = test_context.map_of_names.find(std::string("GFX_unciv_progress"));
		REQUIRE(found_obj_it != test_context.map_of_names.end());
		auto found_tag = found_obj_it->second;
		REQUIRE(state->ui_defs.gfx[found_tag].size.x == int16_t(371));
		REQUIRE(state->ui_defs.gfx[found_tag].size.y == int16_t(25));
		REQUIRE(uint8_t(ui::object_type::horizontal_progress_bar) == (state->ui_defs.gfx[found_tag].flags & ui::gfx_object::type_mask));
		REQUIRE(state->to_string_view(state->ui_defs.textures[state->ui_defs.gfx[found_tag].primary_texture_handle]) == "gfx" NATIVE_SEP "interface" NATIVE_SEP "unciv_progress2.tga");
		REQUIRE(state->to_string_view(state->ui_defs.textures[dcon::texture_id(uint16_t(state->ui_defs.gfx[found_tag].type_dependent - 1))]) == "gfx" NATIVE_SEP "interface" NATIVE_SEP "unciv_progress1.tga");


		{
			auto all_gui_files = list_files(interfc, NATIVE(".gui"));

			for(auto& file : all_gui_files) {
				auto file_name = get_full_name(file);
				if(!parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("confirmbuild.gui"))
					&& !parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("convoys.gui"))
					&& !parsers::native_has_fixed_suffix_ci(file_name.data(), file_name.data() + file_name.length(), NATIVE("brigadeview.gui"))
					) {
					auto ofile = open_file(file);
					if(ofile) {
						auto content = view_contents(*ofile);
						err.file_name = simple_fs::native_to_utf8(get_full_name(*ofile));
						parsers::token_generator gen(content.data, content.data + content.file_size);
						parsers::parse_gui_files(gen, err, test_context);
					}
				}
			}

			REQUIRE(err.accumulated_errors == "");

			{
				auto index = find_gui_by_name(*state, "rallypoint_checkbox_naval");
				REQUIRE(index != -1);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].get_element_type() == ui::element_type::button);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.button.is_checkbox() == true);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.button.shortcut == sys::virtual_key::V);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.x == int16_t(30));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.y == int16_t(322));
				REQUIRE(bool(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.button.button_image) == true);
			}
			/*
			iconType =
			{
				name ="diplo_default_tile_bg"
				spriteType = "GFX_diplo_default_tile_bg"
				position = { x= 25 y = 100 }
				Orientation = "UPPER_LEFT"
			}
			*/
			{
				auto index = find_gui_by_name(*state, "diplo_default_tile_bg");
				REQUIRE(index != -1);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].get_element_type() == ui::element_type::image);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.x == int16_t(25));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.y == int16_t(100));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].get_orientation() == ui::orientation::upper_left);
				REQUIRE(bool(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.image.gfx_object) == true);
			}
			/*
			instantTextBoxType={
				position = {x = 28 y = 58 }
				name = "settings_label"
				font = "vic_22_black"
				borderSize = {x = 0 y = 0}	
				maxWidth = 172
				maxHeight = 18
				text = "SM_SETTINGS"	
				orientation = "UPPER_LEFT"
				format = centre
			}
			*/
			{
				auto index = find_gui_by_name(*state, "settings_label");
				REQUIRE(index != -1);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].get_element_type() == ui::element_type::text);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.x == int16_t(28));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.y == int16_t(58));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].get_orientation() == ui::orientation::upper_left);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.text.is_instant() == true);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.text.is_edit() == false);
				REQUIRE(text::size_from_font_id(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.text.font_handle) == uint16_t(16));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].size.x == int16_t(172));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].size.y == int16_t(18));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.text.border_size.x == 0);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.text.border_size.y == 0);
				REQUIRE(bool(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.text.txt) == true);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.text.get_alignment() == ui::alignment::centered);
			}
			/*
			windowType = {
				name = "topbar"
				backGround=""
				position = { x=0 y=0 }	
				size = { x=2048 y=100 }
				moveable = 0
				dontRender = ""
				horizontalBorder= ""
				verticalBorder= ""
				fullScreen = yes

				iconType = {
					name ="topbar_bg"
					spriteType = "GFX_topbar_bg"
					position = { x= 0 y = -1 }
					Orientation = "UPPER_LEFT"
				}
				...
			*/

			{
				auto index = find_gui_by_name(*state, "topbar");
				REQUIRE(index != -1);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].get_element_type() == ui::element_type::window);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.x == int16_t(0));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].position.y == int16_t(0));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].size.x == int16_t(2048));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].size.y == int16_t(100));
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.window.is_moveable() == false);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.window.is_fullscreen() == true);
				REQUIRE(state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.window.num_children > 0);

				auto first_child = state->ui_defs.gui[dcon::gui_def_id(uint16_t(index))].data.window.first_child;
				REQUIRE(bool(first_child) == true);
				REQUIRE(state->ui_defs.gui[first_child].get_element_type() == ui::element_type::image);
				REQUIRE(state->to_string_view(state->ui_defs.gui[first_child].name) == "topbar_bg");
			}
		}
	}
}

#endif
