#include "economy_trade_routes.hpp"
#include "economy.hpp"
#include "economy_stats.hpp"
#include "system_state.hpp"
#include "economy_government.hpp"
#include "adaptive_ve.hpp"
#include "province_templates.hpp"
#include "advanced_province_buildings.hpp"
#include "economy_constants.hpp"
#include "economy_templates.hpp"
#include "money.hpp"
#include "province.hpp"
#include "price.hpp"

// implements trade routes
// when changing logic of trade routes please update it everywhere
// due to performance reasons we have to duplicate it

namespace economy {

void make_trade_volume_tooltip(
	sys::state& state,
	text::columnar_layout& contents,
	dcon::trade_route_id route,
	dcon::commodity_id cid,
	dcon::market_id point_of_view
) {
	auto prediction = predict_trade_route_volume_change(state, route, cid);

	auto multiplier = 1.f;
	auto B = state.world.trade_route_get_target(route);
	if(B == point_of_view) {
		multiplier = -1.f;
	}

	int this_id = 0;
	if(B == point_of_view) {
		this_id = 1;
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
		text::variable_type::val, text::fp_two_places{ multiplier * prediction.profit_score }
	);

	text::add_line(state, contents, "trade_route_volume_transport_cost",
		text::variable_type::val, text::format_money( prediction.transport_cost_per_unit )
	);
	text::add_line(state, contents, "trade_route_volume_export_price",
		text::variable_type::val, text::format_money(prediction.export_price)
	);
	text::add_line(state, contents, "trade_route_volume_import_price",
		text::variable_type::val, text::format_money(prediction.import_price)
	);

	//text::add_line(state, contents, "trade_route_volume_profit_score_export",
	//	text::variable_type::val, text::fp_two_places{ prediction.export_profit[this_id] / prediction.export_price[this_id] },
	//	text::variable_type::x, text::fp_two_places{ prediction.export_profit[this_id] },
	//	text::variable_type::y, text::fp_two_places{ prediction.export_price[this_id] }
	//);

	//text::add_line(state, contents, "trade_route_volume_profit_score_import",
	//	text::variable_type::val, text::fp_two_places{ -prediction.export_profit[1 - this_id] / prediction.export_price[1 - this_id] },
	//	text::variable_type::x, text::fp_two_places{ -prediction.export_profit[1 - this_id] },
	//	text::variable_type::y, text::fp_two_places{ prediction.export_price[1 - this_id] }
	//);

	text::add_line(state, contents, "trade_route_volume_base_change",
		text::variable_type::val, text::fp_two_places{ multiplier * prediction.base_change },
		text::variable_type::x, text::fp_two_places{ std::abs(prediction.current_volume) }
	);

	text::add_line(state, contents, "trade_route_volume_expansion_multiplier",
		text::variable_type::val, text::fp_two_places{ prediction.expansion_multiplier }
	);
	text::add_line(state, contents, "trade_route_volume_transport_availability",
		text::variable_type::val, text::fp_two_places{ prediction.transport_availability }
	);
	text::add_line(state, contents, "trade_route_volume_budget_availability",
		text::variable_type::val, text::fp_two_places{ prediction.import_ratio }
	);
	text::add_line(state, contents, "trade_route_volume_risk",
		text::variable_type::val, text::fp_two_places{ prediction.risk }
	);
	text::add_line(state, contents, "trade_route_confidence_export",
		text::variable_type::val, text::fp_two_places{ prediction.export_price_confidence }
	);
	text::add_line(state, contents, "trade_route_confidence_import",
		text::variable_type::val, text::fp_two_places{ prediction.import_price_confidence }
	);

	text::add_line(state, contents, "trade_route_volume_decay",
		text::variable_type::val, text::fp_two_places{ multiplier * prediction.decay }
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
	// Subjects have embargo of overlords propagated onto them
	auto market_leader_A = nations::get_market_leader(state, n_A);
	auto market_leader_B = nations::get_market_leader(state, n_B);

	// if market capital controllers are at war then we will break the link
	auto at_war = military::are_at_war(state, n_A, n_B);

	auto is_A_civ = state.world.nation_get_is_civilized(n_A);
	auto is_B_civ = state.world.nation_get_is_civilized(n_B);

	// sphere joins embargo
	// subject joins embargo
	// diplomatic embargos
	auto A_joins_sphere_wide_embargo = military::are_at_war(state, market_leader_A, market_leader_B);
	auto B_joins_sphere_wide_embargo = military::are_at_war(state, market_leader_B, market_leader_A);

	auto A_has_embargo = non_war_embargo_status(state, n_A, n_B, market_leader_A, market_leader_B);

	auto B_has_embargo = non_war_embargo_status(state, n_B, n_A, market_leader_B, market_leader_A);

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
	sys::state const& state, dcon::trade_route_id route, dcon::commodity_id cid
) {
	trade_route_volume_change_reasons result{
		.export_price = 0.f,
		.import_price = 0.f,
		.export_profit = 0.f,

		.trade_blocked = false,
		.commodity_is_not_tradable = false
	};

	auto A = state.world.trade_route_get_origin(route);
	auto B = state.world.trade_route_get_target(route);
	auto s_A = state.world.market_get_zone_from_local_market(A);
	auto s_B = state.world.market_get_zone_from_local_market(B);
	auto formal_n_A = state.world.state_instance_get_nation_from_state_ownership(s_A);
	auto formal_n_B = state.world.state_instance_get_nation_from_state_ownership(s_B);
	auto capital_A = state.world.state_instance_get_capital(s_A);
	auto capital_B = state.world.state_instance_get_capital(s_B);
	auto port_A = province::state_get_coastal_capital(state, s_A);
	auto port_B = province::state_get_coastal_capital(state, s_B);
	auto controller_capital_A = state.world.province_get_nation_from_province_control(capital_A);
	auto controller_capital_B = state.world.province_get_nation_from_province_control(capital_B);
	controller_capital_A = controller_capital_A ? controller_capital_A : formal_n_A;
	controller_capital_B = controller_capital_B ? controller_capital_B : formal_n_B;
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
	// Subjects have embargo of overlords propagated onto them
	auto market_leader_A = nations::get_market_leader(state, controller_capital_A);
	auto market_leader_B = nations::get_market_leader(state, controller_capital_B);

	// Equal/unequal trade treaties
	auto A_is_open_to_B = !state.world.trade_route_get_is_tariff_applied_origin(route);
	auto B_is_open_to_A = !state.world.trade_route_get_is_tariff_applied_target(route);

	auto port_occupied_A = military::are_at_war(state, controller_capital_A, controller_port_A);
	auto port_occupied_B = military::are_at_war(state, controller_capital_B, controller_port_B);

	auto is_A_blockaded = state.world.province_get_is_blockaded(port_A) || port_occupied_A;
	auto is_B_blockaded = state.world.province_get_is_blockaded(port_B) || port_occupied_B;

	// if market capital controllers are at war then we will break the link
	auto at_war = military::are_at_war(state, controller_capital_A, controller_capital_B);

	auto is_A_civ = state.world.nation_get_is_civilized(controller_capital_A);
	auto is_B_civ = state.world.nation_get_is_civilized(controller_capital_B);

	auto is_sea_route = state.world.trade_route_get_is_sea_route(route);
	auto same_nation = controller_capital_A == controller_capital_B;

	// US3AC7. Ban international sea routes or international land routes based on the corresponding modifiers
	auto A_bans_sea_trade = state.world.nation_get_modifier_values(controller_capital_A, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
	auto B_bans_sea_trade = state.world.nation_get_modifier_values(controller_capital_B, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
	auto sea_trade_banned = A_bans_sea_trade || B_bans_sea_trade;
	// US3AC8. Ban international sea routes or international land routes based on the corresponding modifiers
	auto A_bans_land_trade = state.world.nation_get_modifier_values(controller_capital_A, sys::national_mod_offsets::disallow_land_trade) > 0.f;
	auto B_bans_land_trade = state.world.nation_get_modifier_values(controller_capital_B, sys::national_mod_offsets::disallow_land_trade) > 0.f;
	auto land_trade_banned = A_bans_land_trade || B_bans_land_trade;
	auto trade_banned = (is_sea_route && sea_trade_banned && !same_nation) || (!is_sea_route && land_trade_banned && !same_nation);

	is_sea_route = is_sea_route && !is_A_blockaded && !is_B_blockaded;

	// US3AC9. Wartime embargoes
	auto A_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
		return military::are_at_war(state, n_a, n_b);
	}, market_leader_A, market_leader_B);

	auto B_joins_sphere_wide_embargo = ve::apply([&](auto n_a, auto n_b) {
		return military::are_at_war(state, n_a, n_b);
	}, market_leader_B, market_leader_A);

	// US3AC10. diplomatic embargos
	// US3AC11. sphere joins embargo
	// US3AC12 subject joins embargo
	auto A_has_embargo = non_war_embargo_status(state, controller_capital_A, controller_capital_B, market_leader_A, market_leader_B);
	A_joins_sphere_wide_embargo = A_has_embargo || A_joins_sphere_wide_embargo;

	auto B_has_embargo = non_war_embargo_status(state, controller_capital_B, controller_capital_A, market_leader_B, market_leader_A);
	B_joins_sphere_wide_embargo = B_has_embargo || B_joins_sphere_wide_embargo;

	// US3AC13
	auto merchant_cut = same_nation ? economy::merchant_cut_domestic : economy::merchant_cut_foreign;

	// US3AC14
	auto export_tariff = (same_nation || A_is_open_to_B) ? 0.f : effective_tariff_export_rate(state, controller_capital_A, A);
	auto import_tariff = (same_nation || B_is_open_to_A) ? 0.f : effective_tariff_import_rate(state, controller_capital_B, B);

	auto distance = state.world.trade_route_get_distance_km(route);
	auto trade_good_loss_mult = std::max(0.f, 1.f - trade_goods_lost_per_km * distance);

	auto current_volume = state.world.trade_route_get_volume(route, cid);
	result.current_volume = current_volume;

	auto owner = state.world.trade_route_get_owner(route);

	auto transport_cost_per_distance_weight = state.world.market_get_land_transportation_price(owner);
	if(is_sea_route) {
		transport_cost_per_distance_weight = state.world.market_get_naval_transportation_price(owner);
	}

	auto transport_cost_per_weight = distance * transport_cost_per_distance_weight;

	result.transport_cost_per_unit = transport_cost_per_weight;

	result.trade_blocked = at_war
		|| A_joins_sphere_wide_embargo
		|| B_joins_sphere_wide_embargo
		|| trade_banned
		|| !state.world.trade_route_is_valid(route);

	result.commodity_is_not_tradable =
		state.world.commodity_get_money_rgo(cid)
		|| state.world.commodity_get_is_local(cid);

	result.commodity_is_not_discovered =
		state.world.commodity_get_rgo_amount(cid) > 0.f
		&& !state.world.commodity_get_actually_exists_in_nature(cid);

	auto price_export_adjusted = price(state, A, cid) * (1.f + export_tariff + merchant_cut);
	result.export_price = price_export_adjusted;

	auto price_import_adjusted = price(state, B, cid) * (1.f - import_tariff) * trade_good_loss_mult;
	result.import_price = price_import_adjusted;

	auto c = cid;

	ve::fp_vector total_count = 0.f;
	ve::fp_vector total_reality_buy = 0.f;
	ve::fp_vector total_reality_sell = 0.f;
	ve::fp_vector total_confidence = 0.f;

	state.world.execute_serial_over_market([&](auto market) {
		auto valid = ve::apply([&](auto m) {
			return state.world.market_is_valid(m);
		}, market);
		total_count = total_count + ve::select(valid, ve::fp_vector{1.f}, ve::fp_vector{0.f});
		total_reality_buy = total_reality_buy + ve::select(valid, state.world.market_get_expected_probability_to_buy(market, cid), 0.f);
		total_reality_sell = total_reality_sell + ve::select(valid, state.world.market_get_expected_probability_to_sell(market, cid), 0.f);
		total_confidence = total_confidence + ve::select(valid,
			state.world.market_get_aggregated_supply_history(market, cid)
			+ state.world.market_get_aggregated_demand_history(market, cid)
			, 0.f
		);
	});

	auto buy_optimism =0.5f + 0.5f * total_reality_buy.reduce() / (total_count.reduce() + 1.f);
	auto sell_optimism = 0.5f + 0.5f * total_reality_sell.reduce() / (total_count.reduce() + 1.f);
	auto optimism_confidence = 2.f + 2.f * (total_confidence.reduce() + 1.f) / (total_count.reduce() + 1.f);

	auto expected_to_buy = state.world.market_get_expected_probability_to_buy(A, c);
	auto expected_to_sell = state.world.market_get_expected_probability_to_sell(B, c);

	auto pessimism_confidence_A = 0.5f * (state.world.market_get_aggregated_demand_history(A, c) + state.world.market_get_aggregated_supply_history(A, c));
	auto pessimism_confidence_B = 0.5f * (state.world.market_get_aggregated_demand_history(B, c) + state.world.market_get_aggregated_supply_history(B, c));

	auto sold_boundary = stockpile_to_supply / (stockpile_spoilage + stockpile_to_supply);

	auto confidence_export = std::max(0.5f, state.world.market_get_price_confidence(A, c));
	auto confidence_import = std::max(0.5f, state.world.market_get_price_confidence(B, c));

	result.export_price_confidence = confidence_export;
	result.import_price_confidence = confidence_import;

	auto price_export = price(state, A, c) / confidence_export;
	auto price_import = price(state, B, c) * confidence_import;

	auto transport_availability = is_sea_route
		? state.world.market_get_naval_transportation_demand_satisfaction(owner)
		: state.world.market_get_land_transportation_demand_satisfaction(owner);

	auto spend = price_export * merchant_cut + transport_cost_per_weight;

	auto sell_rate_perception_B = (optimism_confidence * std::max(expected_to_sell, sell_optimism) + pessimism_confidence_B * expected_to_sell);
	auto buy_rate_perception_A = (optimism_confidence * std::max(expected_to_buy, buy_optimism) + pessimism_confidence_A * expected_to_buy);
	auto buy_transport_perception = std::min(1.f, (economy::numerical::employment_unit::epsilon / (1.f + current_volume) + transport_availability * 2.f));

	auto perception_divisor = (optimism_confidence + pessimism_confidence_B) * (optimism_confidence + pessimism_confidence_A);

	auto earn = price_import * sold_boundary * sell_rate_perception_B * buy_rate_perception_A / perception_divisor * buy_transport_perception;

	result.export_profit = earn - spend;

	auto earn_per_unit = price_import * trade_good_loss_mult;
	auto pay_per_unit = price_export * (1.f + export_tariff + merchant_cut) + price_import * import_tariff + transport_cost_per_weight;


	auto import_budget_scale = state.world.market_get_trade_house_budget_import_scale(B);

	//auto perception_divisor_origin = (optimism_confidence.get(c) + pessimism_confidence_origin);
	//auto perception_divisor_target = (optimism_confidence.get(c) + pessimism_confidence_target);
	//auto perception_divisor = perception_divisor_origin * perception_divisor_target;

	//auto earn_expectation = earn_per_unit * sold_boundary * sell_rate_perception * buy_rate_perception / perception_divisor;

	auto budget = 1.f + state.world.market_get_total_earn(B);
	//auto risk = std::min(1.f, budget * expected_to_sell == 0.f ? 1.f : (economy::price_properties::commodity::min + price_export) / (budget * 0.01f));
	//auto risk = std::min(1.f, budget * 0.01f / (economy::price_properties::commodity::min + price_export));

	auto price_sold = owner == A ? price_import : price_export;

	auto budget_factor = std::min(1.f, std::max(0.f, budget / 10000.f * (0.8f - import_budget_scale) / (economy::price_properties::commodity::min + price_sold)));
	auto budget_scale = 1.f - import_budget_scale;

	auto hard_limit = transport_availability;
	auto soft_limit = expected_to_sell * expected_to_buy;
	auto change_multiplier =
		std::max(0.f, (budget_scale - 0.1f) / 0.9f)
		* std::max(0.f, (hard_limit - 0.9f) / 0.1f)
		* std::max(0.f, (soft_limit - 0.8f) / 0.2f)
		* std::max(0.f, (budget_factor - 0.1f) / 0.9f);
	auto decay = std::max(0.999f, std::min(1.f, 0.5f + budget_scale * soft_limit * hard_limit));


	auto diff = 2.f * (earn_per_unit - pay_per_unit) / (earn_per_unit + economy::price_properties::commodity::min);
	auto base_change = (current_volume * 0.001f + 0.1f) * diff;
	auto change = base_change <= 0.f ? base_change : base_change * change_multiplier;
	auto next = std::max(0.f, current_volume * decay + change);

	result.profit_score = diff;

	result.expected_to_buy_in_origin_ratio = expected_to_buy;

	result.expansion_multiplier = change_multiplier;
	result.import_ratio = budget_scale;
	result.transport_availability = transport_availability;
	result.risk = budget_factor;

	result.profit = earn - spend;

	result.base_change = base_change;
	result.decay = current_volume * (1.f - decay);
	result.final_change = next;

	return result;
}

void update_trade_routes_volume(
	sys::state& state,
	bool ignore_reality,
	ve::vectorizable_buffer<float, dcon::market_id>& export_tariff_buffer,
	ve::vectorizable_buffer<float, dcon::market_id>& import_tariff_buffer,
	ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>& coastal_capital_buffer,
	ve::vectorizable_buffer<float, dcon::state_instance_id>& state_port_is_occupied
) {

	state.world.execute_parallel_over_trade_route([&](auto trade_route) {
		auto origin = state.world.trade_route_get_origin(trade_route);
		auto target = state.world.trade_route_get_target(trade_route);

		auto owner = state.world.trade_route_get_owner(trade_route);
		auto budget = 1.f + state.world.market_get_total_earn(target);

		auto s_origin = state.world.market_get_zone_from_local_market(origin);
		auto s_target = state.world.market_get_zone_from_local_market(target);

		auto capital_origin = state.world.state_instance_get_capital(s_origin);
		auto capital_target = state.world.state_instance_get_capital(s_target);

		auto controller_origin = state.world.province_get_nation_from_province_control(capital_origin);
		auto controller_target = state.world.province_get_nation_from_province_control(capital_target);

		auto formal_owner_origin = state.world.province_get_nation_from_province_ownership(capital_origin);
		auto formal_owner_target = state.world.province_get_nation_from_province_ownership(capital_target);

		controller_origin = ve::select(controller_origin != dcon::nation_id{ }, controller_origin, formal_owner_origin);
		controller_target = ve::select(controller_target != dcon::nation_id{ }, controller_target, formal_owner_target);

		auto origin_tariff_is_applied = state.world.trade_route_get_is_tariff_applied_origin(trade_route);
		auto target_tariff_is_applied = state.world.trade_route_get_is_tariff_applied_target(trade_route);

		ve::mask_vector is_origin_blockaded = state_port_is_occupied.get(s_origin) > 0.f;
		ve::mask_vector is_target_blockaded = state_port_is_occupied.get(s_target) > 0.f;

		auto trade_closed = state.world.trade_route_get_is_trade_forbidden(trade_route);

		auto is_origin_civ = state.world.nation_get_is_civilized(controller_origin);
		auto is_target_civ = state.world.nation_get_is_civilized(controller_target);
		auto is_sea_route = state.world.trade_route_get_is_sea_route(trade_route);
		auto blockaded = is_sea_route && (is_origin_blockaded || is_target_blockaded);
		auto same_nation = controller_origin == controller_target;

		// US3AC7 US3AC8 Ban international sea routes or international land routes based on the corresponding modifiers
		auto origin_bans_sea_trade = state.world.nation_get_modifier_values(controller_origin, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
		auto target_bans_sea_trade = state.world.nation_get_modifier_values(controller_target, sys::national_mod_offsets::disallow_naval_trade) > 0.f;
		auto sea_trade_banned = origin_bans_sea_trade || target_bans_sea_trade;
		auto origin_bans_land_trade = state.world.nation_get_modifier_values(controller_origin, sys::national_mod_offsets::disallow_land_trade) > 0.f;
		auto target_bans_land_trade = state.world.nation_get_modifier_values(controller_target, sys::national_mod_offsets::disallow_land_trade) > 0.f;
		auto land_trade_banned = origin_bans_land_trade || target_bans_land_trade;
		auto trade_banned = (is_sea_route && sea_trade_banned && !same_nation) || (!is_sea_route && land_trade_banned && !same_nation);

		auto merchant_cut = ve::select(same_nation, ve::fp_vector{ economy::merchant_cut_domestic }, ve::fp_vector{ economy::merchant_cut_foreign });

		auto export_tariff = ve::select(same_nation || !origin_tariff_is_applied, ve::fp_vector{ 0.f }, export_tariff_buffer.get(origin));
		auto import_tariff = ve::select(same_nation || !target_tariff_is_applied, ve::fp_vector{ 0.f }, import_tariff_buffer.get(target));

		ve::fp_vector distance = state.world.trade_route_get_distance_km(trade_route);

		// US3AC18
		auto trade_good_loss_mult = ve::max(0.f, 1.f - trade_goods_lost_per_km * distance);

		auto transport_cost = distance * ve::select(is_sea_route, state.world.market_get_naval_transportation_price(owner), state.world.market_get_land_transportation_price(owner));

		auto reset_route =
			trade_closed || blockaded || trade_banned
			|| !ve::apply([&](auto r) { return state.world.trade_route_is_valid(r); }, trade_route);

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

			auto reset_route_commodity = reset_route;

			auto current_volume = state.world.trade_route_get_volume(trade_route, c);
			auto confidence_export = ve::max(0.5f, state.world.market_get_price_confidence(origin, c));
			auto confidence_import = ve::max(0.5f, state.world.market_get_price_confidence(target, c));

			auto price_export = ve_price(state, origin, c) / confidence_export;
			auto price_import = ve_price(state, target, c) * confidence_import;

			auto earn_per_unit = price_import * trade_good_loss_mult;
			auto pay_per_unit = price_export * (1.f + export_tariff + merchant_cut) + price_import * import_tariff + transport_cost;

			// US3AC21 effect of scale
			// volume reduces transport costs

			auto expected_to_buy = state.world.market_get_expected_probability_to_buy(origin, c);
			auto expected_to_sell = state.world.market_get_expected_probability_to_sell(target, c);

			//if(ignore_reality) {
			                //expected_to_buy = ve::min(ve::fp_vector{ 1.f }, expected_to_buy + 0.5f);
			                //expected_to_sell = ve::min(ve::fp_vector{ 1.f }, expected_to_sell + 0.5f);
			//}

			/*

			New model of trade update:
			Assume that there is a segment [0, 1] of traders operating this route.
			Trader 0 gets 0x of the sales.
			Trader 1 gets 2x of the sales.
			Traders inbetween earn a linear combination of trader 0 and trader 1 earning.
			If a certain trader earns less than SPEND, they change their volume (1 - a) times - q dt.
			If a certain trader earns more than SPEND, they change their volume (1 + a) times + q dt.
			We want to find a proportion of traders which would earn more than they spend and update the total traded amount accordingly.

			2 * EARN * x + y = 2 * EARN;
			if y = SPEND
			2 * EARN * x + SPEND = 2 * EARN
			x = (2 * EARN - SPEND) / (2 * EARN) --- the ratio of traders with profitable trade
			2 * (x - 0.5) = (EARN - SPEND) / (EARN)  --- signed amount of traders increasing/reducing their volume

			*/

			// making it into a hard cap?
			auto transport_availability = ve::select(is_sea_route, state.world.market_get_naval_transportation_demand_satisfaction(owner), state.world.market_get_land_transportation_demand_satisfaction(owner));
			auto import_budget_scale = state.world.market_get_trade_house_budget_import_scale(target);

			auto price_sold = ve::select(owner == origin, price_import, price_export);
			//auto risk = ve::min(1.f, budget * 0.01f / (economy::price_properties::commodity::min + price_export));
			auto budget_factor = ve::min(ve::fp_vector{ 1.f }, ve::max(ve::fp_vector{ 0.f }, budget / 10000.f * (0.8f - import_budget_scale) / (economy::price_properties::commodity::min + price_sold)));
			auto budget_scale = 1.f - import_budget_scale;

			auto hard_limit = transport_availability;
			auto soft_limit = expected_to_sell * expected_to_buy;

			auto change_multiplier =
				ve::max(ve::fp_vector{ 0.f }, (budget_scale - 0.1f) / 0.9f)
				* ve::max(ve::fp_vector{ 0.f }, (hard_limit - 0.9f) / 0.1f)
				* ve::max(ve::fp_vector{ 0.f }, (soft_limit - 0.8f) / 0.2f)
				* ve::max(ve::fp_vector{ 0.f }, (budget_factor - 0.1f) / 0.9f);
			auto decay = ve::max(0.999f, ve::min(1.f, 0.5f + hard_limit * soft_limit * budget_scale));

			auto diff = 2.f * (earn_per_unit - pay_per_unit) / (earn_per_unit + economy::price_properties::commodity::min);
			auto change = (current_volume * 0.001f + 0.1f) * diff;

			change = ve::select(change <= 0.f, change, change * change_multiplier);
			//if(ignore_reality) {
				//change = change * 10.f;
			//}
			auto next = ve::select(reset_route_commodity, 0.f, ve::max(0.f, current_volume * decay + change));
			state.world.trade_route_set_volume(trade_route, c, next);
		}
	});
}


void update_trade_routes_consumption(sys::state& state) {
	auto const total_commodities = state.world.commodity_size();

	// Register trade demand on goods
	concurrency::parallel_for(uint32_t(0), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		if(state.world.commodity_get_money_rgo(cid)) {
			return;
		}
		state.world.for_each_trade_route([&](auto trade_route) {
			auto origin = state.world.trade_route_get_origin(trade_route);
			auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
			register_demand(state, origin, cid, current_volume);
		});
	});

	// Transportation demand
	state.world.for_each_trade_route([&](auto trade_route) {
		auto owner = state.world.trade_route_get_owner(trade_route);
		auto distance = state.world.trade_route_get_distance_km(trade_route);
		state.world.for_each_commodity([&](auto cid){
			auto volume = state.world.trade_route_get_volume(trade_route, cid);
			if(state.world.trade_route_get_is_sea_route(trade_route)) {
				auto demand = state.world.market_get_naval_transportation_demand(owner);
				state.world.market_set_naval_transportation_demand(owner, demand + volume * distance);
			} else {
				auto demand = state.world.market_get_land_transportation_demand(owner);
				state.world.market_set_land_transportation_demand(owner, demand + volume * distance);
			}
		});
	});
}

// CAUTION: when we generate trade demand for a good, we promise to pay money to local producers during the next tick
// it means that during economy update, rgo profits were calculated according to this promise
// so local producers ALREADY received money for their production and local market take only a "merchant cut"

template<typename TRADE_ROUTE>
trade_and_tariff<TRADE_ROUTE> explain_trade_route_commodity_internal(
	sys::state const& state,
	TRADE_ROUTE trade_route,
	tariff_data<TRADE_ROUTE>& additional_data,
	dcon::commodity_id cid
) {
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;
	auto current_volume = state.world.trade_route_get_volume(trade_route, cid);

	auto owner = state.world.trade_route_get_owner(trade_route);

	auto origin = state.world.trade_route_get_origin(trade_route);
	auto target = state.world.trade_route_get_target(trade_route);

	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
	auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);

