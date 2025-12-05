#include "economy_pops.hpp"
#include "economy_production.hpp"
#include "price.hpp"
#include "province_templates.hpp"
#include "economy_templates.hpp"
#include "demographics.hpp"
#include "money.hpp"
#include "economy_constants.hpp"
#include "economy_templates_pure.hpp"

namespace economy {
namespace pops {


template<typename VALUE, typename POPS>
VALUE investment_rate(const sys::state& state, POPS ids) {
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
VALUE bank_saving_rate(const sys::state& state, POPS ids) {
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

template<typename VALUE, typename POPS>
VALUE adjusted_subsistence_score(
	const sys::state& state,
	POPS p
) {
	return state.world.province_get_subsistence_score(p)
		* state.world.province_get_subsistence_employment(p)
		/ (state.world.province_get_demographics(p, demographics::total) + 1.f);
}

template<typename POPS>
auto prepare_pop_budget_templated(
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
		result.can_use_free_services = adaptive_ve::select<BOOL_VALUE, VALUE>(state.world.pop_get_is_primary_or_accepted_culture(ids), ve::fp_vector{ 1.f }, ve::fp_vector{ 0.f });
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

	life_spending_ratio = life_spending_ratio / total_spending_ratio;
	everyday_spending_ratio = everyday_spending_ratio / total_spending_ratio;
	luxury_spending_ratio = luxury_spending_ratio / total_spending_ratio;
	education_spending_ratio = education_spending_ratio / total_spending_ratio;
	investment_ratio = investment_ratio / total_spending_ratio;
	banking_ratio = banking_ratio / total_spending_ratio;

	// set actual budgets

	VALUE spend_on_life_needs = life_spending_ratio * savings;
	VALUE spend_on_everyday_needs = everyday_spending_ratio * savings;
	VALUE spend_on_luxury_needs = luxury_spending_ratio * savings;
	VALUE spend_on_education = education_spending_ratio * savings;
	VALUE spend_on_investments = investment_ratio * savings;
	VALUE spend_on_bank_savings = banking_ratio * savings;

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

	auto education_price = state.world.province_get_service_price(provs, services::list::education);

	auto literacy = pop_demographics::get_literacy(state, ids);
	result.education.demand_scale = literacy * literacy / 0.5f + 0.1f;
	auto required_education = result.education.demand_scale * pop_size;
	result.education.required = required_education * education_price;

	// if education is crazy expensive and impossible to access, we want to spend 0 because it's hopeless


	auto rich_but_uneducated = adaptive_ve::select<BOOL_VALUE, VALUE>(
		required_education == 0.f,
		1.f,
		adaptive_ve::max<VALUE>
		(
			0.f,
			adaptive_ve::min<VALUE>
			(
				1000.f,
				(1.f - literacy)
				* adaptive_ve::max<VALUE>(0.f, spend_on_education - result.education.required * 5.f)
				/ result.education.required
			) - 0.1f
		)
	);
	auto supposed_to_spend = adaptive_ve::min<VALUE>(savings, adaptive_ve::min<VALUE>(spend_on_education, result.education.required * rich_but_uneducated));
	auto potentially_free_ratio = state.world.province_get_service_satisfaction_for_free(provs, services::list::education) / adaptive_ve::max<VALUE>(1.f, rich_but_uneducated);
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
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_life,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_everyday,
	ve::vectorizable_buffer<float, dcon::pop_id>& demand_luxury,
	ve::vectorizable_buffer<float, dcon::pop_id>& subsistence_ratio
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

	// temporary buffers for actual pop demand
	auto demand_education_public_forbidden = state.world.pop_make_vectorizable_float_buffer();
	auto demand_education_public_allowed = state.world.pop_make_vectorizable_float_buffer();

	auto to_bank = state.world.pop_make_vectorizable_float_buffer();
	auto to_investments = state.world.pop_make_vectorizable_float_buffer();

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto pop_size = state.world.pop_get_size(ids);

		// get all data into vectors
		vectorized_pops_budget<ve::fp_vector> data = prepare_pop_budget_templated(state, ids);

		// "for free" in the context of life/everyday/luxury needs means subsistence/gifts
		// so we apply it directly there, it's not registered as demand
		// "for free" in the context of education means that someone paid for you
		// so it will be registered as demand

		buffer_life.set(ids, data.life_needs.satisfied_with_money_ratio);
		buffer_everyday.set(ids, data.everyday_needs.satisfied_with_money_ratio);
		buffer_luxury.set(ids, data.luxury_needs.satisfied_with_money_ratio);
		buffer_education_private.set(ids, data.education.satisfied_with_money_ratio);
		buffer_education_public.set(ids, data.education.satisfied_for_free_ratio);
		subsistence_ratio.set(ids, data.life_needs.satisfied_for_free_ratio);

		auto multiplier = pop_size / state.defines.alice_needs_scaling_factor;

		demand_life.set(ids, multiplier * data.life_needs.demand_scale * data.life_needs.satisfied_with_money_ratio);
		demand_everyday.set(ids, multiplier * data.everyday_needs.demand_scale * data.everyday_needs.satisfied_with_money_ratio);
		demand_luxury.set(ids, multiplier * data.luxury_needs.demand_scale * data.luxury_needs.satisfied_with_money_ratio);
		demand_education_public_forbidden.set(
			ids,
			pop_size
			* data.education.demand_scale * data.education.satisfied_with_money_ratio
		);
		demand_education_public_allowed.set(
			ids,
			pop_size
			* data.can_use_free_services
			* data.education.demand_scale * data.education.satisfied_for_free_ratio
		);

		to_bank.set(ids, data.bank_savings.spent);
		to_investments.set(ids, data.investments.spent);

		// we do save savings here because a part of education is given for free
		// which leads to some part of wealth not being spent most of the time
		state.world.pop_set_savings(ids, ve::max(0.f, data.remaining_savings));
	});

	// services are at province level:

	concurrency::parallel_for(int32_t(0), state.province_definitions.first_sea_province.index(), [&](uint32_t raw_province_id) {
		auto pid = dcon::province_id{ dcon::province_id::value_base_t(raw_province_id) };
		state.world.province_for_each_pop_location(pid, [&](auto location) {
			auto pop = state.world.pop_location_get_pop(location);

			auto demand_allow_public = demand_education_public_allowed.get(pop);
			auto demand_forbid_public = demand_education_public_forbidden.get(pop);

			auto old_allow = state.world.province_get_service_demand_allowed_public_supply(pid, services::list::education);
			auto old_forbid = state.world.province_get_service_demand_forbidden_public_supply(pid, services::list::education);

			state.world.province_set_service_demand_allowed_public_supply(pid, services::list::education, old_allow + demand_allow_public);
			state.world.province_set_service_demand_forbidden_public_supply(pid, services::list::education, old_forbid + demand_forbid_public);
		});
	});

	// commodities (actually groups of commodities) are at market level

	concurrency::parallel_for(uint32_t(0), state.world.market_size(), [&](uint32_t raw_market_id) {
		auto m = dcon::market_id{ dcon::market_id::value_base_t(raw_market_id) };
		if(!state.world.market_is_valid(m)) return;
		auto zone = state.world.market_get_zone_from_local_market(m);
		province::for_each_province_in_state_instance(state, zone, [&](auto pid) {
			state.world.province_for_each_pop_location(pid, [&](auto location) {
				auto pop = state.world.pop_location_get_pop(location);
				auto pop_type = state.world.pop_get_poptype(pop);

				auto life = demand_life.get(pop);
				auto everyday = demand_everyday.get(pop);
				auto luxury = demand_luxury.get(pop);

				auto old_life = state.world.market_get_life_needs_scale(m, pop_type);
				auto old_everyday = state.world.market_get_everyday_needs_scale(m, pop_type);
				auto old_luxury = state.world.market_get_luxury_needs_scale(m, pop_type);

				state.world.market_set_life_needs_scale(m, pop_type, old_life + life);
				state.world.market_set_everyday_needs_scale(m, pop_type, old_everyday + everyday);
				state.world.market_set_luxury_needs_scale(m, pop_type, old_luxury + luxury);
			});
		});
	});

	// investments are at national level:

	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t raw_nation_id) {
		auto n = dcon::nation_id{ dcon::nation_id::value_base_t(raw_nation_id) };
		if(!state.world.nation_is_valid(n)) return;
		state.world.nation_for_each_province_ownership(n, [&](auto poid) {
			state.world.province_for_each_pop_location(state.world.province_ownership_get_province(poid), [&](auto location) {
				auto pop = state.world.pop_location_get_pop(location);

				auto investment = to_investments.get(pop);
				auto current_inv = state.world.nation_get_private_investment(n);
				state.world.nation_set_private_investment(n, current_inv + investment);

				if(n == state.local_player_nation) {
					state.ui_state.last_tick_investment_pool_change += investment;
				}

				auto banking = to_bank.get(pop);
				auto current_bank = state.world.nation_get_national_bank(n);
				state.world.nation_set_national_bank(n, current_bank + banking);
			});
		});
	});

