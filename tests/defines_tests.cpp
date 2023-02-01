#include "catch.hpp"
#include "system_state.hpp"
#include "defines.hpp"

TEST_CASE("defines tests", "[defines_tests]") {
	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	add_root(state->common_fs, NATIVE_M(GAME_DIR));
	auto root = get_root(state->common_fs);
	auto common = open_directory(root, NATIVE("common"));

#ifndef IGNORE_REAL_FILES_TESTS
	auto file = open_file(common, NATIVE("defines.lua"));
	REQUIRE(file);
	if(file) {
		auto content = view_contents(*file);
		parsing::defines d{};
		d.parse_file(content.data);

		REQUIRE(d.greatness_days == 365.0f);
	}
#endif
}