	auto actual_n_origin = state.world.province_get_nation_from_province_control(capital_origin);
	auto actual_n_target = state.world.province_get_nation_from_province_control(capital_target);

	actual_n_origin = adaptive_ve::select(actual_n_origin != dcon::nation_id{ }, actual_n_origin, n_origin);
	actual_n_target = adaptive_ve::select(actual_n_target != dcon::nation_id{ }, actual_n_target, n_target);

	auto price_origin = state.world.market_get_price(origin, cid);
	auto price_target = state.world.market_get_price(target, cid);

	auto sat = state.world.market_get_actual_probability_to_buy(origin, cid);

	auto absolute_volume = sat * adaptive_ve::abs(current_volume);

	auto import_amount = absolute_volume * additional_data.loss;

	auto sea_route = state.world.trade_route_get_is_sea_route(trade_route);
	auto transport_cost = adaptive_ve::select(sea_route, state.world.market_get_naval_transportation_price(owner), state.world.market_get_land_transportation_price(owner));

	const VALUE cut_domestic = economy::merchant_cut_domestic;
	const VALUE cut_foreign = economy::merchant_cut_foreign;

	auto merchant_cut = adaptive_ve::select(actual_n_origin == actual_n_target, cut_domestic, cut_foreign);

	auto export_tariff = additional_data.export_tariff;
	auto import_tariff = additional_data.import_tariff;

