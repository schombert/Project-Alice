#pragma once

#include "gui_element_types.hpp"

namespace ui {

class diplomacy_window_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.ui_state.tab_window->set_visible(state, false);
	}
};

class diplomacy_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<diplomacy_window_close_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
