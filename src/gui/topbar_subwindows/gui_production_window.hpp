#pragma once

#include "gui_element_types.hpp"
#include "gui_factory_buttons_window.hpp"
#include "gui_invest_brow_window.hpp"
#include "gui_pop_sort_buttons_window.hpp"
#include "gui_commodity_filters_window.hpp"
#include "gui_projects_window.hpp"
#include "gui_build_factory_window.hpp"
#include "gui_project_investment_window.hpp"
#include "gui_foreign_investment_window.hpp"
#include "economy_templates.hpp"
#include "province_templates.hpp"

namespace ui {

struct production_selection_wrapper {
	dcon::state_instance_id data{};
	bool is_build = false;
	xy_pair focus_pos{0, 0};
};

class factory_employment_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::factory_id>(state, parent);
		frame = int32_t(state.world.factory_get_primary_employment(content) * 10.f);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);

		auto max_emp = economy::factory_max_employment(state, fid);
		{
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(state.culture_definitions.primary_factory_worker));
			text::add_to_layout_box(state, contents, box, std::string_view{": " });
			text::add_to_layout_box(state, contents, box, int64_t(std::ceil(state.world.factory_get_primary_employment(fid) * max_emp * state.economy_definitions.craftsmen_fraction)));
			text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
			text::add_to_layout_box(state, contents, box, int64_t(std::ceil(max_emp * state.economy_definitions.craftsmen_fraction)));
			text::close_layout_box(contents, box);
		}
		{
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(state.culture_definitions.secondary_factory_worker));
			text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
			text::add_to_layout_box(state, contents, box, int64_t(std::ceil(state.world.factory_get_secondary_employment(fid) * max_emp * (1.0f -state.economy_definitions.craftsmen_fraction))));
			text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
			text::add_to_layout_box(state, contents, box, int64_t(std::ceil(max_emp * (1.0f - state.economy_definitions.craftsmen_fraction))));
			text::close_layout_box(contents, box);
		}
	}
};

class factory_priority_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		frame = economy::factory_priority(state, fid);
		auto rules = state.world.nation_get_combined_issue_rules(n);
		disabled = (rules & issue_rule::factory_priority) == 0 || n != state.local_player_nation;
	}

	void button_action(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		switch(economy::factory_priority(state, fid)) {
		case 0:
			command::change_factory_settings(state, state.local_player_nation, fid, 1, fat.get_subsidized());
			break;
		case 1:
			command::change_factory_settings(state, state.local_player_nation, fid, 2, fat.get_subsidized());
			break;
		case 2:
			command::change_factory_settings(state, state.local_player_nation, fid, 3, fat.get_subsidized());
			break;
		case 3:
			command::change_factory_settings(state, state.local_player_nation, fid, 0, fat.get_subsidized());
			break;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		if(n != state.local_player_nation)
			return;

		if(disabled) {
			text::add_line(state, contents, "production_not_allowed_to_change_prio_tooltip");
			text::add_line(state, contents, "cant_prioritize_explanation");
		} else {
			text::add_line(state, contents, "production_allowed_to_change_prio_tooltip");

			switch(economy::factory_priority(state, fid)) {
			case 0:
				text::add_line(state, contents, "diplomacy_prio_none");
				break;
			case 1:
				text::add_line(state, contents, "diplomacy_prio_low");
				break;
			case 2:
				text::add_line(state, contents, "diplomacy_prio_middle");
				break;
			case 3:
				text::add_line(state, contents, "diplomacy_prio_high");
				break;
			}
		}
		text::add_line_break_to_layout(state, contents); // TODO: Classic needs this as a divider
		text::add_line(state, contents, "production_prio_factory_desc_tooltip");
	}
};

class factory_upgrade_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);

		disabled = !command::can_begin_factory_building_construction(state, state.local_player_nation, sid,
			fat.get_building_type().id, true);

	}

	void button_right_action(sys::state& state) noexcept {

	}

	void button_shift_action(sys::state& state) noexcept {
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		for(auto p : state.world.state_definition_get_abstract_state_membership_as_state(state.world.state_instance_get_definition(sid))) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(command::can_begin_factory_building_construction(state, state.local_player_nation,
					p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true)) {
					command::begin_factory_building_construction(state, state.local_player_nation,
						p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true);
				}
			}
		}
	}

	void button_shift_right_action(sys::state& state) noexcept {
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		for(auto p : state.world.state_definition_get_abstract_state_membership_as_state(state.world.state_instance_get_definition(sid))) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(fac.get_factory().get_primary_employment() >= 0.95f && fac.get_factory().get_production_scale() > 0.8f) {
					if(command::can_begin_factory_building_construction(state, state.local_player_nation,
						p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true)) {
						command::begin_factory_building_construction(state, state.local_player_nation,
							p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true);
					}
				}
			}
		}
	}

	void button_ctrl_action(sys::state& state) noexcept {
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		for(auto p : state.world.nation_get_province_ownership(n)) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(command::can_begin_factory_building_construction(state, state.local_player_nation,
					p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true)) {
					command::begin_factory_building_construction(state, state.local_player_nation,
						p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true);
				}
			}
		}
	}

	void button_ctrl_right_action(sys::state& state) noexcept {
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		for(auto p : state.world.nation_get_province_ownership(n)) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(fac.get_factory().get_primary_employment() >= 0.95f && fac.get_factory().get_production_scale() > 0.8f) {
					if(command::can_begin_factory_building_construction(state, state.local_player_nation,
						p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true)) {
						command::begin_factory_building_construction(state, state.local_player_nation,
							p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true);
					}
				}
			}
		}
	}

	void button_ctrl_shift_action(sys::state& state) noexcept {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		for(auto p : state.world.nation_get_province_ownership(n)) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(fac.get_factory().get_building_type() == state.world.factory_get_building_type(fid)
				&& command::can_begin_factory_building_construction(state, state.local_player_nation,
					p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true)) {
					command::begin_factory_building_construction(state, state.local_player_nation,
						p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true);
				}
			}
		}
	}

	void button_ctrl_shift_right_action(sys::state& state) noexcept {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		for(auto p : state.world.nation_get_province_ownership(n)) {
			for(auto fac : p.get_province().get_factory_location()) {
				if(fac.get_factory().get_building_type() == state.world.factory_get_building_type(fid)
				&& fac.get_factory().get_primary_employment() >= 0.95f && fac.get_factory().get_production_scale() > 0.8f) {
					if(command::can_begin_factory_building_construction(state, state.local_player_nation,
						p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true)) {
						command::begin_factory_building_construction(state, state.local_player_nation,
							p.get_province().get_state_membership(), fac.get_factory().get_building_type(), true);
					}
				}
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		command::begin_factory_building_construction(state, state.local_player_nation, sid, fat.get_building_type().id, true);
	}

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(!disabled) {
				//ToDo: Make button change appearance while pressed
				//disabled = true;
			}
		} else if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else
				button_action(state);
		}
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if(!disabled) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_right_action(state);
			else if(mods == sys::key_modifiers::modifiers_ctrl)
				button_ctrl_right_action(state);
			else
				button_right_action(state);
		}
		return message_result::consumed;
	}
	message_result on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept override {
		if(state.user_settings.left_mouse_click_hold_and_release) {
			if(under_mouse) {
				sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
				if(mods == sys::key_modifiers::modifiers_shift)
					button_shift_action(state);
				else if(mods == sys::key_modifiers::modifiers_ctrl)
					button_ctrl_action(state);
				else
					button_action(state);
			} else {
				//ToDo: Make button revert appearance when released
				//disabled = false;
			}
		}
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		if(!disabled && base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			sound::play_interface_sound(state, sound::get_click_sound(state),
					state.user_settings.interface_volume * state.user_settings.master_volume);
			if(mods == sys::key_modifiers::modifiers_shift)
				button_shift_action(state);
			else
				button_action(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
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
		if(!is_not_upgrading) {
			return;
		}

		text::add_line(state, contents, "production_expand_factory_tooltip");

		text::add_line_break_to_layout(state, contents);

		bool is_civ = state.world.nation_get_is_civilized(state.local_player_nation);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_1", is_civ);

		bool state_is_not_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_2", state_is_not_colonial);

		bool is_activated = state.world.nation_get_active_building(n, type) == true || state.world.factory_type_get_is_available_from_start(type);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_3", is_activated);
		if(n != state.local_player_nation) {
			bool gp_condition = (state.world.nation_get_is_great_power(state.local_player_nation) == true &&
					state.world.nation_get_is_great_power(n) == false);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_4", gp_condition);

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_5", state.world.nation_get_is_civilized(n));

			auto rules = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_6",
					(rules & issue_rule::allow_foreign_investment) != 0);

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_7",
					!military::are_at_war(state, state.local_player_nation, n));
		} else {
			auto rules = state.world.nation_get_combined_issue_rules(state.local_player_nation);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_8", (rules & issue_rule::expand_factory) != 0);
		}
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_9", is_not_upgrading);
		text::add_line_with_condition(state, contents, "factory_upgrade_condition_10", fat.get_level() < 255);
		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "alice_expand_factory_controls_1");
		text::add_line(state, contents, "alice_expand_factory_controls_2");
		text::add_line(state, contents, "alice_expand_factory_controls_3");
		text::add_line(state, contents, "alice_expand_factory_controls_4");
		text::add_line(state, contents, "alice_expand_factory_controls_5");
		text::add_line(state, contents, "alice_expand_factory_controls_6");
	}
};

