#include "gui_common_elements.hpp"
#include "ai_alliances.hpp"

namespace ui {
bool country_category_filter_check(sys::state& state, country_list_filter filt, dcon::nation_id a, dcon::nation_id b) {
	switch(filt) {
	case country_list_filter::all:
		return true;
	case country_list_filter::allies:
		return nations::are_allied(state, a, b);
	case country_list_filter::enemies:
		return military::are_at_war(state, a, b);
	case country_list_filter::sphere:
		return state.world.nation_get_in_sphere_of(b) == a;
	case country_list_filter::neighbors:
		return bool(state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b));
	case country_list_filter::find_allies:
		return ai::ai_will_accept_alliance(state, b, a)
			&& command::can_ask_for_alliance(state, a, b, false);
	case country_list_filter::influenced:
		return (state.world.gp_relationship_get_status(state.world.get_gp_relationship_by_gp_influence_pair(b, a))
			& nations::influence::priority_mask) != nations::influence::priority_zero
			&& state.world.nation_get_in_sphere_of(b) != a;
	case country_list_filter::neighbors_no_vassals:
		for(const auto sub : state.world.nation_get_overlord_as_ruler(b)) {
			if(state.world.get_nation_adjacency_by_nation_adjacency_pair(a, sub.get_subject()))
				return true;
		}
		return !state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(b))
			&& state.world.get_nation_adjacency_by_nation_adjacency_pair(a, b);
	default:
		return true;
	}
}

void sort_countries(sys::state& state, std::vector<dcon::nation_id>& list, country_list_sort sort, bool sort_ascend) {
	switch(sort) {
	case country_list_sort::country: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto a_name = text::produce_simple_string(state, text::get_name(state, a));
			auto b_name = text::produce_simple_string(state, text::get_name(state, b));
			return a_name < b_name;
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::economic_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_industrial_rank(a) < state.world.nation_get_industrial_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::military_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_military_rank(a) < state.world.nation_get_military_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::prestige_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_prestige_rank(a) < state.world.nation_get_prestige_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::total_rank: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_rank(a) < state.world.nation_get_rank(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::relation: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rid_a = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, a);
			auto va = bool(rid_a) ? state.world.diplomatic_relation_get_value(rid_a) : 0;
			auto rid_b = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, b);
			auto vb = bool(rid_b) ? state.world.diplomatic_relation_get_value(rid_b) : 0;
			return va > vb;
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::opinion: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto grid_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			auto va = bool(grid_a) ? state.world.gp_relationship_get_status(grid_a) : 0;
			auto grid_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			auto vb = bool(grid_b) ? state.world.gp_relationship_get_status(grid_b) : 0;
			return nations::influence::is_influence_level_greater(va & nations::influence::level_mask,
					vb & nations::influence::level_mask);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::priority: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			uint8_t rel_flags_a = bool(rel_a) ? state.world.gp_relationship_get_status(rel_a) : 0;
			auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			uint8_t rel_flags_b = bool(rel_b) ? state.world.gp_relationship_get_status(rel_b) : 0;
			return (rel_flags_a & nations::influence::priority_mask) > (rel_flags_b & nations::influence::priority_mask);
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::boss: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return state.world.nation_get_in_sphere_of(a).id.index() > state.world.nation_get_in_sphere_of(b).id.index();
		};
		std::stable_sort(list.begin(), list.end(), f);
	}
		break;
	case country_list_sort::player_influence: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, state.local_player_nation);
			auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, state.local_player_nation);
			return state.world.gp_relationship_get_influence(rel_a) > state.world.gp_relationship_get_influence(rel_b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::player_investment: {
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			auto urel_a = state.world.get_unilateral_relationship_by_unilateral_pair(a, state.local_player_nation);
			auto urel_b = state.world.get_unilateral_relationship_by_unilateral_pair(b, state.local_player_nation);
			return state.world.unilateral_relationship_get_foreign_investment(urel_a) >
						 state.world.unilateral_relationship_get_foreign_investment(urel_b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	case country_list_sort::factories: {
		auto num_factories = [&](dcon::nation_id n) {
			int32_t total = 0;
			for(auto p : state.world.nation_get_province_ownership(n)) {
				auto fac = p.get_province().get_factory_location();
				total += int32_t(fac.end() - fac.begin());
			}
			return total;
		};
		auto f = [&](dcon::nation_id a, dcon::nation_id b) {
			return num_factories(a) > num_factories(b);
		};
		std::stable_sort(list.begin(), list.end(), f);
	} break;
	default:
		uint8_t rank = uint8_t(sort) & 0x3F;
		dcon::nation_id gp = nations::get_nth_great_power(state, rank);

		if((uint8_t(sort) & uint8_t(country_list_sort::gp_influence)) != 0) {
			auto f = [&](dcon::nation_id a, dcon::nation_id b) {
				auto rel_a = state.world.get_gp_relationship_by_gp_influence_pair(a, gp);
				auto rel_b = state.world.get_gp_relationship_by_gp_influence_pair(b, gp);
				return state.world.gp_relationship_get_influence(rel_a) > state.world.gp_relationship_get_influence(rel_b);
			};
			std::sort(list.begin(), list.end(), f);
		} else if((uint8_t(sort) & uint8_t(country_list_sort::gp_investment)) != 0) {
			auto f = [&](dcon::nation_id a, dcon::nation_id b) {
				auto urel_a = state.world.get_unilateral_relationship_by_unilateral_pair(a, gp);
				auto urel_b = state.world.get_unilateral_relationship_by_unilateral_pair(b, gp);
				return state.world.unilateral_relationship_get_foreign_investment(urel_a) >
							 state.world.unilateral_relationship_get_foreign_investment(urel_b);
			};
			std::stable_sort(list.begin(), list.end(), f);
		} else {
			assert(0);
		}
		break;
	}

	if(!sort_ascend) {
		std::reverse(list.begin(), list.end());
	}

}
std::string get_status_text(sys::state& state, dcon::nation_id nation_id) {
	switch(nations::get_status(state, nation_id)) {
	case nations::status::great_power:
		return text::produce_simple_string(state, "diplomacy_greatnation_status");
	case nations::status::secondary_power:
		return text::produce_simple_string(state, "diplomacy_colonialnation_status");
	case nations::status::civilized:
		return text::produce_simple_string(state, "diplomacy_civilizednation_status");
	case nations::status::westernizing:
		return text::produce_simple_string(state, "diplomacy_almost_western_nation_status");
	case nations::status::uncivilized:
		return text::produce_simple_string(state, "diplomacy_uncivilizednation_status");
	case nations::status::primitive:
		return text::produce_simple_string(state, "diplomacy_primitivenation_status");
	default:
		return text::produce_simple_string(state, "diplomacy_greatnation_status");
	}
}

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
	for(int i = 0; i < set.set_size; i++) {
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

} // namespace ui
