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

std::unique_ptr<sys::state> load_testing_scenario_file() {
	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack

	add_root(game_state->common_fs, NATIVE("."));        // for the moment this lets us find the shader files

	if (!sys::try_read_scenario_and_save_file(*game_state, NATIVE("tests_scenario.bin"))) {
		// scenario making functions
		parsers::error_handler err("");
		game_state->load_scenario_data(err, sys::year_month_day{ 1836, 1, 1 });
		sys::write_scenario_file(*game_state, NATIVE("tests_scenario.bin"), 1);
		INFO("Wrote new scenario");
		std::abort();
	} else {
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
