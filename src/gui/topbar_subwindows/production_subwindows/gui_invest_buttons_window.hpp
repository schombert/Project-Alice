#pragma once

#include "gui_element_types.hpp"

namespace ui {

class invest_buttons_window : public window_element_base {
  public:
	void on_create(sys::state &state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "country_flag") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if (name == "invest_country_browse") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if (name == "select_all") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