	// iterate over all (market, pop type, trade good) triples to finalise this calculation and register demand to actual commodities
	state.world.execute_parallel_over_market([&](auto ids) {
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

				register_demand(state, ids, cid, demand_life);
				register_demand(state, ids, cid, demand_everyday);
				register_demand(state, ids, cid, demand_luxury);
			}
		}
	});
}

float estimate_artisan_income(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, float size) {
	auto const artisan_type = state.culture_definitions.artisans;
	auto key = demographics::to_key(state, artisan_type);

	if(ptid != artisan_type) {
		return 0.f;
	}

	auto artisan_profit = state.world.province_get_artisan_profit(pid);
	auto current_bank = state.world.province_get_artisan_bank(pid);
	auto total = artisan_profit + current_bank;
	auto dividents = total > 0.f ? total * 0.1f : 0.f;

	auto num_artisans = state.world.province_get_demographics(pid, key);
	auto per_artisan = num_artisans > 0.f ? dividents / num_artisans : 0.f;
	return size * per_artisan;
}

float estimate_artisan_income(sys::state const& state, dcon::pop_id pop) {
	return estimate_artisan_income(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}

void update_income_artisans(sys::state& state) {
	auto const artisan_type = state.culture_definitions.artisans;
	auto key = demographics::to_key(state, artisan_type);

	state.world.execute_parallel_over_province([&](auto pids) {
		auto artisan_profit = state.world.province_get_artisan_profit(pids);
		auto current_bank = state.world.province_get_artisan_bank(pids);
		state.world.province_set_artisan_bank(pids, current_bank + artisan_profit);
		state.world.province_set_artisan_profit(pids, 0.f);

		auto new_bank = state.world.province_get_artisan_bank(pids);
		auto dividents = ve::select(new_bank > 0.f, new_bank * 0.1f, 0.f);
		state.world.province_set_artisan_bank(pids, new_bank - dividents);

		auto num_artisans = state.world.province_get_demographics(pids, key);
		auto per_artisan = ve::select(num_artisans > 0.f, dividents / num_artisans, 0.f);

		ve::apply([&](auto province, auto payment) {
			for(auto pl : state.world.province_get_pop_location(province)) {
				if(artisan_type == pl.get_pop().get_poptype()) {
					pl.get_pop().set_savings(
						pl.get_pop().get_savings()
						+ state.inflation
						* pl.get_pop().get_size()
						* payment
					);
#ifndef NDEBUG
					assert(
						std::isfinite(pl.get_pop().get_savings())
						&& pl.get_pop().get_savings() >= 0
					);
#endif
				}
			}
		}, pids, per_artisan);
	});
}

float estimate_trade_income(sys::state const& state, dcon::market_id mid, dcon::pop_type_id ptid, float size) {
	auto const artisan_def = state.culture_definitions.artisans;
	auto artisan_key = demographics::to_key(state, artisan_def);
	auto const clerks_def = state.culture_definitions.secondary_factory_worker;
	auto clerks_key = demographics::to_key(state, clerks_def);
	auto const capis_def = state.culture_definitions.capitalists;
	auto capis_key = demographics::to_key(state, capis_def);
	auto sids = state.world.market_get_zone_from_local_market(mid);
	auto artisans = state.world.state_instance_get_demographics(sids, artisan_key);
	auto clerks = state.world.state_instance_get_demographics(sids, clerks_key);
	auto capis = state.world.state_instance_get_demographics(sids, capis_key);
	auto total = state.world.state_instance_get_demographics(sids, demographics::total);
	auto artisans_weight = state.world.state_instance_get_demographics(sids, artisan_key) / 1000.f;
	auto clerks_weight = state.world.state_instance_get_demographics(sids, clerks_key) * 100.f;
	auto capis_weight = state.world.state_instance_get_demographics(sids, capis_key) * 100'000.f;
	auto base_weight = total;
	auto total_weight = artisans_weight + clerks_weight + capis_weight + base_weight;

	if(total_weight == 0.f) {
		return 0.f;
	}

	auto balance = state.world.market_get_stockpile(mid, economy::money);
	auto trade_dividents = balance > 0.f ? balance * 0.001f : 0.f;

	auto artisans_share = artisans_weight / total_weight * trade_dividents;
	auto clerks_share = clerks_weight / total_weight * trade_dividents;
	auto capis_share = capis_weight / total_weight * trade_dividents;
	auto other_share = base_weight / total_weight * trade_dividents;

	auto per_artisan = artisans > 0.f ? artisans_share / artisans : 0.f;
	auto per_clerk = clerks > 0.f ? clerks_share / clerks : 0.f;
	auto per_capi = capis > 0.f ? capis_share / capis : 0.f;
	auto per_person = total > 0.f ? other_share / total : 0.f;

	if(artisan_def == ptid) {
		return state.inflation * size * (per_artisan + per_person);
	} else if(clerks_def == ptid) {
		return state.inflation * size * (per_clerk + per_person);
	} else if(capis_def == ptid) {
		return state.inflation * size * (per_capi + per_person);
	}

	return state.inflation * size * per_person;
}

float estimate_trade_income(sys::state const& state, dcon::pop_id pop) {
	auto provs = state.world.pop_get_province_from_pop_location(pop);
	auto states = state.world.province_get_state_membership(provs);
	auto markets = state.world.state_instance_get_market_from_local_market(states);

	return estimate_trade_income(
		state,
		markets,
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}

void update_income_trade(sys::state& state) {
	auto const artisan_def = state.culture_definitions.artisans;
	auto artisan_key = demographics::to_key(state, artisan_def);
	auto const clerks_def = state.culture_definitions.secondary_factory_worker;
	auto clerks_key = demographics::to_key(state, clerks_def);
	auto const capis_def = state.culture_definitions.capitalists;
	auto capis_key = demographics::to_key(state, capis_def);


	state.world.execute_parallel_over_market([&](auto markets) {
		/* {
			// we forgive a part of the debt to avoid monetary death spirals:
			auto current = state.world.market_get_stockpile(markets, economy::money);
			state.world.market_set_stockpile(markets, economy::money, current * 0.9f);
		}*/

		auto sids = state.world.market_get_zone_from_local_market(markets);

		auto artisans = state.world.state_instance_get_demographics(sids, artisan_key);
		auto clerks = state.world.state_instance_get_demographics(sids, clerks_key);
		auto capis = state.world.state_instance_get_demographics(sids, capis_key);
		auto total = state.world.state_instance_get_demographics(sids, demographics::total);

		auto artisans_weight = artisans;
		auto clerks_weight = clerks * 100.f;
		auto capis_weight = capis * 100'000.f;
		auto base_weight = total;

		auto total_weight = artisans_weight + clerks_weight + capis_weight + base_weight;

		auto balance = state.world.market_get_stockpile(markets, economy::money);
		auto trade_dividents = ve::select(balance > 0.f, balance * 0.001f, ve::fp_vector{ 0.f });
		state.world.market_set_stockpile(markets, economy::money, balance - trade_dividents);

		auto artisans_share = artisans_weight / total_weight * trade_dividents;
		auto clerks_share = clerks_weight / total_weight * trade_dividents;
		auto capis_share = capis_weight / total_weight * trade_dividents;
		auto other_share = base_weight / total_weight * trade_dividents;

		auto per_artisan = ve::select(artisans > 0.f, artisans_share / artisans, ve::fp_vector{ 0.f });
		auto per_clerk = ve::select(clerks > 0.f, clerks_share / clerks, ve::fp_vector{ 0.f });
		auto per_capi = ve::select(capis > 0.f, capis_share / capis, ve::fp_vector{ 0.f });
		auto per_person = ve::select(total > 0.f, other_share / total, ve::fp_vector{ 0.f });

		// proceeed payments:

		ve::apply([&](auto sid, auto to_artisan, auto to_clerk, auto to_capi, auto to_everyone) {
			if(to_artisan == 0.f && to_clerk == 0.f && to_capi == 0.f && to_everyone == 0.f) {
				return;
			}
			province::for_each_province_in_state_instance(state, sid, [&](dcon::province_id p) {
				for(auto pl : state.world.province_get_pop_location(p)) {
					auto pop = pl.get_pop();
					auto savings = pop.get_savings();
					auto size = pop.get_size();

					if(artisan_def == pl.get_pop().get_poptype()) {
						pop.set_savings(savings + state.inflation * size * (to_artisan + to_everyone));
#ifndef NDEBUG
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
					} else if(clerks_def == pl.get_pop().get_poptype()) {
						pop.set_savings(savings + state.inflation * size * (to_clerk + to_everyone));
#ifndef NDEBUG
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
					} else if(capis_def == pl.get_pop().get_poptype()) {
						pop.set_savings(savings + state.inflation * size * (to_capi + to_everyone));
#ifndef NDEBUG
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
					} else {
						pop.set_savings(savings + state.inflation * size * to_everyone);
#ifndef NDEBUG
						assert(std::isfinite(pl.get_pop().get_savings()) && pl.get_pop().get_savings() >= 0);
#endif
					}


				}
			});
		}, sids, per_artisan, per_clerk, per_capi, per_person);
	});
}


money_from_nation estimate_income_from_nation(sys::state const& state, dcon::pop_id pop) {
	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);

	auto prov = state.world.pop_get_province_from_pop_location(pop);
	auto owner = state.world.province_get_nation_from_province_ownership(prov);
	auto population = state.world.nation_get_demographics(owner, demographics::total);
	auto unemployed = population - state.world.nation_get_demographics(owner, demographics::employed);
	auto capitalists = state.world.nation_get_demographics(owner, capitalists_key);
	auto aristocrats = state.world.nation_get_demographics(owner, aristocracy_key);
	auto investors = capitalists + aristocrats;

	auto states = state.world.province_get_state_membership(prov);
	auto markets = state.world.state_instance_get_market_from_local_market(states);
	auto owner_spending = state.world.nation_get_spending_level(owner);

	auto size = state.world.pop_get_size(pop);
	auto adj_size = size / state.defines.alice_needs_scaling_factor;

	auto budget = state.world.nation_get_last_base_budget(owner);

	auto social_budget =
		owner_spending
		* budget
		* float(state.world.nation_get_social_spending(owner))
		/ 100.f;

	auto investment_budget =
		owner_spending
		* budget
		* float(state.world.nation_get_domestic_investment_spending(owner))
		/ 100.f;

	auto const p_level = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::pension_level);
	auto const unemp_level = state.world.nation_get_modifier_values(owner, sys::national_mod_offsets::unemployment_benefit);

	auto pension_ratio = p_level * population > 0.f ? p_level * population / (p_level * population + unemp_level * unemployed) : 0.f;
	auto unemployment_ratio = unemp_level * unemployed > 0.f ? unemp_level * unemployed / (p_level * population + unemp_level * unemployed) : 0.f;

	auto const pension_per_person =
		pension_ratio
		* social_budget
		/ (population + 1.f);

	auto const benefits_per_person =
		unemployment_ratio
		* social_budget
		/ (unemployed + 1.f);

	auto const payment_per_investor =
		investment_budget
		/ (investors + 1.f);

	auto const m_spending = owner_spending * float(state.world.nation_get_military_spending(owner)) / 100.0f;

	auto types = state.world.pop_get_poptype(pop);

	auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
	auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
	auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

	auto ln_costs = state.world.market_get_life_needs_costs(markets, types);
	auto en_costs = state.world.market_get_everyday_needs_costs(markets, types);
	auto lx_costs = state.world.market_get_luxury_needs_costs(markets, types);

	auto total_costs = ln_costs + en_costs + lx_costs;

	auto is_military_requires_life_needs = ln_types == int32_t(culture::income_type::military);
	auto is_military_requires_everyday_needs = en_types == int32_t(culture::income_type::military);
	auto is_military_requires_luxury_needs = lx_types == int32_t(culture::income_type::military);
	auto is_military = is_military_requires_life_needs || is_military_requires_everyday_needs || is_military_requires_luxury_needs;
	auto is_investor = (types == state.culture_definitions.capitalists) || (types == state.culture_definitions.aristocrat);

	auto mil_pay = 0.f;
	mil_pay += is_military_requires_life_needs ? m_spending * adj_size * ln_costs * payouts_spending_multiplier : 0.0f;
	mil_pay += is_military_requires_everyday_needs ? m_spending * adj_size * en_costs * payouts_spending_multiplier : 0.0f;
	mil_pay += is_military_requires_luxury_needs ? m_spending * adj_size * lx_costs * payouts_spending_multiplier : 0.0f;

	return {
		.pension = pension_per_person * size,
		.unemployment = is_military ? 0.f : benefits_per_person * (size - pop_demographics::get_employment(state, pop)),
		.military = mil_pay,
		.investment = size * price_properties::labor::min * 0.05f + (is_investor ? payment_per_investor * size : 0.f)
	};
}

