#include "nations/policy_execution.hpp"
#include "demographics.hpp"
#include "system_state.hpp"

namespace execution = nations::policy_execution;

TEST_CASE("policy execution is a legacy-compatible no-op", "[politics][capacity]") {
	execution::inputs inputs;
	inputs.enabled = false;
	inputs.local_control = 0.f;
	auto result = execution::calculate(execution::policy_kind::crime_suppression, inputs);
	REQUIRE_FALSE(result.enabled);
	REQUIRE(result.effective_execution == Approx(1.f));
}

TEST_CASE("policy execution sanitizes inputs and reports a stable bottleneck", "[politics][capacity]") {
	execution::inputs inputs;
	inputs.enabled = true;
	inputs.national_administration = 0.8f;
	inputs.local_control = -1.f;
	inputs.funding = 2.f;
	inputs.bureaucratic_labor = 0.75f;
	inputs.political_compliance = 0.6f;
	auto result = execution::calculate(execution::policy_kind::education, inputs);
	REQUIRE(result.enabled);
	REQUIRE(result.factors.local_control == Approx(0.f));
	REQUIRE(result.factors.funding == Approx(1.f));
	REQUIRE(result.bottleneck == execution::capacity_factor::local_control);
	REQUIRE(result.bottleneck_value == Approx(0.f));
	REQUIRE(std::isfinite(result.effective_execution));
	REQUIRE(result.effective_execution >= 0.f);
	REQUIRE(result.effective_execution <= 1.f);
}

TEST_CASE("policy domains expose different priorities", "[politics][capacity]") {
	execution::inputs inputs;
	inputs.enabled = true;
	inputs.national_administration = 1.f;
	inputs.local_control = 1.f;
	inputs.funding = 0.f;
	inputs.bureaucratic_labor = 1.f;
	inputs.political_compliance = 1.f;
	auto crime = execution::calculate(execution::policy_kind::crime_suppression, inputs);
	auto benefits = execution::calculate(execution::policy_kind::social_benefits, inputs);
	REQUIRE(benefits.effective_execution < crime.effective_execution);
}

TEST_CASE("state-backed policy execution joins administration control labor funding and legitimacy",
	"[politics][capacity][integration]") {
	auto state = std::make_unique<sys::state>();
	auto rule = state->world.create_gamerule();
	state->hardcoded_gamerules.unused_gamerule = rule;
	state->world.gamerule_set_current_setting(
		rule, uint8_t(gamerule::age_of_transformation_settings::enabled));
	auto nation = state->world.create_nation();
	auto province = state->world.create_province();
	state->world.province_resize_labor_demand_satisfaction(economy::labor::total);
	state->world.nation_set_administrative_efficiency(nation, 0.8f);
	state->world.nation_set_administrative_spending(nation, 100);
	state->world.nation_set_spending_level(nation, 1.f);
	state->world.province_set_control_ratio(province, 0.5f);
	state->world.province_set_labor_demand_satisfaction(
		province, economy::labor::high_education_and_accepted, 0.75f);
	state->transformation_politics_cache.resize(state->world.nation_size());
	state->transformation_politics_cache[nation.index()].enabled = true;
	state->transformation_politics_cache[nation.index()].legitimacy.total = 100.f;
	state->transformation_politics_cache_valid = true;

	auto result = execution::effective_policy(
		*state, nation, province, execution::policy_kind::crime_suppression);
	REQUIRE(result.enabled);
	REQUIRE(result.factors.national_administration == Approx(0.8f));
	REQUIRE(result.factors.local_control == Approx(0.5f));
	REQUIRE(result.factors.funding == Approx(1.f));
	REQUIRE(result.factors.bureaucratic_labor == Approx(0.75f));
	REQUIRE(result.factors.political_compliance == Approx(1.f));
	REQUIRE(result.bottleneck == execution::capacity_factor::local_control);
	REQUIRE(result.effective_execution > 0.f);
	REQUIRE(result.effective_execution < 1.f);
}

TEST_CASE("national policy execution is population weighted and stays bounded",
	"[politics][capacity][integration]") {
	auto state = std::make_unique<sys::state>();
	state->force_age_of_transformation_ruleset = true;
	auto nation = state->world.create_nation();
	auto core = state->world.create_province();
	auto periphery = state->world.create_province();
	state->world.province_set_nation_from_province_ownership(core, nation);
	state->world.province_set_nation_from_province_ownership(periphery, nation);
	state->world.province_resize_demographics(demographics::size(*state));
	state->world.province_resize_labor_demand_satisfaction(economy::labor::total);
	state->world.nation_set_administrative_efficiency(nation, 1.f);
	state->world.nation_set_administrative_spending(nation, 100);
	state->world.nation_set_spending_level(nation, 1.f);
	state->world.province_set_control_ratio(core, 1.f);
	state->world.province_set_control_ratio(periphery, 0.f);
	state->world.province_set_demographics(core, demographics::total, 900.f);
	state->world.province_set_demographics(periphery, demographics::total, 100.f);
	state->world.province_set_labor_demand_satisfaction(
		core, economy::labor::high_education, 1.f);
	state->world.province_set_labor_demand_satisfaction(
		periphery, economy::labor::high_education, 1.f);

	auto const core_execution = execution::effective_policy(
		*state, nation, core, execution::policy_kind::reform_implementation).effective_execution;
	auto const periphery_execution = execution::effective_policy(
		*state, nation, periphery, execution::policy_kind::reform_implementation).effective_execution;
	auto const average = execution::average_effective_policy(
		*state, nation, execution::policy_kind::reform_implementation);
	REQUIRE(core_execution > periphery_execution);
	REQUIRE(average > periphery_execution);
	REQUIRE(average < core_execution);
	REQUIRE(average == Approx(core_execution * 0.9f + periphery_execution * 0.1f));
}
