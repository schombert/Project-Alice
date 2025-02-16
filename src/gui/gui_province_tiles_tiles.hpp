#pragma once

#include "gui_element_types.hpp"
#include "gui_context_window.hpp"
#include "military.hpp"
#include "ai.hpp"

namespace ui {

class tile_type_logic {
public:
	virtual dcon::text_key get_name(sys::state& state, economy::province_tile target) {
		return state.lookup_key("null");
	}

	virtual int get_frame(sys::state& state, economy::province_tile target) {
		return 0;
	}

	virtual bool is_available(sys::state& state, economy::province_tile target) {
		return false;
	}

	virtual void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) {

	}

	virtual void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) {

	}
	virtual ~tile_type_logic() {
	};
};


class empty_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.lookup_key("???");
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 0;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		
	}
};

class factory_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		auto type = state.world.factory_get_building_type(target.factory);
		return state.world.factory_type_get_name(type);
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 5;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override {
		show_context_menu(state, { .factory=target.factory });
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		auto type = state.world.factory_get_building_type(target.factory);
		auto fid = target.factory;

		if(!fid)
			return;
		dcon::nation_id n = state.world.province_get_nation_from_province_ownership(target.province);
		auto p = target.province;
		auto p_fat = fatten(state.world, target.province);
		auto sdef = state.world.abstract_state_membership_get_state(state.world.province_get_abstract_state_membership(p_fat));
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

		auto& inputs = type.get_inputs();
		auto& einputs = type.get_efficiency_inputs();

		//inputs

		float input_total = economy::factory_input_total_cost(state, market, type);
		float min_input_available = economy::factory_min_input_available(state, market, type);
		float e_input_total = economy::factory_e_input_total_cost(state, market, type);
		float min_e_input_available = economy::factory_min_e_input_available(state, market, type);

		//modifiers

		float input_multiplier = economy::factory_input_multiplier(state, fac, n, p, s);
		float e_input_multiplier = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_maintenance) + 1.0f;
		float throughput_multiplier = economy::factory_throughput_multiplier(state, type, n, p, s, fac.get_level());
		float output_multiplier = economy::factory_output_multiplier(state, fac, n, market, p);

		float bonus_profit_thanks_to_max_e_input = fac.get_building_type().get_output_amount()
			* 0.25f
			* throughput_multiplier
			* output_multiplier
			* min_input_available
			* economy::price(state, market, fac.get_building_type().get_output());

		// if efficiency inputs are not worth it, then do not buy them
		if(bonus_profit_thanks_to_max_e_input < e_input_total * e_input_multiplier * input_multiplier)
			min_e_input_available = 0.f;

		float base_throughput =
			(
				state.world.factory_get_unqualified_employment(fac)
				* state.world.market_get_labor_demand_satisfaction(market, economy::labor::no_education)
				* economy::unqualified_throughput_multiplier
				+
				state.world.factory_get_primary_employment(fac)
				* state.world.market_get_labor_demand_satisfaction(market, economy::labor::basic_education)
			)
			* state.world.factory_get_level(fac)
			* economy::factory_throughput_additional_multiplier(
				state,
				fac,
				mobilization_impact,
				false
			);

		float effective_production_scale = base_throughput;

		auto amount = (0.75f + 0.25f * min_e_input_available) * min_input_available * effective_production_scale;

		text::add_line(state, contents, state.world.factory_type_get_name(type));
		text::add_line(state, contents, "factory_level", text::variable_type::val, state.world.factory_get_level(fac));

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_stats_1", text::variable_type::val, text::fp_percentage{ amount });

		text::add_line(state, contents, "factory_stats_2", text::variable_type::val,
				text::fp_percentage{ economy::factory_total_employment_score(state, fid) });

		text::add_line(state, contents, "factory_stats_3", text::variable_type::val,
				text::fp_one_place{ state.world.factory_get_actual_production(fid) }, text::variable_type::x, type.get_output().get_name());

		text::add_line(state, contents, "factory_stats_4", text::variable_type::val,
			text::fp_currency{
				state.world.factory_get_full_output_cost(fid)
				- state.world.factory_get_full_input_cost(fid)
				- state.world.factory_get_full_labor_cost(fid)
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
				* min_input_available
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
				* min_e_input_available
				* min_input_available
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
				* (0.75f + 0.25f * min_e_input_available)
				* throughput_multiplier
				* output_multiplier
				* min_input_available
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

		auto wage_unqualified = state.world.market_get_labor_price(market, economy::labor::no_education);
		auto wage_primary = state.world.market_get_labor_price(market, economy::labor::basic_education);
		auto wage_secondary = state.world.market_get_labor_price(market, economy::labor::high_education);

		float wage_estimation = 0.f;

		auto per_level_employment = state.world.factory_type_get_base_workforce(type);

		wage_estimation +=
			wage_unqualified
			* per_level_employment
			* float(state.world.factory_get_level(fid))
			* state.world.factory_get_unqualified_employment(fid)
			* state.world.market_get_labor_demand_satisfaction(market, economy::labor::no_education);

		wage_estimation +=
			wage_primary
			* per_level_employment
			* float(state.world.factory_get_level(fid))
			* state.world.factory_get_primary_employment(fid)
			* state.world.market_get_labor_demand_satisfaction(market, economy::labor::basic_education);

		wage_estimation +=
			wage_secondary
			* per_level_employment
			* float(state.world.factory_get_level(fid))
			* state.world.factory_get_secondary_employment(fid)
			* state.world.market_get_labor_demand_satisfaction(market, economy::labor::high_education);

		total_expenses += wage_estimation;

		named_money_line("factory_stats_wage",
			-wage_estimation
		);

		text::add_line_break_to_layout(state, contents);

		named_money_line("factory_stats_expenses",
			-total_expenses
		);

		output_cost_line(type.get_output(), type.get_output_amount());

		float desired_income = economy::factory_desired_raw_profit(fac, total_expenses);

		named_money_line("factory_stats_desired_income",
			desired_income
		);

		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "factory_stats_7", text::variable_type::val, text::fp_percentage{ fac.get_level() / 100.f });

	}
};

