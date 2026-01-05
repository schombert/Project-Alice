#pragma once

#include "gui_element_types.hpp"
#include "gui_context_window.hpp"
#include "gui_province_window.hpp"
#include "economy_trade_routes.hpp"
#include "military.hpp"
#include "ai.hpp"
#include "labour_details.hpp"
#include "advanced_province_buildings.hpp"
#include "economy_tooltips.hpp"
#include "gui_tooltips.hpp"
#include "money.hpp"
#include "game_scene.hpp"
#include "economy_government.hpp"

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
		show_context_menu(state, { .province = dcon::province_id{ }, .factory = target.factory, .fconstruction = dcon::factory_construction_id{} });
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
		return state.world.commodity_get_name(target.commodity);
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override  {

		if(state.to_string_view(state.world.commodity_get_name(target.commodity)) == "fish") {
			return 11;
		}
		if(state.to_string_view(state.world.commodity_get_name(target.commodity)) == "timber") {
			return 4;
		}
		return (state.world.commodity_get_is_mine(target.commodity) ? 3 : 2);
	}

	dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) noexcept override  {
		return target.commodity;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		// Switch to economy scene on click
		game_scene::switch_scene(state, game_scene::scene_id::in_game_economy_viewer);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto commodity_name = state.world.commodity_get_name(target.commodity);
		text::add_line(state, contents, "rgo_tile_header", text::variable_type::good, commodity_name);
		text::add_line_break_to_layout(state, contents);

		province_owner_rgo_commodity_tooltip(state, contents, target.province, target.commodity);
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
			auto reinf = state.defines.pop_size_per_regiment * military::calculate_army_combined_reinforce<military::reinforcement_estimation_type::monthly>(state, a);
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

class province_port_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.lookup_key("civilian_port");
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 22;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		// Switch to economy scene on click
		game_scene::switch_scene(state, game_scene::scene_id::in_game_economy_viewer);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		text::add_line(state, contents, "civilian_port");
		text::add_line(state, contents, "civilian_port_size", text::variable_type::val, text::fp_one_place { state.world.province_get_advanced_province_building_max_private_size(target.province, advanced_province_buildings::list::civilian_ports) });
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

	int get_frame(sys::state& state, province_tile target) noexcept override {
		return 13;
	}

	dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) noexcept override  {
		return target.commodity;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto limit = state.world.province_get_factory_max_size(target.province, target.commodity) / 10000.f;
		text::add_line(state, contents, "available_potential", text::variable_type::what, state.world.commodity_get_name(target.commodity),
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
		show_context_menu(state, { .province = target.province, .factory = dcon::factory_id { }, .fconstruction = dcon::factory_construction_id{} });
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
		show_context_menu(state, { .province = dcon::province_id{ }, .factory = dcon::factory_id{}, .fconstruction = target.factory_construction });
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		economy::factory_construction_tooltip(state, contents, target.factory_construction);
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

		auto national_budget = state.world.nation_get_stockpiles(state.local_player_nation, economy::money);
		auto admin_budget_approx = budget_priority * national_budget;
		auto admin_count = economy::count_active_administrations(state, state.local_player_nation);
		auto budget_per_administration = admin_count == 0.f ? 0.f : admin_budget_approx / admin_count;

		text::add_line(state, contents, "local_admin_spending", text::variable_type::value, text::fp_currency{ economy::estimate_spendings_administration(state, n, budget_per_administration) });

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

		text::add_line(state, contents, "admin_production", text::variable_type::value, text::fp_one_place{ economy::local_administration_control_production(state, n, target.province) });
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

		auto national_budget = state.world.nation_get_stockpiles(state.local_player_nation, economy::money);
		auto admin_budget_approx = budget_priority * national_budget;
		auto admin_count = economy::count_active_administrations(state, state.local_player_nation);
		auto budget_per_administration = admin_count == 0.f ? 0.f : admin_budget_approx / admin_count;

		text::add_line(state, contents, "local_admin_spending", text::variable_type::value, text::fp_currency{ economy::estimate_spendings_administration_capital(state, n, budget_per_administration) });

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

		text::add_line(state, contents, "admin_production", text::variable_type::value, text::fp_one_place{ economy::capital_administration_control_production(state, n, target.province) });
	}
};

class tax_collector_tile : public tile_type_logic {
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

