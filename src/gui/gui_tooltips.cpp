#include "gui_tooltips.hpp"

#include "system_state.hpp"
#include "text.hpp"
#include "labour_details.hpp"
#include "economy_production.hpp"
#include "construction.hpp"

namespace ui {

namespace commodity_tooltip_settings {
inline constexpr int position_cost = 20;
inline constexpr int position_name = 70;
inline constexpr int position_amount = 180;
inline constexpr int position_ratio = 250;
inline constexpr float green_threshold = 0.6f;
}

void commodity_tooltip(sys::state& state, text::columnar_layout& contents, dcon::commodity_id cid, float amount, float price, float ratio, float direction) {
	auto box = text::open_layout_box(contents);
	text::layout_box name_entry = box;
	text::layout_box ratio_box = box;
	text::layout_box amount_box = box;
	text::layout_box cost_box = box;

	name_entry.x_position += commodity_tooltip_settings::position_name;
	ratio_box.x_position += commodity_tooltip_settings::position_ratio;
	amount_box.x_position += commodity_tooltip_settings::position_amount;
	cost_box.x_position += commodity_tooltip_settings::position_cost;

	name_entry.x_size /= 10;

	std::string padding = cid.index() < 10 ? "0" : "";
	std::string description = "@$" + padding + std::to_string(cid.index());
	text::add_unparsed_text_to_layout_box(state, contents, name_entry, description);

	text::add_to_layout_box(state, contents, name_entry, state.world.commodity_get_name(cid));

	text::add_to_layout_box(state, contents,
		ratio_box,
		text::fp_percentage{ ratio },
		ratio >= commodity_tooltip_settings::green_threshold
		? text::text_color::green
		: text::text_color::red
	);

	float money_flow = price * amount * direction;

	text::add_to_layout_box(state, contents, amount_box, text::fp_two_places{ amount });

	if(money_flow > 0) {
		text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ money_flow }, text::text_color::green);
	} else {
		text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ money_flow }, text::text_color::red);
	}

	text::add_to_layout_box(state, contents, box, std::string(" "));
	text::close_layout_box(contents, box);
}

void labor_tooltip(sys::state& state, text::columnar_layout& contents, int32_t labor_id, float amount, float price, float ratio, float direction) {
	auto box = text::open_layout_box(contents);
	text::layout_box name_entry = box;
	text::layout_box ratio_box = box;
	text::layout_box amount_box = box;
	text::layout_box cost_box = box;

	name_entry.x_position += commodity_tooltip_settings::position_name;
	ratio_box.x_position += commodity_tooltip_settings::position_ratio;
	amount_box.x_position += commodity_tooltip_settings::position_amount;
	cost_box.x_position += commodity_tooltip_settings::position_cost;

	name_entry.x_size /= 10;

	text::add_to_layout_box(state, contents, name_entry, text::produce_simple_string(state, labour_type_to_employment_name_text_key(labor_id)));

	text::add_to_layout_box(state, contents,
		ratio_box,
		text::fp_percentage{ ratio },
		ratio >= commodity_tooltip_settings::green_threshold
		? text::text_color::green
		: text::text_color::red
	);

	float money_flow = price * amount * direction;

	text::add_to_layout_box(state, contents, amount_box, text::fp_two_places{ amount });

	if(money_flow > 0) {
		text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ money_flow }, text::text_color::red);
	} else {
		text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ money_flow }, text::text_color::red);
	}

	text::add_to_layout_box(state, contents, box, std::string(" "));
	text::close_layout_box(contents, box);
}

void commodity_set_tooltip(sys::state& state, text::columnar_layout& contents, economy::detailed_commodity_set& set, bool is_inputs) {
	for(uint32_t i = 0; i < set.set_size; i++) {
		if(!set.commodity_type[i]) {
			break;
		}

		commodity_tooltip(
			state, contents, set.commodity_type[i],
			set.commodity_actual_amount[i],
			set.commodity_price[i],
			set.efficient_ratio[i],
			is_inputs ? -1.f : 1.f
		);
	}
}

