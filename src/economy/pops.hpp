#pragma once

#include "system_state.hpp"
#include "commodities.hpp"
#include "advanced_province_buildings.hpp"

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
		ve::select(
			nation_allows_investment && capitalists_mask,
			invest_ratio_capitalists + state.defines.alice_invest_capitalist,
			0.0f
		)
		+ ve::select(
			nation_allows_investment && landowners_mask,
			invest_ratio_landowners + state.defines.alice_invest_aristocrat,
			0.0f
		)
		+ ve::select(
			nation_allows_investment && middle_class_investors_mask,
			invest_ratio_middle_class + state.defines.alice_invest_middle_class,
			0.0f
		)
		+ ve::select(
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
		ve::select(
			capitalists_mask,
			bank_saving_ratio_capitalists + state.defines.alice_save_capitalist,
			0.0f
		)
		+ ve::select(
			landowners_mask,
			bank_saving_ratio_landowners + state.defines.alice_save_aristocrat,
			0.0f
		)
		+ ve::select(
			middle_class_investors_mask,
			bank_saving_ratio_middle_class + state.defines.alice_save_middle_class,
			0.0f
		)
		+ ve::select(
			farmers_mask,
			bank_saving_ratio_farmers + state.defines.alice_save_farmer,
			0.0f
		);

	return bank_saving_ratio;
}

namespace adaptive_ve {
template<typename VALUE>
VALUE min(VALUE a, VALUE b) {
	if constexpr(std::same_as<VALUE, float>) {
		return std::min(a, b);
	} else {
		return ve::min(a, b);
	}
}
template<typename VALUE>
VALUE max(VALUE a, VALUE b) {
	if constexpr(std::same_as<VALUE, float>) {
		return std::max(a, b);
	} else {
		return ve::max(a, b);
	}
}
}

