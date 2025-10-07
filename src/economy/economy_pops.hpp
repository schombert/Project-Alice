#pragma once

#include "system_state.hpp"
#include "advanced_province_buildings.hpp"
#include "demographics.hpp"
#include "adaptive_ve.hpp"

namespace economy {

template<typename VALUE, typename POPS>
VALUE adjusted_subsistence_score(
	const sys::state& state,
	POPS p
) {
	return state.world.province_get_subsistence_score(p)
		* state.world.province_get_subsistence_employment(p)
		/ (state.world.province_get_demographics(p, demographics::total) + 1.f);
}

namespace pops {

template<typename VALUE>
struct vectorized_budget_position {
	VALUE required{};
	VALUE satisfied_with_money_ratio{};
	VALUE satisfied_for_free_ratio{};
	VALUE spent{};
	VALUE demand_scale{};
};

template<typename VALUE>
struct vectorized_pops_budget {
	vectorized_budget_position<VALUE> life_needs{};
	vectorized_budget_position<VALUE> everyday_needs{};
	vectorized_budget_position<VALUE> luxury_needs{};
	vectorized_budget_position<VALUE> investments{};
	vectorized_budget_position<VALUE> bank_savings{};
	vectorized_budget_position<VALUE> education{};

	VALUE can_use_free_services { };
	VALUE remaining_savings{ };
	VALUE spent_total{ };
};

float constexpr inline base_qol = 0.5f;
constexpr inline uint32_t build_factory = issue_rule::pop_build_factory;
constexpr inline uint32_t expand_factory = issue_rule::pop_expand_factory;
constexpr inline uint32_t can_invest = expand_factory | build_factory;

template<typename VALUE, typename POPS>
VALUE inline investment_rate(const sys::state& state, POPS ids) {
	using BOOL_VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, bool, ve::mask_vector>;

	auto provs = state.world.pop_get_province_from_pop_location(ids);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
	auto pop_type = state.world.pop_get_poptype(ids);
	auto nation_rules = state.world.nation_get_combined_issue_rules(nations);
	auto is_civilised = state.world.nation_get_is_civilized(nations);
	auto allows_investment_mask = (nation_rules & can_invest) != 0;
	auto nation_allows_investment = is_civilised && allows_investment_mask;

	auto capitalists_mask = pop_type == state.culture_definitions.capitalists;
	auto middle_class_investors_mask = pop_type == state.culture_definitions.artisans || pop_type == state.culture_definitions.secondary_factory_worker;
	auto farmers_mask = pop_type == state.culture_definitions.farmers;
	auto landowners_mask = pop_type == state.culture_definitions.aristocrat;

	auto invest_ratio_capitalists = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::capitalist_reinvestment);
	auto invest_ratio_landowners = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::aristocrat_reinvestment);
	auto invest_ratio_middle_class = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::middle_class_reinvestment);
	auto invest_ratio_farmers = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::farmers_reinvestment);

	auto investment_ratio =
		adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && capitalists_mask,
			invest_ratio_capitalists + state.defines.alice_invest_capitalist,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && landowners_mask,
			invest_ratio_landowners + state.defines.alice_invest_aristocrat,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && middle_class_investors_mask,
			invest_ratio_middle_class + state.defines.alice_invest_middle_class,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			nation_allows_investment && farmers_mask,
			invest_ratio_farmers + state.defines.alice_invest_farmer,
			0.0f
		);
	return investment_ratio;
}


