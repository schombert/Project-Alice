#include "gui_leader_select.hpp"

namespace ui {
void open_leader_selection(sys::state& state, dcon::army_id a, dcon::navy_id v, int32_t x, int32_t y) {
	leader_selection_window* win = static_cast<leader_selection_window*>(state.ui_state.change_leader_window);
	win->a = a;
	win->v = v;
	if(state.ui_state.change_leader_window->is_visible()) {
		state.ui_state.change_leader_window->impl_on_update(state);
	} else {
		state.ui_state.change_leader_window->set_visible(state, true);
	}
	state.ui_state.change_leader_window->base_data.position.x = int16_t(x);
	state.ui_state.change_leader_window->base_data.position.y= int16_t(std::clamp(y, 64, int32_t(state.ui_state.root->base_data.size.y - state.ui_state.change_leader_window->base_data.size.y)));
	state.ui_state.root->move_child_to_front(state.ui_state.change_leader_window);
}
} // namespace ui