class rgo_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.world.commodity_get_name(target.rgo_commodity);
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return (state.world.commodity_get_is_mine(target.rgo_commodity) ? 3 : 2);
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	// Done
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		auto commodity_name = state.world.commodity_get_name(target.rgo_commodity);
		text::add_line(state, contents, commodity_name);

		auto owner = state.world.province_get_nation_from_province_ownership(target.province);

		{
			text::add_line(state, contents, "provinceview_goodsincome", text::variable_type::goods, commodity_name, text::variable_type::value,
						text::fp_three_places{ province::rgo_income(state, target.province) });

			auto profit = state.world.province_get_rgo_profit_per_good(target.province, target.rgo_commodity);
			text::add_line(state, contents, "provinceview_goodsprofit", text::variable_type::value,
						text::fp_currency{ profit });
		}

		{
			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub_map;
			auto const production = province::rgo_production_quantity(state, target.province, target.rgo_commodity);

			text::add_to_substitution_map(sub_map, text::variable_type::curr, text::fp_two_places{ production });
			text::localised_format_box(state, contents, box, std::string_view("production_output_goods_tooltip2"), sub_map);
			text::localised_format_box(state, contents, box, std::string_view("production_output_explanation"));
			text::close_layout_box(contents, box);
		}

		text::add_line_break_to_layout(state, contents);

		{
			auto const base_size = economy::rgo_effective_size(state, owner, target.province, target.rgo_commodity) * state.world.commodity_get_rgo_amount(target.rgo_commodity);
			text::add_line(state, contents, std::string_view("production_base_output_goods_tooltip"), text::variable_type::base, text::fp_two_places{ base_size });
		}

		{
			auto box = text::open_layout_box(contents, 0);
			bool const is_mine = state.world.commodity_get_is_mine(target.rgo_commodity);
			auto const efficiency = 1.0f +
				state.world.province_get_modifier_values(target.province,
						is_mine ? sys::provincial_mod_offsets::mine_rgo_eff : sys::provincial_mod_offsets::farm_rgo_eff) +
				state.world.nation_get_modifier_values(owner,
						is_mine ? sys::national_mod_offsets::mine_rgo_eff : sys::national_mod_offsets::farm_rgo_eff);
			text::localised_format_box(state, contents, box, std::string_view("production_output_efficiency_tooltip"));
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ efficiency }, efficiency >= 0.0f ? text::text_color::green : text::text_color::red);
			text::close_layout_box(contents, box);
		}

		text::add_line_break_to_layout(state, contents);

		{
			auto box = text::open_layout_box(contents, 0);
			auto const throughput = state.world.province_get_rgo_employment(target.province);
			text::localised_format_box(state, contents, box, std::string_view("production_throughput_efficiency_tooltip"));
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ throughput }, throughput >= 0.0f ? text::text_color::green : text::text_color::red);

			text::close_layout_box(contents, box);
		}
	}
};


