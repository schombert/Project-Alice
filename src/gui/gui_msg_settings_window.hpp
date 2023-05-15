#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

#define MSG_SETTING_LIST \
	MSG_SETTING_ITEM(gaincb, "wegaincb") \
	MSG_SETTING_ITEM(gaincb_target, "wegaincb_target") \
	MSG_SETTING_ITEM_R(cb_justify_no_longer_valid) \
	MSG_SETTING_ITEM_R(losecb_target) \
	MSG_SETTING_ITEM(revoltalliance_cause, "revoltalliance") \
	MSG_SETTING_ITEM(revoltalliance_target, "revoltalliance") \
	MSG_SETTING_ITEM(war_cause, "war_they_accept") \
	MSG_SETTING_ITEM(war_target, "war_we_accept") \
	MSG_SETTING_ITEM(wargoal_cause, "wargoal_they_accept") \
	MSG_SETTING_ITEM(wargoal_target, "wargoal_we_accept") \
	MSG_SETTING_ITEM_R(socreform) \
	MSG_SETTING_ITEM_R(socreform_bad) \
	MSG_SETTING_ITEM_R(polreform) \
	MSG_SETTING_ITEM_R(polreform_bad) \
	MSG_SETTING_ITEM_R(economicreform) \
	MSG_SETTING_ITEM_R(economicreform_bad) \
	MSG_SETTING_ITEM_R(militaryreform) \
	MSG_SETTING_ITEM_R(militaryreform_bad) \
	MSG_SETTING_ITEM_R(setparty) \
	MSG_SETTING_ITEM_R(upperhouse) \
	MSG_SETTING_ITEM_R(electionstart) \
	MSG_SETTING_ITEM_R(electiondone) \
	MSG_SETTING_ITEM_R(breakcountry) \
	MSG_SETTING_ITEM(rebels, "partisans") \
	MSG_SETTING_ITEM(annex_cause, "annex") \
	MSG_SETTING_ITEM(annex_target, "annex") \
	MSG_SETTING_ITEM(peace_accept_cause, "peace_they_accept") \
	MSG_SETTING_ITEM(peace_accept_target, "peace_we_accept") \
	MSG_SETTING_ITEM(peace_decline_cause, "peace_they_decline") \
	MSG_SETTING_ITEM(peace_decline_target, "peace_we_decline") \
	MSG_SETTING_ITEM(mobilize, "we_mobilize") \
	MSG_SETTING_ITEM(demobilize, "we_demobilize") \
	MSG_SETTING_ITEM_R(building_done) \
	MSG_SETTING_ITEM_R(eventhappenother) \
	MSG_SETTING_ITEM_R(eventhappenotheroption) \
	MSG_SETTING_ITEM_R(majoreventhappenother) \
	MSG_SETTING_ITEM_R(majoreventhappenotheroption) \
	MSG_SETTING_ITEM_R(invention) \
	MSG_SETTING_ITEM(tech, "tech_once") \
	MSG_SETTING_ITEM_R(leaderdied) \
	MSG_SETTING_ITEM_R(landbattleover) \
	MSG_SETTING_ITEM_R(navalbattleover) \
	MSG_SETTING_ITEM_R(decisionother) \
	MSG_SETTING_ITEM(no_longer_great_power, "we_no_longer_great_power") \
	MSG_SETTING_ITEM(became_great_power, "we_became_great_power") \
	MSG_SETTING_ITEM(alliance_accept_cause, "alliance_they_accept") \
	MSG_SETTING_ITEM(alliance_accept_target, "alliance_we_accept") \
	MSG_SETTING_ITEM(alliance_decline_cause, "alliance_they_decline") \
	MSG_SETTING_ITEM(alliance_decline_target, "alliance_we_decline") \
	MSG_SETTING_ITEM(cancelalliance_cause, "cancelalliance_they_accept") \
	MSG_SETTING_ITEM(cancelalliance_target, "cancelalliance_we_accept") \
	MSG_SETTING_ITEM(increaseopinion_cause, "increaseopinion_they_accept") \
	MSG_SETTING_ITEM(increaseopinion_target, "increaseopinion_we_accept") \
	MSG_SETTING_ITEM(addtosphere_cause, "addtosphere_they_accept") \
	MSG_SETTING_ITEM(addtosphere_target, "addtosphere_we_accept") \
	MSG_SETTING_ITEM(removefromsphere_cause, "removefromsphere_they_accept") \
	MSG_SETTING_ITEM(removefromsphere_target, "removefromsphere_we_accept") \
	MSG_SETTING_ITEM(removefromsphere_other_target, "removefromsphere_other_accept") \
	MSG_SETTING_ITEM(increaserelation_cause, "increaserelation_they_accept") \
	MSG_SETTING_ITEM(increaserelation_target, "increaserelation_we_accept") \
	MSG_SETTING_ITEM(decreaserelation_cause, "decreaserelation_they_accept") \
	MSG_SETTING_ITEM(decreaserelation_target, "decreaserelation_we_accept") \
	MSG_SETTING_ITEM(intervention_same_side, "intervention_our_side") \
	MSG_SETTING_ITEM(intervention_other_side, "intervention_their_side") \
	MSG_SETTING_ITEM(intervention, "intervention_other_side") \
	MSG_SETTING_ITEM_R(bankruptcy) \
	MSG_SETTING_ITEM_R(new_party_avail) \
	MSG_SETTING_ITEM_R(party_unavail) \
	MSG_SETTING_ITEM(cb_detected_cause, "others_cb_detected") \
	MSG_SETTING_ITEM(cb_detected_target, "our_cb_detected") \
	MSG_SETTING_ITEM(join_crisis_offer_cause, "crisis_offer_they_accept") \
	MSG_SETTING_ITEM(join_crisis_offer_target, "crisis_offer_we_accept") \
	MSG_SETTING_ITEM(join_crisis_offer_declind_cause, "crisis_offer_they_decline") \
	MSG_SETTING_ITEM(join_crisis_offer_decline_target, "crisis_offer_we_decline") \
	MSG_SETTING_ITEM(join_crisis_cause, "crisis_offer_we_accept") \
	MSG_SETTING_ITEM(join_crisis_target, "crisis_offer_other_accept") \
	MSG_SETTING_ITEM(crisis_offer_cause, "crisis_offer_they_accept") \
	MSG_SETTING_ITEM(crisis_offer_target, "crisis_offer_we_accept") \
	MSG_SETTING_ITEM(crisis_offer_decline_cause, "crisis_offer_they_decline") \
	MSG_SETTING_ITEM(crisis_offer_decline_target, "crisis_offer_we_decline") \
	MSG_SETTING_ITEM(crisis_resolved_cause, "crisis_back_down_they_accept") \
	MSG_SETTING_ITEM(crisis_resolved_target, "crisis_back_down_we_accept") \
	MSG_SETTING_ITEM_R(crisis_started) \
	MSG_SETTING_ITEM(crisis_became_war_target, "crisis_became_war")