class factory_reopen_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		disabled = !command::can_change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), true);
	}

	void button_action(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		command::change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), true);
	}

	void on_create(sys::state& state) noexcept override {
		frame = 0;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		if(n == state.local_player_nation) {
			text::add_line(state, contents, "open_and_sub");

			if(disabled) {
				text::add_line(state, contents, "production_not_allowed_to_subsidise_tooltip");
				text::add_line(state, contents, "cant_subsidize_explanation");
			}
		}
	}
};

class factory_subsidise_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		auto rules = state.world.nation_get_combined_issue_rules(n);
		disabled = (rules & issue_rule::can_subsidise) == 0 || state.local_player_nation != n;
		frame = state.world.factory_get_subsidized(fid) ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		auto fat = dcon::fatten(state.world, fid);
		if(fat.get_subsidized()) {
			if(command::can_change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), false)) {
				command::change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)),
						false);
			}
		} else {
			if(command::can_change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), true)) {
				command::change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), true);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		if(n == state.local_player_nation) {
			if(dcon::fatten(state.world, fid).get_subsidized()) {
				text::add_line(state, contents, "production_cancel_subsidies");
			} else {
				if(disabled) {
					text::add_line(state, contents, "production_not_allowed_to_subsidise_tooltip");
					text::add_line(state, contents, "cant_subsidize_explanation");
				} else {
					text::add_line(state, contents, "production_allowed_to_subsidise_tooltip");
				}
			}
			text::add_line_break_to_layout(state, contents);
			text::add_line(state, contents, "production_subsidies_desc");
		}
	}
};

class factory_delete_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		disabled = !command::can_delete_factory(state, state.local_player_nation, fid);
	}

	void button_action(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		command::delete_factory(state, state.local_player_nation, fid);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		if(n == state.local_player_nation) {
			text::add_line(state, contents, "factory_delete_header");
			if(disabled) {
				text::add_line_break_to_layout(state, contents);
				text::add_line(state, contents, "factory_delete_not_allowed");
			}
		}
	}
};

class factory_close_and_delete_button : public button_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		visible = dcon::fatten(state.world, fid).get_production_scale() >= 0.05f;
		disabled = !command::can_delete_factory(state, state.local_player_nation, fid);
		frame = 1;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		if(visible)
			return button_element_base::test_mouse(state, x, y, type);
		return message_result::unseen;
	}
	void button_action(sys::state& state) noexcept override {
		const dcon::factory_id fid = retrieve<dcon::factory_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		command::delete_factory(state, state.local_player_nation, fid);
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		if(n == state.local_player_nation) {
			text::add_line(state, contents, "close_and_del");
			if(disabled) {
				text::add_line_break_to_layout(state, contents);
				text::add_line(state, contents, "factory_delete_not_allowed");
			}
		}
	}
};

struct state_factory_slot {
	dcon::factory_id id;
	std::variant<std::monostate, economy::upgraded_factory, economy::new_factory> activity;
};

struct production_factory_slot_data : public state_factory_slot {
	size_t index = 0;
};

class factory_build_progress_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		progress = retrieve<economy::new_factory>(state, parent).progress;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nf = retrieve< economy::new_factory>(state, parent);
		auto si = retrieve<dcon::state_instance_id>(state, parent);
		if(!nf.type)
			return;
		for(auto p : state.world.state_instance_get_state_building_construction(si)) {
			if(p.get_type() == nf.type) {
				float admin_eff = state.world.nation_get_administrative_efficiency(p.get_nation());
				float factory_mod = state.world.nation_get_modifier_values(p.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
				float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(p.get_nation(), sys::national_mod_offsets::factory_owner_cost));
				float admin_cost_factor = (p.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;

				auto& goods = state.world.factory_type_get_construction_costs(nf.type);
				auto& cgoods = p.get_purchased_goods();

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(goods.commodity_type[i]) {
						auto box = text::open_layout_box(contents, 0);
						text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(goods.commodity_type[i]));
						text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
						text::add_to_layout_box(state, contents, box, text::fp_one_place{ cgoods.commodity_amounts[i] });
						text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
						text::add_to_layout_box(state, contents, box, text::fp_one_place{ goods.commodity_amounts[i] * admin_cost_factor });
						text::close_layout_box(contents, box);
					}
				}
				return;
			}
		}
	}
};

