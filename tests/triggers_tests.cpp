#include "catch.hpp"
#include "parsers_declarations.hpp"

TEST_CASE("trigger scope recursion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::generic_scope));
		t.push_back(uint16_t(11));
		t.push_back(uint16_t(trigger::no_payload | trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::is_scope | trigger::state_scope_pop));
		t.push_back(uint16_t(3));
		t.push_back(uint16_t(trigger::association_eq | trigger::owns));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::no_payload | trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(0));

		int32_t total = 0;
		parsers::recurse_over_triggers(t.data(), [&total](uint16_t*) { ++total; });
		REQUIRE(5 == total);

		int32_t blockade_count = 0;
		parsers::recurse_over_triggers(t.data(), [&blockade_count](uint16_t* v) { if((*v & trigger::code_mask) == trigger::blockade) ++blockade_count; });
		REQUIRE(2 == blockade_count);

	}
}

TEST_CASE("trigger inversion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(0));

		parsers::invert_trigger(t.data());

		REQUIRE(t[0] == uint16_t(trigger::is_scope | trigger::is_disjunctive_scope | trigger::placeholder_not_scope));
		REQUIRE(t[1] == uint16_t(4));
		REQUIRE(t[2] == uint16_t(trigger::association_ne | trigger::blockade));
		REQUIRE(t[3] == uint16_t(0));
		REQUIRE(t[4] == uint16_t(0));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::is_existence_scope | trigger::is_disjunctive_scope | trigger::x_core_scope_nation));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_ge | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		parsers::invert_trigger(t.data());

		REQUIRE(t[0] == uint16_t(trigger::is_scope | trigger::x_core_scope_nation));
		REQUIRE(t[1] == uint16_t(4));
		REQUIRE(t[2] == uint16_t(trigger::association_lt | trigger::blockade));
		REQUIRE(t[3] == uint16_t(2));
		REQUIRE(t[4] == uint16_t(1));
	}
}

TEST_CASE("scope sizing", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(4));

		REQUIRE(false == parsers::scope_is_empty(t.data()));
		REQUIRE(true == parsers::scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		t.push_back(uint16_t(0));

		REQUIRE(false == parsers::scope_is_empty(t.data()));
		REQUIRE(false == parsers::scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::integer_scope));
		t.push_back(uint16_t(5));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(6));

		REQUIRE(false == parsers::scope_is_empty(t.data()));
		REQUIRE(true == parsers::scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::integer_scope));
		t.push_back(uint16_t(8));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::blockade));
		t.push_back(0);
		t.push_back(0);

		REQUIRE(false == parsers::scope_is_empty(t.data()));
		REQUIRE(false == parsers::scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(1));

		REQUIRE(true == parsers::scope_is_empty(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::integer_scope));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		REQUIRE(true == parsers::scope_is_empty(t.data()));
	}
}

TEST_CASE("simple negation removal", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(3 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::association_ne | trigger::blockade));
		REQUIRE(t[1] == uint16_t(2));
		REQUIRE(t[2] == uint16_t(1));
	}
}

TEST_CASE("double negation removal", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(3 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::association_eq | trigger::blockade));
		REQUIRE(t[1] == uint16_t(2));
		REQUIRE(t[2] == uint16_t(1));
	}
}

