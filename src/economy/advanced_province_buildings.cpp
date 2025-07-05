#include "advanced_province_buildings.hpp"
#include "price.hpp"
#include "demographics.hpp"
#include "economy_stats.hpp"

namespace services {

void initialize_size_of_dcon_arrays(sys::state& state) {
	state.world.province_resize_service_supply_public(list::total);
	state.world.province_resize_service_supply_private(list::total);
	state.world.province_resize_service_demand_allowed_public_supply(list::total);
	state.world.province_resize_service_demand_forbidden_public_supply(list::total);
	state.world.province_resize_service_price(list::total);
	state.world.province_resize_service_sold(list::total);
	state.world.province_resize_service_satisfaction(list::total);
	state.world.province_resize_service_satisfaction_for_free(list::total);
}

void update_price(sys::state& state) {
	for(int32_t i = 0; i < list::total; i++) {
		state.world.execute_serial_over_province([&](auto pids) {
			auto supply = state.world.province_get_service_supply_private(pids, i)
				+ state.world.province_get_service_supply_public(pids, i);
			auto demand = state.world.province_get_service_demand_allowed_public_supply(pids, i)
				+ state.world.province_get_service_demand_forbidden_public_supply(pids, i);
			auto price = state.world.province_get_service_price(pids, i);
			auto change = economy::price_properties::change(price, supply, demand);
			auto new_price = ve::min(ve::max(price + change, economy::price_properties::service::min), economy::price_properties::service::max);
			state.world.province_set_service_price(pids, i, new_price);
		});
	}
}
void reset_supply(sys::state& state) {
	for(int32_t i = 0; i < list::total; i++) {
		state.world.execute_serial_over_province([&](auto pids) {
			state.world.province_set_service_supply_private(pids, i, 0.f);
			state.world.province_set_service_supply_public(pids, i, 0.f);
		});
	}
}
void reset_demand(sys::state& state) {
	for(int32_t i = 0; i < list::total; i++) {
		state.world.execute_serial_over_province([&](auto pids) {
			state.world.province_set_service_demand_allowed_public_supply(pids, i, 0.f);
			state.world.province_set_service_demand_forbidden_public_supply(pids, i, 0.f);
		});
	}
}
void reset_price(sys::state& state) {
	for(int32_t i = 0; i < list::total; i++) {
		state.world.execute_serial_over_province([&](auto pids) {
			state.world.province_set_service_price(pids, i, economy::price_properties::service::min);
		});
	}
}

void match_supply_and_demand(sys::state& state) {
	for(int32_t i = 0; i < list::total; i++) {
		//state.world.execute_serial_over_province([&](auto pids) {
		state.world.for_each_province([&](auto pids) {
			auto demand_can_be_free = state.world.province_get_service_demand_allowed_public_supply(pids, i);
			auto demand_only_paid = state.world.province_get_service_demand_forbidden_public_supply(pids, i);

			auto supply_free = state.world.province_get_service_supply_public(pids, i);
			auto supply_paid = state.world.province_get_service_supply_private(pids, i);

			auto matched_free_demand = ve::min(demand_can_be_free, supply_free);
			auto demand_paid = demand_can_be_free - matched_free_demand + demand_only_paid;
			auto matched_paid_demand = ve::min(demand_paid, supply_paid);

			// free demand is always satisfied by 100% because remaining demand is always moved to paid demand
			// but we are interested in ratio of satisfied free demand to total demand which is eligible to be free

			auto free_ratio = ve::select(demand_can_be_free == 0.f, 0.f, matched_free_demand / demand_can_be_free);

			// for paid demand, business as usual

			auto satisfaction_paid = ve::select(demand_paid == 0.f, 0.f, ve::min(1.f, supply_paid / demand_paid));
			auto supply_sold_paid = ve::select(supply_paid == 0.f, 0.f, ve::min(1.f, demand_paid / supply_paid));

			state.world.province_set_service_satisfaction_for_free(pids, i, free_ratio);
			state.world.province_set_service_satisfaction(pids, i, satisfaction_paid);
			state.world.province_set_service_sold(pids, i, supply_sold_paid);
		});
	}
}

}

