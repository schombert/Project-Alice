#pragma once

#include "gui_element_types.hpp"

namespace ui {


class controls_menu_window : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button")
			return make_element_by_type<generic_close_button>(state, id);
		else if(name == "background")
			return make_element_by_type<draggable_target>(state, id);
		else
			return nullptr;
	}
};
class graphics_menu_window : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button")
			return make_element_by_type<generic_close_button>(state, id);
		else if(name == "background")
			return make_element_by_type<draggable_target>(state, id);
		else
			return nullptr;
	}
};
class audio_menu_window : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button")
			return make_element_by_type<generic_close_button>(state, id);
		else if(name == "background")
			return make_element_by_type<draggable_target>(state, id);
		else
			return nullptr;
	}
};

enum class main_menu_sub_window {
	none, controls, audio, graphics
};

class open_controls_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = main_menu_sub_window::controls;
			parent->impl_get(state, payload);
		}
	}
};
class open_audio_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = main_menu_sub_window::audio;
			parent->impl_get(state, payload);
		}
	}
};
class open_graphics_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = main_menu_sub_window::graphics;
			parent->impl_get(state, payload);
		}
	}
};

class main_menu_window : public window_element_base {
public:
	controls_menu_window* controls_menu = nullptr;
	graphics_menu_window* graphics_menu = nullptr;
	audio_menu_window* audio_menu = nullptr;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "graphics") {
			return make_element_by_type<open_graphics_button>(state, id);
		} else if(name == "sound") {
			return make_element_by_type<open_audio_button>(state, id);
		} else if(name == "controls") {
			return make_element_by_type<open_controls_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "alice_graphics_menu") {
			auto tmp = make_element_by_type<graphics_menu_window>(state, id);
			graphics_menu = tmp.get();
			tmp->set_visible(state, false);
			return tmp;
		} else if(name == "alice_controls_menu") {
			auto tmp = make_element_by_type<controls_menu_window>(state, id);
			controls_menu = tmp.get();
			tmp->set_visible(state, false);
			return tmp;
		} else if(name == "alice_audio_menu") {
			auto tmp = make_element_by_type<audio_menu_window>(state, id);
			tmp->set_visible(state, false);
			audio_menu = tmp.get();
			return tmp;
		} else {
			return nullptr;
		}
	}

	void hide_subwindows(sys::state& state) {
		if(controls_menu)
			controls_menu->set_visible(state, false);
		if(graphics_menu)
			graphics_menu->set_visible(state, false);
		if(audio_menu)
			audio_menu->set_visible(state, false);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<main_menu_sub_window>()) {
			auto enum_val = any_cast<main_menu_sub_window>(payload);
			hide_subwindows(state);
			switch(enum_val) {
				case main_menu_sub_window::none:
					break;
				case main_menu_sub_window::controls:
					controls_menu->set_visible(state, true);
					break;
				case main_menu_sub_window::audio:
					audio_menu->set_visible(state, true);
					break;
				case main_menu_sub_window::graphics:
					graphics_menu->set_visible(state, true);
					break;
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
