#pragma once

#include "gui_element_types.hpp"
#include "gui_message_settings_window.hpp"

namespace ui {

class ui_scale_left : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class ui_scale_right : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class ui_scale_display : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override;
};

class window_mode_checkbox : public checkbox_button {
public:
	void on_update(sys::state& state) noexcept override;
	void button_action(sys::state& state) noexcept override;
	bool is_active(sys::state& state) noexcept override;
};
class projection_mode_left : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class projection_mode_right : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class projection_mode_display : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override;
};
class fonts_mode_checkbox : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override;
	bool is_active(sys::state& state) noexcept override;
};

class master_volume : public scrollbar {
	void on_value_change(sys::state& state, int32_t v) noexcept final;
	void on_update(sys::state& state) noexcept final;
};
class music_volume : public scrollbar {
	void on_value_change(sys::state& state, int32_t v) noexcept final;
	void on_update(sys::state& state) noexcept final;
};
class effects_volume : public scrollbar {
	void on_value_change(sys::state& state, int32_t v) noexcept final;
	void on_update(sys::state& state) noexcept final;
};
class interface_volume : public scrollbar {
	void on_value_change(sys::state& state, int32_t v) noexcept final;
	void on_update(sys::state& state) noexcept final;
};

class autosave_left : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class autosave_right : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class autosave_display : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override;
};

class tooltip_mode_checkbox : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override;
	bool is_active(sys::state& state) noexcept override;
};
class fow_checkbox : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept override;
	void button_action(sys::state& state) noexcept override;
};

class map_label_left : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class map_label_right : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class map_label_display : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override;
};

class antialiasing_left : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class antialiasing_right : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class antialiasing_display : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override;
};

class music_player_left : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
};
class music_player_right : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override;
};
class music_player_display : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override;
};

struct notify_setting_update { };

class controls_menu_window : public window_element_base {
	bool setting_changed = false;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "auto_save_value") {
			return make_element_by_type<autosave_display>(state, id);
		} else if(name == "auto_save_left") {
			return make_element_by_type<autosave_left>(state, id);
		} else if(name == "auto_save_right") {
			return make_element_by_type<autosave_right>(state, id);
		} else if(name == "tooltip_mode_checkbox") {
			return make_element_by_type<tooltip_mode_checkbox>(state, id);
		} else {
			return nullptr;
		}
	}
	void on_hide(sys::state& state) noexcept override {
		if(setting_changed)
			state.save_user_settings();
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<notify_setting_update>()) {
			setting_changed = true;
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
class graphics_menu_window : public window_element_base {
	bool setting_changed = false;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "ui_scale_value") {
			return make_element_by_type<ui_scale_display>(state, id);
		} else if(name == "ui_scale_left") {
			return make_element_by_type<ui_scale_left>(state, id);
		} else if(name == "ui_scale_right") {
			return make_element_by_type<ui_scale_right>(state, id);
		} else if(name == "window_mode_checkbox") {
			return make_element_by_type<window_mode_checkbox>(state, id);
		} else if(name == "projection_left") {
			return make_element_by_type<projection_mode_left>(state, id);
		} else if(name == "projection_right") {
			return make_element_by_type<projection_mode_right>(state, id);
		} else if(name == "projection_value") {
			return make_element_by_type<projection_mode_display>(state, id);
		} else if(name == "fonts_checkbox") {
			return make_element_by_type<fonts_mode_checkbox>(state, id);
		} else if(name == "fow_checkbox") {
			return make_element_by_type<fow_checkbox>(state, id);
		} else if(name == "map_label_value") {
			return make_element_by_type<map_label_display>(state, id);
		} else if(name == "map_label_left") {
			return make_element_by_type<map_label_left>(state, id);
		} else if(name == "map_label_right") {
			return make_element_by_type<map_label_right>(state, id);
		} else if(name == "antialiasing_value") {
			return make_element_by_type<antialiasing_display>(state, id);
		} else if(name == "antialiasing_left") {
			return make_element_by_type<antialiasing_left>(state, id);
		} else if(name == "antialiasing_right") {
			return make_element_by_type<antialiasing_right>(state, id);
		} else {
			return nullptr;
		}
	}
	void on_hide(sys::state& state) noexcept override {
		if(setting_changed)
			state.save_user_settings();
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<notify_setting_update>()) {
			setting_changed = true;
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
class audio_menu_window : public window_element_base {
	bool setting_changed = false;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button")
			return make_element_by_type<generic_close_button>(state, id);
		else if(name == "background")
			return make_element_by_type<draggable_target>(state, id);
		else if(name == "master_volume_scroll_bar")
			return make_element_by_type<master_volume>(state, id);
		else if(name == "music_volume_scroll_bar")
			return make_element_by_type<music_volume>(state, id);
		else if(name == "interface_volume_scroll_bar")
			return make_element_by_type<interface_volume>(state, id);
		else if(name == "effect_volume_scroll_bar")
			return make_element_by_type<effects_volume>(state, id);
		else if(name == "music_player_value")
			return make_element_by_type<music_player_display>(state, id);
		else if(name == "music_player_left")
			return make_element_by_type<music_player_left>(state, id);
		else if(name == "music_player_right")
			return make_element_by_type<music_player_right>(state, id);
		else
			return nullptr;
	}
	void on_hide(sys::state& state) noexcept override {
		if(setting_changed)
			state.save_user_settings();
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<notify_setting_update>()) {
			setting_changed = true;
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

enum class main_menu_sub_window { controls, audio, graphics, message_settings };

class close_application_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		window::close_window(state);
	}
};

