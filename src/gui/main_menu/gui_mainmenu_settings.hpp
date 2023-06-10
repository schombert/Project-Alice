#pragma once

#include "gui_element_types.hpp"

namespace ui {

class mainmenu_game_settings_subwindow : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "counterdistance_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "counterdistance_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else if(name == "autosave_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "autosave_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "decrease_autosave_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "increase_autosave_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "difficulty_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "difficulty_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "decrease_difficulty_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "increase_difficulty_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "shortcut_checkbox") {
			return make_element_by_type<checkbox_button>(state, id);

		} else if(name == "shortcut_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class mainmenu_video_settings_subwindow : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "resolution_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "resolution_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "decrease_resolution_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "increase_resolution_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "window_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "window_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "decrease_window_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "increase_window_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "refreshrate_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "refreshrate_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "decrease_refreshrate_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "increase_refreshrate_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "multisample_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "multisample_value") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "decrease_multisample_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "increase_multisample_button") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "gamma_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "gamma_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else if(name == "details_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "details_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else if(name == "water_checkbox") {
			return make_element_by_type<checkbox_button>(state, id);

		} else if(name == "water_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else {
			return nullptr;
		}
	}
};

class mainmenu_audio_settings_subwindow : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "mastervolume_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "mastervolume_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else if(name == "effectvolume_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "effectvolume_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else if(name == "musicvolume_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "musicvolume_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else {
			return nullptr;
		}
	}
};

class mainmenu_controls_settings_subwindow : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "scrollspeed_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "scrollspeed_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else if(name == "zoomspeed_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "zoomspeed_slider") {
			auto ptr = make_element_by_type<scrollbar>(state, id);
			ptr->track->base_data.position.y -= 20;
			return ptr;

		} else {
			return nullptr;
		}
	}
};

enum class settings_tab : uint8_t {
	game,
	video,
	audio,
	controls
};

class mainmenu_settings_window : public window_element_base {
private:
	window_element_base* game_settings_win = nullptr;
	window_element_base* video_settings_win = nullptr;
	window_element_base* audio_settings_win = nullptr;
	window_element_base* controls_settings_win = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "settings_bg") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "settings_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "settings_tab_1") {
			auto ptr = make_element_by_type<generic_tab_button<settings_tab>>(state, id);
			ptr->target = settings_tab::game;
			return ptr;

		} else if(name == "settings_tab_2") {
			auto ptr = make_element_by_type<generic_tab_button<settings_tab>>(state, id);
			ptr->target = settings_tab::video;
			return ptr;

		} else if(name == "settings_tab_3") {
			auto ptr = make_element_by_type<generic_tab_button<settings_tab>>(state, id);
			ptr->target = settings_tab::audio;
			return ptr;

		} else if(name == "settings_tab_4") {
			auto ptr = make_element_by_type<generic_tab_button<settings_tab>>(state, id);
			ptr->target = settings_tab::controls;
			return ptr;

		} else if(name == "gamesettings_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "videosettings_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "audiosettings_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "controlssettings_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "menu_settings_game") {
			auto ptr = make_element_by_type<mainmenu_game_settings_subwindow>(state, id);
			game_settings_win = ptr.get();
			ptr->set_visible(state, true);
			return ptr;

		} else if(name == "menu_settings_video") {
			auto ptr = make_element_by_type<mainmenu_video_settings_subwindow>(state, id);
			video_settings_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "menu_settings_audio") {
			auto ptr = make_element_by_type<mainmenu_audio_settings_subwindow>(state, id);
			audio_settings_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "menu_settings_controls") {
			auto ptr = make_element_by_type<mainmenu_controls_settings_subwindow>(state, id);
			controls_settings_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;

		} else if(name == "applybutton") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "backbutton") {
			return make_element_by_type<generic_close_button>(state, id);

		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<settings_tab>()) {
			auto content = any_cast<settings_tab>(payload);
			switch(content) {
				case settings_tab::game:
					game_settings_win->set_visible(state, true);
					video_settings_win->set_visible(state, false);
					audio_settings_win->set_visible(state, false);
					controls_settings_win->set_visible(state, false);
					break;
				case settings_tab::video:
					game_settings_win->set_visible(state, false);
					video_settings_win->set_visible(state, true);
					audio_settings_win->set_visible(state, false);
					controls_settings_win->set_visible(state, false);
					break;
				case settings_tab::audio:
					game_settings_win->set_visible(state, false);
					video_settings_win->set_visible(state, false);
					audio_settings_win->set_visible(state, true);
					controls_settings_win->set_visible(state, false);
					break;
				case settings_tab::controls:
					game_settings_win->set_visible(state, false);
					video_settings_win->set_visible(state, false);
					audio_settings_win->set_visible(state, false);
					controls_settings_win->set_visible(state, true);
					break;
				default:
					game_settings_win->set_visible(state, true);
					video_settings_win->set_visible(state, false);
					audio_settings_win->set_visible(state, false);
					controls_settings_win->set_visible(state, false);
					break;
			};
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
