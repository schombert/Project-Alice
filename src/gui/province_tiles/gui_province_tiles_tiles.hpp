#pragma once

#include "gui_element_types.hpp"
#include "gui_context_window.hpp"
#include "gui_province_window.hpp"
#include "economy_trade_routes.hpp"
#include "military.hpp"
#include "ai.hpp"
#include "labour_details.hpp"

namespace ui {

class tile_type_logic {
public:
	virtual dcon::text_key get_name(sys::state& state, province_tile target) {
		return state.lookup_key("null");
	}

	virtual int get_frame(sys::state& state, province_tile target) {
		return 0;
	}

	virtual dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) {
		return dcon::commodity_id{};
	}

	virtual dcon::unit_type_id get_unit_frame(sys::state& state, province_tile target) {
		return dcon::unit_type_id{};
	}

	virtual bool is_available(sys::state& state, province_tile target) {
		return false;
	}

	virtual void button_action(sys::state& state, province_tile target, ui::element_base* parent) {

	}

	virtual void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) {

	}
	virtual ~tile_type_logic() {
	};
};

class empty_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("???");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 0;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		
	}
};

class factory_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		auto type = state.world.factory_get_building_type(target.factory);
		return state.world.factory_type_get_name(type);
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override  {
		return 5;
	}

	dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) noexcept override  {
		auto type = state.world.factory_get_building_type(target.factory);
		return state.world.factory_type_get_output(type);
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		show_context_menu(state, { .factory=target.factory });
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto fid = target.factory;

		if(!fid)
			return;

		factory_stats_tooltip(state, contents, fid);
	}
};

class rgo_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.world.commodity_get_name(target.rgo_commodity);
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override  {
		return (state.world.commodity_get_is_mine(target.rgo_commodity) ? 3 : 2);
	}

	dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) noexcept override  {
		return target.rgo_commodity;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		// Switch to economy scene on click
		game_scene::switch_scene(state, game_scene::scene_id::in_game_economy_viewer);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto commodity_name = state.world.commodity_get_name(target.rgo_commodity);
		text::add_line(state, contents, commodity_name);
		text::add_line_break_to_layout(state, contents);

		province_owner_rgo_commodity_tooltip(state, contents, target.province, target.rgo_commodity);
	}
};


class regiment_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		auto type = state.world.regiment_get_type(target.regiment);
		return state.military_definitions.unit_base_definitions[type].name;
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 10;
	}

	dcon::unit_type_id get_unit_frame(sys::state& state, province_tile target) noexcept override {
		if(!target.regiment) {
			return dcon::unit_type_id{};
		}
		dcon::unit_type_id utid = state.world.regiment_get_type(target.regiment);
		return utid;
	}

	// Done
	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		auto army = state.world.regiment_get_army_from_army_membership(target.regiment);
		state.selected_armies.clear();
		state.selected_navies.clear();
		state.select(army);
		state.map_state.center_map_on_province(state, state.world.army_get_location_from_army_location(army));
	}

	// Done
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
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
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key(economy::province_building_type_get_name(target.province_building));
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		auto level = state.world.province_get_building_level(target.province, uint8_t(target.province_building));

		if(target.province_building == economy::province_building_type::railroad && level == 0) {
			return 21;
		} else if(target.province_building == economy::province_building_type::railroad && level > 0) {
			return 8;
		} else if(target.province_building == economy::province_building_type::naval_base) {
			return 9;
		} else if(target.province_building == economy::province_building_type::fort) {
			return 12;
		}
		return 0;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		province_building_tooltip(state, contents, target.province, target.province_building);
	}
};


class province_resource_potential_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("resource_potential");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override  {
		return 13;
	}

	dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) noexcept override  {
		return target.potential_commodity;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto limit = state.world.province_get_factory_max_size(target.province, target.potential_commodity) / 10000.f;
		text::add_line(state, contents, "available_potential", text::variable_type::what, state.world.commodity_get_name(target.potential_commodity),
			text::variable_type::val, (int)limit);
	}
};


class province_build_new_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("new");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 1;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		show_context_menu(state, { .province=target.province });
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		text::add_line(state, contents, state.lookup_key("new"));
	}
};

class factory_construction_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("new");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 14;
	}

	dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) noexcept override {
		auto c = target.factory_construction;
		auto ft = state.world.factory_construction_get_type(c);
		return state.world.factory_type_get_output(ft);
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		factory_construction_tooltip(state, contents, target.factory_construction);
	}
};


class local_administration_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("administration_tile");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 15;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto n = state.world.province_get_nation_from_province_ownership(target.province);
		auto budget_priority = float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.f;

		text::add_line(state, contents, "local_admin");

		text::add_line(state, contents, "local_admin_spending", text::variable_type::value, text::fp_currency{ economy::estimate_spendings_administration(state, n, budget_priority) });

		auto records = economy::explain_local_administration_employment(state, target.province);
		for(auto record : records) {
			text::add_line(state, contents, "admin_employment", text::variable_type::value, text::fp_one_place{ record.actual_employment });
			text::add_line(state, contents, labour_type_to_employment_type_text_key(record.employment_type), 15);
			text::add_line(state, contents, "target_employment", text::variable_type::value, text::fp_one_place{ record.target_employment }, 15);
			text::add_line(state, contents, "employment_satisfaction", text::variable_type::value, text::fp_percentage{ record.satisfaction }, 15);

			auto wage = state.world.province_get_labor_price(target.province, record.employment_type);
			text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ wage }, 15);
		}

		text::add_line(state, contents, "local_admin_efficiency", text::variable_type::value, text::fp_percentage{ economy::local_administration_efficiency });

		text::add_line_break_to_layout(state, contents);

		auto info = economy::explain_tax_income_local(state, n, target.province);

		text::add_line(state, contents, "tax_collection_rate", text::variable_type::value, text::fp_percentage{ info.local_multiplier });
		text::add_line(state, contents, "poor_potential", text::variable_type::value, text::fp_currency{ info.poor_potential });
		text::add_line(state, contents, "mid_potential", text::variable_type::value, text::fp_percentage{ info.mid_potential });
		text::add_line(state, contents, "rich_potential", text::variable_type::value, text::fp_percentage{ info.rich_potential });
		text::add_line(state, contents, "poor_taxes", text::variable_type::value, text::fp_percentage{ info.poor });
		text::add_line(state, contents, "mid_taxes", text::variable_type::value, text::fp_percentage{ info.mid });
		text::add_line(state, contents, "rich_taxes", text::variable_type::value, text::fp_percentage{ info.rich });
	}
};


