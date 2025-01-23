#include "dcon_generated.hpp"
#include "system_state.hpp"

#include "demographics.hpp"
#include "economy.hpp"
#include "economy_government.hpp"
#include "province_templates.hpp"

namespace economy {

// your administration has to work on two things
// holding entire realm together
// and solving administrative questions of local pops
//
// there are several counters:
// imperial administration: total population divided by C plus some base factor
// local administration: total population in a given market divided by C
// 
// imperial administration generates demand on accepted high educated labor in capital market
// local administration generates demand on local high educated labor
// (we don't want too much demand on accepted highly educated pops in the middle of nowhere)
//
// employed people in administration influence the maximum amount of taxes the nation can collect

// to avoid death traps
inline constexpr float base_tax_collection_capacity = 5000.f;

inline constexpr float base_population_per_admin = 50.f;

float population_per_admin(sys::state& state, dcon::nation_id n) {
	return base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));
}

inline constexpr float base_admin_employment = 500.f;
inline constexpr float base_tax_collection = 10000.f;

// tax collector can collect taxes N times larger than local sum of wages
// this value is modified by tax and admin efficiency techs

inline constexpr float tax_collection_multiplier = 10.f;

float required_labour_in_capital_administration(sys::state& state, dcon::nation_id n) {
	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));
	auto total_population = state.world.nation_get_demographics(n, demographics::total);
	return total_population / admin_efficiency + base_admin_employment;
}

float tax_collection_capacity(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid) {
	auto local_market = state.world.state_instance_get_market_from_local_market(sid);
	auto tax_collection_global = 1.f + global_admin_ratio(state, n);
	auto local_tax_collectors = state.world.market_get_administration_employment_target(local_market)
		* state.world.market_get_labor_demand_satisfaction(local_market, economy::labor::high_education);

	auto collection_rate_per_tax_collector =
		state.world.market_get_labor_price(local_market, economy::labor::high_education)
		+ state.world.market_get_labor_price(local_market, economy::labor::basic_education)
		+ state.world.market_get_labor_price(local_market, economy::labor::no_education);

	auto effort =
		float(state.world.nation_get_poor_tax(n))
		+ float(state.world.nation_get_middle_tax(n))
		+ float(state.world.nation_get_rich_tax(n));

	return
		base_tax_collection_capacity
		* collection_rate_per_tax_collector
		+
		tax_collection_global
		* (1.f + state.world.nation_get_administrative_efficiency(n))
		* nations::tax_efficiency(state, n)
		* collection_rate_per_tax_collector
		* effort / 100.f
		* (base_tax_collection + local_tax_collectors)
		* tax_collection_multiplier;
}

float total_tax_collection_capacity(sys::state& state, dcon::nation_id n) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);
		total += tax_collection_capacity(state, n, local_state);
	});
	return total;
}

float global_admin_ratio(sys::state& state, dcon::nation_id n) {
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);

	auto required_labor_capital = required_labour_in_capital_administration(state, n);
	auto current_labor_capital =
		state.world.nation_get_administration_employment_target_in_capital(n)
		* state.world.market_get_labor_demand_satisfaction(capital_market, economy::labor::high_education_and_accepted);

	return current_labor_capital / required_labor_capital;
}

float local_admin_ratio(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid) {
	auto local_market = state.world.state_instance_get_market_from_local_market(sid);
	auto local_population = state.world.state_instance_get_demographics(sid, demographics::total);
	float required_labor_local = local_population / population_per_admin(state, n);

	auto current_labor_local =
		state.world.market_get_administration_employment_target(local_market)
		* state.world.market_get_labor_demand_satisfaction(local_market, economy::labor::high_education);

	return current_labor_local / required_labor_local;
}

float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget_priority) {
	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * budget_priority;
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);
	auto required_labor_capital = required_labour_in_capital_administration(state, n);
	auto labor_price_capital = state.world.market_get_labor_price(capital_market, economy::labor::high_education_and_accepted);
	float required_budget = required_labor_capital * labor_price_capital * state.world.market_get_labor_demand_satisfaction(capital_market, labor::high_education_and_accepted);

	return std::min(required_budget, admin_budget);
}

float estimate_spendings_administration_state(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid, float budget_priority) {
	auto local_market = state.world.state_instance_get_market_from_local_market(sid);
	auto local_population = state.world.state_instance_get_demographics(sid, demographics::total);
	float required_labor_local = local_population / population_per_admin(state, n);
	float labor_price_local = state.world.market_get_labor_price(local_market, economy::labor::high_education);

	return
		required_labor_local
		* labor_price_local
		* state.world.market_get_labor_demand_satisfaction(local_market, labor::high_education);
}

