
#include "gui_factory_refit_window.hpp"

namespace ui {

void hide_factory_refit_menu(sys::state& state) {
	if(!state.ui_state.factory_refit_win) {
		return;
	}

	state.ui_state.factory_refit_win->set_visible(state, false);
}

void show_factory_refit_menu(sys::state& state, dcon::factory_id selected_factory) {
	if(!state.ui_state.factory_refit_win) {
		auto win3 = make_element_by_type<factory_refit_window>(state, state.ui_state.defs_by_name.find(state.lookup_key("factory_refit_window"))->second.definition);
		state.ui_state.factory_refit_win = win3.get();
		state.ui_state.root->add_child_to_front(std::move(win3));
	}
	state.ui_state.factory_refit_win->selected_factory = selected_factory;
	state.ui_state.factory_refit_win->set_visible(state, true);
	state.ui_state.factory_refit_win->impl_on_update(state);
	state.ui_state.root->move_child_to_front(state.ui_state.factory_refit_win);
	state.ui_state.factory_refit_win->base_data.position.x = (int16_t)(state.mouse_x_position / state.user_settings.ui_scale + 10);
	state.ui_state.factory_refit_win->base_data.position.y = (int16_t)(state.mouse_y_position / state.user_settings.ui_scale - 150);
}

bool factory_refit_button_active(sys::state& state, dcon::factory_id fid, dcon::province_id pid, dcon::state_instance_id sid, dcon::nation_id n) {
	auto fat = dcon::fatten(state.world, fid);
	auto type = state.world.factory_get_building_type(fid);

	// no double upgrade
	bool is_not_upgrading = true;
	for(auto p : state.world.province_get_factory_construction(pid)) {
		if(p.get_type() == type)
			is_not_upgrading = false;
	}
	if(!is_not_upgrading) {
		return false;
	}

	bool is_civ = state.world.nation_get_is_civilized(n);
	if(!is_civ) {
		return false;
	}

	if(economy::is_colony(state, sid) && !economy::can_build_factory_type_in_colony(state, sid, type))
		return false;

	return true;
}

void factory_refit_button_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, dcon::factory_id fid) {
	auto type = state.world.factory_get_building_type(fid);
	auto pid = state.world.factory_get_province_from_factory_location(fid);
	auto sid = state.world.province_get_state_membership(pid);
	auto n = state.world.province_get_nation_from_province_ownership(pid);

	// no double upgrade
	bool is_not_upgrading = true;
	for(auto p : state.world.province_get_factory_construction(pid)) {
		if(p.get_type() == type)
			is_not_upgrading = false;
	}

	text::add_line(state, contents, "production_refit_factory_tooltip_1", text::variable_type::what, state.world.factory_type_get_name(type));

	text::add_line_break_to_layout(state, contents);

	bool is_civ = state.world.nation_get_is_civilized(state.local_player_nation);
	text::add_line_with_condition(state, contents, "factory_upgrade_condition_1", is_civ);

	bool state_is_not_colonial = !state.world.province_get_is_colonial(state.world.state_instance_get_capital(sid));
	text::add_line_with_condition(state, contents, "factory_upgrade_condition_2", state_is_not_colonial);

	bool is_activated = state.world.nation_get_active_building(n, type) || state.world.factory_type_get_is_available_from_start(type);
	text::add_line_with_condition(state, contents, "factory_upgrade_condition_3", is_activated);

	text::add_line_with_condition(state, contents, "factory_upgrade_condition_9", is_not_upgrading);
}

}
