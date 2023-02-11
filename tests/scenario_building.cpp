#include "catch.hpp"
#include "parsers_declarations.hpp"
#include "dcon_generated.hpp"
#include "nations.hpp"
#include "container_types.hpp"
#include "system_state.hpp"

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

		{
			auto name_num = nations::tag_to_int('T', 'P', 'G');
			auto it = context.map_of_ident_names.find(name_num);
			REQUIRE(it != context.map_of_ident_names.end());
			auto id = it->second;
			REQUIRE(bool(id) == true);
			REQUIRE(context.file_names_for_idents[id] == "countries/Taiping.txt");
		}
		{
			REQUIRE(nations::tag_to_int('M', 'E', 'X') != nations::tag_to_int('M', 'T', 'C'));
			auto name_num = nations::tag_to_int('M', 'E', 'X');
			auto it = context.map_of_ident_names.find(name_num);
			REQUIRE(it != context.map_of_ident_names.end());
			auto id = it->second;
			auto file_name = simple_fs::win1250_to_native(context.file_names_for_idents[id]);
			REQUIRE(file_name == NATIVE("countries/Mexico.txt"));
		}
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
		state->world.factory_type_resize_construction_costs(state->world.commodity_size());

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
	{
		auto buildings = open_file(common, NATIVE("buildings.txt"));
		if(buildings) {
			auto content = view_contents(*buildings);
			err.file_name = "buildings.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_building_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto it = context.map_of_production_types.find(std::string("liquor_distillery"));
		REQUIRE(it != context.map_of_production_types.end());
		auto id = fatten(state->world, it->second);
		REQUIRE(id.get_construction_time() == 730);
		REQUIRE(id.get_is_available_from_start() == true);
		REQUIRE(id.get_construction_costs(context.map_of_commodity_names.find(std::string("machine_parts"))->second) == 80.0f);

		REQUIRE(bool(state->economy_definitions.railroad_definition.province_modifier) == true);
		sys::modifier_definition pmod = state->world.modifier_get_province_values(state->economy_definitions.railroad_definition.province_modifier);
		REQUIRE(pmod.get_offet_at_index(0) == sys::provincial_mod_offsets::movement_cost);
		REQUIRE(pmod.values[0] == Approx(-0.05f));
	}
	{
		context.ideologies_file = open_file(common, NATIVE("ideologies.txt"));
		if(context.ideologies_file) {
			auto content = view_contents(*context.ideologies_file);
			err.file_name = "ideologies.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_ideology_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto ita = context.map_of_ideology_groups.find(std::string("socialist_group"));
		REQUIRE(ita != context.map_of_ideology_groups.end());

		auto itb = context.map_of_ideologies.find(std::string("communist"));
		REQUIRE(itb != context.map_of_ideologies.end());

		auto fata = fatten(state->world, ita->second);
		auto fatb = fatten(state->world, itb->second.id);

		REQUIRE(fatb.get_ideology_group_membership_as_ideology().get_ideology_group() == fata);
		int32_t count = 0;
		for(auto c : fata.get_ideology_group_membership_as_ideology_group())
			++count;
		REQUIRE(count == 2);
	}
	{
		context.issues_file = open_file(common, NATIVE("issues.txt"));
		if(context.issues_file) {
			auto content = view_contents(*context.issues_file);
			err.file_name = "issues.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_issues_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		//school_reforms
		auto ita = context.map_of_issues.find(std::string("school_reforms"));
		REQUIRE(ita != context.map_of_issues.end());

		auto fata = fatten(state->world, ita->second);
		REQUIRE(fata.get_is_next_step_only() == true);
		REQUIRE(fata.get_is_administrative() == true);

		auto itb = context.map_of_options.find(std::string("acceptable_schools"));
		REQUIRE(itb != context.map_of_options.end());
		auto fatb = fatten(state->world, itb->second.id);
		REQUIRE(fata.get_options().at(2) == fatb);

		REQUIRE(std::find(state->culture_definitions.social_issues.begin(), state->culture_definitions.social_issues.end(), ita->second) != state->culture_definitions.social_issues.end());
	}
	{
		auto governments = open_file(common, NATIVE("governments.txt"));
		if(governments) {
			auto content = view_contents(*governments);
			err.file_name = "governments.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_governments_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		//prussian_constitutionalism

		auto ita = context.map_of_governments.find(std::string("prussian_constitutionalism"));
		REQUIRE(ita != context.map_of_governments.end());
		auto ida = ita->second;
		REQUIRE(state->culture_definitions.governments[ida].has_elections == true);
		REQUIRE(state->culture_definitions.governments[ida].duration == 48);
		REQUIRE(state->culture_definitions.governments[ida].can_appoint_ruling_party == true);
		REQUIRE(state->culture_definitions.governments[ida].flag == ::culture::flag_type::monarchy);
		REQUIRE(
			(state->culture_definitions.governments[ida].ideologies_allowed &
				::culture::to_bits(context.map_of_ideologies.find(std::string("conservative"))->second.id)) != 0);
	}
	{
		context.cb_types_file = open_file(common, NATIVE("cb_types.txt"));
		if(context.cb_types_file) {
			auto content = view_contents(*context.cb_types_file);
			err.file_name = "cb_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_cb_types_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->world.cb_type_size() > 5);
	}
	{
		auto traits = open_file(common, NATIVE("traits.txt"));
		if(traits) {
			auto content = view_contents(*traits);
			err.file_name = "traits.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_traits_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto ita = context.map_of_leader_traits.find(std::string("wretched"));
		REQUIRE(ita != context.map_of_leader_traits.end());

		auto fata = fatten(state->world, ita->second);
		REQUIRE(fata.get_attack() == -1.0f);
		REQUIRE(fata.get_experience() == 0.0f);
	}
	{
		context.crimes_file = open_file(common, NATIVE("crime.txt"));
		if(context.crimes_file) {
			auto content = view_contents(*context.crimes_file);
			err.file_name = "crime.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_crimes_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto ita = context.map_of_crimes.find(std::string("machine_politics"));
		REQUIRE(ita != context.map_of_crimes.end());
		REQUIRE(ita->second.id.index() != 0);
		REQUIRE(state->culture_definitions.crimes.size() > size_t(ita->second.id.index()));
	}
	{
		context.triggered_modifiers_file = open_file(common, NATIVE("triggered_modifiers.txt"));
		if(context.triggered_modifiers_file) {
			auto content = view_contents(*context.triggered_modifiers_file);
			err.file_name = "triggered_modifiers.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_triggered_modifiers_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");
		// default file is empty, unfortunately
	}
	{
		auto nv_file = open_file(common, NATIVE("nationalvalues.txt"));
		if(nv_file) {
			auto content = view_contents(*nv_file);
			err.file_name = "nationalvalues.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_values_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("nv_liberty"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;
		REQUIRE(state->world.modifier_get_national_values(id).get_offet_at_index(0) == sys::national_mod_offsets::mobilisation_size);
		REQUIRE(state->world.modifier_get_national_values(id).get_offet_at_index(1) == sys::national_mod_offsets::mobilisation_economy_impact);
		REQUIRE(state->world.modifier_get_national_values(id).offsets[2] == 0);

		REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(0.02f));
		REQUIRE(state->world.modifier_get_national_values(id).values[1] == Approx(0.75f));
	}
	{
		auto sm_file = open_file(common, NATIVE("static_modifiers.txt"));
		if(sm_file) {
			auto content = view_contents(*sm_file);
			err.file_name = "static_modifiers.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_static_modifiers_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		/*
		* has_siege = {
			farm_rgo_eff = -0.5
			mine_rgo_eff = -0.5
		}
		*/

		REQUIRE(bool(state->national_definitions.has_siege) == true);
		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).get_offet_at_index(0) == sys::provincial_mod_offsets::farm_rgo_eff);
		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).get_offet_at_index(1) == sys::provincial_mod_offsets::mine_rgo_eff);
		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).offsets[2] == 0);

		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).values[0] == Approx(-0.5f));
		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).values[1] == Approx(-0.5f));
	}
	{
		auto em_file = open_file(common, NATIVE("event_modifiers.txt"));
		if(em_file) {
			auto content = view_contents(*em_file);
			err.file_name = "event_modifiers.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_event_modifiers_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");


		auto nvit = context.map_of_modifiers.find(std::string("stock_market_crash"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;

		REQUIRE(state->world.modifier_get_icon(id) == 12);

		REQUIRE(state->world.modifier_get_national_values(id).get_offet_at_index(0) == sys::national_mod_offsets::factory_throughput);
		REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(-0.2f));
	}
	{
		auto defines_file = open_file(common, NATIVE("defines.lua"));
		if(defines_file) {
			auto content = view_contents(*defines_file);
			err.file_name = "defines.lua";
			state->defines.parse_file(*state, std::string_view(content.data, content.data + content.file_size), err);
		}

		REQUIRE(err.accumulated_errors == "");
	}
	{
		sys::list_pop_types(*state, context);

		auto nvit = context.map_of_poptypes.find(std::string("clergymen"));
		REQUIRE(nvit != context.map_of_poptypes.end());
		REQUIRE(bool(nvit->second) == true);
	}
	{
		context.rebel_types_file = open_file(common, NATIVE("rebel_types.txt"));
		if(context.rebel_types_file) {
			auto content = view_contents(*context.rebel_types_file);
			err.file_name = "rebel_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_rebel_types_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_rebeltypes.find(std::string("boxer_rebels"));
		REQUIRE(nvit != context.map_of_rebeltypes.end());
		REQUIRE(bool(nvit->second.id) == true);
	}
	auto map = open_directory(root, NATIVE("map"));
	{
		auto def_map_file = open_file(map, NATIVE("default.map"));
		if(def_map_file) {
			auto content = view_contents(*def_map_file);
			err.file_name = "default.map";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_default_map_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->world.province_size() == size_t(3248));
	}
	{
		auto def_csv_file = open_file(map, NATIVE("definition.csv"));
		if(def_csv_file) {
			auto content = view_contents(*def_csv_file);
			err.file_name = "definition.csv";
			parsers::read_map_colors(content.data, content.data + content.file_size, err, context);
		}
		REQUIRE(err.accumulated_errors == "");
		REQUIRE(context.map_color_to_province_id.size() != size_t(0));
		auto clr = sys::pack_color(4, 78, 135);
		auto it = context.map_color_to_province_id.find(clr);
		REQUIRE(it != context.map_color_to_province_id.end());
		auto id = it->second;
		REQUIRE(id);
		REQUIRE(id.index() < state->province_definitions.first_sea_province.index());
		//REQUIRE(context.prov_id_to_original_id_map[id].is_sea == false);
		REQUIRE(context.prov_id_to_original_id_map[id].id == 2702);
	}
	{
		auto terrain_file = open_file(map, NATIVE("terrain.txt"));
		if(terrain_file) {
			auto content = view_contents(*terrain_file);
			err.file_name = "terrain.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_terrain_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");


		auto nvit = context.map_of_modifiers.find(std::string("mountain"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;
		REQUIRE(state->world.modifier_get_province_values(id).get_offet_at_index(0) == sys::provincial_mod_offsets::movement_cost);
		REQUIRE(state->world.modifier_get_province_values(id).get_offet_at_index(1) == sys::provincial_mod_offsets::defense);

		REQUIRE(state->world.modifier_get_province_values(id).values[0] == Approx(1.4f));
		REQUIRE(state->world.modifier_get_province_values(id).values[1] == Approx(2.0f));

		REQUIRE(state->province_definitions.modifier_by_terrain_index[24] == id);
		REQUIRE(state->province_definitions.color_by_terrain_index[24] == sys::pack_color(117, 108, 119));
	}
	{
		auto region_file = open_file(map, NATIVE("region.txt"));
		if(region_file) {
			auto content = view_contents(*region_file);
			err.file_name = "region.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_region_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");
		auto id721 = fatten(state->world, context.original_id_to_prov_id_map[721]);
		auto id724 = fatten(state->world, context.original_id_to_prov_id_map[724]);
		auto absa = id721.get_abstract_state_membership_as_province().get_state();
		auto absb = id724.get_abstract_state_membership_as_province().get_state();
		REQUIRE(absa == absb);
	}
	{
		auto continent_file = open_file(map, NATIVE("continent.txt"));
		if(continent_file) {
			auto content = view_contents(*continent_file);
			err.file_name = "continent.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_continent_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("asia"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;

		REQUIRE(state->world.modifier_get_province_values(id).get_offet_at_index(0) == sys::provincial_mod_offsets::assimilation_rate);

		REQUIRE(state->world.modifier_get_province_values(id).values[0] == Approx(-0.5f));
		REQUIRE(state->world.province_get_continent(context.original_id_to_prov_id_map[2702]) == id);
	}
	{
		auto climate_file = open_file(map, NATIVE("climate.txt"));
		if(climate_file) {
			auto content = view_contents(*climate_file);
			err.file_name = "climate.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_climate_file(gen, err, context);
		}

		//inhospitable_climate


		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("inhospitable_climate"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;

		REQUIRE(state->world.modifier_get_province_values(id).get_offet_at_index(0) == sys::provincial_mod_offsets::farm_rgo_size);

		REQUIRE(state->world.modifier_get_province_values(id).values[0] == 0.0f);
		REQUIRE(state->world.province_get_climate(context.original_id_to_prov_id_map[2702]) == id);
	}
	{
		auto tech_file = open_file(common, NATIVE("technology.txt"));
		if(tech_file) {
			auto content = view_contents(*tech_file);
			err.file_name = "technology.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_technology_main_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("army_tech_school"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;

		REQUIRE(state->world.modifier_get_national_values(id).get_offet_at_index(0) == sys::national_mod_offsets::army_tech_research_bonus);
		REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(0.15f));

		auto fit = context.map_of_tech_folders.find("naval_engineering");
		REQUIRE(fit != context.map_of_tech_folders.end());
		REQUIRE(state->culture_definitions.tech_folders[fit->second].category == culture::tech_category::navy);

		auto tit = context.map_of_technologies.find(std::string("modern_army_doctrine"));
		REQUIRE(tit != context.map_of_technologies.end());
		REQUIRE(bool(tit->second.id) == true);
	}
	{
		auto inventions = open_directory(root, NATIVE("inventions"));
		{
			parsers::tech_group_context invention_context{ context, culture::tech_category::army };
			auto i_file = open_file(inventions, NATIVE("army_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "army_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{ context, culture::tech_category::navy };
			auto i_file = open_file(inventions, NATIVE("navy_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "navy_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{ context, culture::tech_category::commerce };
			auto i_file = open_file(inventions, NATIVE("commerce_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "commerce_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{ context, culture::tech_category::culture };
			auto i_file = open_file(inventions, NATIVE("culture_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "culture_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{ context, culture::tech_category::industry };
			auto i_file = open_file(inventions, NATIVE("industry_inventions.txt"));
			if(i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "industry_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}

		REQUIRE(err.accumulated_errors == "");

		auto tit = context.map_of_inventions.find(std::string("direct_current"));
		REQUIRE(tit != context.map_of_inventions.end());
		REQUIRE(bool(tit->second.id) == true);
		REQUIRE(culture::tech_category(state->world.invention_get_technology_type(tit->second.id)) == culture::tech_category::industry);
	}
	{
		auto units = open_directory(root, NATIVE("units"));
		for(auto unit_file : simple_fs::list_files(units, NATIVE(".txt"))) {
			auto opened_file = open_file(unit_file);
			if(opened_file) {
				auto content = view_contents(*opened_file);
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_unit_file(gen, err, context);
			}
		}

		REQUIRE(err.accumulated_errors == "");

		auto uit = context.map_of_unit_types.find("battleship");
		REQUIRE(uit != context.map_of_unit_types.end());
		auto id = uit->second;

		REQUIRE(state->military_definitions.unit_base_definitions[id].active == false);
		REQUIRE(state->military_definitions.unit_base_definitions[id].defence_or_hull == 70.0f);
		REQUIRE(state->military_definitions.unit_base_definitions[id].supply_consumption_score == 50);
		REQUIRE(state->military_definitions.unit_base_definitions[id].build_cost.commodity_amounts[0] == 80.0f);
		REQUIRE(state->military_definitions.unit_base_definitions[id].build_cost.commodity_amounts[1] == 20.0f);
		REQUIRE(state->military_definitions.unit_base_definitions[id].build_cost.commodity_amounts[2] == 10.0f);
		REQUIRE(state->military_definitions.unit_base_definitions[id].build_cost.commodity_amounts[3] == 25.0f);
		REQUIRE(state->military_definitions.unit_base_definitions[id].build_cost.commodity_amounts[4] == 0.0f);
	}
	state->world.national_identity_resize_unit_names_count(uint32_t(state->military_definitions.unit_base_definitions.size()));
	state->world.national_identity_resize_unit_names_first(uint32_t(state->military_definitions.unit_base_definitions.size()));

	state->world.political_party_resize_party_issues(uint32_t(state->culture_definitions.party_issues.size()));

	state->world.province_resize_party_loyalty(state->world.ideology_size());

	state->world.pop_type_resize_everyday_needs(state->world.commodity_size());
	state->world.pop_type_resize_luxury_needs(state->world.commodity_size());
	state->world.pop_type_resize_life_needs(state->world.commodity_size());
	state->world.pop_type_resize_ideology(state->world.ideology_size());
	state->world.pop_type_resize_issues(state->world.issue_option_size());
	state->world.pop_type_resize_promotion(state->world.pop_type_size());

	{
		state->world.for_each_national_identity([&](dcon::national_identity_id i) {
			auto file_name = simple_fs::win1250_to_native(context.file_names_for_idents[i]);
			auto country_file = open_file(common, file_name);
			if(country_file) {
				parsers::country_file_context c_context{ context, i };
				auto content = view_contents(*country_file);
				err.file_name = context.file_names_for_idents[i];
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_country_file(gen, err, c_context);
			}
		});

		REQUIRE(err.accumulated_errors == "");

		auto name_num = nations::tag_to_int('M', 'E', 'X');
		auto it = context.map_of_ident_names.find(name_num);
		REQUIRE(it != context.map_of_ident_names.end());
		auto id = it->second;

		
		REQUIRE(sys::int_red_from_int(state->world.national_identity_get_color(id)) == 173);
		REQUIRE(sys::int_green_from_int(state->world.national_identity_get_color(id)) == 242);
		REQUIRE(sys::int_blue_from_int(state->world.national_identity_get_color(id)) == 175);
		REQUIRE(state->world.national_identity_get_color(id) == sys::pack_color(173, 242, 175));

		REQUIRE(state->world.national_identity_get_political_party_count(id) == uint8_t(12));

		auto first_party = state->world.national_identity_get_political_party_first(id);
		REQUIRE(state->world.political_party_get_end_date(first_party) == sys::date(sys::year_month_day{ 2000, uint16_t(1), uint16_t(1) }, state->start_date));
		REQUIRE(context.map_of_ideologies.find(std::string("conservative"))->second.id == state->world.political_party_get_ideology(first_party));
	}
	auto history = open_directory(root, NATIVE("history"));
	{
		auto prov_history = open_directory(history, NATIVE("provinces"));
		for(auto subdir : list_subdirectories(prov_history)) {
			for(auto prov_file : list_files(subdir, NATIVE(".txt"))) {
				auto file_name = simple_fs::native_to_utf8(get_full_name(prov_file));
				auto name_begin = file_name.c_str();
				auto name_end = name_begin + file_name.length();
				for(; --name_end > name_begin; ) {
					if(isdigit(*name_end))
						break;
				}
				auto value_start = name_end;
				for(; value_start > name_begin; --value_start) {
					if(!isdigit(*value_start))
						break;
				}
				++value_start;
				++name_end;

				err.file_name = file_name;
				auto province_id = parsers::parse_int(std::string_view(value_start, name_end - value_start), 0, err);
				if(province_id > 0 && uint32_t(province_id) < context.original_id_to_prov_id_map.size()) {
					auto opened_file = open_file(prov_file);
					if(opened_file) {
						auto pid = context.original_id_to_prov_id_map[province_id];
						parsers::province_file_context pf_context{ context, pid };
						auto content = view_contents(*opened_file);
						parsers::token_generator gen(content.data, content.data + content.file_size);
						parsers::parse_province_history_file(gen, err, pf_context);
					}
				}
			}
		}

		REQUIRE(err.accumulated_errors == "");
		auto france_tag = context.map_of_ident_names.find(nations::tag_to_int('F', 'R', 'A'))->second;
		auto ident_rel = state->world.national_identity_get_identity_holder(france_tag);
		auto france = state->world.identity_holder_get_nation(ident_rel);

		auto prov = fatten(state->world, context.original_id_to_prov_id_map[408]);
		REQUIRE(prov.get_nation_from_province_ownership() == france);
		REQUIRE(prov.get_nation_from_province_control() == france);
		REQUIRE(prov.get_rgo() == context.map_of_commodity_names.find(std::string("fruit"))->second);
		REQUIRE(prov.get_life_rating() == uint8_t(34));
		REQUIRE(prov.get_railroad_level() == uint8_t(0));

		bool found_france = false;
		for(auto cr : prov.get_core()) {
			if(cr.get_identity() == france_tag)
				found_france = true;
		}
		REQUIRE(found_france);
	}

	// load pop history files
	{
		auto pop_history = open_directory(history, NATIVE("pops"));
		auto startdate = sys::date(0).to_ymd(state->start_date);
		auto start_dir_name = std::to_string(startdate.year) + "." + std::to_string(startdate.month) + "." + std::to_string(startdate.day);
		auto date_directory = open_directory(pop_history, simple_fs::utf8_to_native(start_dir_name));

		for(auto pop_file : list_files(date_directory, NATIVE(".txt"))) {
			auto opened_file = open_file(pop_file);
			if(opened_file) {
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_pop_history_file(gen, err, context);
			}
		}


		dcon::pop_type_id ptype = context.map_of_poptypes.find(std::string("artisans"))->second;
		dcon::culture_id cid = context.map_of_culture_names.find(std::string("british"))->second;
		dcon::religion_id rid = context.map_of_religion_names.find(std::string("protestant"))->second;

		float count = 0;

		for(auto pops_by_location : state->world.province_get_pop_location(context.original_id_to_prov_id_map[302])) {
			auto pop_id = pops_by_location.get_pop();
			if(pop_id.get_culture() == cid && pop_id.get_poptype() == ptype && pop_id.get_religion() == rid) {
				count = pop_id.get_size();
			}
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(count == 11250.0f);
	}
	// load poptype definitions
	{
		auto poptypes = open_directory(root, NATIVE("poptypes"));
		for(auto pr : context.map_of_poptypes) {
			auto opened_file = open_file(poptypes, simple_fs::utf8_to_native(pr.first + ".txt"));
			if(opened_file) {
				err.file_name = pr.first + ".txt";
				auto content = view_contents(*opened_file);
				parsers::poptype_context inner_context{ context, pr.second };
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_poptype_file(gen, err, inner_context);
			}
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(bool(state->culture_definitions.artisans) == true);
		REQUIRE(state->world.pop_type_get_sprite(state->culture_definitions.artisans) == uint8_t(11));
		REQUIRE(state->world.pop_type_get_color(state->culture_definitions.artisans) == sys::pack_color(127, 3, 3));
		REQUIRE(state->world.pop_type_get_strata(state->culture_definitions.artisans) == uint8_t(culture::pop_strata::middle));

		auto wine = context.map_of_commodity_names.find("wine")->second;
		REQUIRE(state->world.pop_type_get_luxury_needs(state->culture_definitions.artisans, wine) == 10.0f);
		REQUIRE(state->value_modifiers[state->world.pop_type_get_country_migration_target(state->culture_definitions.artisans)].base_factor == 1.0f);

		auto react = context.map_of_ideologies.find("reactionary")->second.id;
		REQUIRE(state->value_modifiers[state->world.pop_type_get_ideology(state->culture_definitions.artisans, react)].base_factor == 1.0f);
	}
	// load ideology contents
	{
		for(auto& pr : context.map_of_ideologies) {
			parsers::individual_ideology_context new_context{ context, pr.second.id };
			parsers::parse_individual_ideology(pr.second.generator_state, err, new_context);
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(bool(state->culture_definitions.conservative) == true);
		REQUIRE(state->world.ideology_get_color(state->culture_definitions.conservative) == sys::pack_color(10, 10, 250));
		auto mkey = state->world.ideology_get_add_economic_reform(state->culture_definitions.conservative);
		REQUIRE(state->value_modifiers[mkey].base_factor == -0.5f);
	}
	// triggered modifier contents
	{
		for(auto& r : context.set_of_triggered_modifiers) {
			state->national_definitions.triggered_modifiers[r.index].trigger_condition = parsers::read_triggered_modifier_condition(r.generator_state, err, context);
		}
		REQUIRE(err.accumulated_errors == "");
	}
}
#endif
