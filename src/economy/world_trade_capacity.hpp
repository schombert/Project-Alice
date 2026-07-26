#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"

#include <array>

namespace economy::world_trade {

// Soft capacity is deliberately opt-in. Existing scenarios keep their current
// trade dynamics until the Age of Transformation gamerule is enabled.
struct capacity_config {
	bool enabled = false;
	float minimum_expansion_multiplier = 0.10f;
	float maximum_transport_cost_multiplier = 3.00f;
};

struct capacity_inputs {
	float cargo = 0.0f;
	std::array<float, 2> endpoint_capacity{0.0f, 0.0f};
	std::array<float, 2> endpoint_transport_availability{1.0f, 1.0f};
};

struct capacity_result {
	bool enabled = false;
	float cargo = 0.0f;
	float nominal_capacity = 0.0f;
	float transport_availability = 0.0f;
	float effective_capacity = 0.0f;
	float utilization = 0.0f;
	float congestion = 0.0f;
	float headroom = 0.0f;
	float shortfall = 0.0f;
	float expansion_multiplier = 1.0f;
	float transport_cost_multiplier = 1.0f;
};

// Pure, deterministic and finite for arbitrary floating-point inputs.
capacity_result evaluate_capacity(capacity_config const& config, capacity_inputs const& inputs);

capacity_config ruleset_config_for(sys::state const& state);

// State-backed diagnostics/decision API. It derives nominal capacity from the
// existing market max-throughput cache and transport availability from the
// same labor/service signals used by trade-route updates.
capacity_inputs inputs_for_route(sys::state const& state, dcon::trade_route_id route);
capacity_result evaluate_route_capacity(sys::state const& state, dcon::trade_route_id route);

} // namespace economy::world_trade