void update_income_national_subsidy(sys::state& state){
	auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
	auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);

	state.world.execute_parallel_over_pop([&](auto ids) {
		auto provs = state.world.pop_get_province_from_pop_location(ids);
		auto owners = state.world.province_get_nation_from_province_ownership(provs);
		auto population = state.world.nation_get_demographics(owners, demographics::total);
		auto unemployed = population - state.world.nation_get_demographics(owners, demographics::employed);
		auto capitalists = state.world.nation_get_demographics(owners, capitalists_key);
		auto aristocrats = state.world.nation_get_demographics(owners, aristocracy_key);
		auto investors = capitalists + aristocrats;

		auto states = state.world.province_get_state_membership(provs);
		auto markets = state.world.state_instance_get_market_from_local_market(states);
		auto owner_spending = state.world.nation_get_spending_level(owners);

		auto pop_savings = state.world.pop_get_savings(ids);

		auto pop_of_type = state.world.pop_get_size(ids);
		auto adj_pop_of_type = pop_of_type / state.defines.alice_needs_scaling_factor;

		auto budget = state.world.nation_get_last_base_budget(owners);

		auto social_budget =
			owner_spending
			* budget
			* ve::to_float(state.world.nation_get_social_spending(owners))
			/ 100.f;

		auto investment_budget =
			owner_spending
			* budget
			* ve::to_float(state.world.nation_get_domestic_investment_spending(owners))
			/ 100.f;

		auto const p_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(owners, sys::national_mod_offsets::unemployment_benefit);

		auto pension_ratio = ve::select(p_level * population > 0.f, p_level * population / (p_level * population + unemp_level * unemployed), 0.f);
		auto unemployment_ratio = ve::select(unemp_level * unemployed > 0.f, unemp_level * unemployed / (p_level * population + unemp_level * unemployed), 0.f);

		// +1.f is here to avoid division by zero
		// which should never happen
		// but i want to be extra sure

		auto const pension_per_person =
			pension_ratio
			* social_budget
			/ (population + 1.f);

		auto const benefits_per_person =
			unemployment_ratio
			* social_budget
			/ (unemployed + 1.f);

		auto const payment_per_investor =
			investment_budget
			/ (investors + 1.f);

		auto const m_spending = owner_spending * ve::to_float(state.world.nation_get_military_spending(owners)) / 100.0f;

		auto types = state.world.pop_get_poptype(ids);

		auto ln_types = state.world.pop_type_get_life_needs_income_type(types);
		auto en_types = state.world.pop_type_get_everyday_needs_income_type(types);
		auto lx_types = state.world.pop_type_get_luxury_needs_income_type(types);

		auto ln_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_life_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto en_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_everyday_needs_costs(n, pt) : 0.0f; },
				types, markets);
		auto lx_costs = ve::apply(
				[&](dcon::pop_type_id pt, dcon::market_id n) { return pt ? state.world.market_get_luxury_needs_costs(n, pt) : 0.0f; },
				types, markets);

		auto total_costs = ln_costs + en_costs + lx_costs;

		auto acc_m = ve::select(ln_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * ln_costs * payouts_spending_multiplier, 0.0f);


		auto acc_u = pension_per_person * pop_of_type;

		acc_m = acc_m + ve::select(en_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * en_costs * payouts_spending_multiplier, 0.0f);

		acc_u = acc_u + ve::select(
			types == state.culture_definitions.capitalists,
			payment_per_investor
			* pop_of_type,
			0.0f
		);
		acc_u = acc_u + ve::select(
			types == state.culture_definitions.aristocrat,
			payment_per_investor
			* pop_of_type,
			0.0f
		);

		acc_m = acc_m + ve::select(lx_types == int32_t(culture::income_type::military), m_spending * adj_pop_of_type * lx_costs * payouts_spending_multiplier, 0.0f);

		auto not_military = !((ln_types == int32_t(culture::income_type::military)) & (en_types == int32_t(culture::income_type::military)) & (lx_types == int32_t(culture::income_type::military)));
		auto employment = pop_demographics::get_employment(state, ids);
		acc_u = acc_u + ve::select(
			not_military,
			benefits_per_person
			* (pop_of_type - employment),
			0.0f
		);

		ve::fp_vector base_income = pop_of_type * price_properties::labor::min * 0.05f;

		state.world.pop_set_savings(ids, state.world.pop_get_savings(ids) + state.inflation * (base_income + (acc_u + acc_m)));
