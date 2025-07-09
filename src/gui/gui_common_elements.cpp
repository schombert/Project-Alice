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


void factory_stats_tooltip(sys::state& state, text::columnar_layout& contents, dcon::factory_id fid) {
	auto p = state.world.factory_get_province_from_factory_location(fid);
	auto p_fat = fatten(state.world, p);
	auto n = p_fat.get_nation_from_province_ownership();
	auto sdef = state.world.abstract_state_membership_get_state(state.world.province_get_abstract_state_membership(p));
	dcon::state_instance_id s{};
	state.world.for_each_state_instance([&](dcon::state_instance_id id) {
		if(state.world.state_instance_get_definition(id) == sdef)
			s = id;
	});
	auto market = state.world.state_instance_get_market_from_local_market(s);

	// nation data

	float mobilization_impact = state.world.nation_get_is_mobilized(n) ? military::mobilization_impact(state, n) : 1.0f;
	auto cap_prov = state.world.nation_get_capital(n);
	auto cap_continent = state.world.province_get_continent(cap_prov);
	auto cap_region = state.world.province_get_connected_region_id(cap_prov);


	auto fac = fatten(state.world, fid);
	auto type = state.world.factory_get_building_type(fid);

	auto& inputs = type.get_inputs();
	auto& einputs = type.get_efficiency_inputs();

	//inputs
	auto inputs_data = economy::get_inputs_data(state, market, type.get_inputs());
	auto e_inputs_data = economy::get_inputs_data(state, market, type.get_efficiency_inputs());

	//modifiers

	float input_multiplier = economy::factory_input_multiplier(state, fac, n, p, s);
	float e_input_multiplier = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
	float throughput_multiplier = economy::factory_throughput_multiplier(state, type, n, p, s, fac.get_size());
	float output_multiplier = economy::factory_output_multiplier_no_secondary_workers(state, fac, n, p);

	float bonus_profit_thanks_to_max_e_input = fac.get_building_type().get_output_amount()
		* 0.25f
		* throughput_multiplier
		* output_multiplier
		* economy::price(state, market, fac.get_building_type().get_output());

	// if efficiency inputs are not worth it, then do not buy them
	if(bonus_profit_thanks_to_max_e_input < e_inputs_data.total_cost * e_input_multiplier * input_multiplier)
		e_inputs_data.min_available = 0.f;

	float base_throughput =
		(
			state.world.factory_get_unqualified_employment(fac)
			* state.world.province_get_labor_demand_satisfaction(fac.get_province_from_factory_location(), economy::labor::no_education)
			* economy::unqualified_throughput_multiplier
			+
			state.world.factory_get_primary_employment(fac)
			* state.world.province_get_labor_demand_satisfaction(fac.get_province_from_factory_location(), economy::labor::basic_education)
		)
		* economy::factory_throughput_additional_multiplier(
			state,
			fac,
			mobilization_impact,
			false
		);

	float effective_production_scale = economy::factory_total_employment_score(state, fid);
	auto amount = (0.75f + 0.25f * e_inputs_data.min_available) * inputs_data.min_available * effective_production_scale;

	text::add_line(state, contents, state.world.factory_type_get_name(type));

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "factory_stats_1", text::variable_type::val, text::fp_percentage{ amount });
	text::add_line(state, contents, "factory_stats_2", text::variable_type::val, text::fp_percentage{ effective_production_scale });

	auto employment = economy::factory_total_employment(state, fid);
	auto target_employment = economy::factory_total_desired_employment(state, fid);
	auto profit_explanation = economy::explain_last_factory_profit(state, fid);

	text::add_line(state, contents, "factory_employment", text::variable_type::value, text::fp_one_place{ employment });
	text::add_line(state, contents, "target_employment", text::variable_type::value, text::fp_one_place{ target_employment }, 15);
	text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ -profit_explanation.wages }, 15);

	{
		auto ftte = state.world.factory_get_unqualified_employment(fid);
		auto satisfaction = state.world.province_get_labor_demand_satisfaction(p, economy::labor::no_education);
		auto wage = state.world.province_get_labor_price(p, economy::labor::no_education);

		text::add_line(state, contents, "employment_type_no_education", 30);
		text::add_line(state, contents, "employment_satisfaction", text::variable_type::value, text::fp_percentage{ satisfaction }, 30);
		text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ wage }, 30);
	}
	{
		auto ftte = state.world.factory_get_primary_employment(fid);
		auto satisfaction = state.world.province_get_labor_demand_satisfaction(p, economy::labor::basic_education);
		auto wage = state.world.province_get_labor_price(p, economy::labor::basic_education);

		text::add_line(state, contents, "employment_type_basic_education", 30);
		text::add_line(state, contents, "employment_satisfaction", text::variable_type::value, text::fp_percentage{ satisfaction }, 30);
		text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ wage }, 30);
	}
	{
		auto ftte = state.world.factory_get_secondary_employment(fid);
		auto satisfaction = state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);
		auto wage = state.world.province_get_labor_price(p, economy::labor::high_education);

		text::add_line(state, contents, "employment_type_high_education", 30);
		text::add_line(state, contents, "employment_satisfaction", text::variable_type::value, text::fp_percentage{ satisfaction }, 30);
		text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ wage }, 30);
	}

	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ -profit_explanation.inputs }, text::text_color::red);
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ -profit_explanation.maintenance }, text::text_color::red);
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ -profit_explanation.expansion }, text::text_color::red);
		text::close_layout_box(contents, box);
	}
	text::add_line(state, contents, "factory_stats_3",
		text::variable_type::val, text::fp_one_place{ state.world.factory_get_output(fid) },
		text::variable_type::good, type.get_output().get_name(),
	text::variable_type::x, text::fp_currency{ profit_explanation.output });

	text::add_line(state, contents, "factory_stats_4", text::variable_type::val,
		text::fp_currency{
			profit_explanation.profit
		}
	);

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "factory_stats_5");

	float total_expenses = 0.f;

	int position_demand_sat = 100;
	int position_amount = 180;
	int position_cost = 250;

	auto input_cost_line = [&](
		dcon::commodity_id cid,
		float base_amount
	) {
		auto box = text::open_layout_box(contents);
		text::layout_box name_entry = box;
		text::layout_box demand_satisfaction = box;
		text::layout_box amount_box = box;
		text::layout_box cost_box = box;

		demand_satisfaction.x_position += position_demand_sat;
		amount_box.x_position += position_amount;
		cost_box.x_position += position_cost;

		name_entry.x_size /= 10;

		std::string padding = cid.index() < 10 ? "0" : "";
		std::string description = "@$" + padding + std::to_string(cid.index());
		text::add_unparsed_text_to_layout_box(state, contents, name_entry, description);

		text::add_to_layout_box(state, contents, name_entry, state.world.commodity_get_name(cid));

		auto sat = state.world.market_get_demand_satisfaction(market, cid);
		text::add_to_layout_box(state, contents,
			demand_satisfaction,
			text::fp_percentage{ sat },
			sat >= 0.9f ? text::text_color::green : text::text_color::red
		);

		float amount =
			base_amount
			* input_multiplier
			* throughput_multiplier
			* inputs_data.min_available
			* effective_production_scale;

		float cost =
			economy::price(state, market, cid)
			* amount;

		total_expenses += cost;

		text::add_to_layout_box(state, contents, amount_box, text::fp_two_places{ amount });
		text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ -cost }, text::text_color::red);

		text::add_to_layout_box(state, contents, box, std::string(" "));
		text::close_layout_box(contents, box);
		};

	auto e_input_cost_line = [&](
		dcon::commodity_id cid,
		float base_amount
	) {
		auto box = text::open_layout_box(contents);
		text::layout_box name_entry = box;
		text::layout_box demand_satisfaction = box;
		text::layout_box amount_box = box;
		text::layout_box cost_box = box;

		demand_satisfaction.x_position += position_demand_sat;
		amount_box.x_position += position_amount;
		cost_box.x_position += position_cost;

		name_entry.x_size /= 10;
		std::string padding = cid.index() < 10 ? "0" : "";
		std::string description = "@$" + padding + std::to_string(cid.index());
		text::add_unparsed_text_to_layout_box(state, contents, name_entry, description);
		text::add_to_layout_box(state, contents, name_entry, state.world.commodity_get_name(cid));

		auto sat = state.world.market_get_demand_satisfaction(market, cid);
		text::add_to_layout_box(state, contents,
			demand_satisfaction,
			text::fp_percentage{ sat },
			sat >= 0.9f ? text::text_color::green : text::text_color::red
		);

		float amount =
			base_amount
			* input_multiplier * e_input_multiplier
			* e_inputs_data.min_available
			* effective_production_scale;

		float cost =
			economy::price(state, market, cid)
			* amount;

		total_expenses += cost;

		text::add_to_layout_box(state, contents, amount_box, text::fp_two_places{ amount });
		text::add_to_layout_box(state, contents, cost_box, text::fp_currency{ -cost }, text::text_color::red);

		text::add_to_layout_box(state, contents, box, std::string(" "));
		text::close_layout_box(contents, box);
		};

	auto named_money_line = [&](
		std::string_view loc,
		float value
	) {
		auto box = text::open_layout_box(contents);
		text::layout_box name_entry = box;
		text::layout_box cost = box;

		cost.x_position += position_cost;
		name_entry.x_size /= 10;

		text::localised_format_box(state, contents, name_entry, loc);
		text::add_to_layout_box(state, contents, cost, text::fp_currency{ value }, value >= 0.f ? text::text_color::green : text::text_color::red);

		text::add_to_layout_box(state, contents, box, std::string(" "));
		text::close_layout_box(contents, box);
		};

	auto output_cost_line = [&](
		dcon::commodity_id cid,
		float base_amount
	) {
		auto box = text::open_layout_box(contents);
		text::layout_box name_entry = box;
		text::layout_box amount = box;
		text::layout_box cost = box;

		amount.x_position += position_amount;
		cost.x_position += position_cost;

		name_entry.x_size /= 10;

		std::string padding = cid.index() < 10 ? "0" : "";
		std::string description = "@$" + padding + std::to_string(cid.index());
		text::add_unparsed_text_to_layout_box(state, contents, name_entry, description);
		text::add_to_layout_box(state, contents, name_entry, state.world.commodity_get_name(cid));

		float output_amount =
			base_amount
			* (0.75f + 0.25f * e_inputs_data.min_available)
			* throughput_multiplier
			* output_multiplier
			* inputs_data.min_available
			* effective_production_scale;

		float output_cost =
			economy::price(state, market, cid)
			* output_amount;

		text::add_to_layout_box(state, contents, amount, text::fp_two_places{ output_amount });
		text::add_to_layout_box(state, contents, cost, text::fp_currency{ output_cost }, text::text_color::green);

		text::add_to_layout_box(state, contents, box, std::string(" "));
		text::close_layout_box(contents, box);
		};

	for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
		if(inputs.commodity_type[i]) {
			input_cost_line(inputs.commodity_type[i], inputs.commodity_amounts[i]);
		} else {
			break;
		}
	}

	text::add_line_break_to_layout(state, contents);

	text::add_line(state, contents, "factory_stats_6");

	for(uint32_t i = 0; i < economy::small_commodity_set::set_size; ++i) {
		if(einputs.commodity_type[i]) {
			e_input_cost_line(einputs.commodity_type[i], einputs.commodity_amounts[i]);
		} else {
			break;
		}
	}

	text::add_line_break_to_layout(state, contents);

	auto const min_wage_factor = economy::pop_min_wage_factor(state, n);

	auto prov = fac.get_province_from_factory_location();

	auto wage_unqualified = state.world.province_get_labor_price(prov, economy::labor::no_education);
	auto wage_primary = state.world.province_get_labor_price(prov, economy::labor::basic_education);
	auto wage_secondary = state.world.province_get_labor_price(prov, economy::labor::high_education);

	float wage_estimation = 0.f;

	wage_estimation +=
		wage_unqualified
		* state.world.factory_get_unqualified_employment(fid)
		* state.world.province_get_labor_demand_satisfaction(prov, economy::labor::no_education);

	wage_estimation +=
		wage_primary
		* state.world.factory_get_primary_employment(fid)
		* state.world.province_get_labor_demand_satisfaction(prov, economy::labor::basic_education);

	wage_estimation +=
		wage_secondary
		* state.world.factory_get_secondary_employment(fid)
		* state.world.province_get_labor_demand_satisfaction(prov, economy::labor::high_education);

	total_expenses += wage_estimation;

	named_money_line("factory_stats_wage",
		-wage_estimation
	);

	text::add_line_break_to_layout(state, contents);

	named_money_line("factory_stats_expenses",
		-total_expenses
	);

	output_cost_line(type.get_output(), type.get_output_amount());

	float desired_income = total_expenses;

	named_money_line("factory_stats_desired_income",
		desired_income
	);

	text::add_line(state, contents, "factory_stats_7", text::variable_type::val, text::fp_percentage{ fac.get_size() / fac.get_building_type().get_base_workforce() / 100.f });
};

} // namespace ui
