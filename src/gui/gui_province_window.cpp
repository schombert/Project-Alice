#include "gui_province_window.hpp"

namespace ui {

void province_national_focus_button::button_action(sys::state& state) noexcept {
    auto province_window = static_cast<province_view_window*>(state.ui_state.province_window);
    province_window->national_focus_window->set_visible(state, !province_window->national_focus_window->is_visible());
}

void province_rgo_icon::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto rgo_good = state.world.province_get_rgo(prov_id);
	if(rgo_good) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, state.world.commodity_get_name(rgo_good), text::substitution_map{ });
		text::close_layout_box(contents, box);
	}
}

}
