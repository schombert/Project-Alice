#include "gui_technology_window.hpp"

namespace ui {

bool technology_folder_tab_sub_button::is_active(sys::state &state) noexcept {
	return parent && parent->parent && static_cast<technology_window *>(parent->parent)->active_tab == category;
}

void technology_folder_tab_sub_button::button_action(sys::state &state) noexcept {
	if (parent) {
		Cyto::Any payload = category;
		parent->parent->impl_set(state, payload);
	}
}

message_result technology_item_window::set(sys::state &state, Cyto::Any &payload) noexcept {
	if (payload.holds_type<dcon::technology_id>()) {
		tech_id = any_cast<dcon::technology_id>(payload);
		auto tech = dcon::fatten(state.world, tech_id);
		category = state.culture_definitions.tech_folders[tech.get_folder_index()].category;
		return message_result::consumed;
	} else if (payload.holds_type<culture::tech_category>()) {
		auto enum_val = any_cast<culture::tech_category>(payload);
		set_visible(state, category == enum_val);
		return message_result::consumed;
	}
	return message_result::unseen;
}
} // namespace ui
