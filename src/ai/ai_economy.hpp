#pragma once
#include "dcon_generated_ids.hpp"
#include "dcon_generated.hpp"
#include <stackedcalculation.hpp>


namespace sys {
struct state;
}

namespace ai {

bool province_has_available_workers(sys::state& state, dcon::province_id p);
bool province_has_workers(sys::state& state, dcon::province_id p);

void update_budget(sys::state& state, bool presim = false);

using factory_evaluation_stack = decltype(
	std::declval<sys::stacked_calculation<>>()
		.multiply(1.f, "profitability")
		.divide(1.f, "payback_time")
		.multiply(1.f, "output_is_in_demand")
		.divide(1.f, "private_investors_avoid_high_taxes")
		.multiply(1.f, "state_seeks_to_maximize_taxes")
		.multiply(10.f, "factory_exploits_local_potentials")
		.multiply(1.f, "factory_output_consumed_by_other_factory")
		.multiply(1.f, "factory_consumes_other_factory_output")
		.multiply(1.f, "factory_consumes_local_potential_resource")
		.multiply(1.f, "factory_consumes_local_rgo_resource")
);

// Returns score AI (nation or private investors) places to the factory type construction in the given market with explanation localisation keys. Filters are redundant for now
factory_evaluation_stack evaluate_factory_type(sys::state& state,
	dcon::nation_id nid,
	dcon::market_id mid,
	dcon::province_id pid, dcon::factory_type_id type,
	bool pop_project,
	float filter_profitability,
	float filter_output_demand_satisfaction,
	float filter_payback_time,
	float effective_profit);

void get_craved_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id, std::vector<dcon::factory_type_id>& desired_types, bool pop_project);
void get_desired_factory_types(sys::state& state, dcon::nation_id nid, dcon::market_id mid, dcon::province_id, std::vector<dcon::factory_type_id>& desired_types, bool pop_project);
void update_ai_econ_construction(sys::state& state);
void update_factory_types_priority(sys::state& state);
}
