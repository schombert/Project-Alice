#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"

#include <array>
#include <cstdint>

namespace nations::policy_execution {

enum class policy_kind : uint8_t {
	crime_suppression,
	education,
	social_benefits,
	reform_implementation,
	mobilization_logistics,
};

enum class capacity_factor : uint8_t {
	none,
	national_administration,
	local_control,
	funding,
	bureaucratic_labor,
	political_compliance,
};

struct inputs {
	bool enabled = false;
	float national_administration = 1.f;
	float local_control = 1.f;
	float funding = 1.f;
	float bureaucratic_labor = 1.f;
	float political_compliance = 1.f;
};

struct weights {
	float national_administration = 0.f;
	float local_control = 0.f;
	float funding = 0.f;
	float bureaucratic_labor = 0.f;
	float political_compliance = 0.f;
};

struct breakdown {
	bool enabled = false;
	policy_kind policy = policy_kind::crime_suppression;
	inputs factors{};
	weights factor_weights{};
	capacity_factor bottleneck = capacity_factor::none;
	float bottleneck_value = 1.f;
	float weighted_capacity = 1.f;
	// Legacy mode deliberately returns one, making the API safe to multiply
	// into existing effects without changing classic rules.
	float effective_execution = 1.f;
};

weights weights_for(policy_kind policy);
breakdown calculate(policy_kind policy, inputs raw_inputs);
breakdown effective_policy(sys::state const& state, dcon::nation_id nation,
	dcon::province_id province, policy_kind policy);
float average_effective_policy(sys::state const& state, dcon::nation_id nation,
	policy_kind policy);

} // namespace nations::policy_execution
