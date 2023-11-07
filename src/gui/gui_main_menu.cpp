#include "gui_main_menu.hpp"
#include "sound.hpp"

#include <cstdio>

namespace ui {

void show_main_menu(sys::state& state) {
	if(state.mode == sys::game_mode_type::pick_nation) {
		if(!state.ui_state.r_main_menu) {
			auto new_mm = make_element_by_type<restricted_main_menu_window>(state, "alice_main_menu");
			state.ui_state.r_main_menu = new_mm.get();
			state.ui_state.nation_picker->add_child_to_front(std::move(new_mm));
		} else {
			state.ui_state.r_main_menu->set_visible(state, true);
			state.ui_state.nation_picker->move_child_to_front(state.ui_state.r_main_menu);
		}
	} else if(state.mode == sys::game_mode_type::in_game) {
		if(!state.ui_state.main_menu) {
			auto new_mm = make_element_by_type<main_menu_window>(state, "alice_main_menu");
			state.ui_state.main_menu = new_mm.get();
			state.ui_state.root->add_child_to_front(std::move(new_mm));
		} else {
			state.ui_state.main_menu->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.main_menu);
		}
	}
}

uint32_t get_ui_scale_index(float current_scale) {
	for(uint32_t i = 0; i < sys::ui_scales_count; ++i) {
		if(current_scale <= sys::ui_scales[i])
			return i;
	}
	return uint32_t(sys::ui_scales_count - 1);
}

void ui_scale_left::button_action(sys::state& state) noexcept {
	auto scale_index = get_ui_scale_index(state.user_settings.ui_scale);
	if(scale_index > 0) {
		state.update_ui_scale(sys::ui_scales[scale_index - 1]);
		send(state, parent, notify_setting_update{});
	}
}
void ui_scale_left::on_update(sys::state& state) noexcept {
	auto scale_index = get_ui_scale_index(state.user_settings.ui_scale);
	disabled = (scale_index == 0);
}
void ui_scale_right::button_action(sys::state& state) noexcept {
	auto scale_index = get_ui_scale_index(state.user_settings.ui_scale);
	if(scale_index < uint32_t(sys::ui_scales_count - 1)) {
		state.update_ui_scale(sys::ui_scales[scale_index + 1]);
		send(state, parent, notify_setting_update{});
	}
}
void ui_scale_right::on_update(sys::state& state) noexcept {
	auto scale_index = get_ui_scale_index(state.user_settings.ui_scale);
	disabled = (scale_index >= uint32_t(sys::ui_scales_count - 1));
}
void ui_scale_display::on_update(sys::state& state) noexcept {
	std::string temp(16, '\0');
	std::snprintf(temp.data(), temp.size(), "%.2fx", state.user_settings.ui_scale);
	set_text(state, temp);
}

void autosave_left::button_action(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.autosaves);
	if(scale_index > 0) {
		state.user_settings.autosaves = sys::autosave_frequency(scale_index - 1);
		send(state, parent, notify_setting_update{});
	}
}
void autosave_left::on_update(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.autosaves);
	disabled = (scale_index == 0);
}
void autosave_right::button_action(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.autosaves);
	if(scale_index < 3) {
		state.user_settings.autosaves = sys::autosave_frequency(scale_index + 1);
		send(state, parent, notify_setting_update{});
	}
}
void autosave_right::on_update(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.autosaves);
	disabled = (scale_index >= 3);
}
void autosave_display::on_update(sys::state& state) noexcept {
	switch(state.user_settings.autosaves) {
	case sys::autosave_frequency::none:
		set_text(state, text::produce_simple_string(state, "auto_save_0"));
		break;
	case sys::autosave_frequency::daily:
		set_text(state, text::produce_simple_string(state, "auto_save_1"));
		break;
	case sys::autosave_frequency::monthly:
		set_text(state, text::produce_simple_string(state, "auto_save_2"));
		break;
	case sys::autosave_frequency::yearly:
		set_text(state, text::produce_simple_string(state, "auto_save_3"));
		break;
	default:
		set_text(state, "???");
		break;
	}
}

