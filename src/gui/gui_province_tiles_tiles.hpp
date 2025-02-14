#pragma once

#include "gui_element_types.hpp"
#include "military.hpp"
#include "ai.hpp"

namespace ui {

enum class province_tile_types : uint8_t {
	province_building,
	factory,
	regiment,
};

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
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		auto type = state.world.factory_get_building_type(target.factory);

		text::add_line(state, contents, state.world.factory_type_get_name(type));
		text::add_line_break_to_layout(state, contents);

		text::add_line_with_condition(state, contents, "iaction_explain_5", state.world.nation_get_is_great_power(state.local_player_nation));

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
			return 13;
		}
		return 0;
	}

	void button_action(sys::state& state, economy::province_tile target, ui::element_base* parent) noexcept override { }

	// Done
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, economy::province_tile target) noexcept override {
		text::add_line(state, contents, state.lookup_key(economy::province_building_type_get_name(target.province_building)));
	}
};

} // namespace ui