namespace advanced_province_buildings {

const advanced_building_definition definitions[services::list::total] = {
	// school
	{
		.throughput_labour_type = economy::labor::high_education,
		.output = services::list::education,
		.output_amount = 30.f,
	}
};

void initialize_size_of_dcon_arrays(sys::state& state) {
	state.world.province_resize_advanced_province_building_national_size(list::total);
	state.world.province_resize_advanced_province_building_private_size(list::total);
}

void update_consumption(sys::state& state) {
	for(int32_t i = 0; i < list::total; i++) {
		auto& def = definitions[i];
		state.world.execute_serial_over_province([&](auto pids) {
			auto private_size = state.world.province_get_advanced_province_building_private_size(pids, i);
			auto national_size = state.world.province_get_advanced_province_building_national_size(pids, i);
			auto total = private_size + national_size;

			auto current_demand = state.world.province_get_labor_demand(pids, def.throughput_labour_type);
			state.world.province_set_labor_demand(pids, def.throughput_labour_type, current_demand + total);
		});
	}
}

void update_size(sys::state& state) {
	// schools
	{
		auto bid = list::schools_and_universities;
		auto& def = definitions[bid];

		//state.world.execute_serial_over_province([&](auto pids) {
		state.world.for_each_province([&](auto pids) {
			auto owner = state.world.province_get_nation_from_province_ownership(pids);
			auto tmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency) + 1.0f;
			auto nmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency_modifier) + 1.0f;

			auto cost_of_input = state.world.province_get_labor_price(pids, def.throughput_labour_type);
			auto cost_of_output = state.world.province_get_service_price(pids, def.output) * tmod * nmod * def.output_amount;

			auto current_private_size = state.world.province_get_advanced_province_building_private_size(pids, bid);
			auto margin = (cost_of_output - cost_of_input) / cost_of_input;
			auto new_private_size = current_private_size + ve::min(margin, 100.f) + ve::min(ve::max(margin, -0.01f), 0.01f) * current_private_size;
			auto max_size = state.world.province_get_demographics(pids, demographics::total) * state.world.province_get_labor_price(pids, economy::labor::no_education) / cost_of_input;
			new_private_size = ve::min(max_size, new_private_size);
			state.world.province_set_advanced_province_building_private_size(pids, bid, ve::max(0.f, new_private_size));


			auto national_budget = state.world.nation_get_stockpiles(owner, economy::money);
			auto education_priority = ve::to_float(state.world.nation_get_education_spending(owner)) / 100.f;
			auto education_budget = national_budget * education_priority;
			auto total_population = state.world.nation_get_demographics(owner, demographics::primary_or_accepted);
			auto local_population = state.world.province_get_demographics(pids, demographics::primary_or_accepted);
			auto weight = ve::select(total_population == 0.f, 0.f, local_population / total_population);
			auto local_education_budget = weight * education_budget;
			state.world.province_set_advanced_province_building_national_size(pids, bid, ve::max(0.f, local_education_budget / cost_of_input));
		});
	}
}

void update_production(sys::state& state) {
	// schools
	{
		auto bid = list::schools_and_universities;
		auto& def = definitions[bid];

		state.world.execute_serial_over_province([&](auto pids) {
			auto owner = state.world.province_get_nation_from_province_ownership(pids);
			auto tmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency) + 1.0f;
			auto nmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency_modifier) + 1.0f;

			auto input_satisfaction = state.world.province_get_labor_demand_satisfaction(pids, def.throughput_labour_type);
			auto output = tmod * nmod * input_satisfaction * def.output_amount;

			auto current_private_size = state.world.province_get_advanced_province_building_private_size(pids, bid);
			auto current_private_supply = state.world.province_get_service_supply_private(pids, def.output);
			state.world.province_set_service_supply_private(pids, def.output, current_private_supply + current_private_size * output);

			auto current_public_size = state.world.province_get_advanced_province_building_national_size(pids, bid);
			auto current_public_supply = state.world.province_get_service_supply_public(pids, def.output);
			state.world.province_set_service_supply_public(pids, def.output, current_public_supply + current_public_size * output);
		});

		// TODO:
		// update gdp of local markets with education
	}
}

}
