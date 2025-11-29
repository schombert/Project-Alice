#pragma once
#include "system_state.hpp"
#include "economy_constants.hpp"
#include "adaptive_ve.hpp"

namespace economy {

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

} // namespace economy
