#pragma once

#include "gui_element_types.h"

namespace ui {

class multiplayer_menu_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "bg_multiplayer_menu_02") {
			return make_element_by_type<image_element_base>(state, id);
		} else if (name == "bg_multiplayer_menu") {
			return make_element_by_type<image_element_base>(state, id);
		} else if (name == "games") {
			// TODO - listbox here :3
			return nullptr;
		} else if (name == "host") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "join") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "join_direct") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "scan") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "metaserver") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "back") {
			return make_element_by_type<button_element_base>(state, id);
		} else if (name == "playername") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if (name == "name_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class multiplayer_connect_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "frontend_connectlog_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if (name == "info_list") {
			// TODO - listbox here :3
			return nullptr;
		} else if (name == "back") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

} // namespace ui
