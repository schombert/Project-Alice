#include "economy_trade_routes.hpp"
#include "economy_stats.hpp"
#include "system_state.hpp"

// implements trade routes
// when changing logic of trade routes please update it everywhere
// due to performance reasons we have to duplicate it

namespace economy {

// US3AC2 Labour demand for a single trade route
float trade_route_labour_demand(sys::state& state, dcon::trade_route_id trade_route, dcon::province_fat_id A_capital, dcon::province_fat_id B_capital) {
	auto total_demanded_labor = 0.f;
	auto available_labor = std::min(
		state.world.province_get_labor_demand_satisfaction(A_capital, labor::no_education),
		state.world.province_get_labor_demand_satisfaction(B_capital, labor::no_education)
	);

	state.world.for_each_commodity([&](auto cid) {
		auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
		if(current_volume == 0.f) {
			return;
		}
		current_volume = current_volume * std::max(0.999999f, available_labor);
		state.world.trade_route_set_volume(trade_route, cid, current_volume);
		auto effect_of_scale = std::max(
			trade_effect_of_scale_lower_bound,
			1.f - std::abs(current_volume) * effect_of_transportation_scale
		);
		total_demanded_labor += std::abs(current_volume)
			* state.world.trade_route_get_distance(trade_route)
			/ trade_distance_covered_by_pair_of_workers_per_unit_of_good
			* effect_of_scale;
		assert(std::isfinite(total_demanded_labor));
	});

	return total_demanded_labor;
}

// US3AC2 Calculate labour demand for trade routes between markets
float transportation_between_markets_labor_demand(sys::state& state, dcon::market_id market) {

	auto total_demanded_labour = 0.f;

	for(auto route : state.world.market_get_trade_route(market)) {
		auto A = state.world.trade_route_get_connected_markets(route, 0);
		auto B = state.world.trade_route_get_connected_markets(route, 1);

		auto A_capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(A));
		auto B_capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(B));

		total_demanded_labour += trade_route_labour_demand(state, route, A_capital, B_capital);
	}

	return total_demanded_labour;
}


// US3AC3 Calculate labour demand for trade inside the market
float transportation_inside_market_labor_demand(sys::state& state, dcon::market_id market, dcon::province_id capital) {
	auto base_cargo_transport_demand = 0.f;

	state.world.for_each_commodity([&](auto commodity) {
		state.world.market_for_each_trade_route(market, [&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, commodity);
			auto origin = 
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			//auto sat = state.world.market_get_direct_demand_satisfaction(origin, commodity);
			base_cargo_transport_demand += std::abs(current_volume);
		});
	});

	// auto soft_transport_demand_limit = state.world.market_get_max_throughput(market);
	//if(base_cargo_transport_demand > soft_transport_demand_limit) {
	//	base_cargo_transport_demand = base_cargo_transport_demand * base_cargo_transport_demand / soft_transport_demand_limit;
	//}

	return base_cargo_transport_demand;
}

tariff_data explain_trade_route(sys::state& state, dcon::trade_route_id trade_route) {
	auto m0 = state.world.trade_route_get_connected_markets(trade_route, 0);
	auto m1 = state.world.trade_route_get_connected_markets(trade_route, 1);
	auto s0 = state.world.market_get_zone_from_local_market(m0);
	auto s1 = state.world.market_get_zone_from_local_market(m1);
	auto n0 = state.world.state_instance_get_nation_from_state_ownership(s0);
	auto n1 = state.world.state_instance_get_nation_from_state_ownership(s1);

	bool same_nation = n0 == n1;

	auto capital_0 = state.world.state_instance_get_capital(s0);
	auto capital_1 = state.world.state_instance_get_capital(s1);

	auto controller_capital_0 = state.world.province_get_nation_from_province_control(capital_0);
	auto controller_capital_1 = state.world.province_get_nation_from_province_control(capital_1);

	auto sphere_0 = state.world.nation_get_in_sphere_of(controller_capital_0);
	auto sphere_1 = state.world.nation_get_in_sphere_of(controller_capital_1);

	auto overlord_0 = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(sphere_0)
	);
	auto overlord_1 = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(sphere_1)
	);

	auto applies_tariffs_0 = true;
	auto applies_tariffs_1 = true;
	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n1, n0);
	auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n0, n1);
	if(source_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
		if(state.current_date < enddt) {
			applies_tariffs_0 = false;
		}
	}
	if(target_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
		if(state.current_date < enddt) {
			applies_tariffs_1 = false;
		}
	}

	auto is_open_0_to_1 = sphere_0 == controller_capital_1 || overlord_0 == controller_capital_1 || !applies_tariffs_0;
	auto is_open_1_to_0 = sphere_1 == controller_capital_0 || overlord_1 == controller_capital_0 || !applies_tariffs_1;

	auto distance = state.world.trade_route_get_distance(trade_route);
	auto trade_good_loss_mult = std::max(0.f, 1.f - trade_loss_per_distance_unit * distance);

	return {
		.applies_tariff = {is_open_0_to_1, is_open_1_to_0},
		.export_tariff = {effective_tariff_export_rate(state, n0, m0), effective_tariff_export_rate(state, n1, m1) },
		.import_tariff = {effective_tariff_import_rate(state, n0, m0), effective_tariff_import_rate(state, n1, m1) },
		.distance = distance,
		.loss = trade_good_loss_mult,
		.base_distance_cost = distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good
		* (
			state.world.province_get_labor_price(capital_0, labor::no_education)
			+ state.world.province_get_labor_price(capital_1, labor::no_education)
		),
		.workers_satisfaction = std::min(
			state.world.province_get_labor_demand_satisfaction(capital_0, labor::no_education),
			state.world.province_get_labor_demand_satisfaction(capital_1, labor::no_education)
		)
	};
}

