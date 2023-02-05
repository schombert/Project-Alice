#include "gui_production_window.hpp"

namespace ui {

bool production_window_tab_button::is_active(sys::state& state) noexcept {
	return parent && static_cast<production_window*>(parent)->active_sub_window == target;
}

void production_window_tab_button::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = target;
		parent->impl_get(state, payload);
	}
}

}
