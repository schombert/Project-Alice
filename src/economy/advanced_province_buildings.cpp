#include "advanced_province_buildings.hpp"
#include "price.hpp"
#include "demographics.hpp"
#include "economy_stats.hpp"
#include "constants_dcon.hpp"
#include "province_templates.hpp"
#include "money.hpp"

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
			// public demand doesn't matter: it doesn't have any money behind it
			// we are interested only in balance on the "market"
			// which could be indirectly influenced by public supply,
			// but at this point we are only interested in probability to sell our services in current conditions

			auto supply = state.world.province_get_service_supply_private(pids, i);
			auto demand = state.world.province_get_service_demand_forbidden_public_supply(pids, i);
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
			auto demand_public = state.world.province_get_service_demand_allowed_public_supply(pids, i);
			auto demand_private = state.world.province_get_service_demand_forbidden_public_supply(pids, i);

			auto supply_public = state.world.province_get_service_supply_public(pids, i);
			auto supply_private = state.world.province_get_service_supply_private(pids, i);

			auto matched_free_demand = ve::min(demand_public, supply_public);
			auto matched_paid_demand = ve::min(demand_private, supply_private);

			// for public service it doesn't matter how much of produced services were actually consumed
			// the only interesting things is how much of demand on public service was satisfied

			auto free_ratio = ve::select(demand_public == 0.f, 1.f, matched_free_demand / demand_public);

			// for paid demand, business as usual

			auto satisfaction_paid = ve::select(demand_private == 0.f, 0.f, ve::min(1.f, supply_private / demand_private));
			auto supply_sold_paid = ve::select(supply_private == 0.f, 0.f, ve::min(1.f, demand_private / supply_private));

			state.world.province_set_service_satisfaction_for_free(pids, i, free_ratio);
			state.world.province_set_service_satisfaction(pids, i, satisfaction_paid);
			state.world.province_set_service_sold(pids, i, supply_sold_paid);
		});
	}
}

}

namespace advanced_province_buildings {

// could be expanded by 1000 per 1 years
constexpr float max_port_expansion_speed = 1000.f / 365.f;
constexpr float max_city_expansion_rate= 0.1f / 365.f;
constexpr float max_city_expansion_speed = 1000.f / 365.f;
constexpr float ports_decay_speed = 0.99999f;

// it uses speed and other stuff because currently we don't have merchant fleets and transportation is done by port workers instead of sailors
float ports_efficiency(sys::state& state, dcon::nation_id n, float size) {
	auto base_id = state.military_definitions.base_naval_unit;
	auto& base_stats = state.world.nation_get_unit_stats(n, base_id);
	auto speed = std::max(1.f, 10.f + base_stats.maximum_speed);
	auto nmod = std::max(0.1f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::supply_range) + 1.0f);
	return speed / 10.f * nmod * (1.f + size / 10000.f);
}

// TODO: move definitions to assets
const advanced_building_definition definitions[services::list::total] = {
// school
{
	.throughput_labour_type = economy::labor::high_education,
	.output = services::list::education,
	.output_amount = 30.f,
},
// ports
{
	.throughput_labour_type = economy::labor::no_education,
	.output = services::list::port_capacity,
	.output_amount = 1.f,
	.associated_building = economy::province_building_type::naval_base
},
//cities
{
	.requires_labor = false,
	.output = services::list::urban_housing,
	.output_amount = 1.f,
	.maintenance_rate = 0.00001f,
	.associated_building = economy::province_building_type::railroad
}
};

constexpr inline float a_few_months = 120.f;

void initialize_size_of_dcon_arrays(sys::state& state) {
	state.world.province_resize_advanced_province_building_national_size(list::total);
	state.world.province_resize_advanced_province_building_private_size(list::total);
	state.world.province_resize_advanced_province_building_private_savings(list::total);
	state.world.province_resize_advanced_province_building_private_output(list::total);
	state.world.province_resize_advanced_province_building_max_national_size(list::total);
	state.world.province_resize_advanced_province_building_max_private_size(list::total);
}