void make_trade_center_tooltip(sys::state& state, text::columnar_layout& contents, dcon::market_id market) {
	auto trade_volume = 0.f;
	auto trade_value = 0.f;
	auto imports_volume = 0.f;
	auto imports_value = 0.f;
	auto exports_volume = 0.f;
	auto exports_value = 0.f;
	auto profit = 0.f;

	auto labour_demand = 0.f;

	for(auto commodity : state.world.in_commodity) {
		for(auto route : state.world.market_get_trade_route(market)) {
			auto explain = explain_trade_route_commodity(state, route, commodity);

			if(market == explain.origin && explain.amount_origin > 0) {
				exports_volume += explain.amount_origin;
				exports_value += explain.amount_origin * explain.price_origin;
				trade_value += explain.amount_origin * explain.price_origin;
				trade_volume += explain.amount_origin;
				profit += explain.amount_origin * explain.payment_received_per_unit;
			}
			else if(market == explain.target && explain.amount_target > 0) {
				imports_volume += explain.amount_target;
				imports_value += explain.amount_target * explain.price_target;
				trade_value += explain.amount_target * explain.price_target;
				trade_volume += explain.amount_target;
				profit -= explain.amount_target * explain.payment_per_unit;
			}
		}
	}

	text::add_line(state, contents, "trade_centre_trade_volume", text::variable_type::val, text::fp_two_places{ trade_volume });
	text::add_line(state, contents, "trade_centre_imports_volume", text::variable_type::val, text::fp_two_places{ imports_volume }, 15);
	text::add_line(state, contents, "trade_centre_exports_volume", text::variable_type::val, text::fp_two_places{ exports_volume }, 15);

	text::add_line(state, contents, "trade_centre_trade_value", text::variable_type::val, text::fp_currency{ trade_value });
	text::add_line(state, contents, "trade_centre_imports_value", text::variable_type::val, text::fp_currency{ imports_value }, 15);
	text::add_line(state, contents, "trade_centre_exports_value", text::variable_type::val, text::fp_currency{ exports_value }, 15);

	text::add_line(state, contents, "trade_centre_trade_routes_profit", text::variable_type::val, text::fp_currency{ profit });
	text::add_line(state, contents, "trade_centre_money", text::variable_type::val, text::fp_currency{ state.world.market_get_stockpile(market, money) });
}

void make_trade_volume_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::trade_route_id route,
	dcon::commodity_id cid,
	dcon::market_id point_of_view
) {
	auto prediction = predict_trade_route_volume_change(state, route, cid);

	auto multiplier = 1.f;
	auto B = state.world.trade_route_get_connected_markets(route, 1);
	if(B == point_of_view) {
		multiplier = -1.f;
	}

	if(prediction.commodity_is_not_discovered) {
		text::add_line(state, contents, "commodity_is_unknown");
		return;
	}

	if(prediction.commodity_is_not_tradable) {
		text::add_line(state, contents, "commodity_is_not_tradable");
		return;
	}

	if(prediction.trade_blocked) {
		text::add_line(state, contents, "trade_is_blocked");
		return;
	}

	text::add_line(state, contents, "trade_route_volume_profit_score",
		text::variable_type::val, text::fp_two_places{ prediction.profit_score },
		text::variable_type::x, text::fp_two_places{ prediction.profit },
		text::variable_type::y, text::fp_two_places{ prediction.divisor }
	);

	text::add_line(state, contents, "trade_route_volume_base_change",
		text::variable_type::val, text::fp_two_places{ multiplier * prediction.base_change },
		text::variable_type::x, text::fp_two_places{ std::abs(prediction.current_volume) }
	);

	text::add_line(state, contents, "trade_route_volume_expansion_multiplier",
		text::variable_type::val, text::fp_two_places{ prediction.expansion_multiplier }
	);

	text::add_line(state, contents, "trade_route_volume_decay",
		text::variable_type::val, text::fp_two_places{ prediction.decay }
	);

	text::add_line(state, contents, "trade_route_volume_final_change",
		text::variable_type::val, text::fp_two_places{ multiplier * prediction.final_change }
	);
}

embargo_explanation embargo_exists(
	sys::state& state, dcon::nation_id n_A, dcon::nation_id n_B
) {
	auto sphere_A = state.world.nation_get_in_sphere_of(n_A);
	auto sphere_B = state.world.nation_get_in_sphere_of(n_B);
	auto overlord_A = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(n_A)
	);
	auto overlord_B = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(n_B)
	);
	auto has_overlord_mask_A = overlord_A != dcon::nation_id{};
	auto has_overlord_mask_B = overlord_B != dcon::nation_id{};
	auto has_sphere_mask_A = sphere_A != dcon::nation_id{};
	auto has_sphere_mask_B = sphere_B != dcon::nation_id{};
	// Subjects have embargo of overlords propagated onto them
	auto market_leader_A = has_overlord_mask_A ? overlord_A : (has_sphere_mask_A ? sphere_A : n_A);
	auto market_leader_B = has_overlord_mask_B ? overlord_B : (has_sphere_mask_B ? sphere_B : n_B);

	// if market capital controllers are at war then we will break the link
	auto at_war = military::are_at_war(state, n_A, n_B);

	auto is_A_civ = state.world.nation_get_is_civilized(n_A);
	auto is_B_civ = state.world.nation_get_is_civilized(n_B);

	// sphere joins embargo
	// subject joins embargo
	// diplomatic embargos
	auto A_joins_sphere_wide_embargo = military::are_at_war(state, sphere_A, n_B);
	auto B_joins_sphere_wide_embargo = military::are_at_war(state, sphere_B, n_A);

	auto A_has_embargo =
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(n_B, market_leader_A)
		)
		||
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(market_leader_A, n_B)
		);

	auto B_has_embargo =
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(n_A, market_leader_B)
		)
		||
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(market_leader_B, n_A)
		);

	embargo_explanation result;

	result.war = at_war;
	result.origin_embargo = A_has_embargo;
	result.target_embargo = B_has_embargo;
	result.origin_join_embargo = A_joins_sphere_wide_embargo;
	result.target_join_embargo = B_joins_sphere_wide_embargo;

	result.combined = at_war
		|| A_has_embargo
		|| B_has_embargo
		|| A_joins_sphere_wide_embargo
		|| B_joins_sphere_wide_embargo;

	return result;
}

