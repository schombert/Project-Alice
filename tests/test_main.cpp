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
