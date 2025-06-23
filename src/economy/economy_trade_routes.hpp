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
inline constexpr float min_trade_expansion_multiplier = 0.0000001f;
inline constexpr float trade_route_min_shift = 0.05f;
// if exporter has demand satisfaction lower than this value,
// the trade will effectively stop
inline constexpr float trade_demand_satisfaction_cutoff = 0.7f;

void update_trade_routes_volume(sys::state& state);
void update_trade_routes_consumption(sys::state& state);

struct trade_route_volume_change_reasons {
	std::array<float, 2> export_price;
	std::array<float, 2> import_price;
	std::array<float, 2> export_profit;

	float max_expansion;
	float max_shrinking;

	float profit;
	float divisor;
	float profit_score;
	float current_volume;
	float base_change;
	float actually_sold_in_origin;
	float expansion_multiplier;
	float decay;
	float final_change;

	bool trade_blocked;
	bool commodity_is_not_tradable;
	bool commodity_is_not_discovered;
};

float trade_route_labour_demand(sys::state& state, dcon::trade_route_id trade_route, dcon::province_fat_id A_capital, dcon::province_fat_id B_capital);
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

tariff_data explain_trade_route(sys::state& state, dcon::trade_route_id trade_route);
trade_and_tariff explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, tariff_data& additional_data, dcon::commodity_id cid);
trade_and_tariff explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, dcon::commodity_id cid);

struct trade_breakdown_item {
	dcon::nation_id trade_partner;
	dcon::commodity_id commodity;
	float traded_amount;
	float tariff;
};
std::vector<trade_breakdown_item> explain_national_tariff(sys::state& state, dcon::nation_id n, bool import_flag, bool export_flag);


}