trade_route_volume_change_reasons predict_trade_route_volume_change(
	sys::state& state, dcon::trade_route_id route, dcon::commodity_id cid
) {
	trade_route_volume_change_reasons result{
		.export_price = { 0.f, 0.f },
		.import_price = { 0.f, 0.f },
		.trade_blocked = false,
		.commodity_is_not_tradable = false
	};

	auto A = state.world.trade_route_get_connected_markets(route, 0);
	auto B = state.world.trade_route_get_connected_markets(route, 1);
	auto s_A = state.world.market_get_zone_from_local_market(A);
	auto s_B = state.world.market_get_zone_from_local_market(B);
	auto n_A = state.world.state_instance_get_nation_from_state_ownership(s_A);
	auto n_B = state.world.state_instance_get_nation_from_state_ownership(s_B);
	auto capital_A = state.world.state_instance_get_capital(s_A);
	auto capital_B = state.world.state_instance_get_capital(s_B);
	auto port_A = province::state_get_coastal_capital(state, s_A);
	auto port_B = province::state_get_coastal_capital(state, s_B);
	auto controller_capital_A = state.world.province_get_nation_from_province_control(capital_A);
	auto controller_capital_B = state.world.province_get_nation_from_province_control(capital_B);
	auto controller_port_A = state.world.province_get_nation_from_province_control(port_A);
	auto controller_port_B = state.world.province_get_nation_from_province_control(port_B);
	auto sphere_A = state.world.nation_get_in_sphere_of(controller_capital_A);
	auto sphere_B = state.world.nation_get_in_sphere_of(controller_capital_B);
	auto overlord_A = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(controller_capital_A)
	);
	auto overlord_B = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(controller_capital_B)
	);
	auto has_overlord_mask_A = overlord_A != dcon::nation_id{};
	auto has_overlord_mask_B = overlord_B != dcon::nation_id{};
	auto has_sphere_mask_A = sphere_A != dcon::nation_id{};
	auto has_sphere_mask_B = sphere_B != dcon::nation_id{};
	// Subjects have embargo of overlords propagated onto them
	auto market_leader_A = has_overlord_mask_A ? overlord_A : (has_sphere_mask_A ? sphere_A : n_A);
	auto market_leader_B = has_overlord_mask_B ? overlord_B : (has_sphere_mask_B ? sphere_B : n_B);

	// Equal/unequal trade treaties
	auto A_open_to_B = false;
	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(
		controller_capital_B, market_leader_A
	);
	if(source_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
		if(enddt) {
			A_open_to_B = true;
		}
	}
	auto B_open_to_A = false;
	auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(
		controller_capital_A, market_leader_B
	);
	if(target_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
		if(enddt) {
			B_open_to_A = true;
		}
	}

	auto A_is_open_to_B = sphere_A == controller_capital_B || overlord_A == controller_capital_B || A_open_to_B;
	auto B_is_open_to_A = sphere_B == controller_capital_A || overlord_B == controller_capital_A || B_open_to_A;

	auto port_occupied_A = military::are_at_war(state, controller_capital_A, controller_port_A);
	auto port_occupied_B = military::are_at_war(state, controller_capital_B, controller_port_B);

	auto is_A_blockaded = state.world.province_get_is_blockaded(port_A) || port_occupied_A;
	auto is_B_blockaded = state.world.province_get_is_blockaded(port_B) || port_occupied_B;

	// if market capital controllers are at war then we will break the link
	auto at_war = military::are_at_war(state, controller_capital_A, controller_capital_B);

	auto is_A_civ = state.world.nation_get_is_civilized(n_A);
	auto is_B_civ = state.world.nation_get_is_civilized(n_B);

	auto is_sea_route = state.world.trade_route_get_is_sea_route(route);
	auto is_land_route = state.world.trade_route_get_is_land_route(route);
	auto same_nation = controller_capital_A == controller_capital_B;

	// US3AC7. Ban international sea routes or international land routes based on the corresponding modifiers
	auto A_bans_sea_trade = state.world.nation_get_modifier_values(n_A, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
	auto B_bans_sea_trade = state.world.nation_get_modifier_values(n_B, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
	auto sea_trade_banned = A_bans_sea_trade || B_bans_sea_trade;
	// US3AC8. Ban international sea routes or international land routes based on the corresponding modifiers
	auto A_bans_land_trade = state.world.nation_get_modifier_values(n_A, sys::national_mod_offsets::disallow_land_trade) > 0.f;
	auto B_bans_land_trade = state.world.nation_get_modifier_values(n_B, sys::national_mod_offsets::disallow_land_trade) > 0.f;
	auto land_trade_banned = A_bans_land_trade || B_bans_land_trade;
	auto trade_banned = (is_sea_route && sea_trade_banned && !same_nation) || (is_land_route && land_trade_banned && !same_nation);

	is_sea_route = is_sea_route && !is_A_blockaded && !is_B_blockaded;

	// US3AC9. Wartime embargoes
	auto A_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
		return military::are_at_war(state, n_a, n_b);
	}, sphere_A, controller_capital_B);

	auto B_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
		return military::are_at_war(state, n_a, n_b);
	}, sphere_B, controller_capital_A);

	// US3AC10. diplomatic embargos
	// US3AC11. sphere joins embargo
	// US3AC12 subject joins embargo
	auto A_has_embargo =
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(controller_capital_B, market_leader_A)
		)
		||
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(market_leader_A, controller_capital_B)
		);
	A_joins_sphere_wide_embargo = A_has_embargo || A_joins_sphere_wide_embargo;

	auto B_has_embargo =
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(controller_capital_A, market_leader_B)
		)
		||
		state.world.unilateral_relationship_get_embargo(
			state.world.get_unilateral_relationship_by_unilateral_pair(market_leader_B, controller_capital_A)
		);
	B_joins_sphere_wide_embargo = B_has_embargo || B_joins_sphere_wide_embargo;

	// US3AC13
	auto merchant_cut = 1.f + (same_nation ? economy::merchant_cut_domestic : economy::merchant_cut_foreign);

	// US3AC14
	auto import_tariff_A = (same_nation || A_is_open_to_B) ? 0.f : effective_tariff_import_rate(state, n_A, A);
	auto export_tariff_A = (same_nation || A_is_open_to_B) ? 0.f : effective_tariff_export_rate(state, n_A, A);
	auto import_tariff_B = (same_nation || B_is_open_to_A) ? 0.f : effective_tariff_import_rate(state, n_B, B);
	auto export_tariff_B = (same_nation || B_is_open_to_A) ? 0.f : effective_tariff_export_rate(state, n_B, B);

	auto wage_A = (state.world.province_get_labor_price(capital_A, labor::no_education) + 0.00001f);
	auto wage_B = (state.world.province_get_labor_price(capital_B, labor::no_education) + 0.00001f);

	auto distance = invalid_trade_route_distance;
	auto land_distance = state.world.trade_route_get_land_distance(route);
	auto sea_distance = state.world.trade_route_get_sea_distance(route);

	distance = is_land_route ? std::min(distance, land_distance) : distance;
	distance = is_sea_route ? std::min(distance, sea_distance) : distance;

	auto trade_good_loss_mult = std::max(0.f, 1.f - trade_loss_per_distance_unit * distance);

	// US3AC2 we assume that 2 uneducated persons (1 from each market) can transport 1 unit of goods along path of 1 effective day length
	// we do it this way to avoid another assymetry in calculations
	auto transport_cost =
		distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good
		* (
			state.world.province_get_labor_price(capital_A, labor::no_education)
			+ state.world.province_get_labor_price(capital_B, labor::no_education)
		);

	result.trade_blocked = at_war
		|| A_joins_sphere_wide_embargo
		|| B_joins_sphere_wide_embargo
		|| trade_banned
		|| !state.world.trade_route_is_valid(route)
		|| (!is_sea_route && !is_land_route);


	result.commodity_is_not_tradable =
		state.world.commodity_get_money_rgo(cid)
		|| state.world.commodity_get_is_local(cid);

	result.commodity_is_not_discovered =
		state.world.commodity_get_rgo_amount(cid) > 0.f
		&& !state.world.commodity_get_actually_exists_in_nature(cid);

	if(!state.world.commodity_get_is_available_from_start(cid)) {
		auto unlocked_A = state.world.nation_get_unlocked_commodities(n_A, cid);
		auto unlocked_B = state.world.nation_get_unlocked_commodities(n_B, cid);
		result.commodity_is_not_discovered = result.commodity_is_not_discovered
			|| (!unlocked_A && !unlocked_B);
	}

	auto current_volume = state.world.trade_route_get_volume(route, cid);
	auto absolute_volume = std::abs(current_volume);

	auto effect_of_scale = std::max(trade_effect_of_scale_lower_bound, 1.f - absolute_volume * effect_of_transportation_scale);
	auto price_A_export = price(state, A, cid) * (1.f + export_tariff_A);
	auto price_B_export = price(state, B, cid) * (1.f + export_tariff_B);

	result.export_price[0] = price_A_export;
	result.export_price[1] = price_B_export;

	auto price_A_import = price(state, A, cid) * (1.f - import_tariff_A) * trade_good_loss_mult;
	auto price_B_import = price(state, B, cid) * (1.f - import_tariff_B) * trade_good_loss_mult;

	result.import_price[0] = price_A_import;
	result.import_price[1] = price_B_import;

	auto current_profit_A_to_B = price_B_import - price_A_export * merchant_cut - transport_cost * effect_of_scale;
	auto current_profit_B_to_A = price_A_import - price_B_export * merchant_cut - transport_cost * effect_of_scale;

	result.export_profit[0] = current_profit_A_to_B;
	result.export_profit[1] = current_profit_B_to_A;

	auto none_is_profiable = (current_profit_A_to_B <= 0.f) && (current_profit_B_to_A <= 0.f);
	auto max_expansion = 0.5f / (price_A_export + price_B_export) + absolute_volume * 0.05f + trade_route_min_shift;
	auto max_shrinking = -0.1f / (price_A_export + price_B_export) - absolute_volume * 0.005f;

	result.max_expansion = max_expansion;
	result.max_shrinking = max_shrinking;

	result.current_volume = current_volume;
	result.profit_score = -0.1f;
	result.profit = 0.f;
	result.divisor = 1.f;
	if(current_profit_A_to_B > 0.f) {
		result.profit = current_profit_A_to_B;
		result.divisor = price_B_import;
		if(current_volume > 0.f) {
			result.profit_score = current_profit_A_to_B / result.divisor;
		} else {
			result.profit_score = -current_profit_A_to_B / result.divisor;
		}
	} else if(current_profit_B_to_A > 0.f) {
		result.profit = current_profit_B_to_A;
		result.divisor = price_A_import;
		if(current_volume > 0.f) {
			result.profit_score = -current_profit_B_to_A / result.divisor;
		} else {
			result.profit_score = current_profit_B_to_A / result.divisor;
		}
	}

	result.base_change = result.profit_score * current_volume;

	if(current_profit_A_to_B > 0.f) {
		result.base_change += trade_route_min_shift;
	}
	if(current_profit_B_to_A < 0.f) {
		result.base_change -= trade_route_min_shift;
	}

	result.decay = 1.f - trade_base_decay;

	// expand the route slower if goods are not actually bought:

	result.actually_sold_in_origin =
		current_volume > 0.f
		? state.world.market_get_demand_satisfaction(A, cid)
		: state.world.market_get_demand_satisfaction(B, cid);

	result.expansion_multiplier = std::max(
		min_trade_expansion_multiplier,
		(result.actually_sold_in_origin - trade_demand_satisfaction_cutoff) * 2.f
	);

	result.final_change = result.base_change;

	if(current_volume > 0.f) {
		if(result.final_change > 0) {
			result.final_change = std::min(max_expansion, result.final_change * result.expansion_multiplier);
		} else {
			result.final_change = std::max(max_shrinking, result.final_change);
		}
	} else {
		if(result.final_change > 0) {
			result.final_change = std::min(-max_shrinking, result.final_change);
		} else {
			result.final_change = std::max(-max_expansion, result.final_change * result.expansion_multiplier);
		}
	}

	result.final_change = result.final_change - (1.f - result.decay) * current_volume;

	return result;
}