class factory_upgrade_progress_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		progress = retrieve<economy::upgraded_factory>(state, parent).progress;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nf = retrieve<economy::upgraded_factory>(state, parent);
		auto si = retrieve<dcon::state_instance_id>(state, parent);
		if(!nf.type)
			return;
		for(auto p : state.world.state_instance_get_state_building_construction(si)) {
			if(p.get_type() == nf.type) {
				float admin_eff = state.world.nation_get_administrative_efficiency(p.get_nation());
				float factory_mod = state.world.nation_get_modifier_values(p.get_nation(), sys::national_mod_offsets::factory_cost) + 1.0f;
				float pop_factory_mod = std::max(0.1f, state.world.nation_get_modifier_values(p.get_nation(), sys::national_mod_offsets::factory_owner_cost));
				float admin_cost_factor = (p.get_is_pop_project() ? pop_factory_mod : (2.0f - admin_eff)) * factory_mod;

				auto& goods = state.world.factory_type_get_construction_costs(nf.type);
				auto& cgoods = p.get_purchased_goods();

				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					if(goods.commodity_type[i]) {
						auto box = text::open_layout_box(contents, 0);
						text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(goods.commodity_type[i]));
						text::add_to_layout_box(state, contents, box, std::string_view{ ": " });
						text::add_to_layout_box(state, contents, box, text::fp_one_place{ cgoods.commodity_amounts[i] });
						text::add_to_layout_box(state, contents, box, std::string_view{ " / " });
						text::add_to_layout_box(state, contents, box, text::fp_one_place{ goods.commodity_amounts[i] * admin_cost_factor });
						text::close_layout_box(contents, box);
					}
				}
				return;
			}
		}
	}
};

class normal_factory_background : public opaque_element_base {
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto fid = retrieve<dcon::factory_id>(state, parent);
		if(!fid)
			return;
		dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		auto type = state.world.factory_get_building_type(fid);

		auto& inputs = type.get_inputs();
		auto& einputs = type.get_efficiency_inputs();

		float min_input = 1.0f;
		float min_efficiency_input = 1.0f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(inputs.commodity_type[i]) {
				min_input = std::min(min_input, state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[i]));
			} else {
				break;
			}
		}

		// and for efficiency inputs
		for(uint32_t i = 0; i < economy::small_commodity_set::set_size; ++i) {
			if(einputs.commodity_type[i]) {
				min_efficiency_input = std::min(min_efficiency_input, state.world.nation_get_demand_satisfaction(n, einputs.commodity_type[i]));
			} else {
				break;
			}
		}

		auto amount = (0.75f + 0.25f * min_efficiency_input) * min_input * state.world.factory_get_production_scale(fid);

		text::add_line(state, contents, "factory_stats_1", text::variable_type::val, text::fp_percentage{amount});

		text::add_line(state, contents, "factory_stats_2", text::variable_type::val,
				text::fp_percentage{state.world.factory_get_production_scale(fid)});

		text::add_line(state, contents, "factory_stats_3", text::variable_type::val,
				text::fp_one_place{state.world.factory_get_actual_production(fid) }, text::variable_type::x, type.get_output().get_name());

		text::add_line(state, contents, "factory_stats_4", text::variable_type::val,
				text::fp_currency{state.world.factory_get_full_profit(fid)});

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_stats_5");


		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(inputs.commodity_type[i]) {
				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(inputs.commodity_type[i]));
				text::add_to_layout_box(state, contents, box, std::string_view{": "});
				auto sat = state.world.nation_get_demand_satisfaction(n, inputs.commodity_type[i]);
				text::add_to_layout_box(state, contents, box, text::fp_percentage{sat},
						sat >= 0.9f ? text::text_color::green : text::text_color::red);
				text::close_layout_box(contents, box);
			} else {
				break;
			}
		}

		text::add_line_break_to_layout(state, contents);

		text::add_line(state, contents, "factory_stats_6");

		for(uint32_t i = 0; i < economy::small_commodity_set::set_size; ++i) {
			if(einputs.commodity_type[i]) {
				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, state.world.commodity_get_name(einputs.commodity_type[i]));
				text::add_to_layout_box(state, contents, box, std::string_view{": "});
				auto sat = state.world.nation_get_demand_satisfaction(n, einputs.commodity_type[i]);
				text::add_to_layout_box(state, contents, box, text::fp_percentage{sat},
						sat >= 0.9f ? text::text_color::green : text::text_color::red);
				text::close_layout_box(contents, box);
			} else {
				break;
			}
		}
	}
};

class factory_input_icon : public image_element_base {
public:
	dcon::commodity_id com{};

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return com ? tooltip_behavior::variable_tooltip : tooltip_behavior::no_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		auto p = state.world.factory_get_province_from_factory_location(retrieve<dcon::factory_id>(state, parent));
		//auto com = retrieve<dcon::commodity_id>(state, parent);
		if(!com)
			return;

		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_name(com)), text::text_color::yellow);
		text::close_layout_box(contents, box);

		auto commodity_mod_description = [&](float value, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
			if(value == 0.f)
				return;
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_name(com)), text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(com) ? locale_base_name : locale_farm_base_name), text::text_color::white);
			text::add_to_layout_box(state, contents, box, std::string{ ":" }, text::text_color::white);
			text::add_space_to_layout_box(state, contents, box);
			auto color = value > 0.f ? text::text_color::green : text::text_color::red;
			text::add_to_layout_box(state, contents, box, (value > 0.f ? "+" : "") + text::format_percentage(value, 1), color);
			text::close_layout_box(contents, box);
		};
		commodity_mod_description(state.world.nation_get_factory_goods_output(n, com), "tech_output", "tech_output");
		commodity_mod_description(state.world.nation_get_rgo_goods_output(n, com), "tech_mine_output", "tech_farm_output");
		commodity_mod_description(state.world.nation_get_rgo_size(n, com), "tech_mine_size", "tech_farm_size");
		if(state.world.commodity_get_key_factory(com)) {
			active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::factory_output, true);
			active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::local_factory_output, true);
			active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::factory_throughput, true);
			active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::local_factory_throughput, true);
		} else {
			if(state.world.commodity_get_is_mine(com)) {
				active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mine_rgo_eff, true);
				active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::mine_rgo_eff, true);
				active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::mine_rgo_size, true);
				active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::mine_rgo_size, true);
			} else {
				active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::farm_rgo_eff, true);
				active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::farm_rgo_eff, true);
				active_modifiers_description(state, contents, n, 0, sys::national_mod_offsets::farm_rgo_size, true);
				active_modifiers_description(state, contents, p, 0, sys::provincial_mod_offsets::farm_rgo_size, true);
			}
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(com)
			image_element_base::render(state, x, y);
	}
};

class factory_cancel_new_const_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto v = retrieve<economy::new_factory>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		disabled = !command::can_cancel_factory_building_construction(state, state.local_player_nation, sid, v.type);
	}
	void button_action(sys::state& state) noexcept override {
		auto v = retrieve<economy::new_factory>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		command::cancel_factory_building_construction(state, state.local_player_nation, sid, v.type);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "cancel_fac_construction");
	}
};
class factory_cancel_upgrade_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto v = retrieve<economy::upgraded_factory>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		disabled = !command::can_cancel_factory_building_construction(state, state.local_player_nation, sid, v.type);
	}
	void button_action(sys::state& state) noexcept override {
		auto v = retrieve<economy::upgraded_factory>(state, parent);
		auto sid = retrieve<dcon::state_instance_id>(state, parent);
		command::cancel_factory_building_construction(state, state.local_player_nation, sid, v.type);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "cancel_fac_upgrade");
	}
};