		text::add_line(state, contents, "province_control_scale", text::variable_type::value, text::fp_one_place{ state.world.province_get_control_scale(target.province) });

		// Explains nations::update_administrative_efficiency

		// Explain incoming control_scale
		auto control_buffer_asis = state.world.province_make_vectorizable_float_buffer();
		state.world.execute_serial_over_province([&](auto ids) {
			control_buffer_asis.set(ids, state.world.province_get_control_scale(ids));
		});
		auto control_buffer_tobe = state.world.province_make_vectorizable_float_buffer(); // To correctly calculate impact of earlier control transfer rules on later ones, write these changes in a temporary buffer
		state.world.execute_serial_over_province([&](auto ids) {
			control_buffer_tobe.set(ids, state.world.province_get_control_scale(ids));
		});
		auto coastal_capital_buffer = ve::vectorizable_buffer<dcon::province_id, dcon::state_instance_id>(state.world.state_instance_size());
		state.world.execute_parallel_over_state_instance([&](auto ids) {
			ve::apply([&](auto sid) {
				coastal_capital_buffer.set(sid, province::state_get_coastal_capital(state, sid));
			}, ids);
		});

		// propagate control better for state capitals via market connections
		state.world.for_each_trade_route([&](dcon::trade_route_id trid) {
			auto distance = state.world.trade_route_get_distance(trid);
			auto A_market = state.world.trade_route_get_connected_markets(trid, 0);
			auto B_market = state.world.trade_route_get_connected_markets(trid, 1);
			auto A_state_instance = state.world.market_get_zone_from_local_market(A_market);
			auto B_state_instance = state.world.market_get_zone_from_local_market(B_market);
			auto A_owner = state.world.state_instance_get_nation_from_state_ownership(A_state_instance);
			auto B_owner = state.world.state_instance_get_nation_from_state_ownership(B_state_instance);
			float propagation_multiplier = 0.01f;
			auto sphere_A = state.world.nation_get_in_sphere_of(A_owner);
			auto sphere_B = state.world.nation_get_in_sphere_of(B_owner);
			auto overlord_A = state.world.overlord_get_ruler(
				state.world.nation_get_overlord_as_subject(A_owner)
			);
			auto overlord_B = state.world.overlord_get_ruler(
				state.world.nation_get_overlord_as_subject(B_owner)
			);
			auto leader_A = (overlord_A) ? overlord_A : ((sphere_A) ? sphere_A : A_owner);
			auto leader_B = (overlord_B) ? overlord_B : ((sphere_B) ? sphere_B : B_owner);
			if(leader_A != leader_B) {
				return;
			}
			if(A_owner != B_owner) {
				propagation_multiplier /= 2.f;
			}
			// propagate control between ports
			if(state.world.trade_route_get_is_sea_route(trid)) {
				auto port_A = coastal_capital_buffer.get(A_state_instance);
				auto port_B = coastal_capital_buffer.get(B_state_instance);

				// check that ports are not occupied
				bool interrupted = false;
				if(
					state.world.province_get_nation_from_province_control(port_A)
					!=
					state.world.province_get_nation_from_province_ownership(port_A)
				) {
					interrupted = true;
				}
				if(
					state.world.province_get_nation_from_province_control(port_B)
					!=
					state.world.province_get_nation_from_province_ownership(port_B)
				) {
					interrupted = true;
				}

				if(!interrupted) {
					auto port_A_control = control_buffer_asis.get(port_A);
					auto port_B_control = control_buffer_asis.get(port_B);

					auto naval_base_level_A = military::state_naval_base_level(state, A_state_instance);
					auto naval_base_level_B = military::state_naval_base_level(state, B_state_instance);

					auto naval_base_multiplier = (1.f + naval_base_level_A * 2.f + naval_base_level_B * 2.f);

					auto naval_shift_of_control =
						(port_B_control - port_A_control)
						* std::min(0.1f, propagation_multiplier / (distance + 1.f) * naval_base_multiplier);
					control_buffer_tobe.set(port_A, control_buffer_tobe.get(port_A) + naval_shift_of_control);
					control_buffer_tobe.set(port_B, control_buffer_tobe.get(port_B) - naval_shift_of_control);

					if(port_A == target.province) {
						text::add_line(state, contents, "province_control_scale_trade_incoming", text::variable_type::value, text::fp_one_place{ naval_shift_of_control }, text::variable_type::prov, state.world.province_get_name(port_B));
					}
					else if(port_B == target.province) {
						text::add_line(state, contents, "province_control_scale_trade_outgoing", text::variable_type::value, text::fp_one_place{ naval_shift_of_control }, text::variable_type::prov, state.world.province_get_name(port_A));
					}
				}
			}
			// propagate along land trade routes
			{
				auto capital_A = state.world.state_instance_get_capital(A_state_instance);
				auto capital_B = state.world.state_instance_get_capital(B_state_instance);

				// check that capitals are not occupied
				bool interrupted = false;
				if(
					state.world.province_get_nation_from_province_control(capital_A)
					!=
					state.world.province_get_nation_from_province_ownership(capital_A)
				) {
					interrupted = true;
				}
				if(
					state.world.province_get_nation_from_province_control(capital_B)
					!=
					state.world.province_get_nation_from_province_ownership(capital_B)
				) {
					interrupted = true;
				}

				if(!interrupted) {
					auto capital_A_control = control_buffer_asis.get(capital_A);
					auto capital_B_control = control_buffer_asis.get(capital_B);

					auto land_shift_of_control =
						(capital_B_control - capital_A_control)
						* std::min(0.1f, propagation_multiplier / (distance + 1.f) / 2.f);
					control_buffer_tobe.set(capital_A, control_buffer_tobe.get(capital_A) + land_shift_of_control);
					control_buffer_tobe.set(capital_B, control_buffer_tobe.get(capital_B) - land_shift_of_control);

					if(capital_A == target.province) {
						text::add_line(state, contents, "province_control_scale_trade_incoming", text::variable_type::value, text::fp_one_place{ land_shift_of_control }, text::variable_type::prov, state.world.province_get_name(capital_B));
					} else if(capital_B == target.province) {
						text::add_line(state, contents, "province_control_scale_trade_outgoing", text::variable_type::value, text::fp_one_place{ land_shift_of_control }, text::variable_type::prov, state.world.province_get_name(capital_A));
					}
				}
			}
		});