// handle bank savings
// Note that farmers and middle_class don't do bank savings by default
// - that doens't mean they don't have savings.
// They don't use banks for savings without modifier (from tech, from example).
template<typename VALUE, typename POPS>
VALUE inline bank_saving_rate(const sys::state& state, POPS ids) {
	using BOOL_VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, bool, ve::mask_vector>;

	auto provs = state.world.pop_get_province_from_pop_location(ids);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
	auto pop_type = state.world.pop_get_poptype(ids);

	auto capitalists_mask = pop_type == state.culture_definitions.capitalists;
	auto middle_class_investors_mask = pop_type == state.culture_definitions.artisans || pop_type == state.culture_definitions.secondary_factory_worker;
	auto farmers_mask = pop_type == state.culture_definitions.farmers;
	auto landowners_mask = pop_type == state.culture_definitions.aristocrat;

	auto bank_saving_ratio_capitalists = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::capitalist_savings);
	auto bank_saving_ratio_landowners = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::aristocrat_savings);
	auto bank_saving_ratio_middle_class = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::middle_class_savings);
	auto bank_saving_ratio_farmers = state.world.nation_get_modifier_values(nations, sys::national_mod_offsets::farmers_savings);

	auto bank_saving_ratio =
		adaptive_ve::select<BOOL_VALUE, VALUE>(
			capitalists_mask,
			bank_saving_ratio_capitalists + state.defines.alice_save_capitalist,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			landowners_mask,
			bank_saving_ratio_landowners + state.defines.alice_save_aristocrat,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			middle_class_investors_mask,
			bank_saving_ratio_middle_class + state.defines.alice_save_middle_class,
			0.0f
		)
		+ adaptive_ve::select<BOOL_VALUE, VALUE>(
			farmers_mask,
			bank_saving_ratio_farmers + state.defines.alice_save_farmer,
			0.0f
		);

	return bank_saving_ratio;
}