#ifndef NDEBUG
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_m);
		ve::apply([](float v) { assert(std::isfinite(v) && v >= 0); }, acc_u);
#endif
	});
}

std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, bool accepted, float size) {
	float no_education_wage =
		state.world.province_get_labor_price(pid, labor::no_education)
		* state.world.province_get_labor_supply_sold(pid, labor::no_education);
	float basic_education_wage =
		state.world.province_get_labor_price(pid, labor::basic_education)
		* state.world.province_get_labor_supply_sold(pid, labor::basic_education); // craftsmen
	float high_education_wage =
		state.world.province_get_labor_price(pid, labor::high_education)
		* state.world.province_get_labor_supply_sold(pid, labor::high_education); // clerks, clergy and bureaucrats
	float guild_education_wage =
		state.world.province_get_labor_price(pid, labor::guild_education)
		* state.world.province_get_labor_supply_sold(pid, labor::guild_education); // artisans
	float high_education_and_accepted_wage =
		state.world.province_get_labor_price(pid, labor::high_education_and_accepted)
		* state.world.province_get_labor_supply_sold(pid, labor::high_education_and_accepted); // clerks, clergy and bureaucrats of accepted culture

	if(state.world.pop_type_get_is_paid_rgo_worker(ptid)) {
		return { {labor::no_education, 1.f, size * no_education_wage } };
	} else if(state.culture_definitions.primary_factory_worker == ptid) {
		auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_no_education);
		auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_basic_education);
		return {
			{labor::no_education, no_education, no_education * size * no_education_wage },
			{labor::basic_education, basic_education, basic_education * size * basic_education_wage }
		};
	} else if(state.culture_definitions.secondary_factory_worker == ptid || state.culture_definitions.bureaucrat == ptid || state.culture_definitions.clergy == ptid) {
		if(accepted) {
			auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_no_education);
			auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_basic_education);
			auto high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education);
			auto high_education_accepted = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education_accepted);
			return {
				{labor::no_education, no_education, no_education * size * no_education_wage },
				{labor::basic_education, basic_education, basic_education * size * basic_education_wage },
				{labor::high_education, high_education, high_education * size * high_education_wage },
				{labor::high_education_and_accepted, high_education_accepted, high_education_accepted * size * high_education_and_accepted_wage }
			};
		} else {
			auto no_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_no_education);
			auto basic_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_basic_education);
			auto high_education = state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_high_education);
			return {
				{labor::no_education, no_education, no_education * size * no_education_wage },
				{labor::basic_education, basic_education, basic_education * size * basic_education_wage },
				{labor::high_education, high_education, high_education * size * high_education_wage },
			};
		}
	}
	return {};
}

