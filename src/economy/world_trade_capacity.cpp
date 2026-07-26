#include "world_trade_capacity.hpp"

#include "advanced_province_buildings.hpp"
#include "economy_stats.hpp"
#include "gamerule.hpp"
#include "price.hpp"
#include "province.hpp"
#include "province_templates.hpp"
#include "system_state.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace economy::world_trade {
namespace {

constexpr float maximum_reported_utilization = 1000.0f;

float finite_nonnegative(float value) {
	return std::isfinite(value) && value > 0.0f ? value : 0.0f;
}

float unit_interval(float value) {
	return std::isfinite(value) ? std::clamp(value, 0.0f, 1.0f) : 0.0f;
}

float sanitized_minimum_expansion(float value) {
	return std::isfinite(value) ? std::clamp(value, 0.0f, 1.0f) : 0.10f;
}

float sanitized_maximum_cost(float value) {
	return std::isfinite(value) ? std::clamp(value, 1.0f, 10.0f) : 3.0f;
}

float state_port_availability(sys::state const& state, dcon::state_instance_id state_instance) {
	if(!state_instance || !state.world.state_instance_is_valid(state_instance)) {
		return 0.0f;
	}

	double total_weight = 0.0;
	double weighted_satisfaction = 0.0;
	province::for_each_province_in_state_instance(state, state_instance, [&](dcon::province_id province_id) {
		auto const raw_price = state.world.province_get_service_price(province_id, services::list::port_capacity);
		auto const price = finite_nonnegative(raw_price);
		auto const private_size = finite_nonnegative(
			state.world.province_get_advanced_province_building_max_private_size(
				province_id, advanced_province_buildings::list::civilian_ports));
		auto const weight = (100.0 + double(private_size))
			/ double(price_properties::service::epsilon + price);
		if(!std::isfinite(weight) || weight <= 0.0) {
			return;
		}
		auto const satisfaction = unit_interval(
			state.world.province_get_service_satisfaction(province_id, services::list::port_capacity));
		total_weight += weight;
		weighted_satisfaction += weight * double(satisfaction);
	});

	if(!std::isfinite(total_weight) || !std::isfinite(weighted_satisfaction) || total_weight <= 0.0) {
		return 0.0f;
	}
	return unit_interval(float(weighted_satisfaction / total_weight));
}

float state_land_availability(sys::state const& state, dcon::state_instance_id state_instance) {
	if(!state_instance || !state.world.state_instance_is_valid(state_instance)) {
		return 0.0f;
	}
	auto const capital = state.world.state_instance_get_capital(state_instance);
	if(!capital || !state.world.province_is_valid(capital)) {
		return 0.0f;
	}
	return unit_interval(
		state.world.province_get_labor_demand_satisfaction(capital, labor::no_education));
}

} // namespace

capacity_result evaluate_capacity(capacity_config const& config, capacity_inputs const& inputs) {
	capacity_result result{};
	result.enabled = config.enabled;
	result.cargo = finite_nonnegative(inputs.cargo);
	result.nominal_capacity = std::min(
		finite_nonnegative(inputs.endpoint_capacity[0]),
		finite_nonnegative(inputs.endpoint_capacity[1]));
	result.transport_availability = std::min(
		unit_interval(inputs.endpoint_transport_availability[0]),
		unit_interval(inputs.endpoint_transport_availability[1]));
	result.effective_capacity = result.nominal_capacity * result.transport_availability;
	if(!std::isfinite(result.effective_capacity)) {
		result.effective_capacity = 0.0f;
	}

	result.headroom = std::max(0.0f, result.effective_capacity - result.cargo);
	result.shortfall = std::max(0.0f, result.cargo - result.effective_capacity);

	if(result.cargo > 0.0f) {
		if(result.effective_capacity > 0.0f) {
			result.utilization = std::clamp(
				result.cargo / result.effective_capacity, 0.0f, maximum_reported_utilization);
			result.congestion = std::clamp(
				1.0f - result.effective_capacity / result.cargo, 0.0f, 1.0f);
		} else {
			result.utilization = maximum_reported_utilization;
			result.congestion = 1.0f;
		}
	}

	if(config.enabled) {
		auto const minimum_expansion = sanitized_minimum_expansion(config.minimum_expansion_multiplier);
		auto const maximum_cost = sanitized_maximum_cost(config.maximum_transport_cost_multiplier);
		result.expansion_multiplier = std::clamp(1.0f - result.congestion, minimum_expansion, 1.0f);
		result.transport_cost_multiplier = std::clamp(
			1.0f + result.congestion * (maximum_cost - 1.0f), 1.0f, maximum_cost);
	}

	return result;
}

capacity_config ruleset_config_for(sys::state const& state) {
	capacity_config config{};
	config.enabled = gamerule::age_of_transformation_enabled(state);
	return config;
}

capacity_inputs inputs_for_route(sys::state const& state, dcon::trade_route_id route) {
	capacity_inputs inputs{};
	if(!route || !state.world.trade_route_is_valid(route)) {
		return inputs;
	}

	auto const market_a = state.world.trade_route_get_connected_markets(route, 0);
	auto const market_b = state.world.trade_route_get_connected_markets(route, 1);
	if(!market_a || !market_b || !state.world.market_is_valid(market_a) || !state.world.market_is_valid(market_b)) {
		return inputs;
	}

	inputs.endpoint_capacity = {
		state.world.market_get_max_throughput(market_a),
		state.world.market_get_max_throughput(market_b)};

	double cargo = 0.0;
	state.world.for_each_commodity([&](dcon::commodity_id commodity) {
		auto const volume = state.world.trade_route_get_volume(route, commodity);
		if(std::isfinite(volume)) {
			cargo += std::abs(double(volume));
		}
	});
	inputs.cargo = float(std::min(cargo, double(std::numeric_limits<float>::max())));

	auto const state_a = state.world.market_get_zone_from_local_market(market_a);
	auto const state_b = state.world.market_get_zone_from_local_market(market_b);
	if(state.world.trade_route_get_is_sea_route(route)) {
		inputs.endpoint_transport_availability = {
			state_port_availability(state, state_a),
			state_port_availability(state, state_b)};
	} else if(state.world.trade_route_get_is_land_route(route)) {
		inputs.endpoint_transport_availability = {
			state_land_availability(state, state_a),
			state_land_availability(state, state_b)};
	} else {
		inputs.endpoint_transport_availability = {0.0f, 0.0f};
	}

	return inputs;
}

capacity_result evaluate_route_capacity(sys::state const& state, dcon::trade_route_id route) {
	return evaluate_capacity(ruleset_config_for(state), inputs_for_route(state, route));
}

} // namespace economy::world_trade
