#pragma once

#include "gui_element_types.hpp"
#include <vector>

namespace ui {

enum class politics_window_tab : uint8_t {
	reforms = 0x0,
	movements = 0x1,
	decisions = 0x2,
	releasables = 0x3
};

class politics_window : public generic_tabbed_window<politics_window_tab> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "reforms_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::reforms;
			return ptr;
		} else if(name == "movements_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::movements;
			return ptr;
		} else if(name == "decisions_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::decisions;
			return ptr;
		} else if(name == "release_nations_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::releasables;
			return ptr;
		} else {
			return nullptr;
		}
	}
	void hide_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, false);
		}
	}

	void show_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, true);
		}
	}
	void hide_sub_windows(sys::state& state) {
		hide_vector_elements(state, reform_elements);
		hide_vector_elements(state, movement_elements);
		hide_vector_elements(state, decision_elements);
		hide_vector_elements(state, release_elements);
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<politics_window_tab>()) {
			auto enum_val = any_cast<politics_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case politics_window_tab::reforms:
					show_vector_elements(state, reform_elements);
					break;
				case politics_window_tab::movements:
					show_vector_elements(state, movement_elements);
					break;
				case politics_window_tab::decisions:
					show_vector_elements(state, decision_elements);
					break;
				case politics_window_tab::releasables:
					show_vector_elements(state, release_elements);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	std::vector<element_base*> reform_elements;
	std::vector<element_base*> movement_elements;
	std::vector<element_base*> decision_elements;
	std::vector<element_base*> release_elements;
};

}
