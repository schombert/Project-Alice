#include "catch.hpp"
#include "parsers_declarations.hpp"

TEST_CASE("trigger scope recursion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::generic_scope));
		t.push_back(uint16_t(11));
		t.push_back(uint16_t(trigger::no_payload | trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::state_scope_pop));
		t.push_back(uint16_t(3));
		t.push_back(uint16_t(trigger::association_eq | trigger::owns));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::no_payload | trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(0));

		int32_t total = 0;
		trigger::recurse_over_triggers(t.data(), [&total](uint16_t *) { ++total; });
		REQUIRE(5 == total);

		int32_t blockade_count = 0;
		trigger::recurse_over_triggers(t.data(), [&blockade_count](uint16_t *v) { if((*v & trigger::code_mask) == trigger::blockade) ++blockade_count; });
		REQUIRE(2 == blockade_count);
	}
}

TEST_CASE("effect scope recursion", "[effect_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::generic_scope));
		t.push_back(uint16_t(11));
		t.push_back(uint16_t(effect::no_payload | effect::add_core_from_province));
		t.push_back(uint16_t(effect::state_scope_pop));
		t.push_back(uint16_t(8));
		t.push_back(uint16_t(effect::add_core_from_province));
		t.push_back(uint16_t(effect::add_war_goal));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(effect::add_casus_belli_tag));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(3));

		int32_t total = 0;
		parsers::recurse_over_effects(t.data(), [&total](uint16_t *) { ++total; });
		REQUIRE(6 == total);

		int32_t no_payload_count = 0;
		parsers::recurse_over_effects(t.data(), [&no_payload_count](uint16_t *v) { if(effect::get_generic_effect_payload_size(v) == 0) ++no_payload_count; });
		REQUIRE(2 == no_payload_count);

		int32_t total_payload = 0;
		parsers::recurse_over_effects(t.data(), [&total_payload](uint16_t *v) { total_payload += effect::get_generic_effect_payload_size(v); });
		REQUIRE(23 == total_payload);
	}
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::generic_scope));
		t.push_back(uint16_t(14));
		t.push_back(uint16_t(effect::no_payload | effect::add_core_from_province));
		t.push_back(uint16_t(effect::scope_has_limit | effect::integer_scope));
		t.push_back(uint16_t(11));
		t.push_back(trigger::payload(dcon::trigger_key()).value);
		t.push_back(uint16_t(100));
		t.push_back(uint16_t(effect::define_general));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(3));
		t.push_back(uint16_t(effect::no_payload | effect::secede_province_this_state));
		t.push_back(uint16_t(effect::war_exhaustion));
		t.push_back(uint16_t(7));
		t.push_back(uint16_t(8));

		int32_t total = 0;
		parsers::recurse_over_effects(t.data(), [&total](uint16_t *) { ++total; });
		REQUIRE(6 == total);

		int32_t no_payload_count = 0;
		parsers::recurse_over_effects(t.data(), [&no_payload_count](uint16_t *v) { if(effect::get_generic_effect_payload_size(v) == 0) ++no_payload_count; });
		REQUIRE(2 == no_payload_count);

		int32_t total_payload = 0;
		parsers::recurse_over_effects(t.data(), [&total_payload](uint16_t *v) { total_payload += effect::get_generic_effect_payload_size(v); });
		REQUIRE(30 == total_payload);
	}
}

TEST_CASE("trigger inversion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::placeholder_not_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(0));

		parsers::invert_trigger(t.data());

		REQUIRE(t[0] == uint16_t(trigger::is_disjunctive_scope | trigger::placeholder_not_scope));
		REQUIRE(t[1] == uint16_t(4));
		REQUIRE(t[2] == uint16_t(trigger::association_ne | trigger::blockade));
		REQUIRE(t[3] == uint16_t(0));
		REQUIRE(t[4] == uint16_t(0));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::is_existence_scope | trigger::is_disjunctive_scope | trigger::x_core_scope_nation));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_ge | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		parsers::invert_trigger(t.data());

		REQUIRE(t[0] == uint16_t(trigger::x_core_scope_nation));
		REQUIRE(t[1] == uint16_t(4));
		REQUIRE(t[2] == uint16_t(trigger::association_lt | trigger::blockade));
		REQUIRE(t[3] == uint16_t(2));
		REQUIRE(t[4] == uint16_t(1));
	}
}