		// propagate control inside states
		// sadly a lot of states have separate "islands" inside which require us to do it
		state.world.for_each_state_instance([&](auto sid) {
			auto capital = state.world.state_instance_get_capital(sid);
			province::for_each_province_in_state_instance(state, sid, [&](auto pid) {
				auto change = control_buffer_asis.get(capital) - control_buffer_asis.get(pid);
				control_buffer_tobe.set(capital, control_buffer_tobe.get(capital) - change * 0.01f);
				control_buffer_tobe.set(pid, control_buffer_tobe.get(pid) + change * 0.01f);

				if(pid == target.province) {
					text::add_line(state, contents, "province_control_scale_from_state_capital", text::variable_type::value, text::fp_one_place{ change }, text::variable_type::prov, state.world.province_get_name(capital));
				} else if(capital == target.province) {
					text::add_line(state, contents, "province_control_scale_to_state_capital", text::variable_type::value, text::fp_one_place{ change }, text::variable_type::prov, state.world.province_get_name(pid));
				}
			});
		});

		// reset buffer to avoid introduction of negative values
		state.world.execute_serial_over_province([&](auto ids) {
			control_buffer_asis.set(ids, state.world.province_get_control_scale(ids));
		});
		auto total_adjacency_weight = state.world.province_make_vectorizable_float_buffer();

		state.world.for_each_province_adjacency([&](auto paid) {
			auto A = state.world.province_adjacency_get_connected_provinces(paid, 0);
			auto B = state.world.province_adjacency_get_connected_provinces(paid, 1);
			auto mult = nations::control_shift_weight_mult(state, paid);
			auto weight_A = nations::desire_score_province(state, A) * mult;
			auto weight_B = nations::desire_score_province(state, B) * mult;
			auto old_A = total_adjacency_weight.get(A);
			auto old_B = total_adjacency_weight.get(B);
			total_adjacency_weight.set(B, old_B + weight_A);
			total_adjacency_weight.set(A, old_A + weight_B);
		});