void update_trade_routes_volume(sys::state& state) {
	auto coastal_capital_buffer = ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>(state.world.state_instance_size());

	state.world.execute_parallel_over_state_instance([&](auto ids) {
		ve::apply([&](auto sid) {
			coastal_capital_buffer.set(sid, province::state_get_coastal_capital(state, sid));
		}, ids);
	});

	auto export_tariff_buffer = state.world.market_make_vectorizable_float_buffer();
	auto import_tariff_buffer = state.world.market_make_vectorizable_float_buffer();

	state.world.execute_serial_over_market([&](auto ids) {
		auto sids = state.world.market_get_zone_from_local_market(ids);
		auto nids = state.world.state_instance_get_nation_from_state_ownership(sids);
		ve::apply([&](auto nid, auto mid) {
			export_tariff_buffer.set(mid, effective_tariff_export_rate(state, nid, mid));
			import_tariff_buffer.set(mid, effective_tariff_import_rate(state, nid, mid));
		}, nids, ids);
	});

	state.world.execute_parallel_over_trade_route([&](auto trade_route) {
		auto A = ve::apply([&](auto route) {
			return state.world.trade_route_get_connected_markets(route, 0);
			}, trade_route);

		auto B = ve::apply([&](auto route) {
			return state.world.trade_route_get_connected_markets(route, 1);
		}, trade_route);

		auto s_A = state.world.market_get_zone_from_local_market(A);
		auto s_B = state.world.market_get_zone_from_local_market(B);

		auto n_A = state.world.state_instance_get_nation_from_state_ownership(s_A);
		auto n_B = state.world.state_instance_get_nation_from_state_ownership(s_B);

		auto capital_A = state.world.state_instance_get_capital(s_A);
		auto capital_B = state.world.state_instance_get_capital(s_B);

		auto port_A = coastal_capital_buffer.get(s_A);
		auto port_B = coastal_capital_buffer.get(s_B);

		auto controller_capital_A = state.world.province_get_nation_from_province_control(capital_A);
		auto controller_capital_B = state.world.province_get_nation_from_province_control(capital_B);

		auto controller_port_A = state.world.province_get_nation_from_province_control(port_A);
		auto controller_port_B = state.world.province_get_nation_from_province_control(port_B);

		auto sphere_A = state.world.nation_get_in_sphere_of(controller_capital_A);
		auto sphere_B = state.world.nation_get_in_sphere_of(controller_capital_B);

		auto overlord_A = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(controller_capital_A)
		);
		auto overlord_B = state.world.overlord_get_ruler(
			state.world.nation_get_overlord_as_subject(controller_capital_B)
		);

		auto has_overlord_mask_A = overlord_A != dcon::nation_id{};
		auto has_overlord_mask_B = overlord_B != dcon::nation_id{};
		auto has_sphere_mask_A = sphere_A != dcon::nation_id{};
		auto has_sphere_mask_B = sphere_B != dcon::nation_id{};

		// US3AC12. Subjects have embargo of overlords propagated onto them
		auto market_leader_A = ve::select(has_overlord_mask_A, overlord_A, ve::select(has_sphere_mask_A, sphere_A, n_A));
		auto market_leader_B = ve::select(has_overlord_mask_B, overlord_B, ve::select(has_sphere_mask_B, sphere_B, n_B));

		// US3AC15. Equal/unequal trade treaties
		auto A_open_to_B = ve::apply([&](auto n_a, auto n_b) {
			auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
			if(source_tariffs_rel) {
				auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
				// Enddt empty signalises revoken agreement
				// Enddt > cur_date signalises that the agreement can't be broken
				if(enddt) {
					return true;
				}
			}
			return false;
		}, market_leader_A, controller_capital_B);

		auto B_open_to_A = ve::apply([&](auto n_a, auto n_b) {
			auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);

			if(target_tariffs_rel) {
				auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
				if(enddt) {
					return true;
				}
			}

			return false;
		}, market_leader_B, controller_capital_A);

		auto A_is_open_to_B = sphere_A == controller_capital_B || overlord_A == controller_capital_B || A_open_to_B;
		auto B_is_open_to_A = sphere_B == controller_capital_A || overlord_B == controller_capital_A || B_open_to_A;

		// these are not needed at the moment
		// because overlord and subject are always in the same war

		/*
		auto A_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_A, controller_capital_B);

		auto B_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_B, controller_capital_A);
		*/

		// US3AC17. if market capital controller is at war with market coastal controller is different
		// or it's actually blockaded
		// consider province blockaded

		ve::mask_vector port_occupied_A = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, controller_capital_A, controller_port_A);
		ve::mask_vector port_occupied_B = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, controller_capital_B, controller_port_B);

		// US3AC16
		ve::mask_vector is_A_blockaded = state.world.province_get_is_blockaded(port_A) || port_occupied_A;
		ve::mask_vector is_B_blockaded = state.world.province_get_is_blockaded(port_B) || port_occupied_B;

		// US3AC9. if market capital controllers are at war then we will break the link
		auto at_war = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, controller_capital_A, controller_capital_B);

		auto is_A_civ = state.world.nation_get_is_civilized(n_A);
		auto is_B_civ = state.world.nation_get_is_civilized(n_B);

		auto is_sea_route = state.world.trade_route_get_is_sea_route(trade_route);
		auto is_land_route = state.world.trade_route_get_is_land_route(trade_route);
		auto same_nation = controller_capital_A == controller_capital_B;

		// US3AC7 US3AC8 Ban international sea routes or international land routes based on the corresponding modifiers
		auto A_bans_sea_trade = state.world.nation_get_modifier_values(n_A, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
		auto B_bans_sea_trade = state.world.nation_get_modifier_values(n_B, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
		auto sea_trade_banned = A_bans_sea_trade || B_bans_sea_trade;
		auto A_bans_land_trade = state.world.nation_get_modifier_values(n_A, sys::national_mod_offsets::disallow_land_trade) > 0.f;
		auto B_bans_land_trade = state.world.nation_get_modifier_values(n_B, sys::national_mod_offsets::disallow_land_trade) > 0.f;
		auto land_trade_banned = A_bans_land_trade || B_bans_land_trade;
		auto trade_banned = (is_sea_route && sea_trade_banned && !same_nation) || (is_land_route && land_trade_banned && !same_nation);

		is_sea_route = is_sea_route && !is_A_blockaded && !is_B_blockaded;

		// US3AC9. Wartime embargoes
		// US3AC10. diplomatic embargos
		// US3AC11. sphere joins embargo
		// US3AC12 subject joins embargo
		auto A_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, sphere_A, controller_capital_B);

		auto B_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, sphere_B, controller_capital_A);

		auto A_has_embargo = ve::apply([&](auto n_a, auto n_b) {
			auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
			auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);

			return state.world.unilateral_relationship_get_embargo(source_tariffs_rel) || state.world.unilateral_relationship_get_embargo(target_tariffs_rel);
		}, market_leader_A, controller_capital_B);

		A_joins_sphere_wide_embargo = A_has_embargo || A_joins_sphere_wide_embargo;

		auto B_has_embargo = ve::apply([&](auto n_a, auto n_b) {
			auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_b, n_a);
			auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_a, n_b);

			return state.world.unilateral_relationship_get_embargo(source_tariffs_rel) || state.world.unilateral_relationship_get_embargo(target_tariffs_rel);
		}, market_leader_B, controller_capital_A);

		B_joins_sphere_wide_embargo = B_has_embargo || B_joins_sphere_wide_embargo;

		// these are not needed at the moment
		// because overlord and subject are always in the same war

		/*
		auto A_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_A, controller_capital_B);

		auto B_joins_overlord_embargo = ve::apply([&](auto n_a, auto n_b) {
			return military::are_at_war(state, n_a, n_b);
		}, overlord_B, controller_capital_A);
		*/


		auto merchant_cut = ve::select(same_nation, ve::fp_vector{ 1.f + economy::merchant_cut_domestic }, ve::fp_vector{ 1.f + economy::merchant_cut_foreign });

		auto import_tariff_A = ve::select(same_nation || A_is_open_to_B, ve::fp_vector{ 0.f }, import_tariff_buffer.get(A));
		auto export_tariff_A = ve::select(same_nation || A_is_open_to_B, ve::fp_vector{ 0.f }, export_tariff_buffer.get(A));
		auto import_tariff_B = ve::select(same_nation || B_is_open_to_A, ve::fp_vector{ 0.f }, import_tariff_buffer.get(B));
		auto export_tariff_B = ve::select(same_nation || B_is_open_to_A, ve::fp_vector{ 0.f }, export_tariff_buffer.get(B));

		auto wage_A = (state.world.province_get_labor_price(capital_A, labor::no_education) + 0.00001f);
		auto wage_B = (state.world.province_get_labor_price(capital_B, labor::no_education) + 0.00001f);

		ve::fp_vector distance = invalid_trade_route_distance;
		auto land_distance = state.world.trade_route_get_land_distance(trade_route);
		auto sea_distance = state.world.trade_route_get_sea_distance(trade_route);

		distance = ve::select(is_land_route, ve::min(distance, land_distance), distance);
		distance = ve::select(is_sea_route, ve::min(distance, sea_distance), distance);

		ve::apply([&](auto value) {
			assert(std::isfinite(value));
		}, distance);

		state.world.trade_route_set_distance(trade_route, distance);

		// US3AC18
		auto trade_good_loss_mult = ve::max(0.f, 1.f - trade_loss_per_distance_unit * distance);

		// US3AC2. we assume that 2 uneducated persons (1 from each market) can transport 1 unit of goods along path of 1 effective day length
		// we do it this way to avoid another assymetry in calculations
		auto transport_cost =
			distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good
			* (
				state.world.province_get_labor_price(capital_A, labor::no_education)
				+ state.world.province_get_labor_price(capital_B, labor::no_education)
			);

		auto reset_route = at_war
			|| A_joins_sphere_wide_embargo
			|| B_joins_sphere_wide_embargo
			|| trade_banned
			|| !ve::apply([&](auto r) { return state.world.trade_route_is_valid(r); }, trade_route)
			|| (!is_sea_route && !is_land_route);

		for(auto c : state.world.in_commodity) {
			// US3AC19
			if(state.world.commodity_get_money_rgo(c) || state.world.commodity_get_is_local(c)) {
				continue;
			}
			if(
				state.world.commodity_get_rgo_amount(c) > 0.f
				&& !state.world.commodity_get_actually_exists_in_nature(c)
			) {
				continue;
			}

			// US3AC20. 
			auto unlocked_A = state.world.nation_get_unlocked_commodities(n_A, c);
			auto unlocked_B = state.world.nation_get_unlocked_commodities(n_B, c);

			auto reset_route_commodity = reset_route;

			if(!state.world.commodity_get_is_available_from_start(c)) {
				reset_route_commodity = reset_route_commodity
					|| (!unlocked_A && !unlocked_B);
			}

			//state.world.execute_serial_over_trade_route([&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, c);

			auto absolute_volume = ve::abs(current_volume);
			//auto sat = state.world.market_get_direct_demand_satisfaction(origin, c);

			// US3AC21 effect of scale
			// volume reduces transport costs
			auto effect_of_scale = ve::max(trade_effect_of_scale_lower_bound, 1.f - absolute_volume * effect_of_transportation_scale);

			auto price_A_export = ve_price(state, A, c) * (1.f + export_tariff_A);
			auto price_B_export = ve_price(state, B, c) * (1.f + export_tariff_B);

			auto price_A_import = ve_price(state, A, c) * (1.f - import_tariff_A) * trade_good_loss_mult;
			auto price_B_import = ve_price(state, B, c) * (1.f - import_tariff_B) * trade_good_loss_mult;

			auto current_profit_A_to_B = price_B_import - price_A_export * merchant_cut - transport_cost * effect_of_scale;
			auto current_profit_B_to_A = price_A_import - price_B_export * merchant_cut - transport_cost * effect_of_scale;

			auto none_is_profiable = (current_profit_A_to_B <= 0.f) && (current_profit_B_to_A <= 0.f);

			// we make changes slow to ensure more or less smooth changes
			// otherwise assumption of stable demand breaks
			// constant term is there to allow moving away from 0
			auto max_expansion = 0.5f / (price_A_export + price_B_export) + absolute_volume * 0.05f + trade_route_min_shift;
			auto max_shrinking = -0.1f / (price_A_export + price_B_export) - absolute_volume * 0.005f;

			auto profit_score = ve::select(
				current_profit_A_to_B > 0.f,
				ve::select(
					current_volume > 0.f,
					current_profit_A_to_B / price_B_import,
					-current_profit_A_to_B / price_B_import
				),
				ve::select(
					current_profit_B_to_A > 0.f,
					ve::select(
						current_volume > 0.f,
						-current_profit_B_to_A / price_A_import,
						current_profit_B_to_A / price_A_import
					),
					ve::fp_vector{ -0.1f }
				)
			);

			auto base_shift = ve::select(
				current_profit_A_to_B > 0.f,
				trade_route_min_shift,
				ve::select(
					current_profit_B_to_A > 0.f,
					ve::fp_vector{ -trade_route_min_shift },
					ve::fp_vector{ 0.f }
				)
			);

			auto change = current_volume * profit_score + base_shift;

			// modifier for trade to slowly decay to create soft limit on transportation
			// essentially, regularisation of trade weights, but can lead to weird effects
			ve::fp_vector decay = 1.f - trade_base_decay;

			// expand the route slower if goods are not actually bought:
			auto bought_A = state.world.market_get_demand_satisfaction(A, c);
			auto bought_B = state.world.market_get_demand_satisfaction(B, c);
			auto bought = ve::select(
				current_volume > 0.f,
				bought_A,
				bought_B
			);
			change = ve::select(
				change * current_volume >= 0.f, // change and volume are collinear
				change * ve::max(min_trade_expansion_multiplier, (bought - trade_demand_satisfaction_cutoff) * 2.f),
				change
			);

			change = ve::select(current_volume > 0.05f,
				ve::min(ve::max(change, max_shrinking), max_expansion),
				ve::select(current_volume < -0.05f,
					ve::min(ve::max(change, -max_expansion), -max_shrinking),
					ve::min(ve::max(change, -max_expansion), max_expansion)
				)
			);

			// decay is unconditional safeguard against high weights
			change = change - current_volume * (1.f - decay);

			auto new_volume = ve::select(reset_route_commodity, 0.f, current_volume + change);

			state.world.trade_route_set_volume(trade_route, c, new_volume);

			ve::apply([&](auto route) {
				assert(std::isfinite(state.world.trade_route_get_volume(route, c)));
			}, trade_route);
		}
	});
}


