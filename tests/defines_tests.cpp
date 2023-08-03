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
		parsers::error_handler err("");
		auto content = view_contents(*file);
		parsing::defines d{};

		// Overriden by file values
		d.greatness_days = 0.f;
		d.parse_file(*state, content.data, err);
		REQUIRE(d.greatness_days == 365.0f);

		sys::date date{0};
		auto ymds = date.to_ymd(state->start_date);
		REQUIRE(ymds.year == 1836);
		REQUIRE(ymds.month == 1);
		REQUIRE(ymds.day == 1);

		auto ymde = date.to_ymd(state->end_date);
		REQUIRE(ymde.year == 1935);
		REQUIRE(ymde.month == 12);
		REQUIRE(ymde.day == 31);

		REQUIRE(err.accumulated_errors.length() == size_t(0));
	}
#endif
}
