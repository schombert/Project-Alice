#include "dcon_generated.hpp"
#include "system_state.hpp"

#include "demographics.hpp"
#include "economy.hpp"
#include "economy_government.hpp"
#include "economy_stats.hpp"
#include "province_templates.hpp"

namespace economy {


// imperial administration generates demand on accepted high educated labor in capital market
//
// employed people in administration influence the maximum amount of taxes the nation can collect

// to avoid death traps
inline constexpr float base_tax_collection_capacity = 10000.f;

inline constexpr float base_population_per_admin = 200.f;

float population_per_admin(sys::state& state, dcon::nation_id n) {
	return base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));
}

// represents amount of people
// which would coordinate actions of local administration
// with the central administration
inline constexpr float base_admin_employment = 250.f;

// tax collector can collect taxes N times larger than local sum of wages
// this value is modified by tax and admin efficiency techs

inline constexpr float tax_collection_multiplier = 10.f;

float required_labour_in_capital_administration(sys::state& state, dcon::nation_id n) {
	auto admin_efficiency = base_population_per_admin * (1.f + state.world.nation_get_administrative_efficiency(n));
	auto total_population = state.world.nation_get_demographics(n, demographics::total);
	return total_population / admin_efficiency + base_admin_employment;
}

float tax_collection_capacity(sys::state& state, dcon::nation_id n, dcon::province_id pid) {
	auto tax_collection_global = 1.f + global_admin_ratio(state, n);
	auto local_tax_collectors = state.world.province_get_administration_employment_target(pid)
		* state.world.province_get_labor_demand_satisfaction(pid, economy::labor::high_education);

	auto collection_rate_per_tax_collector =
		state.world.province_get_labor_price(pid, economy::labor::high_education)
		+ state.world.province_get_labor_price(pid, economy::labor::basic_education)
		+ state.world.province_get_labor_price(pid, economy::labor::no_education);

	

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
	state.world.nation_for_each_province_ownership(n, [&](auto poid) {
		auto local_province = state.world.province_ownership_get_province(poid);
		total += tax_collection_capacity(state, n, local_province);
		});
	return num_of_administrations;
}
float count_active_administrations(sys::state& state, dcon::nation_id n) {
	auto num_of_administrations = 0.f;
	{
		auto capital = state.world.nation_get_capital(n);
		if(state.world.province_get_nation_from_province_control(capital) == n) {
			num_of_administrations += 1.f;
		}
	}
	state.world.nation_for_each_nation_administration(n, [&](auto naid) {
		auto admin = state.world.nation_administration_get_administration(naid);
		auto capital = state.world.administration_get_capital(admin);
		if(state.world.province_get_nation_from_province_control(capital) == n)
			num_of_administrations += 1.f;
	});
	return num_of_administrations;
}

float tax_collection_rate(sys::state& state, dcon::nation_id n, dcon::province_id pid) {
	// assume that in perfect conditions
	// without techs we can collect only this ratio
	// of desired taxes.
	auto base = 0.5f;
	auto from_control = state.world.province_get_control_ratio(pid);
	auto efficiency = nations::tax_efficiency(state, n);

	// we can always collect at least some taxes in the capital:
	auto capital = state.world.nation_get_capital(n);
	if(pid == capital) {
		from_control = std::max(0.1f, from_control);
	}

	auto required_labor_capital = required_labour_in_capital_administration(state, n);
	auto current_labor_capital =
		state.world.nation_get_administration_employment_target_in_capital(n)
		* state.world.province_get_labor_demand_satisfaction(capital, economy::labor::high_education_and_accepted);

	return current_labor_capital / required_labor_capital;
}

