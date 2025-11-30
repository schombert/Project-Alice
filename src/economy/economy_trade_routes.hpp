#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"
#include "economy_stats.hpp"
#include "adaptive_ve.hpp"
#include "economy_templates_pure.hpp"

namespace sys {
struct state;
}

namespace economy {

void update_trade_routes_volume(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::market_id>& export_tariff_buffer,
	ve::vectorizable_buffer<float, dcon::market_id>& import_tariff_buffer,
	ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>& coastal_capital_buffer,
	ve::vectorizable_buffer<float, dcon::state_instance_id>& state_port_is_occupied,
	ve::vectorizable_buffer<float, dcon::market_id>& available_port_capacity,
	ve::vectorizable_buffer<float, dcon::market_id>& price_port_capacity
);
void update_trade_routes_consumption(sys::state& state);

struct trade_route_volume_change_reasons {
	std::array<float, 2> export_price{ 0.f, 0.f };
	std::array<float, 2> import_price{ 0.f, 0.f };
	std::array<float, 2> export_profit{ 0.f, 0.f };

	//float max_expansion = 0.f;
	//float max_shrinking = 0.f;

	float profit = 0.f;
	float profit_score = 0.f;
	float current_volume = 0.f;
	float base_change = 0.f;
	float expected_to_buy_in_origin_ratio = 0.f;
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

bool is_trade_route_relevant(sys::state& state, dcon::trade_route_id trade_route, dcon::nation_id n);


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

struct trade_breakdown_item {
	dcon::nation_id trade_partner;
	dcon::commodity_id commodity;
	float traded_amount;
	float tariff;
};
std::vector<trade_breakdown_item> explain_national_tariff(sys::state& state, dcon::nation_id n, bool import_flag, bool export_flag);


trade_and_tariff<dcon::trade_route_id> explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, dcon::commodity_id cid);
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

void fill_trade_buffers(
	sys::state& state,

	ve::vectorizable_buffer<float, dcon::market_id>& export_tariff_buffer,
	ve::vectorizable_buffer<float, dcon::market_id>& import_tariff_buffer,

	ve::vectorizable_buffer<float, dcon::trade_route_id>& buffer_payment_0,
	ve::vectorizable_buffer<float, dcon::trade_route_id>& buffer_payment_1,

	ve::vectorizable_buffer<float, dcon::trade_route_id>& buffer_tariff_0,
	ve::vectorizable_buffer<float, dcon::trade_route_id>& buffer_tariff_1,

	std::vector<ve::vectorizable_buffer<float, dcon::trade_route_id>>& per_commodity_export_0,
	std::vector<ve::vectorizable_buffer<float, dcon::trade_route_id>>& per_commodity_export_1,
	std::vector<ve::vectorizable_buffer<float, dcon::trade_route_id>>& per_commodity_import_0,
	std::vector<ve::vectorizable_buffer<float, dcon::trade_route_id>>& per_commodity_import_1
);

}
