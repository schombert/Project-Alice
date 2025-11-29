#pragma once
#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"
#include "culture.hpp"

namespace economy {

namespace national_budget {

float estimate_pop_payouts_by_income_type(sys::state& state, dcon::nation_id n, culture::income_type in);

struct budget_spending_category {
	float dedicated_budget = 0.f;
	float actual_spending = 0.f;
	float priority = 0.f;
};

struct value_per_budget_category {
	float interest;
	float administration_wages;
	float diplomacy;
	float social;
	float military_wages;
	float education_wages;
	float domestic_investments;
	float overseas_penalty;
	float subsidy;
	float construction_supplies;
	float military_supplies_land;
	float military_supplies_navy;
	float stockpile;
};

struct budget_spending_details {
	budget_spending_category interest{};
	budget_spending_category diplomacy{};
	budget_spending_category administration_wages{};
	budget_spending_category social{};
	budget_spending_category military_wages{};
	budget_spending_category education_wages{};
	budget_spending_category domestic_investments{};
	budget_spending_category overseas_penalty{};
	budget_spending_category subsidy{};
	budget_spending_category construction_supplies{};
	budget_spending_category military_supplies_land{};
	budget_spending_category military_supplies_navy{};
	budget_spending_category stockpile{};

	float total_actual_spending = 0.f;
};

budget_spending_details estimate_budget_detailed(sys::state& state, dcon::nation_id n, value_per_budget_category priority, float available_funds);
budget_spending_details estimate_budget_detailed(sys::state& state, dcon::nation_id n, float available_funds);
	
}

}