class production_factory_info : public window_element_base {
	factory_input_icon* input_icons[economy::commodity_set::set_size] = {nullptr};
	image_element_base* input_lack_icons[economy::commodity_set::set_size] = {nullptr};
	std::vector<element_base*> factory_elements;
	std::vector<element_base*> upgrade_elements;
	std::vector<element_base*> build_elements;
	std::vector<element_base*> closed_elements;
	dcon::commodity_id output_commodity;

public:
	uint8_t index = 0; // from 0 to int32_t(state.defines.factories_per_state)

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "prod_factory_bg") {
			return make_element_by_type<normal_factory_background>(state, id);
		} else if(name == "level") {
			auto ptr = make_element_by_type<factory_level_text>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "income") {
			auto ptr = make_element_by_type<factory_profit_text>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "income_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "output") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "closed_overlay") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			closed_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "factory_closed_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			closed_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "prod_factory_inprogress_bg") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			build_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "build_factory_progress") {
			auto ptr = make_element_by_type<factory_build_progress_bar>(state, id);
			build_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "prod_cancel_progress") {
			auto ptr = make_element_by_type<factory_cancel_new_const_button>(state, id);
			build_elements.push_back(ptr.get());

			/* // Where should this button go ?
			auto ptrb = make_element_by_type<factory_cancel_upgrade_button>(state, id);
			upgrade_elements.push_back(ptrb.get());
			add_child_to_front(std::move(ptrb));
			*/

			return ptr;
		} else if(name == "upgrade_factory_progress") {
			auto ptr = make_element_by_type<factory_upgrade_progress_bar>(state, id);
			upgrade_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "progress_overlay_16_64") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			upgrade_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "employment_ratio") {
			auto ptr = make_element_by_type<factory_employment_image>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "priority") {
			auto ptr = make_element_by_type<factory_priority_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "upgrade") {
			auto ptr = make_element_by_type<factory_upgrade_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "subsidise") {
			auto ptr = make_element_by_type<factory_subsidise_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "delete_factory") {
			auto ptr = make_element_by_type<factory_delete_button>(state, id);
			closed_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "open_close") {
			auto ptr = make_element_by_type<factory_reopen_button>(state, id);
			closed_elements.push_back(ptr.get());

			auto ptrb = make_element_by_type<factory_close_and_delete_button>(state, id);
			factory_elements.push_back(ptrb.get());
			add_child_to_front(std::move(ptrb));

			return ptr;
		} else if(name.substr(0, 6) == "input_") {
			auto input_index = size_t(std::stoi(std::string(name.substr(6))));
			if(name.ends_with("_lack2")) {
				auto ptr = make_element_by_type<image_element_base>(state, id);
				input_lack_icons[input_index] = ptr.get();
				return ptr;
			} else {
				auto ptr = make_element_by_type<factory_input_icon>(state, id);
				input_icons[input_index] = static_cast<factory_input_icon*>(ptr.get());
				return ptr;
			}
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = production_factory_slot_data{{dcon::factory_id{}, std::monostate{}}, index};
			parent->impl_get(state, payload);
			auto content = any_cast<production_factory_slot_data>(payload);

			const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
			dcon::factory_type_fat_id fat_btid(state.world, dcon::factory_type_id{});
			if(std::holds_alternative<economy::new_factory>(content.activity)) {
				// New factory
				economy::new_factory nf = std::get<economy::new_factory>(content.activity);
				fat_btid = dcon::fatten(state.world, nf.type);

				for(auto const& e : factory_elements)
					e->set_visible(state, false);
				for(auto const& e : upgrade_elements)
					e->set_visible(state, false);
				for(auto const& e : build_elements)
					e->set_visible(state, true);
				for(auto const& e : closed_elements)
					e->set_visible(state, false);
			} else if(std::holds_alternative<economy::upgraded_factory>(content.activity)) {
				// Upgrade
				economy::upgraded_factory uf = std::get<economy::upgraded_factory>(content.activity);
				fat_btid = dcon::fatten(state.world, uf.type);

				for(auto const& e : factory_elements)
					e->set_visible(state, true);
				for(auto const& e : upgrade_elements)
					e->set_visible(state, true);
				for(auto const& e : build_elements)
					e->set_visible(state, false);
				for(auto const& e : closed_elements)
					e->set_visible(state, false);
			} else {
				// "Normal" factory, not being upgraded or built
				dcon::factory_id fid = content.id;
				fat_btid = state.world.factory_get_building_type(fid);

				bool is_closed = dcon::fatten(state.world, fid).get_production_scale() < 0.05f;
				for(auto const& e : factory_elements)
					e->set_visible(state, true);
				for(auto const& e : upgrade_elements)
					e->set_visible(state, false);
				for(auto const& e : build_elements)
					e->set_visible(state, false);
				for(auto const& e : closed_elements)
					e->set_visible(state, is_closed);
			}

			auto& cset = fat_btid.get_inputs();
			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
				if(input_icons[size_t(i)]) {
					dcon::commodity_id cid = cset.commodity_type[size_t(i)];
					input_icons[size_t(i)]->frame = int32_t(state.world.commodity_get_icon(cid));
					input_icons[size_t(i)]->com = cid;
					bool is_lack = cid != dcon::commodity_id{} ? state.world.nation_get_demand_satisfaction(n, cid) < 0.5f : false;
					input_lack_icons[size_t(i)]->set_visible(state, is_lack);
				}
			}
			output_commodity = fat_btid.get_output().id;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(parent) {
			Cyto::Any p_payload = production_factory_slot_data{{dcon::factory_id{}, std::monostate{}}, index};
			parent->impl_get(state, p_payload);
			auto content = any_cast<production_factory_slot_data>(p_payload);
			if(payload.holds_type<dcon::factory_id>()) {
				payload.emplace<dcon::factory_id>(content.id);
				return message_result::consumed;
			} else if(payload.holds_type<economy::upgraded_factory>()) {
				if(std::holds_alternative<economy::upgraded_factory>(content.activity))
					payload.emplace<economy::upgraded_factory>(std::get<economy::upgraded_factory>(content.activity));
				return message_result::consumed;
			} else if(payload.holds_type<economy::new_factory>()) {
				if(std::holds_alternative<economy::new_factory>(content.activity))
					payload.emplace<economy::new_factory>(std::get<economy::new_factory>(content.activity));
				return message_result::consumed;
			} else if(payload.holds_type<dcon::commodity_id>()) {
				payload.emplace<dcon::commodity_id>(output_commodity);
				return message_result::consumed;
			} else if(payload.holds_type<dcon::province_id>()) {
				payload.emplace<dcon::province_id>(state.world.factory_get_province_from_factory_location(content.id));
				return message_result::consumed;
			}
		}
		return message_result::unseen;
	}
};



class production_factory_info_bounds_window : public window_element_base {
	std::vector<element_base*> infos;
	std::vector<state_factory_slot> factories;

