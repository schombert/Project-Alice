#include "gui_province_window.hpp"
#include "gui_common_elements.hpp"

namespace ui {

void province_owner_rgo_draw_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov_id) noexcept {
	auto rgo_good = state.world.province_get_rgo(prov_id);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto production = economy::rgo_output(state, c, prov_id);
		auto profit = state.world.province_get_rgo_profit(prov_id);

		if(production < 0.0001f) {
			return;
		}

		auto base_box = text::open_layout_box(contents);
		auto name_box = base_box;
		name_box.x_size = 75;
		auto production_box = base_box;
		production_box.x_position += 90.f;

		auto profit_box = base_box;
		profit_box.x_position += 180.f;

		text::add_to_layout_box(state, contents, name_box, text::get_name_as_string(state, dcon::fatten(state.world, c)));
		text::add_to_layout_box(state, contents, production_box, text::fp_two_places{ production });
		text::add_to_layout_box(state, contents, profit_box, text::format_money(profit));
		text::add_to_layout_box(state, contents, base_box, std::string(" "));
		text::close_layout_box(contents, base_box);
	});

	if(rgo_good) {
		province_owner_rgo_commodity_tooltip(state, contents, prov_id, rgo_good);
	}
}

void province_national_focus_button::button_action(sys::state& state) noexcept {
	auto province_window = static_cast<province_view_window*>(state.ui_state.province_window);
	province_window->nf_win->set_visible(state, !province_window->nf_win->is_visible());
}

float trade_route_profit(sys::state& state, dcon::trade_route_id route, dcon::commodity_id c) {
	auto current_volume = state.world.trade_route_get_volume(route, c);
	auto A = state.world.trade_route_get_connected_markets(route, 0);
	auto B = state.world.trade_route_get_connected_markets(route, 1);

	auto absolute_volume = std::abs(current_volume);
	//auto sat = state.world.market_get_direct_demand_satisfaction(origin, c);

	auto s_A = state.world.market_get_zone_from_local_market(A);
	auto s_B = state.world.market_get_zone_from_local_market(B);

	if(A == B) {
		return 0.f;
	}

	auto n_A = state.world.state_instance_get_nation_from_state_ownership(s_A);
	auto n_B = state.world.state_instance_get_nation_from_state_ownership(s_B);

	auto at_war = military::are_at_war(state, n_A, n_B);

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

	auto A_is_open_to_B = sphere_A == controller_capital_B || overlord_A == controller_capital_B;
	auto B_is_open_to_A = sphere_B == controller_capital_A || overlord_B == controller_capital_A;

	auto import_tariff_A = economy::effective_tariff_import_rate(state, controller_capital_A, A);
	auto export_tariff_A = economy::effective_tariff_export_rate(state, controller_capital_A, A);
	auto import_tariff_B = economy::effective_tariff_import_rate(state, controller_capital_B, B);
	auto export_tariff_B = economy::effective_tariff_export_rate(state, controller_capital_B, B);

	auto is_sea_route = state.world.trade_route_get_is_sea_route(route);
	auto is_land_route = state.world.trade_route_get_is_land_route(route);

	if(is_sea_route) {
		if(state.world.province_get_is_blockaded(port_A)) {
			is_sea_route = false;
		}
		if(state.world.province_get_is_blockaded(port_B)) {
			is_sea_route = false;
		}
	}

	if(!(is_sea_route || is_land_route)) {
		return 0.f;
	}

	auto merchant_cut = economy::merchant_cut_foreign;

	if(controller_capital_A == controller_capital_B) {
		import_tariff_A = 0.f;
		export_tariff_A = 0.f;
		import_tariff_B = 0.f;
		export_tariff_B = 0.f;
		merchant_cut = economy::merchant_cut_domestic;
	}
	if(A_is_open_to_B) {
		import_tariff_A = 0.f;
		export_tariff_A = 0.f;
	}
	if(B_is_open_to_A) {
		import_tariff_B = 0.f;
		export_tariff_B = 0.f;
	}

	auto distance = 999999.f;

	if(is_land_route) {
		distance = std::min(distance, state.world.trade_route_get_land_distance(route));
	}
	if(is_sea_route) {
		distance = std::min(distance, state.world.trade_route_get_sea_distance(route));
	}

	auto trade_good_loss_mult = std::max(0.f, 1.f - 0.0001f * distance);

	auto transport_cost = distance / economy::trade_distance_covered_by_pair_of_workers_per_unit_of_good
	* (
		state.world.province_get_labor_price(capital_A, economy::labor::no_education)
		+ state.world.province_get_labor_price(capital_B, economy::labor::no_education)
	);
	// effect of scale
	// volume reduces transport costs
	auto effect_of_scale = std::max(0.1f, 1.f - absolute_volume * economy::effect_of_transportation_scale);


	auto price_A_export = economy::price(state, A, c) * (1.f + export_tariff_A);
	auto price_B_export = economy::price(state, B, c) * (1.f + export_tariff_B);

	auto price_A_import = economy::price(state, A, c) * (1.f - import_tariff_A) * trade_good_loss_mult;
	auto price_B_import = economy::price(state, B, c) * (1.f - import_tariff_B) * trade_good_loss_mult;

	auto current_profit_A_to_B = price_B_import - price_A_export * merchant_cut - transport_cost * effect_of_scale;
	auto current_profit_B_to_A = price_A_import - price_B_export * merchant_cut - transport_cost * effect_of_scale;

	return std::max(0.f, std::max(current_profit_A_to_B / price_A_export, current_profit_B_to_A / price_B_export));
}

}