void update_consumption(sys::state& state) {
	for(int32_t i = 0; i < list::total; i++) {
		auto& def = definitions[i];
		if (!def.requires_labor) continue;
		state.world.execute_serial_over_province([&](auto pids) {
			auto private_size = state.world.province_get_advanced_province_building_private_size(pids, i);
			auto national_size = state.world.province_get_advanced_province_building_national_size(pids, i);
			auto total = private_size + national_size;

			auto current_demand = state.world.province_get_labor_demand(pids, def.throughput_labour_type);
			state.world.province_set_labor_demand(pids, def.throughput_labour_type, current_demand + total);
		});
	}

	// construction of cities
	/*

	Cities are special as instead of using labor to produce services, they require labor for construction and maintenance.
	Construction requires skilled labor.
	Used property requires maintenance and would decay if unmaintained.
	But construction of new housing is quite expensive and takes time.
	So construction of a city will happen only when it will pay off in a few years
	We should also add subsidies to construction of housing, so players could interact with the system

	*/

	{
		auto id = list::local_cities_and_towns;
		auto& def = definitions[id];
		auto& costs = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].cost;
		auto build_time = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].time;

		province::for_each_market_province_parallel_over_market(state, [&](dcon::market_id mid, dcon::state_instance_id sid, dcon::province_id pid) {
			auto output_cost = state.world.province_get_service_price(pid, def.output);

			auto labor_cost_per_constructed_unit =
				state.world.province_get_labor_price(pid, economy::labor_constants::construction_labor)
				* economy::labor_constants::labor_per_construction_unit;

			auto material_cost_per_constructed_unit = 0.f;
			for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
				auto cid = costs.commodity_type[i];
				if(!cid) {
					break;
				}
				auto amount = costs.commodity_amounts[i] / build_time;
				material_cost_per_constructed_unit += amount * economy::price(state, mid, cid);
			}
			
			auto expected_profit_per_size = output_cost * def.output_amount;
			auto expected_maintenance = def.maintenance_rate * (material_cost_per_constructed_unit + labor_cost_per_constructed_unit);

			auto expected_days_to_payoff = (material_cost_per_constructed_unit + labor_cost_per_constructed_unit) / (expected_profit_per_size - expected_maintenance);

			auto max_size = state.world.province_get_advanced_province_building_max_private_size(pid, id);
			auto size = state.world.province_get_advanced_province_building_private_size(pid, id);
			auto max_expansion_speed = max_size * max_city_expansion_rate + max_city_expansion_speed;

			auto lots_of_empty_housing = size < max_size * 0.8f;

			auto demand_scale = 0.f;

			if((expected_profit_per_size - expected_maintenance > 0) && expected_days_to_payoff > 0.f && expected_days_to_payoff < 365.f * 5.f && !lots_of_empty_housing) {
				/*
				If new housing could pay off in a few months, we construct it as fast as possible.
				If new housing could pay off only in 5 years, we stop construction
				Also used property requires maintenance
				*/
				demand_scale = max_expansion_speed * a_few_months / (a_few_months + expected_days_to_payoff);
			}

			/*
			We don't want to expand faster than local labor allows us
			*/
			auto sat = state.world.province_get_labor_demand_satisfaction(pid, economy::labor_constants::construction_labor);
			demand_scale = demand_scale * sat;

			/*
			Register demand both on construction materials and construction labor
			*/
			for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
				auto cid = costs.commodity_type[i];
				if(!cid) {
					break;
				}
				auto amount =  costs.commodity_amounts[i] / build_time;
				economy::register_demand(state, mid, cid, amount * demand_scale);
			}
			auto labor_demand = state.world.province_get_labor_demand(pid, economy::labor_constants::construction_labor);
			state.world.province_set_labor_demand(
				pid,
				economy::labor_constants::construction_labor,
				labor_demand
				+ demand_scale * economy::labor_constants::labor_per_construction_unit
			);
		});
	}

	//private construction demand of ports
	{
		auto id = list::civilian_ports;
		auto& def = definitions[id];
		auto& costs = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].cost;
		auto build_time = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].time;

		province::for_each_market_province_parallel_over_market(state, [&](dcon::market_id mid, dcon::state_instance_id sid, dcon::province_id pid) {
			if(!state.world.province_get_is_coast(pid)) return;
			auto output_cost = state.world.province_get_service_price(pid, def.output);
			auto input_cost = state.world.province_get_labor_price(pid, def.throughput_labour_type);
			auto expected_profit_per_size = output_cost * def.output_amount - input_cost;
			auto max_size = state.world.province_get_advanced_province_building_max_private_size(pid, id);
			auto size = state.world.province_get_advanced_province_building_private_size(pid, id);
			if(expected_profit_per_size > 0.f && size > 0.8f * max_size) {
				// calculate costs:
				auto cost = 0.f;
				for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
					auto cid = costs.commodity_type[i];
					if(!cid) {
						break;
					}
					auto amount = max_port_expansion_speed * costs.commodity_amounts[i] / build_time;
					cost += amount * economy::price(state, mid, cid);
				}
				assert(cost != 0.f);
				auto scale = std::min(1.f, expected_profit_per_size * 0.1f / cost);

				// if ports are profitable and size is close to max size, register demand on commodities
				for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
					auto cid = costs.commodity_type[i];
					if(!cid) {
						break;
					}
					auto amount = max_port_expansion_speed * costs.commodity_amounts[i] / build_time;
					economy::register_demand(state, mid, cid, amount * scale);
				}
			}
		});
	}
}

