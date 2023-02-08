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


TEST_CASE("complex inversion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::placeholder_not_scope));
		t.push_back(uint16_t(10));
		t.push_back(uint16_t(trigger::is_scope | trigger::generic_scope));
		t.push_back(uint16_t(3));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::is_scope | trigger::is_existence_scope | trigger::is_disjunctive_scope | trigger::x_core_scope_nation));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_ge | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(9 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::is_scope | trigger::is_disjunctive_scope | trigger::generic_scope));
		REQUIRE(t[1] == uint16_t(8));
		REQUIRE(t[2] == uint16_t(trigger::association_ne | trigger::no_payload | trigger::owns));
		REQUIRE(t[3] == uint16_t(0));
		REQUIRE(t[4] == uint16_t(trigger::is_scope | trigger::x_core_scope_nation));
		REQUIRE(t[5] == uint16_t(4));
		REQUIRE(t[6] == uint16_t(trigger::association_lt | trigger::blockade));
		REQUIRE(t[7] == uint16_t(2));
		REQUIRE(t[8] == uint16_t(1));
	}
}

TEST_CASE("scope absorbsion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::x_neighbor_province_scope | trigger::is_disjunctive_scope | trigger::is_existence_scope));
		t.push_back(uint16_t(8));
		t.push_back(uint16_t(trigger::is_scope | trigger::generic_scope));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		t.push_back(uint16_t(0));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(7 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::is_scope | trigger::x_neighbor_province_scope | trigger::is_existence_scope));
		REQUIRE(t[1] == uint16_t(6));
		REQUIRE(t[2] == uint16_t(trigger::association_eq | trigger::blockade));
		REQUIRE(t[3] == uint16_t(2));
		REQUIRE(t[4] == uint16_t(1));
		REQUIRE(t[5] == uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		REQUIRE(t[6] == uint16_t(0));
	}
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_scope | trigger::integer_scope));
		t.push_back(uint16_t(9));
		t.push_back(uint16_t(15));
		t.push_back(uint16_t(trigger::is_scope | trigger::generic_scope | trigger::is_disjunctive_scope));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		t.push_back(uint16_t(0));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(8 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::is_scope | trigger::integer_scope | trigger::is_disjunctive_scope));
		REQUIRE(t[1] == uint16_t(7));
		REQUIRE(t[2] == uint16_t(15));
		REQUIRE(t[3] == uint16_t(trigger::association_eq | trigger::blockade));
		REQUIRE(t[4] == uint16_t(2));
		REQUIRE(t[5] == uint16_t(1));
		REQUIRE(t[6] == uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		REQUIRE(t[7] == uint16_t(0));
	}
}


TEST_CASE("trivial trigger", "[trigger_tests]") {
	const char trigger_text[] = "always = no";

	parsers::error_handler err("no file");
	parsers::token_generator gen(trigger_text, trigger_text + strlen(trigger_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::trigger_building_context tc(context, trigger::slot_contents::empty, trigger::slot_contents::empty, trigger::slot_contents::empty);

	parsers::parse_trigger_body(gen, err, tc);

	REQUIRE(size_t(1) == tc.compiled_trigger.size());
	REQUIRE(uint16_t(::trigger::always | ::trigger::no_payload | ::trigger::association_ne) == tc.compiled_trigger[0]);
}


TEST_CASE("scope sensitive trigger", "[trigger_tests]") {
	const char trigger_text[] = "militancy < 12";

	parsers::error_handler err("no file");
	parsers::token_generator gen(trigger_text, trigger_text + strlen(trigger_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::trigger_building_context tc(context, trigger::slot_contents::province, trigger::slot_contents::empty, trigger::slot_contents::empty);

	parsers::parse_trigger_body(gen, err, tc);

	REQUIRE(size_t(3) == tc.compiled_trigger.size());
	REQUIRE(uint16_t(::trigger::militancy_province | ::trigger::association_lt) == tc.compiled_trigger[0]);
}

TEST_CASE("scope trigger", "[trigger_tests]") {
	const char trigger_text[] = "this = { average_consciousness >= 1.0 average_militancy != 0.5 }";

	parsers::error_handler err("no file");
	parsers::token_generator gen(trigger_text, trigger_text + strlen(trigger_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::trigger_building_context tc(context, trigger::slot_contents::state, trigger::slot_contents::province, trigger::slot_contents::empty);

	parsers::parse_trigger_body(gen, err, tc);

	REQUIRE(size_t(8) == tc.compiled_trigger.size());
	REQUIRE(tc.compiled_trigger[0] == uint16_t(trigger::is_scope | trigger::this_scope_state));
	REQUIRE(tc.compiled_trigger[1] == uint16_t(7));
	REQUIRE(tc.compiled_trigger[2] == uint16_t(trigger::association_ge | trigger::average_consciousness_province));
	REQUIRE(tc.compiled_trigger[5] == uint16_t(trigger::association_ne | trigger::average_militancy_province));
}

TEST_CASE("empty trigger", "[trigger_tests]") {
	const char trigger_text[] = " ";

	parsers::error_handler err("no file");
	parsers::token_generator gen(trigger_text, trigger_text + strlen(trigger_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::trigger_building_context tc(context, trigger::slot_contents::nation, trigger::slot_contents::empty, trigger::slot_contents::empty);

	parsers::parse_trigger_body(gen, err, tc);

	REQUIRE(size_t(0) == tc.compiled_trigger.size());
}

TEST_CASE("complex full reduction", "[trigger_tests]") {
	const char trigger_text[] = "NOT = { AND = { AND = { always = yes } any_core = { average_consciousness >= 1.0 } } }";

	parsers::error_handler err("no file");
	parsers::token_generator gen(trigger_text, trigger_text + strlen(trigger_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::trigger_building_context tc(context, trigger::slot_contents::nation, trigger::slot_contents::empty, trigger::slot_contents::empty);

	parsers::parse_trigger_body(gen, err, tc);

	const auto new_size = parsers::simplify_trigger(tc.compiled_trigger.data());
	tc.compiled_trigger.resize(static_cast<size_t>(new_size));

	REQUIRE(size_t(8) == tc.compiled_trigger.size());
	REQUIRE(tc.compiled_trigger[0] == uint16_t(trigger::is_scope | trigger::is_disjunctive_scope | trigger::generic_scope));
	REQUIRE(tc.compiled_trigger[1] == uint16_t(7));
	REQUIRE(tc.compiled_trigger[2] == uint16_t(trigger::association_ne | trigger::no_payload | trigger::always));
	REQUIRE(tc.compiled_trigger[3] == uint16_t(trigger::is_scope | trigger::is_disjunctive_scope | trigger::x_core_scope_nation));
	REQUIRE(tc.compiled_trigger[4] == uint16_t(4));
	REQUIRE(tc.compiled_trigger[5] == uint16_t(trigger::association_lt | trigger::average_consciousness_province));
}

