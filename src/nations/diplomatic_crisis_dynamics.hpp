#pragma once

#include "system_state_forward.hpp"

#include <cstdint>
#include <string_view>

namespace nations::diplomatic_crisis_dynamics {

// These are diagnostic stages layered over the legacy crisis state machine.
// They are deliberately runtime-only: no value from this module is serialized.
enum class escalation_stage : uint8_t {
	inactive = 0,
	diplomatic_opening = 1,
	bargaining = 2,
	confrontation = 3,
	brinkmanship = 4,
	war_imminent = 5,
};

struct inputs {
	// Compatibility boundary. Disabled calculations return an exact no-op
	// result, even if the remaining inputs contain malformed values.
	bool enabled = false;
	bool active = false;
	// Native Project Alice crisis temperature, expected in [0, 100].
	float temperature = 0.f;
	uint64_t total_participants = 0;
	uint64_t committed_attackers = 0;
	uint64_t committed_defenders = 0;
	float attacker_power = 0.f;
	float defender_power = 0.f;
	// Normalized [0, 1] values. The state adapter converts native 0..100 war
	// exhaustion before invoking the pure calculation.
	float attacker_war_exhaustion = 0.f;
	float defender_war_exhaustion = 0.f;
	uint64_t attacker_wargoals = 0;
	uint64_t defender_wargoals = 0;
};

struct breakdown {
	bool enabled = false;
	escalation_stage stage = escalation_stage::inactive;
	inputs factors{};
	bool missing_committed_side = false;
	float normalized_temperature = 0.f;
	float commitment_ratio = 0.f;
	float bilateral_commitment = 0.f;
	float attacker_power_share = 0.5f;
	float power_asymmetry = 0.f;
	float mean_war_exhaustion = 0.f;
	float claim_pressure = 0.f;
	// Explainable bounded indices, not random probabilities. The simulation
	// continues to use its legacy temperature threshold to create a war.
	float escalation_pressure = 0.f;
	float settlement_pressure = 0.f;
	float war_risk = 0.f;
};

std::string_view stage_name(escalation_stage stage);
escalation_stage stage_for(bool active, float temperature, bool missing_committed_side);
breakdown calculate(inputs raw_inputs);

// Read-only adapter over the existing crisis state, participants, scores,
// exhaustion and wargoals. It is enabled only by Alice: Age of Transformation.
breakdown evaluate_current_crisis(sys::state const& state);

} // namespace nations::diplomatic_crisis_dynamics
