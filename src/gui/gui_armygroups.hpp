#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_unit_panel_subwindow.hpp"
#include "text.hpp"
#include "prng.hpp"
#include "gui_leader_tooltip.hpp"
#include "gui_leader_select.hpp"
#include "gui_unit_grid_box.hpp"

namespace ui {
	class army_management_window : public window_element_base {
		std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
			if(name == "new_army_group_button") {
				return make_element_by_type<new_army_group_button>(state, id);
			} else {
				return nullptr;
			}
		}
	};
}