	bool get_filter(sys::state& state, dcon::commodity_id cid) {
		Cyto::Any payload = commodity_filter_query_data{cid, false};
		parent->impl_get(state, payload);
		auto content = any_cast<commodity_filter_query_data>(payload);
		return content.filter;
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		factories.resize(size_t(state.defines.factories_per_state));
		// Create factory slots for each of the provinces
		for(uint8_t factory_index = 0; factory_index < uint8_t(state.defines.factories_per_state); ++factory_index) {
			auto ptr = make_element_by_type<production_factory_info>(state,
					state.ui_state.defs_by_name.find("factory_info")->second.definition);
			ptr->index = factory_index;
			ptr->base_data.position.x = factory_index * ptr->base_data.size.x;
			infos.push_back(ptr.get());
			add_child_to_front(std::move(ptr));
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto state_id = retrieve<dcon::state_instance_id>(state, parent);

		for(auto const c : infos)
			c->set_visible(state, false);

		std::vector<bool> visited_types(state.world.factory_type_size(), false);
		size_t index = 0;
		// First, the new factories are taken into account
		economy::for_each_new_factory(state, state_id, [&](economy::new_factory const& nf) {
			dcon::commodity_id cid = state.world.factory_type_get_output(nf.type).id;
			if(!visited_types[nf.type.index()] && get_filter(state, cid) && index < state.defines.factories_per_state) {
				factories[index].activity = nf;
				factories[index].id = dcon::factory_id{};
				visited_types[nf.type.index()] = true;
				infos[index]->set_visible(state, true);
				++index;
			}
		});
		// Then, the factories being upgraded
		economy::for_each_upgraded_factory(state, state_id, [&](economy::upgraded_factory const& uf) {
			dcon::commodity_id cid = state.world.factory_type_get_output(uf.type).id;
			if(!visited_types[uf.type.index()] && get_filter(state, cid) && index < state.defines.factories_per_state) {
				factories[index].activity = uf;
				province::for_each_province_in_state_instance(state, state_id, [&](dcon::province_id prov) {
					for(auto fa : state.world.province_get_factory_location(prov)) {
						if(fa.get_factory().get_building_type() == uf.type) {
							factories[index].id = fa.get_factory().id;
						}
					}
				});

				visited_types[uf.type.index()] = true;
				infos[index]->set_visible(state, true);
				++index;
			}
		});
		// Finally, factories "doing nothing" are accounted for
		province::for_each_province_in_state_instance(state, state_id, [&](dcon::province_id pid) {
			dcon::fatten(state.world, pid).for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
				dcon::factory_id fid = state.world.factory_location_get_factory(flid);
				dcon::factory_type_id ftid = state.world.factory_get_building_type(fid);
				dcon::commodity_id cid = state.world.factory_type_get_output(ftid).id;
				if(!visited_types[ftid.index()] && get_filter(state, cid) && index < state.defines.factories_per_state) {
					factories[index].activity = std::monostate{};
					factories[index].id = fid;
					infos[index]->set_visible(state, true);
					++index;
				}
			});
		});
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_factory_slot_data>()) {
			auto content = any_cast<production_factory_slot_data>(payload);
			auto index = content.index;
			static_cast<state_factory_slot&>(content) = factories[index];
			content.index = index;
			payload.emplace<production_factory_slot_data>(content);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class province_build_new_factory : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::province_id pid = retrieve<dcon::province_id>(state, parent);
		const dcon::state_instance_id sid = state.world.province_get_state_membership(pid);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		bool can_build = false;
		state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
			can_build =
				can_build || command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false);
		});
		disabled = !can_build;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			const dcon::province_id pid = retrieve<dcon::province_id>(state, parent);
			const dcon::state_instance_id sid = state.world.province_get_state_membership(pid);
			state.ui_state.production_subwindow->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.production_subwindow);
			state.ui_state.topbar_subwindow = state.ui_state.production_subwindow;
			send(state, state.ui_state.production_subwindow, production_window_tab::factories);
			send(state, state.ui_state.production_subwindow, production_selection_wrapper{ sid, true, xy_pair{0, 0} });
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::province_id pid = retrieve<dcon::province_id>(state, parent);
		const dcon::state_instance_id sid = state.world.province_get_state_membership(pid);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		bool non_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));

		bool is_civilized = state.world.nation_get_is_civilized(n);
		int32_t num_factories = economy::state_factory_count(state, sid, n);

		text::add_line(state, contents, "production_build_new_factory_tooltip");
		text::add_line_break_to_layout(state, contents);
		text::add_line_with_condition(state, contents, "factory_condition_1", is_civilized);
		text::add_line_with_condition(state, contents, "factory_condition_2", non_colonial);

		if(n == state.local_player_nation) {
			auto rules = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_condition_3", (rules & issue_rule::build_factory) != 0);
		} else {
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_4", state.world.nation_get_is_great_power(state.local_player_nation) && !state.world.nation_get_is_great_power(n));

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_5", state.world.nation_get_is_civilized(n));

			auto target = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_6",
					(target & issue_rule::allow_foreign_investment) != 0);

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_7", !military::are_at_war(state, state.local_player_nation, n));
		}

		{
			auto box = text::open_layout_box(contents);
			if(num_factories < int32_t(state.defines.factories_per_state)) {
				text::add_to_layout_box(state, contents, box, std::string_view("\x02"), text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, std::string_view("\x01"), text::text_color::red);
			}
			text::add_space_to_layout_box(state, contents, box);
			text::localised_single_sub_box(state, contents, box, "factory_condition_4", text::variable_type::val,
					int64_t(state.defines.factories_per_state));
			text::close_layout_box(contents, box);
		}
	}
};

class production_build_new_factory : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const dcon::state_instance_id sid = retrieve<dcon::state_instance_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		bool can_build = false;
		state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
			can_build =
					can_build || command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false);
		});
		disabled = !can_build;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			dcon::state_instance_id sid = retrieve<dcon::state_instance_id>(state, parent);
			send(state, parent, production_selection_wrapper{sid, true, xy_pair{0, 0}});
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		const dcon::state_instance_id sid = retrieve<dcon::state_instance_id>(state, parent);
		const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		bool non_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));

		bool is_civilized = state.world.nation_get_is_civilized(n);
		int32_t num_factories = economy::state_factory_count(state, sid, n);

		text::add_line(state, contents, "production_build_new_factory_tooltip");
		text::add_line_break_to_layout(state, contents);
		text::add_line_with_condition(state, contents, "factory_condition_1", is_civilized);
		text::add_line_with_condition(state, contents, "factory_condition_2", non_colonial);

		if(n == state.local_player_nation) {
			auto rules = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_condition_3", (rules & issue_rule::build_factory) != 0);
		} else {
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_4", state.world.nation_get_is_great_power(state.local_player_nation) && !state.world.nation_get_is_great_power(n));

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_5", state.world.nation_get_is_civilized(n));

			auto target = state.world.nation_get_combined_issue_rules(n);
			text::add_line_with_condition(state, contents, "factory_upgrade_condition_6",
					(target & issue_rule::allow_foreign_investment) != 0);

			text::add_line_with_condition(state, contents, "factory_upgrade_condition_7", !military::are_at_war(state, state.local_player_nation, n));
		}

		{
			auto box = text::open_layout_box(contents);
			if(num_factories < int32_t(state.defines.factories_per_state)) {
				text::add_to_layout_box(state, contents, box, std::string_view("\x02"), text::text_color::green);
			} else {
				text::add_to_layout_box(state, contents, box, std::string_view("\x01"), text::text_color::red);
			}
			text::add_space_to_layout_box(state, contents, box);
			text::localised_single_sub_box(state, contents, box, "factory_condition_4", text::variable_type::val,
					int64_t(state.defines.factories_per_state));
			text::close_layout_box(contents, box);
		}
	}
};

