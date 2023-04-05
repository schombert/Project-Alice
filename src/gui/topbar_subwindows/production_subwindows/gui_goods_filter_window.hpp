#pragma once

#include "gui_element_types.hpp"

namespace ui {

class goods_filter_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "filter_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "filter_enabled") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			return ptr;
		} else if(name == "goods_type") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
