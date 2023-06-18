#include "gui_province_window.hpp"

namespace ui {

void province_owner_rgo_draw_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov_id) noexcept {
	auto rgo_good = state.world.province_get_rgo(prov_id);
	auto nat_id = state.world.province_get_nation_from_province_ownership(prov_id);

	if(rgo_good) {
		auto box = text::open_layout_box(contents, 0);

		text::substitution_map sub_map;
		text::add_to_substitution_map(sub_map, text::variable_type::goods, rgo_good.get_name());
		text::add_to_substitution_map(sub_map, text::variable_type::value,
				text::fp_three_places{province::rgo_income(state, prov_id)});
		text::localised_format_box(state, contents, box, std::string_view("provinceview_goodsincome"), sub_map);
		text::add_line_break_to_layout_box(state, contents, box);

		auto const production = province::rgo_production_quantity(state, prov_id);
		sub_map.clear();
		text::add_to_substitution_map(sub_map, text::variable_type::curr, text::fp_two_places{production});
		text::localised_format_box(state, contents, box, std::string_view("production_output_goods_tooltip2"), sub_map);
		text::localised_format_box(state, contents, box, std::string_view("production_output_explanation"));

		text::add_divider_to_layout_box(state, contents, box);

		auto const base_size = economy::rgo_effective_size(state, nat_id, prov_id) * state.world.commodity_get_rgo_amount(rgo_good);
		sub_map.clear();
		text::add_to_substitution_map(sub_map, text::variable_type::base, text::fp_two_places{base_size});
		text::localised_format_box(state, contents, box, std::string_view("production_base_output_goods_tooltip"), sub_map);

		bool const is_mine = state.world.commodity_get_is_mine(rgo_good);
		auto const efficiency = 1.0f +
														state.world.province_get_modifier_values(prov_id,
																is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff) +
														state.world.nation_get_modifier_values(nat_id,
																is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff);
		text::localised_format_box(state, contents, box, std::string_view("production_output_efficiency_tooltip"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage{efficiency},
				efficiency >= 0.0f ? text::text_color::green : text::text_color::red);

		text::add_line_break_to_layout_box(state, contents, box);
		text::add_line_break_to_layout_box(state, contents, box);

		auto const throughput = state.world.province_get_rgo_employment(prov_id);
		text::localised_format_box(state, contents, box, std::string_view("production_throughput_efficiency_tooltip"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage{throughput},
				throughput >= 0.0f ? text::text_color::green : text::text_color::red);

		text::close_layout_box(contents, box);
	}
}

}
