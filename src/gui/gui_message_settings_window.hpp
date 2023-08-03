#pragma once

#include "constants.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

static std::string get_setting_text_key(sys::message_setting_type type) {
	static char const* key_str[] = {
		"msg_revolt",								  // revolt
		"msg_war_on_nation",						  // war_on_nation
		"msg_war_by_nation",						  // war_by_nation
		"msg_wargoal_added",						  // wargoal_added
		"msg_siegeover_by_nation",					  // siegeover_by_nation
		"msg_siegeover_on_nation",					  // siegeover_on_nation
		"msg_colony_finished",						  // colony_finished
		"msg_reform_gained",						  // reform_gained
		"msg_reform_lost",							  // reform_lost
		"msg_ruling_party_change",					  // ruling_party_change
		"msg_upperhouse",							  // upperhouse
		"msg_electionstart",						  // electionstart
		"msg_electiondone",							  // electiondone
		"msg_breakcountry",							  // breakcountry
		"msg_peace_accepted_from_nation",			  // peace_accepted_from_nation
		"msg_peace_rejected_from_nation",			  // peace_rejected_from_nation
		"msg_peace_accepted_by_nation",				  // peace_accepted_by_nation
		"msg_peace_rejected_by_nation",				  // peace_rejected_by_nation
		"msg_mobilization_start",					  // mobilization_start
		"msg_mobilization_end",						  // mobilization_end
		"msg_factory_complete",						  // factory_complete
		"msg_rr_complete",							  // rr_complete
		"msg_fort_complete",						  // fort_complete
		"msg_naval_base_complete",					  // naval_base_complete
		"msg_province_event",						  // province_event
		"msg_national_event",						  // national_event
		"msg_major_event",							  // major_event
		"msg_invention",							  // invention
		"msg_tech",									  // tech
		"msg_leader_dies",							  // leader_dies
		"msg_land_combat_starts_on_nation",			  // land_combat_starts_on_nation
		"msg_naval_combat_starts_on_nation",		  // naval_combat_starts_on_nation
		"msg_land_combat_starts_by_nation",			  // land_combat_starts_by_nation
		"msg_naval_combat_starts_by_nation",		  // naval_combat_starts_by_nation
		"msg_movement_finishes",					  // movement_finishes
		"msg_decision",								  // decision
		"msg_lose_great_power",						  // lose_great_power
		"msg_become_great_power",					  // become_great_power
		"msg_war_subsidies_start_by_nation",		  // war_subsidies_start_by_nation
		"msg_war_subsidies_start_on_nation",		  // war_subsidies_start_on_nation
		"msg_war_subsidies_end_by_nation",			  // war_subsidies_end_by_nation
		"msg_war_subsidies_end_on_nation",			  // war_subsidies_end_on_nation
		"msg_reparations_start_by_nation",			  // reparations_start_by_nation
		"msg_reparations_start_on_nation",			  // reparations_start_on_nation
		"msg_reparations_end_by_nation",			  // reparations_end_by_nation
		"msg_reparations_end_on_nation",			  // reparations_end_on_nation
		"msg_mil_access_start_by_nation",			  // mil_access_start_by_nation
		"msg_mil_access_start_on_nation",			  // mil_access_start_on_nation
		"msg_mil_access_end_by_nation",				  // mil_access_end_by_nation
		"msg_mil_access_end_on_nation",				  // mil_access_end_on_nation
		"msg_mil_access_declined_by_nation",		  // mil_access_declined_by_nation
		"msg_mil_access_declined_on_nation",		  // mil_access_declined_on_nation
		"msg_alliance_starts",						  // alliance_starts
		"msg_alliance_ends",						  // alliance_ends
		"msg_alliance_declined_by_nation",			  // alliance_declined_by_nation
		"msg_alliance_declined_on_nation",			  // alliance_declined_on_nation
		"msg_ally_called_accepted_by_nation",		  // ally_called_accepted_by_nation
		"msg_ally_called_declined_by_nation",		  // ally_called_declined_by_nation
		"msg_discredit_by_nation",					  // discredit_by_nation
		"msg_ban_by_nation",						  // ban_by_nation
		"msg_expell_by_nation",						  // expell_by_nation
		"msg_discredit_on_nation",					  // discredit_on_nation
		"msg_ban_on_nation",						  // ban_on_nation
		"msg_expell_on_nation",						  // expell_on_nation
		"msg_increase_opinion",						  // increase_opinion
		"msg_decrease_opinion_by_nation",			  // decrease_opinion_by_nation
		"msg_decrease_opinion_on_nation",			  // decrease_opinion_on_nation
		"msg_rem_sphere_by_nation",					  // rem_sphere_by_nation
		"msg_rem_sphere_on_nation",					  // rem_sphere_on_nation
		"msg_removed_from_sphere",					  // removed_from_sphere
		"msg_add_sphere",							  // add_sphere
		"msg_added_to_sphere",						  // added_to_sphere
		"msg_increase_relation_by_nation",			  // increase_relation_by_nation
		"msg_increase_relation_on_nation",			  // increase_relation_on_nation
		"msg_decrease_relation_by_nation",			  // decrease_relation_by_nation
		"msg_decrease_relation_on_nation",			  // decrease_relation_on_nation
		"msg_join_war_by_nation",					  // join_war_by_nation
		"msg_join_war_on_nation",					  // join_war_on_nation
		"msg_gw_unlocked",							  // gw_unlocked
		"msg_war_becomes_great",					  // war_becomes_great
		"msg_cb_detected_on_nation",				  // cb_detected_on_nation
		"msg_cb_detected_by_nation",				  // cb_detected_by_nation
		"msg_crisis_join_offer_accepted_by_nation",	  // crisis_join_offer_accepted_by_nation
		"msg_crisis_join_offer_declined_by_nation",	  // crisis_join_offer_declined_by_nation
		"msg_crisis_join_offer_accepted_from_nation", // crisis_join_offer_accepted_from_nation
		"msg_crisis_join_offer_declined_from_nation", // crisis_join_offer_declined_from_nation
		"msg_crisis_resolution_accepted",			  // crisis_resolution_accepted
		"msg_crisis_becomes_war",					  // crisis_becomes_war
		"msg_crisis_resolution_declined_from_nation", // crisis_resolution_declined_from_nation
		"msg_crisis_starts",						  // crisis_starts
		"msg_crisis_attacker_backer",				  // crisis_attacker_backer
		"msg_crisis_defender_backer",				  // crisis_defender_backer
		"msg_crisis_fizzle",						  // crisis_fizzle
		"msg_war_join_by",							  // war_join_by
		"msg_war_join_on",							  // war_join_on
		"msg_cb_fab_finished",						  // cb_fab_finished
		"msg_cb_fab_cancelled",						  // cb_fab_cancelled
		"msg_crisis_voluntary_join",				  // crisis_voluntary_join
	};
	return std::string{key_str[static_cast< int>(type)]};
}

