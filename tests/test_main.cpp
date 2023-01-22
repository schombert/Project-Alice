#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "simple_fs.hpp"
#include "system_state.hpp"
#include "parsers.cpp"
#include "system_state.cpp"

#ifdef _WIN64
#include "simple_fs_win.cpp"
#include "window_win.cpp"
#else
#include "simple_fs_nix.cpp"
#include "window_nix.cpp"
#endif

#include "opengl_wrapper.cpp"

#include "misc_tests.cpp"

// yeah, I know this is dumb, leave me alone
namespace sys {
	state::~state() {
		// why does this exist ? So that the destructor of the unique pointer doesn't have to be known before it is implemented
	}
}

TEST_CASE("Dummy test", "[dummy test instance]") {
    REQUIRE(1 + 1 == 2); 
}