void factory_stats_tooltip(sys::state& state, text::columnar_layout& contents, dcon::factory_id fid) {

	auto details = economy::factory_operation::explain_everything(state, fid);

	int indent = 10;


	text::add_line(state, contents, state.world.factory_type_get_name(details.base_type));

	// description of money flows
	{
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_money_flow_sum", text::variable_type::value, text::format_money(details.profit));

		text::add_line(state, contents, "factory_money_flow_sales", text::variable_type::value, text::format_money(details.income_from_sales), indent);
		commodity_tooltip(state, contents, details.output, details.output_actual_amount, details.output_price, details.output_actually_sold_ratio, 1.f);

		text::add_line(state, contents, "factory_money_flow_primary_inputs", text::variable_type::value, text::format_money(-details.spending_from_primary_inputs), indent);
		commodity_set_tooltip(state, contents, details.primary_inputs, true);

		text::add_line(state, contents, "factory_money_flow_secondary_inputs", text::variable_type::value, text::format_money(-details.spending_from_efficiency_inputs), indent);
		commodity_set_tooltip(state, contents, details.efficiency_inputs, true);

		text::add_line(state, contents, "factory_money_flow_expansion", text::variable_type::value, text::format_money(-details.spending_from_expansion), indent);
		commodity_set_tooltip(state, contents, details.expansion_inputs, true);

		text::add_line(state, contents, "factory_money_flow_maintenance", text::variable_type::value, text::format_money(-details.spending_from_maintenance), indent);
		commodity_set_tooltip(state, contents, details.maintenance_inputs, true);

		text::add_line(state, contents, "factory_money_flow_wages", text::variable_type::value, text::format_money(-details.spending_from_wages), indent);
		labor_tooltip(state, contents,
			economy::labor::no_education,
			details.employment.unqualified,
			details.employment_wages_per_person.unqualified,
			details.employment_available_ratio.unqualified,
			-1.f
		);
		labor_tooltip(state, contents,
			economy::labor::basic_education,
			details.employment.primary,
			details.employment_wages_per_person.primary,
			details.employment_available_ratio.primary,
			-1.f
		);
		labor_tooltip(state, contents,
			economy::labor::high_education,
			details.employment.secondary,
			details.employment_wages_per_person.secondary,
			details.employment_available_ratio.secondary,
			-1.f
		);
	}

	// description of target labor and estimated changes
	{
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_hired_unqualified",
			text::variable_type::value, text::format_float(details.employment.unqualified),
			text::variable_type::x, text::format_percentage(economy::unqualified_throughput_multiplier)
		);

		if(details.employment_expected_change.unqualified >= 0) {
			text::add_line(state, contents, "factory_hiring_rate_unqualified",
				text::variable_type::value, text::format_float(details.employment_expected_change.unqualified),
				text::variable_type::x, text::format_float(details.employment_target.unqualified)
			);
		} else {
			text::add_line(state, contents, "factory_firing_rate_unqualified",
				text::variable_type::value, text::format_float(-details.employment_expected_change.unqualified),
				text::variable_type::x, text::format_float(details.employment_target.unqualified)
			);
		}

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_hired_primary",
			text::variable_type::value, text::format_float(details.employment.primary)
		);

		if(details.employment_expected_change.primary >= 0) {
			text::add_line(state, contents, "factory_hiring_rate_primary",
				text::variable_type::value, text::format_float(details.employment_expected_change.primary),
				text::variable_type::x, text::format_float(details.employment_target.primary)
			);
		} else {
			text::add_line(state, contents, "factory_firing_rate_primary",
				text::variable_type::value, text::format_float(-details.employment_expected_change.primary),
				text::variable_type::x, text::format_float(details.employment_target.primary)
			);
		}

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_hired_secondary",
			text::variable_type::value, text::format_float(details.employment.secondary),
			text::variable_type::x, text::format_percentage(details.output_multipliers.from_secondary_workers)
		);

		if(details.employment_expected_change.secondary >= 0) {
			text::add_line(state, contents, "factory_hiring_rate_secondary",
				text::variable_type::value, text::format_float(details.employment_expected_change.secondary),
				text::variable_type::x, text::format_float(details.employment_target.secondary)
			);
		} else {
			text::add_line(state, contents, "factory_firing_rate_secondary",
				text::variable_type::value, text::format_float(-details.employment_expected_change.secondary),
				text::variable_type::x, text::format_float(details.employment_target.secondary)
			);
		}
	}

	// description of expansion
	{
		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "factory_expansion_rate", text::variable_type::value, text::format_float(details.expansion_size));
	}

	// description of multipliers
	{
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_throughput_explanation",
			text::variable_type::val, text::format_percentage(details.throughput_multipliers.total),
			text::variable_type::x, text::format_float(details.production_units),
			text::variable_type::y, text::format_float(details.employment_units)
		);

		text::add_line(state, contents, "factory_throughput_base",
			text::variable_type::val, text::format_percentage(details.throughput_multipliers.base),
			indent
		);
		text::add_line(state, contents, "factory_stats_7",
			text::variable_type::val, text::format_percentage(details.throughput_multipliers.from_scale),
			indent
		);
		text::add_line(state, contents, "factory_throughput_modifiers",
			text::variable_type::val, text::format_percentage(details.throughput_multipliers.from_modifiers),
			indent
		);

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_input_multiplier_explanation",
			text::variable_type::val, text::format_percentage(details.input_multipliers.total)
		);

		text::add_line(state, contents, "factory_input_multiplier_competition",
			text::variable_type::val, text::format_percentage(details.input_multipliers.from_competition),
			indent
		);

		text::add_line(state, contents, "factory_input_multiplier_scale",
			text::variable_type::val, text::format_percentage(details.input_multipliers.from_scale),
			indent
		);

		text::add_line(state, contents, "factory_input_multiplier_specialisation",
			text::variable_type::val, text::format_percentage(details.input_multipliers.from_specialisation),
			indent
		);

		text::add_line(state, contents, "factory_input_multiplier_modifiers",
			text::variable_type::val, text::format_percentage(details.input_multipliers.from_modifiers * details.input_multipliers.from_triggered_modifiers),
			indent
		);

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_output_multiplier_explanation",
			text::variable_type::val, text::format_percentage(details.output_multipliers.total)
		);

		text::add_line(state, contents, "factory_output_multiplier_lack_of_inputs",
			text::variable_type::val, text::format_percentage(details.output_multipliers.from_inputs_lack)
		);
		text::add_line(state, contents, "factory_output_multiplier_efficiency_inputs",
			text::variable_type::val, text::format_percentage(details.output_multipliers.from_efficiency_goods)
		);
		text::add_line(state, contents, "factory_output_multiplier_secondary_workers",
			text::variable_type::val, text::format_percentage(details.output_multipliers.from_secondary_workers)
		);
		text::add_line(state, contents, "factory_output_multiplier_modifiers",
			text::variable_type::val, text::format_percentage(details.output_multipliers.from_modifiers)
		);

		text::add_line_break_to_layout(state, contents);
	}
};