TEST_CASE("scope sizing", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::placeholder_not_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(4));

		REQUIRE(false == parsers::scope_is_empty(t.data()));
		REQUIRE(true == parsers::scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::placeholder_not_scope));
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

		t.push_back(uint16_t(trigger::integer_scope));
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

		t.push_back(uint16_t(trigger::integer_scope));
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

		t.push_back(uint16_t(trigger::placeholder_not_scope));
		t.push_back(uint16_t(1));

		REQUIRE(true == parsers::scope_is_empty(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::integer_scope));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		REQUIRE(true == parsers::scope_is_empty(t.data()));
	}
}

TEST_CASE("effect scope sizing", "[effect_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::generic_scope));
		t.push_back(uint16_t(3));
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(2));

		REQUIRE(true == effect::effect_scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::generic_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(1));

		REQUIRE(false == effect::effect_scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::integer_scope));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(400));
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(4));

		REQUIRE(true == effect::effect_scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::integer_scope));
		t.push_back(uint16_t(5));
		t.push_back(uint16_t(400));
		t.push_back(uint16_t(effect::add_core_from_province));
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(4));

		REQUIRE(false == effect::effect_scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::scope_has_limit | effect::integer_scope));
		t.push_back(uint16_t(5));
		t.push_back(trigger::payload(dcon::trigger_key(1)).value);
		t.push_back(uint16_t(400));
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(400));

		REQUIRE(true == effect::effect_scope_has_single_member(t.data()));
	}

	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::scope_has_limit | effect::integer_scope));
		t.push_back(uint16_t(7));
		t.push_back(trigger::payload(dcon::trigger_key(1)).value);
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(effect::no_payload | effect::add_accepted_culture));
		t.push_back(uint16_t(0));

		REQUIRE(false == effect::effect_scope_has_single_member(t.data()));
	}
}

TEST_CASE("simple negation removal", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::placeholder_not_scope));
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

		t.push_back(uint16_t(trigger::placeholder_not_scope));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(trigger::placeholder_not_scope));
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

		t.push_back(uint16_t(trigger::placeholder_not_scope));
		t.push_back(uint16_t(10));
		t.push_back(uint16_t(trigger::generic_scope));
		t.push_back(uint16_t(3));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		t.push_back(uint16_t(0));
		t.push_back(uint16_t(trigger::is_existence_scope | trigger::is_disjunctive_scope | trigger::x_core_scope_nation));
		t.push_back(uint16_t(4));
		t.push_back(uint16_t(trigger::association_ge | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(9 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::is_disjunctive_scope | trigger::generic_scope));
		REQUIRE(t[1] == uint16_t(8));
		REQUIRE(t[2] == uint16_t(trigger::association_ne | trigger::no_payload | trigger::owns));
		REQUIRE(t[3] == uint16_t(0));
		REQUIRE(t[4] == uint16_t(trigger::x_core_scope_nation));
		REQUIRE(t[5] == uint16_t(4));
		REQUIRE(t[6] == uint16_t(trigger::association_lt | trigger::blockade));
		REQUIRE(t[7] == uint16_t(2));
		REQUIRE(t[8] == uint16_t(1));
	}
}

