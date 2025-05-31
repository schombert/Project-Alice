#pragma once

#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace economy {

namespace labor {
inline constexpr int32_t no_education = 0; // labourer, farmers and slaves
inline constexpr int32_t basic_education = 1; // craftsmen
inline constexpr int32_t high_education = 2; // clerks, clergy and bureaucrats
inline constexpr int32_t guild_education = 3; // artisans
inline constexpr int32_t high_education_and_accepted = 4; // clerks, clergy and bureaucrats of accepted culture
inline constexpr int32_t total = 5;
}

namespace pop_labor {
inline constexpr int32_t rgo_worker_no_education = 0;
inline constexpr int32_t primary_no_education = 1;
inline constexpr int32_t high_education_accepted_no_education = 2;
inline constexpr int32_t high_education_not_accepted_no_education = 3;

inline constexpr int32_t primary_basic_education = 4;
inline constexpr int32_t high_education_accepted_basic_education = 5;
inline constexpr int32_t high_education_not_accepted_basic_education = 6;

inline constexpr int32_t high_education_accepted_high_education = 7;
inline constexpr int32_t high_education_not_accepted_high_education = 8;

inline constexpr int32_t high_education_accepted_high_education_accepted = 9;

inline constexpr int32_t total = 10;
}

enum class economy_reason {
	pop, factory, rgo, artisan, construction, nation, stockpile, overseas_penalty, trade
};

float inline market_speculation_budget(
	sys::state const& state,
	dcon::market_id m,
	dcon::commodity_id c
);
template<typename M>
ve::fp_vector market_speculation_budget(
	sys::state const& state,
	M m,
	dcon::commodity_id c
);
ve::fp_vector ve_market_speculation_budget(
	sys::state const& state,
	ve::contiguous_tags<dcon::market_id> m,
	dcon::commodity_id c
);
ve::fp_vector ve_market_speculation_budget(
	sys::state const& state,
	ve::partial_contiguous_tags<dcon::market_id> m,
	dcon::commodity_id c
);
ve::fp_vector ve_market_speculation_budget(
	sys::state const& state,
	ve::tagged_vector<dcon::market_id> m,
	dcon::commodity_id c
);
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

std::array<trade_volume_data_detailed, 2> export_import_volume_detailed(
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

float average_capitalists_luxury_cost(
	sys::state& state,
	dcon::nation_id s
);

int32_t state_factory_count(sys::state& state, dcon::state_instance_id sid, dcon::nation_id n);
float state_factory_level(sys::state& state, dcon::state_instance_id sid, dcon::nation_id n);
int32_t province_factory_count(sys::state& state, dcon::province_id sid);
float province_factory_level(sys::state& state, dcon::province_id sid);

float get_factory_level(sys::state& state, dcon::factory_id f);

// checks existence of factory in province (could be under construction)
bool has_factory(sys::state const&, dcon::province_id);
// checks existence of factory in state (could be under construction)
bool has_factory(sys::state const&, dcon::state_instance_id);
// checks existence of factory in province (only constructed factories count)
bool has_constructed_factory(sys::state& state, dcon::state_instance_id si, dcon::factory_type_id ft);
// checks existence of factory of given type in state (could be under construction)
bool has_factory(sys::state& state, dcon::state_instance_id s, dcon::factory_type_id ft);
}
