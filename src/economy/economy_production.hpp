#pragma once

// this .cpp and .hpp pair of files contains:
// - employment updates of productive forces
// - consumption updates of productive forces
// - production updates of productive forces
// as all of these things are related to "productive forces", the file is named as "economy_production"

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

inline constexpr float secondary_employment_output_bonus = 3.f;
inline constexpr float unqualified_throughput_multiplier = 0.70f;

inline constexpr float artisans_per_employment_unit = 10'000.f;

inline constexpr float construction_units_to_maintenance_units = 0.01f;

float expansion_trigger = 0.8f;

struct ve_inputs_data {
	ve::fp_vector min_available = 0.f;
	ve::fp_vector total_cost = 0.f;
};
struct inputs_data {
	float min_available = 0.f;
	float total_cost = 0.f;
};

template<typename SET>
inputs_data get_inputs_data(sys::state& state, dcon::market_id markets, SET const& inputs);

void update_single_factory_production(
	sys::state& state,
	dcon::factory_id f,
	dcon::market_id m,
	dcon::nation_id n
);

void update_province_rgo_production(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n
);

float base_artisan_profit(
	sys::state& state,
	dcon::market_id market,
	dcon::commodity_id c
);

float priority_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n);
float nation_factory_input_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n);
float nation_factory_output_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n);

void update_employment(sys::state& state);

void update_artisan_production(sys::state& state);
void update_production_consumption(sys::state& state);

float factory_input_multiplier(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s);
float factory_throughput_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s);
float factory_output_multiplier_no_secondary_workers(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p);

struct profit_explanation {
	float inputs;
	float wages;
	float maintenance;
	float expansion;
	float output;

	float profit;
};

profit_explanation explain_last_factory_profit(sys::state& state, dcon::factory_id f);

float factory_type_output_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::market_id m,
	dcon::factory_type_id factory_type
);
float factory_type_input_cost(
	sys::state& state,
	dcon::nation_id n,
	dcon::market_id m,
	dcon::factory_type_id factory_type
);
float factory_type_build_cost(sys::state& state, dcon::nation_id n, dcon::market_id m, dcon::factory_type_id factory_type);

// SHORTAGES, SURPLUS

bool commodity_surplus(sys::state& state, dcon::commodity_id c, dcon::state_instance_id);
bool commodity_shortage(sys::state& state, dcon::commodity_id c, dcon::state_instance_id);

bool labor_surplus(sys::state& state, int32_t c, dcon::province_id);
bool labor_shortage(sys::state& state, int32_t c, dcon::province_id);

// FACTORIES

float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p);
float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::state_instance_id s);
float estimate_factory_consumption(sys::state& state, dcon::commodity_id c, dcon::nation_id n);
float estimate_factory_consumption(sys::state& state, dcon::commodity_id c);

float factory_output(sys::state& state, dcon::commodity_id c, dcon::province_id id);
float factory_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id);
float factory_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id);
float factory_output(sys::state& state, dcon::commodity_id c);

// RGO:

float rgo_income(sys::state& state, dcon::province_id id);
float rgo_income(sys::state& state, dcon::commodity_id c, dcon::province_id id);

float rgo_output(sys::state& state, dcon::commodity_id c, dcon::province_id id);
float rgo_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id);
float rgo_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id);
float rgo_output(sys::state& state, dcon::commodity_id c);

float rgo_potential_output(sys::state& state, dcon::commodity_id c, dcon::province_id id);

float rgo_max_employment(sys::state& state, dcon::commodity_id c, dcon::province_id p);
float rgo_max_employment(sys::state& state, dcon::province_id p);

float rgo_employment(sys::state& state, dcon::commodity_id c, dcon::province_id p);
float rgo_employment(sys::state& state, dcon::province_id p);

// ARTISANS:

float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p, dcon::commodity_id output);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::state_instance_id s);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::nation_id n);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c);

float artisan_output(sys::state& state, dcon::commodity_id c, dcon::province_id id);
float artisan_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id);
float artisan_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id);
float artisan_output(sys::state& state, dcon::commodity_id c);

float artisan_employment_target(sys::state& state, dcon::commodity_id c, dcon::province_id id);
float artisan_employment_target(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id);
float artisan_employment_target(sys::state& state, dcon::commodity_id c, dcon::market_id id);

// BREAKDOWN

struct breakdown_commodity {
	float rgo;
	float guild;
	float factories;
};

breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c, dcon::province_id id);
breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id);
breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id);
breakdown_commodity explain_output(sys::state& state, dcon::commodity_id c);

}