void update_trade_routes_consumption(sys::state& state) {
	auto const total_commodities = state.world.commodity_size();

	// register trade demand on goods
	concurrency::parallel_for(uint32_t(0), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };

		if(state.world.commodity_get_money_rgo(cid)) {
			return;
		}

		state.world.for_each_trade_route([&](auto trade_route) {
			auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
			auto origin =
				current_volume > 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);
			auto target =
				current_volume <= 0.f
				? state.world.trade_route_get_connected_markets(trade_route, 0)
				: state.world.trade_route_get_connected_markets(trade_route, 1);

			auto actually_bought_at_origin = state.world.market_get_direct_demand_satisfaction(origin, cid);

			// reduce volume in case of low supply
			// do not reduce volume too much to avoid hardcore jumps
			current_volume = current_volume * std::max(0.999999f, actually_bought_at_origin);
			state.world.trade_route_set_volume(trade_route, cid, current_volume);

			auto absolute_volume = std::abs(current_volume);

			auto s_origin = state.world.market_get_zone_from_local_market(origin);
			auto s_target = state.world.market_get_zone_from_local_market(target);

			auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
			auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);

			register_demand(state, origin, cid, absolute_volume, economy_reason::trade);
		});
	});

	// US3AC2 register trade demand on transportation labor:
	// money are paid during calculation of trade route profits and actual movement of goods
	state.world.for_each_trade_route([&](auto trade_route) {

		auto A = state.world.trade_route_get_connected_markets(trade_route, 0);
		auto B = state.world.trade_route_get_connected_markets(trade_route, 1);

		auto A_capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(A));
		auto B_capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(B));
		
		auto total_demanded_labor = trade_route_labour_demand(state, trade_route, A_capital, B_capital);

		state.world.province_get_labor_demand(A_capital, labor::no_education) += total_demanded_labor;
		state.world.province_get_labor_demand(B_capital, labor::no_education) += total_demanded_labor;
		assert(std::isfinite(total_demanded_labor));
		assert(std::isfinite(state.world.province_get_labor_demand(A_capital, labor::no_education)));
		assert(std::isfinite(state.world.province_get_labor_demand(B_capital, labor::no_education)));
	});

	// US3AC3 register demand on local transportation/accounting due to trade
	// all trade generates uneducated labor demand for goods transport locally
	// labor demand satisfaction does not set limits on transportation: it would be way too jumpy
	// we assume that 1 human could move 100 units of goods daily locally

	state.world.for_each_market([&](auto market) {
		auto capital = state.world.state_instance_get_capital(state.world.market_get_zone_from_local_market(market));
		auto base_cargo_transport_demand = transportation_inside_market_labor_demand(state, market, capital);

		// auto soft_transport_demand_limit = state.world.market_get_max_throughput(market);
		//if(base_cargo_transport_demand > soft_transport_demand_limit) {
		//	base_cargo_transport_demand = base_cargo_transport_demand * base_cargo_transport_demand / soft_transport_demand_limit;
		//}

		state.world.province_get_labor_demand(capital, labor::no_education) += base_cargo_transport_demand / 100.f;
		assert(std::isfinite(base_cargo_transport_demand));
		assert(std::isfinite(state.world.province_get_labor_demand(capital, labor::no_education)));
		// proceed payments:
		state.world.market_get_stockpile(market, money) -=
			base_cargo_transport_demand
			* state.world.province_get_labor_demand_satisfaction(capital, labor::no_education)
			* state.world.province_get_labor_price(capital, labor::no_education);
	});
}

