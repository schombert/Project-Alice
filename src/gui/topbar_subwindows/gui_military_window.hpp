#pragma once

#include "gui_element_types.hpp"
#include "gui_leaders_window.hpp"
#include "gui_units_window.hpp"
#include "gui_build_unit_large_window.hpp"

namespace ui {

class military_window : public window_element_base {
	public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		// Unit information comes first
		auto win1 = make_element_by_type<military_units_window<dcon::army_id>>(state,
				state.ui_state.defs_by_name.find("unit_window")->second.definition);
		win1->base_data.position = state.ui_defs.gui[state.ui_state.defs_by_name.find("army_pos")->second.definition].position;
		state.ui_state.unit_window_army = win1.get();
		add_child_to_front(std::move(win1));

		// Navy information is right next to the army information
		auto win2 = make_element_by_type<military_units_window<dcon::navy_id>>(state,
				state.ui_state.defs_by_name.find("unit_window")->second.definition);
		win2->base_data.position = state.ui_defs.gui[state.ui_state.defs_by_name.find("navy_pos")->second.definition].position;
		state.ui_state.unit_window_navy = win2.get();
		add_child_to_front(std::move(win2));

		auto build_units_window = make_element_by_type<build_unit_large_window>(state,
				state.ui_state.defs_by_name.find("build_unit_view_large")->second.definition);
		build_units_window->base_data.position.y -= 63;
		build_units_window->base_data.position.x += 1;
		state.ui_state.build_unit_window = build_units_window.get();
		build_units_window->set_visible(state, false);
		add_child_to_front(std::move(build_units_window));

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

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