/*
TEST_CASE("complex inversion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger_codes::is_scope | trigger_codes::placeholder_not_scope));
		t.push_back(9ui16);
		t.push_back(uint16_t(trigger_codes::is_scope | trigger_codes::generic_scope));
		t.push_back(2ui16);
		t.push_back(uint16_t(trigger_codes::association_eq | trigger_codes::no_payload | trigger_codes::blockade));
		t.push_back(uint16_t(trigger_codes::is_scope | trigger_codes::is_existance_scope | trigger_codes::is_disjunctive_scope | trigger_codes::x_core_scope_nation));
		t.push_back(4ui16);
		t.push_back(uint16_t(trigger_codes::association_ge | trigger_codes::blockade));
		t.push_back(2ui16);
		t.push_back(1ui16);

		const auto new_size = simplify_trigger(t.data());

		EXPECT_EQ(8, new_size);
		EXPECT_EQ(t[0], uint16_t(trigger_codes::is_scope | trigger_codes::is_disjunctive_scope | trigger_codes::generic_scope));
		EXPECT_EQ(t[1], 7ui16);
		EXPECT_EQ(t[2], uint16_t(trigger_codes::association_ne | trigger_codes::no_payload | trigger_codes::blockade));
		EXPECT_EQ(t[3], uint16_t(trigger_codes::is_scope | trigger_codes::x_core_scope_nation));
		EXPECT_EQ(t[4], 4ui16);
		EXPECT_EQ(t[5], uint16_t(trigger_codes::association_lt | trigger_codes::blockade));
		EXPECT_EQ(t[6], 2ui16);
		EXPECT_EQ(t[7], 1ui16);
	}
}

TEST(trigger_reading, scope_absorbtion) {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger_codes::is_scope | trigger_codes::x_neighbor_province_scope | trigger_codes::is_disjunctive_scope | trigger_codes::is_existance_scope));
		t.push_back(7ui16);
		t.push_back(uint16_t(trigger_codes::is_scope | trigger_codes::generic_scope));
		t.push_back(5ui16);
		t.push_back(uint16_t(trigger_codes::association_eq | trigger_codes::blockade));
		t.push_back(2ui16);
		t.push_back(1ui16);
		t.push_back(uint16_t(trigger_codes::association_eq | trigger_codes::no_payload | trigger_codes::blockade));
		t.push_back(0ui16);

		const auto new_size = simplify_trigger(t.data());

		EXPECT_EQ(6, new_size);
		EXPECT_EQ(t[0], uint16_t(trigger_codes::is_scope | trigger_codes::x_neighbor_province_scope | trigger_codes::is_existance_scope));
		EXPECT_EQ(t[1], 5ui16);
		EXPECT_EQ(t[2], uint16_t(trigger_codes::association_eq | trigger_codes::blockade));
		EXPECT_EQ(t[3], 2ui16);
		EXPECT_EQ(t[4], 1ui16);
		EXPECT_EQ(t[5], uint16_t(trigger_codes::association_eq | trigger_codes::no_payload | trigger_codes::blockade));
	}
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger_codes::is_scope | trigger_codes::integer_scope));
		t.push_back(8ui16);
		t.push_back(15ui16);
		t.push_back(uint16_t(trigger_codes::is_scope | trigger_codes::generic_scope | trigger_codes::is_disjunctive_scope));
		t.push_back(5ui16);
		t.push_back(uint16_t(trigger_codes::association_eq | trigger_codes::blockade));
		t.push_back(2ui16);
		t.push_back(1ui16);
		t.push_back(uint16_t(trigger_codes::association_eq | trigger_codes::no_payload | trigger_codes::blockade));
		t.push_back(0ui16);

		const auto new_size = simplify_trigger(t.data());

		EXPECT_EQ(7, new_size);
		EXPECT_EQ(t[0], uint16_t(trigger_codes::is_scope | trigger_codes::integer_scope | trigger_codes::is_disjunctive_scope));
		EXPECT_EQ(t[1], 6ui16);
		EXPECT_EQ(t[2], 15ui16);
		EXPECT_EQ(t[3], uint16_t(trigger_codes::association_eq | trigger_codes::blockade));
		EXPECT_EQ(t[4], 2ui16);
		EXPECT_EQ(t[5], 1ui16);
		EXPECT_EQ(t[6], uint16_t(trigger_codes::association_eq | trigger_codes::no_payload | trigger_codes::blockade));
	}
}


TEST(trigger_reading, simple_trigger) {
	const char trigger[] = "always = no";

	scenario::scenario_manager sm;
	trigger_parsing_environment parse_env(
		sm,
		trigger_scope_state{
			trigger_slot_contents::empty,
			trigger_slot_contents::empty,
			trigger_slot_contents::empty,
			false });

	std::vector<token_group> parse_results;
	parse_pdx_file(parse_results, RANGE(trigger));

	add_simple_trigger(
		parse_env,
		parse_results[0].token,
		parse_results[0].association,
		parse_results[1].token);

	EXPECT_EQ(1ui64, parse_env.data.size());
	EXPECT_EQ(uint16_t(trigger_codes::always | trigger_codes::no_payload | trigger_codes::association_ne), parse_env.data[0]);
}

TEST(trigger_reading, storing_data) {
	{
		std::vector<uint16_t> t;
		add_float_to_payload(t, 2.5f);
		EXPECT_EQ(2ui64, t.size());
		EXPECT_EQ(2.5f, read_float_from_payload(t.data()));
	}
	{
		std::vector<uint16_t> t;
		add_int32_t_to_payload(t, 10002);
		EXPECT_EQ(2ui64, t.size());
		EXPECT_EQ(10002, read_int32_t_from_payload(t.data()));
	}
}

TEST(trigger_reading, scope_dependant_trigger) {
	const char trigger[] = "militancy < 12";

	scenario::scenario_manager sm;
	trigger_parsing_environment parse_env(
		sm,
		trigger_scope_state{
			trigger_slot_contents::province,
			trigger_slot_contents::empty,
			trigger_slot_contents::empty,
			false });

	std::vector<token_group> parse_results;
	parse_pdx_file(parse_results, RANGE(trigger));

	add_simple_trigger(
		parse_env,
		parse_results[0].token,
		parse_results[0].association,
		parse_results[1].token);

	EXPECT_EQ(4ui64, parse_env.data.size());
	EXPECT_EQ(uint16_t(trigger_codes::militancy_province | trigger_codes::association_lt), parse_env.data[0]);
	EXPECT_EQ(3ui16, parse_env.data[1]);
	EXPECT_EQ(12.0f, read_float_from_payload(parse_env.data.data() + 2));
}


TEST(trigger_reading, variable_scope) {
	const char trigger[] = "6 = { average_consciousness >= 1.0 average_militancy != 0.5 }";

	scenario::scenario_manager sm;

	std::vector<token_group> parse_results;
	parse_pdx_file(parse_results, RANGE(trigger));

	auto data = parse_trigger(
		sm,
		trigger_scope_state{
			trigger_slot_contents::nation,
			trigger_slot_contents::empty,
			trigger_slot_contents::empty,
			false },
			parse_results.data(),
			parse_results.data() + parse_results.size());

	EXPECT_EQ(11ui64, data.size());
	EXPECT_EQ(data[0], uint16_t(trigger_codes::is_scope | trigger_codes::integer_scope));
	EXPECT_EQ(data[1], 10ui16);
	EXPECT_EQ(data[2], 6ui16);
	EXPECT_EQ(data[3], uint16_t(trigger_codes::association_ge | trigger_codes::average_consciousness_province));
	EXPECT_EQ(data[4], 3ui16);
	EXPECT_EQ(1.0f, read_float_from_payload(data.data() + 5));
	EXPECT_EQ(data[7], uint16_t(trigger_codes::association_ne | trigger_codes::average_militancy_province));
	EXPECT_EQ(data[8], 3ui16);
	EXPECT_EQ(0.5f, read_float_from_payload(data.data() + 9));
}


TEST(trigger_reading, empty_trigger) {
	const char trigger[] = "";

	scenario::scenario_manager sm;

	std::vector<token_group> parse_results;
	parse_pdx_file(parse_results, RANGE(trigger));

	auto t_result = parse_trigger(
		sm,
		trigger_scope_state{
			trigger_slot_contents::empty,
			trigger_slot_contents::empty,
			trigger_slot_contents::empty,
			false },
			parse_results.data(),
			parse_results.data() + parse_results.size());

	EXPECT_EQ(0ui64, t_result.size());
}

TEST(trigger_reading, commit_trigger) {
	trigger_manager m;
	EXPECT_EQ(1ui64, m.trigger_data.size());

	const auto etag = commit_trigger(m, std::vector<uint16_t>());
	EXPECT_EQ(1ui64, m.trigger_data.size());
	EXPECT_EQ(trigger_tag(), etag);

	std::vector<uint16_t> a{ 1ui16, 2ui16, 3ui16 };

	const auto atag = commit_trigger(m, a);
	EXPECT_EQ(4ui64, m.trigger_data.size());
	EXPECT_EQ(trigger_tag(0), atag);

	std::vector<uint16_t> b{ 6ui16, 7ui16 };

	const auto btag = commit_trigger(m, b);
	EXPECT_EQ(6ui64, m.trigger_data.size());
	EXPECT_EQ(trigger_tag(3), btag);

	std::vector<uint16_t> c{ 2ui16, 3ui16, 6ui16 };

	const auto ctag = commit_trigger(m, c);
	EXPECT_EQ(6ui64, m.trigger_data.size());
	EXPECT_EQ(trigger_tag(1), ctag);
}
*/
