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

	//COUNTRIES
	{
		auto countries = open_file(common, NATIVE("countries.txt"));
		REQUIRE(countries);
		if(countries) {
			auto content = view_contents(*countries);
			err.file_name = "countries.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_identity_file(gen, err, context);
		}
		REQUIRE(err.accumulated_errors == "");

		auto name_num = nations::tag_to_int('T', 'P', 'G');
		auto it = context.map_of_ident_names.find(name_num);
		REQUIRE(it != context.map_of_ident_names.end());
		auto id = it->second;
		REQUIRE(bool(id) == true);
		REQUIRE(context.file_names_for_idents[id] == "countries/Taiping.txt");
	}
	//RELIGION
	{
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
	// CULTURE
	{
		auto cultures = open_file(common, NATIVE("cultures.txt"));
		if(cultures) {
			auto content = view_contents(*cultures);
			err.file_name = "cultures.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_culture_file(gen, err, context);
		}


		REQUIRE(err.accumulated_errors == "");

		auto it = context.map_of_culture_names.find(std::string("french_canadian"));
		REQUIRE(it != context.map_of_culture_names.end());
		auto id = dcon::fatten(state->world, it->second);
		REQUIRE(sys::red_from_int(state->world.culture_get_color(id)) >= 0.5f);
		REQUIRE(state->world.culture_get_radicalism(id) == 10);
		REQUIRE(state->world.culture_get_first_names(id).size() == 19);
		REQUIRE(state->world.culture_get_last_names(id).size() == 14);

		auto itb = context.map_of_culture_group_names.find(std::string("neo_european_cultures"));
		REQUIRE(itb != context.map_of_culture_group_names.end());
		auto idb = dcon::fatten(state->world, itb->second);
		REQUIRE(state->world.culture_group_get_leader(idb) == uint8_t(sys::leader_type::european));
		REQUIRE(idb.get_is_overseas() == false);
		int32_t count = 0;
		for(auto c : idb.get_culture_group_membership())
			++count;
		REQUIRE(count == 4);
		REQUIRE(id.get_culture_group_membership_as_member().get_group() == idb);

		auto itc = context.map_of_culture_group_names.find(std::string("american_cultures"));
		REQUIRE(itc != context.map_of_culture_group_names.end());
		auto idc = dcon::fatten(state->world, itc->second);

		auto union_tag = idc.get_identity_from_cultural_union_of();
		auto usa_tag = context.map_of_ident_names.find(nations::tag_to_int('U', 'S', 'A'))->second;
		
		REQUIRE(union_tag == usa_tag);
	}
	{
		// FIRST: make sure that we have a money good
		if(state->world.commodity_size() == 0) {
			// create money
			auto money_id = state->world.create_commodity();
			assert(money_id.index() == 0);
		}
		auto goods = open_file(common, NATIVE("goods.txt"));
		if(goods) {
			auto content = view_contents(*goods);
			err.file_name = "goods.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_goods_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		{
			auto it = context.map_of_commodity_names.find(std::string("aeroplanes"));
			REQUIRE(it != context.map_of_commodity_names.end());
			auto id = dcon::fatten(state->world, it->second);
			REQUIRE(sys::red_from_int(id.get_color()) >= 0.5f);
			REQUIRE(id.get_cost() == 110.0f);
			REQUIRE(id.get_is_available_from_start() == false);
			REQUIRE(sys::commodity_group(id.get_commodity_group()) == sys::commodity_group::military_goods);
		}
		{
			auto it = context.map_of_commodity_names.find(std::string("tobacco"));
			REQUIRE(it != context.map_of_commodity_names.end());
			auto id = dcon::fatten(state->world, it->second);
			REQUIRE(sys::red_from_int(id.get_color()) <= 0.5f);
			REQUIRE(id.get_cost() == Approx(1.1f));
			REQUIRE(id.get_is_available_from_start() == true);
			REQUIRE(sys::commodity_group(id.get_commodity_group()) == sys::commodity_group::consumer_goods);
		}
	}
}
#endif
