#include "gui_error_window.hpp"

void ui::popup_error_window(sys::state& state, std::string_view title, std::string_view body) {
	auto new_elm = ui::make_element_by_type<ui::error_dialog_window>(state, "defaultinfodialog");
	state.ui_state.error_win = new_elm.get();
	if(state.mode == sys::game_mode_type::pick_nation) {
		state.ui_state.nation_picker->add_child_to_front(std::move(new_elm));
	} else if(state.mode == sys::game_mode_type::in_game) {
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else if(state.mode == sys::game_mode_type::end_screen) {
		state.ui_state.end_screen->add_child_to_front(std::move(new_elm));
	} else if(state.mode == sys::game_mode_type::select_states) {
		state.ui_state.select_states_legend->add_child_to_front(std::move(new_elm));
	}

	auto win = static_cast<ui::error_dialog_window*>(state.ui_state.error_win);
	win->title->set_text(state, std::string(title));
	win->body->msg = std::string(body);
	win->set_visible(state, true);
	win->impl_on_update(state);
}