std::vector<labor_ratio_wage> estimate_wage(sys::state const& state, dcon::pop_id pop) {
	return estimate_wage(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_is_primary_or_accepted_culture(pop),
		state.world.pop_get_size(pop)
	);
}

float estimate_total_wage(sys::state const& state, dcon::pop_id pop) {
	float total = 0.f;
	auto list = estimate_wage(state, pop);
	for(auto& item : list) {
		total += item.wage;
	}
	return total;
}


float estimate_rgo_income(sys::state const& state, dcon::province_id pid, dcon::pop_type_id ptid, float size) {
	float no_education_wage =
		state.world.province_get_labor_price(pid, labor::no_education)
		* state.world.province_get_labor_supply_sold(pid, labor::no_education);
	float rgo_workers_wage =
		state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education)
		* no_education_wage;
	auto total_rgo_profit = state.world.province_get_rgo_profit(pid);
	for(auto pl : state.world.province_get_pop_location(pid)) {
		if(pl.get_pop().get_poptype() == state.culture_definitions.slaves) {
			total_rgo_profit += pl.get_pop().get_size() * rgo_workers_wage;
		}
	}
	float aristocrats_share = state.world.province_get_landowners_share(pid);
	float num_aristocrat = state.world.province_get_demographics(
		pid,
		demographics::to_key(state, state.culture_definitions.aristocrat)
	);
	if(total_rgo_profit >= 0.f && num_aristocrat > 0.f && state.culture_definitions.aristocrat == ptid) {
		return size * total_rgo_profit * aristocrats_share / num_aristocrat;
	} else {
		return 0.f;
	}
}

