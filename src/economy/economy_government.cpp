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

float full_spendings_administration(sys::state& state, dcon::nation_id n, float budget) {
	auto admin_count = count_active_administrations(state, n);
	if(admin_count == 0.f) {
		return 0.f;
	}

	auto admin_budget = budget * float(state.world.nation_get_administrative_spending(n)) / 100.f;
	// prevent floating point errors
	admin_budget = std::min(budget, admin_budget);

	return admin_budget;
}

void update_consumption_administration(sys::state& state, dcon::nation_id n, float total_budget) {
	// admin budget is not scaled down

	auto admin_budget = total_budget * float(state.world.nation_get_administrative_spending(n)) / 100.f;

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
			auto& cur_control_scale = state.world.province_get_control_scale(capital);
			state.world.province_set_control_scale(capital, cur_control_scale + demand * sat * population_per_admin(state, n) + 10'000.f);
			state.world.nation_set_administration_employment_target_in_capital(n, demand);
			auto& cur_labor_demand = state.world.province_get_labor_demand(capital_of_capital_state, economy::labor::high_education_and_accepted);
			state.world.province_set_labor_demand(capital_of_capital_state, economy::labor::high_education_and_accepted, cur_labor_demand + demand);
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
		auto& cur_control_scale = state.world.province_get_control_scale(capital);
		state.world.province_set_control_scale(capital, cur_control_scale + std::max(0.f, (demand * sat - base_admin_employment) * population_per_admin(state, n) * local_administration_efficiency));
		state.world.province_set_administration_employment_target(capital_of_capital_state, demand);
		auto& cur_labor_demand = state.world.province_get_labor_demand(capital_of_capital_state, economy::labor::high_education_and_accepted);
		state.world.province_set_labor_demand(capital_of_capital_state, economy::labor::high_education_and_accepted, cur_labor_demand + demand);
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
					pl.get_pop().set_savings(pop_money * poor_effect);
				} else if(strata == culture::pop_strata::middle) {
					pl.get_pop().set_savings(pop_money * middle_effect);
				} else if(strata == culture::pop_strata::rich) {
					pl.get_pop().set_savings(pop_money * rich_effect);
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
	auto& cur_money = state.world.nation_get_stockpiles(n, money);
	state.world.nation_set_stockpiles(n, money, cur_money + collected_tax);
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

bool has_active_embargo(sys::state& state, dcon::nation_id from, dcon::nation_id to) {
	// When embargo is issued, it is automatically propagated onto all subjects. Therefore, checking for direct relationship is enough

	auto rel_1 = state.world.get_unilateral_relationship_by_unilateral_pair(to, from);
	if(!rel_1) {
		return false;
	}

	return state.world.unilateral_relationship_get_embargo(rel_1);
}

// Calculate employment of local administrations in the province for the UI.
// When using, check for capital administration separately
std::vector<employment_record> explain_local_administration_employment(sys::state& state, dcon::province_id p) {
	auto n = state.world.province_get_nation_from_province_ownership(p);
	auto record = employment_record{ economy::labor::high_education_and_accepted, 0.f, 0.f, 0.f };

	for(auto admin : state.world.nation_get_nation_administration(n)) {
		if(admin.get_administration().get_capital() == p) {
			record.target_employment = state.world.province_get_administration_employment_target(p);
			record.satisfaction = state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education_and_accepted);
			record.actual_employment = record.target_employment * record.satisfaction;
			return std::vector<employment_record> {record};
		}
	}

	return std::vector<employment_record> { record };
}

// Calculate employment of the capital administration for the UI
std::vector<employment_record> explain_capital_administration_employment(sys::state& state, dcon::nation_id n) {
	auto capital = state.world.nation_get_capital(n);
	auto capital_state = state.world.province_get_state_membership(capital);
	auto capital_of_capital_state = state.world.state_instance_get_capital(capital_state);
	auto record = employment_record{ economy::labor::high_education_and_accepted, 0.f, 0.f, 0.f };

	record.target_employment = state.world.nation_get_administration_employment_target_in_capital(n);
	record.satisfaction = state.world.province_get_labor_demand_satisfaction(capital, economy::labor::high_education_and_accepted);
	record.actual_employment = record.target_employment * record.satisfaction;
	
	return std::vector<employment_record> {record};
}

}