void province_building_effect_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id p, economy::province_building_type bt, float level = 1.f) {
	auto def = state.economy_definitions.building_definitions[uint8_t(bt)];
	modifier_description(state, contents, state.economy_definitions.building_definitions[uint8_t(bt)].province_modifier, 0, level);
	// Since trade accounts for naval bases level separately, show special case for trade attractiveness for them
	if(bt == economy::province_building_type::naval_base) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "alice_trade_attractiveness"), text::text_color::white);
		text::add_to_layout_box(state, contents, box, std::string_view{ ":" }, text::text_color::white);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ nations::naval_base_level_to_market_attractiveness * level }, text::text_color::green);
		text::close_layout_box(contents, box);
	} else if(bt == economy::province_building_type::railroad) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "infrastructure"), text::text_color::white);
		text::add_to_layout_box(state, contents, box, std::string_view{ ":" }, text::text_color::white);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ def.infrastructure }, text::text_color::green);
		text::close_layout_box(contents, box);
	}
}

void province_building_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id p, economy::province_building_type bt) {
	auto level = state.world.province_get_building_level(p, uint8_t(bt));

	text::add_line(state, contents, state.lookup_key(economy::province_building_type_get_name(bt)));
	text::add_line(state, contents, "alice_building_level", text::variable_type::val, level);
	text::add_line_break_to_layout(state, contents);

	if(level > 0) {
		text::add_line(state, contents, "alice_building_modifier");
		province_building_effect_tooltip(state, contents, p, bt, level);
	}
};