float estimate_rgo_income(sys::state const& state, dcon::pop_id pop) {
	return estimate_rgo_income(
		state,
		state.world.pop_get_province_from_pop_location(pop),
		state.world.pop_get_poptype(pop),
		state.world.pop_get_size(pop)
	);
}


float market_cut(sys::state const& state, dcon::market_id market, float no_education_wage) {
	auto modified = local_market_cut_baseline - state.world.market_get_stockpile(market, economy::money) / (no_education_wage + 0.000001f) / 100'000.f;
	return std::clamp(modified, 0.f, 0.1f);
}

constexpr inline float market_tax = 0.05f;

void update_income_wages(sys::state& state){
	// TODO: rewrite in vectorized way

	for(auto n : state.world.in_nation) {
		for(auto poid : state.world.nation_get_province_ownership(n)) {
			float total_factory_profit = 0.f;
			float total_rgo_profit = 0.f;

			float market_profit = 0.f;

			auto pid = poid.get_province();
			auto sid = pid.get_state_membership();
			auto market = sid.get_market_from_local_market();

			float no_education_wage =
				state.world.province_get_labor_price(pid, labor::no_education)
				* state.world.province_get_labor_supply_sold(pid, labor::no_education);
			float basic_education_wage =
				state.world.province_get_labor_price(pid, labor::basic_education)
				* state.world.province_get_labor_supply_sold(pid, labor::basic_education); // craftsmen
			float high_education_wage =
				state.world.province_get_labor_price(pid, labor::high_education)
				* state.world.province_get_labor_supply_sold(pid, labor::high_education); // clerks, clergy and bureaucrats
			float guild_education_wage =
				state.world.province_get_labor_price(pid, labor::guild_education)
				* state.world.province_get_labor_supply_sold(pid, labor::guild_education); // artisans
			float high_education_and_accepted_wage =
				state.world.province_get_labor_price(pid, labor::high_education_and_accepted)
				* state.world.province_get_labor_supply_sold(pid, labor::high_education_and_accepted); // clerks, clergy and bureaucrats of accepted culture

			float sum_of_wages = no_education_wage + basic_education_wage + high_education_wage + high_education_and_accepted_wage;

			float rgo_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::rgo_worker_no_education)
				* no_education_wage;

			float primary_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_no_education)
				* no_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::primary_basic_education)
				* basic_education_wage;

			float high_not_accepted_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_no_education)
				* no_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_basic_education)
				* basic_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_not_accepted_high_education)
				* high_education_wage;

			float high_accepted_workers_wage =
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_no_education)
				* no_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_basic_education)
				* basic_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education)
				* high_education_wage
				+
				state.world.province_get_pop_labor_distribution(pid, pop_labor::high_education_accepted_high_education_accepted)
				* high_education_and_accepted_wage;

			float num_aristocrat = state.world.province_get_demographics(
				pid,
				demographics::to_key(state, state.culture_definitions.aristocrat)
			);

			float payment_per_aristocrat = 0.f;
			float aristocrats_share = state.world.province_get_landowners_share(pid);
			float others_share = (1.f - aristocrats_share);

			// FACTORIES
			// all profits go to market stockpiles and then they are distributed to capitalists
			for(auto f : state.world.province_get_factory_location(pid)) {
				auto fac = f.get_factory();
				auto profit = explain_last_factory_profit(state, fac);
				total_factory_profit += profit.profit;
			}
			market_profit += total_factory_profit;

			// RGOS and slaves cashback
			{
				total_rgo_profit += state.world.province_get_rgo_profit(pid);
				for(auto pl : state.world.province_get_pop_location(pid)) {
					if(pl.get_pop().get_poptype() == state.culture_definitions.slaves) {
						total_rgo_profit += pl.get_pop().get_size() * rgo_workers_wage;
					}
				}
			}

			auto local_market_cut = market_cut(state, market, no_education_wage);

			auto market_rgo_activity_cut = total_rgo_profit * local_market_cut;
			total_rgo_profit -= market_rgo_activity_cut;

			if(total_rgo_profit >= 0.f && num_aristocrat > 0.f) {
				payment_per_aristocrat += total_rgo_profit * aristocrats_share / num_aristocrat;
				market_profit += total_rgo_profit * others_share;
			} else {
				market_profit += total_rgo_profit;
			}
			auto& cur_money = state.world.market_get_stockpile(market, economy::money);
			state.world.market_set_stockpile(market, economy::money, cur_money + market_profit + market_rgo_activity_cut);

			auto market_cut_from_wages = 0.f;

			for(auto pl : state.world.province_get_pop_location(pid)) {
				auto pop = pl.get_pop();
				auto savings = pop.get_savings();
				auto poptype = pop.get_poptype();
				auto size = pop.get_size();

				auto accepted = state.world.nation_get_accepted_cultures(n, pop.get_culture())
					|| state.world.nation_get_primary_culture(n) == pop.get_culture();

				if(poptype.get_is_paid_rgo_worker()) {
					pop.set_savings(pop.get_savings() + size * rgo_workers_wage);
				} else if(state.culture_definitions.primary_factory_worker == poptype) {
					pop.set_savings(pop.get_savings() + size * primary_workers_wage);
				} else if(state.culture_definitions.secondary_factory_worker == pop.get_poptype()) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + size * high_accepted_workers_wage);
					} else {
						pop.set_savings(pop.get_savings() + size * high_not_accepted_workers_wage);
					}
					assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
				} else if(pop.get_poptype() == state.culture_definitions.bureaucrat) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + size * high_accepted_workers_wage);
					} else {
						pop.set_savings(pop.get_savings() + size * high_not_accepted_workers_wage);
					}
				} else if(pop.get_poptype() == state.culture_definitions.clergy) {
					if(accepted) {
						pop.set_savings(pop.get_savings() + size * high_accepted_workers_wage);
					} else {
						pop.set_savings(pop.get_savings() + size * high_not_accepted_workers_wage);
					}
				} else if(state.culture_definitions.aristocrat == pop.get_poptype()) {
					pop.set_savings(pop.get_savings() + size * payment_per_aristocrat);
				}

				// pops pay a "tax" to market to import expensive goods:
				{
					float new_savings = pop.get_savings();
					pop.set_savings(state.inflation * new_savings * (1.f - market_tax));
					market_cut_from_wages += new_savings * market_tax;
				}

				assert(std::isfinite(pop.get_savings()) && pop.get_savings() >= 0);
			}

			state.world.market_set_stockpile(market, economy::money, state.world.market_get_stockpile(market, economy::money) + market_cut_from_wages);
		}
	}
}