template<typename POPS>
auto inline prepare_pop_budget(
	const sys::state& state, POPS ids
) {
	using VALUE = std::conditional<std::same_as<POPS, dcon::pop_id>, float, ve::fp_vector>::type;
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


	// cultural stuff
	auto culture = state.world.pop_get_culture(ids);
	auto accepted = ve::apply(
		[&](auto n, auto c) { return state.world.nation_get_accepted_cultures(n, c); }, nations, culture
	);
	result.can_use_free_services = ve::select((culture == state.world.nation_get_primary_culture(nations)) || accepted, 1.f, 0.f);
	

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
	total_spending_ratio = ve::select(total_spending_ratio < 1.f, 1.f, total_spending_ratio);

	// normalize:

	life_spending_ratio			= life_spending_ratio			/ total_spending_ratio;
	everyday_spending_ratio		= everyday_spending_ratio		/ total_spending_ratio;
	luxury_spending_ratio		= luxury_spending_ratio			/ total_spending_ratio;
	education_spending_ratio	= education_spending_ratio		/ total_spending_ratio;
	investment_ratio			= investment_ratio				/ total_spending_ratio;
	banking_ratio				= banking_ratio					/ total_spending_ratio;

	// set actual budgets

	VALUE spend_on_life_needs		= life_spending_ratio		* savings;
	VALUE spend_on_everyday_needs	= everyday_spending_ratio	* savings;
	VALUE spend_on_luxury_needs		= luxury_spending_ratio		* savings;
	VALUE spend_on_education		= education_spending_ratio	* savings;
	VALUE spend_on_investments		= investment_ratio			* savings;
	VALUE spend_on_bank_savings		= banking_ratio				* savings;

	// upload data to structure
	// here we do logic which can't be made uniform


	// ##########
	// life needs
	// ##########

	VALUE old_life = pop_demographics::get_life_needs(state, ids);
	VALUE subsistence = adjusted_subsistence_score<VALUE, decltype(provs)>(state, provs);
	auto rgo_worker = state.world.pop_type_get_is_paid_rgo_worker(pop_type);
	subsistence = ve::select(rgo_worker, subsistence, 0.f);
	VALUE available_subsistence = adaptive_ve::min<VALUE>(subsistence_score_life, subsistence);
	subsistence = subsistence - available_subsistence;
	VALUE qol_from_subsistence = available_subsistence / subsistence_score_life;
	VALUE demand_scale_life = old_life / base_qol;
	result.life_needs.demand_scale = demand_scale_life * demand_scale_life;
	result.life_needs.required =
		result.life_needs.demand_scale
		* life_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	result.life_needs.spent = adaptive_ve::min<VALUE>(spend_on_life_needs, result.life_needs.required);
	auto zero_life_costs = result.life_needs.required == 0;
	result.life_needs.satisfied_with_money_ratio = ve::select(
		zero_life_costs,
		1.f,
		result.life_needs.spent
		/ result.life_needs.required
	);
	// subsistence gives free "level of consumption"
	result.life_needs.satisfied_for_free_ratio = qol_from_subsistence;
	result.spent_total = result.spent_total + result.life_needs.spent;
	savings = savings - result.life_needs.spent;


	// ##############
	// everyday needs
	// ##############

	auto old_everyday = pop_demographics::get_everyday_needs(state, ids);
	auto demand_scale_everyday = old_everyday / base_qol;
	result.everyday_needs.demand_scale = demand_scale_everyday * demand_scale_everyday;
	result.everyday_needs.required =
		result.everyday_needs.demand_scale
		* everyday_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	result.everyday_needs.spent = adaptive_ve::min<VALUE>(spend_on_everyday_needs, result.everyday_needs.required);
	auto zero_everyday_costs = result.everyday_needs.required == 0;
	result.everyday_needs.satisfied_with_money_ratio = ve::select(
		zero_everyday_costs,
		1.f,
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
	result.luxury_needs.demand_scale = demand_scale_luxury * demand_scale_luxury;
	result.luxury_needs.required =
		result.luxury_needs.demand_scale
		* luxury_costs
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
	result.luxury_needs.spent = adaptive_ve::min<VALUE>(spend_on_luxury_needs, result.luxury_needs.required);
	auto zero_luxury_costs = result.luxury_needs.required == 0;
	result.luxury_needs.satisfied_for_free_ratio = 0.f;
	result.luxury_needs.satisfied_with_money_ratio = ve::select(
		zero_luxury_costs,
		1.f,
		result.luxury_needs.spent
		/ result.luxury_needs.required
	);
	result.spent_total = result.spent_total + result.luxury_needs.spent;
	savings = savings - result.luxury_needs.spent;



	// #########
	// education
	// #########

	auto literacy = pop_demographics::get_literacy(state, ids);
	result.education.demand_scale = literacy;
	auto required_education = result.education.demand_scale * pop_size;
	result.education.required = required_education * state.world.province_get_service_price(provs, services::list::education);
	auto supposed_to_spend = adaptive_ve::min<VALUE>(spend_on_education, result.education.required);

	auto potentially_free_ratio = state.world.province_get_service_satisfaction_for_free(provs, services::list::education);
	auto ratio_of_free_education = decltype(potentially_free_ratio)(0.f);	
	ratio_of_free_education = ve::select(result.can_use_free_services > 0.f, potentially_free_ratio, ratio_of_free_education);
	result.education.satisfied_for_free_ratio = ratio_of_free_education;
	result.education.spent = supposed_to_spend * (1.f - ratio_of_free_education);
	result.education.satisfied_with_money_ratio = ve::select(
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
	result.investments.spent = savings * investment_ratio;
	result.investments.demand_scale = 0.f;
	result.spent_total = result.spent_total + result.investments.spent;
	savings = savings - result.investments.spent;

	// #####
	// banks
	// #####

	result.bank_savings.required = 0.f;
	result.bank_savings.satisfied_with_money_ratio = 1.f;
	result.bank_savings.satisfied_for_free_ratio = 0.f;
	result.bank_savings.spent = savings * banking_ratio;
	result.bank_savings.demand_scale = 0.f;
	result.spent_total = result.spent_total + result.bank_savings.spent;
	savings = savings - result.bank_savings.spent;

	result.remaining_savings = savings;

	return result;
}

// MOVE TO CPP FILE!!!
void inline update_consumption(
	sys::state& state,
	ve::vectorizable_buffer<float, dcon::nation_id>& invention_count
) {
	uint32_t total_commodities = state.world.commodity_size();

	state.ui_state.last_tick_investment_pool_change = 0;

	// satisfaction buffers
	// they store how well pops satisfy their needs
	// we store them per pop now
	// because iteration per state
	// and per pop of each state is way too slow
	// we start with filling them with according subsistence values
	// and then attempt to buy the rest

	ve::fp_vector total_spendings{};

	state.world.execute_serial_over_pop([&](auto ids) {
		auto pop_size = state.world.pop_get_size(ids);

		// get all data into vectors
		vectorized_pops_budget<ve::fp_vector> data = prepare_pop_budget(state, ids);

		auto base_shift = ve::fp_vector{ 1.f / 200.f };
		auto small_shift = pop_demographics::pop_u16_scaling;

		ve::fp_vector shift_life = ve::select(
			data.life_needs.satisfied_with_money_ratio + data.life_needs.satisfied_for_free_ratio > 0.9f,
			base_shift,
			ve::select(
				data.life_needs.satisfied_with_money_ratio + data.life_needs.satisfied_for_free_ratio < 0.7f,
				-base_shift,
				ve::fp_vector{ 0.f }
			)
		);
		ve::fp_vector shift_everyday = ve::select(
			data.everyday_needs.satisfied_with_money_ratio > 0.9f,
			base_shift,
			ve::select(
				data.everyday_needs.satisfied_with_money_ratio < 0.7f,
				-base_shift,
				ve::fp_vector{ 0.f }
			)
		);
		ve::fp_vector shift_luxury = ve::select(
			data.luxury_needs.satisfied_with_money_ratio > 0.9f,
			base_shift,
			ve::select(
				data.luxury_needs.satisfied_with_money_ratio < 0.7f,
				-base_shift,
				ve::fp_vector{ 0.f }
			)
		);
		ve::fp_vector shift_literacy = ve::select(
			data.education.satisfied_with_money_ratio + data.education.satisfied_for_free_ratio > 0.9f,
			small_shift,
			ve::select(
				data.education.satisfied_with_money_ratio + data.education.satisfied_for_free_ratio < 0.7f,
				-small_shift,
				ve::fp_vector{ 0.f }
			)
		);

		// clamping for now
		auto result_life = ve::max(0.f, ve::min(1.f, pop_demographics::get_life_needs(state, ids) + shift_life));
		auto result_everyday = ve::max(0.f, ve::min(1.f, pop_demographics::get_everyday_needs(state, ids) + shift_everyday));
		auto result_luxury = ve::max(0.f, ve::min(1.f, pop_demographics::get_luxury_needs(state, ids) + shift_luxury));
		auto result_literacy = ve::max(0.f, ve::min(1.f, pop_demographics::get_literacy(state, ids) + shift_literacy));

		pop_demographics::set_life_needs(state, ids, result_life);
		pop_demographics::set_everyday_needs(state, ids, result_everyday);
		pop_demographics::set_luxury_needs(state, ids, result_luxury);
		pop_demographics::set_literacy(state, ids, result_literacy);

		auto final_demand_scale_life =
			pop_size / state.defines.alice_needs_scaling_factor
			* data.life_needs.demand_scale;

		auto final_demand_scale_everyday =
			pop_size / state.defines.alice_needs_scaling_factor
			* data.everyday_needs.demand_scale;

		auto final_demand_scale_luxury =
			pop_size / state.defines.alice_needs_scaling_factor
			* data.luxury_needs.demand_scale;

		auto final_demand_scale_education_free_allowed =
			data.can_use_free_services * data.education.demand_scale * pop_size;
		auto final_demand_scale_education_free_not_allowed =
			(1.f - data.can_use_free_services) * data.education.demand_scale * pop_size;

		auto provs = state.world.pop_get_province_from_pop_location(ids);

		ve::apply([&](
			dcon::province_id pid,
			float potentially_free,
			float paid_only
		) {
			{
				auto old_value = state.world.province_get_service_demand_allowed_public_supply(pid, services::list::education);
				state.world.province_set_service_demand_allowed_public_supply(pid, services::list::education, old_value + potentially_free);
			}
			{
				auto old_value = state.world.province_get_service_demand_forbidden_public_supply(pid, services::list::education);
				state.world.province_set_service_demand_forbidden_public_supply(pid, services::list::education, old_value + paid_only);
			}
		},	
			provs, final_demand_scale_education_free_allowed, final_demand_scale_education_free_not_allowed
		);

		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);

		ve::apply([&](
			dcon::market_id m,
			float scale_life,
			float scale_everyday,
			float scale_luxury,
			float investment,
			auto pop_type
		) {
			assert(scale_life >= 0.0f);
			assert(scale_everyday >= 0.0f);
			assert(scale_luxury >= 0.0f);
			assert(!isinf(scale_life));
			assert(!isinf(scale_everyday));
			assert(!isinf(scale_luxury));

			state.world.market_set_life_needs_scale(m, pop_type, state.world.market_get_life_needs_scale(m, pop_type) + scale_life);
			state.world.market_set_everyday_needs_scale(m, pop_type, state.world.market_get_everyday_needs_scale(m, pop_type) + scale_everyday);
			state.world.market_set_luxury_needs_scale(m, pop_type, state.world.market_get_luxury_needs_scale(m, pop_type) + scale_luxury);
			auto zone = state.world.market_get_zone_from_local_market(m);
			auto nation = state.world.state_instance_get_nation_from_state_ownership(zone);
			state.world.nation_set_private_investment(nation, state.world.nation_get_private_investment(nation) + investment);
			if(nation == state.local_player_nation) {
				state.ui_state.last_tick_investment_pool_change += investment;
			}
		},
			markets,
			final_demand_scale_life,
			final_demand_scale_everyday,
			final_demand_scale_luxury,
			data.investments.spent,
			state.world.pop_get_poptype(ids)
		);

		auto bank_deposits = data.bank_savings.spent;
		bank_deposits = ve::max(bank_deposits, 0.0f);
#ifndef NDEBUG
		ve::apply([&](float r) {
			assert(r >= 0.f);
		}, bank_deposits);
#endif
		ve::apply(
			[&](float transfer, dcon::nation_id n) {
				state.world.nation_set_national_bank(n, state.world.nation_get_national_bank(n) + transfer);
				return 0;
			}, bank_deposits, nations
		);

		// we do save savings here because a part of education is given for free
		// which leads to some part of wealth not being spent most of the time
		state.world.pop_set_savings(ids, data.remaining_savings);
		total_spendings = total_spendings + data.spent_total;
	});

#ifndef NDEBUG
	std::string debug_output = "Total pops spendings on needs: " + std::to_string(total_spendings.reduce());
	state.console_log(debug_output);
#endif // !NDEBUG

	// iterate over all (market,pop type,trade good) pairs to finalise this calculation
	state.world.execute_serial_over_market([&](auto ids) {
		auto states = state.world.market_get_zone_from_local_market(ids);
		auto nations = state.world.state_instance_get_nation_from_state_ownership(states);
		auto invention_factor = state.defines.invention_impact_on_demand * invention_count.get(nations) + 1.f;

		ve::fp_vector life_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_life_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_life_needs) + 1.0f
		};
		ve::fp_vector everyday_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
		};
		ve::fp_vector luxury_mul[3] = {
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
			state.world.nation_get_modifier_values(
				nations, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
		};

		for(const auto t : state.world.in_pop_type) {
			auto scale_life = state.world.market_get_life_needs_scale(ids, t);
			auto scale_everyday = state.world.market_get_everyday_needs_scale(ids, t);
			auto scale_luxury = state.world.market_get_luxury_needs_scale(ids, t);

			auto strata = t.get_strata();

			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

				auto life_weight =
					state.world.market_get_life_needs_weights(ids, cid);
				auto everyday_weight =
					state.world.market_get_everyday_needs_weights(ids, cid);
				auto luxury_weight =
					state.world.market_get_luxury_needs_weights(ids, cid);
				auto base_life =
					state.world.pop_type_get_life_needs(t, cid);
				auto base_everyday =
					state.world.pop_type_get_everyday_needs(t, cid);
				auto base_luxury =
					state.world.pop_type_get_luxury_needs(t, cid);

				auto valid_good_mask = valid_need(state, nations, cid);

				auto demand_life =
					base_life
					* scale_life
					* life_mul[strata]
					* life_weight
					* state.defines.alice_lf_needs_scale;
				auto demand_everyday =
					base_everyday
					* scale_everyday
					* everyday_mul[strata]
					* everyday_weight
					* state.defines.alice_ev_needs_scale
					* invention_factor;
				auto demand_luxury =
					base_luxury
					* scale_luxury
					* luxury_mul[strata]
					* luxury_weight
					* state.defines.alice_lx_needs_scale
					* invention_factor;

				demand_life = ve::select(valid_good_mask, demand_life, 0.f);
				demand_everyday = ve::select(valid_good_mask, demand_everyday, 0.f);
				demand_luxury = ve::select(valid_good_mask, demand_luxury, 0.f);

				register_demand(state, ids, cid, demand_life, economy_reason::pop);
				register_demand(state, ids, cid, demand_everyday, economy_reason::pop);
				register_demand(state, ids, cid, demand_luxury, economy_reason::pop);
			}
		}
	});
}

}


}
