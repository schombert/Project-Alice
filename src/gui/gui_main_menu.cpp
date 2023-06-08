#include "gui_main_menu.hpp"
#include "sound.hpp"

#include <cstdio>

namespace ui {

void show_main_menu(sys::state& state) {
	if(!state.ui_state.main_menu) {
		auto new_mm = make_element_by_type<main_menu_window>(state, "alice_main_menu");
		state.ui_state.main_menu = new_mm.get();
		state.ui_state.root->add_child_to_front(std::move(new_mm));
	} else {
		state.ui_state.main_menu->set_visible(state, true);
		state.ui_state.root->move_child_to_front(state.ui_state.main_menu);
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
		Cyto::Any payload = notify_setting_update{};
		if(parent)
			parent->impl_get(state, payload);
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
		Cyto::Any payload = notify_setting_update{};
		if(parent)
			parent->impl_get(state, payload);
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

void window_mode_left::button_action(sys::state& state) noexcept {
	state.user_settings.prefer_fullscreen = !state.user_settings.prefer_fullscreen;
	window::set_borderless_full_screen(state, state.user_settings.prefer_fullscreen);
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void window_mode_left::on_update(sys::state& state) noexcept {
	disabled = (state.user_settings.prefer_fullscreen == true);
}
void window_mode_right::button_action(sys::state& state) noexcept {
	state.user_settings.prefer_fullscreen = !state.user_settings.prefer_fullscreen;
	window::set_borderless_full_screen(state, state.user_settings.prefer_fullscreen);
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void window_mode_right::on_update(sys::state& state) noexcept {
	disabled = (state.user_settings.prefer_fullscreen == false);
}
void window_mode_display::on_update(sys::state& state) noexcept {
	auto it = state.key_to_text_sequence.find(
			state.user_settings.prefer_fullscreen ? std::string_view("alice_mode_fullscreen") : std::string_view("alice_mode_window"));
	auto temp_string = (it != state.key_to_text_sequence.end()) ? text::produce_simple_string(state, it->second) : std::string("");
	set_text(state, temp_string);
}

void projection_mode_left::button_action(sys::state& state) noexcept {
	state.user_settings.map_is_globe = !state.user_settings.map_is_globe;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void projection_mode_left::on_update(sys::state& state) noexcept { }
void projection_mode_right::button_action(sys::state& state) noexcept {
	state.user_settings.map_is_globe = !state.user_settings.map_is_globe;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void projection_mode_right::on_update(sys::state& state) noexcept { }
void projection_mode_display::on_update(sys::state& state) noexcept {
	auto it = state.user_settings.map_is_globe ? std::string_view("map_projection_globe") : std::string_view("map_projection_flat");
	set_text(state, text::produce_simple_string(state, it));
}

void fonts_mode_left::button_action(sys::state& state) noexcept {
	state.user_settings.use_classic_fonts = !state.user_settings.use_classic_fonts;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);

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
void fonts_mode_left::on_update(sys::state& state) noexcept { }
void fonts_mode_right::button_action(sys::state& state) noexcept {
	state.user_settings.use_classic_fonts = !state.user_settings.use_classic_fonts;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);

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
void fonts_mode_right::on_update(sys::state& state) noexcept { }
void fonts_mode_display::on_update(sys::state& state) noexcept {
	auto it = state.key_to_text_sequence.find(
			state.user_settings.use_classic_fonts ? std::string_view("use_classic_fonts") : std::string_view("use_standard_fonts"));
	auto temp_string = (it != state.key_to_text_sequence.end()) ? text::produce_simple_string(state, it->second) : std::string("");
	set_text(state, temp_string);
}

void linegraph_mode_left::button_action(sys::state& state) noexcept {
	state.user_settings.fake_graphs = !state.user_settings.fake_graphs;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void linegraph_mode_left::on_update(sys::state& state) noexcept { }
void linegraph_mode_right::button_action(sys::state& state) noexcept {
	state.user_settings.fake_graphs = !state.user_settings.fake_graphs;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void linegraph_mode_right::on_update(sys::state& state) noexcept { }
void linegraph_mode_display::on_update(sys::state& state) noexcept {
	auto it = state.key_to_text_sequence.find(state.user_settings.fake_graphs ? std::string_view("linegraph_mode_aesthetic")
																																						: std::string_view("linegraph_mode_accurate"));
	auto temp_string = (it != state.key_to_text_sequence.end()) ? text::produce_simple_string(state, it->second) : std::string("");
	set_text(state, temp_string);
}

void gui_mode_left::button_action(sys::state& state) noexcept {
	/*
	uint8_t value = uint8_t(state.user_settings.guimode);
	(state.user_settings.guimode == sys::gui_modes{0}) ? state.user_settings.guimode = sys::gui_modes::faithful
														 : state.user_settings.guimode = sys::gui_modes{value--};
	*/
	state.user_settings.guimode = !state.user_settings.guimode;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void gui_mode_left::on_update(sys::state& state) noexcept { }
void gui_mode_right::button_action(sys::state& state) noexcept {
	/*
	uint8_t value = uint8_t(state.user_settings.guimode);
	(state.user_settings.guimode == sys::gui_modes{DEF_guimode_size})
		? state.user_settings.guimode = sys::gui_modes{DEF_guimode_size}
		: state.user_settings.guimode = sys::gui_modes{value++};
	*/
	state.user_settings.guimode = !state.user_settings.guimode;
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void gui_mode_right::on_update(sys::state& state) noexcept { }
void gui_mode_display::on_update(sys::state& state) noexcept {
	/*
	std::string var;
	switch(state.user_settings.guimode) {
	case sys::gui_modes::faithful:
		var = "gui_mode_faithful";
		break;
	case sys::gui_modes::inaccurate:
		var = "gui_mode_inaccurate";
		break;
	default:
		var = "gui_mode_faithful";
		break;
	};*/
	auto it = state.key_to_text_sequence.find(
			state.user_settings.guimode ? std::string_view("gui_mode_new") : std::string_view("gui_mode_faithful"));
	auto temp_string = (it != state.key_to_text_sequence.end()) ? text::produce_simple_string(state, it->second) : std::string("");
	set_text(state, temp_string);
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
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
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

	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void effects_volume::on_value_change(sys::state& state, int32_t v) noexcept {
	auto float_v = float(v) / 128.0f;
	state.user_settings.effects_volume = float_v;
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
}
void interface_volume::on_value_change(sys::state& state, int32_t v) noexcept {
	auto float_v = float(v) / 128.0f;
	state.user_settings.interface_volume = float_v;
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	Cyto::Any payload = notify_setting_update{};
	if(parent)
		parent->impl_get(state, payload);
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

} // namespace ui
