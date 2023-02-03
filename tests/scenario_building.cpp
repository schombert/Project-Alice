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
}
#endif
