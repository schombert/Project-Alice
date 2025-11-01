#pragma once

#include "gui_element_types.hpp"
#include "gui_common_elements.hpp"
#include "construction.hpp"

namespace ui {

void hide_factory_refit_menu(sys::state& state);
void show_factory_refit_menu(sys::state& state, dcon::factory_id selected_factory);

class factory_refit_type_listbox_entry_label : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto refit_target_id = retrieve<dcon::factory_type_id>(state, parent);
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto pid = fat.get_factory_location().get_province();

		if(refit_target_id) {
			set_button_text(state, text::produce_simple_string(state, state.world.factory_type_get_name(refit_target_id)));
		}

		disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, pid, fat.get_building_type(), false, refit_target_id);

	}

	void button_action(sys::state& state) noexcept override {
		auto refit_target_id = retrieve<dcon::factory_type_id>(state, parent);
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto pid = fat.get_factory_location().get_province();
		command::begin_factory_building_construction(state, state.local_player_nation, pid, fat.get_building_type().id, false, refit_target_id);
		// Close the window
		hide_factory_refit_menu(state);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto refit_target = retrieve<dcon::factory_type_id>(state, parent);
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto pid = fat.get_factory_location().get_province();
		auto n = fat.get_factory_location().get_province().get_nation_from_province_ownership();
		auto type = fat.get_building_type();

		// no double upgrade
		bool is_not_upgrading = true;
		for(auto p : state.world.province_get_factory_construction(pid)) {
			if(p.get_type() == type)
				is_not_upgrading = false;
		}

		text::add_line(state, contents, "production_refit_factory_tooltip_2", text::variable_type::what, state.world.factory_type_get_name(type),
			text::variable_type::name, state.world.factory_type_get_name(refit_target));

		text::add_line_break_to_layout(state, contents);

		auto refit_cost = economy::calculate_factory_refit_goods_cost(state, state.local_player_nation, fat.get_factory_location().get_province(), type, refit_target);
		auto total = 0.f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(refit_cost.commodity_type[i] && refit_cost.commodity_amounts[i] > 0) {
				float factor = economy::factory_build_cost_multiplier(state, n, fat.get_province_from_factory_location(), false);
				auto price =
					economy::price(state, pid.get_state_membership(), refit_cost.commodity_type[i])
					* refit_cost.commodity_amounts[i]
					* factor;
				total += price;

				text::add_line(state, contents, "factory_refit_cost", text::variable_type::what, state.world.commodity_get_name(refit_cost.commodity_type[i]), text::variable_type::val, text::fp_three_places{ refit_cost.commodity_amounts[i] }, text::variable_type::value, text::format_money(price));
			}
		}

		text::add_line(state, contents, "factory_refit_cost_total", text::variable_type::value, text::format_money(total));
		
		text::add_line_break_to_layout(state, contents);

		bool is_civ = state.world.nation_get_is_civilized(state.local_player_nation);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_1", is_civ);

		bool state_is_not_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(pid.get_state_membership()));
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_2", state_is_not_colonial);

		bool is_activated = state.world.nation_get_active_building(n, refit_target) || state.world.factory_type_get_is_available_from_start(refit_target);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_3", is_activated);

		text::add_line_with_condition(state, contents, "factory_upgrade_condition_9", is_not_upgrading);

		/*
		OR:
		- Ruling party allows building factories
		- Old and new types match in inputs or outputs
		*/
		{
			auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
			auto cond_1 = (rules & issue_rule::build_factory) != 0;

			/* For capitalist economies, refit factories must match in output good or inputs.
			In vanilla, for example, paper mill into lumber mill.
			Primarily useful for mods where are several factory options for one production chain.
			*/
			auto output_1 = state.world.factory_type_get_output(type);
			auto output_2 = state.world.factory_type_get_output(refit_target);
			auto inputs_1 = state.world.factory_type_get_inputs(type);
			auto inputs_2 = state.world.factory_type_get_inputs(refit_target);
			auto inputs_match = true;

			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
				auto input_1 = inputs_1.commodity_type[i];
				auto input_2 = inputs_2.commodity_type[i];

				if(input_1 != input_2) {
					inputs_match = false;
					break;
				}
			}

			auto cond_2 = output_1 == output_2 || inputs_match;
			text::add_line_with_condition(state, contents, "OR", cond_1 || cond_2);
			text::add_line_with_condition(state, contents, "production_refit_factory_tooltip_3", cond_1, 15);
			text::add_line_with_condition(state, contents, "production_refit_factory_tooltip_4", cond_2, 15);
		}
	}
};

