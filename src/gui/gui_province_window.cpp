#include "gui_province_window.hpp"

namespace ui {

void province_national_focus_button::button_action(sys::state& state) noexcept {
    auto province_window = static_cast<province_view_window*>(state.ui_state.province_window);
    province_window->national_focus_window->set_visible(state, !province_window->national_focus_window->is_visible());
}

}
