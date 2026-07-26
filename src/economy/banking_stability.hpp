#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"

namespace economy::banking_stability {

// All monetary values use the simulation's existing money units. The module
// derives credit risk from already-serialized stocks and does not own state.
struct inputs {
	bool enabled = false;
	bool in_default = false;
	float outstanding_debt = 0.f;
	float legacy_credit_limit = 0.f;
	float banking_reserves = 0.f;
	float private_investment = 0.f;
	float liquid_treasury = 0.f;
	float daily_income = 0.f;
	float base_daily_interest_due = 0.f;
};

struct breakdown {
	bool enabled = false;
	bool in_default = false;
	inputs sanitized{};
	// Ratios are normalized to [0, 1], where one is healthy.
	float credit_headroom = 1.f;
	float reserve_coverage = 1.f;
	float debt_service_coverage = 1.f;
	float private_capital_buffer = 1.f;
	float credit_health = 1.f;
	float financial_stress = 0.f;
	// Kept diagnostic for now: consumers can display the risk-adjusted amount
	// without changing the legacy borrowing ceiling.
	float effective_credit_limit = 0.f;
	// The only gameplay hook in the MVP. Legacy mode returns exactly one.
	float interest_cost_multiplier = 1.f;
};

breakdown calculate(inputs raw_inputs);

// local_loan is stored as a positive liability. Keep this predicate shared by
// bankruptcy event routing so sign conventions cannot silently diverge again.
bool is_small_default(float outstanding_debt, float small_debt_limit);

// Project-state adapter used by observability and the interest-payment hook.
// It is an exact legacy no-op unless Alice: Age of Transformation is enabled.
breakdown evaluate_nation(sys::state const& state, dcon::nation_id nation);

} // namespace economy::banking_stability