class message_settings_item : public listbox_row_element_base<sys::message_setting_type> {
	simple_text_element_base* text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override { text->set_text(state, text::produce_simple_string(state, get_setting_text_key(content))); }
};

class message_settings_listbox : public listbox_element_base<message_settings_item, sys::message_setting_type> {
protected:
	std::string_view get_row_element_name() override { return "message_entry"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto i = 0; i != uint8_t(sys::message_setting_type::count); ++i) {
			row_contents.push_back(sys::message_setting_type(i));
		}
		update(state);
	}
};

enum class message_settings_category : uint8_t { all, combat, diplomacy, units, provinces, events, others, count };

class message_settings_window : public generic_tabbed_window<message_settings_category> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window<message_settings_category>::on_create(state);
		base_data.position.y -= 21; // Nudge
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "settings") {
			// Nudge required for listbox before it is created...
			state.ui_defs.gui[id].size.x -= 21; // Nudge
			auto ptr = make_element_by_type<message_settings_listbox>(state, id);
			ptr->base_data.position.x += 21; // Nudge
			ptr->base_data.position.y += 21; // Nudge
			return ptr;
		} else if(name == "messagecat_all") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::all;
			return ptr;
		} else if(name == "messagecat_combat") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::combat;
			return ptr;
		} else if(name == "messagecat_diplomacy") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::diplomacy;
			return ptr;
		} else if(name == "messagecat_units") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::units;
			return ptr;
		} else if(name == "messagecat_provinces") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::provinces;
			return ptr;
		} else if(name == "messagecat_events") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::events;
			return ptr;
		} else if(name == "messagecat_others") {
			auto ptr = make_element_by_type<generic_tab_button<message_settings_category>>(state, id);
			ptr->target = message_settings_category::others;
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<message_settings_category>()) {
			auto content = any_cast<message_settings_category>(payload);
			active_tab = content;
			switch(content) {
			case message_settings_category::all:
				break;
			case message_settings_category::combat:
				break;
			case message_settings_category::diplomacy:
				break;
			case message_settings_category::units:
				break;
			case message_settings_category::provinces:
				break;
			case message_settings_category::events:
				break;
			case message_settings_category::others:
				break;
			default:
				break;
			};
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class message_log_text : public multiline_button_element_base {
public:
	void on_update(sys::state& state) noexcept override;
};

class message_log_entry : public listbox_row_element_base<int32_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "messagelogbutton") {
			return make_element_by_type<message_log_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class message_log_listbox : public listbox_element_base<message_log_entry, int32_t> {
protected:
	std::string_view get_row_element_name() override { return "logtext"; }
};

template<message_settings_category Filter> class message_log_filter_checkbox : public checkbox_button {
	static std::string_view get_filter_text_key(message_settings_category f) noexcept {
		switch(f) {
		case message_settings_category::all:
			return "messagesettings_all";
		case message_settings_category::combat:
			return "messagesettings_combat";
		case message_settings_category::diplomacy:
			return "messagesettings_diplomacy";
		case message_settings_category::units:
			return "messagesettings_units";
		case message_settings_category::provinces:
			return "messagesettings_provinces";
		case message_settings_category::events:
			return "messagesettings_events";
		case message_settings_category::others:
			return "messagesettings_other";
		default:
			return "???";
		}
	}

public:
	bool is_active(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = Filter;
			parent->impl_get(state, payload);
			return any_cast<bool>(payload);
		}
		return false;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<message_settings_category>{Filter};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, get_filter_text_key(Filter));
		text::close_layout_box(contents, box);
	}
};

class message_log_window : public window_element_base {
	std::vector<bool> cat_filters;
	message_log_listbox* log_list = nullptr;

public:
	std::vector<notification::message> messages;

	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

#undef MSG_SETTING_LIST
} // namespace ui