float local_admin_ratio(sys::state& state, dcon::nation_id n, dcon::province_id pid) {
	auto local_population = state.world.province_get_demographics(pid, demographics::total);
	float required_labor_local = local_population / population_per_admin(state, n);

	auto current_labor_local =
		state.world.province_get_administration_employment_target(pid)
		* state.world.province_get_labor_demand_satisfaction(pid, economy::labor::high_education);

	float side_effects = 0.0f;
	float bsum = 0.0f;
	float rsum = 0.0f;

	if(!state.world.province_get_is_owner_core(pid)) {
		side_effects += state.defines.noncore_tax_penalty;
	}
	if(state.world.province_get_nationalism(pid) > 0.f) {
		side_effects += state.defines.separatism_tax_penalty;
	}
	for(auto po : state.world.province_get_pop_location(pid)) {
		if(po.get_pop().get_is_primary_or_accepted_culture() &&
				po.get_pop().get_poptype() == state.culture_definitions.bureaucrat) {
			bsum += po.get_pop().get_size();
			if(po.get_pop().get_rebel_faction_from_pop_rebellion_membership()) {
				rsum += po.get_pop().get_size();
			}
		}
	}

	float issue_sum = 0.0f;
	for(auto i : state.culture_definitions.social_issues) {
		issue_sum += state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(n, i));
	}
	auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment + state.defines.max_bureaucracy_percentage;
	current_labor_local *= ((bsum - rsum) / bsum); // Rebellious bureaucrats damage local tax collection
	current_labor_local /= 0.01f / from_issues;

	if(std::isnan(current_labor_local)) {
		return 0.f;
	}

	return base * from_control * efficiency;
}

float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget_priority) {
	auto admin_count = count_active_administrations(state, n);
	if(admin_count == 0.f) {
		return 0.f;
	}

	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * budget_priority;
	return admin_budget;
}

float estimate_spendings_administration(sys::state& state, dcon::nation_id n, float budget_priority) {
	auto admin_count = count_active_administrations(state, n);
	if(admin_count == 0.f) {
		return 0.f;
	}

	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * budget_priority;
	return admin_budget;
}

float full_spendings_administration(sys::state& state, dcon::nation_id n) {
	auto admin_count = count_active_administrations(state, n);
	if(admin_count == 0.f) {
		return 0.f;
	}

	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * float(state.world.nation_get_administrative_spending(n)) / 100.f;

	return admin_budget;
}

void update_consumption_administration(sys::state& state, dcon::nation_id n) {

	auto total_budget = std::max(0.f, state.world.nation_get_stockpiles(n, economy::money));
	auto admin_budget = total_budget * state.world.nation_get_spending_level(n) * float(state.world.nation_get_administrative_spending(n)) / 100.f;

	auto admin_count = count_active_administrations(state, n);
	if(admin_count == 0.f) {
		return;
	}

	auto budget_per_administration = admin_budget / admin_count;
	auto admin_efficiency = population_per_admin(state, n);

	{
		auto capital = state.world.nation_get_capital(n);
		if(state.world.province_get_nation_from_province_control(capital) == n) {
			auto capital_state = state.world.province_get_state_membership(capital);
			auto capital_of_capital_state = state.world.state_instance_get_capital(capital_state);
			auto wage = state.world.province_get_labor_price(capital_of_capital_state, economy::labor::high_education_and_accepted);
			auto demand = budget_per_administration / wage;
			auto sat = state.world.province_get_labor_demand_satisfaction(capital_of_capital_state, economy::labor::high_education_and_accepted);
			// capitals generate base amount of control to avoid death spirals
			state.world.province_get_control_scale(capital) += demand * sat * population_per_admin(state, n) + 10'000.f;
			state.world.nation_set_administration_employment_target_in_capital(n, demand);
			state.world.province_get_labor_demand(capital_of_capital_state, economy::labor::high_education_and_accepted) += demand;
		}
	}

	// additional administrations are less efficient
	// and require additional people to generate control
	state.world.nation_for_each_nation_administration(n, [&](auto naid) {
		auto admin = state.world.nation_administration_get_administration(naid);
		auto capital = state.world.administration_get_capital(admin);
		if(state.world.province_get_nation_from_province_control(capital) != n) {
			return;
		}
		auto capital_state = state.world.province_get_state_membership(capital);
		auto capital_of_capital_state = state.world.state_instance_get_capital(capital_state);
		auto wage = state.world.province_get_labor_price(capital_of_capital_state, economy::labor::high_education_and_accepted);
		auto demand = budget_per_administration / wage;
		auto sat = state.world.province_get_labor_demand_satisfaction(capital_of_capital_state, economy::labor::high_education_and_accepted);
		state.world.province_get_control_scale(capital) += std::max(0.f, (demand * sat - base_admin_employment) * population_per_admin(state, n) * 0.5f);
		state.world.province_set_administration_employment_target(capital_of_capital_state, demand);
		state.world.province_get_labor_demand(capital_of_capital_state, economy::labor::high_education_and_accepted) += demand;
	});
}