template<typename POPS>
auto inline prepare_pop_budget(
	const sys::state& state, POPS ids
) {
	using VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, float, ve::fp_vector>;
	using BOOL_VALUE = typename std::conditional_t<std::same_as<POPS, dcon::pop_id>, bool, ve::mask_vector>;

	vectorized_pops_budget<VALUE> result{ };

	auto pop_size = state.world.pop_get_size(ids);
	auto savings = state.world.pop_get_savings(ids);
	auto provs = state.world.pop_get_province_from_pop_location(ids);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
	auto pop_type = state.world.pop_get_poptype(ids);
	auto strata = state.world.pop_type_get_strata(pop_type);

	VALUE life_costs = ve::apply(
		[&](dcon::market_id m, dcon::pop_type_id pt) {
			return state.world.market_get_life_needs_costs(m, pt);
		}, markets, pop_type
	);
	VALUE everyday_costs = ve::apply(
		[&](dcon::market_id m, dcon::pop_type_id pt) {
			return state.world.market_get_everyday_needs_costs(m, pt);
		}, markets, pop_type
	);
	VALUE luxury_costs = ve::apply(
		[&](dcon::market_id m, dcon::pop_type_id pt) {
			return state.world.market_get_luxury_needs_costs(m, pt);
		}, markets, pop_type
	);

	if constexpr(std::same_as<POPS, dcon::pop_id>) {
		result.can_use_free_services = state.world.pop_get_is_primary_or_accepted_culture(ids) ? 1.f : 0.f;
	} else {
		result.can_use_free_services = adaptive_ve::select<BOOL_VALUE, VALUE>(state.world.pop_get_is_primary_or_accepted_culture(ids), ve::fp_vector { 1.f }, ve::fp_vector { 0.f });
	}

	// we want to focus on life needs first if we are poor AND our satisfaction is low

	VALUE base_life_costs = (0.00001f + life_costs * pop_size / state.defines.alice_needs_scaling_factor);
	VALUE is_poor = adaptive_ve::max<VALUE>(0.f, 1.f - 4.f * savings / base_life_costs);
	VALUE current_life = pop_demographics::get_life_needs(state, ids);
	is_poor = adaptive_ve::min<VALUE>(1.f, adaptive_ve::max<VALUE>(0.f, is_poor + (1.f - current_life) * 2.f));

	// prepare desired spending rate for every category

	VALUE life_spending_ratio = state.defines.alice_needs_lf_spend * (1.f - is_poor) + is_poor;
	VALUE everyday_spending_ratio = state.defines.alice_needs_ev_spend * (1.f - is_poor);
	VALUE luxury_spending_ratio = state.defines.alice_needs_lx_spend * (1.f - is_poor);
	VALUE education_spending_ratio = (0.2f) * (1.f - is_poor);
	VALUE investment_ratio = adaptive_ve::max<VALUE>(investment_rate<VALUE>(state, ids), 0.0f);
	VALUE banking_ratio = adaptive_ve::max<VALUE>(bank_saving_rate<VALUE>(state, ids), 0.0f);

	VALUE total_spending_ratio =
		life_spending_ratio
		+ everyday_spending_ratio
		+ luxury_spending_ratio
		+ education_spending_ratio
		+ investment_ratio
		+ banking_ratio;

	if constexpr(std::same_as<POPS, dcon::pop_id>) {
		total_spending_ratio = total_spending_ratio < 1.f ? 1.f : total_spending_ratio;
	} else {
		total_spending_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(total_spending_ratio < 1.f, ve::fp_vector{ 1.f }, total_spending_ratio);
	}

	// normalize:

	life_spending_ratio			= life_spending_ratio			/ total_spending_ratio;
	everyday_spending_ratio			= everyday_spending_ratio		/ total_spending_ratio;
	luxury_spending_ratio			= luxury_spending_ratio			/ total_spending_ratio;
	education_spending_ratio		= education_spending_ratio		/ total_spending_ratio;
	investment_ratio			= investment_ratio			/ total_spending_ratio;
	banking_ratio				= banking_ratio				/ total_spending_ratio;

	// set actual budgets

	VALUE spend_on_life_needs		= life_spending_ratio		* savings;
	VALUE spend_on_everyday_needs		= everyday_spending_ratio	* savings;
	VALUE spend_on_luxury_needs		= luxury_spending_ratio		* savings;
	VALUE spend_on_education		= education_spending_ratio	* savings;
	VALUE spend_on_investments		= investment_ratio		* savings;
	VALUE spend_on_bank_savings		= banking_ratio			* savings;

	// upload data to structure
	// here we do logic which can't be made uniform


	// ##########
	// life needs
	// ##########

	VALUE old_life = pop_demographics::get_life_needs(state, ids);
	VALUE subsistence = adjusted_subsistence_score<VALUE, decltype(provs)>(state, provs);
	BOOL_VALUE rgo_worker = state.world.pop_type_get_is_paid_rgo_worker(pop_type);
	subsistence = adaptive_ve::select<BOOL_VALUE, VALUE>(rgo_worker, subsistence, 0.f);
	VALUE available_subsistence = adaptive_ve::min<VALUE>(subsistence_score_life, subsistence);
	subsistence = subsistence - available_subsistence;
	VALUE qol_from_subsistence = available_subsistence / subsistence_score_life;
	VALUE demand_scale_life = old_life / base_qol;
	result.life_needs.demand_scale = demand_scale_life * demand_scale_life + 0.01f;
	result.life_needs.required =
		result.life_needs.demand_scale
		* life_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	auto zero_life_costs = result.life_needs.required == 0;
	auto rich_but_life_needs_are_not_satisfied = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_life_costs,
		1.f,
		adaptive_ve::min<VALUE>
			(
				1000.f,
				(1.f - old_life) * adaptive_ve::max<VALUE>(0.f, spend_on_life_needs - result.life_needs.required * 5.f) / result.life_needs.required
			)
	);
	result.life_needs.spent = adaptive_ve::min<VALUE>(spend_on_life_needs, result.life_needs.required * (1.f + rich_but_life_needs_are_not_satisfied));
	result.life_needs.satisfied_with_money_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_life_costs,
		10.f,
		result.life_needs.spent
		/ result.life_needs.required
	);
	// subsistence gives free "level of consumption"
	result.life_needs.satisfied_for_free_ratio = qol_from_subsistence / (1.f + result.life_needs.demand_scale);
	result.spent_total = result.spent_total + result.life_needs.spent;
	savings = savings - result.life_needs.spent;


	// ##############
	// everyday needs
	// ##############

	auto old_everyday = pop_demographics::get_everyday_needs(state, ids);
	auto demand_scale_everyday = old_everyday / base_qol;
	result.everyday_needs.demand_scale = demand_scale_everyday * demand_scale_everyday + 0.01f;
	result.everyday_needs.required =
		result.everyday_needs.demand_scale
		* everyday_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	auto zero_everyday_costs = result.everyday_needs.required == 0;
	auto rich_but_everyday_needs_are_not_satisfied = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_everyday_costs,
		1.f,
		adaptive_ve::min<VALUE>
		(
			1000.f,
			(1.f - old_everyday) * adaptive_ve::max<VALUE>(0.f, spend_on_everyday_needs - result.everyday_needs.required * 5.f) / result.everyday_needs.required
		)
	);
	result.everyday_needs.spent = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_everyday_needs, result.everyday_needs.required * (1.f + rich_but_everyday_needs_are_not_satisfied)));
	result.everyday_needs.satisfied_with_money_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_everyday_costs,
		10.f,
		result.everyday_needs.spent
		/ result.everyday_needs.required
	);
	result.everyday_needs.satisfied_for_free_ratio = 0.f;
	result.spent_total = result.spent_total + result.everyday_needs.spent;
	savings = savings - result.everyday_needs.spent;



	// ############
	// luxury needs
	// ############

	auto old_luxury = pop_demographics::get_luxury_needs(state, ids);
	auto demand_scale_luxury = old_luxury / base_qol;
	result.luxury_needs.demand_scale = demand_scale_luxury * demand_scale_luxury + 0.01f;
	result.luxury_needs.required =
		result.luxury_needs.demand_scale
		* luxury_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	auto zero_luxury_costs = result.luxury_needs.required == 0;
	auto rich_but_luxury_needs_are_not_satisfied = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_luxury_costs,
		1.f,
		adaptive_ve::min<VALUE>
		(
			1000.f,
			(1.f - old_luxury) * adaptive_ve::max<VALUE>(0.f, spend_on_luxury_needs - result.luxury_needs.required * 5.f) / result.luxury_needs.required
		)
	);
	result.luxury_needs.spent = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_luxury_needs, result.luxury_needs.required * (1.f + rich_but_luxury_needs_are_not_satisfied)));
	result.luxury_needs.satisfied_for_free_ratio = 0.f;
	result.luxury_needs.satisfied_with_money_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(
		zero_luxury_costs,
		10.f,
		result.luxury_needs.spent
		/ result.luxury_needs.required
	);
	result.spent_total = result.spent_total + result.luxury_needs.spent;
	savings = savings - result.luxury_needs.spent;



	// #########
	// education
	// #########

	auto literacy = pop_demographics::get_literacy(state, ids);
	result.education.demand_scale = literacy * literacy / 0.5f + 0.1f;
	auto required_education = result.education.demand_scale * pop_size;
	result.education.required = required_education * state.world.province_get_service_price(provs, services::list::education);
	auto rich_but_uneducated = adaptive_ve::select<BOOL_VALUE, VALUE>(
		required_education == 0.f,
		1.f,
		adaptive_ve::min<VALUE>
		(
			1000.f,
			(1.f - literacy) * adaptive_ve::max<VALUE>(0.f, spend_on_education - result.education.required * 5.f) / result.education.required
		)
	);
	auto supposed_to_spend = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_education, result.education.required * (1.f + rich_but_uneducated)));

	auto potentially_free_ratio = state.world.province_get_service_satisfaction_for_free(provs, services::list::education) / (1.f + rich_but_uneducated);
	auto ratio_of_free_education = decltype(potentially_free_ratio)(0.f);
	ratio_of_free_education = adaptive_ve::select<BOOL_VALUE, VALUE>(result.can_use_free_services > 0.f, potentially_free_ratio, ratio_of_free_education);
	result.education.satisfied_for_free_ratio = ratio_of_free_education;
	result.education.spent = supposed_to_spend * (1.f - ratio_of_free_education);
	result.education.satisfied_with_money_ratio = adaptive_ve::select<BOOL_VALUE, VALUE>(
		required_education == 0.f,
		1.f,
		result.education.spent
		/ result.education.required
	);
	result.spent_total = result.spent_total + result.education.spent;
	savings = savings - result.education.spent;



	// ###########
	// investments
	// ###########

	result.investments.required = 0.f;
	result.investments.satisfied_with_money_ratio = 1.f;
	result.investments.satisfied_for_free_ratio = 0.f;
	result.investments.spent = spend_on_investments;
	result.investments.demand_scale = 0.f;
	result.spent_total = result.spent_total + result.investments.spent;
	savings = savings - result.investments.spent;

	// #####
	// banks
	// #####

	result.bank_savings.required = 0.f;
	result.bank_savings.satisfied_with_money_ratio = 1.f;
	result.bank_savings.satisfied_for_free_ratio = 0.f;
	result.bank_savings.spent = spend_on_bank_savings;
	result.bank_savings.demand_scale = 0.f;
	result.spent_total = result.spent_total + result.bank_savings.spent;
	savings = savings - result.bank_savings.spent;

	result.remaining_savings = savings;

	return result;
}

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
}

float estimate_pops_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p);

}