float estimate_next_day_raw_income(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto estimated =
		estimate_artisan_income(state, pop)
		+ estimate_rgo_income(state, pop)
		+ estimate_trade_income(state, pop)
		+ estimate_total_wage(state, pop);

	auto from_nation = estimate_income_from_nation(state, pop);

	estimated +=
		from_nation.investment
		+ from_nation.military
		+ from_nation.pension
		+ from_nation.unemployment;

	return estimated;
}

float estimate_next_day_budget_before_taxes(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto current = state.world.pop_get_savings(pop);
	current -= prepare_pop_budget(state, pop).spent_total;

	auto estimated = current
		+ estimate_artisan_income(state, pop)
		+ estimate_rgo_income(state, pop)
		+ estimate_trade_income(state, pop)
		+ estimate_total_wage(state, pop);

	auto from_nation = estimate_income_from_nation(state, pop);

	estimated +=
		from_nation.investment
		+ from_nation.military
		+ from_nation.pension
		+ from_nation.unemployment;

	return estimated;
}

float estimate_trade_spending(
	sys::state const& state,
	dcon::pop_id pop
) {
	auto next_day = estimate_next_day_budget_before_taxes(state, pop);
	return market_tax * next_day;
}

float estimate_tax_spending(
	sys::state const& state,
	dcon::pop_id pop,
	float tax_rate
) {
	auto next_day = estimate_next_day_raw_income(state, pop);
	return next_day * (1.f - market_tax) * tax_rate;
}