void update_profit_and_refund(sys::state& state) {
	// can't be vectorised directly
	// TODO: add refund buffer to execute in serial way and then sum it up in parallel over nation/market

	// refund to nation:
	for(int32_t i = 0; i < list::total; i++) {
		auto& def = definitions[i];
		state.world.for_each_province([&](auto pids) {
			auto local_nation = state.world.province_get_nation_from_province_ownership(pids);
			if(state.world.nation_is_valid(local_nation)) { // Check that it is valid, or we may have an invalid write on uncolonized!
				auto cost_of_input = state.world.province_get_labor_price(pids, def.throughput_labour_type);
				auto national_size = state.world.province_get_advanced_province_building_national_size(pids, i);
				auto actually_bought = state.world.province_get_labor_demand_satisfaction(pids, def.throughput_labour_type);
				auto treasury = state.world.nation_get_stockpiles(local_nation, economy::money);
				assert(std::isfinite(treasury + (1.f - actually_bought) * national_size * cost_of_input));
				state.world.nation_set_stockpiles(local_nation, economy::money, treasury + (1.f - actually_bought) * national_size * cost_of_input);
			}
			
		});
	}

	province::for_each_market_province_parallel_over_market(state, [&](dcon::market_id mid, dcon::state_instance_id sid, dcon::province_id pid) {
		auto owner = state.world.province_get_nation_from_province_ownership(pid);

		// profit exists only for private enterprises
		for(int32_t i = 0; i < list::total; i++) {
			auto& def = definitions[i];
			auto private_size = state.world.province_get_advanced_province_building_private_size(pid, i);
			auto cost_of_input = state.world.province_get_labor_price(pid, def.throughput_labour_type);
			if(!def.requires_labor) {
				cost_of_input = 0.f;
			}
			auto cost_of_output = state.world.province_get_service_price(pid, def.output);
			auto actually_bought = state.world.province_get_labor_demand_satisfaction(pid, def.throughput_labour_type);

			auto output = state.world.province_get_advanced_province_building_private_output(pid, i);
			auto actually_sold = state.world.province_get_service_sold(pid, def.output);

			auto profit = output * actually_sold * cost_of_output - private_size * cost_of_input * actually_bought;

			auto current_money = state.world.province_get_advanced_province_building_private_savings(pid, i);
			state.world.province_set_advanced_province_building_private_savings(pid, i, current_money + profit);
		}

		// expand ports
		if(state.world.province_get_is_coast(pid)) {
			auto id = list::civilian_ports;
			auto& def = definitions[id];
			auto& costs = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].cost;
			auto build_time = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].time;
			auto output_cost = state.world.province_get_service_price(pid, def.output);
			auto input_cost = state.world.province_get_labor_price(pid, def.throughput_labour_type);
			auto max_size = state.world.province_get_advanced_province_building_max_private_size(pid, id);
			auto efficiency = ports_efficiency(state, owner, max_size);
			auto expected_profit_per_size = output_cost * def.output_amount * efficiency - input_cost;
			auto size = state.world.province_get_advanced_province_building_private_size(pid, id);
			if(expected_profit_per_size > 0.f && size > 0.8f * max_size) {
				// calculate costs:
				auto total_cost = 0.f;
				for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
					auto cid = costs.commodity_type[i];
					if(!cid) {
						break;
					}
					auto amount = max_port_expansion_speed * costs.commodity_amounts[i] / build_time;
					total_cost += amount * economy::price(state, mid, cid);
				}
				assert(total_cost != 0.f);
				auto scale = std::min(1.f, expected_profit_per_size * 0.1f / total_cost);

				auto expansion_scale = 1.f;
				auto cost = 0.f;
				// figure out how much we were able to buy
				for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
					auto cid = costs.commodity_type[i];
					if(!cid) {
						break;
					}
					auto probability = state.world.market_get_actual_probability_to_buy(mid, cid);
					// we promised to buy - we spend money and throw away excess items
					// otherwise we generated demand and then haven't fulfilled our promise
					cost += max_port_expansion_speed * costs.commodity_amounts[i] / build_time * scale * economy::price(state, mid, cid) * probability;
					if(probability < expansion_scale) {
						expansion_scale = probability;
					}
				}

				auto current_money = state.world.province_get_advanced_province_building_private_savings(pid, id);
				state.world.province_set_advanced_province_building_private_savings(pid, id, current_money - cost);

				auto current_max_size = state.world.province_get_advanced_province_building_max_private_size(pid, id);
				state.world.province_set_advanced_province_building_max_private_size(
					pid, id, std::max(500.f, current_max_size * ports_decay_speed + expansion_scale * max_port_expansion_speed)
				);
			} else {
				auto current_max_size = state.world.province_get_advanced_province_building_max_private_size(pid, id);
				state.world.province_set_advanced_province_building_max_private_size(
					pid, id, std::max(500.f, current_max_size * ports_decay_speed)
				);
			}
		}

		// expand or shrink cities
		{
			auto id = list::local_cities_and_towns;
			auto& def = definitions[id];
			auto& costs = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].cost;
			auto build_time = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].time;

			auto output_cost = state.world.province_get_service_price(pid, def.output);

			auto labor_cost_per_constructed_unit =
				state.world.province_get_labor_price(pid, economy::labor_constants::construction_labor)
				* economy::labor_constants::labor_per_construction_unit;

			auto material_cost_per_constructed_unit = 0.f;
			for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
				auto cid = costs.commodity_type[i];
				if(!cid) {
					break;
				}
				auto amount = costs.commodity_amounts[i] / build_time;
				material_cost_per_constructed_unit += amount * economy::price(state, mid, cid);
			}
			
			auto expected_profit_per_size = output_cost * def.output_amount;
			auto expected_maintenance = def.maintenance_rate * (material_cost_per_constructed_unit + labor_cost_per_constructed_unit);

			auto expected_days_to_payoff = (material_cost_per_constructed_unit + labor_cost_per_constructed_unit) / (expected_profit_per_size - expected_maintenance);

			auto max_size = state.world.province_get_advanced_province_building_max_private_size(pid, id);
			auto size = state.world.province_get_advanced_province_building_private_size(pid, id);
			auto max_expansion_speed = max_size * max_city_expansion_rate + max_city_expansion_speed;

			auto lots_of_empty_housing = size < max_size * 0.8f;

			auto construction_scale = 0.f;

			if((expected_profit_per_size - expected_maintenance > 0.f) && expected_days_to_payoff > 0.f && expected_days_to_payoff < 365.f * 5.f && !lots_of_empty_housing) {
				/*
				If new housing could pay off in a few months, we construct it as fast as possible.
				If new housing could pay off only in 5 years, we stop construction.
				*/
				construction_scale = max_expansion_speed * a_few_months / (a_few_months + expected_days_to_payoff);
			}

			/*
			Used property requires maintenance
			*/

			auto labor_sat = state.world.province_get_labor_demand_satisfaction(pid, economy::labor_constants::construction_labor);
			auto demand_scale = construction_scale * labor_sat;

			/*
			Find out how much we were able to buy.
			*/
			float actually_bought = labor_sat;
			float spendings = 0.f;
			for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
				auto cid = costs.commodity_type[i];
				if(!cid) {
					break;
				}
				auto sat = state.world.market_get_actual_probability_to_buy(mid, cid);
				actually_bought = std::min(actually_bought, sat);
				auto amount =  costs.commodity_amounts[i] / build_time;
				spendings += amount * economy::price(state, mid, cid) * demand_scale * sat;
			}
			auto sat = state.world.province_get_labor_demand_satisfaction(pid, economy::labor_constants::construction_labor);
			actually_bought = std::min(actually_bought, sat);
			spendings += economy::labor_constants::labor_per_construction_unit * state.world.province_get_labor_price(pid, economy::labor_constants::construction_labor) * demand_scale * sat;

			/*
			Finally, we can advance construction and decay 
			*/

			float size_increase = actually_bought * construction_scale - def.maintenance_rate * size;
			state.world.province_set_advanced_province_building_max_private_size(
				pid, id, max_size + size_increase
			);

			auto current_money = state.world.province_get_advanced_province_building_private_savings(pid, id);
			state.world.province_set_advanced_province_building_private_savings(pid, id, current_money - spendings);
		}
	});
}

