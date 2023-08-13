#pragma once

#include "constants.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

inline std::string get_setting_text_key(int32_t type) {
	static char const* key_str[] = {
		"amsg_revolt",								  // revolt
		"amsg_war_on_nation",						  // war_on_nation
		"amsg_war_by_nation",						  // war_by_nation
		"amsg_wargoal_added",						  // wargoal_added
		"amsg_siegeover_by_nation",					  // siegeover_by_nation
		"amsg_siegeover_on_nation",					  // siegeover_on_nation
		"amsg_colony_finished",						  // colony_finished
		"amsg_reform_gained",						  // reform_gained
		"amsg_reform_lost",							  // reform_lost
		"amsg_ruling_party_change",					  // ruling_party_change
		"amsg_upperhouse",							  // upperhouse
		"amsg_electionstart",						  // electionstart
		"amsg_electiondone",							  // electiondone
		"amsg_breakcountry",							  // breakcountry
		"amsg_peace_accepted_from_nation",			  // peace_accepted_from_nation
		"amsg_peace_rejected_from_nation",			  // peace_rejected_from_nation
		"amsg_peace_accepted_by_nation",				  // peace_accepted_by_nation
		"amsg_peace_rejected_by_nation",				  // peace_rejected_by_nation
		"amsg_mobilization_start",					  // mobilization_start
		"amsg_mobilization_end",						  // mobilization_end
		"amsg_factory_complete",						  // factory_complete
		"amsg_rr_complete",							  // rr_complete
		"amsg_fort_complete",						  // fort_complete
		"amsg_naval_base_complete",					  // naval_base_complete
		"amsg_province_event",						  // province_event
		"amsg_national_event",						  // national_event
		"amsg_major_event",							  // major_event
		"amsg_invention",							  // invention
		"amsg_tech",									  // tech
		"amsg_leader_dies",							  // leader_dies
		"amsg_land_combat_starts_on_nation",			  // land_combat_starts_on_nation
		"amsg_naval_combat_starts_on_nation",		  // naval_combat_starts_on_nation
		"amsg_land_combat_starts_by_nation",			  // land_combat_starts_by_nation
		"amsg_naval_combat_starts_by_nation",		  // naval_combat_starts_by_nation
		"amsg_movement_finishes",					  // movement_finishes
		"amsg_decision",								  // decision
		"amsg_lose_great_power",						  // lose_great_power
		"amsg_become_great_power",					  // become_great_power
		"amsg_war_subsidies_start_by_nation",		  // war_subsidies_start_by_nation
		"amsg_war_subsidies_start_on_nation",		  // war_subsidies_start_on_nation
		"amsg_war_subsidies_end_by_nation",			  // war_subsidies_end_by_nation
		"amsg_war_subsidies_end_on_nation",			  // war_subsidies_end_on_nation
		"amsg_reparations_start_by_nation",			  // reparations_start_by_nation
		"amsg_reparations_start_on_nation",			  // reparations_start_on_nation
		"amsg_reparations_end_by_nation",			  // reparations_end_by_nation
		"amsg_reparations_end_on_nation",			  // reparations_end_on_nation
		"amsg_mil_access_start_by_nation",			  // mil_access_start_by_nation
		"amsg_mil_access_start_on_nation",			  // mil_access_start_on_nation
		"amsg_mil_access_end_by_nation",				  // mil_access_end_by_nation
		"amsg_mil_access_end_on_nation",				  // mil_access_end_on_nation
		"amsg_mil_access_declined_by_nation",		  // mil_access_declined_by_nation
		"amsg_mil_access_declined_on_nation",		  // mil_access_declined_on_nation
		"amsg_alliance_starts",						  // alliance_starts
		"amsg_alliance_ends",						  // alliance_ends
		"amsg_alliance_declined_by_nation",			  // alliance_declined_by_nation
		"amsg_alliance_declined_on_nation",			  // alliance_declined_on_nation
		"amsg_ally_called_accepted_by_nation",		  // ally_called_accepted_by_nation
		"amsg_ally_called_declined_by_nation",		  // ally_called_declined_by_nation
		"amsg_discredit_by_nation",					  // discredit_by_nation
		"amsg_ban_by_nation",						  // ban_by_nation
		"amsg_expell_by_nation",						  // expell_by_nation
		"amsg_discredit_on_nation",					  // discredit_on_nation
		"amsg_ban_on_nation",						  // ban_on_nation
		"amsg_expell_on_nation",						  // expell_on_nation
		"amsg_increase_opinion",						  // increase_opinion
		"amsg_decrease_opinion_by_nation",			  // decrease_opinion_by_nation
		"amsg_decrease_opinion_on_nation",			  // decrease_opinion_on_nation
		"amsg_rem_sphere_by_nation",					  // rem_sphere_by_nation
		"amsg_rem_sphere_on_nation",					  // rem_sphere_on_nation
		"amsg_removed_from_sphere",					  // removed_from_sphere
		"amsg_add_sphere",							  // add_sphere
		"amsg_added_to_sphere",						  // added_to_sphere
		"amsg_increase_relation_by_nation",			  // increase_relation_by_nation
		"amsg_increase_relation_on_nation",			  // increase_relation_on_nation
		"amsg_decrease_relation_by_nation",			  // decrease_relation_by_nation
		"amsg_decrease_relation_on_nation",			  // decrease_relation_on_nation
		"amsg_join_war_by_nation",					  // join_war_by_nation
		"amsg_join_war_on_nation",					  // join_war_on_nation
		"amsg_gw_unlocked",							  // gw_unlocked
		"amsg_war_becomes_great",					  // war_becomes_great
		"amsg_cb_detected_on_nation",				  // cb_detected_on_nation
		"amsg_cb_detected_by_nation",				  // cb_detected_by_nation
		"amsg_crisis_join_offer_accepted_by_nation",	  // crisis_join_offer_accepted_by_nation
		"amsg_crisis_join_offer_declined_by_nation",	  // crisis_join_offer_declined_by_nation
		"amsg_crisis_join_offer_accepted_from_nation", // crisis_join_offer_accepted_from_nation
		"amsg_crisis_join_offer_declined_from_nation", // crisis_join_offer_declined_from_nation
		"amsg_crisis_resolution_accepted",			  // crisis_resolution_accepted
		"amsg_crisis_becomes_war",					  // crisis_becomes_war
		"amsg_crisis_resolution_declined_from_nation", // crisis_resolution_declined_from_nation
		"amsg_crisis_starts",						  // crisis_starts
		"amsg_crisis_attacker_backer",				  // crisis_attacker_backer
		"amsg_crisis_defender_backer",				  // crisis_defender_backer
		"amsg_crisis_fizzle",						  // crisis_fizzle
		"amsg_war_join_by",							  // war_join_by
		"amsg_war_join_on",							  // war_join_on
		"amsg_cb_fab_finished",						  // cb_fab_finished
		"amsg_cb_fab_cancelled",						  // cb_fab_cancelled
		"amsg_crisis_voluntary_join",				  // crisis_voluntary_join
	};
	return std::string{key_str[type]};
}

