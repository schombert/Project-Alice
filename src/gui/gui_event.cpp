#include "gui_event.hpp"

namespace ui {
template<typename V>
static bool use_reusable_event_window(sys::state& state, V& v, Cyto::Any& payload) {
	if(v.empty())
		return false;
	auto it = v.begin();
	(*it)->impl_set(state, payload);
	state.ui_state.root->add_child_to_front(std::move(*it));
	v.erase(it);
	return true;
}
template<typename T>
static void fire_event_national_internal(sys::state& state, T fat_id) {
	Cyto::Any payload = ui::national_event_data_wrapper(fat_id.id);
	if(fat_id.get_is_major()) {
		if(use_reusable_event_window(state, state.ui_state.spare_major_event_subwindows, payload))
			return;
		// Otherwise add it to the window list
		auto new_elm = ui::make_element_by_type<ui::national_event_window<true>>(state, "event_major_window");
		new_elm->impl_set(state, payload);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	} else {
		if(use_reusable_event_window(state, state.ui_state.spare_national_event_subwindows, payload))
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
	if(use_reusable_event_window(state, state.ui_state.spare_provincial_event_subwindows, payload))
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
}