class factory_refit_type_listbox_entry : public listbox_row_element_base<dcon::factory_type_id> {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "factory_type_icon") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "factory_type_select") {
			return make_element_by_type<factory_refit_type_listbox_entry_label>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			auto output = state.world.factory_type_get_output(content);
			payload.emplace<dcon::commodity_id>(output);
			return message_result::consumed;
		}

		return listbox_row_element_base<dcon::factory_type_id>::get(state, payload);
	}
};

class factory_refit_type_listbox : public listbox_element_base<factory_refit_type_listbox_entry, dcon::factory_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "factory_refit_type_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		auto type = state.world.factory_get_building_type(fid);
		auto pid = state.world.factory_get_province_from_factory_location(fid);

		std::vector<dcon::factory_type_id> types;
		for(auto t : state.world.in_factory_type) {
			types.push_back(t);
		}

		std::sort(types.begin(), types.end(), [&](auto a, auto b) {
			// Constructable first
			if(command::can_begin_factory_building_construction(state, state.local_player_nation, pid, type, false, a) != command::can_begin_factory_building_construction(state, state.local_player_nation, pid, type, false, b)) {
				return (int)command::can_begin_factory_building_construction(state, state.local_player_nation, pid, type, false, a) > (int)command::can_begin_factory_building_construction(state, state.local_player_nation, pid, type, false, b);
			}
			// Then sort by commodity
			if(state.world.factory_type_get_output(a).id.index() != state.world.factory_type_get_output(b).id.index()) {
				// First show types with the same output
				if(state.world.factory_type_get_output(type) == state.world.factory_type_get_output(a) && state.world.factory_type_get_output(type) != state.world.factory_type_get_output(b)) {
					return false;
				}
				if(state.world.factory_type_get_output(type) == state.world.factory_type_get_output(b) && state.world.factory_type_get_output(type) != state.world.factory_type_get_output(a)) {
					return true;
				}
				return state.world.factory_type_get_output(a).id.index() < state.world.factory_type_get_output(b).id.index();
			}
			// Then sort by tier
			if(state.world.factory_type_get_factory_tier(a) != state.world.factory_type_get_factory_tier(b)) {
				return state.world.factory_type_get_factory_tier(a) < state.world.factory_type_get_factory_tier(b);
			}
			return a.value > b.value;
		});

		for(auto t : types) {
			row_contents.push_back(t);
		}

		update(state);
	}
};

class factory_refit_window : public window_element_base {
public:
	dcon::factory_id selected_factory;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "factory_refit_type_list") {
			return make_element_by_type<factory_refit_type_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::factory_id>()) {
			payload.emplace<dcon::factory_id>(selected_factory);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

bool factory_refit_button_active(sys::state& state, dcon::factory_id fid, dcon::province_id pid, dcon::state_instance_id sid, dcon::nation_id n);
void factory_refit_button_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, dcon::factory_id fid);

class factory_refit_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::province_id pid = retrieve<dcon::province_id>(state, parent);
		const dcon::state_instance_id sid = retrieve<dcon::state_instance_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		disabled = !factory_refit_button_active(state, fid, pid, sid, n);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::province_id pid = retrieve<dcon::province_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto type = state.world.factory_get_building_type(fid);

		// no double upgrade
		bool is_not_upgrading = true;
		for(auto p : state.world.province_get_factory_construction(pid)) {
			if(p.get_type() == type)
				is_not_upgrading = false;
		}
		if(is_not_upgrading) {
			button_element_base::render(state, x, y);
		}
	}
	void button_action(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		if(state.ui_state.factory_refit_win && state.ui_state.factory_refit_win->is_visible()) {
			hide_factory_refit_menu(state);
		} else {
			show_factory_refit_menu(state, fid);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}


	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		factory_refit_button_tooltip(state, x, y, contents, fid);
	}
};

}
