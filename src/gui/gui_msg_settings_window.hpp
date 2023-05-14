#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

#define MSG_SETTING_LIST \
	MSG_SETTING_ITEM(gaincb, 0) \
	MSG_SETTING_ITEM(gaincb_target, 1) \
	MSG_SETTING_ITEM(losecb, 2) \
	MSG_SETTING_ITEM(losecb_target, 3) \
	MSG_SETTING_ITEM(revoltalliance_cause, 4) \
	MSG_SETTING_ITEM(revoltalliance_target, 5) \
	MSG_SETTING_ITEM(war_cause, 6) \
	MSG_SETTING_ITEM(war_target, 7) \
	MSG_SETTING_ITEM(wargoal_cause, 8) \
	MSG_SETTING_ITEM(wargoal_target, 9) \
	MSG_SETTING_ITEM(socreform, 10) \
	MSG_SETTING_ITEM(socreform_bad, 11) \
	MSG_SETTING_ITEM(polreform, 12) \
	MSG_SETTING_ITEM(polreform_bad, 13) \
	MSG_SETTING_ITEM(economicreform, 14) \
	MSG_SETTING_ITEM(economicreform_bad, 15) \
	MSG_SETTING_ITEM(militaryreform, 16) \
	MSG_SETTING_ITEM(militaryreform_bad, 17) \
	MSG_SETTING_ITEM(setparty, 18) \
	MSG_SETTING_ITEM(upperhouse, 19) \
	MSG_SETTING_ITEM(electionstart, 20) \
	MSG_SETTING_ITEM(electiondone, 21) \
	MSG_SETTING_ITEM(breakcountry, 22) \
	MSG_SETTING_ITEM(rebels, 23) \
	MSG_SETTING_ITEM(annex_cause, 24) \
	MSG_SETTING_ITEM(annex_target, 25) \
	MSG_SETTING_ITEM(peace_accept_cause, 26) \
	MSG_SETTING_ITEM(peace_accept_target, 27) \
	MSG_SETTING_ITEM(peace_decline_cause, 28) \
	MSG_SETTING_ITEM(peace_decline_target, 29) \
	MSG_SETTING_ITEM(mobilize, 30) \
	MSG_SETTING_ITEM(demobilize, 31) \
	MSG_SETTING_ITEM(building_done, 32) \
	MSG_SETTING_ITEM(eventhappenother, 33) \
	MSG_SETTING_ITEM(eventhappenotheroption, 34) \
	MSG_SETTING_ITEM(majoreventhappenother, 35) \
	MSG_SETTING_ITEM(majoreventhappenotheroption, 36) \
	MSG_SETTING_ITEM(invention, 37) \
	MSG_SETTING_ITEM(tech, 38) \
	MSG_SETTING_ITEM(leaderdied, 39) \
	MSG_SETTING_ITEM(landbattleover, 40) \
	MSG_SETTING_ITEM(navalbattleover, 41) \
	MSG_SETTING_ITEM(decisionother, 42) \
	MSG_SETTING_ITEM(no_longer_great_power, 43) \
	MSG_SETTING_ITEM(became_great_power, 44) \
	MSG_SETTING_ITEM(alliance_accept_cause, 45) \
	MSG_SETTING_ITEM(alliance_accept_target, 46) \
	MSG_SETTING_ITEM(alliance_decline_cause, 47) \
	MSG_SETTING_ITEM(alliance_decline_target, 48) \
	MSG_SETTING_ITEM(cancelalliance_cause, 49) \
	MSG_SETTING_ITEM(cancelalliance_target, 50) \
	MSG_SETTING_ITEM(increaseopinion_cause, 51) \
	MSG_SETTING_ITEM(increaseopinion_target, 52) \
	MSG_SETTING_ITEM(addtosphere_cause, 53) \
	MSG_SETTING_ITEM(addtosphere_target, 54) \
	MSG_SETTING_ITEM(removefromsphere_cause, 55) \
	MSG_SETTING_ITEM(removefromsphere_target, 56) \
	MSG_SETTING_ITEM(removefromsphere_other_target, 57) \
	MSG_SETTING_ITEM(increaserelation_cause, 58) \
	MSG_SETTING_ITEM(increaserelation_target, 59) \
	MSG_SETTING_ITEM(decreaserelation_cause, 60) \
	MSG_SETTING_ITEM(decreaserelation_target, 61) \
	MSG_SETTING_ITEM(intervention_same_side, 62) \
	MSG_SETTING_ITEM(intervention_other_side, 63) \
	MSG_SETTING_ITEM(intervention, 64) \
	MSG_SETTING_ITEM(bankruptcy, 65) \
	MSG_SETTING_ITEM(new_party_avail, 66) \
	MSG_SETTING_ITEM(party_unavail, 67) \
	MSG_SETTING_ITEM(cb_detected_cause, 68) \
	MSG_SETTING_ITEM(cb_detected_target, 69) \
	MSG_SETTING_ITEM(cb_justify_no_longer_valid, 70) \
	MSG_SETTING_ITEM(join_crisis_offer_cause, 71) \
	MSG_SETTING_ITEM(join_crisis_offer_target, 72) \
	MSG_SETTING_ITEM(join_crisis_offer_reject_cause, 73) \
	MSG_SETTING_ITEM(join_crisis_offer_reject_target, 74) \
	MSG_SETTING_ITEM(join_crisis_cause, 75) \
	MSG_SETTING_ITEM(join_crisis_target, 76) \
	MSG_SETTING_ITEM(crisis_offer_cause, 77) \
	MSG_SETTING_ITEM(crisis_offer_target, 78) \
	MSG_SETTING_ITEM(crisis_offer_declined_cause, 79) \
	MSG_SETTING_ITEM(crisis_offer_declined_target, 80) \
	MSG_SETTING_ITEM(crisis_resolved_cause, 81) \
	MSG_SETTING_ITEM(crisis_resolved_target, 82) \
	MSG_SETTING_ITEM(crisis_started, 83) \
	MSG_SETTING_ITEM(crisis_became_war_target, 84)

