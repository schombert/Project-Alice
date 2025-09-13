#pragma once

#include "dcon_generated.hpp"
#include "economy_stats.hpp"

namespace sys {
struct state;
}

namespace economy {

inline constexpr float merchant_cut_foreign = 0.05f;
inline constexpr float merchant_cut_domestic = 0.001f;
inline constexpr float effect_of_transportation_scale = 0.0005f;
inline constexpr float trade_distance_covered_by_pair_of_workers_per_unit_of_good = 100.f;
inline constexpr float invalid_trade_route_distance = 999999.f;
inline constexpr float trade_loss_per_distance_unit = 0.0001f;
inline constexpr float trade_effect_of_scale_lower_bound = 0.1f;
inline constexpr float trade_base_decay = 0.05f;
inline constexpr float min_trade_expansion_multiplier = 0.05f;
inline constexpr float trade_route_min_shift = 0.001f;
// if exporter has demand satisfaction lower than this value,
// the trade will effectively stop
inline constexpr float trade_demand_satisfaction_cutoff = 0.7f;


template<typename TRADE_ROUTE>
auto trade_route_effect_of_scale(sys::state& state, TRADE_ROUTE trade_route) {
	using MARKET = convert_value_type<TRADE_ROUTE, dcon::trade_route_id, dcon::market_id>;
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;
	MARKET A = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 0);
	}, trade_route);
	MARKET B = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 1);
	}, trade_route);

	VALUE cargo = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		VALUE volume = state.world.trade_route_get_volume(trade_route, cid);
		MARKET origin = ve::select(volume > 0.f, A, B);
		cargo = cargo + adaptive_ve::abs(volume) * state.world.market_get_direct_demand_satisfaction(origin, cid);;
	});
	return adaptive_ve::max<VALUE>(
		trade_effect_of_scale_lower_bound,
		1.f - cargo * effect_of_transportation_scale
	);
}

//trade_and_tariff<dcon::trade_route_id> explain_trade_route_commodity(
//	sys::state& state,
//	dcon::trade_route_id trade_route,
//	tariff_data<dcon::trade_route_id>& additional_data,
//	dcon::commodity_id cid
//);
trade_and_tariff<ve::contiguous_tags<dcon::trade_route_id>> explain_trade_route_commodity(
	sys::state& state,
	ve::contiguous_tags<dcon::trade_route_id> trade_route,
	tariff_data<ve::contiguous_tags<dcon::trade_route_id>>& additional_data,
	dcon::commodity_id cid
);
trade_and_tariff<ve::partial_contiguous_tags<dcon::trade_route_id>> explain_trade_route_commodity(
	sys::state& state,
	ve::partial_contiguous_tags<dcon::trade_route_id> trade_route,
	tariff_data<ve::partial_contiguous_tags<dcon::trade_route_id>>& additional_data,
	dcon::commodity_id cid
);

void update_trade_routes_volume(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::market_id>& export_tariff_buffer,
	ve::vectorizable_buffer<float, dcon::market_id>& import_tariff_buffer,
	ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>& coastal_capital_buffer,
	ve::vectorizable_buffer<float, dcon::state_instance_id>& state_port_is_occupied
);
void update_trade_routes_consumption(sys::state& state);

struct trade_route_volume_change_reasons {
	std::array<float, 2> export_price{ 0.f, 0.f };
	std::array<float, 2> import_price{ 0.f, 0.f };
	std::array<float, 2> export_profit{ 0.f, 0.f };

	float max_expansion = 0.f;
	float max_shrinking = 0.f;

	float profit = 0.f;
	float divisor = 0.f;
	float profit_score = 0.f;
	float current_volume = 0.f;
	float base_change = 0.f;
	float actually_sold_in_origin = 0.f;
	float expansion_multiplier = 0.f;
	float decay = 0.f;
	float final_change = 0.f;

	bool trade_blocked = 0.f;
	bool commodity_is_not_tradable = 0.f;
	bool commodity_is_not_discovered = 0.f;
};

float trade_route_labour_demand(sys::state& state, dcon::trade_route_id trade_route, dcon::province_id A_capital, dcon::province_id B_capital);
float transportation_between_markets_labor_demand(sys::state& state, dcon::market_id market);
float transportation_inside_market_labor_demand(sys::state& state, dcon::market_id market, dcon::province_id capital);

void make_trade_center_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::market_id market
);

void make_trade_volume_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::trade_route_id route,
	dcon::commodity_id cid,
	dcon::market_id point_of_view
);

trade_route_volume_change_reasons predict_trade_route_volume_change(
	sys::state& state, dcon::trade_route_id route, dcon::commodity_id cid
);