float estimate_spendings_administration(sys::state& state, dcon::nation_id n, float budget_priority) {
	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * budget_priority;
	float required_budget = estimate_spendings_administration_capital(state, n, budget_priority);
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		required_budget += estimate_spendings_administration_state(state, n, state.world.state_ownership_get_state(soid), budget_priority);
	});
	return std::min(required_budget, admin_budget);
}

float full_spendings_administration(sys::state& state, dcon::nation_id n) {
	auto total_cost = 0.f;
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);

	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * float(state.world.nation_get_administrative_spending(n)) / 100.f;

	total_cost +=
		state.world.nation_get_administration_employment_target_in_capital(n)
		* state.world.market_get_labor_demand_satisfaction(capital_market, economy::labor::high_education_and_accepted)
		* state.world.market_get_labor_price(capital_market, economy::labor::high_education_and_accepted);

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

		total_cost +=
			state.world.market_get_administration_employment_target(local_market)
			* state.world.market_get_labor_demand_satisfaction(local_market, labor::high_education)
			* state.world.market_get_labor_price(local_market, labor::high_education);
	});

	assert(total_cost >= 0.f);

	return std::min(total_cost, admin_budget);
}

void update_consumption_administration(sys::state& state, dcon::nation_id n) {
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);

	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * state.world.nation_get_spending_level(n) * float(state.world.nation_get_administrative_spending(n)) / 100.f;

	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));

	auto required_labor_capital = required_labour_in_capital_administration(state, n);
	auto labor_price_capital = state.world.market_get_labor_price(capital_market, economy::labor::high_education_and_accepted);
	auto effective_demand = std::min(required_labor_capital, admin_budget / labor_price_capital);

	state.world.nation_set_administration_employment_target_in_capital(n, effective_demand);
	state.world.market_get_labor_demand(capital_market, economy::labor::high_education_and_accepted) += effective_demand;
	admin_budget = admin_budget - effective_demand * labor_price_capital;

	if(admin_budget > 0.f) {
		// now we can demand labor in local lands
		// to do it, we have to estimate how much of the labor we can actually demand
		// so there should be at least two passes
		// the first one estimates total costs
		// the second one actually demands labor

		float required_budget = 0.f;

		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

			auto local_population = state.world.state_instance_get_demographics(local_state, demographics::total);

			required_labor_capital += local_population / admin_efficiency;
			float required_labor_local = local_population / admin_efficiency;
			float labor_price_local = state.world.market_get_labor_price(local_market, economy::labor::high_education);
			required_budget += required_labor_local * labor_price_local;
		});

		auto scale = 1.f;
		if(required_budget > 0.f) {
			scale = std::min(1.f, admin_budget / required_budget);
		}

		state.world.nation_for_each_state_ownership(n, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

			auto local_population = state.world.state_instance_get_demographics(local_state, demographics::total);

			required_labor_capital += local_population / admin_efficiency;
			float required_labor_local = local_population / admin_efficiency;
			state.world.market_get_labor_demand(local_market, economy::labor::high_education) += required_labor_local * scale;
			state.world.market_set_administration_employment_target(local_market, required_labor_local * scale);
		});
	}
}