		state.world.for_each_province([&](auto pid) {
			auto total_weight = total_adjacency_weight.get(pid) + 0.00001f;
			auto control_to_transfer = control_buffer_asis.get(pid) * 0.9f;
			state.world.province_for_each_province_adjacency(pid, [&](auto adj) {
				auto other = state.world.province_adjacency_get_connected_provinces(adj, 0);
				if(other == pid) {
					other = state.world.province_adjacency_get_connected_provinces(adj, 1);
				}
				auto score = nations::desire_score_province(state, other);
				auto mult = nations::control_shift_weight_mult(state, adj);
				control_buffer_tobe.set(other, control_buffer_tobe.get(other) + control_to_transfer * score * mult / total_weight);
				if(other == target.province) {
					text::add_line(state, contents, "province_control_scale_adjacency_received", text::variable_type::value, text::fp_one_place{ control_to_transfer * score * mult / total_weight }, text::variable_type::prov, state.world.province_get_name(pid));
				}
				else if(pid == target.province) {
					text::add_line(state, contents, "province_control_scale_adjacency_sent", text::variable_type::value, text::fp_one_place{ control_to_transfer * score * mult / total_weight }, text::variable_type::prov, state.world.province_get_name(other));
				}
			});
			control_buffer_tobe.set(pid, control_buffer_tobe.get(pid) - control_to_transfer);
		});

		// add friction to control expansion :
		auto pids = target.province; // For code simplification
		auto is_coastal = state.world.province_get_is_coast(pids);
		auto has_major_river = state.world.province_get_has_major_river(pids);

		auto current_control = state.world.province_get_control_ratio(pids);
		auto mass = nations::admin_cost_of_province(state, target.province);;
		auto prize = state.world.province_get_demographics(pids, demographics::total);
		// Higher population relative to admin cost = more desirable to control provinces
		auto desire = std::max(0.f, (prize / mass - 0.1f));

		auto control_scale = std::max(0.f, state.world.province_get_control_scale(pids)); // Bureaucratic capacity assigned to the province
		// as we expand control over local land, it requires much higher levels of administrative work to increase it
		auto available_control = std::min(control_scale * desire * 5.f, mass); // How much control can be established this tick capped at mass (can't exceed admin capacity needed)

		auto speed = (available_control / mass - current_control); // Difference between potential and current control. Control grows slowly at 1% per tick to avoid sudden drops in taxes

		text::add_line(state, contents, "province_control_change", text::variable_type::value, text::fp_one_place{ 0.01f * speed });

		// Control Scale Decay

		auto supply = std::max(
			0.f,
			state.world.province_get_modifier_values(pids, sys::provincial_mod_offsets::supply_limit) + 1.f
		); // Low supply increases decay (harder to maintain control)
		auto normal_multiplier = 1.f;
		auto reduced_multiplier = 0.2f;
		auto coast_multiplier = is_coastal ? reduced_multiplier : normal_multiplier; // Coastal reduces decay by 20%
		auto river_multiplier = has_major_river ? reduced_multiplier : normal_multiplier; // Rivers reduce decay by 20%
		auto movement = std::max(
			0.f,
			state.world.province_get_modifier_values(pids, sys::provincial_mod_offsets::movement_cost) + 1.f
		); // High movement cost increases decay
		auto attrition = std::max(
			0.f,
			state.world.province_get_modifier_values(pids, sys::provincial_mod_offsets::max_attrition) + 1.f
		); // High attrition increases decay
		auto decay = 0.001f / (1.f + supply) * (1.f + movement) * (1.f + attrition) * coast_multiplier * river_multiplier;

		text::add_line(state, contents, "province_control_decay", text::variable_type::value, text::fp_percentage{ decay });

		// Explain control_scale consumption
		text::add_line(state, contents, "admin_cost_of_province", text::variable_type::value, text::fp_one_place{ mass });
		text::add_line(state, contents, "admin_cost_of_province_area", text::variable_type::value, text::fp_one_place{ state.map_state.map_data.province_area_km2[province::to_map_id(target.province)] }, 15);
		text::add_line(state, contents, "admin_cost_of_province_population", text::variable_type::value, text::fp_one_place{ state.world.province_get_demographics(target.province, demographics::total) }, 15);
		if(is_coastal) {
			text::add_line(state, contents, "admin_cost_of_province_coastal", text::variable_type::value, text::fp_percentage{ 0.5f }, 30);
		}
		if(has_major_river) {
			text::add_line(state, contents, "admin_cost_of_province_river", text::variable_type::value, text::fp_percentage{ 0.5f }, 30);
		}

		text::add_line_break_to_layout(state, contents);

		// Explain tax collection coming from control_ratio
		auto info = economy::explain_tax_income_local(state, n, target.province);