// CAUTION: when we generate trade demand for a good, we promise to pay money to local producers during the next tick
// it means that during economy update, rgo profits were calculated according to this promise
// so local producers ALREADY received money for their production and local market take only a "merchant cut"

trade_and_tariff explain_trade_route_commodity(
	sys::state& state,
	dcon::trade_route_id trade_route,
	tariff_data& additional_data,
	dcon::commodity_id cid
) {
	auto current_volume = state.world.trade_route_get_volume(trade_route, cid);

	auto origin_i = current_volume > 0.f ? 0 : 1;
	auto target_i = current_volume > 0.f ? 1 : 0;
	auto origin = state.world.trade_route_get_connected_markets(trade_route, origin_i);
	auto target = state.world.trade_route_get_connected_markets(trade_route, target_i);
	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
	auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);
	bool same_nation = n_origin == n_target;

	auto origin_is_open_to_target = additional_data.applies_tariff[origin_i];
	auto target_is_open_to_origin = additional_data.applies_tariff[target_i];

	auto price_origin = price(state, origin, cid);
	auto price_target = price(state, target, cid);

	auto sat = state.world.market_get_direct_demand_satisfaction(origin, cid);

	auto absolute_volume = sat * std::abs(current_volume);

	auto import_amount = absolute_volume * additional_data.loss;
	auto effect_of_scale = std::max(trade_effect_of_scale_lower_bound, 1.f - absolute_volume * effect_of_transportation_scale);
	auto transport_cost = additional_data.base_distance_cost * effect_of_scale;
	auto export_tariff = origin_is_open_to_target ? 0.f : additional_data.export_tariff[origin_i];
	auto import_tariff = target_is_open_to_origin ? 0.f : additional_data.import_tariff[target_i];

	if(same_nation) {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,

			.tariff_origin = 0.f,
			.tariff_target = 0.f,

			.tariff_rate_origin = 0.f,
			.tariff_rate_target = 0.f,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = additional_data.loss,
			.distance = additional_data.distance,

			.payment_per_unit = price_origin * (1 + economy::merchant_cut_domestic) + transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_domestic
		};
	} else {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,
			.tariff_origin = absolute_volume * price_origin * export_tariff,
			.tariff_target = import_amount * price_target * import_tariff,

			.tariff_rate_origin = export_tariff,
			.tariff_rate_target = import_tariff,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = additional_data.loss,
			.distance = additional_data.distance,

			.payment_per_unit = price_origin
				* (1.f + export_tariff)
				* (1 + economy::merchant_cut_foreign)
				+ price(state, target, cid)
				* import_tariff
				+ transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_foreign
		};
	}
}