	auto origin_tariff = price_origin * export_tariff;
	auto origin_merchant_cut = price_origin * merchant_cut;
	auto target_tariff = price_target * import_tariff;

	/*
	When transporting a unit of commodity:
	Pay for commodity at origin, taxes to governments.
	The payment will be negated by sales from the stockpile, so actually only tariffs are paid
	When arriving at target port, receive arbitrage pay and unload goods to their stockpiles.
	Transport cost is paid separately via mechanics of ships maintenance
	*/

	return {
		.origin = origin,
		.target = target,
		.origin_nation = actual_n_origin,
		.target_nation = actual_n_target,

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

		// Origin earns mostly via normal supply demand matching
		.origin_earn_per_unit = origin_merchant_cut,
		// Target pays the normal price
		.target_spend_per_unit = price_target,
		// We receive payment of target
		.owner_earn_per_unit = price_target,
		// We have to actually pay the export price and taxes
		.owner_spend_per_unit = price_origin + origin_tariff + target_tariff + origin_merchant_cut
	};
}

trade_and_tariff<ve::contiguous_tags<dcon::trade_route_id>> explain_trade_route_commodity(
	sys::state const& state,
	ve::contiguous_tags<dcon::trade_route_id> trade_route,
	tariff_data<ve::contiguous_tags<dcon::trade_route_id>>& additional_data,
	dcon::commodity_id cid
) {
	return explain_trade_route_commodity_internal(state, trade_route, additional_data, cid);
}
trade_and_tariff<ve::partial_contiguous_tags<dcon::trade_route_id>> explain_trade_route_commodity(
	sys::state const& state,
	ve::partial_contiguous_tags<dcon::trade_route_id> trade_route,
	tariff_data<ve::partial_contiguous_tags<dcon::trade_route_id>>& additional_data,
	dcon::commodity_id cid
) {
	return explain_trade_route_commodity_internal(state, trade_route, additional_data, cid);
}

