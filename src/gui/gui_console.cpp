#include "gui_console.hpp"

void ui::console_window::show_toggle(sys::state& state) {
	if(!state.ui_state.console_window) {
		auto window = make_element_by_type<console_window>(state, "console_wnd");
		state.ui_state.console_window = window.get();
		state.ui_state.root->add_child_to_front(std::move(window));
	} else {
        if(state.ui_state.console_window->is_visible()) {
            state.ui_state.console_window->set_visible(state, false);
        } else {
            state.ui_state.console_window->set_visible(state, true);
            state.ui_state.root->move_child_to_front(state.ui_state.console_window);
        }
	}
}