void update_private_size(sys::state& state) {
	// schools
	{
		auto bid = list::schools_and_universities;
		auto& def = definitions[bid];

		state.world.execute_serial_over_province([&](auto pids) {
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
		});
	}

	// ports
	{
		auto bid = list::civilian_ports;
		auto& def = definitions[bid];

		province::for_each_market_province_parallel_over_market(state, [&](dcon::market_id mid, dcon::state_instance_id sid, dcon::province_id pid) {
			auto owner = state.world.province_get_nation_from_province_ownership(pid);
			auto cost_of_input = state.world.province_get_labor_price(pid, def.throughput_labour_type);
			auto max_size = state.world.province_get_advanced_province_building_max_private_size(pid, bid);
			auto efficiency = ports_efficiency(state, owner, max_size);
			auto cost_of_output = state.world.province_get_service_price(pid, def.output) * efficiency * def.output_amount;
			auto current_private_size = state.world.province_get_advanced_province_building_private_size(pid, bid);
			auto margin = (cost_of_output - cost_of_input) / cost_of_input;
			auto probability_to_hire = state.world.province_get_labor_demand_satisfaction(pid, def.throughput_labour_type);
			margin = margin > 0.f ? std::max(0.f, (probability_to_hire - 0.4f)) * margin : margin;
			auto new_private_size = current_private_size + ve::min(margin, 100.f) + ve::min(ve::max(margin, -0.01f), 0.01f) * current_private_size;
			new_private_size = ve::min(max_size, new_private_size);
			state.world.province_set_advanced_province_building_private_size(pid, bid, ve::max(0.f, new_private_size));
		});
	}

	/*
	
	Here local landlords decide on how much of the land they plan to rent out.
	If cost of maintenance of living space is high relatively to rent prices, they decrease rent area
	Otherwise, they rent out more living space.

	*/

	{
		auto bid = list::local_cities_and_towns;
		auto& def = definitions[bid];
		auto& costs = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].cost;
		auto build_time = state.economy_definitions.building_definitions[(size_t)(def.associated_building)].time;

		province::for_each_market_province_parallel_over_market(state, [&](dcon::market_id mid, dcon::state_instance_id sid, dcon::province_id pid) {
			auto owner = state.world.province_get_nation_from_province_ownership(pid);
			auto cost_of_labor = state.world.province_get_labor_price(pid, def.throughput_labour_type);
			auto material_cost_per_constructed_unit = 0.f;
			for(size_t i = 0; i < economy::commodity_set::set_size; i++) {
				auto cid = costs.commodity_type[i];
				if(!cid) {
					break;
				}
				auto amount = costs.commodity_amounts[i] / build_time;
				material_cost_per_constructed_unit += amount * economy::price(state, mid, cid);
			}
			auto maintenance_cost_per_unit = def.maintenance_rate * (
				cost_of_labor
				* economy::labor_constants::labor_per_construction_unit
				+ material_cost_per_constructed_unit
			);

			auto income_per_unit = state.world.province_get_service_price(pid, def.output) * def.output_amount;
			auto max_size = state.world.province_get_advanced_province_building_max_private_size(pid, bid);
			auto current_private_size = state.world.province_get_advanced_province_building_private_size(pid, bid);
			auto margin = (income_per_unit - maintenance_cost_per_unit) / maintenance_cost_per_unit;
			auto new_private_size = current_private_size + ve::min(margin, 100.f) + ve::min(ve::max(margin, -0.01f), 0.01f) * current_private_size;
			new_private_size = ve::min(max_size, new_private_size);
			state.world.province_set_advanced_province_building_private_size(pid, bid, ve::max(0.f, new_private_size));
		});
	}
}