enum class msg_setting_type : uint8_t {
#define MSG_SETTING_ITEM(name, num) name = num,
	MSG_SETTING_LIST
	count
#undef MSG_SETTING_ITEM
};

class msg_settings_item : public listbox_row_element_base<msg_setting_type> {
	simple_text_element_base* text = nullptr;

	static std::string get_setting_text_key(msg_setting_type type) {
		switch(type) {
#define MSG_SETTING_ITEM(name, num) case msg_setting_type::name: return #name "_setup";
		MSG_SETTING_LIST
#undef MSG_SETTING_ITEM
		default:
			return "???";
		}
	}
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

    void update(sys::state& state) noexcept override {
        text->set_text(state, text::produce_simple_string(state, get_setting_text_key(content)));
    }
};

class msg_settings_listbox : public listbox_element_base<msg_settings_item, msg_setting_type> {
protected:
    std::string_view get_row_element_name() override {
        return "message_entry";
    }
public:
    void on_update(sys::state& state) noexcept override {
        row_contents.clear();
        for(auto i = 0; i != uint8_t(msg_setting_type::count); ++i) {
			row_contents.push_back(msg_setting_type(i));
		}
        update(state);
    }
};

enum class msg_settings_category : uint8_t {
	all, combat, diplomacy, units, provinces, events, others
};

class msg_settings_window : public generic_tabbed_window<msg_settings_category> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window<msg_settings_category>::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
		    return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "settings") {
			// Nudge required for listbox before it is created...
			state.ui_defs.gui[id].size.x -= 21; // Nudge
			auto ptr = make_element_by_type<msg_settings_listbox>(state, id);
			ptr->base_data.position.x += 21; // Nudge
			ptr->base_data.position.y += 21; // Nudge
			return ptr;
		} else if(name == "messagecat_all") {
			auto ptr = make_element_by_type<generic_tab_button<msg_settings_category>>(state, id);
			ptr->target = msg_settings_category::all;
			return ptr;
		} else if(name == "messagecat_combat") {
			auto ptr = make_element_by_type<generic_tab_button<msg_settings_category>>(state, id);
			ptr->target = msg_settings_category::combat;
			return ptr;
		} else if(name == "messagecat_diplomacy") {
			auto ptr = make_element_by_type<generic_tab_button<msg_settings_category>>(state, id);
			ptr->target = msg_settings_category::diplomacy;
			return ptr;
		} else if(name == "messagecat_units") {
			auto ptr = make_element_by_type<generic_tab_button<msg_settings_category>>(state, id);
			ptr->target = msg_settings_category::units;
			return ptr;
		} else if(name == "messagecat_provinces") {
			auto ptr = make_element_by_type<generic_tab_button<msg_settings_category>>(state, id);
			ptr->target = msg_settings_category::provinces;
			return ptr;
		} else if(name == "messagecat_events") {
			auto ptr = make_element_by_type<generic_tab_button<msg_settings_category>>(state, id);
			ptr->target = msg_settings_category::events;
			return ptr;
		} else if(name == "messagecat_others") {
			auto ptr = make_element_by_type<generic_tab_button<msg_settings_category>>(state, id);
			ptr->target = msg_settings_category::others;
			return ptr;
		} else {
			return nullptr;
		}
	}
};

#undef MSG_SETTING_LIST
}
