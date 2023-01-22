#include "catch.hpp"

TEST_CASE("gfx parsers tests", "[parsers]") {
	SECTION("empty_file_with_types") {
		ui::defintions ui_defs;
		ankerl::unordered_dense::map<std::string, dcon::gfx_object_id> map_of_names;

		parsers::error_handler err("no file");
		const char tex[] = "spritetypes = {}\n"
			"bitmapfonts = {}\n"
			"fonts = {}\n"
			"lighttypes = {}\n"
			"objecttypes = {}\n";

		parsers::token_generator gen(RANGE(tex));
		parsers::building_gfx_context test_context{&ui_defs, &map_of_names };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(ui_defs.gfx.size() == size_t(0));
	}
	SECTION("unknown_spritetype") {
		ui::defintions ui_defs;
		ankerl::unordered_dense::map<std::string, dcon::gfx_object_id> map_of_names;

		parsers::error_handler err("no file");

		const char tex[] = "spritetypes = {\n"
			"badspritetype = {}\n"
			"}\n"
			"bitmapfonts = {}\n"
			"fonts = {}\n"
			"lighttypes = {}\n"
			"objecttypes = {}\n";

		parsers::token_generator gen(RANGE(tex));
		parsers::building_gfx_context test_context{ &ui_defs, &map_of_names };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors.length() != size_t(0));
		REQUIRE(ui_defs.gfx.size() == size_t(0));
	}
	SECTION("sprite_names") {
		ui::defintions ui_defs;
		ankerl::unordered_dense::map<std::string, dcon::gfx_object_id> map_of_names;

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
		parsers::building_gfx_context test_context{ &ui_defs, &map_of_names };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(ui_defs.gfx.size() == size_t(2));

		REQUIRE(0 == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::always_transparent));
		REQUIRE(0 == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::flip_v));
		REQUIRE(0 == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::has_click_sound));
		REQUIRE(0 == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::do_transparency_check));
		REQUIRE(0 == ui_defs.gfx[dcon::gfx_object_id(0).index()].size.x);
		REQUIRE(0 == ui_defs.gfx[dcon::gfx_object_id(0).index()].size.y);
		REQUIRE(dcon::texture_id() == ui_defs.gfx[dcon::gfx_object_id(0).index()].primary_texture_handle);
		REQUIRE(0 == ui_defs.gfx[dcon::gfx_object_id(0).index()].type_dependant);
		REQUIRE(1 == ui_defs.gfx[dcon::gfx_object_id(0).index()].number_of_frames);
		REQUIRE(uint8_t(ui::object_type::generic_sprite) == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::type_mask));

		REQUIRE(map_of_names.find(std::string("unknown"))->second == dcon::gfx_object_id(0));
		REQUIRE(map_of_names.find(std::string("dummy"))->second == dcon::gfx_object_id(1));
	}
	SECTION("non_default_properties") {
		ui::defintions ui_defs;
		ankerl::unordered_dense::map<std::string, dcon::gfx_object_id> map_of_names;

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
		parsers::building_gfx_context test_context{ &ui_defs, &map_of_names };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(ui_defs.gfx.size() == size_t(1));

		REQUIRE(ui::gfx_object::always_transparent == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::always_transparent));
		REQUIRE(ui::gfx_object::flip_v == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::flip_v));
		REQUIRE(ui::gfx_object::has_click_sound == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::has_click_sound));
		REQUIRE(0 == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::do_transparency_check));
		REQUIRE(20 == ui_defs.gfx[dcon::gfx_object_id(0).index()].size.x);
		REQUIRE(40 == ui_defs.gfx[dcon::gfx_object_id(0).index()].size.y);
		REQUIRE(dcon::texture_id(0) == ui_defs.gfx[dcon::gfx_object_id(0).index()].primary_texture_handle);
		REQUIRE(4 == ui_defs.gfx[dcon::gfx_object_id(0).index()].type_dependant);
		REQUIRE(6 == ui_defs.gfx[dcon::gfx_object_id(0).index()].number_of_frames);
		REQUIRE(uint8_t(ui::object_type::barchart) == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::type_mask));

		REQUIRE(map_of_names.find(std::string("dummy"))->second == dcon::gfx_object_id(0));
	}
	SECTION("non_default_properties_b") {
		ui::defintions ui_defs;
		ankerl::unordered_dense::map<std::string, dcon::gfx_object_id> map_of_names;

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
		parsers::building_gfx_context test_context{ &ui_defs, &map_of_names };

		parsers::parse_gfx_files(gen, err, test_context);

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(ui_defs.gfx.size() == size_t(1));

		REQUIRE(ui::gfx_object::always_transparent == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::always_transparent));
		REQUIRE(ui::gfx_object::flip_v == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::flip_v));
		REQUIRE(ui::gfx_object::has_click_sound == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::has_click_sound));
		REQUIRE(ui::gfx_object::do_transparency_check == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::do_transparency_check));
		REQUIRE(30 == ui_defs.gfx[dcon::gfx_object_id(0).index()].size.x);
		REQUIRE(30 == ui_defs.gfx[dcon::gfx_object_id(0).index()].size.y);
		REQUIRE(dcon::texture_id() != ui_defs.gfx[dcon::gfx_object_id(0).index()].primary_texture_handle);
		REQUIRE(0 != ui_defs.gfx[dcon::gfx_object_id(0).index()].type_dependant);
		REQUIRE(6 == ui_defs.gfx[dcon::gfx_object_id(0).index()].number_of_frames);
		REQUIRE(uint8_t(ui::object_type::vertical_progress_bar) == (ui_defs.gfx[dcon::gfx_object_id(0).index()].flags & ui::gfx_object::type_mask));

		REQUIRE(map_of_names.find(std::string("dummy"))->second == dcon::gfx_object_id(0));
	}
}
