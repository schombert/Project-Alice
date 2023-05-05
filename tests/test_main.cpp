#define CATCH_CONFIG_ENABLE_BENCHMARKING
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "simple_fs.hpp"
#include "system_state.hpp"

#ifndef DCON_TRAP_INVALID_STORE
#define DCON_TRAP_INVALID_STORE 1
#endif

#define ALICE_NO_ENTRY_POINT
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

	assert(std::string("NONE") != GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)

	add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
	add_root(game_state->common_fs, NATIVE(".")); // for the moment this lets us find the shader files


	if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("tests_scenario.bin"))) {
		// scenario making functions
		game_state->load_scenario_data();
		sys::write_scenario_file(*game_state, NATIVE("tests_scenario.bin"));
	} else {
		game_state->fill_unsaved_data();
	}

	return game_state;
}

#include "gui_graphics_parsing_tests.cpp"
#include "misc_tests.cpp"
#include "parsers_tests.cpp"
#include "file_system_tests.cpp"
#include "text_tests.cpp"
#include "scenario_building.cpp"
#include "defines_tests.cpp"
#include "triggers_tests.cpp"

TEST_CASE("Dummy test", "[dummy test instance]") {
    REQUIRE(1 + 1 == 2); 
}
