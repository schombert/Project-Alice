#include "gui_leader_select.hpp"

namespace ui {
void open_leader_selection(sys::state& state, dcon::army_id a, dcon::navy_id v, int32_t x, int32_t y) {
	leader_selection_window* win = static_cast<leader_selection_window*>(state.ui_state.change_leader_window);
	win->a = a;
	win->v = v;
	win->set_visible(state, !win->is_visible());
	win->base_data.position.x = int16_t(x);
	if(int32_t(win->base_data.position.x + win->base_data.size.x) >= state.x_size) {
		win->base_data.position.x -= win->base_data.size.x;
	}
	win->base_data.position.y = int16_t(std::clamp(y, 64, int32_t(state.ui_state.root->base_data.size.y - state.ui_state.change_leader_window->base_data.size.y)));
	state.ui_state.root->move_child_to_front(win);
}
} // namespace ui