TEST_CASE("scope absorbsion", "[trigger_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::x_neighbor_province_scope | trigger::is_disjunctive_scope | trigger::is_existence_scope));
		t.push_back(uint16_t(8));
		t.push_back(uint16_t(trigger::generic_scope));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		t.push_back(uint16_t(0));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(7 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::x_neighbor_province_scope | trigger::is_existence_scope));
		REQUIRE(t[1] == uint16_t(6));
		REQUIRE(t[2] == uint16_t(trigger::association_eq | trigger::blockade));
		REQUIRE(t[3] == uint16_t(2));
		REQUIRE(t[4] == uint16_t(1));
		REQUIRE(t[5] == uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		REQUIRE(t[6] == uint16_t(0));
	}
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(trigger::integer_scope));
		t.push_back(uint16_t(9));
		t.push_back(uint16_t(15));
		t.push_back(uint16_t(trigger::generic_scope | trigger::is_disjunctive_scope));
		t.push_back(uint16_t(6));
		t.push_back(uint16_t(trigger::association_eq | trigger::blockade));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(1));
		t.push_back(uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		t.push_back(uint16_t(0));

		const auto new_size = parsers::simplify_trigger(t.data());

		REQUIRE(8 == new_size);
		REQUIRE(t[0] == uint16_t(trigger::integer_scope | trigger::is_disjunctive_scope));
		REQUIRE(t[1] == uint16_t(7));
		REQUIRE(t[2] == uint16_t(15));
		REQUIRE(t[3] == uint16_t(trigger::association_eq | trigger::blockade));
		REQUIRE(t[4] == uint16_t(2));
		REQUIRE(t[5] == uint16_t(1));
		REQUIRE(t[6] == uint16_t(trigger::association_eq | trigger::no_payload | trigger::owns));
		REQUIRE(t[7] == uint16_t(0));
	}
}