class mm_disabled_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		disabled = true;
	}

};

class save_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		command::save_game(state, state.local_player_nation, false);
	}
};

class save_and_quit_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		command::save_game(state, state.local_player_nation, true);
	}
};

class restricted_main_menu_window : public generic_tabbed_window<main_menu_sub_window> {
	controls_menu_window* controls_menu = nullptr;
	graphics_menu_window* graphics_menu = nullptr;
	audio_menu_window* audio_menu = nullptr;

public:

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "graphics") {
			auto ptr = make_element_by_type<generic_tab_button<main_menu_sub_window>>(state, id);
			ptr->target = main_menu_sub_window::graphics;
			return ptr;
		} else if(name == "sound") {
			auto ptr = make_element_by_type<generic_tab_button<main_menu_sub_window>>(state, id);
			ptr->target = main_menu_sub_window::audio;
			return ptr;
		} else if(name == "controls") {
			auto ptr = make_element_by_type<generic_tab_button<main_menu_sub_window>>(state, id);
			ptr->target = main_menu_sub_window::controls;
			return ptr;
		} else if(name == "message_settings") {
			return make_element_by_type<mm_disabled_button>(state, id);
		} else if(name == "save") {
			return make_element_by_type<mm_disabled_button>(state, id);
		} else if(name == "save_and_exit") {
			return make_element_by_type<mm_disabled_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "exit") {
			return make_element_by_type<close_application_button>(state, id);
		} else if(name == "alice_graphics_menu") {
			auto ptr = make_element_by_type<graphics_menu_window>(state, id);
			graphics_menu = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "alice_controls_menu") {
			auto ptr = make_element_by_type<controls_menu_window>(state, id);
			controls_menu = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "alice_audio_menu") {
			auto ptr = make_element_by_type<audio_menu_window>(state, id);
			audio_menu = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
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
			case main_menu_sub_window::controls:
				controls_menu->set_visible(state, true);
				break;
			case main_menu_sub_window::audio:
				audio_menu->set_visible(state, true);
				break;
			case main_menu_sub_window::graphics:
				graphics_menu->set_visible(state, true);
				break;
			case main_menu_sub_window::message_settings:
				break;
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class main_menu_window : public generic_tabbed_window<main_menu_sub_window> {
	controls_menu_window* controls_menu = nullptr;
	graphics_menu_window* graphics_menu = nullptr;
	audio_menu_window* audio_menu = nullptr;
	element_base* message_settings_menu = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		auto m = make_element_by_type<message_settings_window>(state, "alice_message_settings");
		message_settings_menu = m.get();
		m->set_visible(state, false);
		state.ui_state.root->add_child_to_front(std::move(m));
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "graphics") {
			auto ptr = make_element_by_type<generic_tab_button<main_menu_sub_window>>(state, id);
			ptr->target = main_menu_sub_window::graphics;
			return ptr;
		} else if(name == "sound") {
			auto ptr = make_element_by_type<generic_tab_button<main_menu_sub_window>>(state, id);
			ptr->target = main_menu_sub_window::audio;
			return ptr;
		} else if(name == "controls") {
			auto ptr = make_element_by_type<generic_tab_button<main_menu_sub_window>>(state, id);
			ptr->target = main_menu_sub_window::controls;
			return ptr;
		} else if(name == "message_settings") {
			auto ptr = make_element_by_type<generic_tab_button<main_menu_sub_window>>(state, id);
			ptr->target = main_menu_sub_window::message_settings;
			return ptr;
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "exit") {
			return make_element_by_type<close_application_button>(state, id);
		} else if(name == "save") {
			return make_element_by_type<save_button>(state, id);
		} else if(name == "save_and_exit") {
			return make_element_by_type<save_and_quit_button>(state, id);
		} else if(name == "alice_graphics_menu") {
			auto ptr = make_element_by_type<graphics_menu_window>(state, id);
			graphics_menu = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "alice_controls_menu") {
			auto ptr = make_element_by_type<controls_menu_window>(state, id);
			controls_menu = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "alice_audio_menu") {
			auto ptr = make_element_by_type<audio_menu_window>(state, id);
			audio_menu = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
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
		if(message_settings_menu)
			message_settings_menu->set_visible(state, false);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<main_menu_sub_window>()) {
			auto enum_val = any_cast<main_menu_sub_window>(payload);
			hide_subwindows(state);
			switch(enum_val) {
				case main_menu_sub_window::controls:
					controls_menu->set_visible(state, true);
					break;
				case main_menu_sub_window::audio:
					audio_menu->set_visible(state, true);
					break;
				case main_menu_sub_window::graphics:
					graphics_menu->set_visible(state, true);
					break;
				case main_menu_sub_window::message_settings:
					message_settings_menu->set_visible(state, true);
					state.ui_state.root->move_child_to_front(message_settings_menu);
					break;
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
