#include "banking_stability.hpp"

#include "gamerule.hpp"
#include "money.hpp"
#include "system_state.hpp"

#include <algorithm>
#include <cmath>

namespace economy::banking_stability {
namespace {

constexpr float epsilon = 0.0001f;
constexpr float maximum_risk_premium = 0.50f;

float nonnegative(float value) {
	return std::isfinite(value) ? std::max(0.f, value) : 0.f;
}

float unit(float value) {
	return std::isfinite(value) ? std::clamp(value, 0.f, 1.f) : 0.f;
}

float remaining_share(float used, float available) {
	if(available <= epsilon)
		return used <= epsilon ? 1.f : 0.f;
	return unit(1.f - used / available);
}

float buffer_ratio(float buffer, float liability) {
	if(liability <= epsilon)
		return 1.f;
	return unit(buffer / liability);
}

} // namespace

breakdown calculate(inputs raw_inputs) {
	breakdown result;
	result.effective_credit_limit = nonnegative(raw_inputs.legacy_credit_limit);
	if(!raw_inputs.enabled)
		return result;

	result.enabled = true;
	result.in_default = raw_inputs.in_default;
	result.sanitized.enabled = true;
	result.sanitized.in_default = raw_inputs.in_default;
	result.sanitized.outstanding_debt = nonnegative(raw_inputs.outstanding_debt);
	result.sanitized.legacy_credit_limit = nonnegative(raw_inputs.legacy_credit_limit);
	result.sanitized.banking_reserves = nonnegative(raw_inputs.banking_reserves);
	result.sanitized.private_investment = nonnegative(raw_inputs.private_investment);
	result.sanitized.liquid_treasury = nonnegative(raw_inputs.liquid_treasury);
	result.sanitized.daily_income = nonnegative(raw_inputs.daily_income);
	result.sanitized.base_daily_interest_due = nonnegative(raw_inputs.base_daily_interest_due);

	auto const debt = result.sanitized.outstanding_debt;
	result.credit_headroom = remaining_share(debt, result.sanitized.legacy_credit_limit);
	// national_bank stores both outstanding loans and funds still available for
	// lending, so debt encumbers that stock without being subtracted from it.
	result.reserve_coverage = remaining_share(debt, result.sanitized.banking_reserves);
	result.private_capital_buffer = buffer_ratio(result.sanitized.private_investment, debt);

	if(result.sanitized.base_daily_interest_due <= epsilon) {
		result.debt_service_coverage = 1.f;
	} else {
		// Thirty days of treasury are treated as a liquid fiscal buffer. Full
		// coverage requires resources equal to twice the daily interest bill.
		auto const daily_resources = result.sanitized.daily_income
			+ result.sanitized.liquid_treasury / 30.f;
		result.debt_service_coverage = unit(
			daily_resources / (2.f * result.sanitized.base_daily_interest_due));
	}

	result.credit_health = unit(
		0.30f * result.credit_headroom
		+ 0.25f * result.reserve_coverage
		+ 0.30f * result.debt_service_coverage
		+ 0.15f * result.private_capital_buffer);
	if(result.in_default)
		result.credit_health = 0.f;
	result.financial_stress = unit(1.f - result.credit_health);
	// Keep the derived limit above half of the legacy limit. The first release
	// exposes it to diagnostics rather than feeding it back into max_loan.
	result.effective_credit_limit = result.sanitized.legacy_credit_limit
		* (0.50f + 0.50f * result.credit_health);
	result.interest_cost_multiplier = 1.f
		+ maximum_risk_premium * result.financial_stress * result.financial_stress;
	return result;
}

bool is_small_default(float outstanding_debt, float small_debt_limit) {
	if(!std::isfinite(outstanding_debt))
		return false;
	return std::max(0.f, outstanding_debt) <= nonnegative(small_debt_limit);
}

breakdown evaluate_nation(sys::state const& state, dcon::nation_id nation) {
	inputs derived;
	if(!gamerule::age_of_transformation_enabled(state)
		|| !nation || !state.world.nation_is_valid(nation)) {
		return calculate(derived);
	}

	derived.enabled = true;
	auto const bankrupt_until = state.world.nation_get_bankrupt_until(nation);
	derived.in_default = bankrupt_until && state.current_date < bankrupt_until;
	derived.outstanding_debt = state.world.nation_get_local_loan(nation);
	derived.banking_reserves = state.world.nation_get_national_bank(nation);
	derived.private_investment = state.world.nation_get_private_investment(nation);
	derived.liquid_treasury = state.world.nation_get_stockpiles(nation, economy::money);
	derived.daily_income = state.world.nation_get_total_rich_income(nation)
		+ state.world.nation_get_total_middle_income(nation)
		+ state.world.nation_get_total_poor_income(nation);
	auto const loan_modifier = state.world.nation_get_modifier_values(
		nation, sys::national_mod_offsets::loan_interest);
	auto const base_interest_rate = std::max(
		0.01f, (loan_modifier + 1.f) * state.defines.loan_base_interest);
	derived.base_daily_interest_due = nonnegative(derived.outstanding_debt)
		* base_interest_rate / 30.f;
	auto const credit_modifier = state.world.nation_get_modifier_values(
		nation, sys::national_mod_offsets::max_loan_modifier) + 1.f;
	derived.legacy_credit_limit = std::max(
		0.f, (derived.daily_income + nonnegative(derived.banking_reserves)) * credit_modifier);
	return calculate(derived);
}

} // namespace economy::banking_stability
