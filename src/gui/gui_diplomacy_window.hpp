#pragma once

#include "gui_element_types.hpp"

namespace ui {

enum class diplomacy_window_tab : uint8_t {
	great_powers = 0x0,
	wars = 0x1,
	casus_belli = 0x2,
	crisis = 0x3
};

class diplomacy_window : public generic_tabbed_window<diplomacy_window_tab> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "gp_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::great_powers;
			return ptr;
		} else if(name == "war_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::wars;
			return ptr;
		} else if(name == "cb_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::casus_belli;
			return ptr;
		} else if(name == "crisis_info") {
			auto ptr = make_element_by_type<generic_tab_button<diplomacy_window_tab>>(state, id);
			ptr->target = diplomacy_window_tab::crisis;
			return ptr;
		} else if(name == "cb_info_win") {
			auto ptr = make_element_immediate(state, id);
			casus_belli_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "crisis_info_win") {
			auto ptr = make_element_immediate(state, id);
			crisis_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void hide_tabs(sys::state& state) {
		casus_belli_window->set_visible(state, false);
		crisis_window->set_visible(state, false);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<diplomacy_window_tab>()) {
			auto enum_val = any_cast<diplomacy_window_tab>(payload);
			hide_tabs(state);
			switch(enum_val) {
				case diplomacy_window_tab::great_powers:
					break;
				case diplomacy_window_tab::wars:
					break;
				case diplomacy_window_tab::casus_belli:
					casus_belli_window->set_visible(state, true);
					break;
				case diplomacy_window_tab::crisis:
					crisis_window->set_visible(state, true);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	
	element_base* casus_belli_window = nullptr;
	element_base* crisis_window = nullptr;
};

}
