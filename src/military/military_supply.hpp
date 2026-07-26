#pragma once

#include "dcon_generated_ids.hpp"

namespace military {

// Army logistics breakdown used by reinforcement and diagnostic UI.
// Instances stored on sys::state are derived snapshots and are never serialized.
struct army_supply_access_data {
	float spatial_access = 0.0f;
	float effective_supply = 0.0f;
	float effective_supply_before_capacity = 0.0f;
	float reinforcement_factor = 0.25f;
	float capacity_factor = 1.0f;
	float route_capacity = 0.0f;
	float route_demand = 0.0f;
	float army_demand = 0.0f;
	float supply_reserve = 0.0f;
	float reserve_daily_change = 0.0f;
	float replacement_load = 0.0f;
	float depot_delivery_factor = 1.0f;
	float depot_stockpile = 0.0f;
	float depot_capacity = 0.0f;
	float distance_factor = 0.0f;
	float infrastructure_factor = 0.0f;
	float control_factor = 0.0f;
	float military_goods_availability = 0.0f;
	float distance_km = 0.0f;
	float sea_distance_km = 0.0f;
	dcon::province_id source;
	dcon::province_id embark_port;
	dcon::province_id disembark_port;
	bool uses_sea_route = false;
	bool source_is_depot = false;
	bool reachable = false;
};

} // namespace military
