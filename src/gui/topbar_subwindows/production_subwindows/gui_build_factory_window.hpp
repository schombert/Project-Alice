#pragma once

#include "gui_element_types.hpp"

namespace ui {

class factory_build_new_factory_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "output") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "input0") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "input1") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "input2") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "input3") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "cancel") {
			return make_element_by_type<button_element_base>(state, id);
		} else
		if(name == "build") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