void province_building_construction_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id p, economy::province_building_type bt) {
	text::add_line(state, contents, state.lookup_key(economy::province_building_type_get_name(bt)));
	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "alice_new_building_modifier");
	province_building_effect_tooltip(state, contents, p, bt);

	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "alice_building_conditions");
	int32_t current_lvl = state.world.province_get_building_level(p, uint8_t(bt));
	int32_t max_local_lvl = state.world.nation_get_max_building_level(state.local_player_nation, uint8_t(bt));
	if(bt == economy::province_building_type::fort) {
		text::add_line_with_condition(state, contents, "fort_build_tt_1", state.world.province_get_nation_from_province_control(p) == state.local_player_nation);
		text::add_line_with_condition(state, contents, "fort_build_tt_2", !military::province_is_under_siege(state, p));

		int32_t min_build = int32_t(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_fort));
		text::add_line_with_condition(state, contents, "fort_build_tt_3", (max_local_lvl - current_lvl - min_build > 0), text::variable_type::x, int64_t(current_lvl), text::variable_type::n, int64_t(min_build), text::variable_type::y, int64_t(max_local_lvl));

	} else if(bt == economy::province_building_type::naval_base) {
		text::add_line_with_condition(state, contents, "fort_build_tt_1", state.world.province_get_nation_from_province_control(p) == state.local_player_nation);
		text::add_line_with_condition(state, contents, "fort_build_tt_2", !military::province_is_under_siege(state, p));
		text::add_line_with_condition(state, contents, "nb_build_tt_1", state.world.province_get_is_coast(p));

		int32_t min_build = int32_t(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_naval_base));

		auto si = state.world.province_get_state_membership(p);
		text::add_line_with_condition(state, contents, "nb_build_tt_2", current_lvl > 0 || !si.get_naval_base_is_taken());

		text::add_line_with_condition(state, contents, "fort_build_tt_3", (max_local_lvl - current_lvl - min_build > 0), text::variable_type::x, int64_t(current_lvl), text::variable_type::n, int64_t(min_build), text::variable_type::y, int64_t(max_local_lvl));

	} else {
		text::add_line_with_condition(state, contents, "fort_build_tt_1", state.world.province_get_nation_from_province_control(p) == state.local_player_nation);
		text::add_line_with_condition(state, contents, "fort_build_tt_2", !military::province_is_under_siege(state, p));

		auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
		text::add_line_with_condition(state, contents, "rr_build_tt_1", (rules & issue_rule::build_railway) != 0);

		int32_t min_build = 0;
		if(bt == economy::province_building_type::railroad) {
			min_build = int32_t(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_railroad));
		} else if(bt == economy::province_building_type::bank) {
			min_build = int32_t(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_bank));
		} else if(bt == economy::province_building_type::university) {
			min_build = int32_t(state.world.province_get_modifier_values(p, sys::provincial_mod_offsets::min_build_university));
		}
		text::add_line_with_condition(state, contents, "fort_build_tt_3", (max_local_lvl - current_lvl - min_build > 0), text::variable_type::x, int64_t(current_lvl), text::variable_type::n, int64_t(min_build), text::variable_type::y, int64_t(max_local_lvl));
	}

	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "alice_province_building_build");

	text::add_line_break_to_layout(state, contents);
	text::add_line(state, contents, "alice_construction_cost");

	// Construction cost goods breakdown
	float factor = economy::build_cost_multiplier(state, p, false);
	auto constr_cost = state.economy_definitions.building_definitions[uint8_t(bt)].cost;

	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		auto box = text::open_layout_box(contents, 0);
		auto cid = constr_cost.commodity_type[i];

		if(!cid) {
			break;
		}
		std::string padding = cid.index() < 10 ? "0" : "";
		std::string description = "@$" + padding + std::to_string(cid.index());
		text::add_unparsed_text_to_layout_box(state, contents, box, description);
		text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(constr_cost.commodity_type[i]));
		text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
		text::add_to_layout_box(state, contents, box, text::fp_one_place{ constr_cost.commodity_amounts[i] * factor });
		text::close_layout_box(contents, box);
	}
};