void collect_taxes(sys::state& state, dcon::nation_id n) {
	float total_poor_tax_base = 0.0f;
	float total_mid_tax_base = 0.0f;
	float total_rich_tax_base = 0.0f;

	auto collected_tax = 0.f;

	for(auto so : state.world.nation_get_state_ownership(n)) {
		auto s = so.get_state();
		auto max_tax_capacity = tax_collection_capacity(state, n, s);
		float potential_tax_poor = 0.f;
		float potential_tax_mid = 0.f;
		float potential_tax_rich = 0.f;

		province::for_each_province_in_state_instance(state, s, [&](auto province) {
			float province_potential_tax_poor = 0.f;
			float province_potential_tax_mid = 0.f;
			float province_potential_tax_rich = 0.f;

			if(
				state.world.province_get_nation_from_province_ownership(province)
				==
				state.world.province_get_nation_from_province_control(province)
			) {
				for(auto pl : state.world.province_get_pop_location(province)) {
					auto& pop_money = pl.get_pop().get_savings();
					auto strata = culture::pop_strata(pl.get_pop().get_poptype().get_strata());
					if(strata == culture::pop_strata::poor) {
						province_potential_tax_poor += pop_money;
					} else if(strata == culture::pop_strata::middle) {
						province_potential_tax_mid += pop_money;
					} else if(strata == culture::pop_strata::rich) {
						province_potential_tax_rich += pop_money;
					}
					assert(std::isfinite(pl.get_pop().get_savings()));
				}
			}

			state.world.province_set_tax_base_poor(province, province_potential_tax_poor);
			state.world.province_set_tax_base_middle(province, province_potential_tax_mid);
			state.world.province_set_tax_base_rich(province, province_potential_tax_rich);

			potential_tax_poor += province_potential_tax_poor;
			potential_tax_mid += province_potential_tax_mid;
			potential_tax_rich += province_potential_tax_rich;
		});

		total_poor_tax_base += potential_tax_poor;
		total_mid_tax_base += potential_tax_mid;
		total_rich_tax_base += potential_tax_rich;


		auto local_tax = potential_tax_poor * float(state.world.nation_get_poor_tax(n)) / 100.f
			+ potential_tax_mid * float(state.world.nation_get_middle_tax(n)) / 100.0f
			+ potential_tax_rich * float(state.world.nation_get_rich_tax(n)) / 100.0f;
		auto total_possible = std::min(local_tax, max_tax_capacity);


		auto collected_ratio = 0.f;
		if(total_possible > 0.f) {
			collected_ratio = total_possible / local_tax;
		}

		collected_tax += local_tax * collected_ratio;

		// apply actual tax

		auto const poor_effect = (1.0f - collected_ratio * float(state.world.nation_get_poor_tax(n)) / 100.0f);
		auto const middle_effect = (1.0f - collected_ratio * float(state.world.nation_get_middle_tax(n)) / 100.0f);
		auto const rich_effect = (1.0f - collected_ratio * float(state.world.nation_get_rich_tax(n)) / 100.0f);

		assert(poor_effect >= 0 && middle_effect >= 0 && rich_effect >= 0);

		province::for_each_province_in_state_instance(state, s, [&](auto province) {
			if(
				state.world.province_get_nation_from_province_ownership(province)
				==
				state.world.province_get_nation_from_province_control(province)
			) {
				for(auto pl : state.world.province_get_pop_location(province)) {
					auto& pop_money = pl.get_pop().get_savings();
					auto strata = culture::pop_strata(pl.get_pop().get_poptype().get_strata());
					if(strata == culture::pop_strata::poor) {
						pop_money *= poor_effect;
					} else if(strata == culture::pop_strata::middle) {
						pop_money *= middle_effect;
					} else if(strata == culture::pop_strata::rich) {
						pop_money *= rich_effect;
					}
					assert(std::isfinite(pl.get_pop().get_savings()));
				}
			}
		});
		/*
		potential_tax_poor * float(state.world.nation_get_poor_tax(n)) / 100.f * collected_ratio;
		potential_tax_mid * float(state.world.nation_get_middle_tax(n)) / 100.f * collected_ratio;
		potential_tax_rich * float(state.world.nation_get_rich_tax(n)) / 100.f * collected_ratio;
		*/
	}

	state.world.nation_set_total_rich_income(n, total_rich_tax_base);
	state.world.nation_set_total_middle_income(n, total_mid_tax_base);
	state.world.nation_set_total_poor_income(n, total_poor_tax_base);

	

	assert(std::isfinite(collected_tax));
	assert(collected_tax >= 0);
	state.world.nation_get_stockpiles(n, money) += collected_tax;
}

tax_information explain_tax_income_local(sys::state& state, dcon::nation_id n, dcon::state_instance_id s) {
	tax_information result{ };
	result.capacity = tax_collection_capacity(state, n, s);
	province::for_each_province_in_state_instance(state, s, [&](auto province) {
		result.poor_potential += state.world.province_get_tax_base_poor(province);
		result.mid_potential += state.world.province_get_tax_base_middle(province);
		result.rich_potential += state.world.province_get_tax_base_rich(province);
	});
	result.poor = result.poor_potential * float(state.world.nation_get_poor_tax(n)) / 100.f;
	result.mid = result.mid_potential * float(state.world.nation_get_middle_tax(n)) / 100.0f;
	result.rich = result.rich_potential * float(state.world.nation_get_rich_tax(n)) / 100.0f;
	auto total = result.poor + result.mid + result.rich;
	auto total_possible = std::min(total, result.capacity);		
	auto collected_ratio = 0.f;
	if(total_possible > 0.f) {
		collected_ratio = total_possible / total;
	}
	result.poor *= collected_ratio;
	result.mid *= collected_ratio;
	result.rich *= collected_ratio;
	return result;
}

tax_information explain_tax_income(sys::state& state, dcon::nation_id n) {
	tax_information result{ };

	auto const tax_eff = nations::tax_efficiency(state, n);

	float total_poor_tax_base = 0.0f;
	float total_mid_tax_base = 0.0f;
	float total_rich_tax_base = 0.0f;

	auto collected_tax_poor = 0.f;
	auto collected_tax_mid = 0.f;
	auto collected_tax_rich = 0.f;

	for(auto so : state.world.nation_get_state_ownership(n)) {
		auto s = so.get_state();
		auto info = explain_tax_income_local(state, n, s);

		result.capacity += info.capacity;
		result.mid += info.mid;
		result.mid_potential += info.mid_potential;
		result.poor += info.poor;
		result.poor_potential += info.poor;
		result.rich += info.rich;
		result.rich_potential += result.rich_potential;
	}

	return result;
}

}
