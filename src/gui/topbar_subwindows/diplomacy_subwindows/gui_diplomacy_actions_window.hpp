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
};

class diplomacy_action_ally_button : public generic_settable_element<button_element_base, dcon::nation_id> {
    bool can_cancel(sys::state& state) noexcept {
        auto drid = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, content);
        return state.world.diplomatic_relation_get_are_allied(drid);
    }
public:
    void on_update(sys::state& state) noexcept override {
        set_button_text(state, text::produce_simple_string(state,
            can_cancel(state) ? "cancelalliance_button"
                : "alliance_button"));

        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = can_cancel(state) ? diplomacy_action::cancel_ally
                : diplomacy_action::ally;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    can_cancel(state) ? text::localised_format_box(state, contents, box, std::string_view("cancelalliance_desc")) : text::localised_format_box(state, contents, box, std::string_view("alliance_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);
		    
            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map); // Always return 0, that way leafs ai ambititions will be stunted :3
            // leaf: Of course this will never occur because the AI would absolutely obliterate the player in 1v1
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_call_ally_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "callally_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else {
            auto drid = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, content);
            disabled = !state.world.diplomatic_relation_get_are_allied(drid);
        }
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::call_ally;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    text::localised_format_box(state, contents, box, std::string_view("remove_callally_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);
		    
            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_military_access_button : public generic_settable_element<button_element_base, dcon::nation_id> {
    bool can_cancel(sys::state& state) noexcept {
	    // TODO - test if we local_player_nation has military access to the other country
        return false;
    }
public:
    void on_update(sys::state& state) noexcept override {
        set_button_text(state, text::produce_simple_string(state,
            can_cancel(state) ? "cancelaskmilitaryaccess_button"
                : "askmilitaryaccess_button"));

        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        bool can_cancel = false;
        if(parent) {
            Cyto::Any payload = can_cancel ? diplomacy_action::cancel_military_access
                : diplomacy_action::military_access;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    can_cancel(state) ? text::localised_format_box(state, contents, box, std::string_view("cancelaskmilitaryaccess_desc")) : text::localised_format_box(state, contents, box, std::string_view("askmilitaryaccess_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);
		    
            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_give_military_access_button : public generic_settable_element<button_element_base, dcon::nation_id> {
    bool can_cancel(sys::state& state) noexcept {
	    // TODO - test if we local_player_nation has military access to the other country
        return false;
    }
public:
    void on_update(sys::state& state) noexcept override {
        set_button_text(state, text::produce_simple_string(state,
            can_cancel(state) ? "cancelgivemilitaryaccess_button"
                : "givemilitaryaccess_button"));

        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        bool can_cancel = false;
        if(parent) {
            Cyto::Any payload = can_cancel ? diplomacy_action::cancel_give_military_access
                : diplomacy_action::give_military_access;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    can_cancel(state) ? text::localised_format_box(state, contents, box, std::string_view("cancelgivemilitaryaccess_desc")) : text::localised_format_box(state, contents, box, std::string_view("givemilitaryaccess_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);
		    
            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_increase_relations_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "increaserelation_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::increase_relations;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    text::localised_format_box(state, contents, box, std::string_view("increaserelation_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);
		    
            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_decrease_relations_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "decreaserelation_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::decrease_relations;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    text::localised_format_box(state, contents, box, std::string_view("decreaserelation_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);
		    
            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_war_subisides_button : public generic_settable_element<button_element_base, dcon::nation_id> {
    bool can_cancel(sys::state& state) noexcept {
	    // TODO - test if we local_player_nation has military access to the other country
        return false;
    }
public:
    void on_update(sys::state& state) noexcept override {
        set_button_text(state, text::produce_simple_string(state,
            can_cancel(state) ? "cancel_warsubsidies_button"
                : "warsubsidies_button"));

        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        bool can_cancel = false;
        if(parent) {
            Cyto::Any payload = can_cancel ? diplomacy_action::cancel_war_subsidies
                : diplomacy_action::war_subsidies;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    can_cancel(state) ? text::localised_format_box(state, contents, box, std::string_view("cancel_warsubsidies_desc")) : text::localised_format_box(state, contents, box, std::string_view("warsubsidies_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);

		    text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_declare_war_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "war_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::declare_war;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    text::localised_format_box(state, contents, box, std::string_view("act_wardesc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);

		    text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_command_units_button : public generic_settable_element<button_element_base, dcon::nation_id> {
    bool can_cancel(sys::state& state) noexcept {
	    // TODO - test if we local_player_nation has military access to the other country
        return false;
    }
public:
    void on_update(sys::state& state) noexcept override {
        set_button_text(state, text::produce_simple_string(state,
            can_cancel(state) ? "cancel_unit_command_button"
                : "give_unit_command_button"));

        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        bool can_cancel = false;
        if(parent) {
            Cyto::Any payload = can_cancel ? diplomacy_action::cancel_command_units
                : diplomacy_action::command_units;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    can_cancel(state) ? text::localised_format_box(state, contents, box, std::string_view("cancel_unit_command_desc")) : text::localised_format_box(state, contents, box, std::string_view("give_unit_command_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);

            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_discredit_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "discredit_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else if(!nations::is_great_power(state, state.local_player_nation))
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::discredit;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_expel_advisors_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "expeladvisors_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else if(!nations::is_great_power(state, state.local_player_nation))
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::expel_advisors;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_ban_embassy_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "banembassy_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else if(!nations::is_great_power(state, state.local_player_nation))
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::ban_embassy;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_increase_opinion_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "increaseopinion_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else if(!nations::is_great_power(state, state.local_player_nation))
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::increase_opinion;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_decrease_opinion_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "decreaseopinion_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else if(!nations::is_great_power(state, state.local_player_nation))
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::decrease_opinion;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_add_to_sphere_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "addtosphere_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else if(!nations::is_great_power(state, state.local_player_nation))
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::add_to_sphere;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_remove_from_sphere_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "removefromsphere_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
        else if(!nations::is_great_power(state, state.local_player_nation))
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = diplomacy_action::remove_from_sphere;
            parent->impl_get(state, payload);
        }
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
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
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
    }
};

class diplomacy_action_justify_war_button : public generic_settable_element<button_element_base, dcon::nation_id> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "make_cb_button"));
    }

    void on_update(sys::state& state) noexcept override {
        // TODO: Conditions for enabling/disabling
        disabled = false;
        if(content == state.local_player_nation)
            disabled = true;
    }

    void button_action(sys::state& state) noexcept override {
        // TODO: button action
    }

    message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
	    return message_result::consumed;
    }

    tooltip_behavior has_tooltip(sys::state& state) noexcept override {
	    return tooltip_behavior::variable_tooltip;
    }

    void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
	    auto box = text::open_layout_box(contents, 0);
	    text::localised_format_box(state, contents, box, std::string_view("make_cb_desc"));
	    text::add_divider_to_layout_box(state, contents, box);
	    if(content == state.local_player_nation) {
		    text::localised_format_box(state, contents, box, std::string_view("act_no_self"));
	    } else {
		    text::localised_format_box(state, contents, box, std::string_view("dip_enough_diplo"));
		    text::add_line_break_to_layout_box(contents, state, box);
		    
            text::substitution_map ai_map{};
            text::add_to_substitution_map(ai_map, text::variable_type::country, std::string_view(text::produce_simple_string(state, state.world.nation_get_name(content))));
		    text::localised_format_box(state, contents, box, std::string_view("diplomacy_ai_acceptance"), ai_map);
	    }
	    text::close_layout_box(contents, box);
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
        }
        return "";
    }
public:
    void on_update(sys::state& state) noexcept override {
        set_text(state, text::produce_simple_string(state, get_title_key()));
    }
};
class diplomacy_action_dialog_agree_button : public generic_settable_element<button_element_base, diplomacy_action> {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "agree"));
    }

    void button_action(sys::state& state) noexcept override {
        if (parent)
            parent->set_visible(state, false);
        // TODO: agree action
    }
};
class diplomacy_action_dialog_decline_button : public button_element_base {
public:
    void on_create(sys::state& state) noexcept override {
        button_element_base::on_create(state);
        set_button_text(state, text::produce_simple_string(state, "decline"));
    }

    void button_action(sys::state& state) noexcept override {
        if (parent)
            parent->set_visible(state, false);
    }
};
// Player's flag
class diplomacy_action_dialog_left_flag_image : public flag_button {
public:
    dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
        return state.world.nation_get_identity_from_identity_holder(state.local_player_nation);
    }

    void on_update(sys::state& state) noexcept override {
        flag_button::set_current_nation(state, get_current_nation(state));
    }
};
// Country's flag
class diplomacy_action_dialog_right_flag_image : public generic_settable_element<flag_button, dcon::nation_id> {
public:
    dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
        return state.world.nation_get_identity_from_identity_holder(content);
    }

    void on_update(sys::state& state) noexcept override {
        flag_button::set_current_nation(state, get_current_nation(state));
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
			return make_element_by_type<diplomacy_action_dialog_left_flag_image>(state, id);
        } else if(name == "rightshield") {
            return make_element_by_type<diplomacy_action_dialog_right_flag_image>(state, id);
        } else if(name == "background") {
            auto ptr = make_element_by_type<opaque_element_base>(state, id);
            ptr->base_data.size = base_data.size;
            return ptr;
        } else {
            return nullptr;
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

	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = nations::get_nth_great_power(state, rank);
		impl_set(state, payload);
	}
};

class diplomacy_gp_action_dialog_window : public window_element_base {
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
			return make_element_by_type<diplomacy_action_dialog_left_flag_image>(state, id);
        } else if(name == "rightshield") {
            return make_element_by_type<diplomacy_action_dialog_right_flag_image>(state, id);
        } else if(name == "background") {
            auto ptr = make_element_by_type<opaque_element_base>(state, id);
            ptr->base_data.size = base_data.size;
            return ptr;
        } else {
            return nullptr;
        }
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
