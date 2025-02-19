#include "gui_element_types.hpp"
#include "construction.hpp"

namespace ui {

struct open_factory_refit {
	dcon::factory_id factory_selection;
};

class factory_refit_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		const dcon::state_instance_id sid = retrieve<dcon::state_instance_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		auto type = state.world.factory_get_building_type(fid);

		// no double upgrade
		bool is_not_upgrading = true;
		for(auto p : state.world.state_instance_get_state_building_construction(sid)) {
			if(p.get_type() == type)
				is_not_upgrading = false;
		}
		if(!is_not_upgrading) {
			disabled = true; return;
		}

		bool is_civ = state.world.nation_get_is_civilized(state.local_player_nation);
		if(!is_civ) {
			disabled = true; return;
		}

		bool state_is_not_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));
		if(!state_is_not_colonial) {
			disabled = true; return;
		}

		auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
		if((rules & issue_rule::expand_factory) == 0) {
			disabled = true; return;
		}

		disabled = false;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		auto type = state.world.factory_get_building_type(fid);

		// no double upgrade
		bool is_not_upgrading = true;
		for(auto p : state.world.state_instance_get_state_building_construction(sid)) {
			if(p.get_type() == type)
				is_not_upgrading = false;
		}
		if(is_not_upgrading) {
			button_element_base::render(state, x, y);
		}
	}
	void button_action(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);

		send(state, parent, open_factory_refit{fid});
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}


	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		const dcon::state_instance_id sid = retrieve<dcon::state_instance_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		auto type = state.world.factory_get_building_type(fid);

		// no double upgrade
		bool is_not_upgrading = true;
		for(auto p : state.world.state_instance_get_state_building_construction(sid)) {
			if(p.get_type() == type)
				is_not_upgrading = false;
		}
		
		text::add_line(state, contents, "production_refit_factory_tooltip_1", text::variable_type::what, state.world.factory_type_get_name(type));

		text::add_line_break_to_layout(state, contents);

		bool is_civ = state.world.nation_get_is_civilized(state.local_player_nation);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_1", is_civ);

		bool state_is_not_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_2", state_is_not_colonial);

		bool is_activated = state.world.nation_get_active_building(n, type) == true || state.world.factory_type_get_is_available_from_start(type);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_3", is_activated);


		text::add_line_with_condition(state, contents, "factory_upgrade_condition_9", is_not_upgrading);
	}
};


class factory_refit_type_listbox_entry_label : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto refit_target_id = retrieve<dcon::factory_type_id>(state, parent);
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto sid = fat.get_factory_location().get_province().get_state_membership();

		if(refit_target_id) {
			set_button_text(state, text::produce_simple_string(state, state.world.factory_type_get_name(refit_target_id)));
		}

		disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, sid,
			fat.get_building_type().id, false, refit_target_id);

	}

	void button_action(sys::state& state) noexcept override {
		auto refit_target_id = retrieve<dcon::factory_type_id>(state, parent);
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto sid = fat.get_factory_location().get_province().get_state_membership();
		command::begin_factory_building_construction(state, state.local_player_nation, sid, fat.get_building_type().id, false, refit_target_id);
		// Close the window and reset selected factory
		send(state, parent, open_factory_refit{ dcon::factory_id{} });
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto refit_target = retrieve<dcon::factory_type_id>(state, parent);
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto sid = fat.get_factory_location().get_province().get_state_membership();
		auto n = fat.get_factory_location().get_province().get_nation_from_province_ownership();
		auto type = fat.get_building_type();

		// no double upgrade
		bool is_not_upgrading = true;
		for(auto p : state.world.state_instance_get_state_building_construction(sid)) {
			if(p.get_type() == type)
				is_not_upgrading = false;
		}

		text::add_line(state, contents, "production_refit_factory_tooltip_2", text::variable_type::what, state.world.factory_type_get_name(type),
			text::variable_type::name, state.world.factory_type_get_name(refit_target));

		text::add_line_break_to_layout(state, contents);

		auto refit_cost = economy::calculate_factory_refit_goods_cost(state, state.local_player_nation, fat.get_factory_location().get_province().get_state_membership(), type, refit_target);
		auto total = 0.f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(refit_cost.commodity_type[i] && refit_cost.commodity_amounts[i] > 0) {
				float admin_eff = state.world.nation_get_administrative_efficiency(n);
				float admin_cost_factor = 2.0f - admin_eff;
				float factory_mod = state.world.nation_get_modifier_values(n, sys::national_mod_offsets::factory_cost) + 1.0f;
				auto price = economy::price(state, sid, refit_cost.commodity_type[i]) * refit_cost.commodity_amounts[i] * factory_mod * admin_cost_factor;
				total += price;

				text::add_line(state, contents, "factory_refit_cost", text::variable_type::what, state.world.commodity_get_name(refit_cost.commodity_type[i]), text::variable_type::val, text::fp_three_places{ refit_cost.commodity_amounts[i] }, text::variable_type::value, text::format_money(price));
			}
		}

		text::add_line(state, contents, "factory_refit_cost_total", text::variable_type::value, text::format_money(total));
		
		text::add_line_break_to_layout(state, contents);

		bool is_civ = state.world.nation_get_is_civilized(state.local_player_nation);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_1", is_civ);

		bool state_is_not_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_2", state_is_not_colonial);

		bool is_activated = state.world.nation_get_active_building(n, refit_target) == true || state.world.factory_type_get_is_available_from_start(refit_target);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_3", is_activated);

		auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_8", (rules & issue_rule::expand_factory) != 0);

		text::add_line_with_condition(state, contents, "factory_upgrade_condition_9", is_not_upgrading);

		text::add_line_with_condition(state, contents, "production_refit_factory_tooltip_3", (rules & issue_rule::build_factory) != 0, 5);

		// For capitalist economies, refit factories must match in output good or inputs.
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

		text::add_line_with_condition(state, contents, "production_refit_factory_tooltip_4", output_1 == output_2 || inputs_match, 5);
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

		auto is_land = true;

		for(auto ftid : state.world.in_factory_type) {
			if(type == ftid) {
				continue;
			}
			row_contents.push_back(ftid);
		}

		update(state);
	}

};

class factory_refit_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "factory_refit_type_list") {
			return make_element_by_type<factory_refit_type_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};


}
