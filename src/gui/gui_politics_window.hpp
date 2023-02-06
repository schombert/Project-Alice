#pragma once

#include "gui_element_types.hpp"
#include <vector>

namespace ui {

enum class politics_window_sub_window : uint8_t {
	reforms = 0x0,
	movements = 0x1,
	decisions = 0x2,
	releasables = 0x3
};

class politics_window_tab_button : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept override;
	void button_action(sys::state& state) noexcept override;
	politics_window_sub_window target = politics_window_sub_window::reforms;
};

class politics_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_tab_close_button>(state, id);
		} else if(name == "reforms_tab") {
			auto ptr = make_element_by_type<politics_window_tab_button>(state, id);
			ptr->target = politics_window_sub_window::reforms;
			return ptr;
		} else if(name == "movements_tab") {
			auto ptr = make_element_by_type<politics_window_tab_button>(state, id);
			ptr->target = politics_window_sub_window::movements;
			return ptr;
		} else if(name == "decisions_tab") {
			auto ptr = make_element_by_type<politics_window_tab_button>(state, id);
			ptr->target = politics_window_sub_window::decisions;
			return ptr;
		} else if(name == "release_nations_tab") {
			auto ptr = make_element_by_type<politics_window_tab_button>(state, id);
			ptr->target = politics_window_sub_window::releasables;
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
		if(payload.holds_type<politics_window_sub_window>()) {
			auto enum_val = any_cast<politics_window_sub_window>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case politics_window_sub_window::reforms:
					show_vector_elements(state, reform_elements);
					break;
				case politics_window_sub_window::movements:
					show_vector_elements(state, movement_elements);
					break;
				case politics_window_sub_window::decisions:
					show_vector_elements(state, decision_elements);
					break;
				case politics_window_sub_window::releasables:
					show_vector_elements(state, release_elements);
					break;
			}
			active_sub_window = enum_val;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	std::vector<element_base*> reform_elements;
	std::vector<element_base*> movement_elements;
	std::vector<element_base*> decision_elements;
	std::vector<element_base*> release_elements;
	politics_window_sub_window active_sub_window = politics_window_sub_window::reforms;
};

}
