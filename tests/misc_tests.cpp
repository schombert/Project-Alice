#include "catch.hpp"
#include "system_state.hpp"
#include "date_interface.hpp"

TEST_CASE("string pool tests", "[misc_tests]") {
	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

	auto a = state->add_to_pool(std::string_view("blah_blah"));
	auto b = state->add_to_pool(std::string_view("1234"));

	auto old_size_a = state->text_data.size();
	auto c = state->add_to_pool(std::string());
	REQUIRE(old_size_a == state->text_data.size());

	auto d = state->add_to_pool(std::string("last"));

	REQUIRE(state->to_string_view(a) == "blah_blah");
	REQUIRE(state->to_string_view(b) == "1234");
	REQUIRE(state->to_string_view(c).length() == size_t(0));
	REQUIRE(state->to_string_view(d) == "last");

	auto old_size = state->text_data.size();
	auto x = state->add_unique_to_pool("1234");
	REQUIRE(old_size == state->text_data.size());

	auto y = state->add_unique_to_pool("new");

	REQUIRE(state->to_string_view(x) == "1234");
	REQUIRE(state->to_string_view(y) == "new");

	auto la = state->add_to_pool_lowercase(std::string_view("MiXeD"));
	auto lb = state->add_to_pool_lowercase(std::string("LaTeX"));

	REQUIRE(state->to_string_view(la) == "mixed");
	REQUIRE(state->to_string_view(lb) == "latex");
}

TEST_CASE("date tests", "[misc_tests]") {
	sys::absolute_time_point base_time{ sys::year_month_day{ 2020, 1, 2 } };
	sys::date first{ 0 };

	auto ymda = first.to_ymd(base_time);
	REQUIRE(ymda.year == 2020);
	REQUIRE(ymda.month == 1);
	REQUIRE(ymda.day == 2);

	sys::date start_date{ sys::year_month_day{ 2023, 1, 16 }, base_time };

	auto ymdb = start_date.to_ymd(base_time);
	REQUIRE(ymdb.year == 2023);
	REQUIRE(ymdb.month == 1);
	REQUIRE(ymdb.day == 16);

	sys::date end_date = start_date + 1673;

	auto ymdc = end_date.to_ymd(base_time);
	REQUIRE(ymdc.year == 2027);
	REQUIRE(ymdc.month == 8);
	REQUIRE(ymdc.day == 16);
}