void update_national_size(sys::state& state) {
	{
		auto bid = list::schools_and_universities;
		auto& def = definitions[bid];

		state.world.execute_serial_over_province([&](auto pids) {
			auto owner = state.world.province_get_nation_from_province_ownership(pids);
			auto tmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency) + 1.0f;
			auto nmod = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::education_efficiency_modifier) + 1.0f;

			auto cost_of_input = state.world.province_get_labor_price(pids, def.throughput_labour_type);
			auto cost_of_output = state.world.province_get_service_price(pids, def.output) * tmod * nmod * def.output_amount;

			auto spending_scale = state.world.nation_get_spending_level(owner);
			auto national_budget = state.world.nation_get_last_base_budget(owner);
			auto education_priority = ve::to_float(state.world.nation_get_education_spending(owner)) / 100.f;
			auto education_budget = national_budget * education_priority * spending_scale;
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
			state.world.province_set_advanced_province_building_private_output(pids, bid, current_private_size * output);

			auto current_public_size = state.world.province_get_advanced_province_building_national_size(pids, bid);
			auto current_public_supply = state.world.province_get_service_supply_public(pids, def.output);
			state.world.province_set_service_supply_public(pids, def.output, current_public_supply + current_public_size * output);
		});

		// TODO:
		// update gdp of local markets with education
	}

	// ports
	{
		auto bid = list::civilian_ports;
		auto& def = definitions[bid];

		state.world.execute_serial_over_province([&](auto pids) {
			auto input_satisfaction = state.world.province_get_labor_demand_satisfaction(pids, def.throughput_labour_type);
			auto current_private_size = state.world.province_get_advanced_province_building_private_size(pids, bid);
			auto owner = state.world.province_get_nation_from_province_ownership(pids);
			auto max_size = state.world.province_get_advanced_province_building_max_private_size(pids, bid);
			auto efficiency = ve::apply(
				[&](dcon::nation_id n, float max_size_port) { return ports_efficiency(state, n, max_size_port); },
				owner, max_size
			);
			//assume that low trade volume doesn't require any additional infrastructure or workers
			auto output = 100.f + current_private_size * input_satisfaction * def.output_amount * efficiency;
			auto current_private_supply = state.world.province_get_service_supply_private(pids, def.output);
			state.world.province_set_service_supply_private(pids, def.output, current_private_supply + output);
			state.world.province_set_advanced_province_building_private_output(pids, bid, output);
		});
	}

	// housing
	/*
	Doesn't require any inputs, so it's much more simple
	*/
	{
		auto bid = list::local_cities_and_towns;
		auto& def = definitions[bid];

		state.world.execute_serial_over_province([&](auto pids) {
			auto current_private_size = state.world.province_get_advanced_province_building_private_size(pids, bid);
			auto output = current_private_size * def.output_amount;
			auto current_private_supply = state.world.province_get_service_supply_private(pids, def.output);
			state.world.province_set_service_supply_private(pids, def.output, current_private_supply + output);
			state.world.province_set_advanced_province_building_private_output(pids, bid, output);
		});
	}
}

}