class production_national_focus_button : public button_element_base {
	int32_t get_icon_frame(sys::state& state) noexcept {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		return bool(state.world.state_instance_get_owner_focus(content).id)
								? state.world.state_instance_get_owner_focus(content).get_icon() - 1
								: 0;
	}

public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);

		disabled = n != state.local_player_nation;
		state.world.for_each_national_focus([&](dcon::national_focus_id nfid) {
			disabled = command::can_set_national_focus(state, state.local_player_nation, content, nfid) ? false : disabled;
		});
		frame = get_icon_frame(state);
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			Cyto::Any s_payload = production_selection_wrapper{any_cast<dcon::state_instance_id>(payload), false, base_data.position};
			parent->impl_get(state, s_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		dcon::national_focus_fat_id focus = state.world.state_instance_get_owner_focus(content);
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, focus.get_name());
		text::close_layout_box(contents, box);
	}
};

class per_state_primary_worker_amount : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto total = state.world.state_instance_get_demographics(content, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		auto employed = state.world.state_instance_get_demographics(content, demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
		auto unemployed = total - employed;
		set_text(state, text::prettify(int64_t(unemployed)) + "/" + text::prettify(int64_t(employed)));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto total = state.world.state_instance_get_demographics(content, demographics::to_key(state, state.culture_definitions.primary_factory_worker));
		auto employed = state.world.state_instance_get_demographics(content, demographics::to_employment_key(state, state.culture_definitions.primary_factory_worker));
		auto unemployed = total - employed;
		text::add_line(state, contents, "alice_factory_worker_1", text::variable_type::x, text::pretty_integer{ int32_t(employed) });
		text::add_line(state, contents, "alice_factory_worker_2", text::variable_type::x, text::pretty_integer{ int32_t(unemployed) });
	}
};

class per_state_secondary_worker_amount : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto total = state.world.state_instance_get_demographics(content, demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
		auto employed = state.world.state_instance_get_demographics(content, demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));
		auto unemployed = total - employed;
		set_text(state, text::prettify(int64_t(unemployed)) + "/" + text::prettify(int64_t(employed)));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto total = state.world.state_instance_get_demographics(content, demographics::to_key(state, state.culture_definitions.secondary_factory_worker));
		auto employed = state.world.state_instance_get_demographics(content, demographics::to_employment_key(state, state.culture_definitions.secondary_factory_worker));
		auto unemployed = total - employed;
		text::add_line(state, contents, "alice_factory_worker_1", text::variable_type::x, text::pretty_integer{ int32_t(employed) });
		text::add_line(state, contents, "alice_factory_worker_2", text::variable_type::x, text::pretty_integer{ int32_t(unemployed) });
	}
};

class per_state_capitalist_amount : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		auto total = state.world.state_instance_get_demographics(content,
				demographics::total);
		if(total > 0)
			set_text(state, text::format_percentage(
				state.world.state_instance_get_demographics(content, demographics::to_key(state, state.culture_definitions.capitalists)) / total,
				1));
		else
			set_text(state, "0.0%");
	}
};


class state_infrastructure : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_instance_id>(state, parent);
		float total = 0.0f;
		float p_total = 0.0f;
		province::for_each_province_in_state_instance(state, content, [&](dcon::province_id p) {
			total += state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure * float(state.world.province_get_building_level(p, economy::province_building_type::railroad));
			p_total += 1.0f;
		});
		set_text(state, text::format_float(p_total > 0 ? total / p_total : 0.0f, 3));
	}
};

class production_state_info : public listbox_row_element_base<dcon::state_instance_id> {
public:
	ui::element_data factory_number_def;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_focus") {
			return make_element_by_type<production_national_focus_button>(state, id);
		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "factory_count") {
			auto temp = make_element_by_type<state_factory_count_text>(state, id);
			memcpy(&factory_number_def, &(temp->base_data), sizeof(ui::element_data));
			return temp;
		} else if(name == "build_new_factory") {
			return make_element_by_type<production_build_new_factory>(state, id);
		} else if(name == "avg_infra_text") {
			return make_element_by_type<state_infrastructure>(state, id);
		} else if(name == "factory_info_bounds") {
			return make_element_by_type<production_factory_info_bounds_window>(state, id);
		} else {
			return nullptr;
		}
	}
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<dcon::state_instance_id>::on_create(state);

		xy_pair base_sort_template_offset =
				state.ui_defs.gui[state.ui_state.defs_by_name.find("sort_by_pop_template_offset")->second.definition].position;

		{
			auto text_elm = std::make_unique< per_state_primary_worker_amount>();
			memcpy(&(text_elm->base_data), &factory_number_def, sizeof(ui::element_data));
			text_elm->base_data.position.x = int16_t(500 + base_sort_template_offset.x * 0);
			text_elm->on_create(state);
			add_child_to_front(std::move(text_elm));
		}
		{
			auto text_elm = std::make_unique< per_state_secondary_worker_amount>();
			memcpy(&(text_elm->base_data), &factory_number_def, sizeof(ui::element_data));
			text_elm->base_data.position.x = int16_t(500 + base_sort_template_offset.x * 1);
			text_elm->on_create(state);
			add_child_to_front(std::move(text_elm));
		}
		{
			auto text_elm = std::make_unique< per_state_capitalist_amount>();
			memcpy(&(text_elm->base_data), &factory_number_def, sizeof(ui::element_data));
			text_elm->base_data.position.x = int16_t(500 + base_sort_template_offset.x * 2);
			text_elm->on_create(state);
			add_child_to_front(std::move(text_elm));
		}
	}
};

void populate_production_states_list(sys::state& state, std::vector<dcon::state_instance_id>& row_contents, dcon::nation_id n, bool show_empty, production_sort_order sort_order);

struct production_foreign_invest_target {
	dcon::nation_id n;
};

class production_state_invest_listbox : public listbox_element_base<production_state_info, dcon::state_instance_id> {
protected:
	std::string_view get_row_element_name() override {
		return "state_info";
	}

public:
	production_sort_order sort_order = production_sort_order::name;

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			auto show_empty = retrieve<bool>(state, parent);
			dcon::nation_id n = retrieve<production_foreign_invest_target>(state, parent).n;

			populate_production_states_list(state, row_contents, n, show_empty, sort_order);
		}
		update(state);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(retrieve<production_foreign_invest_target>(state, parent).n);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class production_state_listbox : public listbox_element_base<production_state_info, dcon::state_instance_id> {
protected:
	std::string_view get_row_element_name() override {
		return "state_info";
	}

public:
	production_sort_order sort_order = production_sort_order::name;

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			auto show_empty = retrieve<bool>(state, parent);
			const dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
			populate_production_states_list(state, row_contents, n, show_empty, sort_order);
		}
		update(state);
	}
};

class production_goods_category_name : public window_element_base {
	simple_text_element_base* goods_cat_name = nullptr;

