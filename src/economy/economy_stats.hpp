#pragma once

#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace economy {

enum class economy_reason {
	pop, factory, rgo, artisan, construction, nation, stockpile, overseas_penalty, trade
};

ve::fp_vector ve_price(
	sys::state const& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c
);
ve::fp_vector ve_price(
	sys::state const& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c
);
ve::fp_vector ve_price(
	sys::state const& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id c
);

void register_demand(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
);

void register_demand(
	sys::state& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
);
void register_demand(
	sys::state& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
);
void register_demand(
	sys::state& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
);

void register_intermediate_demand(
	sys::state& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c,
	ve::fp_vector amount,
	economy_reason reason
);
void register_intermediate_demand(
	sys::state& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id c,
	ve::fp_vector amount,
	economy_reason reason
);
void register_intermediate_demand(
	sys::state& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id c,
	ve::fp_vector amount,
	economy_reason reason
);

void register_intermediate_demand(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c,
	float amount,
	economy_reason reason
);

void register_domestic_supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
);
void register_domestic_supply(
	sys::state& state,
	ve::contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
);
void register_domestic_supply(
	sys::state& state,
	ve::partial_contiguous_tags<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
);
void register_domestic_supply(
	sys::state& state,
	ve::tagged_vector<dcon::market_id> s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
);
void register_foreign_supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id commodity_type,
	float amount,
	economy_reason reason
);

template<typename T>
void ve_register_domestic_supply(
	sys::state& state,
	T s,
	dcon::commodity_id commodity_type,
	ve::fp_vector amount,
	economy_reason reason
);

float trade_influx(sys::state& state,
	dcon::market_id m,
	dcon::commodity_id c
);
float trade_outflux(sys::state& state,
	dcon::market_id m,
	dcon::commodity_id c
);

float trade_supply(sys::state& state,
	dcon::market_id m,
	dcon::commodity_id c
);

float trade_demand(sys::state& state,
	dcon::market_id m,
	dcon::commodity_id c
);

float stockpile(sys::state& state, dcon::nation_id n, dcon::commodity_id c);

float trade_supply(sys::state& state,
	dcon::nation_id n,
	dcon::commodity_id c
);
float trade_demand(sys::state& state,
	dcon::nation_id n,
	dcon::commodity_id c
);

float price(
	sys::state const& state,
	dcon::state_instance_id s,
	dcon::commodity_id c
);

float price(
	sys::state const& state,
	dcon::nation_id s,
	dcon::commodity_id c
);
float price(
	sys::state& state,
	dcon::commodity_id c
);
float median_price(
	sys::state& state,
	dcon::commodity_id c
);
float median_price(sys::state& state, dcon::nation_id s, dcon::commodity_id c);

float price(
	sys::state const& state,
	dcon::market_id s,
	dcon::commodity_id c
);

float supply(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
);
float supply(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);
float supply(
	sys::state& state,
	dcon::commodity_id c
);

float demand(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
);
float demand(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);
float demand(
	sys::state& state,
	dcon::commodity_id c
);

float consumption(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
);
float consumption(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);
float consumption(
	sys::state& state,
	dcon::commodity_id c
);

float demand_satisfaction(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
);
float demand_satisfaction(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);
float demand_satisfaction(
	sys::state& state,
	dcon::commodity_id c
);

float market_pool(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
);
float market_pool(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);
float market_pool(
	sys::state& state,
	dcon::commodity_id c
);

float export_volume(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
);
float export_volume(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);

float domestic_trade_volume(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);


struct nation_enriched_float {
	float value;
	dcon::nation_id nation;
};

struct trade_volume_data_detailed {
	float volume;
	dcon::commodity_id commodity;
	nation_enriched_float targets[5];
};

trade_volume_data_detailed export_volume_detailed(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);

float import_volume(
	sys::state& state,
	dcon::market_id s,
	dcon::commodity_id c
);
float import_volume(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);
trade_volume_data_detailed import_volume_detailed(
	sys::state& state,
	dcon::nation_id s,
	dcon::commodity_id c
);

float average_capitalists_luxury_cost(
	sys::state& state,
	dcon::nation_id s
);

}
