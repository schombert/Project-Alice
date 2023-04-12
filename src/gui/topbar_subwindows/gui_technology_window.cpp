#include "gui_technology_window.hpp"

namespace ui {

bool technology_folder_tab_sub_button::is_active(sys::state& state) noexcept {
    return parent && parent->parent && static_cast<technology_window*>(parent->parent)->active_tab == target;
}

void technology_folder_tab_sub_button::button_action(sys::state& state) noexcept {
    if(parent) {
        Cyto::Any payload = target;
        parent->parent->impl_set(state, payload);
    }
}

void technology_item_button::button_action(sys::state& state) noexcept {
    if(parent) {
        Cyto::Any payload = static_cast<technology_item_window*>(parent)->tech_id;
        parent->impl_get(state, payload);
    }
}

message_result technology_item_window::set(sys::state& state, Cyto::Any& payload) noexcept {
    if(payload.holds_type<culture::tech_category>()) {
        auto enum_val = any_cast<culture::tech_category>(payload);
        set_visible(state, category == enum_val);
        return message_result::consumed;
    }
    return message_result::unseen;
}
}
