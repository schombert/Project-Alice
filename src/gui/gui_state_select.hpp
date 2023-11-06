#pragma once

#include "demographics.hpp"
#include "gui_element_types.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <vector>

namespace ui {

class map_state_select_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, "State select (ESC)"));
	}
};

class map_state_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_title") {
			return make_element_by_type<map_state_select_title>(state, id);
		} else if(name == "gradient_icon") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "gradient_min") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "gradient_max") {
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}
};

} //namespace ui