struct message_setting_changed_notification { };

template<int32_t category>
class message_handling_buton : public right_click_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto base_index = retrieve<int32_t>(state, parent);
		uint8_t current = sys::message_response::ignore;
		if constexpr(category == 0) {
			current = state.user_settings.self_message_settings[base_index];
		} else if constexpr(category == 1) {
			current = state.user_settings.interesting_message_settings[base_index];
		} else if constexpr(category == 2) {
			current = state.user_settings.other_message_settings[base_index];
		}

		if(current == sys::message_response::standard_pause) {
			frame = 3;
		} else  if(current == sys::message_response::standard_popup) {
			frame = 2;
		} else  if(current == sys::message_response::log) {
			frame = 1;
		} else  if(current == sys::message_response::ignore) {
			frame = 0;
		} else {
			frame = 0;
		}
	}
	void button_action(sys::state& state) noexcept override {
		auto base_index = retrieve<int32_t>(state, parent);
		uint8_t current = sys::message_response::ignore;
		if constexpr(category == 0) {
			current = state.user_settings.self_message_settings[base_index];
		} else if constexpr(category == 1) {
			current = state.user_settings.interesting_message_settings[base_index];
		} else if constexpr(category == 2) {
			current = state.user_settings.other_message_settings[base_index];
		}

		if(current == sys::message_response::standard_pause) {
			current = sys::message_response::ignore;
		} else  if(current == sys::message_response::standard_popup) {
			current = sys::message_response::standard_pause;
		} else  if(current == sys::message_response::log) {
			current = sys::message_response::standard_popup;
		} else  if(current == sys::message_response::ignore) {
			current = sys::message_response::log;
		} else {
			current = sys::message_response::ignore;
		}

		if constexpr(category == 0) {
			 state.user_settings.self_message_settings[base_index] = current;
		} else if constexpr(category == 1) {
			state.user_settings.interesting_message_settings[base_index] = current;
		} else if constexpr(category == 2) {
			state.user_settings.other_message_settings[base_index] = current;
		}

		send(state, parent, message_setting_changed_notification{});
		on_update(state);
	}
	void button_right_action(sys::state& state) noexcept override {
		auto base_index = retrieve<int32_t>(state, parent);
		uint8_t current = sys::message_response::ignore;
		if constexpr(category == 0) {
			current = state.user_settings.self_message_settings[base_index];
		} else if constexpr(category == 1) {
			current = state.user_settings.interesting_message_settings[base_index];
		} else if constexpr(category == 2) {
			current = state.user_settings.other_message_settings[base_index];
		}

		if(current == sys::message_response::standard_pause) {
			current = sys::message_response::standard_popup;
		} else  if(current == sys::message_response::standard_popup) {
			current = sys::message_response::log;
		} else  if(current == sys::message_response::log) {
			current = sys::message_response::ignore;
		} else  if(current == sys::message_response::ignore) {
			current = sys::message_response::standard_pause;
		} else {
			current = sys::message_response::ignore;
		}

		if constexpr(category == 0) {
			state.user_settings.self_message_settings[base_index] = current;
		} else if constexpr(category == 1) {
			state.user_settings.interesting_message_settings[base_index] = current;
		} else if constexpr(category == 2) {
			state.user_settings.other_message_settings[base_index] = current;
		}

		send(state, parent, message_setting_changed_notification{});
		on_update(state);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto base_index = retrieve<int32_t>(state, parent);
		uint8_t current = sys::message_response::ignore;
		if constexpr(category == 0) {
			current = state.user_settings.self_message_settings[base_index];
		} else if constexpr(category == 1) {
			current = state.user_settings.interesting_message_settings[base_index];
		} else if constexpr(category == 2) {
			current = state.user_settings.other_message_settings[base_index];
		}
		if(current == sys::message_response::standard_pause) {
			text::add_line(state, contents, "msg_handling_pause");
		} else  if(current == sys::message_response::standard_popup) {
			text::add_line(state, contents, "msg_handling_popup");
		} else  if(current == sys::message_response::log) {
			text::add_line(state, contents, "msg_handling_log");
		} else  if(current == sys::message_response::ignore) {
			text::add_line(state, contents, "msg_handling_ignore");
		}
	}
};