struct embargo_explanation {
	bool combined = false;
	bool war = false;
	bool origin_embargo = false;
	bool target_embargo = false;
	bool origin_join_embargo = false;
	bool target_join_embargo = false;
};

embargo_explanation embargo_exists(
	sys::state& state, dcon::nation_id n_A, dcon::nation_id n_B
);
template <typename TRADE_ROUTE>
auto explain_trade_route(
	sys::state& state,
	TRADE_ROUTE trade_route,
	ve::vectorizable_buffer<float, dcon::market_id>& export_tariff,
	ve::vectorizable_buffer<float, dcon::market_id>& import_tariff
) {
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;

	auto m0 = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 0);
	}, trade_route);
	auto m1 = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 1);
	}, trade_route);

	auto s0 = state.world.market_get_zone_from_local_market(m0);
	auto s1 = state.world.market_get_zone_from_local_market(m1);
	auto n0 = state.world.state_instance_get_nation_from_state_ownership(s0);
	auto n1 = state.world.state_instance_get_nation_from_state_ownership(s1);

	auto capital_0 = state.world.state_instance_get_capital(s0);
	auto capital_1 = state.world.state_instance_get_capital(s1);

	auto controller_capital_0 = state.world.province_get_nation_from_province_control(capital_0);
	auto controller_capital_1 = state.world.province_get_nation_from_province_control(capital_1);

	auto market_leader_0 = nations::get_market_leader(state, n0);
	auto market_leader_1 = nations::get_market_leader(state, n1);

	auto applies_tariffs_0 = ve::apply([&](auto n_a, auto n_b) {
		auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
		if(source_tariffs_rel) {
			auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
			// Enddt empty signalises revoken agreement
			// Enddt > cur_date signalises that the agreement can't be broken
			if(enddt) {
				return false;
			}
		}
		return true;
	}, market_leader_0, controller_capital_1);

	auto applies_tariffs_1 = ve::apply([&](auto n_a, auto n_b) {
		auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);
		if(target_tariffs_rel) {
			auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
			if(enddt) {
				return false;
			}
		}
		return true;
	}, market_leader_1, controller_capital_0);

	auto is_open_0_to_1 = market_leader_0 == controller_capital_1 || !applies_tariffs_0 || n0 == n1;
	auto is_open_1_to_0 = market_leader_1 == controller_capital_0 || !applies_tariffs_1 || n0 == n1;

	auto distance = state.world.trade_route_get_distance(trade_route);
	auto trade_good_loss_mult = adaptive_ve::max<VALUE>(0.f, 1.f - trade_loss_per_distance_unit * distance);
	auto scale = trade_route_effect_of_scale(state, trade_route);
	auto base_cost_per_unit = distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good * (
		state.world.province_get_labor_price(capital_0, labor::no_education)
		+ state.world.province_get_labor_price(capital_1, labor::no_education)
	);

	auto export_tariff_0 = ve::select(is_open_0_to_1, 0.f, export_tariff.get(m0));
	auto import_tariff_0 = ve::select(is_open_0_to_1, 0.f, import_tariff.get(m1));
	auto export_tariff_1 = ve::select(is_open_1_to_0, 0.f, export_tariff.get(m0));
	auto import_tariff_1 = ve::select(is_open_1_to_0, 0.f, import_tariff.get(m1));

	tariff_data<TRADE_ROUTE> result{
		.applies_tariff = {is_open_0_to_1, is_open_1_to_0},
		.export_tariff = {export_tariff_0, export_tariff_1 },
		.import_tariff = {import_tariff_0, import_tariff_1 },
		.markets = { m0, m1 },
		.distance = distance,
		.loss = trade_good_loss_mult,
		.base_distance_cost = base_cost_per_unit,
		.workers_satisfaction = adaptive_ve::min(
			state.world.province_get_labor_demand_satisfaction(capital_0, labor::no_education),
			state.world.province_get_labor_demand_satisfaction(capital_1, labor::no_education)
		),
		.effect_of_scale = scale,
		.distance_cost_scaled = scale * base_cost_per_unit
	};

	return result;
}
template <typename TRADE_ROUTE>
trade_and_tariff<TRADE_ROUTE> explain_trade_route_commodity(sys::state& state, TRADE_ROUTE trade_route, tariff_data<TRADE_ROUTE>& additional_data, dcon::commodity_id cid);

trade_and_tariff<dcon::trade_route_id> explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, dcon::commodity_id cid);

struct trade_breakdown_item {
	dcon::nation_id trade_partner;
	dcon::commodity_id commodity;
	float traded_amount;
	float tariff;
};
std::vector<trade_breakdown_item> explain_national_tariff(sys::state& state, dcon::nation_id n, bool import_flag, bool export_flag);


}