trade_and_tariff explain_trade_route_commodity(sys::state& state, dcon::trade_route_id trade_route, dcon::commodity_id cid) {
	auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
	auto origin =
		current_volume > 0.f
		? state.world.trade_route_get_connected_markets(trade_route, 0)
		: state.world.trade_route_get_connected_markets(trade_route, 1);
	auto target =
		current_volume <= 0.f
		? state.world.trade_route_get_connected_markets(trade_route, 0)
		: state.world.trade_route_get_connected_markets(trade_route, 1);

	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
	auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
	bool same_nation = n_origin == n_target;
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);
	auto controller_capital_origin = state.world.province_get_nation_from_province_control(capital_origin);
	auto controller_capital_target = state.world.province_get_nation_from_province_control(capital_target);
	auto sphere_origin = state.world.nation_get_in_sphere_of(controller_capital_origin);
	auto sphere_target = state.world.nation_get_in_sphere_of(controller_capital_target);

	auto overlord_origin = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(controller_capital_origin)
	);
	auto overlord_target = state.world.overlord_get_ruler(
		state.world.nation_get_overlord_as_subject(controller_capital_target)
	);

	// Equal/unequal trade agreements
	// Rel source if obliged towards target
	auto source_applies_tariffs = true;
	auto target_applies_tariffs = true;
	auto source_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_target, n_origin);
	auto target_tariffs_rel = state.world.get_unilateral_relationship_by_unilateral_pair(n_origin, n_target);
	if(source_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(source_tariffs_rel);
		if(state.current_date < enddt) {
			source_applies_tariffs = false;
		}
	}
	if(target_tariffs_rel) {
		auto enddt = state.world.unilateral_relationship_get_no_tariffs_until(target_tariffs_rel);
		if(state.current_date < enddt) {
			target_applies_tariffs = false;
		}
	}

	auto origin_is_open_to_target = sphere_origin == controller_capital_target || overlord_origin == controller_capital_target || !source_applies_tariffs;
	auto target_is_open_to_origin = sphere_target == controller_capital_origin || overlord_target == controller_capital_origin || !target_applies_tariffs;

	auto sat = state.world.market_get_direct_demand_satisfaction(origin, cid);

	auto absolute_volume = sat * std::abs(current_volume);
	auto distance = state.world.trade_route_get_distance(trade_route);

	auto trade_good_loss_mult = std::max(0.f, 1.f - trade_loss_per_distance_unit * distance);
	auto import_amount = absolute_volume * trade_good_loss_mult;

	auto effect_of_scale = std::max(trade_effect_of_scale_lower_bound, 1.f - absolute_volume * effect_of_transportation_scale);
	auto transport_cost =
		distance / trade_distance_covered_by_pair_of_workers_per_unit_of_good
		* (
			state.world.province_get_labor_price(capital_origin, labor::no_education)
			+ state.world.province_get_labor_price(capital_target, labor::no_education)
		)
		* effect_of_scale;

	auto export_tariff = origin_is_open_to_target ? 0.f : effective_tariff_export_rate(state, n_origin, origin);
	auto import_tariff = target_is_open_to_origin ? 0.f : effective_tariff_import_rate(state, n_target, target);

	auto price_origin = price(state, origin, cid);
	auto price_target = price(state, target, cid);

	if(same_nation) {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,

			.tariff_origin = 0.f,
			.tariff_target = 0.f,

			.tariff_rate_origin = 0.f,
			.tariff_rate_target = 0.f,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = trade_good_loss_mult,
			.distance = distance,

			.payment_per_unit = price_origin * (1 + economy::merchant_cut_domestic) + transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_domestic
		};
	} else {
		return {
			.origin = origin,
			.target = target,
			.origin_nation = n_origin,
			.target_nation = n_target,

			.amount_origin = absolute_volume,
			.amount_target = import_amount,
			.tariff_origin = absolute_volume * price_origin * export_tariff,
			.tariff_target = import_amount * price_target * import_tariff,

			.tariff_rate_origin = export_tariff,
			.tariff_rate_target = import_tariff,

			.price_origin = price_origin,
			.price_target = price_target,

			.transport_cost = transport_cost,
			.transportaion_loss = trade_good_loss_mult,
			.distance = distance,

			.payment_per_unit = price_origin
				* (1.f + export_tariff)
				* (1 + economy::merchant_cut_foreign)
				+ price(state, target, cid)
				* import_tariff
				+ transport_cost,
			// the rest of payment is handled as satisfaction of generic demand
			.payment_received_per_unit = price_origin * economy::merchant_cut_foreign
		};
	}
}