	std::string_view get_commodity_group_name(sys::commodity_group g) {
		switch(g) {
		case sys::commodity_group::military_goods:
			return "military_goods";
		case sys::commodity_group::raw_material_goods:
			return "raw_material_goods";
		case sys::commodity_group::industrial_goods:
			return "industrial_goods";
		case sys::commodity_group::consumer_goods:
			return "consumer_goods";
		// Non-vanilla
		case sys::commodity_group::industrial_and_consumer_goods:
			return "industrial_and_consumer_goods";
		default:
			return "???";
		}
	}

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cat_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			goods_cat_name = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<sys::commodity_group>()) {
			auto group = any_cast<sys::commodity_group>(payload);
			goods_cat_name->set_text(state, text::produce_simple_string(state, get_commodity_group_name(group)));
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class commodity_primary_worker_amount : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::commodity_id>(state, parent);
		auto commodity_type = economy::get_commodity_production_type(state, content);

		float total = 0.0f;

		auto nation = dcon::fatten(state.world, state.local_player_nation);
		switch (commodity_type) {
		case economy::commodity_production_type::primary:
		case economy::commodity_production_type::both:
			for(auto province_ownership : state.world.nation_get_province_ownership(nation)) {
				auto province = province_ownership.get_province();

				if(province.get_rgo() == content) {
					total += economy::rgo_max_employment(state, nation, province) * state.world.province_get_rgo_employment(province);
				}
			}
			break;

		case economy::commodity_production_type::derivative:
			total += nation.get_artisan_distribution(content) * nation.get_demographics(demographics::to_key(state, state.culture_definitions.artisans));
			for(auto province_ownership : state.world.nation_get_province_ownership(nation)) {
				auto province = province_ownership.get_province();
				for(auto fac : province.get_factory_location()) {
					if(fac.get_factory().get_building_type().get_output() == content) {
						total += economy::factory_primary_employment(state, fac.get_factory());
					}
				}
			}
			break;

		}


		set_text(state, text::prettify(int64_t(total)));
	}
};

class commodity_secondary_worker_amount : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::commodity_id>(state, parent);
		auto commodity_type = economy::get_commodity_production_type(state, content);

		float total = 0.0f;

		auto nation = dcon::fatten(state.world, state.local_player_nation);
		switch(commodity_type) {
		case economy::commodity_production_type::primary:
			break;

		case economy::commodity_production_type::derivative:
			for(auto province_ownership : state.world.nation_get_province_ownership(nation)) {
				auto province = province_ownership.get_province();
				for(auto fac : province.get_factory_location()) {
					if(fac.get_factory().get_building_type().get_output() == content) {
						total += economy::factory_secondary_employment(state, fac.get_factory());
					}
				}
			}
			break;

		case economy::commodity_production_type::both:
			total += nation.get_artisan_distribution(content) * nation.get_demographics(demographics::to_key(state, state.culture_definitions.artisans));
			for(auto province_ownership : state.world.nation_get_province_ownership(nation)) {
				auto province = province_ownership.get_province();
				for(auto fac : province.get_factory_location()) {
					if(fac.get_factory().get_building_type().get_output() == content) {
						total += economy::factory_primary_employment(state, fac.get_factory()) + economy::factory_secondary_employment(state, fac.get_factory());
					}
				}
			}
			break;
		}


		set_text(state, text::prettify(int64_t(total)));
	}
};

class commodity_player_production_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto commodity_id = retrieve<dcon::commodity_id>(state, parent);
		if(commodity_id)
			set_text(state, text::format_float(state.world.nation_get_domestic_market_pool(state.local_player_nation, commodity_id), 1));
	}
};

