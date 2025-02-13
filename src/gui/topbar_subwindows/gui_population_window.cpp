#include "gui_population_window.hpp"
#include "demographics.hpp"
#include "gui_modifier_tooltips.hpp"
#include "gui_trigger_tooltips.hpp"
#include "triggers.hpp"

namespace ui {

/*
void pop_national_focus_button::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = dcon::state_instance_id{};
		parent->impl_get(state, payload);

		auto pop_window = static_cast<population_window*>(state.ui_state.population_subwindow);
		pop_window->focus_state = any_cast<dcon::state_instance_id>(payload);
		pop_window->nf_win->set_visible(state, !pop_window->nf_win->is_visible());
		pop_window->nf_win->base_data.position = base_data.position;
		pop_window->move_child_to_front(pop_window->nf_win);
		pop_window->impl_on_update(state);
	}
}*/

} // namespace ui
