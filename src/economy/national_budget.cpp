#include "system_state.hpp"
#include "national_budget.hpp"
#include "economy_government.hpp"
#include "economy.hpp"
#include "advanced_province_buildings.hpp"
#include "construction.hpp"
#include "demographics.hpp"

namespace economy {

namespace national_budget {

float budget_ratio(float budget, float priority) {
	return budget * priority;
}

float estimate_education_spending(sys::state& state, dcon::nation_id n, float budget) {
	auto& def = advanced_province_buildings::definitions[advanced_province_buildings::list::schools_and_universities];
	auto total = 0.f;
	state.world.nation_for_each_province_ownership(n, [&](auto ownership) {
		auto p = state.world.province_ownership_get_province(ownership);
		auto total_population = state.world.nation_get_demographics(n, demographics::primary_or_accepted);
		auto local_population = state.world.province_get_demographics(p, demographics::primary_or_accepted);
		auto weight = total_population == 0.f ? 0.f : local_population / total_population;
		auto local_education_budget = weight * budget;
		total = total + std::max(0.f, local_education_budget * state.world.province_get_labor_demand_satisfaction(p, def.throughput_labour_type));
	});
	return total;
}

float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in) {
	auto total = 0.f;
	state.world.nation_for_each_state_ownership(n, [&](auto soid) {
		auto local_state = state.world.state_ownership_get_state(soid);
		auto market = state.world.state_instance_get_market_from_local_market(local_state);
		state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
			auto adj_pop_of_type =
				state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt)) / state.defines.alice_needs_scaling_factor;

			if(adj_pop_of_type <= 0)
				return;

			auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
			if(ln_type == in) {
				total += adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
			}

			auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
			if(en_type == in) {
				total += adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
			}

			auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
			if(lx_type == in) {
				total += adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
			}
		});
	});
	return total * payouts_spending_multiplier;
}

float estimate_subsidy_spending(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_subsidies_spending(n);
}

