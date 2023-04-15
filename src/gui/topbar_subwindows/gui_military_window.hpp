#pragma once

#include "gui_element_types.hpp"
#include "gui_leaders_window.hpp"
#include "gui_units_window.hpp"

namespace ui {

class military_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		// Unit information comes first
		auto win1 = make_element_by_type<military_units_window<dcon::army_id>>(state, state.ui_state.defs_by_name.find("unit_window")->second.definition);
		// TODO: read army_pos for position info
		win1->base_data.position = xy_pair{ 336, 20 };
		add_child_to_front(std::move(win1));
		
		// Navy information is right next to the army information
		auto win2 = make_element_by_type<military_units_window<dcon::navy_id>>(state, state.ui_state.defs_by_name.find("unit_window")->second.definition);
		// TODO: read navy_pos for position info
		win2->base_data.position = xy_pair{ 663, 20 };
		add_child_to_front(std::move(win2));

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