void province_owner_rgo_commodity_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov_id, dcon::commodity_id c) {
	auto rgo_good = dcon::fatten(state.world, c);
	auto nat_id = state.world.province_get_nation_from_province_ownership(prov_id);
	auto sid = state.world.province_get_state_membership(prov_id);
	auto market = state.world.state_instance_get_market_from_local_market(sid);
	auto mobilization_impact = (state.world.nation_get_is_mobilized(nat_id) ? military::mobilization_impact(state, nat_id) : 1.f);

	text::add_line(state, contents, "provinceview_goodsincome", text::variable_type::goods, rgo_good.get_name(), text::variable_type::value,
					text::fp_currency{ economy::rgo_income(state, c, prov_id) },
		text::variable_type::x, text::fp_two_places{ economy::rgo_output(state, c, prov_id) });

	text::add_line(state, contents, "PROVINCEVIEW_EMPLOYMENT", text::variable_type::value, text::fp_two_places{ economy::rgo_employment(state, rgo_good, prov_id) });
	auto target_employment = state.world.province_get_rgo_target_employment(prov_id, rgo_good);
	auto satisfaction = state.world.province_get_labor_demand_satisfaction(prov_id, economy::labor::no_education);
	text::add_line(state, contents, labour_type_to_employment_type_text_key(economy::labor::no_education), 15);
	text::add_line(state, contents, "target_employment", text::variable_type::value, text::fp_one_place{ target_employment }, 15);
	text::add_line(state, contents, "employment_satisfaction", text::variable_type::value, text::fp_percentage{ satisfaction }, 15);
	auto wage = state.world.province_get_labor_price(prov_id, economy::labor::no_education);
	text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ wage }, 15);

	text::add_line(state, contents, "provinceview_max_employment", text::variable_type::value, text::fp_two_places{ economy::rgo_max_employment(state, rgo_good, prov_id) });
	{
		auto box = text::open_layout_box(contents, 0);
		text::substitution_map sub_map;
		auto const production = economy::rgo_output(state, rgo_good, prov_id);
		text::add_to_substitution_map(sub_map, text::variable_type::curr, text::fp_two_places{ production });
		text::localised_format_box(state, contents, box, std::string_view("production_output_goods_tooltip2"), sub_map);
		text::localised_format_box(state, contents, box, std::string_view("production_output_explanation"));
		text::close_layout_box(contents, box);
	}

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, std::string_view("production_base_output_goods_tooltip"), text::variable_type::base, text::fp_two_places{ economy::rgo_potential_output(state, rgo_good, prov_id) });

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
		auto const throughput = 1.0f + state.world.province_get_modifier_values(prov_id, sys::provincial_mod_offsets::local_rgo_throughput) +
			state.world.nation_get_modifier_values(nat_id, sys::national_mod_offsets::rgo_throughput);
		text::localised_format_box(state, contents, box, std::string_view("production_throughput_efficiency_tooltip"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ throughput }, throughput >= 0.0f ? text::text_color::green : text::text_color::red);

		text::close_layout_box(contents, box);
	}

	auto inputs = economy::rgo_calculate_actual_efficiency_inputs(state, nat_id, market, prov_id, c, mobilization_impact);
	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			if(i == 0) {
				text::add_line(state, contents, "province_rgo_efficiency_inputs");
			}
			auto input_c = dcon::fatten(state.world, inputs.commodity_type[i]);
			text::add_line(state, contents, "province_rgo_efficiency_input", text::variable_type::good, input_c.get_name(), text::variable_type::value, text::fp_two_places{ inputs.commodity_amounts[i] });
		} else {
			break;
		}
	}
};

}
