#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "simple_fs.hpp"
#include "system_state.hpp"


#define ALICE_NO_ENTRY_POINT
#include "main.cpp"

#define RANGE(x) (x), (x) + ((sizeof(x)) / sizeof((x)[0])) - 1

#ifdef _WIN64
#define NATIVE_SEP "\\"
#else
#define NATIVE_SEP "/"
#endif

#define NATIVE_M(X) NATIVE(X)

#include "gui_graphics_parsing_tests.cpp"
#include "misc_tests.cpp"
#include "parsers_tests.cpp"
#include "file_system_tests.cpp"


TEST_CASE("Dummy test", "[dummy test instance]") {
    REQUIRE(1 + 1 == 2); 
}
