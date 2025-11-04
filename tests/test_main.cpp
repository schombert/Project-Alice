#define CATCH_CONFIG_ENABLE_BENCHMARKING 1
#define CATCH_CONFIG_MAIN 1
#define CATCH_CONFIG_DISABLE_EXCEPTIONS 1
#include "catch2/catch.hpp"

#ifndef DCON_TRAP_INVALID_STORE
#define DCON_TRAP_INVALID_STORE 1
#endif

#pragma comment(lib, "icu.lib")

#define ALICE_NO_ENTRY_POINT 1
#include "main.cpp"

#define RANGE(x) (x), (x) + ((sizeof(x)) / sizeof((x)[0])) - 1
#define RANGE_SZ(x) (x), ((sizeof(x)) / sizeof((x)[0])) - 1

#ifdef _WIN64
#define NATIVE_SEP "\\"
#else
#define NATIVE_SEP "/"
#endif


std::unique_ptr<sys::state> load_testing_scenario_file(sys::network_mode_type mode = sys::network_mode_type::single_player) {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	game_state->network_mode = mode;
	game_state->user_settings.autosaves = sys::autosave_frequency::yearly;

	add_root(game_state->common_fs, NATIVE("."));        // for the moment this lets us find the shader files
	if(!sys::try_read_scenario_file(*game_state, NATIVE("tests_scenario.bin"))) {
		std::abort();
	} else {
		game_state->on_scenario_load();
		INFO("Scenario loaded");
	}



	return game_state;
}




std::unique_ptr<sys::state> load_testing_scenario_file_with_save(sys::network_mode_type mode = sys::network_mode_type::single_player, dcon::nation_id selected_nation = dcon::nation_id{ }) {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	game_state->network_mode = mode;
	game_state->user_settings.autosaves = sys::autosave_frequency::yearly;

	add_root(game_state->common_fs, NATIVE("."));        // for the moment this lets us find the shader files
	if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("tests_scenario.bin"))) {
		// scenario making functions
		parsers::error_handler err("");
		game_state->load_scenario_data(err, sys::year_month_day{ 1836, 1, 1 });
		sys::write_scenario_file(*game_state, NATIVE("tests_scenario.bin"), 1);
		INFO("Wrote new scenario");
		std::abort();
	} else {
		if(!selected_nation) {
			auto observer_nation = game_state->world.national_identity_get_nation_from_identity_holder(game_state->national_definitions.rebel_id);
			network::create_mp_player(*game_state, sys::player_name{ 'P', 'l' ,'a', 'y', 'e', 'r' }, sys::player_password_raw{ }, true, false, observer_nation);
			game_state->local_player_nation = observer_nation;
		} else {
			network::create_mp_player(*game_state, sys::player_name{ 'P', 'l' ,'a', 'y', 'e', 'r' }, sys::player_password_raw{ },true , false, selected_nation);
			game_state->local_player_nation = selected_nation;
		}
		game_state->fill_unsaved_data();
		INFO("Scenario loaded");
	}



	return game_state;
}

#include "determinism_tests.cpp"
#include "gui_graphics_parsing_tests.cpp"
#include "misc_tests.cpp"
#include "parsers_tests.cpp"
#include "file_system_tests.cpp"
#include "text_tests.cpp"
//#include "scenario_building.cpp"
#include "defines_tests.cpp"
#include "triggers_tests.cpp"
#include "dcon_tests.cpp"
#include "network_tests.cpp"

TEST_CASE("Dummy test", "[dummy test instance]") {
	REQUIRE(1 + 1 == 2);
}