class capital_administration_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("administration_tile");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 17;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto n = state.world.province_get_nation_from_province_ownership(target.province);
		auto budget_priority = float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.f;

		text::add_line(state, contents, "capital_admin");

		text::add_line(state, contents, "local_admin_spending", text::variable_type::value, text::fp_currency{ economy::estimate_spendings_administration_capital(state, n, budget_priority) });

		text::add_line_break_to_layout(state, contents);

		auto records = economy::explain_capital_administration_employment(state, n);
		for(auto record : records) {
			text::add_line(state, contents, "admin_employment", text::variable_type::value, text::fp_one_place{ record.actual_employment });
			text::add_line(state, contents, labour_type_to_employment_type_text_key(record.employment_type), 15);
			text::add_line(state, contents, "target_employment", text::variable_type::value, text::fp_one_place{ record.target_employment }, 15);
			text::add_line(state, contents, "employment_satisfaction", text::variable_type::value, text::fp_percentage{ record.satisfaction }, 15);

			auto wage = state.world.province_get_labor_price(target.province, record.employment_type);
			text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ wage }, 15);
		}

		text::add_line_break_to_layout(state, contents);

		auto info = economy::explain_tax_income_local(state, n, target.province);

		text::add_line(state, contents, "tax_collection_rate", text::variable_type::value, text::fp_percentage{ info.local_multiplier });
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "poor_taxes", text::variable_type::value, text::fp_currency{ info.poor });
		text::add_line(state, contents, "poor_potential", text::variable_type::value, text::fp_currency{ info.poor_potential }, 15);
		text::add_line(state, contents, "mid_taxes", text::variable_type::value, text::fp_currency{ info.mid });
		text::add_line(state, contents, "mid_potential", text::variable_type::value, text::fp_currency{ info.mid_potential }, 15);
		text::add_line(state, contents, "rich_taxes", text::variable_type::value, text::fp_currency{ info.rich });
		text::add_line(state, contents, "rich_potential", text::variable_type::value, text::fp_currency{ info.rich_potential }, 15);
	}
};

class no_administration_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("administration_tile");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 18;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override { }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto n = state.world.province_get_nation_from_province_ownership(target.province);

		auto info = economy::explain_tax_income_local(state, n, target.province);

		text::add_line(state, contents, "tax_collection_rate", text::variable_type::value, text::fp_percentage{ info.local_multiplier });
		text::add_line_break_to_layout(state, contents);


		text::add_line(state, contents, "poor_taxes", text::variable_type::value, text::fp_currency{ info.poor });
		text::add_line(state, contents, "poor_potential", text::variable_type::value, text::fp_currency{ info.poor_potential }, 15);
		text::add_line(state, contents, "mid_taxes", text::variable_type::value, text::fp_currency{ info.mid });
		text::add_line(state, contents, "mid_potential", text::variable_type::value, text::fp_currency{ info.mid_potential }, 15);
		text::add_line(state, contents, "rich_taxes", text::variable_type::value, text::fp_currency{ info.rich });
		text::add_line(state, contents, "rich_potential", text::variable_type::value, text::fp_currency{ info.rich_potential }, 15);
	}
};

class market_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("market_tile");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 16;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		// send<province_subtab_toggle_signal>(state, parent, province_subtab_toggle_signal::economy);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto n = state.world.province_get_nation_from_province_ownership(target.province);

		text::add_line(state, contents, "trade_center");
		text::add_line(state, contents, "trade_center_desc");

		text::add_line_break_to_layout(state, contents);
		economy::make_trade_center_tooltip(state, contents, target.market);
		text::add_line_break_to_layout(state, contents);

		// TODO: Organize abstract "explain market labour demand" for the two
		// US3AC2
		auto external_trade_employment = economy::transportation_between_markets_labor_demand(state, target.market);
		// US3AC3
		// Since the tile is rendered only for state capitals, we assume that target.province = market capital
		auto internal_trade_employment = economy::transportation_inside_market_labor_demand(state, target.market, target.province);
		auto target_employment = external_trade_employment + internal_trade_employment;

		auto satisfaction = state.world.province_get_labor_demand_satisfaction(target.province, economy::labor::no_education);
		auto employment = target_employment * satisfaction;
		text::add_line(state, contents, "trade_center_employment", text::variable_type::value, text::fp_one_place{ employment });
		text::add_line(state, contents, labour_type_to_employment_type_text_key(economy::labor::no_education), 15);
		text::add_line(state, contents, "target_employment", text::variable_type::value, text::fp_one_place{ target_employment }, 15);
		text::add_line(state, contents, "employment_satisfaction", text::variable_type::value, text::fp_percentage{ satisfaction }, 15);

		auto wage = (state.world.province_get_labor_price(target.province, economy::labor::no_education) + 0.00001f);
		text::add_line(state, contents, "wage", text::variable_type::value, text::fp_one_place{ wage }, 15);
	}
};

} // namespace ui