void collect_taxes(sys::state& state, dcon::nation_id n) {
	float total_poor_tax_base = 0.0f;
	float total_mid_tax_base = 0.0f;
	float total_rich_tax_base = 0.0f;

	auto collected_tax = 0.f;

	for(auto po : state.world.nation_get_province_ownership(n)) {
		auto province = po.get_province();
		auto tax_multiplier = tax_collection_rate(state, n, province);
		float potential_tax_poor = 0.f;
		float potential_tax_mid = 0.f;
		float potential_tax_rich = 0.f;

		if(
			state.world.province_get_nation_from_province_ownership(province)
			==
			state.world.province_get_nation_from_province_control(province)
		) {
			for(auto pl : state.world.province_get_pop_location(province)) {
				auto& pop_money = pl.get_pop().get_savings();
				auto strata = culture::pop_strata(pl.get_pop().get_poptype().get_strata());
				if(strata == culture::pop_strata::poor) {
					potential_tax_poor += pop_money;
				} else if(strata == culture::pop_strata::middle) {
					potential_tax_mid += pop_money;
				} else if(strata == culture::pop_strata::rich) {
					potential_tax_rich += pop_money;
				}
				assert(std::isfinite(pl.get_pop().get_savings()));
			}
		}

		state.world.province_set_tax_base_poor(province, potential_tax_poor);
		state.world.province_set_tax_base_middle(province, potential_tax_mid);
		state.world.province_set_tax_base_rich(province, potential_tax_rich);

		total_poor_tax_base += potential_tax_poor;
		total_mid_tax_base += potential_tax_mid;
		total_rich_tax_base += potential_tax_rich;

		auto local_tax = potential_tax_poor * float(state.world.nation_get_poor_tax(n)) / 100.f
			+ potential_tax_mid * float(state.world.nation_get_middle_tax(n)) / 100.0f
			+ potential_tax_rich * float(state.world.nation_get_rich_tax(n)) / 100.0f;

		collected_tax += local_tax * tax_multiplier;

		// apply actual tax

		auto const poor_effect = 1.f - float(state.world.nation_get_poor_tax(n)) / 100.0f * tax_multiplier;
		auto const middle_effect = 1.f - float(state.world.nation_get_middle_tax(n)) / 100.0f * tax_multiplier;
		auto const rich_effect = 1.f - float(state.world.nation_get_rich_tax(n)) / 100.0f * tax_multiplier;

		assert(poor_effect >= 0 && middle_effect >= 0 && rich_effect >= 0);

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
	}

	state.world.nation_set_total_rich_income(n, total_rich_tax_base);
	state.world.nation_set_total_middle_income(n, total_mid_tax_base);
	state.world.nation_set_total_poor_income(n, total_poor_tax_base);

	

	assert(std::isfinite(collected_tax));
	assert(collected_tax >= 0);
	state.world.nation_get_stockpiles(n, money) += collected_tax;
}

tax_information explain_tax_income_local(sys::state& state, dcon::nation_id n, dcon::province_id province) {
	tax_information result{ };
	result.local_multiplier = tax_collection_rate(state, n, province);
	result.poor_potential += state.world.province_get_tax_base_poor(province);
	result.mid_potential += state.world.province_get_tax_base_middle(province);
	result.rich_potential += state.world.province_get_tax_base_rich(province);
	result.poor = result.poor_potential * float(state.world.nation_get_poor_tax(n)) / 100.0f * result.local_multiplier;
	result.mid = result.mid_potential * float(state.world.nation_get_middle_tax(n)) / 100.0f * result.local_multiplier;
	result.rich = result.rich_potential * float(state.world.nation_get_rich_tax(n)) / 100.0f * result.local_multiplier;
	return result;
}

tax_information explain_tax_income(sys::state& state, dcon::nation_id n) {
	tax_information result{ };

	float total_poor_tax_base = 0.0f;
	float total_mid_tax_base = 0.0f;
	float total_rich_tax_base = 0.0f;

	auto collected_tax_poor = 0.f;
	auto collected_tax_mid = 0.f;
	auto collected_tax_rich = 0.f;

	result.local_multiplier = 0.f;

	for(auto so : state.world.nation_get_province_ownership(n)) {
		auto province = so.get_province();
		auto info = explain_tax_income_local(state, n, province);
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
