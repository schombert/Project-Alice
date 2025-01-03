#include "gui_province_window.hpp"

namespace ui {

void province_owner_rgo_draw_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov_id) noexcept {
	auto rgo_good = state.world.province_get_rgo(prov_id);
	auto nat_id = state.world.province_get_nation_from_province_ownership(prov_id);

	state.world.for_each_commodity([&](dcon::commodity_id c) {
		auto production = province::rgo_production_quantity(state, prov_id, c);
		auto profit = state.world.province_get_rgo_profit_per_good(prov_id, c);

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

		text::add_line(state, contents, "provinceview_goodsincome", text::variable_type::goods, rgo_good.get_name(), text::variable_type::value,
					text::fp_three_places{ province::rgo_income(state, prov_id) });

		{
			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub_map;
			auto const production = province::rgo_production_quantity(state, prov_id, rgo_good);

			text::add_to_substitution_map(sub_map, text::variable_type::curr, text::fp_two_places{ production });
			text::localised_format_box(state, contents, box, std::string_view("production_output_goods_tooltip2"), sub_map);
			text::localised_format_box(state, contents, box, std::string_view("production_output_explanation"));
			text::close_layout_box(contents, box);
		}

		text::add_line_break_to_layout(state, contents);

		{
			auto const production = province::rgo_production_quantity(state, prov_id, rgo_good);
			auto const base_size = economy::rgo_effective_size(state, nat_id, prov_id, rgo_good) * state.world.commodity_get_rgo_amount(rgo_good);
			text::add_line(state, contents, std::string_view("production_base_output_goods_tooltip"), text::variable_type::base, text::fp_two_places{ base_size });
		}

		{
			auto box = text::open_layout_box(contents, 0);
			bool const is_mine = state.world.commodity_get_is_mine(rgo_good);
			auto const efficiency = 1.0f +
				state.world.province_get_modifier_values(prov_id,
						is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff) +
				state.world.nation_get_modifier_values(nat_id,
						is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff);
			text::localised_format_box(state, contents, box, std::string_view("production_output_efficiency_tooltip"));
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ efficiency }, efficiency >= 0.0f ? text::text_color::green : text::text_color::red);
			text::close_layout_box(contents, box);
		}

		text::add_line_break_to_layout(state, contents);

		{
			auto box = text::open_layout_box(contents, 0);
			auto const throughput = state.world.province_get_rgo_employment(prov_id);
			text::localised_format_box(state, contents, box, std::string_view("production_throughput_efficiency_tooltip"));
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ throughput }, throughput >= 0.0f ? text::text_color::green : text::text_color::red);

			text::close_layout_box(contents, box);
		}
	}
}

void province_national_focus_button::button_action(sys::state& state) noexcept {
	auto province_window = static_cast<province_view_window*>(state.ui_state.province_window);
	province_window->nf_win->set_visible(state, !province_window->nf_win->is_visible());
}

float trade_route_profit(sys::state& state, dcon::market_id from, dcon::trade_route_id route, dcon::commodity_id c) {
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

	auto sphere_A = state.world.nation_get_in_sphere_of(n_A);
	auto sphere_B = state.world.nation_get_in_sphere_of(n_B);

	auto import_tariff_A = economy::effective_tariff_rate(state, n_A);
	auto export_tariff_A = economy::effective_tariff_rate(state, n_A);
	auto import_tariff_B = economy::effective_tariff_rate(state, n_B);
	auto export_tariff_B = economy::effective_tariff_rate(state, n_B);

	auto is_A_civ = state.world.nation_get_is_civilized(n_A);
	auto is_B_civ = state.world.nation_get_is_civilized(n_B);

	auto is_sea_route = state.world.trade_route_get_is_sea_route(route);
	auto is_land_route = state.world.trade_route_get_is_land_route(route);

	if(is_sea_route) {
		auto port_A = province::state_get_coastal_capital(state, s_A);
		if(state.world.province_get_is_blockaded(port_A)) {
			is_sea_route = false;
		}
		auto port_B = province::state_get_coastal_capital(state, s_B);
		if(state.world.province_get_is_blockaded(port_B)) {
			is_sea_route = false;
		}
	}

	if(!(is_sea_route || is_land_route)) {
		return 0.f;
	}

	auto merchant_cut = 1.05f;

	if(n_A == n_B) {
		import_tariff_A = 0.f;
		export_tariff_A = 0.f;
		import_tariff_B = 0.f;
		export_tariff_B = 0.f;
		merchant_cut = 1.001f;
	}
	if(n_A == sphere_B) {
		import_tariff_A = 0.f;
		export_tariff_A = 0.f;
		import_tariff_B = 0.f;
		export_tariff_B = 0.f;
	}
	if(n_B == sphere_A) {
		import_tariff_A = 0.f;
		export_tariff_A = 0.f;
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

	auto transport_cost = distance * 0.005f;
	// effect of scale
	// volume reduces transport costs
	auto effect_of_scale = std::max(0.1f, 1.f - absolute_volume * 0.0005f);


	auto price_A_export = economy::price(state, A, c) * (1.f + export_tariff_A);
	auto price_B_export = economy::price(state, B, c) * (1.f + export_tariff_B);

	auto price_A_import = economy::price(state, A, c) * (1.f - import_tariff_A) * trade_good_loss_mult;
	auto price_B_import = economy::price(state, B, c) * (1.f - import_tariff_B) * trade_good_loss_mult;

	auto current_profit_A_to_B = price_B_import - price_A_export * merchant_cut - transport_cost * effect_of_scale;
	auto current_profit_B_to_A = price_A_import - price_B_export * merchant_cut - transport_cost * effect_of_scale;

	return std::max(0.f, std::max(current_profit_A_to_B / price_A_export, current_profit_B_to_A / price_B_export));
}

}
