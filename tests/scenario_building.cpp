#include "catch.hpp"
#include "parsers_declarations.hpp"
#include "dcon_generated.hpp"
#include "nations.hpp"
#include "container_types.hpp"
#include "system_state.hpp"
#include "serialization.hpp"

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

	auto map = open_directory(root, NATIVE("map"));
	{
		auto def_map_file = open_file(map, NATIVE("default.map"));
		if (def_map_file) {
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
		if (def_csv_file) {
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
		// REQUIRE(context.prov_id_to_original_id_map[id].is_sea == false);
		REQUIRE(context.prov_id_to_original_id_map[id].id == 2702);
	}
	{
		err.file_name = "adjacencies.csv";
		auto adj_csv_file = open_file(map, NATIVE("adjacencies.csv"));
		if (adj_csv_file) {
			auto adj_content = view_contents(*adj_csv_file);
			parsers::read_map_adjacency(adj_content.data, adj_content.data + adj_content.file_size, err, context);
		}
	}

	std::thread map_loader([&]() {
		state->map_state.load_map_data(context);
	});

	parsers::make_leader_images(context);

	// COUNTRIES
	{
		auto countries = open_file(common, NATIVE("countries.txt"));
		REQUIRE(countries);
		if (countries) {
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
	// RELIGION
	{
		auto religion = open_file(common, NATIVE("religion.txt"));
		if (religion) {
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
		if (cultures) {
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
		//REQUIRE(state->world.culture_group_get_leader(idb) == uint8_t(sys::leader_type::european));
		REQUIRE(idb.get_is_overseas() == false);
		int32_t count = 0;
		for (auto c : idb.get_culture_group_membership())
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
		if (state->world.commodity_size() == 0) {
			// create money
			auto money_id = state->world.create_commodity();
			assert(money_id.index() == 0);
		}
		auto goods = open_file(common, NATIVE("goods.txt"));
		if (goods) {
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
	{
		auto buildings = open_file(common, NATIVE("buildings.txt"));
		if (buildings) {
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
		// REQUIRE(id.get_construction_costs(context.map_of_commodity_names.find(std::string("machine_parts"))->second) == 80.0f);

		REQUIRE(bool(state->economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].province_modifier) == true);
		sys::provincial_modifier_definition pmod = state->world.modifier_get_province_values(state->economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].province_modifier);
		REQUIRE(pmod.offsets[0] == sys::provincial_mod_offsets::movement_cost);
		REQUIRE(pmod.values[0] == Approx(-0.05f));
	}
	{
		context.ideologies_file = open_file(common, NATIVE("ideologies.txt"));
		if (context.ideologies_file) {
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
		for (auto c : fata.get_ideology_group_membership_as_ideology_group())
			++count;
		REQUIRE(count == 2);
	}
	{
		context.issues_file = open_file(common, NATIVE("issues.txt"));
		if (context.issues_file) {
			auto content = view_contents(*context.issues_file);
			err.file_name = "issues.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_issues_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		// school_reforms
		auto ita = context.map_of_iissues.find(std::string("school_reforms"));
		REQUIRE(ita != context.map_of_iissues.end());

		auto fata = fatten(state->world, ita->second);
		REQUIRE(fata.get_is_next_step_only() == true);
		REQUIRE(fata.get_is_administrative() == true);

		auto itb = context.map_of_ioptions.find(std::string("acceptable_schools"));
		REQUIRE(itb != context.map_of_ioptions.end());
		auto fatb = fatten(state->world, itb->second.id);
		REQUIRE(fata.get_options().at(2) == fatb);

		REQUIRE(std::find(state->culture_definitions.social_issues.begin(), state->culture_definitions.social_issues.end(), ita->second) != state->culture_definitions.social_issues.end());
	}
	{
		auto governments = open_file(common, NATIVE("governments.txt"));
		if (governments) {
			auto content = view_contents(*governments);
			err.file_name = "governments.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_governments_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		// prussian_constitutionalism

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
		if (context.cb_types_file) {
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
		if (traits) {
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
		if (context.crimes_file) {
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
		if (context.triggered_modifiers_file) {
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
		if (nv_file) {
			auto content = view_contents(*nv_file);
			err.file_name = "nationalvalues.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_values_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("nv_liberty"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;
		REQUIRE(state->world.modifier_get_national_values(id).offsets[0] == sys::national_mod_offsets::mobilization_size);
		REQUIRE(state->world.modifier_get_national_values(id).offsets[1] == sys::national_mod_offsets::mobilization_impact);
		REQUIRE(bool(state->world.modifier_get_national_values(id).offsets[2]) == false);

		REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(0.02f));
		REQUIRE(state->world.modifier_get_national_values(id).values[1] == Approx(0.75f));
	}
	{
		auto sm_file = open_file(common, NATIVE("static_modifiers.txt"));
		if (sm_file) {
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
		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).offsets[0] == sys::provincial_mod_offsets::farm_rgo_eff);
		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).offsets[1] == sys::provincial_mod_offsets::mine_rgo_eff);
		REQUIRE(bool(state->world.modifier_get_province_values(state->national_definitions.has_siege).offsets[2]) == false);

		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).values[0] == Approx(-0.5f));
		REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).values[1] == Approx(-0.5f));
	}
	{
		auto em_file = open_file(common, NATIVE("event_modifiers.txt"));
		if (em_file) {
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

		REQUIRE(state->world.modifier_get_national_values(id).offsets[0] == sys::national_mod_offsets::factory_throughput);
		REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(-0.2f));
	}
	{
		auto defines_file = open_file(common, NATIVE("defines.lua"));
		if (defines_file) {
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
		if (context.rebel_types_file) {
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
	{
		auto terrain_file = open_file(map, NATIVE("terrain.txt"));
		if (terrain_file) {
			auto content = view_contents(*terrain_file);
			err.file_name = "terrain.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_terrain_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("mountain"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;
		REQUIRE(state->world.modifier_get_province_values(id).offsets[0] == sys::provincial_mod_offsets::movement_cost);
		REQUIRE(state->world.modifier_get_province_values(id).offsets[1] == sys::provincial_mod_offsets::defense);

		REQUIRE(state->world.modifier_get_province_values(id).values[0] == Approx(1.4f));
		REQUIRE(state->world.modifier_get_province_values(id).values[1] == Approx(2.0f));

		REQUIRE(context.modifier_by_terrain_index[24] == id);
		REQUIRE(context.color_by_terrain_index[24] == sys::pack_color(117, 108, 119));
	}
	{
		auto region_file = open_file(map, NATIVE("region.txt"));
		if (region_file) {
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
		if (continent_file) {
			auto content = view_contents(*continent_file);
			err.file_name = "continent.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_continent_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("asia"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;

		REQUIRE(state->world.modifier_get_province_values(id).offsets[0] == sys::provincial_mod_offsets::assimilation_rate);

		REQUIRE(state->world.modifier_get_province_values(id).values[0] == Approx(-0.5f));
		REQUIRE(state->world.province_get_continent(context.original_id_to_prov_id_map[2702]) == id);
	}
	{
		auto climate_file = open_file(map, NATIVE("climate.txt"));
		if (climate_file) {
			auto content = view_contents(*climate_file);
			err.file_name = "climate.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_climate_file(gen, err, context);
		}

		// inhospitable_climate

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("inhospitable_climate"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;

		REQUIRE(state->world.modifier_get_province_values(id).offsets[0] == sys::provincial_mod_offsets::farm_rgo_size);

		REQUIRE(state->world.modifier_get_province_values(id).values[0] == 0.0f);
		REQUIRE(state->world.province_get_climate(context.original_id_to_prov_id_map[2702]) == id);
	}
	{
		auto tech_file = open_file(common, NATIVE("technology.txt"));
		if (tech_file) {
			auto content = view_contents(*tech_file);
			err.file_name = "technology.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_technology_main_file(gen, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto nvit = context.map_of_modifiers.find(std::string("army_tech_school"));
		REQUIRE(nvit != context.map_of_modifiers.end());
		auto id = nvit->second;

		REQUIRE(state->world.modifier_get_national_values(id).offsets[0] == sys::national_mod_offsets::army_tech_research_bonus);
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
			parsers::tech_group_context invention_context{context, culture::tech_category::army};
			auto i_file = open_file(inventions, NATIVE("army_inventions.txt"));
			if (i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "army_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::navy};
			auto i_file = open_file(inventions, NATIVE("navy_inventions.txt"));
			if (i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "navy_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::commerce};
			auto i_file = open_file(inventions, NATIVE("commerce_inventions.txt"));
			if (i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "commerce_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::culture};
			auto i_file = open_file(inventions, NATIVE("culture_inventions.txt"));
			if (i_file) {
				auto content = view_contents(*i_file);
				err.file_name = "culture_inventions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_inventions_file(gen, err, invention_context);
				context.tech_and_invention_files.emplace_back(std::move(*i_file));
			}
		}
		{
			parsers::tech_group_context invention_context{context, culture::tech_category::industry};
			auto i_file = open_file(inventions, NATIVE("industry_inventions.txt"));
			if (i_file) {
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
		parsers::make_base_units(context);
		auto units = open_directory(root, NATIVE("units"));
		for (auto unit_file : simple_fs::list_files(units, NATIVE(".txt"))) {
			auto opened_file = open_file(unit_file);
			if (opened_file) {
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
	state->world.province_resize_building_level(economy::max_building_types);

	state->world.pop_type_resize_everyday_needs(state->world.commodity_size());
	state->world.pop_type_resize_luxury_needs(state->world.commodity_size());
	state->world.pop_type_resize_life_needs(state->world.commodity_size());
	state->world.pop_type_resize_ideology(state->world.ideology_size());
	state->world.pop_type_resize_issues(state->world.issue_option_size());
	state->world.pop_type_resize_promotion(state->world.pop_type_size());

	state->world.national_focus_resize_production_focus(state->world.commodity_size());

	state->world.technology_resize_activate_building(state->world.factory_type_size());
	state->world.technology_resize_activate_unit(uint32_t(state->military_definitions.unit_base_definitions.size()));
	state->world.technology_resize_increase_building(uint32_t(economy::max_building_types));

	state->world.invention_resize_activate_building(state->world.factory_type_size());
	state->world.invention_resize_activate_unit(uint32_t(state->military_definitions.unit_base_definitions.size()));
	state->world.invention_resize_activate_crime(uint32_t(state->culture_definitions.crimes.size()));

	state->world.rebel_type_resize_government_change(uint32_t(state->culture_definitions.governments.size()));

	state->world.nation_resize_max_building_level(economy::max_building_types);
	state->world.nation_resize_active_inventions(state->world.invention_size());
	state->world.nation_resize_active_technologies(state->world.technology_size());
	state->world.nation_resize_upper_house(state->world.ideology_size());

	state->world.national_identity_resize_government_flag_type(uint32_t(state->culture_definitions.governments.size()));
	{
		state->world.for_each_national_identity([&](dcon::national_identity_id i) {
			auto file_name = simple_fs::win1250_to_native(context.file_names_for_idents[i]);
			auto country_file = open_file(common, file_name);
			if (country_file) {
				parsers::country_file_context c_context{context, i};
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
		REQUIRE(state->world.political_party_get_end_date(first_party) == sys::date(sys::year_month_day{2000, uint16_t(1), uint16_t(1)}, state->start_date));
		REQUIRE(context.map_of_ideologies.find(std::string("conservative"))->second.id == state->world.political_party_get_ideology(first_party));
	}
	auto history = open_directory(root, NATIVE("history"));
	{
		auto prov_history = open_directory(history, NATIVE("provinces"));
		for (auto subdir : list_subdirectories(prov_history)) {
			for (auto prov_file : list_files(subdir, NATIVE(".txt"))) {
				auto file_name = simple_fs::native_to_utf8(get_full_name(prov_file));
				auto name_begin = file_name.c_str();
				auto name_end = name_begin + file_name.length();
				for (; --name_end > name_begin;) {
					if (isdigit(*name_end))
						break;
				}
				auto value_start = name_end;
				for (; value_start > name_begin; --value_start) {
					if (!isdigit(*value_start))
						break;
				}
				++value_start;
				++name_end;

				err.file_name = file_name;
				auto province_id = parsers::parse_int(std::string_view(value_start, name_end - value_start), 0, err);
				if (province_id > 0 && uint32_t(province_id) < context.original_id_to_prov_id_map.size()) {
					auto opened_file = open_file(prov_file);
					if (opened_file) {
						auto pid = context.original_id_to_prov_id_map[province_id];
						parsers::province_file_context pf_context{context, pid};
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
		REQUIRE(prov.get_building_level(economy::province_building_type::railroad) == uint8_t(0));

		bool found_france = false;
		for (auto cr : prov.get_core()) {
			if (cr.get_identity() == france_tag)
				found_france = true;
		}
		REQUIRE(found_france);
	}

	culture::set_default_issue_and_reform_options(*state);

	// load pop history files
	{
		auto pop_history = open_directory(history, NATIVE("pops"));
		auto startdate = sys::date(0).to_ymd(state->start_date);
		auto start_dir_name = std::to_string(startdate.year) + "." + std::to_string(startdate.month) + "." + std::to_string(startdate.day);
		auto date_directory = open_directory(pop_history, simple_fs::utf8_to_native(start_dir_name));

		for (auto pop_file : list_files(date_directory, NATIVE(".txt"))) {
			auto opened_file = open_file(pop_file);
			if (opened_file) {
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

		for (auto pops_by_location : state->world.province_get_pop_location(context.original_id_to_prov_id_map[302])) {
			auto pop_id = pops_by_location.get_pop();
			if (pop_id.get_culture() == cid && pop_id.get_poptype() == ptype && pop_id.get_religion() == rid) {
				count = pop_id.get_size();
			}
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(count == 11250.0f);
	}
	// load poptype definitions
	{
		auto poptypes = open_directory(root, NATIVE("poptypes"));
		for (auto pr : context.map_of_poptypes) {
			auto opened_file = open_file(poptypes, simple_fs::utf8_to_native(pr.first + ".txt"));
			if (opened_file) {
				err.file_name = pr.first + ".txt";
				auto content = view_contents(*opened_file);
				parsers::poptype_context inner_context{context, pr.second};
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
		REQUIRE(state->value_modifiers[state->world.pop_type_get_country_migration_target(state->culture_definitions.artisans)].factor == 1.0f);

		auto react = context.map_of_ideologies.find("reactionary")->second.id;
		REQUIRE(state->value_modifiers[state->world.pop_type_get_ideology(state->culture_definitions.artisans, react)].factor == 1.0f);
	}
	// load ideology contents
	{
		for (auto &pr : context.map_of_ideologies) {
			parsers::individual_ideology_context new_context{context, pr.second.id};
			parsers::parse_individual_ideology(pr.second.generator_state, err, new_context);
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(bool(state->culture_definitions.conservative) == true);
		REQUIRE(state->world.ideology_get_color(state->culture_definitions.conservative) == sys::pack_color(10, 10, 250));
		auto mkey = state->world.ideology_get_add_economic_reform(state->culture_definitions.conservative);
		REQUIRE(state->value_modifiers[mkey].factor == -0.5f);
	}
	// triggered modifier contents
	{
		for (auto &r : context.set_of_triggered_modifiers) {
			state->national_definitions.triggered_modifiers[r.index].trigger_condition = parsers::read_triggered_modifier_condition(r.generator_state, err, context);
		}
		REQUIRE(err.accumulated_errors == "");
	}
	// cb contents
	{
		err.file_name = "cb_types.txt";
		for (auto &r : context.map_of_cb_types) {
			parsers::individual_cb_context new_context{context, r.second.id};
			parsers::parse_cb_body(r.second.generator_state, err, new_context);
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(bool(state->military_definitions.standard_civil_war) == true);
		REQUIRE(state->world.cb_type_get_sprite_index(state->military_definitions.standard_civil_war) == uint8_t(2));
		REQUIRE(state->world.cb_type_get_months(state->military_definitions.standard_civil_war) == uint8_t(1));
		REQUIRE(state->world.cb_type_get_peace_cost_factor(state->military_definitions.standard_civil_war) == 1.0f);
		REQUIRE(state->world.cb_type_get_break_truce_militancy_factor(state->military_definitions.standard_civil_war) == 2.0f);
		REQUIRE(state->world.cb_type_get_truce_months(state->military_definitions.standard_civil_war) == uint8_t(0));
		REQUIRE(state->world.cb_type_get_type_bits(state->military_definitions.standard_civil_war) ==
		        uint32_t(military::cb_flag::is_civil_war | military::cb_flag::po_annex | military::cb_flag::is_triggered_only | military::cb_flag::is_not_constructing_cb | military::cb_flag::not_in_crisis));
		REQUIRE(bool(state->world.cb_type_get_can_use(state->military_definitions.standard_civil_war)) == true);
		REQUIRE(bool(state->world.cb_type_get_on_add(state->military_definitions.standard_civil_war)) == true);
		REQUIRE(bool(state->world.cb_type_get_allowed_states(state->military_definitions.standard_civil_war)) == false);
	}
	// pending crimes
	{
		err.file_name = "crime.txt";
		for (auto &r : context.map_of_crimes) {
			parsers::read_pending_crime(r.second.id, r.second.generator_state, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto ita = context.map_of_crimes.find(std::string("machine_politics"));
		REQUIRE(state->culture_definitions.crimes[ita->second.id].available_by_default == true);
		auto mod_id = state->culture_definitions.crimes[ita->second.id].modifier;
		REQUIRE(bool(mod_id) == true);
		REQUIRE(state->world.modifier_get_icon(mod_id) == uint8_t(4));
		REQUIRE(state->world.modifier_get_province_values(mod_id).offsets[0] == sys::provincial_mod_offsets::boost_strongest_party);
		REQUIRE(state->world.modifier_get_province_values(mod_id).values[0] == 5.0f);
	}
	// pending issue/ reform options
	{
		err.file_name = "issues.txt";
		for (auto &r : context.map_of_ioptions) {
			parsers::read_pending_option(r.second.id, r.second.generator_state, err, context);
		}
		for (auto &r : context.map_of_roptions) {
			parsers::read_pending_reform(r.second.id, r.second.generator_state, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		REQUIRE(bool(state->culture_definitions.jingoism) == true);

		auto itb = context.map_of_ioptions.find(std::string("acceptable_schools"));
		auto fatb = fatten(state->world, itb->second.id);
		REQUIRE(fatb.get_administrative_multiplier() == 2.0f);
		auto mid = fatb.get_modifier();
		REQUIRE(bool(mid) == true);
		REQUIRE(state->world.modifier_get_national_values(mid).offsets[0] == sys::national_mod_offsets::education_efficiency_modifier);
	}
	// parse national_focus.txt
	{
		auto nat_focus = open_file(common, NATIVE("national_focus.txt"));
		if (nat_focus) {
			auto content = view_contents(*nat_focus);
			err.file_name = "national_focus.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_national_focus_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/national_focus.txt could not be opened\n";
		}

		REQUIRE(err.accumulated_errors == "");

		REQUIRE(bool(state->national_definitions.flashpoint_focus) == true);
		REQUIRE(state->world.national_focus_get_icon(state->national_definitions.flashpoint_focus) == uint8_t(4));
		REQUIRE(state->world.national_focus_get_type(state->national_definitions.flashpoint_focus) == uint8_t(nations::focus_type::diplomatic_focus));
		REQUIRE(bool(state->world.national_focus_get_limit(state->national_definitions.flashpoint_focus)) == true);
	}
	// load pop_types.txt
	{
		auto pop_types_file = open_file(common, NATIVE("pop_types.txt"));
		if (pop_types_file) {
			auto content = view_contents(*pop_types_file);
			err.file_name = "pop_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_main_pop_type_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/pop_types.txt could not be opened\n";
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(bool(state->culture_definitions.migration_chance) == true);
	}

	// read pending techs
	{
		err.file_name = "technology file";
		for (auto &r : context.map_of_technologies) {
			parsers::read_pending_technology(r.second.id, r.second.generator_state, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto it = context.map_of_technologies.find(std::string("modern_army_doctrine"));
		auto fit = fatten(state->world, it->second.id);
		REQUIRE(fit.get_year() == 1919);
		REQUIRE(fit.get_cost() == 21600);
		auto unit_adj = fit.get_modified_units();
		REQUIRE(unit_adj.size() == 1);
		REQUIRE(unit_adj[0].type == state->military_definitions.base_army_unit);
		REQUIRE(unit_adj[0].supply_consumption == Approx(0.20f));
		REQUIRE(bool(fit.get_modifier()) == true);
		REQUIRE(fit.get_modifier().get_national_values().offsets[0] == sys::national_mod_offsets::dig_in_cap);
		REQUIRE(fit.get_modifier().get_national_values().values[0] == 1.0f);
		REQUIRE(fit.get_increase_building(economy::province_building_type::fort) == true);
	}
	// read pending inventions
	{
		err.file_name = "inventions file";
		for (auto &r : context.map_of_inventions) {
			parsers::read_pending_invention(r.second.id, r.second.generator_state, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto it = context.map_of_inventions.find(std::string("the_talkies"));
		auto fit = fatten(state->world, it->second.id);

		REQUIRE(bool(fit.get_limit()) == true);
		REQUIRE(bool(fit.get_chance()) == true);
		REQUIRE(fit.get_shared_prestige() == 20.0f);
		REQUIRE(bool(fit.get_modifier()) == true);
		REQUIRE(fit.get_modifier().get_national_values().offsets[0] == sys::national_mod_offsets::suppression_points_modifier);
		REQUIRE(fit.get_modifier().get_national_values().offsets[1] == sys::national_mod_offsets::core_pop_consciousness_modifier);
		REQUIRE(fit.get_modifier().get_national_values().values[0] == Approx(-0.05f));
		REQUIRE(fit.get_modifier().get_national_values().values[1] == Approx(0.01f));
	}
	// parse on_actions.txt
	{
		auto on_action = open_file(common, NATIVE("on_actions.txt"));
		if (on_action) {
			auto content = view_contents(*on_action);
			err.file_name = "on_actions.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);
			parsers::parse_on_action_file(gen, err, context);
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/on_actions.txt could not be opened\n";
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->national_definitions.on_civilize.size() == size_t(3));
		REQUIRE(state->national_definitions.on_civilize[0].chance == int16_t(100));
	}
	// parse production_types.txt
	{
		auto prod_types = open_file(common, NATIVE("production_types.txt"));
		if (prod_types) {
			auto content = view_contents(*prod_types);
			err.file_name = "production_types.txt";
			parsers::token_generator gen(content.data, content.data + content.file_size);

			parsers::production_context new_context{context};
			parsers::parse_production_types_file(gen, err, new_context);

			for(const auto ft : state->world.in_factory_type) {
				if(!bool(state->world.factory_type_get_output(ft))) {
					err.accumulated_errors += "No output defined for factory " + std::string(text::produce_simple_string(*state, state->world.factory_type_get_name(ft))) + " (" + err.file_name + ")\n";
				}
			}
			if (!new_context.found_worker_types) {
				err.fatal = true;
				err.accumulated_errors += "Unable to identify factory worker types from production_types.txt\n";
			}
		} else {
			err.fatal = true;
			err.accumulated_errors += "File common/production_types.txt could not be opened\n";
		}

		REQUIRE(err.accumulated_errors == "");
		REQUIRE(err.accumulated_warnings == "");

		auto it = context.map_of_factory_names.find(std::string("aeroplane_factory"));
		auto fac = fatten(state->world, it->second);
		REQUIRE(fac.get_base_workforce() == 10000);
		REQUIRE(fac.get_output_amount() == Approx(0.91f));
		REQUIRE(fac.get_bonus_2_amount() == Approx(0.25f));
		REQUIRE(fac.get_bonus_3_amount() == 0.0f);
		REQUIRE(state->economy_definitions.craftsmen_fraction == Approx(0.8f));
	}
	// read pending rebel types
	{
		err.file_name = "rebel_types.txt";
		for (auto &r : context.map_of_rebeltypes) {
			parsers::read_pending_rebel_type(r.second.id, r.second.generator_state, err, context);
		}

		REQUIRE(err.accumulated_errors == "");

		auto it = context.map_of_rebeltypes.find(std::string("boxer_rebels"));
		auto fid = fatten(state->world, it->second.id);

		REQUIRE(fid.get_icon() == uint8_t(1));
		REQUIRE(fid.get_break_alliance_on_win() == true);
		REQUIRE(fid.get_area() == uint8_t(culture::rebel_area::nation));

		auto gid = context.map_of_governments.find("fascist_dictatorship")->second;
		auto gidb = context.map_of_governments.find("absolute_monarchy")->second;
		REQUIRE(fid.get_government_change(gid) == gidb);

		REQUIRE(fid.get_defection() == uint8_t(0));
		REQUIRE(fid.get_independence() == uint8_t(0));
		REQUIRE(fid.get_culture_restriction() == false);
		REQUIRE(fid.get_occupation_multiplier() == 1.0f);
	}
	// load decisions
	{
		auto decisions = open_directory(root, NATIVE("decisions"));
		for (auto decision_file : list_files(decisions, NATIVE(".txt"))) {
			auto opened_file = open_file(decision_file);
			if (opened_file) {
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_decision_file(gen, err, context);
			}
		}

		REQUIRE(err.accumulated_errors == "");
	}
	// load events
	{
		std::vector<simple_fs::file> held_open_files;
		auto events = open_directory(root, NATIVE("events"));
		for (auto event_file : list_files(events, NATIVE(".txt"))) {
			auto opened_file = open_file(event_file);
			if (opened_file) {
				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_event_file(gen, err, context);
				held_open_files.emplace_back(std::move(*opened_file));
			}
		}
		err.file_name = "pending events";
		parsers::commit_pending_events(err, context);

		REQUIRE(err.accumulated_errors == "");
		// REQUIRE(err.accumulated_warnings == "");
	}
	// load oob
	{
		auto oob_dir = open_directory(history, NATIVE("units"));
		for (auto oob_file : list_files(oob_dir, NATIVE(".txt"))) {
			auto file_name = get_full_name(oob_file);

			auto last = file_name.c_str() + file_name.length();
			auto first = file_name.c_str();
			auto start_of_name = last;
			for (; start_of_name >= first; --start_of_name) {
				if (*start_of_name == NATIVE('\\') || *start_of_name == NATIVE('/')) {
					++start_of_name;
					break;
				}
			}
			if (last - start_of_name >= 6 && file_name.ends_with(NATIVE("_oob.txt"))) {
				auto utf8name = simple_fs::native_to_utf8(native_string_view(start_of_name, last - start_of_name));

				if (auto it = context.map_of_ident_names.find(nations::tag_to_int(utf8name[0], utf8name[1], utf8name[2])); it != context.map_of_ident_names.end()) {
					auto holder = context.state.world.national_identity_get_nation_from_identity_holder(it->second);
					if (holder) {
						parsers::oob_file_context new_context{context, holder};

						auto opened_file = open_file(oob_file);
						if (opened_file) {
							err.file_name = utf8name;
							auto content = view_contents(*opened_file);
							parsers::token_generator gen(content.data, content.data + content.file_size);
							parsers::parse_oob_file(gen, err, new_context);
						}
					} else {
						// dead tag
					}
				} else {
					err.accumulated_errors += "invalid tag " + utf8name.substr(0, 3) + " encountered while scanning oob files\n";
				}
			}
		}

		REQUIRE(err.accumulated_errors == "");

		dcon::army_id frst;
		int32_t army_count = 0;
		for (auto aloc : state->world.province_get_army_location(context.original_id_to_prov_id_map[300])) {
			frst = aloc.get_army();
			++army_count;
		}
		REQUIRE(army_count == 1);
		REQUIRE(bool(frst) == true);
		int32_t reg_count = 0;
		bool from_294 = false;
		for (auto rmem : state->world.army_get_army_membership(frst)) {
			++reg_count;
			from_294 = from_294 ||
			           rmem.get_regiment().get_pop_from_regiment_source().get_province_from_pop_location() == context.original_id_to_prov_id_map[294];
		}
		REQUIRE(reg_count == 6);
		REQUIRE(from_294 == true);
	}

	// parse diplomacy history
	{
		auto diplomacy = open_directory(history, NATIVE("diplomacy"));
		{
			auto dip_file = open_file(diplomacy, NATIVE("Alliances.txt"));
			if (dip_file) {
				auto content = view_contents(*dip_file);
				err.file_name = "Alliances.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_alliance_file(gen, err, context);
			} else {
				err.accumulated_errors += "File history/diplomacy/Alliances.txt could not be opened\n";
			}
		}
		{
			auto dip_file = open_file(diplomacy, NATIVE("PuppetStates.txt"));
			if (dip_file) {
				auto content = view_contents(*dip_file);
				err.file_name = "PuppetStates.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_puppets_file(gen, err, context);
			} else {
				err.accumulated_errors += "File history/diplomacy/PuppetStates.txt could not be opened\n";
			}
		}
		{
			auto dip_file = open_file(diplomacy, NATIVE("Unions.txt"));
			if (dip_file) {
				auto content = view_contents(*dip_file);
				err.file_name = "Unions.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_union_file(gen, err, context);
			} else {
				err.accumulated_errors += "File history/diplomacy/Unions.txt could not be opened\n";
			}
		}

		REQUIRE(err.accumulated_errors == "");

		auto china_tag = context.map_of_ident_names.find(nations::tag_to_int('C', 'H', 'I'))->second;
		auto cident_rel = state->world.national_identity_get_identity_holder(china_tag);
		auto china = state->world.identity_holder_get_nation(cident_rel);

		// second = GXI
		auto gxi_tag = context.map_of_ident_names.find(nations::tag_to_int('G', 'X', 'I'))->second;
		auto gident_rel = state->world.national_identity_get_identity_holder(gxi_tag);
		auto gxi = fatten(state->world, state->world.identity_holder_get_nation(gident_rel));

		REQUIRE(bool(gxi.get_overlord_as_subject()) == true);
		REQUIRE(gxi.get_overlord_as_subject().get_ruler() == china);
		REQUIRE(gxi.get_is_substate() == true);
	}

	state->world.nation_resize_flag_variables(uint32_t(state->national_definitions.num_allocated_national_flags));

	std::vector<std::pair<dcon::nation_id, dcon::decision_id>> pending_decisions;
	// load country history
	{
		auto country_dir = open_directory(history, NATIVE("countries"));
		for (auto country_file : list_files(country_dir, NATIVE(".txt"))) {
			auto file_name = get_full_name(country_file);

			auto last = file_name.c_str() + file_name.length();
			auto first = file_name.c_str();
			auto start_of_name = last;
			for (; start_of_name >= first; --start_of_name) {
				if (*start_of_name == NATIVE('\\') || *start_of_name == NATIVE('/')) {
					++start_of_name;
					break;
				}
			}
			if (last - start_of_name >= 6) {
				auto utf8name = simple_fs::native_to_utf8(native_string_view(start_of_name, last - start_of_name));

				if (auto it = context.map_of_ident_names.find(nations::tag_to_int(utf8name[0], utf8name[1], utf8name[2])); it != context.map_of_ident_names.end()) {
					auto holder = context.state.world.national_identity_get_nation_from_identity_holder(it->second);

					parsers::country_history_context new_context{context, it->second, holder, pending_decisions };

					auto opened_file = open_file(country_file);
					if (opened_file) {
						err.file_name = utf8name;
						auto content = view_contents(*opened_file);
						parsers::token_generator gen(content.data, content.data + content.file_size);
						parsers::parse_country_history_file(gen, err, new_context);
					}

				} else {
					err.accumulated_errors += "invalid tag " + utf8name.substr(0, 3) + " encountered while scanning country history files\n";
				}
			}
		}

		REQUIRE(err.accumulated_errors == "");

		auto tag = fatten(state->world, context.map_of_ident_names.find(nations::tag_to_int('P', 'O', 'R'))->second);
		auto nation = tag.get_nation_from_identity_holder();

		REQUIRE(nation.get_plurality() == 25.0f);
		REQUIRE(nation.get_prestige() == 10.0f);
		REQUIRE(nation.get_is_civilized() == true);

		REQUIRE(nation.get_issues(context.map_of_iissues.find("voting_system")->second) == context.map_of_ioptions.find("jefferson_method")->second.id);
		REQUIRE(nation.get_active_technologies(context.map_of_technologies.find("alphabetic_flag_signaling")->second.id) == true);
	}
	// load war history
	{
		auto country_dir = open_directory(history, NATIVE("wars"));
		for (auto war_file : list_files(country_dir, NATIVE(".txt"))) {
			auto opened_file = open_file(war_file);
			if (opened_file) {
				parsers::war_history_context new_context{context};

				err.file_name = simple_fs::native_to_utf8(get_full_name(*opened_file));
				auto content = view_contents(*opened_file);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_war_history_file(gen, err, new_context);
			}
		}
		REQUIRE(err.accumulated_errors == "");
		REQUIRE(state->world.war_size() == uint32_t(2));
	}

	state->world.nation_resize_variables(uint32_t(state->national_definitions.num_allocated_national_variables));
	state->world.pop_resize_demographics(pop_demographics::size(*state));

	nations::generate_initial_state_instances(*state);
	state->world.nation_resize_stockpiles(state->world.commodity_size());

	state->national_definitions.global_flag_variables.resize((state->national_definitions.num_allocated_global_flags + 7) / 8, dcon::bitfield_type{0});

	state->world.for_each_ideology([&](dcon::ideology_id id) {
		if (!bool(state->world.ideology_get_activation_date(id))) {
			state->world.ideology_set_enabled(id, true);
		}
	});

	map_loader.join();

	// touch up adjacencies
	state->world.for_each_province_adjacency([&](dcon::province_adjacency_id id) {
		auto frel = fatten(state->world, id);
		auto prov_a = frel.get_connected_provinces(0);
		auto prov_b = frel.get_connected_provinces(1);
		if (prov_a.id.index() < state->province_definitions.first_sea_province.index() &&
		    prov_b.id.index() >= state->province_definitions.first_sea_province.index()) {
			frel.get_type() |= province::border::coastal_bit;
		} else if (prov_a.id.index() >= state->province_definitions.first_sea_province.index() &&
		           prov_b.id.index() < state->province_definitions.first_sea_province.index()) {
			frel.get_type() |= province::border::coastal_bit;
		}
		if (prov_a.get_state_from_abstract_state_membership() != prov_b.get_state_from_abstract_state_membership()) {
			frel.get_type() |= province::border::state_bit;
		}
		if (prov_a.get_nation_from_province_ownership() != prov_b.get_nation_from_province_ownership()) {
			frel.get_type() |= province::border::national_bit;
		}
	});

	// fill in the terrain type

	for (int32_t i = 0; i < state->province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id id{dcon::province_id::value_base_t(i)};
		if (!state->world.province_get_terrain(id)) { // don't overwrite if set by the history file
			auto modifier = context.modifier_by_terrain_index[state->map_state.map_data.median_terrain_type[province::to_map_id(id)]];
			state->world.province_set_terrain(id, modifier);
		}
	}
	for (int32_t i = state->province_definitions.first_sea_province.index(); i < int32_t(state->world.province_size()); ++i) {
		dcon::province_id id{dcon::province_id::value_base_t(i)};
		state->world.province_set_terrain(id, context.ocean_terrain);
	}

	state->fill_unsaved_data(); // we need this to run triggers

	// run pending triggers and effects
	for(auto pending_decision : pending_decisions) {
		dcon::nation_id n = pending_decision.first;
		dcon::decision_id d = pending_decision.second;
		if(auto e = state->world.decision_get_effect(d); e)
			effect::execute(*state, e, trigger::to_generic(n), trigger::to_generic(n), 0, uint32_t(state->current_date.value), uint32_t(n.index() << 4 ^ d.index()));
	}

	demographics::regenerate_from_pop_data(*state);
	economy::initialize(*state);

	culture::create_initial_ideology_and_issues_distribution(*state);
	demographics::regenerate_from_pop_data(*state);

	// serialize and reload

	{
		auto tag = fatten(state->world, context.map_of_ident_names.find(nations::tag_to_int('N', 'E', 'J'))->second);
		int32_t non_def_count = 0;

		state->world.for_each_reform([&](dcon::reform_id r) {
			auto optzero = state->world.reform_get_options(r)[0];
			if (tag.get_nation_from_identity_holder().get_reforms(r) != optzero)
				++non_def_count;
		});
		REQUIRE(non_def_count == 0);

		REQUIRE(tag.get_nation_from_identity_holder().get_modifier_values(sys::national_mod_offsets::civilization_progress_modifier) == 0.0f);
	}

	// Obtain filesystem state just before saving (see test below)
	const auto fs_str = simple_fs::extract_state(state->common_fs);
	sys::write_scenario_file(*state, NATIVE("sb_test_file.bin"), 1);

	state = nullptr;
	state = std::make_unique<sys::state>();

	auto load_result = sys::try_read_scenario_and_save_file(*state, NATIVE("sb_test_file.bin"));
	REQUIRE(load_result == true);

	state->fill_unsaved_data();

	// now ... retest everything

	// Ensure the filesystem state is properly loaded back
	REQUIRE(simple_fs::extract_state(state->common_fs) == fs_str);

	{
		auto tag = fatten(state->world, context.map_of_ident_names.find(nations::tag_to_int('N', 'E', 'J'))->second);
		int32_t non_def_count = 0;

		state->world.for_each_reform([&](dcon::reform_id r) {
			auto optzero = state->world.reform_get_options(r)[0];
			if (tag.get_nation_from_identity_holder().get_reforms(r) != optzero)
				++non_def_count;
		});
		REQUIRE(non_def_count == 0);

		REQUIRE(tag.get_nation_from_identity_holder().get_modifier_values(sys::national_mod_offsets::civilization_progress_modifier) == 0.0f);
	}

	// COUNTRIES
	{

	    {auto name_num = nations::tag_to_int('T', 'P', 'G');
	auto it = context.map_of_ident_names.find(name_num);
	REQUIRE(it != context.map_of_ident_names.end());
	auto id = it->second;
	REQUIRE(bool(id) == true);
}
{
	REQUIRE(nations::tag_to_int('M', 'E', 'X') != nations::tag_to_int('M', 'T', 'C'));
	auto name_num = nations::tag_to_int('M', 'E', 'X');
	auto it = context.map_of_ident_names.find(name_num);
	REQUIRE(it != context.map_of_ident_names.end());
}
}
// RELIGION
{
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
	//REQUIRE(state->world.culture_group_get_leader(idb) == uint8_t(sys::leader_type::european));
	REQUIRE(idb.get_is_overseas() == false);
	int32_t count = 0;
	for (auto c : idb.get_culture_group_membership())
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

    {auto it = context.map_of_commodity_names.find(std::string("aeroplanes"));
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
	auto it = context.map_of_production_types.find(std::string("liquor_distillery"));
	REQUIRE(it != context.map_of_production_types.end());
	auto id = fatten(state->world, it->second);
	REQUIRE(id.get_construction_time() == 730);
	REQUIRE(id.get_is_available_from_start() == true);
	// REQUIRE(id.get_construction_costs(context.map_of_commodity_names.find(std::string("machine_parts"))->second) == 80.0f);

	REQUIRE(bool(state->economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].province_modifier) == true);
	sys::provincial_modifier_definition pmod = state->world.modifier_get_province_values(state->economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].province_modifier);
	REQUIRE(pmod.offsets[0] == sys::provincial_mod_offsets::movement_cost);
	REQUIRE(pmod.values[0] == Approx(-0.05f));
}
{
	auto ita = context.map_of_ideology_groups.find(std::string("socialist_group"));
	REQUIRE(ita != context.map_of_ideology_groups.end());

	auto itb = context.map_of_ideologies.find(std::string("communist"));
	REQUIRE(itb != context.map_of_ideologies.end());

	auto fata = fatten(state->world, ita->second);
	auto fatb = fatten(state->world, itb->second.id);

	REQUIRE(fatb.get_ideology_group_membership_as_ideology().get_ideology_group() == fata);
	int32_t count = 0;
	for (auto c : fata.get_ideology_group_membership_as_ideology_group())
		++count;
	REQUIRE(count == 2);
}
{
	// school_reforms
	auto ita = context.map_of_iissues.find(std::string("school_reforms"));
	REQUIRE(ita != context.map_of_iissues.end());

	auto fata = fatten(state->world, ita->second);
	REQUIRE(fata.get_is_next_step_only() == true);
	REQUIRE(fata.get_is_administrative() == true);

	auto itb = context.map_of_ioptions.find(std::string("acceptable_schools"));
	REQUIRE(itb != context.map_of_ioptions.end());
	auto fatb = fatten(state->world, itb->second.id);
	REQUIRE(fata.get_options().at(2) == fatb);

	REQUIRE(std::find(state->culture_definitions.social_issues.begin(), state->culture_definitions.social_issues.end(), ita->second) != state->culture_definitions.social_issues.end());
}
{
	// prussian_constitutionalism

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
	REQUIRE(state->world.cb_type_size() > 5);
}
{

	auto ita = context.map_of_leader_traits.find(std::string("wretched"));
	REQUIRE(ita != context.map_of_leader_traits.end());

	auto fata = fatten(state->world, ita->second);
	REQUIRE(fata.get_attack() == -1.0f);
	REQUIRE(fata.get_experience() == 0.0f);
}
{
	auto ita = context.map_of_crimes.find(std::string("machine_politics"));
	REQUIRE(ita != context.map_of_crimes.end());
	REQUIRE(ita->second.id.index() != 0);
	REQUIRE(state->culture_definitions.crimes.size() > size_t(ita->second.id.index()));
}
{

	auto nvit = context.map_of_modifiers.find(std::string("nv_liberty"));
	REQUIRE(nvit != context.map_of_modifiers.end());
	auto id = nvit->second;
	REQUIRE(state->world.modifier_get_national_values(id).offsets[0] == sys::national_mod_offsets::mobilization_size);
	REQUIRE(state->world.modifier_get_national_values(id).offsets[1] == sys::national_mod_offsets::mobilization_impact);
	REQUIRE(bool(state->world.modifier_get_national_values(id).offsets[2]) == false);

	REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(0.02f));
	REQUIRE(state->world.modifier_get_national_values(id).values[1] == Approx(0.75f));
}
{

	REQUIRE(bool(state->national_definitions.has_siege) == true);
	REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).offsets[0] == sys::provincial_mod_offsets::farm_rgo_eff);
	REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).offsets[1] == sys::provincial_mod_offsets::mine_rgo_eff);
	REQUIRE(bool(state->world.modifier_get_province_values(state->national_definitions.has_siege).offsets[2]) == false);

	REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).values[0] == Approx(-0.5f));
	REQUIRE(state->world.modifier_get_province_values(state->national_definitions.has_siege).values[1] == Approx(-0.5f));
}
{

	auto nvit = context.map_of_modifiers.find(std::string("stock_market_crash"));
	REQUIRE(nvit != context.map_of_modifiers.end());
	auto id = nvit->second;

	REQUIRE(state->world.modifier_get_icon(id) == 12);

	REQUIRE(state->world.modifier_get_national_values(id).offsets[0] == sys::national_mod_offsets::factory_throughput);
	REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(-0.2f));
}
{
	auto nvit = context.map_of_poptypes.find(std::string("clergymen"));
	REQUIRE(nvit != context.map_of_poptypes.end());
	REQUIRE(bool(nvit->second) == true);
}
{

	auto nvit = context.map_of_rebeltypes.find(std::string("boxer_rebels"));
	REQUIRE(nvit != context.map_of_rebeltypes.end());
	REQUIRE(bool(nvit->second.id) == true);
}
{
	REQUIRE(state->world.province_size() == size_t(3248));
}
{
	REQUIRE(context.map_color_to_province_id.size() != size_t(0));
	auto clr = sys::pack_color(4, 78, 135);
	auto it = context.map_color_to_province_id.find(clr);
	REQUIRE(it != context.map_color_to_province_id.end());
	auto id = it->second;
	REQUIRE(id);
	REQUIRE(id.index() < state->province_definitions.first_sea_province.index());
	// REQUIRE(context.prov_id_to_original_id_map[id].is_sea == false);
	REQUIRE(context.prov_id_to_original_id_map[id].id == 2702);
}
{

	auto nvit = context.map_of_modifiers.find(std::string("mountain"));
	REQUIRE(nvit != context.map_of_modifiers.end());
	auto id = nvit->second;
	REQUIRE(state->world.modifier_get_province_values(id).offsets[0] == sys::provincial_mod_offsets::movement_cost);
	REQUIRE(state->world.modifier_get_province_values(id).offsets[1] == sys::provincial_mod_offsets::defense);

	REQUIRE(state->world.modifier_get_province_values(id).values[0] == Approx(1.4f));
	REQUIRE(state->world.modifier_get_province_values(id).values[1] == Approx(2.0f));

	REQUIRE(context.modifier_by_terrain_index[24] == id);
	REQUIRE(context.color_by_terrain_index[24] == sys::pack_color(117, 108, 119));
}
{
	auto id721 = fatten(state->world, context.original_id_to_prov_id_map[721]);
	auto id724 = fatten(state->world, context.original_id_to_prov_id_map[724]);
	auto absa = id721.get_abstract_state_membership_as_province().get_state();
	auto absb = id724.get_abstract_state_membership_as_province().get_state();
	REQUIRE(absa == absb);
}
{
	auto nvit = context.map_of_modifiers.find(std::string("asia"));
	REQUIRE(nvit != context.map_of_modifiers.end());
	auto id = nvit->second;

	REQUIRE(state->world.modifier_get_province_values(id).offsets[0] == sys::provincial_mod_offsets::assimilation_rate);

	REQUIRE(state->world.modifier_get_province_values(id).values[0] == Approx(-0.5f));
	REQUIRE(state->world.province_get_continent(context.original_id_to_prov_id_map[2702]) == id);
}
{
	auto nvit = context.map_of_modifiers.find(std::string("inhospitable_climate"));
	REQUIRE(nvit != context.map_of_modifiers.end());
	auto id = nvit->second;

	REQUIRE(state->world.modifier_get_province_values(id).offsets[0] == sys::provincial_mod_offsets::farm_rgo_size);

	REQUIRE(state->world.modifier_get_province_values(id).values[0] == 0.0f);
	REQUIRE(state->world.province_get_climate(context.original_id_to_prov_id_map[2702]) == id);
}
{

	auto nvit = context.map_of_modifiers.find(std::string("army_tech_school"));
	REQUIRE(nvit != context.map_of_modifiers.end());
	auto id = nvit->second;

	REQUIRE(state->world.modifier_get_national_values(id).offsets[0] == sys::national_mod_offsets::army_tech_research_bonus);
	REQUIRE(state->world.modifier_get_national_values(id).values[0] == Approx(0.15f));

	auto fit = context.map_of_tech_folders.find("naval_engineering");
	REQUIRE(fit != context.map_of_tech_folders.end());
	REQUIRE(state->culture_definitions.tech_folders[fit->second].category == culture::tech_category::navy);

	auto tit = context.map_of_technologies.find(std::string("modern_army_doctrine"));
	REQUIRE(tit != context.map_of_technologies.end());
	REQUIRE(bool(tit->second.id) == true);
}
{
	auto tit = context.map_of_inventions.find(std::string("direct_current"));
	REQUIRE(tit != context.map_of_inventions.end());
	REQUIRE(bool(tit->second.id) == true);
	REQUIRE(culture::tech_category(state->world.invention_get_technology_type(tit->second.id)) == culture::tech_category::industry);
}
{

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
{

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
	REQUIRE(state->world.political_party_get_end_date(first_party) == sys::date(sys::year_month_day{2000, uint16_t(1), uint16_t(1)}, state->start_date));
	REQUIRE(context.map_of_ideologies.find(std::string("conservative"))->second.id == state->world.political_party_get_ideology(first_party));
}
{

	auto france_tag = context.map_of_ident_names.find(nations::tag_to_int('F', 'R', 'A'))->second;
	auto ident_rel = state->world.national_identity_get_identity_holder(france_tag);
	auto france = state->world.identity_holder_get_nation(ident_rel);

	auto prov = fatten(state->world, context.original_id_to_prov_id_map[408]);
	REQUIRE(prov.get_nation_from_province_ownership() == france);
	REQUIRE(prov.get_nation_from_province_control() == france);
	REQUIRE(prov.get_rgo() == context.map_of_commodity_names.find(std::string("fruit"))->second);
	REQUIRE(prov.get_life_rating() == uint8_t(34));
	REQUIRE(prov.get_building_level(economy::province_building_type::railroad) == uint8_t(0));

	bool found_france = false;
	for (auto cr : prov.get_core()) {
		if (cr.get_identity() == france_tag)
			found_france = true;
	}
	REQUIRE(found_france);
}

// load pop history files
{

	dcon::pop_type_id ptype = context.map_of_poptypes.find(std::string("artisans"))->second;
	dcon::culture_id cid = context.map_of_culture_names.find(std::string("british"))->second;
	dcon::religion_id rid = context.map_of_religion_names.find(std::string("protestant"))->second;

	float count = 0;

	for (auto pops_by_location : state->world.province_get_pop_location(context.original_id_to_prov_id_map[302])) {
		auto pop_id = pops_by_location.get_pop();
		if (pop_id.get_culture() == cid && pop_id.get_poptype() == ptype && pop_id.get_religion() == rid) {
			count = pop_id.get_size();
		}
	}

	REQUIRE(count == 11250.0f);
}
// load poptype definitions
{
	REQUIRE(bool(state->culture_definitions.artisans) == true);
	REQUIRE(state->world.pop_type_get_sprite(state->culture_definitions.artisans) == uint8_t(11));
	REQUIRE(state->world.pop_type_get_color(state->culture_definitions.artisans) == sys::pack_color(127, 3, 3));
	REQUIRE(state->world.pop_type_get_strata(state->culture_definitions.artisans) == uint8_t(culture::pop_strata::middle));

	auto wine = context.map_of_commodity_names.find("wine")->second;
	REQUIRE(state->world.pop_type_get_luxury_needs(state->culture_definitions.artisans, wine) == 10.0f);
	REQUIRE(state->value_modifiers[state->world.pop_type_get_country_migration_target(state->culture_definitions.artisans)].factor == 1.0f);

	auto react = context.map_of_ideologies.find("reactionary")->second.id;
	REQUIRE(state->value_modifiers[state->world.pop_type_get_ideology(state->culture_definitions.artisans, react)].factor == 1.0f);
}
// load ideology contents
{
	REQUIRE(bool(state->culture_definitions.conservative) == true);
	REQUIRE(state->world.ideology_get_color(state->culture_definitions.conservative) == sys::pack_color(10, 10, 250));
	auto mkey = state->world.ideology_get_add_economic_reform(state->culture_definitions.conservative);
	REQUIRE(state->value_modifiers[mkey].factor == -0.5f);
}

// cb contents
{

	REQUIRE(bool(state->military_definitions.standard_civil_war) == true);
	REQUIRE(state->world.cb_type_get_sprite_index(state->military_definitions.standard_civil_war) == uint8_t(2));
	REQUIRE(state->world.cb_type_get_months(state->military_definitions.standard_civil_war) == uint8_t(1));
	REQUIRE(state->world.cb_type_get_peace_cost_factor(state->military_definitions.standard_civil_war) == 1.0f);
	REQUIRE(state->world.cb_type_get_break_truce_militancy_factor(state->military_definitions.standard_civil_war) == 2.0f);
	REQUIRE(state->world.cb_type_get_truce_months(state->military_definitions.standard_civil_war) == uint8_t(0));
	REQUIRE(state->world.cb_type_get_type_bits(state->military_definitions.standard_civil_war) ==
	        uint32_t(military::cb_flag::is_civil_war | military::cb_flag::po_annex | military::cb_flag::is_triggered_only | military::cb_flag::is_not_constructing_cb | military::cb_flag::not_in_crisis));
	REQUIRE(bool(state->world.cb_type_get_can_use(state->military_definitions.standard_civil_war)) == true);
	REQUIRE(bool(state->world.cb_type_get_on_add(state->military_definitions.standard_civil_war)) == true);
	REQUIRE(bool(state->world.cb_type_get_allowed_states(state->military_definitions.standard_civil_war)) == false);
}
// pending crimes
{

	auto ita = context.map_of_crimes.find(std::string("machine_politics"));
	REQUIRE(state->culture_definitions.crimes[ita->second.id].available_by_default == true);
	auto mod_id = state->culture_definitions.crimes[ita->second.id].modifier;
	REQUIRE(bool(mod_id) == true);
	REQUIRE(state->world.modifier_get_icon(mod_id) == uint8_t(4));
	REQUIRE(state->world.modifier_get_province_values(mod_id).offsets[0] == sys::provincial_mod_offsets::boost_strongest_party);
	REQUIRE(state->world.modifier_get_province_values(mod_id).values[0] == 5.0f);
}
// pending issue options
{

	REQUIRE(bool(state->culture_definitions.jingoism) == true);

	auto itb = context.map_of_ioptions.find(std::string("acceptable_schools"));
	auto fatb = fatten(state->world, itb->second.id);
	REQUIRE(fatb.get_administrative_multiplier() == 2.0f);
	auto mid = fatb.get_modifier();
	REQUIRE(bool(mid) == true);
	REQUIRE(state->world.modifier_get_national_values(mid).offsets[0] == sys::national_mod_offsets::education_efficiency_modifier);
}
// parse national_focus.txt
{

	REQUIRE(bool(state->national_definitions.flashpoint_focus) == true);
	REQUIRE(state->world.national_focus_get_icon(state->national_definitions.flashpoint_focus) == uint8_t(4));
	REQUIRE(state->world.national_focus_get_type(state->national_definitions.flashpoint_focus) == uint8_t(nations::focus_type::diplomatic_focus));
	REQUIRE(bool(state->world.national_focus_get_limit(state->national_definitions.flashpoint_focus)) == true);
}
// load pop_types.txt
{

	REQUIRE(bool(state->culture_definitions.migration_chance) == true);
}

// read pending techs
{
	auto it = context.map_of_technologies.find(std::string("modern_army_doctrine"));
	auto fit = fatten(state->world, it->second.id);
	REQUIRE(fit.get_year() == 1919);
	REQUIRE(fit.get_cost() == 21600);
	auto unit_adj = fit.get_modified_units();
	REQUIRE(unit_adj.size() == 1);
	REQUIRE(unit_adj[0].type == state->military_definitions.base_army_unit);
	REQUIRE(unit_adj[0].supply_consumption == Approx(0.20f));
	REQUIRE(bool(fit.get_modifier()) == true);
	REQUIRE(fit.get_modifier().get_national_values().offsets[0] == sys::national_mod_offsets::dig_in_cap);
	REQUIRE(fit.get_modifier().get_national_values().values[0] == 1.0f);
	REQUIRE(fit.get_increase_building(economy::province_building_type::fort) == true);
}
// read pending inventions
{

	auto it = context.map_of_inventions.find(std::string("the_talkies"));
	auto fit = fatten(state->world, it->second.id);

	REQUIRE(bool(fit.get_limit()) == true);
	REQUIRE(bool(fit.get_chance()) == true);
	REQUIRE(fit.get_shared_prestige() == 20.0f);
	REQUIRE(bool(fit.get_modifier()) == true);
	REQUIRE(fit.get_modifier().get_national_values().offsets[0] == sys::national_mod_offsets::suppression_points_modifier);
	REQUIRE(fit.get_modifier().get_national_values().offsets[1] == sys::national_mod_offsets::core_pop_consciousness_modifier);
	REQUIRE(fit.get_modifier().get_national_values().values[0] == Approx(-0.05f));
	REQUIRE(fit.get_modifier().get_national_values().values[1] == Approx(0.01f));
}
// parse on_actions.txt
{

	REQUIRE(state->national_definitions.on_civilize.size() == size_t(3));
	REQUIRE(state->national_definitions.on_civilize[0].chance == int16_t(100));
}
// parse production_types.txt
{

	auto it = context.map_of_factory_names.find(std::string("aeroplane_factory"));
	auto fac = fatten(state->world, it->second);
	REQUIRE(fac.get_base_workforce() == 10000);
	REQUIRE(fac.get_output_amount() == Approx(0.91f));
	REQUIRE(fac.get_bonus_2_amount() == Approx(0.25f));
	REQUIRE(fac.get_bonus_3_amount() == 0.0f);
	REQUIRE(state->economy_definitions.craftsmen_fraction == Approx(0.8f));
}
// read pending rebel types
{

	auto it = context.map_of_rebeltypes.find(std::string("boxer_rebels"));
	auto fid = fatten(state->world, it->second.id);

	REQUIRE(fid.get_icon() == uint8_t(1));
	REQUIRE(fid.get_break_alliance_on_win() == true);
	REQUIRE(fid.get_area() == uint8_t(culture::rebel_area::nation));

	auto gid = context.map_of_governments.find("fascist_dictatorship")->second;
	auto gidb = context.map_of_governments.find("absolute_monarchy")->second;
	REQUIRE(fid.get_government_change(gid) == gidb);

	REQUIRE(fid.get_defection() == uint8_t(0));
	REQUIRE(fid.get_independence() == uint8_t(0));
	REQUIRE(fid.get_culture_restriction() == false);
	REQUIRE(fid.get_occupation_multiplier() == 1.0f);
}

// load oob
{
	dcon::army_id frst;
	int32_t army_count = 0;
	for (auto aloc : state->world.province_get_army_location(context.original_id_to_prov_id_map[300])) {
		frst = aloc.get_army();
		++army_count;
	}
	REQUIRE(army_count == 1);
	REQUIRE(bool(frst) == true);
	int32_t reg_count = 0;
	bool from_294 = false;
	for (auto rmem : state->world.army_get_army_membership(frst)) {
		++reg_count;
		from_294 = from_294 ||
		           rmem.get_regiment().get_pop_from_regiment_source().get_province_from_pop_location() == context.original_id_to_prov_id_map[294];
	}
	REQUIRE(reg_count == 6);
	REQUIRE(from_294 == true);
}

// parse diplomacy history
{

	auto china_tag = context.map_of_ident_names.find(nations::tag_to_int('C', 'H', 'I'))->second;
	auto cident_rel = state->world.national_identity_get_identity_holder(china_tag);
	auto china = state->world.identity_holder_get_nation(cident_rel);

	// second = GXI
	auto gxi_tag = context.map_of_ident_names.find(nations::tag_to_int('G', 'X', 'I'))->second;
	auto gident_rel = state->world.national_identity_get_identity_holder(gxi_tag);
	auto gxi = fatten(state->world, state->world.identity_holder_get_nation(gident_rel));

	REQUIRE(bool(gxi.get_overlord_as_subject()) == true);
	REQUIRE(gxi.get_overlord_as_subject().get_ruler() == china);
	REQUIRE(gxi.get_is_substate() == true);
}

// load country history
{
	auto tag = fatten(state->world, context.map_of_ident_names.find(nations::tag_to_int('P', 'O', 'R'))->second);
	auto nation = tag.get_nation_from_identity_holder();

	REQUIRE(nation.get_plurality() == 25.0f);
	REQUIRE(nation.get_prestige() == 10.0f);
	REQUIRE(nation.get_is_civilized() == true);

	REQUIRE(nation.get_issues(context.map_of_iissues.find("voting_system")->second) == context.map_of_ioptions.find("jefferson_method")->second.id);
	REQUIRE(nation.get_active_technologies(context.map_of_technologies.find("alphabetic_flag_signaling")->second.id) == true);
}

{
	auto tag = fatten(state->world, context.map_of_ident_names.find(nations::tag_to_int('R', 'U', 'S'))->second);
	auto nation = tag.get_nation_from_identity_holder();
	REQUIRE(nation.get_modifier_values(sys::national_mod_offsets::combat_width) == -1.0f);
	auto art_id = context.map_of_unit_types.find("artillery")->second;
	REQUIRE(nation.get_active_unit(art_id) == true);
	REQUIRE(nation.get_unit_stats(state->military_definitions.base_army_unit).default_organisation == 10);
	auto b_id = context.map_of_factory_names.find("fabric_factory")->second;
	REQUIRE(nation.get_active_building(b_id) == true);
	auto c_id = context.map_of_commodity_names.find("sulphur")->second;
	REQUIRE(nation.get_rgo_goods_output(c_id) == 0.25f);
	REQUIRE(nation.get_max_building_level(economy::province_building_type::fort) == 1);
}

/*************************************************
// where some benchmarks live

const auto trash_cache = []() {
    volatile char* p[8192];
    for(uint32_t i = 0; i < 8192; i++) {
        p[i] = static_cast<volatile char*>(::new char[4096]);
        p[i][0] = p[i][4095] = 1;
    }
    for(uint32_t i = 0; i < 8192; i++) {
        delete[] p[i];
    }
};

BENCHMARK_ADVANCED("basic maximum")(Catch::Benchmark::Chronometer meter) {
    trash_cache();
    meter.measure([&]() {
        province::for_each_land_province(*state, [&](dcon::province_id p) {
            // schombert: there is a faster way to do this. Instead of figuring out the max province by province
            // it would be better to go culture by culture, storing the temporary max share per province in a
            // temporary buffer. Why? Because by going through things one culture at a time would be much easier
            // on the prefetcher given the typical number of cultures
            dcon::culture_id max_id;
            float max_value = 0.0f;
            state->world.for_each_culture([&](dcon::culture_id c) {
                if(auto v = state->world.province_get_demographics(p, demographics::to_key(*state, c)); v > max_value) {
                    max_value = v;
                    max_id = c;
                }
            });
            state->world.province_set_dominant_culture(p, max_id);
        });
    });
};
BENCHMARK_ADVANCED("vectorized maximum")(Catch::Benchmark::Chronometer meter) {
    trash_cache();
    meter.measure([&]() {
        ve::execute_serial<dcon::province_id>(uint32_t(state->province_definitions.first_sea_province.index()), [&](auto p) {
            ve::tagged_vector<dcon::culture_id> max_id;
            ve::fp_vector max_value = 0.0f;
            state->world.for_each_culture([&](dcon::culture_id c) {
                auto v = state->world.province_get_demographics(p, demographics::to_key(*state, c));
                auto mask = v > max_value;
                max_id = ve::select(mask, ve::tagged_vector<dcon::culture_id>(c), max_id);
                max_value = ve::select(mask, v, max_value);
            });
            state->world.province_set_dominant_culture(p, max_id);
        });
    });
};

static ve::vectorizable_buffer<float, dcon::province_id> max_buffer(uint32_t(1));
static uint32_t old_count = 1;

// this isn't needed here but I will need it in some of the other places
auto new_count = uint32_t(state->province_definitions.first_sea_province.index());
if(new_count > old_count) {
    max_buffer = state->world.province_make_vectorizable_float_buffer();
    old_count = new_count;
}

BENCHMARK_ADVANCED("vectorized loop exchanged maximum")(Catch::Benchmark::Chronometer meter) {
    trash_cache();
    meter.measure([&]() {

        ve::execute_serial<dcon::province_id>(uint32_t(state->province_definitions.first_sea_province.index()), [&](auto p) {
            max_buffer.set(p, ve::fp_vector());
        });
        state->world.for_each_culture([&](dcon::culture_id c) {
            ve::execute_serial<dcon::province_id>(uint32_t(state->province_definitions.first_sea_province.index()), [&](auto p) {
                auto v = state->world.province_get_demographics(p, demographics::to_key(*state, c));
                auto old_max = max_buffer.get(p);
                auto mask = v > old_max;
                state->world.province_set_dominant_culture(p, ve::select(mask, ve::tagged_vector<dcon::culture_id>(c), state->world.province_get_dominant_culture(p)));
                max_buffer.set(p, ve::select(mask, v, old_max));
            });
        });
    });
};
// ***************************/
}

struct test_event_nation_pair {
	dcon::nation_id n;
	dcon::free_national_event_id e;

	bool operator==(test_event_nation_pair const &other) const noexcept {
		return other.n == n && other.e == e;
	}
	bool operator<(test_event_nation_pair const &other) const noexcept {
		return other.n != n ? (n.value < other.n.value) : (e.value < other.e.value);
	}
};

TEST_CASE("event performance", "[benchmarks]") {
	auto ws = load_testing_scenario_file();
	auto &state = *ws;

	BENCHMARK_ADVANCED("national events in sequence")
	(Catch::Benchmark::Chronometer meter) {
		meter.measure([&]() {
			for (uint32_t k = 0; k < 32; ++k) {
				uint32_t n_block_size = state.world.free_national_event_size() / 32;
				uint32_t p_block_size = state.world.free_provincial_event_size() / 32;

				uint32_t block_index = (state.current_date.value & 31);

				auto n_block_end = block_index == 31 ? state.world.free_national_event_size() : n_block_size * (block_index + 1);
				for (uint32_t i = n_block_size * block_index; i < n_block_end; ++i) {
					dcon::free_national_event_id id{dcon::national_event_id::value_base_t(i)};
					auto mod = state.world.free_national_event_get_mtth(id);
					auto t = state.world.free_national_event_get_trigger(id);

					if (mod && (state.world.free_national_event_get_only_once(id) == false || state.world.free_national_event_get_has_been_triggered(id) == false)) {
						ve::execute_serial_fast<dcon::nation_id>(state.world.nation_size(), [&](auto ids) {
							/*
							For national events: the base factor (scaled to days) is multiplied with all modifiers that hold. If the value is non positive, we take the probability of the event occurring as 0.000001. If the value is less than 0.001, the event is guaranteed to happen. Otherwise, the probability is the multiplicative inverse of the value.
							*/
							auto some_exist = t ? (state.world.nation_get_owned_province_count(ids) != 0) && trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(ids), 0) : (state.world.nation_get_owned_province_count(ids) != 0);
							if (ve::compress_mask(some_exist).v != 0) {
								auto chances = trigger::evaluate_multiplicative_modifier(state, mod, trigger::to_generic(ids), trigger::to_generic(ids), 0);
								auto adj_chance = 1.0f - ve::select(chances <= 1.0f, 1.0f, 1.0f / (chances));
								auto adj_chance_2 = adj_chance * adj_chance;
								auto adj_chance_4 = adj_chance_2 * adj_chance_2;
								auto adj_chance_8 = adj_chance_4 * adj_chance_4;
								auto adj_chance_16 = adj_chance_8 * adj_chance_8;

								ve::apply([&](dcon::nation_id n, float c, bool condition) {
									auto owned_range = state.world.nation_get_province_ownership(n);
									if (condition && (state.world.free_national_event_get_only_once(id) == false || state.world.free_national_event_get_has_been_triggered(id) == false) && owned_range.begin() != owned_range.end()) {

										if (float(rng::get_random(state, uint32_t((i << 1) ^ n.index())) & 0xFFFF) / float(0xFFFF + 1) >= c) {
											event::trigger_national_event(state, id, n, uint32_t((state.current_date.value) ^ (i << 3)), uint32_t(n.index()));
										}
									}
								},
								          ids, adj_chance_16, some_exist);
							}
						});
					}
				}

				state.current_date = state.current_date + 1;
			}
		});
	};
	BENCHMARK_ADVANCED("national events in parallel")
	(Catch::Benchmark::Chronometer meter) {
		meter.measure([&]() {
			for (uint32_t k = 0; k < 32; ++k) {
				concurrency::combinable<std::vector<test_event_nation_pair>> events_triggered;
				uint32_t n_block_size = state.world.free_national_event_size() / 32;
				uint32_t p_block_size = state.world.free_provincial_event_size() / 32;

				uint32_t block_index = (state.current_date.value & 31);

				auto n_block_end = block_index == 31 ? state.world.free_national_event_size() : n_block_size * (block_index + 1);
				// for(uint32_t i = n_block_size * block_index; i < n_block_end; ++i) {
				concurrency::parallel_for(n_block_size * block_index, n_block_end, [&](uint32_t i) {
					dcon::free_national_event_id id{dcon::national_event_id::value_base_t(i)};
					auto mod = state.world.free_national_event_get_mtth(id);
					auto t = state.world.free_national_event_get_trigger(id);

					bool trigger_at_most_once = state.world.free_national_event_get_only_once(id);
					bool has_been_triggered = state.world.free_national_event_get_has_been_triggered(id);

					if (mod && (trigger_at_most_once == false || has_been_triggered == false)) {
						ve::execute_serial_fast<dcon::nation_id>(state.world.nation_size(), [&](auto ids) {
							/*
							For national events: the base factor (scaled to days) is multiplied with all modifiers that hold. If the value is non positive, we take the probability of the event occurring as 0.000001. If the value is less than 0.001, the event is guaranteed to happen. Otherwise, the probability is the multiplicative inverse of the value.
							*/
							if (trigger_at_most_once == true && has_been_triggered == true)
								return;

							auto some_exist =
							    (t ? trigger::evaluate(state, t, trigger::to_generic(ids), trigger::to_generic(ids), 0) : true) && (state.world.nation_get_owned_province_count(ids) != 0);

							if (ve::compress_mask(some_exist).v != 0) {
								auto chances = trigger::evaluate_multiplicative_modifier(state, mod, trigger::to_generic(ids), trigger::to_generic(ids), 0);
								auto adj_chance = 1.0f - ve::select(chances <= 1.0f, 1.0f, 1.0f / (chances));
								auto adj_chance_2 = adj_chance * adj_chance;
								auto adj_chance_4 = adj_chance_2 * adj_chance_2;
								auto adj_chance_8 = adj_chance_4 * adj_chance_4;
								auto adj_chance_16 = adj_chance_8 * adj_chance_8;

								ve::apply([&](dcon::nation_id n, float c, bool condition) {
									if (trigger_at_most_once == true && has_been_triggered == true)
										return;

									auto owned_range = state.world.nation_get_province_ownership(n);
									if (condition && (state.world.free_national_event_get_only_once(id) == false || state.world.free_national_event_get_has_been_triggered(id) == false) && owned_range.begin() != owned_range.end()) {

										if (float(rng::get_random(state, uint32_t((i << 1) ^ n.index())) & 0xFFFF) / float(0xFFFF + 1) >= c) {
											has_been_triggered = true;
											events_triggered.local().push_back(test_event_nation_pair{n, id});
											// event::trigger_national_event(state, id, n, uint32_t((state.current_date.value) ^ (i << 3)), uint32_t(n.index()));
										}
									}
								},
								          ids, adj_chance_16, some_exist);
							}
						});
					}
				});

				auto total_vector = events_triggered.combine([](auto &a, auto &b) {
					std::vector<test_event_nation_pair> result;
					result.insert(result.end(), a.begin(), a.end());
					result.insert(result.end(), b.begin(), b.end());
					return result;
				});
				std::sort(total_vector.begin(), total_vector.end());
				for (auto &v : total_vector) {
					event::trigger_national_event(state, v.e, v.n, uint32_t((state.current_date.value) ^ (v.e.value << 3)), uint32_t(v.n.value));
				}

				state.current_date = state.current_date + 1;
			}
		});
	};
}
//
//TEST_CASE(".mod overrides", "[req-game-files]") {
//	parsers::error_handler err("");
//	REQUIRE(std::string("NONE") != GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)
//
//	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
//	add_root(state->common_fs, NATIVE_M(GAME_DIR));
//	auto root = get_root(state->common_fs);
//	parsers::scenario_building_context context(*state);
//
//	// The default map exists, untouched and undisturbed
//	{
//		auto map = open_directory(root, NATIVE("map"));
//		auto def_map_file = open_file(map, NATIVE("default.map"));
//		REQUIRE(def_map_file.has_value() == true);
//	}
//
//	// As it is about to be overriden by a .mod file with no prior notice
//	{
//		auto content = std::string_view{
//		    "name = \"Test\"\n"
//		    "path = \"mod\\test\"\n"
//		    "replace_path = \"map\"\n"};
//		err.file_name = "test.mod";
//		parsers::token_generator gen(content.data(), content.data() + content.length());
//
//		parsers::mod_file_context mod_file_context(context);
//		parsers::parse_mod_file(gen, err, mod_file_context);
//
//		REQUIRE(err.accumulated_errors == "");
//	}
//
//	{
//		auto map = open_directory(root, NATIVE("map"));
//		auto def_map_file = open_file(map, NATIVE("default.map"));
//		REQUIRE(def_map_file.has_value() == false);
//	}
//
//	// Ensure the filesystem state is kept the same
//	const auto fs_str = simple_fs::extract_state(state->common_fs);
//	simple_fs::restore_state(state->common_fs, fs_str);
//	REQUIRE(simple_fs::extract_state(state->common_fs) == fs_str);
//
//	// Now the default.map file shouldn't exist as we override the path
//	// with a non-existing path
//	{
//		auto map = open_directory(root, NATIVE("map"));
//		auto def_map_file = open_file(map, NATIVE("default.map"));
//		REQUIRE(def_map_file.has_value() == false);
//	}
//}
#endif