bool is_trade_route_relevant(sys::state& state, dcon::trade_route_id trade_route, dcon::nation_id n) {
	auto origin = state.world.trade_route_get_origin(trade_route);
	auto target = state.world.trade_route_get_target(trade_route);
	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
	auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);
	auto controller_capital_origin = state.world.province_get_nation_from_province_control(capital_origin);
	auto controller_capital_target = state.world.province_get_nation_from_province_control(capital_target);

	controller_capital_origin = controller_capital_origin ? controller_capital_origin : n_origin;
	controller_capital_target = controller_capital_target ? controller_capital_target : n_target;

	if(controller_capital_origin == n) {
		return true;
	}
	if(controller_capital_target == n) {
		return true;
	}
	return false;
}

trade_and_tariff<dcon::trade_route_id> explain_trade_route_commodity(sys::state const& state, dcon::trade_route_id trade_route, dcon::commodity_id cid) {
	auto current_volume = state.world.trade_route_get_volume(trade_route, cid);
	auto origin = state.world.trade_route_get_origin(trade_route);
	auto target = state.world.trade_route_get_target(trade_route);

	auto owner = state.world.trade_route_get_owner(trade_route);

	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto n_origin = state.world.state_instance_get_nation_from_state_ownership(s_origin);
	auto n_target = state.world.state_instance_get_nation_from_state_ownership(s_target);
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);
	auto controller_capital_origin = state.world.province_get_nation_from_province_control(capital_origin);
	auto controller_capital_target = state.world.province_get_nation_from_province_control(capital_target);

	controller_capital_origin = controller_capital_origin ? controller_capital_origin : n_origin;
	controller_capital_target = controller_capital_target ? controller_capital_target : n_target;

	auto origin_apply_tariff = state.world.trade_route_get_is_tariff_applied_origin(trade_route);
	auto target_apply_tariff = state.world.trade_route_get_is_tariff_applied_target(trade_route);

	auto sat = state.world.market_get_actual_probability_to_buy(origin, cid);
	auto absolute_volume = sat * current_volume;
	auto distance = state.world.trade_route_get_distance_km(trade_route);

	auto trade_good_loss_mult = std::max(0.f, 1.f - trade_goods_lost_per_km * distance);
	auto import_amount = absolute_volume * trade_good_loss_mult;

	auto is_sea_route = state.world.trade_route_get_is_sea_route(trade_route);
	auto transport_cost = is_sea_route ? state.world.market_get_naval_transportation_price(owner) : state.world.market_get_land_transportation_price(owner);

	auto export_tariff = origin_apply_tariff ? effective_tariff_export_rate(state, controller_capital_origin, origin) : 0.f;
	auto import_tariff = target_apply_tariff ? effective_tariff_import_rate(state, controller_capital_target, target) : 0.f;

	auto price_origin = price(state, origin, cid);
	auto price_target = price(state, target, cid);

	auto merchant_cut = controller_capital_origin == controller_capital_target ? economy::merchant_cut_domestic : economy::merchant_cut_foreign;

	auto origin_tariff = price_origin * export_tariff;
	auto origin_merchant_cut = price_origin * merchant_cut;
	auto target_tariff = price_target * import_tariff;

	/*
	When transporting a unit of commodity:
	Pay for commodity at origin, taxes to governments.
	The payment will be negated by sales from the stockpile, so actually only tariffs are paid
	When arriving at target port, receive arbitrage pay and unload goods to their stockpiles.
	Transport cost is paid separately via mechanics of ships maintenance
	*/

	return {
		.origin = origin,
		.target = target,
		.origin_nation = controller_capital_origin,
		.target_nation = controller_capital_target,

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

		// Origin earns mostly via normal supply demand matching
		.origin_earn_per_unit = origin_merchant_cut,
		// Target pays the normal price
		.target_spend_per_unit = price_target,
		// We receive payment of target
		.owner_earn_per_unit = price_target,
		// We have to actually pay the export price and taxes
		.owner_spend_per_unit = price_origin + origin_tariff + target_tariff + origin_merchant_cut
	};
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

		state.world.for_each_trade_route([&](auto route) {
			if(!is_trade_route_relevant(state, route, n)) return;
			trade_and_tariff route_data = explain_trade_route_commodity(state, route, cid);

			if(import_flag && route_data.target_nation == n) {
				buffer_volume_per_nation.get(route_data.origin_nation) += route_data.amount_target;
				buffer_tariff_per_nation.get(route_data.origin_nation) += route_data.tariff_target;
			}

			if(export_flag && route_data.origin_nation == n) {
				buffer_volume_per_nation.get(route_data.target_nation) += route_data.amount_origin;
				buffer_tariff_per_nation.get(route_data.target_nation) += route_data.tariff_origin;
			}
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



template <typename TRADE_ROUTE>
auto explain_trade_route(
	sys::state& state,
	TRADE_ROUTE trade_route,
	// Paid for each unit x price
	ve::vectorizable_buffer<float, dcon::market_id>& export_tariff,
	// Paid for each unit x price
	ve::vectorizable_buffer<float, dcon::market_id>& import_tariff
) {
	using VALUE = typename std::conditional_t<ve::is_vector_type_s<TRADE_ROUTE>::value, ve::fp_vector, float>;

	auto origin = state.world.trade_route_get_origin(trade_route);
	auto target = state.world.trade_route_get_target(trade_route);
	auto owner = state.world.trade_route_get_owner(trade_route);

	auto s_origin = state.world.market_get_zone_from_local_market(origin);
	auto s_target = state.world.market_get_zone_from_local_market(target);
	auto capital_origin = state.world.state_instance_get_capital(s_origin);
	auto capital_target = state.world.state_instance_get_capital(s_target);	
	auto tariff_applied_origin = state.world.trade_route_get_is_tariff_applied_origin(trade_route);
	auto tariff_applied_target = state.world.trade_route_get_is_tariff_applied_target(trade_route);
	auto distance = state.world.trade_route_get_distance_km(trade_route);
	auto trade_good_loss_mult = adaptive_ve::max<VALUE>(0.f, 1.f - economy::trade_goods_lost_per_km * distance);

	auto is_sea_route = state.world.trade_route_get_is_sea_route(trade_route);

	auto transport_cost_per_distance_weight = adaptive_ve::select(is_sea_route, state.world.market_get_naval_transportation_price(owner), state.world.market_get_land_transportation_price(owner));

	auto export_tariff_origin = ve::select(tariff_applied_origin, export_tariff.get(origin), 0.f);
	auto import_tariff_target = ve::select(tariff_applied_target, import_tariff.get(target), 0.f);

	tariff_data<TRADE_ROUTE> result{
		.applies_export_tariff = tariff_applied_origin,
		.applies_import_tariff = tariff_applied_target,
		.export_tariff = export_tariff_origin,
		.import_tariff = import_tariff_target,

		.distance = distance,
		.loss = trade_good_loss_mult,
		.transportation_cost = transport_cost_per_distance_weight * distance
	};

	return result;
}

void fill_trade_buffers(
	sys::state& state,
	const ve::vectorizable_buffer<float, dcon::market_id>& export_tariff_buffer,
	const ve::vectorizable_buffer<float, dcon::market_id>& import_tariff_buffer,
	bool presimulation
) {
	uint32_t total_commodities = state.world.commodity_size();
	concurrency::parallel_for(uint32_t(1), total_commodities, [&](uint32_t k) {
		dcon::commodity_id cid{ dcon::commodity_id::value_base_t(k) };
		state.world.for_each_trade_route([&](auto route) {
			auto origin = state.world.trade_route_get_origin(route);
			auto target = state.world.trade_route_get_target(route);
			auto distance = state.world.trade_route_get_distance_km(route);
			auto volume = state.world.trade_route_get_volume(route, cid);
			auto lost_modifier = std::max(0.f, 1.f - distance * trade_goods_lost_per_km);
			auto sat = state.world.market_get_actual_probability_to_buy(origin, cid);
			state.world.market_set_export(origin, cid, state.world.market_get_export(origin, cid) + sat * volume);
			state.world.market_set_import(target, cid, state.world.market_get_export(target, cid) + sat * volume * lost_modifier);
			state.world.market_set_stockpile(target, cid, state.world.market_get_stockpile(target, cid) + sat * volume * lost_modifier);
			if(presimulation && state.world.commodity_get_actually_exists_in_nature(cid)) {
				state.world.market_set_stockpile(target, cid, state.world.market_get_stockpile(target, cid) + volume);
			}
		});
	});
	uint32_t total_markets = state.world.market_size();
	concurrency::parallel_for(uint32_t(1), total_markets, [&](uint32_t k) {
		{
			dcon::market_id origin{ dcon::market_id::value_base_t(k) };
			auto total_earn_export = 0.f;
			auto total_origin_tariff = 0.f;
			auto total_arbitrage = 0.f;

			state.world.market_for_each_trade_route_as_origin(origin, [&](auto route) {
				auto origin = state.world.trade_route_get_origin(route);
				auto target = state.world.trade_route_get_target(route);
				auto owner = state.world.trade_route_get_owner(route);
				auto cut = state.world.trade_route_get_origin_cut_rate(route);
				auto export_rate = state.world.trade_route_get_is_tariff_applied_origin(route)
					? export_tariff_buffer.get(origin) : 0.f;
				auto import_rate = state.world.trade_route_get_is_tariff_applied_origin(route)
					? import_tariff_buffer.get(origin) : 0.f;


				state.world.for_each_commodity([&](auto cid) {
					auto sat = state.world.market_get_actual_probability_to_buy(origin, cid);
					auto volume = state.world.trade_route_get_volume(route, cid) * sat;
					auto price_origin = state.world.market_get_price(origin, cid);
					auto price_target = state.world.market_get_price(target, cid);
					total_earn_export = total_earn_export + volume * (price_origin * cut);
					total_origin_tariff = total_origin_tariff + volume * (price_origin * export_rate);

					if(owner == origin) {
						total_arbitrage = total_arbitrage + volume * (price_target * (1.f - import_rate) - price_origin * (1.f + export_rate + cut));
					}
				});
			});

			state.world.market_set_arbitrage(origin, state.world.market_get_arbitrage(origin) + total_arbitrage);
			state.world.market_set_export_cut(origin, state.world.market_get_export_cut(origin) + total_earn_export);
			state.world.market_set_tariff_collected(origin, state.world.market_get_tariff_collected(origin) + total_origin_tariff);
		}
		{
			dcon::market_id target{ dcon::market_id::value_base_t(k) };
			auto total_spend_import = 0.f;
			auto total_target_tariff = 0.f;
			auto total_arbitrage = 0.f;

			state.world.market_for_each_trade_route_as_target(target, [&](auto route) {
				auto origin = state.world.trade_route_get_origin(route);
				auto target = state.world.trade_route_get_target(route);
				auto owner = state.world.trade_route_get_owner(route);
				auto cut = state.world.trade_route_get_origin_cut_rate(route);
				auto export_rate = state.world.trade_route_get_is_tariff_applied_origin(route)
					? export_tariff_buffer.get(origin) : 0.f;
				auto import_rate = state.world.trade_route_get_is_tariff_applied_origin(route)
					? import_tariff_buffer.get(origin) : 0.f;

				state.world.for_each_commodity([&](auto cid) {
					auto sat = state.world.market_get_actual_probability_to_buy(origin, cid);
					auto volume = state.world.trade_route_get_volume(route, cid) * sat;
					auto price_origin = state.world.market_get_price(origin, cid);
					auto price_target = state.world.market_get_price(target, cid);

					total_spend_import = total_spend_import + volume * (price_target);
					total_target_tariff = total_target_tariff + volume * (price_target * import_rate);
					if(owner == target) {
						total_arbitrage = total_arbitrage + volume * (price_target * (1.f - import_rate) - price_origin * (1.f + export_rate + cut));
					}
				});
			});

			state.world.market_set_arbitrage(target, state.world.market_get_arbitrage(target) + total_arbitrage);
			state.world.market_set_import_spending(target, state.world.market_get_import_spending(target) + total_spend_import);
			state.world.market_set_tariff_collected(target, state.world.market_get_tariff_collected(target) + total_target_tariff);
		}
	});
}

}