class message_type_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto base_index = retrieve<int32_t>(state, parent);
		set_text(state, text::produce_simple_string(state, get_setting_text_key(base_index)));
	}
};

class message_settings_item : public listbox_row_element_base<int32_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "entry_text") {
			return make_element_by_type<message_type_name>(state, id);
		} else if(name == "us_button") {
			return make_element_by_type<message_handling_buton<0>>(state, id);
		} else if(name == "interesting_button") {
			return make_element_by_type<message_handling_buton<1>>(state, id);
		} else if(name == "other_button") {
			return make_element_by_type<message_handling_buton<2>>(state, id);
		} else {
			return nullptr;
		}
	}
};

class message_settings_listbox : public listbox_element_base<message_settings_item, int32_t> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_message_setting_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto i = 0; i < 98; ++i) {
			row_contents.push_back(i);
		}
		update(state);
	}
};


class message_settings_window : public window_element_base {
public:
	bool settings_changed = false;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	void on_hide(sys::state& state) noexcept override {
		if(settings_changed) {
			settings_changed = false;
			state.save_user_settings();
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "message_settings_items") {
			// Nudge required for listbox before it is created...
			//state.ui_defs.gui[id].size.x -= 21; // Nudge
			auto ptr = make_element_by_type<message_settings_listbox>(state, id);
			//ptr->base_data.position.x += 21; // Nudge
			//ptr->base_data.position.y += 21; // Nudge
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<message_setting_changed_notification>()) {
			settings_changed = true;
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
	std::string_view get_row_element_name() override {
		return "logtext";
	}
};

enum class message_settings_category : uint8_t {
	all, combat, diplomacy, units, provinces, events, others, count
};

template<message_settings_category Filter>
class message_log_filter_checkbox : public checkbox_button {
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

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
