#pragma once

#include "economy_stats.hpp"

// this .cpp and .hpp pair of files contains:
// - employment updates of productive forces
// - consumption updates of productive forces
// - production updates of productive forces
// as all of these things are related to "productive forces", the file is named as "economy_production"

namespace economy {

inline constexpr float secondary_employment_output_bonus = 3.f;
inline constexpr float unqualified_throughput_multiplier = 0.2f;
inline constexpr float artisans_per_employment_unit = 10'000.f;
inline constexpr float construction_units_to_maintenance_units = 0.0001f;
inline constexpr float expansion_trigger = 0.8f;

inline constexpr float rgo_profit_to_wage_bound = 0.8f;
inline constexpr float factory_profit_to_wage_bound = 2.f;

struct ve_inputs_data {
	ve::fp_vector min_available = 0.f;
	ve::fp_vector total_cost = 0.f;
};
struct inputs_data {
	float min_available = 0.f;
	float total_cost = 0.f;
};

template<typename SET>
inputs_data get_inputs_data(sys::state const& state, dcon::market_id markets, SET const& inputs);

void update_factories_production(sys::state& state);
void update_rgo_production(sys::state& state);

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
float factory_throughput_multiplier(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p, dcon::state_instance_id s, float size);
float factory_output_multiplier_no_secondary_workers(sys::state const& state, dcon::factory_id fac, dcon::nation_id n, dcon::province_id p);
float factory_throughput_additional_multiplier(sys::state const& state, dcon::factory_id fac, float mobilization_impact, bool occupied);

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
float factory_type_build_cost(sys::state& state, dcon::nation_id n, dcon::province_id p, dcon::factory_type_id factory_type, bool is_pop_project);

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

float estimate_factory_profit_margin(
	sys::state& state,
	dcon::province_id pid,
	dcon::factory_type_id factory_type
);
float estimate_factory_payback_time(
	sys::state& state,
	dcon::province_id pid,
	dcon::factory_type_id factory_type
);

float factory_output(sys::state& state, dcon::commodity_id c, dcon::province_id id);
float factory_output(sys::state& state, dcon::commodity_id c, dcon::state_instance_id id);
float factory_output(sys::state& state, dcon::commodity_id c, dcon::nation_id id);
float factory_output(sys::state& state, dcon::commodity_id c);

float factory_total_desired_employment_score(sys::state const& state, dcon::factory_id f);
float factory_total_desired_employment(sys::state const& state, dcon::factory_id f);
float factory_total_employment(sys::state const& state, dcon::factory_id f);
float factory_unqualified_employment(sys::state const& state, dcon::factory_id f);
float factory_primary_employment(sys::state const& state, dcon::factory_id f);
float factory_secondary_employment(sys::state const& state, dcon::factory_id f);

struct factory_employment_vector {
	float unqualified;
	float primary;
	float secondary;
};

struct detailed_commodity_set {
	static constexpr uint32_t set_size = 8;

	float commodity_base_amount[set_size] = { 0.0f };
	float commodity_price[set_size] = { 0.0f };
	float commodity_actual_amount[set_size] = { 0.0f };
	float efficient_ratio[set_size] = { 0.0f };
	dcon::commodity_id commodity_type[set_size] = { dcon::commodity_id{} };
};

namespace factory_operation {

struct input_multipliers_explanation {
	float total = 1.f;
	float from_modifiers = 1.f;
	float from_triggered_modifiers = 1.f;
	float from_scale = 1.f;
	float from_competition = 1.f;
	float from_specialisation = 1.f;
};

struct output_multipliers_explanation {
	float total = 1.f;
	float total_ignore_inputs = 1.f;
	float from_modifiers = 1.f;
	float from_efficiency_goods = 1.f;
	float from_secondary_workers = 1.f;
	float from_inputs_lack = 1.f;
};

struct throughput_multipliers_explanation {
	float total = 1.f;
	float base = 1.f;
	float from_modifiers = 1.f;
	float from_scale = 1.f;
};

struct detailed_explanation {
	dcon::factory_type_id base_type = dcon::factory_type_id{ };

	float profit = 0.f;
	float income_from_sales = 0.f;
	float spending_from_primary_inputs = 0.f;
	float spending_from_efficiency_inputs = 0.f;
	float spending_from_wages = 0.f;
	float spending_from_expansion = 0.f;
	float spending_from_maintenance = 0.f;

	dcon::commodity_id output = dcon::commodity_id{};
	float output_price = 0.f;
	float output_amount_per_production_unit = 0.f;
	float output_amount_per_employment_unit = 0.f;
	float output_amount_per_production_unit_ignore_inputs = 0.f;
	float output_amount_per_employment_unit_ignore_inputs = 0.f;
	float output_base_amount = 0.f;
	float output_actual_amount = 0.f;
	float output_actually_sold_ratio = 0.f;

	detailed_commodity_set efficiency_inputs{};
	float required_efficiency_inputs_multiplier = 1.f;
	bool efficiency_inputs_worth_it = false;

	detailed_commodity_set expansion_inputs{};
	float expansion_scale = 0.f;
	float expansion_size = 0.f;

	detailed_commodity_set maintenance_inputs{};
	float maintenance_multiplier = 1.f;

	float employment_units = 0.f;
	float production_units = 0.f;
	factory_employment_vector employment{};
	factory_employment_vector employment_target{};
	factory_employment_vector employment_wages_paid{};
	factory_employment_vector employment_wages_per_person{};
	factory_employment_vector employment_available_ratio{};
	factory_employment_vector expected_profit_gradient{};
	factory_employment_vector employment_expected_change{};

	detailed_commodity_set primary_inputs{};
	input_multipliers_explanation input_multipliers{};
	output_multipliers_explanation output_multipliers{};
	throughput_multipliers_explanation throughput_multipliers{};

	float employment_mobilisation_impact;
};

detailed_explanation explain_everything(sys::state const& state, dcon::factory_id f);

}
// RGO:
struct rgo_workers_breakdown {
	float paid_workers;
	float slaves;
	float total;
};

rgo_workers_breakdown rgo_relevant_population(sys::state& state, dcon::province_id p, dcon::nation_id n);

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

float rgo_wage(sys::state& state, dcon::commodity_id c, dcon::province_id p);
float rgo_efficiency_spending(sys::state& state, dcon::commodity_id c, dcon::province_id p);

commodity_set rgo_calculate_actual_efficiency_inputs(sys::state& state, dcon::nation_id n, dcon::market_id m, dcon::province_id p, dcon::commodity_id c, float mobilization_impact);

// ARTISANS:

bool valid_artisan_good(sys::state&, dcon::nation_id, dcon::commodity_id);

float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p, dcon::commodity_id output);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::state_instance_id s);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c, dcon::nation_id n);
float estimate_artisan_consumption(sys::state& state, dcon::commodity_id c);

float estimate_artisan_gdp_intermediate_consumption(sys::state& state, dcon::province_id p, dcon::commodity_id output);

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


namespace gdp {

struct breakdown {
	float primary;
	float secondary_factory;
	float secondary_artisan;
	float total;
	float total_non_negative;
};

float value_nation(sys::state& state, dcon::nation_id n);
float value_market(sys::state& state, dcon::market_id n);
float value_nation_adjusted(sys::state& state, dcon::nation_id n);

breakdown breakdown_province(sys::state& state, dcon::province_id pid);
}

}
