
#include "gui_factory_refit_window.hpp"

namespace ui {

void hide_factory_refit_menu(sys::state& state) {
	if(!state.ui_state.factory_refit_win) {
		return;
	}

	state.ui_state.factory_refit_win->set_visible(state, false);
}

void show_factory_refit_menu(sys::state& state, dcon::factory_id selected_factory) {
	if(!state.ui_state.factory_refit_win) {
		auto win3 = make_element_by_type<factory_refit_window>(state, state.ui_state.defs_by_name.find(state.lookup_key("factory_refit_window"))->second.definition);
		state.ui_state.factory_refit_win = win3.get();
		state.ui_state.root->add_child_to_front(std::move(win3));
	}
	state.ui_state.factory_refit_win->selected_factory = selected_factory;
	state.ui_state.factory_refit_win->set_visible(state, true);
	state.ui_state.factory_refit_win->impl_on_update(state);
	state.ui_state.root->move_child_to_front(state.ui_state.factory_refit_win);
	state.ui_state.factory_refit_win->base_data.position.x = (int16_t)(state.mouse_x_position / state.user_settings.ui_scale + 10);
	state.ui_state.factory_refit_win->base_data.position.y = (int16_t)(state.mouse_y_position / state.user_settings.ui_scale - 50);
}

}