class regiment_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		auto type = state.world.regiment_get_type(target.regiment);
		return state.military_definitions.unit_base_definitions[type].name;
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 10;
	}

	// Done
	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override {
		auto army = state.world.regiment_get_army_from_army_membership(target.regiment);
		state.selected_armies.clear();
		state.selected_navies.clear();
		state.select(army);
		state.map_state.center_map_on_province(state, state.world.army_get_location_from_army_location(army));
	}

	// Done
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		auto type = state.world.regiment_get_type(target.regiment);
		text::add_line(state, contents, state.military_definitions.unit_base_definitions[type].name);

		auto base_pop = state.world.regiment_get_pop_from_regiment_source(target.regiment);

		if(!base_pop) {
			text::add_line(state, contents, "reinforce_rate_none");
		} else {
			// Added culture name to the tooltip
			text::add_line(state, contents, "x_from_y", text::variable_type::x, state.world.pop_get_poptype(base_pop).get_name(), text::variable_type::y, state.world.pop_get_province_from_pop_location(base_pop), text::variable_type::culture, state.world.pop_get_culture(base_pop).get_name());
			text::add_line_break_to_layout(state, contents);

			auto reg_range = state.world.pop_get_regiment_source(base_pop);
			text::add_line(state, contents, "pop_size_unitview",
				text::variable_type::val, text::pretty_integer{ int64_t(state.world.pop_get_size(base_pop)) },
				text::variable_type::allowed, military::regiments_possible_from_pop(state, base_pop),
				text::variable_type::current, int64_t(reg_range.end() - reg_range.begin())
			);

			auto a = state.world.regiment_get_army_from_army_membership(target.regiment);
			auto reinf = state.defines.pop_size_per_regiment * military::calculate_army_combined_reinforce(state, a);
			if(reinf >= 2.0f) {
				text::add_line(state, contents, "reinforce_rate", text::variable_type::x, int64_t(reinf));
			} else {
				text::add_line(state, contents, "reinforce_rate_none");
			}
		}
	}
};

class province_building_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.lookup_key(economy::province_building_type_get_name(target.province_building));
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		if(target.province_building == economy::province_building_type::railroad) {
			return 8;
		} else if(target.province_building == economy::province_building_type::naval_base) {
			return 9;
		} else if(target.province_building == economy::province_building_type::fort) {
			return 12;
		}
		return 0;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	// Done
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		text::add_line(state, contents, state.lookup_key(economy::province_building_type_get_name(target.province_building)));
	}
};


class province_resource_potential_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.lookup_key("resource_potential");
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 13;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override {
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		auto limit = state.world.province_get_factory_max_level_per_good(target.province, target.potential_commodity);
		text::add_line(state, contents, "available_potential", text::variable_type::what, state.world.commodity_get_name(target.potential_commodity),
			text::variable_type::val, limit);
	}
};


class province_build_new_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, economy::province_tile target) noexcept override {
		return state.lookup_key("new");
	}

	bool is_available(sys::state& state, economy::province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, economy::province_tile target) {
		return 1;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override {
		show_context_menu(state, { .province=target.province });
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		text::add_line(state, contents, state.lookup_key("new"));
	}
};

} // namespace ui