#define MSG_SETTING_ITEM_R(name) MSG_SETTING_ITEM(name, #name)

enum class msg_setting_type : uint8_t {
#define MSG_SETTING_ITEM(name, locale_name) name,
	MSG_SETTING_LIST
	count
#undef MSG_SETTING_ITEM
};

class msg_settings_item : public listbox_row_element_base<msg_setting_type> {
	simple_text_element_base* text = nullptr;

	static std::string get_setting_text_key(msg_setting_type type) {
		switch(type) {
#define MSG_SETTING_ITEM(name, locale_name) \
case msg_setting_type::name: \
	return locale_name "_setup";
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
	all, combat, diplomacy, units, provinces, events, others, count
};

class msg_settings_window : public generic_tabbed_window<msg_settings_category> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window<msg_settings_category>::on_create(state);
		base_data.position.y -= 21; // Nudge
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close") {
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

struct msg_log_data {
	// TODO: Display message itself
	msg_settings_category category = msg_settings_category::all;
};

class msg_log_entry : public listbox_row_element_base<msg_log_data> {
public:
};

class msg_log_listbox : public listbox_element_base<msg_log_entry, msg_log_data> {
protected:
    std::string_view get_row_element_name() override {
        return "message_entry";
    }
};

template<msg_settings_category Filter>
class msg_log_filter_checkbox : public checkbox_button {
    static std::string_view get_filter_text_key(msg_settings_category f) noexcept {
        switch(f) {
        case msg_settings_category::all:
            return "messagesettings_all";
        case msg_settings_category::combat:
            return "messagesettings_combat";
        case msg_settings_category::diplomacy:
            return "messagesettings_diplomacy";
        case msg_settings_category::units:
            return "messagesettings_units";
        case msg_settings_category::provinces:
            return "messagesettings_provinces";
		case msg_settings_category::events:
            return "messagesettings_events";
		case msg_settings_category::others:
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
            Cyto::Any payload = Filter;
            parent->impl_set(state, payload);
        }
    }

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto name = get_filter_text_key(Filter);
		if(auto k = state.key_to_text_sequence.find(name); k != state.key_to_text_sequence.end()) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, k->second, text::substitution_map{ });
			text::close_layout_box(contents, box);
		}
	}
};

class msg_log_window : public window_element_base {
	std::vector<bool> cat_filters;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		cat_filters.resize(size_t(msg_settings_category::count));
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close") {
		    return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "messagelog") {
		    return make_element_by_type<msg_log_listbox>(state, id);
		} else if(name == "messagecat_combat") {
		    return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::combat>>(state, id);
		} else if(name == "messagecat_diplomacy") {
		    return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::diplomacy>>(state, id);
		} else if(name == "messagecat_units") {
		    return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::units>>(state, id);
		} else if(name == "messagecat_provinces") {
		    return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::provinces>>(state, id);
		} else if(name == "messagecat_events") {
		    return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::events>>(state, id);
		} else if(name == "messagecat_others") {
		    return make_element_by_type<msg_log_filter_checkbox<msg_settings_category::others>>(state, id);
		} else {
			return nullptr;
		}
	}

    message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<msg_settings_category>()) {
			auto cat = any_cast<msg_settings_category>(payload);
			payload.emplace<bool>(cat_filters[uint8_t(cat)]);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<msg_settings_category>()) {
			auto cat = any_cast<msg_settings_category>(payload);
			cat_filters[uint8_t(cat)] = !cat_filters[uint8_t(cat)];
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

#undef MSG_SETTING_LIST
}
