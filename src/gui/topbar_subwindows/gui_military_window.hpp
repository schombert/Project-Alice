#pragma once

#include "gui_element_types.hpp"
#include "gui_leaders_window.hpp"

namespace ui {

class military_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "mobilize") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "demobilize") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "leaders") {
			auto ptr = make_element_by_type<leaders_window>(state, id);
			ptr->set_visible(state, true);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
