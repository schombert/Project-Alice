#include "dcon_generated.hpp"
#include "system_state.hpp"

#include "demographics.hpp"
#include "economy.hpp"

namespace economy {

// your administration has to work on two things
// holding entire realm together
// and solving administrative questions of local pops
//
// there are several counters:
// imperial administration: total population divided by C
// local administration: total population in a given market divided by C
// 
// imperial administration generates demand on accepted high educated labor in capital market
// local administration generates demand on local high educated labor
// (we don't want too much demand on accepted highly educated pops in the middle of nowhere)
//
// employed people in administration influence the maximum amount of taxes the nation can collect

inline constexpr float base_population_per_admin = 1000.f;

float required_labour_in_capital_administration(sys::state& state, dcon::nation_id n, float budget_priority) {
	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));
	auto total_population = state.world.nation_get_demographics(n, demographics::total);
	return total_population / admin_efficiency;
}

float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget_priority) {
	auto total_budget = state.world.nation_get_stockpiles(n, economy::money);
	auto admin_budget = total_budget * budget_priority;
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);
	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));
	auto total_population = state.world.nation_get_demographics(n, demographics::total);
	auto required_labor_capital = total_population / admin_efficiency;
	auto labor_price_capital = state.world.market_get_labor_price(capital_market, economy::labor::high_education_and_accepted);
	float required_budget = required_labor_capital * labor_price_capital * state.world.market_get_labor_demand_satisfaction(capital_market, labor::high_education_and_accepted);

	return std::min(required_budget, admin_budget);
}

float estimate_spendings_administration_state(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid, float budget_priority) {
	auto local_market = state.world.state_instance_get_market_from_local_market(sid);

	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));

	auto local_population = state.world.state_instance_get_demographics(sid, demographics::total);
	float required_labor_local = local_population / admin_efficiency;
	float labor_price_local = state.world.market_get_labor_price(local_market, economy::labor::high_education);

	return
		required_labor_local
		* labor_price_local
		* state.world.market_get_labor_demand_satisfaction(local_market, labor::high_education);
}

float estimate_spendings_administration(sys::state& state, dcon::nation_id n, float budget_priority) {
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);

	auto total_budget = state.world.nation_get_stockpiles(n, economy::money);
	auto admin_budget = total_budget * budget_priority;

	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));

	auto total_population = state.world.nation_get_demographics(n, demographics::total);
	auto required_labor_capital = total_population / admin_efficiency;
	auto labor_price_capital = state.world.market_get_labor_price(capital_market, economy::labor::high_education_and_accepted);
	float required_budget = required_labor_capital * labor_price_capital * state.world.market_get_labor_demand_satisfaction(capital_market, labor::high_education_and_accepted);

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

		auto local_population = state.world.state_instance_get_demographics(local_state, demographics::total);

		float required_labor_local = local_population / admin_efficiency;
		float labor_price_local = state.world.market_get_labor_price(local_market, economy::labor::high_education);
		required_budget += required_labor_local * labor_price_local * state.world.market_get_labor_demand_satisfaction(local_market, labor::high_education);
	});

	return std::min(required_budget, admin_budget);
}

float full_spendings_administration(sys::state& state, dcon::nation_id n) {
	auto total_cost = 0.f;
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_market = state.world.state_instance_get_market_from_local_market(capital_state);

	auto total_budget = state.world.nation_get_stockpiles(n, economy::money);
	auto admin_budget = total_budget * float(state.world.nation_get_administrative_spending(n)) / 100.f;

	auto labor_in_capital =
		state.world.nation_get_administration_employment_target_in_capital(n)
		* state.world.market_get_labor_demand_satisfaction(capital_market, economy::labor::high_education_and_accepted);

	total_cost += labor_in_capital * state.world.market_get_labor_price(capital_market, economy::labor::high_education_and_accepted);

	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto local_market = state.world.state_instance_get_market_from_local_market(local_state);

		float target = state.world.market_get_administration_employment_target(local_market);
		total_cost +=
			target
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

	auto total_budget = state.world.nation_get_stockpiles(n, economy::money);
	auto admin_budget = total_budget * state.world.nation_get_spending_level(n) * float(state.world.nation_get_administrative_spending(n)) / 100.f;

	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));

	auto total_population = state.world.nation_get_demographics(n, demographics::total);
	auto required_labor_capital = total_population / admin_efficiency;
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

}
