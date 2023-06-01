#pragma once

#include "gui_element_types.hpp"

namespace ui {

enum diplomacy_action : uint8_t {
	ally,
	cancel_ally,
	call_ally,
	military_access,
	cancel_military_access,
	give_military_access,
	cancel_give_military_access,
	increase_relations,
	decrease_relations,
	war_subsidies,
	cancel_war_subsidies,
	declare_war,
	command_units,
	cancel_command_units,
	discredit,
	expel_advisors,
	ban_embassy,
	increase_opinion,
	decrease_opinion,
	add_to_sphere,
	remove_from_sphere,
	justify_war,
	make_peace,
	crisis_backdown,
	crisis_support,
};

class diplomacy_action_ally_button : public button_element_base {
	bool can_cancel(sys::state& state, dcon::nation_id nation_id) noexcept {
		auto drid = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, nation_id);
		return drid && state.world.diplomatic_relation_get_are_allied(drid);
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			set_button_text(state, text::produce_simple_string(state,
				can_cancel(state, content) ? "cancelalliance_button"
					: "alliance_button"));

			if(can_cancel(state, content))
				disabled = !command::can_cancel_alliance(state, state.local_player_nation, content);
			else
				disabled = !command::can_ask_for_alliance(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			Cyto::Any ac_payload = can_cancel(state, content) ? diplomacy_action::cancel_ally
				: diplomacy_action::ally;
			parent->impl_get(state, ac_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			can_cancel(state, content) ? text::localised_format_box(state, contents, box, std::string_view("cancelalliance_desc")) : text::localised_format_box(state, contents, box, std::string_view("alliance_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::substitution_map dp_map{};
				text::add_to_substitution_map(dp_map, text::variable_type::current, text::fp_two_places{ state.world.nation_get_diplomatic_points(state.local_player_nation) });
				text::add_to_substitution_map(dp_map, text::variable_type::needed, text::fp_two_places{ !can_cancel(state, content) ? state.defines.alliance_diplomatic_cost : state.defines.cancelalliance_diplomatic_cost });
				text::localised_format_box(state, contents, box, std::string_view(state.world.nation_get_diplomatic_points(state.local_player_nation) >= (!can_cancel(state, content) ? state.defines.alliance_diplomatic_cost : state.defines.cancelalliance_diplomatic_cost) ? "dip_enough_diplo" : "dip_no_diplo"), dp_map);
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map); // Always return 0, that way leafs ai ambititions will be stunted :3
				// leaf: Of course this will never occur because the AI would absolutely obliterate the player in 1v1
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_call_ally_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "callally_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto fat = dcon::fatten(state.world, content);
			for(auto war_par : fat.get_war_participant()) {
				if(command::can_call_to_arms(state, state.local_player_nation, content, dcon::fatten(state.world, war_par).get_war().id)) {
					disabled = !command::can_call_to_arms(state, state.local_player_nation, content, dcon::fatten(state.world, war_par).get_war().id);
					break;
				}
			}
			// TODO: Conditions for enabling/disabling
			/*
			disabled = false;
			if(content == state.local_player_nation)
				disabled = true;
			else {
				auto drid = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, content);
				disabled = !state.world.diplomatic_relation_get_are_allied(drid);
			}*/
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::call_ally;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("remove_callally_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::substitution_map dp_map{};
				text::add_to_substitution_map(dp_map, text::variable_type::current, text::fp_two_places{ state.world.nation_get_diplomatic_points(state.local_player_nation) });
				text::add_to_substitution_map(dp_map, text::variable_type::needed, text::fp_two_places{ state.defines.callally_diplomatic_cost });
				text::localised_format_box(state, contents, box, std::string_view(state.world.nation_get_diplomatic_points(state.local_player_nation) >= state.defines.callally_diplomatic_cost ? "dip_enough_diplo" : "dip_no_diplo"), dp_map);
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_military_access_button : public button_element_base {
	bool can_cancel(sys::state& state, dcon::nation_id nation_id) noexcept {
		auto urid = state.world.get_unilateral_relationship_by_unilateral_pair(nation_id, state.local_player_nation);
		return urid && state.world.unilateral_relationship_get_military_access(urid);
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			set_button_text(state, text::produce_simple_string(state,
				can_cancel(state, content) ? "cancelaskmilitaryaccess_button"
					: "askmilitaryaccess_button"));

			// TODO: Conditions for enabling/disabling
			if(can_cancel(state, content))
				disabled = !command::can_cancel_military_access(state, state.local_player_nation, content);
			else
				disabled = !command::can_ask_for_access(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			Cyto::Any ac_payload = can_cancel(state, content) ? diplomacy_action::cancel_military_access
				: diplomacy_action::military_access;
			parent->impl_get(state, ac_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			can_cancel(state, content) ? text::localised_format_box(state, contents, box, std::string_view("cancelaskmilitaryaccess_desc")) : text::localised_format_box(state, contents, box, std::string_view("askmilitaryaccess_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::substitution_map dp_map{};
				text::add_to_substitution_map(dp_map, text::variable_type::current, text::fp_two_places{ state.world.nation_get_diplomatic_points(state.local_player_nation) });
				text::add_to_substitution_map(dp_map, text::variable_type::needed, text::fp_two_places{ !can_cancel(state, content) ? state.defines.askmilaccess_diplomatic_cost : state.defines.cancelaskmilaccess_diplomatic_cost });
				text::localised_format_box(state, contents, box, std::string_view(state.world.nation_get_diplomatic_points(state.local_player_nation) >= (!can_cancel(state, content) ? state.defines.askmilaccess_diplomatic_cost : state.defines.cancelaskmilaccess_diplomatic_cost) ? "dip_enough_diplo" : "dip_no_diplo"), dp_map);
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_give_military_access_button : public button_element_base {
	bool can_cancel(sys::state& state, dcon::nation_id nation_id) noexcept {
		auto urid = state.world.get_unilateral_relationship_by_unilateral_pair(state.local_player_nation, nation_id);
		return urid && state.world.unilateral_relationship_get_military_access(urid);
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			set_button_text(state, text::produce_simple_string(state,
				can_cancel(state, content) ? "cancelgivemilitaryaccess_button"
					: "givemilitaryaccess_button"));

			// TODO: Conditions for enabling/disabling
			if(can_cancel(state, content))
				disabled = !command::can_cancel_given_military_access(state, state.local_player_nation, content);
			else
				disabled = true;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			Cyto::Any ac_payload = can_cancel(state, content) ? diplomacy_action::cancel_give_military_access
				: diplomacy_action::give_military_access;
			parent->impl_get(state, ac_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			can_cancel(state, content) ? text::localised_format_box(state, contents, box, std::string_view("cancelgivemilitaryaccess_desc")) : text::localised_format_box(state, contents, box, std::string_view("givemilitaryaccess_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::substitution_map dp_map{};
				text::add_to_substitution_map(dp_map, text::variable_type::current, text::fp_two_places{ state.world.nation_get_diplomatic_points(state.local_player_nation) });
				text::add_to_substitution_map(dp_map, text::variable_type::needed, text::fp_two_places{ !can_cancel(state, content) ? state.defines.givemilaccess_diplomatic_cost : state.defines.cancelgivemilaccess_diplomatic_cost });
				text::localised_format_box(state, contents, box, std::string_view(state.world.nation_get_diplomatic_points(state.local_player_nation) >= (!can_cancel(state, content) ? state.defines.givemilaccess_diplomatic_cost : state.defines.cancelgivemilaccess_diplomatic_cost) ? "dip_enough_diplo" : "dip_no_diplo"), dp_map);
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_increase_relations_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "increaserelation_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			disabled = !command::can_increase_relations(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::increase_relations;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("increaserelation_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::substitution_map dp_map{};
				text::add_to_substitution_map(dp_map, text::variable_type::current, text::fp_two_places{ state.world.nation_get_diplomatic_points(state.local_player_nation) });
				text::add_to_substitution_map(dp_map, text::variable_type::needed, text::fp_two_places{ state.defines.increaserelation_diplomatic_cost });
				text::localised_format_box(state, contents, box, std::string_view(state.world.nation_get_diplomatic_points(state.local_player_nation) >= state.defines.increaserelation_diplomatic_cost ? "dip_enough_diplo" : "dip_no_diplo"), dp_map);
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_decrease_relations_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "decreaserelation_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			disabled = !command::can_decrease_relations(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::decrease_relations;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("decreaserelation_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::substitution_map dp_map{};
				text::add_to_substitution_map(dp_map, text::variable_type::current, text::fp_two_places{ state.world.nation_get_diplomatic_points(state.local_player_nation) });
				text::add_to_substitution_map(dp_map, text::variable_type::needed, text::fp_two_places{ state.defines.decreaserelation_diplomatic_cost });
				text::localised_format_box(state, contents, box, std::string_view(state.world.nation_get_diplomatic_points(state.local_player_nation) >= state.defines.decreaserelation_diplomatic_cost ? "dip_enough_diplo" : "dip_no_diplo"), dp_map);
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_war_subisides_button : public button_element_base {
	bool can_cancel(sys::state& state, dcon::nation_id nation_id) noexcept {
		auto rel = state.world.get_unilateral_relationship_by_unilateral_pair(state.local_player_nation, nation_id);
		return rel && state.world.unilateral_relationship_get_war_subsidies(rel);
	}
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			set_button_text(state, text::produce_simple_string(state, can_cancel(state, content) ? "cancel_warsubsidies_button" : "warsubsidies_button"));

			disabled = can_cancel(state, content) ? !command::can_cancel_war_subsidies(state, state.local_player_nation, content) : !command::can_give_war_subsidies(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			Cyto::Any ac_payload = can_cancel(state, content) ? diplomacy_action::cancel_war_subsidies
				: diplomacy_action::war_subsidies;
			parent->impl_get(state, ac_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map ws_map{};
			text::add_to_substitution_map(ws_map, text::variable_type::money, text::fp_currency{ economy::estimate_war_subsidies(state, content) });
			text::localised_format_box(state, contents, box, std::string_view(can_cancel(state, content) ? "cancel_warsubsidies_desc" : "warsubsidies_desc"), ws_map);

			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::substitution_map dp_map{};
				text::add_to_substitution_map(dp_map, text::variable_type::current, text::fp_two_places{ state.world.nation_get_diplomatic_points(state.local_player_nation) });
				text::add_to_substitution_map(dp_map, text::variable_type::needed, text::fp_two_places{ !can_cancel(state, content) ? state.defines.warsubsidy_diplomatic_cost : state.defines.cancelwarsubsidy_diplomatic_cost });
				text::localised_format_box(state, contents, box, std::string_view(state.world.nation_get_diplomatic_points(state.local_player_nation) >= (!can_cancel(state, content) ? state.defines.warsubsidy_diplomatic_cost : state.defines.cancelwarsubsidy_diplomatic_cost) ? "dip_enough_diplo" : "dip_no_diplo"), dp_map);
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_declare_war_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "war_button"));
	}

	void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::nation_id{};
            parent->impl_get(state, payload);
            auto content = any_cast<dcon::nation_id>(payload);

			// TODO: Conditions for enabling/disabling
			disabled = false;
			if(content == state.local_player_nation)
				disabled = true;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::declare_war;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("act_wardesc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_command_units_button : public button_element_base {
	bool can_cancel(sys::state& state, dcon::nation_id nation_id) noexcept {
		// TODO - test if we local_player_nation has ability to command the other country
		return false;
	}
public:
	void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::nation_id{};
            parent->impl_get(state, payload);
            auto content = any_cast<dcon::nation_id>(payload);

			set_button_text(state, text::produce_simple_string(state,
				can_cancel(state, content) ? "cancel_unit_command_button"
					: "give_unit_command_button"));

			// TODO: Conditions for enabling/disabling
			disabled = false;
			if(content == state.local_player_nation)
				disabled = true;
		}
	}

	void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::nation_id{};
            parent->impl_get(state, payload);
            auto content = any_cast<dcon::nation_id>(payload);

			Cyto::Any ac_payload = can_cancel(state, content) ? diplomacy_action::cancel_command_units
				: diplomacy_action::command_units;
			parent->impl_get(state, ac_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
        if(parent) {
            Cyto::Any payload = dcon::nation_id{};
            parent->impl_get(state, payload);
            auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			can_cancel(state, content) ? text::localised_format_box(state, contents, box, std::string_view("cancel_unit_command_desc")) : text::localised_format_box(state, contents, box, std::string_view("give_unit_command_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_discredit_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "discredit_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			disabled = !command::can_discredit_advisors(state, state.local_player_nation, content, dcon::fatten(state.world, content).get_in_sphere_of().id);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::discredit;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("discredit_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else if(!nations::is_great_power(state, state.local_player_nation)) {
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_cannot_set_prio"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_expel_advisors_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "expeladvisors_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			disabled = !command::can_expel_advisors(state, state.local_player_nation, content, dcon::fatten(state.world, content).get_in_sphere_of().id);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::expel_advisors;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("expeladvisors_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else if(!nations::is_great_power(state, state.local_player_nation)) {
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_cannot_set_prio"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_ban_embassy_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "banembassy_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			disabled = !command::can_ban_embassy(state, state.local_player_nation, content, dcon::fatten(state.world, content).get_in_sphere_of().id);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::ban_embassy;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("banembassy_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else if(!nations::is_great_power(state, state.local_player_nation)) {
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_cannot_set_prio"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_increase_opinion_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "increaseopinion_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			disabled = !command::can_increase_opinion(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::increase_opinion;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("increaseopinion_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else if(!nations::is_great_power(state, state.local_player_nation)) {
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_cannot_set_prio"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_decrease_opinion_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "decreaseopinion_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			disabled = !command::can_decrease_opinion(state, state.local_player_nation, content, dcon::fatten(state.world, content).get_in_sphere_of().id);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::decrease_opinion;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("decreaseopinion_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else if(!nations::is_great_power(state, state.local_player_nation)) {
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_cannot_set_prio"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_add_to_sphere_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "addtosphere_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			disabled = !command::can_add_to_sphere(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::add_to_sphere;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("addtosphere_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else if(!nations::is_great_power(state, state.local_player_nation)) {
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_cannot_set_prio"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_remove_from_sphere_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "removefromsphere_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			disabled = !command::can_remove_from_sphere(state, state.local_player_nation, content, dcon::fatten(state.world, content).get_in_sphere_of().id);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::remove_from_sphere;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("removefromsphere_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else if(!nations::is_great_power(state, state.local_player_nation)) {
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_cannot_set_prio"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_justify_war_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "make_cb_button"));
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			// TODO: Conditions for enabling/disabling
			disabled = false;
			if(content == state.local_player_nation)
				disabled = true;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = diplomacy_action::justify_war;
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("make_cb_desc"));
			text::add_divider_to_layout_box(state, contents, box);
			if(content == state.local_player_nation) {
				text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
			} else {
				text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
				text::add_line_break_to_layout_box(contents, state, box);

				text::substitution_map ai_map{};
				text::add_to_substitution_map(ai_map, text::variable_type::country, content);
				text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class diplomacy_action_dialog_title_text : public generic_settable_element<simple_text_element_base, diplomacy_action> {
	std::string_view get_title_key() noexcept {
		switch(content) {
		case diplomacy_action::ally:
			return "alliancetitle";
		case diplomacy_action::cancel_ally:
			return "cancelalliancetitle";
		case diplomacy_action::call_ally:
			return "callallytitle";
		case diplomacy_action::declare_war:
			return "wartitle";
		case diplomacy_action::military_access:
			return "askmilitaryaccesstitle";
		case diplomacy_action::cancel_military_access:
			return "cancelaskmilitaryaccesstitle";
		case diplomacy_action::give_military_access:
			return "givemilitaryaccesstitle";
		case diplomacy_action::cancel_give_military_access:
			return "cancelgivemilitaryaccesstitle";
		case diplomacy_action::increase_relations:
			return "increaserelationtitle";
		case diplomacy_action::decrease_relations:
			return "decreaserelationtitle";
		case diplomacy_action::war_subsidies:
			return "warsubsidiestitle";
		case diplomacy_action::cancel_war_subsidies:
			return "cancel_warsubsidiestitle";
		case diplomacy_action::discredit:
			return "discredittitle";
		case diplomacy_action::expel_advisors:
			return "expeladvisorstitle";
		case diplomacy_action::ban_embassy:
			return "banembassytitle";
		case diplomacy_action::increase_opinion:
			return "increaseopiniontitle";
		case diplomacy_action::decrease_opinion:
			return "decreaseopiniontitle";
		case diplomacy_action::add_to_sphere:
			return "addtospheretitle";
		case diplomacy_action::remove_from_sphere:
			return "removefromspheretitle";
		case diplomacy_action::justify_war:
			return "make_cbtitle";
		case diplomacy_action::command_units:
			return "give_unit_commandtitle";
		case diplomacy_action::cancel_command_units:
			return "cancel_unit_commandtitle";
		case diplomacy_action::make_peace:
			return "make_peacetitle";
		}
		return "";
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_title_key()));
	}
};
class diplomacy_action_dialog_description_text : public generic_settable_element<simple_text_element_base, diplomacy_action> {
	std::string_view get_title_key() noexcept {
		switch(content) {
		case diplomacy_action::ally:
			return "alliancenewdesc";
		case diplomacy_action::cancel_ally:
			return "breakalliancenewdesc";
		case diplomacy_action::call_ally:
			return "callally_desc";
		case diplomacy_action::declare_war:
			return "war_desc";
		case diplomacy_action::military_access:
			return "askmilitaryaccess_desc";
		case diplomacy_action::cancel_military_access:
			return "cancelaskmilitaryaccess_desc";
		case diplomacy_action::give_military_access:
			return "givemilitaryaccess_desc";
		case diplomacy_action::cancel_give_military_access:
			return "cancelgivemilitaryaccess_desc";
		case diplomacy_action::increase_relations:
			return "increaserelation_desc";
		case diplomacy_action::decrease_relations:
			return "decreaserelation_desc";
		case diplomacy_action::war_subsidies:
			return "warsubsidies_desc";
		case diplomacy_action::cancel_war_subsidies:
			return "cancel_warsubsidies_desc";
		case diplomacy_action::discredit:
			return "discredit_desc";
		case diplomacy_action::expel_advisors:
			return "expeladvisors_desc";
		case diplomacy_action::ban_embassy:
			return "banembassy_desc";
		case diplomacy_action::increase_opinion:
			return "increaseopinion_desc";
		case diplomacy_action::decrease_opinion:
			return "decreaseopinion_desc";
		case diplomacy_action::add_to_sphere:
			return "addtosphere_desc";
		case diplomacy_action::remove_from_sphere:
			return "removefromsphere_desc";
		case diplomacy_action::justify_war:
			return "make_cb_desc";
		case diplomacy_action::command_units:
			return "give_unit_command_desc";
		case diplomacy_action::cancel_command_units:
			return "cancel_unit_command_desc";
		case diplomacy_action::make_peace:
			return "make_peace_desc";
		}
		return "";
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_title_key()));
	}
};

struct gp_selection_query_data {
	dcon::nation_id data{};
};

class diplomacy_action_dialog_agree_button : public generic_settable_element<button_element_base, diplomacy_action> {
	bool get_can_perform(sys::state& state) noexcept {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto target = any_cast<dcon::nation_id>(payload);

			Cyto::Any gp_payload = gp_selection_query_data{};
			parent->impl_get(state, gp_payload);
			auto gp_target = any_cast<gp_selection_query_data>(gp_payload).data;

			switch(content) {
			case diplomacy_action::ally:
				return command::can_ask_for_alliance(state, state.local_player_nation, target);
			case diplomacy_action::cancel_ally:
				return command::can_cancel_alliance(state, state.local_player_nation, target);
			case diplomacy_action::call_ally:
				return false;
			case diplomacy_action::declare_war:
				return false;
			case diplomacy_action::military_access:
				return command::can_ask_for_access(state, state.local_player_nation, target);
			case diplomacy_action::cancel_military_access:
				return command::can_cancel_military_access(state, state.local_player_nation, target);
			case diplomacy_action::give_military_access:
				return false;
			case diplomacy_action::cancel_give_military_access:
				return command::can_cancel_given_military_access(state, state.local_player_nation, target);
			case diplomacy_action::increase_relations:
				return command::can_increase_relations(state, state.local_player_nation, target);
			case diplomacy_action::decrease_relations:
				return command::can_decrease_relations(state, state.local_player_nation, target);
			case diplomacy_action::war_subsidies:
				return command::can_give_war_subsidies(state, state.local_player_nation, target);
			case diplomacy_action::cancel_war_subsidies:
				return command::can_cancel_war_subsidies(state, state.local_player_nation, target);
			case diplomacy_action::discredit:
				return command::can_discredit_advisors(state, state.local_player_nation, target, gp_target);
			case diplomacy_action::expel_advisors:
				return command::can_expel_advisors(state, state.local_player_nation, target, gp_target);
			case diplomacy_action::ban_embassy:
				return command::can_ban_embassy(state, state.local_player_nation, target, gp_target);
			case diplomacy_action::increase_opinion:
				return command::can_increase_opinion(state, state.local_player_nation, target);
			case diplomacy_action::decrease_opinion:
				return command::can_decrease_opinion(state, state.local_player_nation, target, gp_target);
			case diplomacy_action::add_to_sphere:
				return command::can_add_to_sphere(state, state.local_player_nation, target);
			case diplomacy_action::remove_from_sphere:
				return command::can_remove_from_sphere(state, state.local_player_nation, target, gp_target);
			case diplomacy_action::justify_war:
				return false;
			case diplomacy_action::command_units:
				return false;
			case diplomacy_action::cancel_command_units:
				return false;
			case diplomacy_action::make_peace:
				return false;
			}
		}
		return false;
	}
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "agree"));
	}

	void on_update(sys::state& state) noexcept override {
		disabled = !get_can_perform(state);
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto target = any_cast<dcon::nation_id>(payload);

			Cyto::Any gp_payload = gp_selection_query_data{};
			parent->impl_get(state, gp_payload);
			auto gp_target = any_cast<gp_selection_query_data>(gp_payload).data;

			switch(content) {
			case diplomacy_action::ally:
				command::ask_for_alliance(state, state.local_player_nation, target);
				break;
			case diplomacy_action::cancel_ally:
				command::cancel_alliance(state, state.local_player_nation, target);
				break;
			case diplomacy_action::call_ally:
				break;
			case diplomacy_action::declare_war:
				break;
			case diplomacy_action::military_access:
				command::ask_for_military_access(state, state.local_player_nation, target);
				break;
			case diplomacy_action::cancel_military_access:
				command::cancel_military_access(state, state.local_player_nation, target);
				break;
			case diplomacy_action::give_military_access:
				break;
			case diplomacy_action::cancel_give_military_access:
				command::cancel_given_military_access(state, state.local_player_nation, target);
				break;
			case diplomacy_action::increase_relations:
				command::increase_relations(state, state.local_player_nation, target);
				break;
			case diplomacy_action::decrease_relations:
				command::decrease_relations(state, state.local_player_nation, target);
				break;
			case diplomacy_action::war_subsidies:
				command::give_war_subsidies(state, state.local_player_nation, target);
				break;
			case diplomacy_action::cancel_war_subsidies:
				command::cancel_war_subsidies(state, state.local_player_nation, target);
				break;
			case diplomacy_action::discredit:
				command::discredit_advisors(state, state.local_player_nation, target, gp_target);
				break;
			case diplomacy_action::expel_advisors:
				command::expel_advisors(state, state.local_player_nation, target, gp_target);
				break;
			case diplomacy_action::ban_embassy:
				command::ban_embassy(state, state.local_player_nation, target, gp_target);
				break;
			case diplomacy_action::increase_opinion:
				command::increase_opinion(state, state.local_player_nation, target);
				break;
			case diplomacy_action::decrease_opinion:
				command::decrease_opinion(state, state.local_player_nation, target, gp_target);
				break;
			case diplomacy_action::add_to_sphere:
				command::add_to_sphere(state, state.local_player_nation, target);
				break;
			case diplomacy_action::remove_from_sphere:
				command::remove_from_sphere(state, state.local_player_nation, target, gp_target);
				break;
			case diplomacy_action::justify_war:
				break;
			case diplomacy_action::command_units:
				break;
			case diplomacy_action::cancel_command_units:
				break;
			case diplomacy_action::make_peace:
				break;
			}
			parent->set_visible(state, false);
		}
	}
};
class diplomacy_action_dialog_decline_button : public generic_close_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "decline"));
	}
};
class diplomacy_action_dialog_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "title") {
			return make_element_by_type<diplomacy_action_dialog_title_text>(state, id);
		} else if(name == "description") {
			return make_element_by_type<diplomacy_action_dialog_description_text>(state, id);
		} else if(name == "chance_string") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_action_dialog_agree_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<diplomacy_action_dialog_decline_button>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size;
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class diplomacy_action_gp_dialog_select_button : public flag_button {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			Cyto::Any s_payload = element_selection_wrapper<dcon::nation_id>{ content };
			parent->impl_get(state, s_payload);
		}
	}
};

class diplomacy_action_gp_dialog_select_window : public window_element_base {
public:
	uint8_t rank = 0;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(nations::get_nth_great_power(state, rank));
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

class diplomacy_gp_action_dialog_window : public window_element_base {
	dcon::nation_id selected_gp{};
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		xy_pair gp_base_select_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("gpselectoptionpos")->second.definition].position;
		xy_pair gp_select_offset = gp_base_select_offset;
		for(uint8_t i = 0; i < uint8_t(state.defines.great_nations_count); i++) {
			auto ptr = make_element_by_type<diplomacy_action_gp_dialog_select_window>(state, state.ui_state.defs_by_name.find("gpselectoption")->second.definition);
			ptr->base_data.position = gp_select_offset;
			ptr->rank = i;
			// Arrange in columns of 2 elements each...
			gp_select_offset.y += ptr->base_data.size.y;
			if(i != 0 && i % 2 == 1) {
				gp_select_offset.x += ptr->base_data.size.x;
				gp_select_offset.y = gp_base_select_offset.y;
			}
			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "title") {
			return make_element_by_type<diplomacy_action_dialog_title_text>(state, id);
		} else if(name == "description") {
			return make_element_by_type<diplomacy_action_dialog_description_text>(state, id);
		} else if(name == "chance_string") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "agreebutton") {
			return make_element_by_type<diplomacy_action_dialog_agree_button>(state, id);
		} else if(name == "declinebutton") {
			return make_element_by_type<diplomacy_action_dialog_decline_button>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<nation_player_flag>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size;
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::nation_id>>()) {
			selected_gp = any_cast<element_selection_wrapper<dcon::nation_id>>(payload).data;
			return message_result::consumed;
		} else if(payload.holds_type<gp_selection_query_data>()) {
			payload.emplace<gp_selection_query_data>(gp_selection_query_data{ selected_gp });
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

template<typename T>
class diplomacy_action_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "action_option") {
			return make_element_by_type<T>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
