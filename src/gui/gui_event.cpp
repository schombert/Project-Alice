#include "gui_event.hpp"

namespace ui {
template<typename V>
static bool find_usable_window(sys::state& state, V& v, Cyto::Any& payload) {
	// Try to find an usable window first...
	for(auto it = v.begin(); it != v.end(); it++) {
		(*it)->impl_set(state, payload);
		state.ui_state.root->add_child_to_front(std::move(*it));
		v.erase(std::remove(v.begin(), v.end(), it), v.end());
		return true;
	}
	return false;
}
template<typename T>
static void fire_event_national_internal(sys::state& state, T fat_id) {
	Cyto::Any payload = ui::national_event_data_wrapper(fat_id.id);
	if(fat_id.get_is_major()) {
		if(find_usable_window(state, state.ui_state.spare_major_event_subwindows, payload))
			return;
		// Otherwise add it to the window list
		auto new_elm = ui::make_element_by_type<ui::national_event_window<true>>(state, "event_major_window");
		new_elm->impl_set(state, payload);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		if(find_usable_window(state, state.ui_state.spare_national_event_subwindows, payload))
			return;
		// Otherwise add it to the window list
		auto new_elm = ui::make_element_by_type<ui::national_event_window<false>>(state, "event_country_window");
		new_elm->impl_set(state, payload);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
}
template<typename T>
static void fire_event_provincial_internal(sys::state& state, T fat_id) {
	Cyto::Any payload = ui::provincial_event_data_wrapper(fat_id.id);
	if(find_usable_window(state, state.ui_state.spare_provincial_event_subwindows, payload))
		return;
	// Otherwise add it to the window list
	auto new_elm = ui::make_element_by_type<ui::provincial_event_window>(state, "event_province_window");
	new_elm->impl_set(state, payload);
	state.ui_state.root->add_child_to_front(std::move(new_elm));
}
void fire_event(sys::state& state, const dcon::national_event_id event_id) {
	fire_event_national_internal(state, dcon::fatten(state.world, event_id));
}
void fire_event(sys::state& state, const dcon::free_national_event_id event_id) {
	fire_event_national_internal(state, dcon::fatten(state.world, event_id));
}
void fire_event(sys::state& state, const dcon::provincial_event_id event_id) {
	fire_event_provincial_internal(state, dcon::fatten(state.world, event_id));
}
void fire_event(sys::state& state, const dcon::free_provincial_event_id event_id) {
	fire_event_provincial_internal(state, dcon::fatten(state.world, event_id));
}

void select_event_option(sys::state& state, const ui::national_event_window<true>& elm, const sys::event_option opt) {
	auto ptr = state.ui_state.root->remove_child(&elm);
	state.ui_state.spare_major_event_subwindows.push_back(std::move(ptr));
}
void select_event_option(sys::state& state, const ui::national_event_window<false>& elm, const sys::event_option opt) {
	auto ptr = state.ui_state.root->remove_child(&elm);
	state.ui_state.spare_national_event_subwindows.push_back(std::move(ptr));
}
void select_event_option(sys::state& state, const ui::provincial_event_window& elm, const sys::event_option opt) {
	auto ptr = state.ui_state.root->remove_child(&elm);
	state.ui_state.spare_provincial_event_subwindows.push_back(std::move(ptr));
}
}
