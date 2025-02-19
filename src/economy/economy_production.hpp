#pragma once

// this .cpp and .hpp pair of files contains:
// - employment updates of productive forces
// - consumption updates of productive forces
// - production updates of productive forces
// as all of these things are related to "productive forces", the file is named as "economy_production"

// short docs:

// required data:
// - max_employment
// - employment_target_%labour_type%
// - expansion costs
// - output
// - inputs
// additional data which reduces amount of calculations
// - input cost per productive unit
// - output cost per productive unit

// employment:
// there are several modes of employment
// - guilds: only artisans are allowed to work there
// - private: everyone is allowed to work there
// - public: depends on the exact type of production building
// - subsistence: employs otherwise unemployed pops to set the lower bar of needs satisfaction

namespace economy {

inline constexpr float secondary_employment_output_bonus = 3.f;
inline constexpr float unqualified_throughput_multiplier = 0.70f;

inline constexpr float artisans_per_employment_unit = 10'000.f;

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

void update_employment(sys::state& state);

void update_artisan_production(sys::state& state);
void update_production_consumption(sys::state& state);

float get_factory_level(sys::state& state, dcon::factory_id f);

float factory_input_multiplier(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s);
float factory_throughput_multiplier(sys::state const& state, dcon::factory_type_id fac_type, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s);
float factory_output_multiplier_no_secondary_workers(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p);

float rgo_desired_worker_norm_profit(
	sys::state& state,
	dcon::province_id p,
	dcon::market_id m,
	dcon::nation_id n,
	float total_relevant_population
);

struct profit_explanation {
	float inputs;
	float wages;
	float output;

	float profit;
};

profit_explanation explain_last_factory_profit(sys::state& state, dcon::factory_id f);
}
