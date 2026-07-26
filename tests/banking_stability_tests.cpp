#include "economy/banking_stability.hpp"
#include "economy/economy.hpp"
#include "nations/nations.hpp"
#include "system_state.hpp"

#include <cmath>
#include <limits>

namespace banking = economy::banking_stability;

TEST_CASE("banking stability is an exact legacy no-op", "[economy][banking]") {
	banking::inputs inputs;
	inputs.enabled = false;
	inputs.outstanding_debt = 900.f;
	inputs.legacy_credit_limit = 1000.f;
	auto const result = banking::calculate(inputs);
	REQUIRE_FALSE(result.enabled);
	REQUIRE(result.credit_health == Approx(1.f));
	REQUIRE(result.financial_stress == Approx(0.f));
	REQUIRE(result.interest_cost_multiplier == Approx(1.f));
	REQUIRE(result.effective_credit_limit == Approx(1000.f));
}

TEST_CASE("banking stability rewards reserves headroom and debt service", "[economy][banking]") {
	banking::inputs healthy;
	healthy.enabled = true;
	healthy.outstanding_debt = 100.f;
	healthy.legacy_credit_limit = 1000.f;
	healthy.banking_reserves = 1000.f;
	healthy.private_investment = 1000.f;
	healthy.daily_income = 100.f;
	healthy.base_daily_interest_due = 1.f;
	auto stressed = healthy;
	stressed.outstanding_debt = 950.f;
	stressed.banking_reserves = 100.f;
	stressed.private_investment = 0.f;
	stressed.daily_income = 0.f;
	stressed.base_daily_interest_due = 20.f;

	auto const healthy_result = banking::calculate(healthy);
	auto const stressed_result = banking::calculate(stressed);
	REQUIRE(healthy_result.credit_health > stressed_result.credit_health);
	REQUIRE(healthy_result.interest_cost_multiplier < stressed_result.interest_cost_multiplier);
	REQUIRE(stressed_result.interest_cost_multiplier >= 1.f);
	REQUIRE(stressed_result.interest_cost_multiplier <= 1.5f);
	REQUIRE(stressed_result.effective_credit_limit >= 500.f);
	REQUIRE(stressed_result.effective_credit_limit <= 1000.f);
}

TEST_CASE("banking stability sanitizes invalid monetary inputs", "[economy][banking]") {
	banking::inputs inputs;
	inputs.enabled = true;
	inputs.outstanding_debt = std::numeric_limits<float>::infinity();
	inputs.legacy_credit_limit = std::numeric_limits<float>::quiet_NaN();
	inputs.banking_reserves = -100.f;
	inputs.private_investment = std::numeric_limits<float>::infinity();
	inputs.daily_income = -1.f;
	inputs.base_daily_interest_due = std::numeric_limits<float>::quiet_NaN();
	auto const result = banking::calculate(inputs);
	REQUIRE(std::isfinite(result.credit_health));
	REQUIRE(std::isfinite(result.financial_stress));
	REQUIRE(std::isfinite(result.effective_credit_limit));
	REQUIRE(std::isfinite(result.interest_cost_multiplier));
	REQUIRE(result.credit_health >= 0.f);
	REQUIRE(result.credit_health <= 1.f);
	REQUIRE(result.interest_cost_multiplier >= 1.f);
	REQUIRE(result.interest_cost_multiplier <= 1.5f);
}

TEST_CASE("default status produces the bounded maximum risk premium", "[economy][banking]") {
	banking::inputs inputs;
	inputs.enabled = true;
	inputs.in_default = true;
	inputs.legacy_credit_limit = 1000.f;
	auto const result = banking::calculate(inputs);
	REQUIRE(result.credit_health == Approx(0.f));
	REQUIRE(result.financial_stress == Approx(1.f));
	REQUIRE(result.interest_cost_multiplier == Approx(1.5f));
}

TEST_CASE("bankruptcy routing treats local loans as positive liabilities", "[economy][banking]") {
	REQUIRE(banking::is_small_default(0.f, 20000.f));
	REQUIRE(banking::is_small_default(20000.f, 20000.f));
	REQUIRE_FALSE(banking::is_small_default(20000.01f, 20000.f));
	REQUIRE_FALSE(banking::is_small_default(
		std::numeric_limits<float>::infinity(), 20000.f));
}

TEST_CASE("state-backed banking uses local loan and preserves classic interest", "[economy][banking][integration]") {
	auto state = std::make_unique<sys::state>();
	state->world.create_commodity();
	auto const nation = state->world.create_nation();
	state->world.nation_resize_stockpiles(state->world.commodity_size());
	state->world.nation_resize_modifier_values(sys::national_mod_offsets::count);
	auto const rule = state->world.create_gamerule();
	state->hardcoded_gamerules.unused_gamerule = rule;
	state->defines.loan_base_interest = 0.03f;
	state->world.nation_set_local_loan(nation, 900.f);
	state->world.nation_set_national_bank(nation, 1000.f);
	state->world.nation_set_private_investment(nation, 50.f);

	state->world.gamerule_set_current_setting(
		rule, uint8_t(gamerule::age_of_transformation_settings::disabled));
	auto const legacy_interest = economy::interest_payment(*state, nation);
	REQUIRE(legacy_interest == Approx(0.9f));
	REQUIRE_FALSE(banking::evaluate_nation(*state, nation).enabled);

	state->world.gamerule_set_current_setting(
		rule, uint8_t(gamerule::age_of_transformation_settings::enabled));
	auto const transformed = banking::evaluate_nation(*state, nation);
	REQUIRE(transformed.enabled);
	REQUIRE(transformed.sanitized.outstanding_debt == Approx(900.f));
	REQUIRE(economy::interest_payment(*state, nation)
		== Approx(legacy_interest * transformed.interest_cost_multiplier));
	REQUIRE(nations::get_debt(*state, nation) == Approx(900.f));
}