		text::add_line(state, contents, "tax_collection_rate", text::variable_type::value, text::fp_percentage{ info.local_multiplier });
		auto from_control = state.world.province_get_control_ratio(target.province);
		auto efficiency = nations::tax_efficiency(state, n);
		// we can always collect at least some taxes in the capital:
		auto capital = state.world.nation_get_capital(n);

		if(target.province == capital && from_control < 0.1f) {
			text::add_line(state, contents, "tax_collection_rate_capital_base", text::variable_type::value, text::fp_percentage{ 0.1f }, 15);
		}
		else {
			text::add_line(state, contents, "tax_collection_rate_control", text::variable_type::value, text::fp_percentage{ from_control }, 15);
		}

		text::add_line(state, contents, "tax_collection_rate_efficiency", text::variable_type::value, text::fp_percentage{ 1.f + efficiency }, 15);

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

class commodity_tile : public tile_type_logic {
public:
	dcon::text_key get_name(sys::state& state, province_tile target) noexcept override {
		return state.world.commodity_get_name(target.commodity);
	}

	bool is_available(sys::state& state, province_tile target) noexcept override {
		return true;
	}

	int get_frame(sys::state& state, province_tile target) noexcept override {
		auto market = target.market;

		if(state.world.market_get_supply(market, target.commodity) < 0.01f) {
			return 0;
		}
		else if(state.world.market_get_supply(market, target.commodity) > state.world.market_get_demand(market, target.commodity) * 1.25f) {
			return 1;
		}
		else if(state.world.market_get_supply(market, target.commodity) < state.world.market_get_demand(market, target.commodity) * 0.75f) {
			return 3;
		}
		return 2;
	}

	dcon::commodity_id get_commodity_frame(sys::state& state, province_tile target) noexcept override {
		return target.commodity;
	}

	void button_action(sys::state& state, province_tile target, ui::element_base* parent) noexcept override {
		// Switch to economy scene on click
		game_scene::switch_scene(state, game_scene::scene_id::in_game_economy_viewer);
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, province_tile target) noexcept override {
		auto commodity_name = state.world.commodity_get_name(target.commodity);

		auto sid = state.world.province_get_state_membership(target.province);
		auto market = target.market;

		text::add_line(state, contents, "province_market_market", text::variable_type::name, sid.get_definition().get_name());
		text::add_line(state, contents, "province_market_header", text::variable_type::good, commodity_name);
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "province_market_price", text::variable_type::val, text::fp_currency{ state.world.market_get_price(market, target.commodity) });
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "province_market_supply", text::variable_type::val, text::fp_two_places{ state.world.market_get_supply(market, target.commodity) });
		text::add_line(state, contents, "province_market_demand", text::variable_type::val, text::fp_two_places{ state.world.market_get_demand(market, target.commodity) });
		text::add_line(state, contents, "province_market_production", text::variable_type::val, text::fp_two_places{ std::max(0.f, state.world.market_get_supply(market, target.commodity) - economy::trade_supply(state, market, target.commodity)) });
		text::add_line(state, contents, "province_market_consumption", text::variable_type::val, text::fp_two_places{ std::max(0.f, state.world.market_get_demand(market, target.commodity) - economy::trade_demand(state, market, target.commodity)) });
		text::add_line(state, contents, "province_market_stockpiles", text::variable_type::val, text::fp_two_places{ state.world.market_get_stockpile(market, target.commodity) });
		{
			auto supply = state.world.market_get_supply(market, target.commodity);
			auto demand = state.world.market_get_demand(market, target.commodity);
			auto shift = 0.001f;
			auto balance = (supply + shift) / (demand + shift) - (demand + shift) / (supply + shift);

			text::add_line(state, contents, "province_market_balance", text::variable_type::val, text::fp_two_places{ balance });
		}

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "province_market_trade_in", text::variable_type::val, text::fp_two_places{ economy::trade_influx(state, market, target.commodity) });
		text::add_line(state, contents, "province_market_trade_out", text::variable_type::val, text::fp_two_places{ economy::trade_outflux(state, market, target.commodity) });
		{
			auto supply = economy::trade_influx(state, market, target.commodity);
			auto demand = economy::trade_outflux(state, market, target.commodity);
			auto shift = 0.001f;
			auto balance = supply - demand;
			text::add_line(state, contents, "province_market_trade_balance", text::variable_type::val, text::fp_two_places{ balance });
		}
	}
};


} // namespace ui
