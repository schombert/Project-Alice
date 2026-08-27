#pragma once
#include "gui_lookup.hpp"
namespace ui {
template<typename T, typename ...Params>
std::unique_ptr<T> make_element_by_type(sys::state& state, std::string_view name, Params&&... params) { // also bypasses global creation hooks
	auto id = ui::get_gui_definition_id(state, name);
	if(!id)return std::unique_ptr<T>{};

	auto def = ui::get_element_data(state, id);

	auto res = std::make_unique<T>(std::forward<Params>(params)...);
	std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
	make_size_from_graphics(state, res->base_data);
	res->on_create(state);
	return res;
}
template<typename T, typename ...Params>
std::unique_ptr<T> make_element_by_type(sys::state& state, dcon::gui_def_id id, Params&&... params) { // also bypasses global creation hooks

	auto def = ui::get_element_data(state, id);

	auto res = std::make_unique<T>(std::forward<Params>(params)...);
	std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
	make_size_from_graphics(state, res->base_data);
	res->on_create(state);
	return res;
}
}