float estimate_pop_demand_internal_life(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.life_needs.demand_scale
		* budget.life_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_lf_needs_scale
		* state.world.pop_type_get_life_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor;
}
float estimate_pop_demand_internal_everyday(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.everyday_needs.demand_scale
		* budget.everyday_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_ev_needs_scale
		* state.world.pop_type_get_everyday_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor
		* invention_factor;
}
float estimate_pop_demand_internal_luxury(
	sys::state const& state, dcon::commodity_id c, dcon::pop_id pop,
	pops::vectorized_pops_budget<float>& budget,
	float mult_per_strata[3], float need_weight, float invention_factor
) {
	auto pop_type = state.world.pop_get_poptype(pop);
	auto strata = state.world.pop_type_get_strata(pop_type);
	auto pop_size = state.world.pop_get_size(pop);
	return budget.luxury_needs.demand_scale
		* budget.luxury_needs.satisfied_with_money_ratio
		* need_weight
		* mult_per_strata[strata]
		* state.defines.alice_lx_needs_scale
		* state.world.pop_type_get_luxury_needs(pop_type, c)
		* pop_size
		/ state.defines.alice_needs_scaling_factor
		* invention_factor;
}

float estimate_pop_spending_life(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_life_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_life_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_life(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, market, cid);
	return demand * actually_bought * cost;
}

float estimate_pop_spending_everyday(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_everyday_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_everyday(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, market, cid);
	return demand * actually_bought * cost;
}

float estimate_pop_spending_luxury(sys::state& state, dcon::pop_id pop, dcon::commodity_id cid) {
	auto pid = state.world.pop_get_province_from_pop_location(pop);
	auto nation = state.world.province_get_nation_from_province_ownership(pid);
	auto zone = state.world.province_get_state_membership(pid);
	auto market = state.world.state_instance_get_market_from_local_market(zone);
	auto budget = prepare_pop_budget(state, pop);
	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;
	auto weight = state.world.market_get_luxury_needs_weights(market, cid);
	float mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_luxury_needs) + 1.0f
	};
	auto demand = pops::estimate_pop_demand_internal_luxury(
		state, cid, pop, budget, mul, weight, invention_factor
	);
	auto actually_bought = state.world.market_get_actual_probability_to_buy(market, cid);
	auto cost = economy::price(state, market, cid);
	return demand * actually_bought * cost;
}

vectorized_pops_budget<float> prepare_pop_budget(const sys::state& state, dcon::pop_id ids) {
	return prepare_pop_budget_templated(state, ids);
}

}

float estimate_pops_consumption(sys::state& state, dcon::commodity_id c, dcon::province_id p) {
	auto zone = state.world.province_get_state_membership(p);
	auto market = state.world.state_instance_get_market_from_local_market(zone);

	auto satisfaction = state.world.market_get_actual_probability_to_buy(market, c);

	auto nation = state.world.province_get_nation_from_province_ownership(p);

	auto weight_life = state.world.market_get_life_needs_weights(market, c);
	auto weight_everyday = state.world.market_get_everyday_needs_weights(market, c);
	auto weight_luxury = state.world.market_get_luxury_needs_weights(market, c);

	float life_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_life_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_life_needs) + 1.0f
	};
	float everyday_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_everyday_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_everyday_needs) + 1.0f
	};
	float luxury_mul[3] = {
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::poor_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::middle_luxury_needs) + 1.0f,
		state.world.nation_get_modifier_values(
			nation, sys::national_mod_offsets::rich_luxury_needs) + 1.0f,
	};

	auto invention_count = 0.f;
	state.world.for_each_invention([&](auto iid) {
		invention_count += state.world.nation_get_active_inventions(nation, iid) ? 1.0f : 0.0f;
	});
	auto invention_factor = state.defines.invention_impact_on_demand * invention_count + 1.f;

	float total = 0.f;
	state.world.province_for_each_pop_location(p, [&](auto location) {
		dcon::pop_id pop = state.world.pop_location_get_pop(location);

		auto pop_type = state.world.pop_get_poptype(pop);
		auto strata = state.world.pop_type_get_strata(pop_type);

		pops::vectorized_pops_budget<float> budget = pops::prepare_pop_budget(state, pop);

		auto consumption_life = pops::estimate_pop_demand_internal_life(
			state, c, pop, budget, life_mul, weight_life, invention_factor
		);
		auto consumption_everyday = pops::estimate_pop_demand_internal_everyday(
			state, c, pop, budget, everyday_mul, weight_everyday, invention_factor
		);
		auto consumption_luxury = pops::estimate_pop_demand_internal_luxury(
			state, c, pop, budget, luxury_mul, weight_luxury, invention_factor
		);

		total += consumption_life + consumption_everyday + consumption_luxury;
	});

	return total * satisfaction;
}
}
