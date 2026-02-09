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
sys::StackedCalculationWithExplanations evaluate_factory_type(sys::state& state,
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