void tooltip_mode_checkbox::button_action(sys::state& state) noexcept {
	state.user_settings.bind_tooltip_mouse = !state.user_settings.bind_tooltip_mouse;
	send(state, parent, notify_setting_update{});
}
bool tooltip_mode_checkbox::is_active(sys::state& state) noexcept {
	return state.user_settings.bind_tooltip_mouse;
}

void fow_checkbox::on_create(sys::state& state) noexcept {
	checkbox_button::on_create(state);
	disabled = (state.network_mode != sys::network_mode_type::single_player);
}
bool fow_checkbox::is_active(sys::state& state) noexcept {
	return state.user_settings.fow_enabled;
}
void fow_checkbox::button_action(sys::state& state) noexcept {
	state.user_settings.fow_enabled = !state.user_settings.fow_enabled;
	state.map_state.map_data.update_fog_of_war(state);
	send(state, parent, notify_setting_update{});
}

void map_label_left::button_action(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.map_label);
	if(scale_index > 0) {
		state.user_settings.map_label = sys::map_label_mode(scale_index - 1);
		state.province_ownership_changed.store(true, std::memory_order::release);
		send(state, parent, notify_setting_update{});
	}
}
void map_label_left::on_update(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.map_label);
	disabled = (scale_index == 0);
}
void map_label_right::button_action(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.map_label);
	if(scale_index < 3) {
		state.user_settings.map_label = sys::map_label_mode(scale_index + 1);
		state.province_ownership_changed.store(true, std::memory_order::release);
		send(state, parent, notify_setting_update{});
	}
}
void map_label_right::on_update(sys::state& state) noexcept {
	auto scale_index = uint8_t(state.user_settings.map_label);
	disabled = (scale_index >= 3);
}
void map_label_display::on_update(sys::state& state) noexcept {
	switch(state.user_settings.map_label) {
	case sys::map_label_mode::none:
		set_text(state, text::produce_simple_string(state, "map_label_0"));
		break;
	case sys::map_label_mode::linear:
		set_text(state, text::produce_simple_string(state, "map_label_1"));
		break;
	case sys::map_label_mode::quadratic:
		set_text(state, text::produce_simple_string(state, "map_label_2"));
		break;
	case sys::map_label_mode::cubic:
		set_text(state, text::produce_simple_string(state, "map_label_3"));
		break;
	default:
		set_text(state, "???");
		break;
	}
}

void antialiasing_left::button_action(sys::state& state) noexcept {
	if(state.user_settings.antialias_level > 0) {
		state.user_settings.antialias_level -= 1;
		ogl::deinitialize_msaa(state);
		ogl::initialize_msaa(state, state.x_size, state.y_size);
		send(state, parent, notify_setting_update{});
	}
}
void antialiasing_left::on_update(sys::state& state) noexcept {
	disabled = (state.user_settings.antialias_level == 0);
}
void antialiasing_right::button_action(sys::state& state) noexcept {
	if(state.user_settings.antialias_level < 16) {
		state.user_settings.antialias_level += 1;
		ogl::deinitialize_msaa(state);
		ogl::initialize_msaa(state, state.x_size, state.y_size);
		send(state, parent, notify_setting_update{});
	}
}
void antialiasing_right::on_update(sys::state& state) noexcept {
	disabled = (state.user_settings.antialias_level >= 16);
}
void antialiasing_display::on_update(sys::state& state) noexcept {
	set_text(state, "x" + std::to_string(int32_t(state.user_settings.antialias_level)));
}

/*
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
*/

