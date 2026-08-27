#include "system_state.hpp"
#include "gui_element_base.hpp"

namespace ui {
tooltip_behavior element_base::has_tooltip(sys::state& state) noexcept { // used to test whether a tooltip is possible
	if(state.cheat_data.ui_debug_mode)
		return tooltip_behavior::tooltip;
	return tooltip_behavior::no_tooltip;
}
void element_base::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, urect& subrect) noexcept {
	ident = 0;
	subrect.top_left = ui::get_absolute_location(state, *this);
	subrect.size = base_data.size;
}
void element_base::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(state.cheat_data.ui_debug_mode) {
		text::add_line(state, contents, "ui_element_name", text::variable_type::x, base_data.name);
		text::add_line(state, contents, "ui_element_position", text::variable_type::x, base_data.position.x, text::variable_type::y, base_data.position.y);
		text::add_line(state, contents, "ui_element_size", text::variable_type::x, base_data.size.x, text::variable_type::y, base_data.size.y);
	}
}
}
