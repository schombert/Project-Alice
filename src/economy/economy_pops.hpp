#pragma once

#include "system_state.hpp"
#include "advanced_province_buildings.hpp"
#include "demographics.hpp"
#include "adaptive_ve.hpp"
#include "economy_templates_pure.hpp"

namespace economy {

namespace pops {

vectorized_pops_budget<float> prepare_pop_budget(const sys::state& state, dcon::pop_id ids);

void update_consumption(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::nation_id>& invention_count,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_life,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_everyday,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_luxury,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_education_private,
	ve::vectorizable_buffer<float, dcon::pop_id>& buffer_education_public,
	ve::vectorizable_buffer<float, dcon::pop_id>&,
	ve::vectorizable_buffer<float, dcon::pop_id>&,
	ve::vectorizable_buffer<float, dcon::pop_id>&,
	ve::vectorizable_buffer<float, dcon::pop_id>&
);
void update_income_artisans(sys::state& state);
void update_income_national_subsidy(sys::state& state);
void update_income_wages(sys::state& state);
void update_income_trade(sys::state& state);

struct labor_ratio_wage {
	int32_t labor_type;
	float ratio;
	float wage;
};

struct money_from_nation {
	float pension;
	float unemployment;
	float military;
	float investment;
};

std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, bool accepted, float size);
std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::pop_id pop);
float estimate_rgo_income(sys::state const& state, dcon::pop_id pop);
float estimate_trade_income(sys::state const& state, dcon::pop_id pop);
float estimate_artisan_income(sys::state const& state, dcon::pop_id pop);
money_from_nation estimate_income_from_nation(sys::state const& state, dcon::pop_id pop);
float estimate_trade_spending(sys::state const& state, dcon::pop_id pop);
float estimate_tax_spending(sys::state const& state, dcon::pop_id pop, float tax_rate);
float estimate_pop_spending_life(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid);
float estimate_pop_spending_everyday(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid);
float estimate_pop_spending_luxury(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid);
}

float estimate_pops_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p);

}