budget_spending_details estimate_budget_detailed(sys::state& state, dcon::nation_id n, value_per_budget_category priority, float available_funds) {
	budget_spending_details result;

	// NO RESCALING

	// INTEREST

	auto interest = economy::interest_payment(state, n);
	if(interest > available_funds) {
		interest = available_funds;
	}
	result.interest.dedicated_budget = interest;
	result.interest.actual_spending = interest;
	result.interest.priority = 1.f;
	available_funds -= interest;
	if(available_funds <= 0.f) {
		return result;
	}

	// DIPLOMACY

	auto diplomacy = economy::estimate_diplomatic_expenses(state, n);
	if(diplomacy > available_funds) {
		diplomacy = available_funds;
	}
	result.diplomacy.dedicated_budget = diplomacy;
	result.diplomacy.actual_spending = diplomacy;
	result.diplomacy.priority = 1.f;
	available_funds -= diplomacy;
	if(available_funds <= 0.f) {
		return result;
	}

	// ADMIN

	auto admin_budget = available_funds * priority.administration_wages;
	result.administration_wages.dedicated_budget = admin_budget;
	result.administration_wages.actual_spending = economy::estimate_spendings_administration(state, n, admin_budget);
	result.administration_wages.priority = priority.administration_wages;
	if(admin_budget >= available_funds) {
		return result;
	}

	// SCALED SPENDING

	auto social_budget = budget_ratio(available_funds, priority.social);
	auto const p_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::pension_level));
	auto const unemp_level = std::max(0.f, state.world.nation_get_modifier_values(n, sys::national_mod_offsets::unemployment_benefit));
	if(p_level + unemp_level == 0.f) {
		social_budget = 0.f;
	}
	auto military_budget = estimate_pop_payouts_by_income_type(state, n, culture::income_type::military) * priority.military_wages;
	auto domestic_investment = budget_ratio(available_funds, priority.domestic_investments);
	auto education_budget = estimate_education_spending(state, n, available_funds * priority.education_wages);
	auto overseas_budget = economy::estimate_overseas_penalty_spending(state, n) * priority.overseas_penalty;
	auto subsidy_budget = budget_ratio(available_funds, priority.subsidy);
	auto construction_budget = economy::estimate_construction_spending_from_budget(state, n, budget_ratio(available_funds, priority.construction_supplies));
	auto land_budget = economy::estimate_land_spending(state, n) * priority.military_supplies_land;
	auto naval_budget = economy::estimate_naval_spending(state, n) * priority.military_supplies_navy;
	auto stockpile_budget = economy::estimate_stockpile_filling_spending(state, n) * priority.stockpile;

	auto total =
		social_budget
		+ military_budget
		+ domestic_investment
		+ education_budget
		+ overseas_budget
		+ subsidy_budget
		+ construction_budget
		+ land_budget
		+ naval_budget
		+ stockpile_budget;

	auto scale = 1.f;
	if(total > available_funds) {
		auto divisor = (available_funds - admin_budget);
		if(divisor == 0.f) {
			scale = 0.f;
		} else {
			scale = (available_funds - admin_budget) / divisor;
		}
	}

	scale = std::clamp(scale, 0.f, 1.f);

	result.social.actual_spending = social_budget * scale;
	result.social.dedicated_budget = social_budget;
	result.social.priority = priority.social;

	result.military_wages.actual_spending = military_budget * scale;
	result.military_wages.dedicated_budget = military_budget;
	result.military_wages.priority = priority.military_wages;

	result.domestic_investments.actual_spending = domestic_investment * scale;
	result.domestic_investments.dedicated_budget = domestic_investment;
	result.domestic_investments.priority = priority.domestic_investments;

	result.education_wages.actual_spending = education_budget * scale;
	result.education_wages.dedicated_budget = education_budget;
	result.education_wages.priority = priority.education_wages;

	result.overseas_penalty.actual_spending = overseas_budget * scale;
	result.overseas_penalty.dedicated_budget = overseas_budget;
	result.overseas_penalty.priority = priority.overseas_penalty;

	result.subsidy.actual_spending = subsidy_budget * scale;
	result.subsidy.dedicated_budget = subsidy_budget;
	result.subsidy.priority = priority.subsidy;

	result.construction_supplies.actual_spending = construction_budget * scale;
	result.construction_supplies.dedicated_budget = construction_budget;
	result.construction_supplies.priority = priority.construction_supplies;

	result.military_supplies_land.actual_spending = land_budget * scale;
	result.military_supplies_land.dedicated_budget = land_budget;
	result.military_supplies_land.priority = priority.military_supplies_land;

	result.military_supplies_navy.actual_spending = naval_budget * scale;
	result.military_supplies_navy.dedicated_budget = naval_budget;
	result.military_supplies_navy.priority = priority.military_supplies_navy;

	result.stockpile.actual_spending = stockpile_budget * scale;
	result.stockpile.dedicated_budget = stockpile_budget;
	result.stockpile.priority = priority.stockpile;

	result.total_actual_spending =
		total * scale
		+ result.administration_wages.actual_spending
		+ result.diplomacy.actual_spending
		+ result.interest.actual_spending;

	return result;
}

value_per_budget_category get_current_priority(sys::state& state, dcon::nation_id n) {
	value_per_budget_category priority{};

	priority.interest = 1.f;
	priority.diplomacy = 1.f;
	priority.administration_wages = (float)(state.world.nation_get_administrative_spending(n)) / 100.f;
	priority.social = (float)(state.world.nation_get_social_spending(n)) / 100.f;
	priority.military_wages = (float)(state.world.nation_get_military_spending(n)) / 100.f;
	priority.domestic_investments = (float)(state.world.nation_get_domestic_investment_spending(n)) / 100.0f;
	priority.education_wages = (float)(state.world.nation_get_education_spending(n)) / 100.f;
	priority.overseas_penalty = (float)(state.world.nation_get_overseas_spending(n)) / 100.f;
	priority.subsidy = (float)(state.world.nation_get_subsidies_spending(n)) / 100.f;
	priority.construction_supplies = (float)(state.world.nation_get_construction_spending(n)) / 100.f;
	priority.military_supplies_land = (float)(state.world.nation_get_land_spending(n)) / 100.f;
	priority.military_supplies_navy = (float)(state.world.nation_get_naval_spending(n)) / 100.f;
	priority.stockpile = 1.f;

	return priority;
}

budget_spending_details estimate_budget_detailed(sys::state& state, dcon::nation_id n, float available_funds) {
	return estimate_budget_detailed(state, n, get_current_priority(state, n), available_funds);
}


}

}