class production_good_info : public window_element_base {
	commodity_player_production_text* good_output_total = nullptr;
	image_element_base* good_not_producing_overlay = nullptr;

public:
	dcon::commodity_id commodity_id;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "output_factory") {
			return make_element_by_type<commodity_image>(state, id);
		} else if(name == "output_total") {
			auto ptr = make_element_by_type<commodity_player_production_text>(state, id);
			good_output_total = ptr.get();
			return ptr;
		} else if(name == "prod_producing_not_total") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			good_not_producing_overlay = ptr.get();
			return ptr;
		} else if(name == "pop_factory") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = int32_t(dcon::fatten(state.world, state.culture_definitions.primary_factory_worker).get_sprite() - 1);
			return ptr;
		} else if(name == "pop_factory2") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = int32_t(dcon::fatten(state.world, state.culture_definitions.secondary_factory_worker).get_sprite() - 1);
			return ptr;
		} else if(name == "output") {
			return make_element_by_type<commodity_primary_worker_amount>(state, id);
		} else if(name == "output2") {
			return make_element_by_type<commodity_secondary_worker_amount>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		bool is_producing = economy::commodity_daily_production_amount(state, commodity_id) > 0.f;
		// Display red-overlay if not producing
		good_not_producing_overlay->set_visible(state, !is_producing);
		good_output_total->set_visible(state, is_producing);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(commodity_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

struct open_investment_nation {
	dcon::nation_id id;
};

class production_window : public generic_tabbed_window<production_window_tab> {
	bool show_empty_states = true;
	std::unique_ptr<bool[]> show_output_commodity;

	production_state_listbox* state_listbox = nullptr;
	production_state_invest_listbox* state_listbox_invest = nullptr;
	element_base* nf_win = nullptr;
	element_base* build_win = nullptr;
	element_base* project_window = nullptr;
	production_foreign_investment_window* foreign_invest_win = nullptr;

	sys::commodity_group curr_commodity_group{};
	dcon::state_instance_id focus_state{};
	dcon::nation_id foreign_nation{};
	xy_pair base_commodity_offset{33, 50};
	xy_pair commodity_offset{33, 50};

	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_brow_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
	std::vector<element_base*> investment_nation;
	std::vector<bool> commodity_filters;
	bool open_foreign_invest = false;

	void set_visible_vector_elements(sys::state& state, std::vector<element_base*>& elements, bool v) noexcept {
		for(auto element : elements)
			element->set_visible(state, v);
	}

	void hide_sub_windows(sys::state& state) noexcept {
		set_visible_vector_elements(state, factory_elements, false);
		set_visible_vector_elements(state, investment_brow_elements, false);
		set_visible_vector_elements(state, project_elements, false);
		set_visible_vector_elements(state, good_elements, false);
		set_visible_vector_elements(state, investment_nation, false);
	}

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);

		// All filters enabled by default
		commodity_filters.resize(state.world.commodity_size(), true);

		for(curr_commodity_group = sys::commodity_group::military_goods; curr_commodity_group != sys::commodity_group::count;
				curr_commodity_group = static_cast<sys::commodity_group>(uint8_t(curr_commodity_group) + 1)) {

			bool is_empty = true;
			for(auto id : state.world.in_commodity) {
				if(sys::commodity_group(state.world.commodity_get_commodity_group(id)) != curr_commodity_group || !bool(id) || id == economy::money)
					continue;
				is_empty = false;
			}
			if(is_empty)
				continue;

			commodity_offset.x = base_commodity_offset.x;

			// Place legend for this category...
			auto ptr = make_element_by_type<production_goods_category_name>(state,
					state.ui_state.defs_by_name.find("production_goods_name")->second.definition);
			ptr->base_data.position = commodity_offset;
			Cyto::Any payload = curr_commodity_group;
			ptr->impl_set(state, payload);
			ptr->set_visible(state, false);
			commodity_offset.y += ptr->base_data.size.y;
			good_elements.push_back(ptr.get());
			add_child_to_front(std::move(ptr));

			int16_t cell_height = 0;
			// Place infoboxes for each of the goods...
			for(auto id : state.world.in_commodity) {
				if(sys::commodity_group(state.world.commodity_get_commodity_group(id)) != curr_commodity_group || !bool(id) || id == economy::money)
					continue;

				auto info_ptr = make_element_by_type<production_good_info>(state,
						state.ui_state.defs_by_name.find("production_info")->second.definition);
				info_ptr->base_data.position = commodity_offset;
				info_ptr->set_visible(state, false);

				int16_t cell_width = info_ptr->base_data.size.x;
				cell_height = info_ptr->base_data.size.y;

				commodity_offset.x += cell_width;
				if(commodity_offset.x + cell_width >= base_data.size.x) {
					commodity_offset.x = base_commodity_offset.x;
					commodity_offset.y += cell_height;
				}

				info_ptr.get()->commodity_id = id;

				good_elements.push_back(info_ptr.get());
				add_child_to_front(std::move(info_ptr));
			}
			// Has atleast 1 good on this row? skip to next row then...
			if(commodity_offset.x > base_commodity_offset.x)
				commodity_offset.y += cell_height;
		}

		{
			auto ptr = make_element_by_type<national_focus_window>(state, "state_focus_window");
			ptr->set_visible(state, false);
			nf_win = ptr.get();
			add_child_to_front(std::move(ptr));
		}

		auto win = make_element_by_type<factory_build_window>(state, state.ui_state.defs_by_name.find("build_factory")->second.definition);
		build_win = win.get();
		add_child_to_front(std::move(win));

		auto win2 = make_element_by_type<project_investment_window>(state,
				state.ui_state.defs_by_name.find("invest_project_window")->second.definition);
		win2->set_visible(state, false);
		project_window = win2.get();
		add_child_to_front(std::move(win2));

		show_output_commodity = std::unique_ptr<bool[]>(new bool[state.world.commodity_size()]);
		set_visible(state, false);
	}
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "tab_factories") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::factories;
			return ptr;
		} else if(name == "tab_invest") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::investments;
			return ptr;
		} else if(name == "tab_popprojects") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::projects;
			return ptr;
		} else if(name == "tab_goodsproduction") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::goods;
			return ptr;
		} else if(name == "tab_factories_text") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "tab_invest_text") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "tab_goodsproduction_text") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "tab_popprojects_text") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "factory_buttons") {
			auto ptr = make_element_by_type<factory_buttons_window>(state, id);
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "invest_buttons") {
			auto ptr = make_element_by_type<production_foreign_investment_window>(state, id);
			foreign_invest_win = ptr.get();
			investment_nation.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "state_listbox") {
			auto ptr = make_element_by_type<production_state_listbox>(state, id);
			state_listbox = ptr.get();
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "state_listbox_invest") {
			auto ptr = make_element_by_type<production_state_invest_listbox>(state, id);
			state_listbox_invest = ptr.get();
			investment_nation.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "investment_browser") {
			auto ptr = make_element_by_type<invest_brow_window>(state, id);
			investment_brow_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_state") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projects") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_completion") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projecteers") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "pop_sort_buttons") {
			auto ptr = make_element_by_type<pop_sort_buttons_window>(state, id);
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "project_listbox") {
			auto ptr = make_element_by_type<production_project_listbox>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_window_tab>()) {
			auto enum_val = any_cast<production_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
			case production_window_tab::factories:
				set_visible_vector_elements(state, factory_elements, true);
				break;
			case production_window_tab::investments:
				set_visible_vector_elements(state, investment_brow_elements, true);
				foreign_invest_win->set_visible(state, open_foreign_invest);
				break;
			case production_window_tab::projects:
				set_visible_vector_elements(state, project_elements, true);
				break;
			case production_window_tab::goods:
				set_visible_vector_elements(state, good_elements, true);
				break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		} else if(payload.holds_type<production_foreign_invest_target>()) {
			payload.emplace<production_foreign_invest_target>(production_foreign_invest_target{foreign_invest_win->curr_nation});
			return message_result::consumed;
		} else if(payload.holds_type<open_investment_nation>()) {
			hide_sub_windows(state);
			auto target = any_cast<open_investment_nation>(payload).id;
			active_tab = production_window_tab::investments;
			foreign_invest_win->curr_nation = target;
			set_visible_vector_elements(state, investment_nation, true);
			return message_result::consumed;
		} else if(payload.holds_type<production_sort_order>()) {
			auto sort_type = any_cast<production_sort_order>(payload);
			state_listbox->sort_order = sort_type;
			if(state_listbox->is_visible())
				state_listbox->impl_on_update(state);
			state_listbox_invest->sort_order = sort_type;
			if(state_listbox_invest->is_visible())
				state_listbox_invest->impl_on_update(state);
		} else if(payload.holds_type<dcon::state_instance_id>()) {
			payload.emplace<dcon::state_instance_id>(focus_state);
			return message_result::consumed;
		} else if(payload.holds_type<production_selection_wrapper>()) {
			auto data = any_cast<production_selection_wrapper>(payload);
			focus_state = data.data;
			if(data.is_build) {
				build_win->set_visible(state, true);
				move_child_to_front(build_win);
			} else {
				nf_win->set_visible(state, true);
				nf_win->base_data.position = data.focus_pos;
				move_child_to_front(nf_win);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(show_empty_states);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			show_empty_states = any_cast<element_selection_wrapper<bool>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<commodity_filter_query_data>()) {
			auto content = any_cast<commodity_filter_query_data>(payload);
			content.filter = commodity_filters[content.cid.index()];
			payload.emplace<commodity_filter_query_data>(content);
			return message_result::consumed;
		} else if(payload.holds_type<commodity_filter_toggle_data>()) {
			auto content = any_cast<commodity_filter_toggle_data>(payload);
			commodity_filters[content.data.index()] = !commodity_filters[content.data.index()];
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<production_action>>()) {
			auto content = any_cast<element_selection_wrapper<production_action>>(payload).data;
			switch(content) {
			case production_action::investment_window:
				project_window->is_visible() ? project_window->set_visible(state, false) : project_window->set_visible(state, true);
				break;
			case production_action::foreign_invest_window:
				foreign_invest_win->is_visible() ? foreign_invest_win->set_visible(state, false)  : foreign_invest_win->set_visible(state, true);
				break;
			default:
				break;
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::nation_id>>()) {
			foreign_nation = any_cast<element_selection_wrapper<dcon::nation_id>>(payload).data;
			open_foreign_invest = true;
			foreign_invest_win->set_visible(state, true);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			if(foreign_invest_win->is_visible())
				payload.emplace<dcon::nation_id>(foreign_nation);
			else
				payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

void open_foreign_investment(sys::state& state, dcon::nation_id n);
void open_build_foreign_factory(sys::state& state, dcon::state_instance_id st);

} // namespace ui
