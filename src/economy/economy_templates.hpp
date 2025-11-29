#pragma once
#include "system_state.hpp"
#include "economy_constants.hpp"
#include "adaptive_ve.hpp"
#include "advanced_province_buildings.hpp"
#include "demographics_templates.hpp"

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

template<typename F>
void for_each_new_factory(sys::state& state, dcon::province_id s, F&& func) {
	for(auto st_con : state.world.province_get_factory_construction(s)) {
		if(!st_con.get_is_upgrade() && !st_con.get_refit_target()) {
			float admin_eff = state.world.province_get_control_ratio(st_con.get_province());
			float factory_mod = state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
			float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_owner_cost));
			float admin_cost_factor = (st_con.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;

			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(st_con.get_type());

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i] * admin_cost_factor;
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			func(new_factory{total > 0.0f ? purchased / total : 0.0f, st_con.get_type().id});
		}
	}
}

template<typename F>
void for_each_upgraded_factory(sys::state& state, dcon::province_id s, F&& func) {
	for(auto st_con : state.world.province_get_factory_construction(s)) {
		if(st_con.get_is_upgrade() || st_con.get_refit_target()) {
			float admin_eff = state.world.province_get_control_ratio(st_con.get_province());
			float factory_mod = state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
			float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(st_con.get_nation(), sys::national_mod_offsets::factory_owner_cost));
			float admin_cost_factor = (st_con.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;
			float refit_discount = (st_con.get_refit_target()) ? state.defines.alice_factory_refit_cost_modifier : 1.0f;

			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(st_con.get_type());

			for(uint32_t i = 0; i < commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i] * admin_cost_factor * refit_discount;
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}

			func(upgraded_factory{total > 0.0f ? purchased / total : 0.0f, st_con.get_type().id, st_con.get_refit_target().id});
		}
	}
}


template<typename TRADE_ROUTE>
auto trade_route_effect_of_scale(sys::state& state, TRADE_ROUTE trade_route) {
	using MARKET = convert_value_type<TRADE_ROUTE, dcon::trade_route_id, dcon::market_id>;
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;
	MARKET A = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 0);
	}, trade_route);
	MARKET B = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 1);
	}, trade_route);

	VALUE cargo = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		VALUE volume = state.world.trade_route_get_volume(trade_route, cid);
		MARKET origin = ve::select(volume > 0.f, A, B);
		cargo = cargo + adaptive_ve::abs(volume) * state.world.market_get_actual_probability_to_buy(origin, cid);;
	});
	return adaptive_ve::max<VALUE>(
		trade_effect_of_scale_lower_bound,
		1.f - cargo * effect_of_transportation_scale
	);
}

template <typename TRADE_ROUTE>
auto explain_trade_route(
	sys::state& state,
	TRADE_ROUTE trade_route,
	ve::vectorizable_buffer<float, dcon::market_id>& export_tariff,
	ve::vectorizable_buffer<float, dcon::market_id>& import_tariff
) {
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;

	auto m0 = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 0);
	}, trade_route);
	auto m1 = ve::apply([&](auto route) {
		return state.world.trade_route_get_connected_markets(route, 1);
	}, trade_route);

	auto s0 = state.world.market_get_zone_from_local_market(m0);
	auto s1 = state.world.market_get_zone_from_local_market(m1);
	auto formal_n0 = state.world.state_instance_get_nation_from_state_ownership(s0);
	auto formal_n1 = state.world.state_instance_get_nation_from_state_ownership(s1);

	auto capital_0 = state.world.state_instance_get_capital(s0);
	auto capital_1 = state.world.state_instance_get_capital(s1);

	auto controller_capital_0 = state.world.province_get_nation_from_province_control(capital_0);
	auto controller_capital_1 = state.world.province_get_nation_from_province_control(capital_1);

	controller_capital_0 = adaptive_ve::select(controller_capital_0 != dcon::nation_id{}, controller_capital_0, formal_n0);
	controller_capital_1 = adaptive_ve::select(controller_capital_1 != dcon::nation_id{}, controller_capital_1, formal_n1);

	auto market_leader_0 = nations::get_market_leader(state, controller_capital_0);
	auto market_leader_1 = nations::get_market_leader(state, controller_capital_1);

	auto applies_tariffs_0 = ve::apply([&](auto n_a, auto n_b) {
		auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
		if(source_tariffs_rel) {
			auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
			// Enddt empty signalises revoken agreement
			// Enddt > cur_date signalises that the agreement can't be broken
			if(enddt) {
				return false;
			}
		}
		return true;
	}, market_leader_0, controller_capital_1);

	auto applies_tariffs_1 = ve::apply([&](auto n_a, auto n_b) {
		auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);
		if(target_tariffs_rel) {
			auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
			if(enddt) {
				return false;
			}
		}
		return true;
	}, market_leader_1, controller_capital_0);

	auto is_open_0_to_1 = market_leader_0 == controller_capital_1 || !applies_tariffs_0 || controller_capital_0 == controller_capital_1;
	auto is_open_1_to_0 = market_leader_1 == controller_capital_0 || !applies_tariffs_1 || controller_capital_0 == controller_capital_1;

	auto distance = state.world.trade_route_get_distance(trade_route);
	auto trade_good_loss_mult = adaptive_ve::max<VALUE>(0.f, 1.f - trade_loss_per_distance_unit * distance);
	auto scale = trade_route_effect_of_scale(state, trade_route);
	auto base_cost_per_unit = distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good * (
		state.world.province_get_labor_price(capital_0, labor::no_education)
		+ state.world.province_get_labor_price(capital_1, labor::no_education)
	);

	auto export_tariff_0 = ve::select(is_open_0_to_1, 0.f, export_tariff.get(m0));
	auto import_tariff_0 = ve::select(is_open_0_to_1, 0.f, import_tariff.get(m0));
	auto export_tariff_1 = ve::select(is_open_1_to_0, 0.f, export_tariff.get(m1));
	auto import_tariff_1 = ve::select(is_open_1_to_0, 0.f, import_tariff.get(m1));

	tariff_data<TRADE_ROUTE> result{
		.applies_tariff = {is_open_0_to_1, is_open_1_to_0},
		.export_tariff = {export_tariff_0, export_tariff_1 },
		.import_tariff = {import_tariff_0, import_tariff_1 },
		.markets = { m0, m1 },
		.distance = distance,
		.loss = trade_good_loss_mult,
		.base_distance_cost = base_cost_per_unit,
		.workers_satisfaction = adaptive_ve::min(
			state.world.province_get_labor_demand_satisfaction(capital_0, labor::no_education),
			state.world.province_get_labor_demand_satisfaction(capital_1, labor::no_education)
		),
		.effect_of_scale = scale,
		.distance_cost_scaled = scale * base_cost_per_unit
	};

	return result;
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

	VALUE can_use_free_services{ };
	VALUE remaining_savings{ };
	VALUE spent_total{ };
};

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

} // namespace pops

} // namespace economy
