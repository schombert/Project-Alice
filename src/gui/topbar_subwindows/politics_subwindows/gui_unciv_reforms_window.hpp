#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

class unciv_reforms_westernize_button : public standard_nation_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		on_update(state);
	}

	void on_update(sys::state& state) noexcept override {
		disabled = state.world.nation_get_modifier_values(nation_id, sys::national_mod_offsets::civilization_progress_modifier) < 1.f;
	}
};

class unciv_reforms_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "civ_progress") {
			return make_element_by_type<nation_westernization_progress_bar>(state, id);
		} else if(name == "westernize_button") {
			return make_element_by_type<unciv_reforms_westernize_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