TEST_CASE("effect scope absorbsion", "[effect_tests]") {
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::scope_has_limit | effect::generic_scope));
		t.push_back(uint16_t(8));
		t.push_back(trigger::payload(dcon::trigger_key()).value);
		t.push_back(uint16_t(effect::scope_has_limit | effect::x_neighbor_province_scope));
		t.push_back(uint16_t(5));
		t.push_back(trigger::payload(dcon::trigger_key()).value);
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(7));
		t.push_back(uint16_t(effect::no_payload | effect::add_core_this_pop));

		const auto new_size = parsers::simplify_effect(t.data());

		REQUIRE(5 == new_size);
		REQUIRE(t[0] == uint16_t(effect::x_neighbor_province_scope));
		REQUIRE(t[1] == uint16_t(4));
		REQUIRE(t[2] == uint16_t(effect::add_accepted_culture));
		REQUIRE(t[3] == uint16_t(7));
		REQUIRE(t[4] == uint16_t(effect::no_payload | effect::add_core_this_pop));
	}
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::scope_has_limit | effect::x_neighbor_province_scope));
		t.push_back(uint16_t(8));
		t.push_back(trigger::payload(dcon::trigger_key()).value);
		t.push_back(uint16_t(effect::scope_has_limit | effect::generic_scope));
		t.push_back(uint16_t(5));
		t.push_back(trigger::payload(dcon::trigger_key()).value);
		t.push_back(uint16_t(effect::add_accepted_culture));
		t.push_back(uint16_t(2));
		t.push_back(uint16_t(effect::no_payload | effect::add_core_this_pop));

		const auto new_size = parsers::simplify_effect(t.data());

		REQUIRE(7 == new_size);
		REQUIRE(t[0] == uint16_t(effect::x_neighbor_province_scope));
		REQUIRE(t[1] == uint16_t(6));
		REQUIRE(t[2] == uint16_t(effect::generic_scope));
		REQUIRE(t[3] == uint16_t(4));
		REQUIRE(t[4] == uint16_t(effect::add_accepted_culture));
		REQUIRE(t[5] == uint16_t(2));
		REQUIRE(t[6] == uint16_t(effect::no_payload | effect::add_core_this_pop));
	}
	{
		std::vector<uint16_t> t;

		t.push_back(uint16_t(effect::scope_has_limit | effect::integer_scope));
		t.push_back(uint16_t(9));
		t.push_back(trigger::payload(dcon::trigger_key(3)).value);
		t.push_back(uint16_t(32));
		t.push_back(uint16_t(effect::generic_scope));
		t.push_back(uint16_t(5));
		t.push_back(uint16_t(effect::activate_technology));
		t.push_back(uint16_t(3));
		t.push_back(uint16_t(effect::no_payload | effect::add_accepted_culture));
		t.push_back(uint16_t(2));

		const auto new_size = parsers::simplify_effect(t.data());

		REQUIRE(10 == new_size);
		REQUIRE(t[0] == uint16_t(effect::scope_has_limit | effect::integer_scope));
		REQUIRE(t[1] == uint16_t(9));
		REQUIRE(trigger::payload(t[2]).tr_id == dcon::trigger_key(3));
		REQUIRE(t[3] == uint16_t(32));
		REQUIRE(t[4] == uint16_t(effect::generic_scope));
		REQUIRE(t[5] == uint16_t(5));
		REQUIRE(t[6] == uint16_t(effect::activate_technology));
		REQUIRE(t[7] == uint16_t(3));
		REQUIRE(t[8] == uint16_t(effect::no_payload | effect::add_accepted_culture));
		REQUIRE(t[9] == uint16_t(2));
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

TEST_CASE("trivial effect", "[effect_tests]") {
	const char effect_text[] = "is_slave = yes";

	parsers::error_handler err("no file");
	parsers::token_generator gen(effect_text, effect_text + strlen(effect_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::effect_building_context tc(context, trigger::slot_contents::state, trigger::slot_contents::state, trigger::slot_contents::empty);

	parsers::parse_effect_body(gen, err, tc);

	REQUIRE(size_t(1) == tc.compiled_effect.size());
	REQUIRE(uint16_t(effect::is_slave_state_yes | effect::no_payload) == tc.compiled_effect[0]);
}

TEST_CASE("multipart effect", "[effect_tests]") {
	const char effect_text[] = "relation = { value = -10 who = THIS }";

	parsers::error_handler err("no file");
	parsers::token_generator gen(effect_text, effect_text + strlen(effect_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::effect_building_context tc(context, trigger::slot_contents::nation, trigger::slot_contents::province, trigger::slot_contents::empty);

	parsers::parse_effect_body(gen, err, tc);

	REQUIRE(size_t(2) == tc.compiled_effect.size());
	REQUIRE(tc.compiled_effect[0] == uint16_t(effect::relation_this_province));
	REQUIRE(trigger::payload(tc.compiled_effect[1]).signed_value == int16_t(-10));
}

TEST_CASE("effect scope", "[effect_tests]") {
	const char effect_text[] = "owner = { world_wars_enabled = yes treasury = 0.5 }";

	parsers::error_handler err("no file");
	parsers::token_generator gen(effect_text, effect_text + strlen(effect_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::effect_building_context tc(context, trigger::slot_contents::province, trigger::slot_contents::empty, trigger::slot_contents::empty);

	parsers::parse_effect_body(gen, err, tc);

	const auto new_size = parsers::simplify_effect(tc.compiled_effect.data());
	tc.compiled_effect.resize(static_cast<size_t>(new_size));

	REQUIRE(size_t(6) == tc.compiled_effect.size());
	REQUIRE(tc.compiled_effect[0] == uint16_t(effect::owner_scope_province));
	REQUIRE(tc.compiled_effect[1] == uint16_t(5));
	REQUIRE(tc.compiled_effect[2] == uint16_t(effect::world_wars_enabled_yes | effect::no_payload));
	REQUIRE(tc.compiled_effect[3] == uint16_t(effect::treasury));
}

TEST_CASE("effect with limit", "[effect_tests]") {
	const char effect_text[] = "any_country = { limit = { tag = this } treasury = 0.5 }";

	parsers::error_handler err("no file");
	parsers::token_generator gen(effect_text, effect_text + strlen(effect_text));

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	parsers::scenario_building_context context(*state);
	parsers::effect_building_context tc(context, trigger::slot_contents::province, trigger::slot_contents::province, trigger::slot_contents::empty);

	parsers::parse_effect_body(gen, err, tc);

	const auto new_size = parsers::simplify_effect(tc.compiled_effect.data());
	tc.compiled_effect.resize(static_cast<size_t>(new_size));

	REQUIRE(size_t(6) == tc.compiled_effect.size());
	REQUIRE(tc.compiled_effect[0] == uint16_t(effect::x_country_scope | effect::scope_has_limit));
	REQUIRE(tc.compiled_effect[1] == uint16_t(5));
	REQUIRE(trigger::payload(tc.compiled_effect[2]).tr_id == dcon::trigger_key(0));
	REQUIRE(tc.compiled_effect[3] == uint16_t(effect::treasury));

	REQUIRE(size_t(1) == state->trigger_data.size());
	REQUIRE(uint16_t(trigger::tag_this_province | trigger::association_eq | trigger::no_payload) == state->trigger_data[0]);
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
	REQUIRE(tc.compiled_trigger[0] == uint16_t(trigger::this_scope_state));
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
	REQUIRE(tc.compiled_trigger[0] == uint16_t(trigger::is_disjunctive_scope | trigger::generic_scope));
	REQUIRE(tc.compiled_trigger[1] == uint16_t(7));
	REQUIRE(tc.compiled_trigger[2] == uint16_t(trigger::association_ne | trigger::no_payload | trigger::always));
	REQUIRE(tc.compiled_trigger[3] == uint16_t(trigger::is_disjunctive_scope | trigger::x_core_scope_nation));
	REQUIRE(tc.compiled_trigger[4] == uint16_t(4));
	REQUIRE(tc.compiled_trigger[5] == uint16_t(trigger::association_lt | trigger::average_consciousness_province));
}

TEST_CASE("batch-individual comparision", "[trigger_tests]") {
	auto ws = load_testing_scenario_file();

	{
		auto modifier = ws->world.pop_type_get_promotion(ws->culture_definitions.bureaucrat, ws->culture_definitions.secondary_factory_worker);
		REQUIRE(bool(modifier) == true);

		ve::contiguous_tags<dcon::pop_id> g1(16);
		auto bulk_eval = trigger::evaluate_additive_modifier(*ws, modifier, trigger::to_generic(g1), trigger::to_generic(nations::owner_of_pop(*ws, g1)), 0);

		ve::apply([&](float v1, dcon::pop_id p, dcon::nation_id n) {
			auto single_eval = trigger::evaluate_additive_modifier(*ws, modifier, trigger::to_generic(p), trigger::to_generic(n), 0);
			REQUIRE(single_eval == v1);
		},
		          bulk_eval, g1, nations::owner_of_pop(*ws, g1));
	}
	{
		auto modifier = ws->world.pop_type_get_promotion(ws->culture_definitions.primary_factory_worker, ws->culture_definitions.secondary_factory_worker);
		REQUIRE(bool(modifier) == true);

		ve::contiguous_tags<dcon::pop_id> g1(32);
		auto bulk_eval = trigger::evaluate_additive_modifier(*ws, modifier, trigger::to_generic(g1), trigger::to_generic(nations::owner_of_pop(*ws, g1)), 0);

		ve::apply([&](float v1, dcon::pop_id p, dcon::nation_id n) {
			auto single_eval = trigger::evaluate_additive_modifier(*ws, modifier, trigger::to_generic(p), trigger::to_generic(n), 0);
			REQUIRE(single_eval == v1);
		},
		          bulk_eval, g1, nations::owner_of_pop(*ws, g1));
	}
	{
		auto modifier = ws->world.pop_type_get_promotion(ws->culture_definitions.aristocrat, ws->culture_definitions.capitalists);
		REQUIRE(bool(modifier) == true);

		ve::contiguous_tags<dcon::pop_id> g1(48);
		auto bulk_eval = trigger::evaluate_additive_modifier(*ws, modifier, trigger::to_generic(g1), trigger::to_generic(nations::owner_of_pop(*ws, g1)), 0);

		ve::apply([&](float v1, dcon::pop_id p, dcon::nation_id n) {
			auto single_eval = trigger::evaluate_additive_modifier(*ws, modifier, trigger::to_generic(p), trigger::to_generic(n), 0);
			REQUIRE(single_eval == v1);
		},
		          bulk_eval, g1, nations::owner_of_pop(*ws, g1));
	}
}

TEST_CASE("trigger payload translation", "[trigger_tests]") {
	{
		auto old_d = trigger::to_generic(dcon::province_id{42});
		auto new_d = trigger::to_prov(old_d);
		REQUIRE(new_d == dcon::province_id{42});
	}
	{
		auto old_d = trigger::to_generic(dcon::nation_id{42});
		auto new_d = trigger::to_nation(old_d);
		REQUIRE(new_d == dcon::nation_id{42});
	}
}