// DO NOT USE OUTSIDE OF UI
std::vector<trade_breakdown_item> explain_national_tariff(sys::state& state, dcon::nation_id n, bool import_flag, bool export_flag) {
	std::vector<trade_breakdown_item> result;
	auto buffer_volume_per_nation = state.world.nation_make_vectorizable_float_buffer();
	auto buffer_tariff_per_nation = state.world.nation_make_vectorizable_float_buffer();

	state.world.for_each_commodity([&](dcon::commodity_id cid) {
		state.world.execute_serial_over_nation([&](auto nids) {
			buffer_volume_per_nation.set(nids, 0.f);
			buffer_tariff_per_nation.set(nids, 0.f);
		});

		state.world.nation_for_each_state_ownership(n, [&](auto sid) {
			auto mid = state.world.state_instance_get_market_from_local_market(state.world.state_ownership_get_state(sid));
			state.world.market_for_each_trade_route(mid, [&](auto trade_route) {
				trade_and_tariff route_data = explain_trade_route_commodity(state, trade_route, cid);

				if(!export_flag) {
					if(route_data.origin == mid) {
						return;
					}
				}

				if(!import_flag) {
					if(route_data.target == mid) {
						return;
					}
				}

				if(import_flag && route_data.target == mid) {
					buffer_volume_per_nation.get(route_data.origin_nation) += route_data.amount_target;
					buffer_tariff_per_nation.get(route_data.origin_nation) += route_data.tariff_target;
				}

				if(export_flag && route_data.origin == mid) {
					buffer_volume_per_nation.get(route_data.target_nation) += route_data.amount_origin;
					buffer_tariff_per_nation.get(route_data.target_nation) += route_data.tariff_origin;
				}
			});
		});

		state.world.for_each_nation([&](auto nid) {
			trade_breakdown_item item = {
				.trade_partner = nid,
				.commodity = cid,
				.traded_amount = buffer_volume_per_nation.get(nid),
				.tariff = buffer_tariff_per_nation.get(nid)
			};

			if(item.traded_amount == 0.f || item.tariff < 0.001f) {
				return;
			}

			result.push_back(item);
		});
	});

	return result;
}

}