void window_mode_checkbox::on_update(sys::state& state) noexcept {

}
void window_mode_checkbox::button_action(sys::state& state) noexcept {
	state.user_settings.prefer_fullscreen = !state.user_settings.prefer_fullscreen;
	window::set_borderless_full_screen(state, state.user_settings.prefer_fullscreen);
	send(state, parent, notify_setting_update{});
}
bool window_mode_checkbox::is_active(sys::state& state) noexcept {
	return state.user_settings.prefer_fullscreen;
}

void projection_mode_left::button_action(sys::state& state) noexcept {
	state.user_settings.map_is_globe = !state.user_settings.map_is_globe;
	send(state, parent, notify_setting_update{});
}
void projection_mode_left::on_update(sys::state& state) noexcept { }
void projection_mode_right::button_action(sys::state& state) noexcept {
	state.user_settings.map_is_globe = !state.user_settings.map_is_globe;
	send(state, parent, notify_setting_update{});
}
void projection_mode_right::on_update(sys::state& state) noexcept { }
void projection_mode_display::on_update(sys::state& state) noexcept {
	auto it = state.user_settings.map_is_globe ? std::string_view("map_projection_globe") : std::string_view("map_projection_flat");
	set_text(state, text::produce_simple_string(state, it));
}

void fonts_mode_checkbox::button_action(sys::state& state) noexcept {
	state.user_settings.use_classic_fonts = !state.user_settings.use_classic_fonts;
	send(state, parent, notify_setting_update{});

	state.ui_state.units_root->impl_on_reset_text(state);
	state.ui_state.rgos_root->impl_on_reset_text(state);
	state.ui_state.root->impl_on_reset_text(state);
	state.ui_state.tooltip->set_visible(state, false);
	state.ui_state.last_tooltip = nullptr;

	if(state.user_settings.use_classic_fonts) {
		state.ui_state.tooltip_font = text::name_into_font_id(state, "vic_18_black");
	} else {
		state.ui_state.tooltip_font = text::name_into_font_id(state, "ToolTip_Font");
	}
}
bool fonts_mode_checkbox::is_active(sys::state& state) noexcept {
	return state.user_settings.use_classic_fonts;
}

void master_volume::on_value_change(sys::state& state, int32_t v) noexcept {
	auto float_v = float(v) / 128.0f;

	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.master_volume = float_v;
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}
	send(state, parent, notify_setting_update{});
}
void music_volume::on_value_change(sys::state& state, int32_t v) noexcept {
	auto float_v = float(v) / 128.0f;
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.music_volume = float_v;
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}
	send(state, parent, notify_setting_update{});
}
void effects_volume::on_value_change(sys::state& state, int32_t v) noexcept {
	auto float_v = float(v) / 128.0f;
	state.user_settings.effects_volume = float_v;
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	send(state, parent, notify_setting_update{});
}
void interface_volume::on_value_change(sys::state& state, int32_t v) noexcept {
	auto float_v = float(v) / 128.0f;
	state.user_settings.interface_volume = float_v;
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	send(state, parent, notify_setting_update{});
}

void master_volume::on_update(sys::state& state) noexcept {
	update_raw_value(state, int32_t(state.user_settings.master_volume * 128.0f));
}
void music_volume::on_update(sys::state& state) noexcept {
	update_raw_value(state, int32_t(state.user_settings.music_volume * 128.0f));
}
void effects_volume::on_update(sys::state& state) noexcept {
	update_raw_value(state, int32_t(state.user_settings.effects_volume * 128.0f));
}
void interface_volume::on_update(sys::state& state) noexcept {
	update_raw_value(state, int32_t(state.user_settings.interface_volume * 128.0f));
}


void music_player_left::button_action(sys::state& state) noexcept {
	sound::play_new_track(state);
	send(state, parent, notify_setting_update{});
}
void music_player_right::button_action(sys::state& state) noexcept {
	sound::play_new_track(state);
	send(state, parent, notify_setting_update{});
}
void music_player_display::on_update(sys::state& state) noexcept {
	set_text(state, simple_fs::native_to_utf8(sound::get_current_track_name(state)));
}

} // namespace ui
