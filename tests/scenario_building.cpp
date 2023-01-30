#include "catch2/catch.hpp"
#include "parsers_declarations.hpp"
#include "nations.hpp"
#include "container_types.hpp"

/*
* parsers::scenario_building_context context(*this);
		// Read national tags from countries.txt
		{
			auto countries = open_file(root, NATIVE("countries.txt"));
			if(countries) {
				auto content = view_contents(*countries);
				err.file_name = "countries.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_national_identity_file(gen, err, context);
			}
		}
*/

#ifndef IGNORE_REAL_FILES_TESTS
TEST_CASE("Scenario building", "[req-game-files]") {

	parsers::error_handler err("");
	REQUIRE(std::string("NONE") != GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	add_root(state->common_fs, NATIVE_M(GAME_DIR));
	auto root = get_root(state->common_fs);
	auto common = open_directory(root, NATIVE("common"));

	parsers::scenario_building_context context(*state);

	SECTION("countries") {
		auto countries = open_file(common, NATIVE("countries.txt"));
		REQUIRE(countries);
		if(countries) {
			auto content = view_contents(*countries);
			err.file_name = "countries.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_identity_file(gen, err, context);
		}
		REQUIRE(err.accumulated_errors == "");

		auto name_num = nations::tag_to_int('T','P','G');
		auto it = context.map_of_ident_names.find(name_num);
		REQUIRE(it != context.map_of_ident_names.end());
		auto id = it->second;
		REQUIRE(bool(id) == true);
		REQUIRE(context.file_names_for_idents[id] == "countries/Taiping.txt");
	}
	SECTION("religion") {
		auto religion = open_file(common, NATIVE("religion.txt"));
		if(religion) {
			auto content = view_contents(*religion);
			err.file_name = "religion.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_religion_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");
		auto it = context.map_of_religion_names.find(std::string("hindu"));
		REQUIRE(it != context.map_of_religion_names.end());
		auto id = it->second;
		REQUIRE(state->world.religion_get_icon(id) == uint8_t(11));
		REQUIRE(sys::red_from_int(state->world.religion_get_color(id)) == 0);
		REQUIRE(sys::green_from_int(state->world.religion_get_color(id)) == 0.8f);
		REQUIRE(state->world.religion_get_is_pagan(id) == false);

		REQUIRE(state->world.religion_get_is_pagan(context.map_of_religion_names.find(std::string("animist"))->second) == true);
	}
}
#endif
