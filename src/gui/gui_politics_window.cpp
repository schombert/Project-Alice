#include "gui_politics_window.hpp"

namespace ui {

bool politics_window_tab_button::is_active(sys::state& state) noexcept {
	return parent && static_cast<politics_window*>(parent)->active_sub_window == target;
}

void politics_window_tab_button::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = target;
		parent->impl_get(state, payload);
	}
}

}
