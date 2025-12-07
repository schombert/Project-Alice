namespace alice_ui {
struct main_menu_base_save_and_exit_t;
struct main_menu_base_save_t;
struct main_menu_base_exit_t;
struct main_menu_base_graphics_options_t;
struct main_menu_base_sound_options_t;
struct main_menu_base_controls_options_t;
struct main_menu_base_message_settings_options_t;
struct main_menu_base_save_name_t;
struct main_menu_base_save_warning_icon_t;
struct main_menu_base_t;
struct main_menu_sound_master_volume_minimum_t;
struct main_menu_sound_master_volume_less_t;
struct main_menu_sound_master_voume_amount_t;
struct main_menu_sound_master_volume_more_t;
struct main_menu_sound_master_volume_maximum_t;
struct main_menu_sound_music_volume_minimum_t;
struct main_menu_sound_music_volume_less_t;
struct main_menu_sound_music_volume_amount_t;
struct main_menu_sound_music_volume_more_t;
struct main_menu_sound_music_volume_max_t;
struct main_menu_sound_interface_volume_min_t;
struct main_menu_sound_interface_volume_less_t;
struct main_menu_sound_interface_volume_amount_t;
struct main_menu_sound_interface_volume_more_t;
struct main_menu_sound_interface_volume_max_t;
struct main_menu_sound_effect_volume_min_t;
struct main_menu_sound_effect_volume_less_t;
struct main_menu_sound_effect_volume_amount_t;
struct main_menu_sound_effect_volume_more_t;
struct main_menu_sound_effect_volume_max_t;
struct main_menu_sound_mute_on_lose_focus_t;
struct main_menu_sound_t;
struct main_menu_misc_auto_save_dropdown_t;
struct main_menu_misc_language_dropdown_t;
struct main_menu_misc_zoom_mode_dropdown_t;
struct main_menu_misc_zoom_speed_dropdown_t;
struct main_menu_misc_floating_tooltip_t;
struct main_menu_misc_wasd_movement_t;
struct main_menu_misc_edge_scrolling_t;
struct main_menu_misc_confirm_disband_t;
struct main_menu_misc_messages_as_popups_t;
struct main_menu_misc_detailed_tooltips_t;
struct main_menu_misc_t;
struct main_menu_string_dropdown_choice_contents_t;
struct main_menu_string_dropdown_choice_t;
struct main_menu_graphics_projection_dropdown_t;
struct main_menu_graphics_ui_scale_dropdown_t;
struct main_menu_graphics_map_label_dropdown_t;
struct main_menu_graphics_antialiasing_dropdown_t;
struct main_menu_graphics_gamma_dropdown_t;
struct main_menu_graphics_vassal_color_dropdown_t;
struct main_menu_graphics_graphics_details_dropdown_t;
struct main_menu_graphics_window_mode_label_t;
struct main_menu_graphics_mouse_left_click_mode_label_t;
struct main_menu_graphics_black_map_font_label_t;
struct main_menu_graphics_river_label_t;
struct main_menu_graphics_railroad_label_t;
struct main_menu_graphics_fonts_label_t;
struct main_menu_graphics_t;
struct main_menu_message_settings_message_notify_rebels_defeat_t;
struct main_menu_message_settings_reset_t;
struct main_menu_message_settings_t;
struct main_menu_message_setting_row_overlay_t;
struct main_menu_message_setting_row_setting_name_t;
struct main_menu_message_setting_row_self_icon_t;
struct main_menu_message_setting_row_interesting_icon_t;
struct main_menu_message_setting_row_other_icon_t;
struct main_menu_message_setting_row_t;
struct main_menu_message_setting_header_contents_t;
struct main_menu_message_setting_header_t;
struct main_menu_base_save_and_exit_t : public alice_ui::template_text_button {
// BEGIN base::save_and_exit::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_save_t : public alice_ui::template_text_button {
// BEGIN base::save::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_exit_t : public alice_ui::template_text_button {
// BEGIN base::exit::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_graphics_options_t : public alice_ui::template_text_button {
// BEGIN base::graphics_options::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_sound_options_t : public alice_ui::template_text_button {
// BEGIN base::sound_options::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_controls_options_t : public alice_ui::template_text_button {
// BEGIN base::controls_options::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_message_settings_options_t : public alice_ui::template_text_button {
// BEGIN base::message_settings_options::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_save_name_t : public ui::edit_box_element_base {
// BEGIN base::save_name::variables
// END
	void edit_box_update(sys::state& state, std::u16string_view s) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
};
struct main_menu_base_save_warning_icon_t : public alice_ui::template_icon_graphic {
// BEGIN base::save_warning_icon::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_master_volume_minimum_t : public alice_ui::template_icon_button {
// BEGIN sound::master_volume_minimum::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_master_volume_less_t : public alice_ui::template_icon_button {
// BEGIN sound::master_volume_less::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_master_voume_amount_t : public alice_ui::template_label {
// BEGIN sound::master_voume_amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_master_volume_more_t : public alice_ui::template_icon_button {
// BEGIN sound::master_volume_more::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_master_volume_maximum_t : public alice_ui::template_icon_button {
// BEGIN sound::master_volume_maximum::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_music_volume_minimum_t : public alice_ui::template_icon_button {
// BEGIN sound::music_volume_minimum::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_music_volume_less_t : public alice_ui::template_icon_button {
// BEGIN sound::music_volume_less::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_music_volume_amount_t : public alice_ui::template_label {
// BEGIN sound::music_volume_amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_music_volume_more_t : public alice_ui::template_icon_button {
// BEGIN sound::music_volume_more::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_music_volume_max_t : public alice_ui::template_icon_button {
// BEGIN sound::music_volume_max::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_interface_volume_min_t : public alice_ui::template_icon_button {
// BEGIN sound::interface_volume_min::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_interface_volume_less_t : public alice_ui::template_icon_button {
// BEGIN sound::interface_volume_less::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_interface_volume_amount_t : public alice_ui::template_label {
// BEGIN sound::interface_volume_amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_interface_volume_more_t : public alice_ui::template_icon_button {
// BEGIN sound::interface_volume_more::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_interface_volume_max_t : public alice_ui::template_icon_button {
// BEGIN sound::interface_volume_max::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_effect_volume_min_t : public alice_ui::template_icon_button {
// BEGIN sound::effect_volume_min::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_effect_volume_less_t : public alice_ui::template_icon_button {
// BEGIN sound::effect_volume_less::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_effect_volume_amount_t : public alice_ui::template_label {
// BEGIN sound::effect_volume_amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_effect_volume_more_t : public alice_ui::template_icon_button {
// BEGIN sound::effect_volume_more::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_effect_volume_max_t : public alice_ui::template_icon_button {
// BEGIN sound::effect_volume_max::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_sound_mute_on_lose_focus_t : public alice_ui::template_toggle_button {
// BEGIN sound::mute_on_lose_focus::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_auto_save_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN misc::auto_save_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_language_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN misc::language_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_zoom_mode_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN misc::zoom_mode_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_zoom_speed_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN misc::zoom_speed_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_floating_tooltip_t : public alice_ui::template_toggle_button {
// BEGIN misc::floating_tooltip::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_wasd_movement_t : public alice_ui::template_toggle_button {
// BEGIN misc::wasd_movement::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_edge_scrolling_t : public alice_ui::template_toggle_button {
// BEGIN misc::edge_scrolling::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_confirm_disband_t : public alice_ui::template_toggle_button {
// BEGIN misc::confirm_disband::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_messages_as_popups_t : public alice_ui::template_toggle_button {
// BEGIN misc::messages_as_popups::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_misc_detailed_tooltips_t : public alice_ui::template_toggle_button {
// BEGIN misc::detailed_tooltips::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_string_dropdown_choice_contents_t : public alice_ui::template_label {
// BEGIN string_dropdown_choice::contents::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_projection_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN graphics::projection_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_ui_scale_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN graphics::ui_scale_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_map_label_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN graphics::map_label_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_antialiasing_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN graphics::antialiasing_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_gamma_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN graphics::gamma_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_vassal_color_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN graphics::vassal_color_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_graphics_details_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN graphics::graphics_details_dropdown::variables
// END
	struct string_dropdown_choice_option { int32_t index; std::string display_value; };
	std::vector<string_dropdown_choice_option> list_contents;
	std::vector<std::unique_ptr<main_menu_string_dropdown_choice_t >> list_pool;
	std::unique_ptr<main_menu_string_dropdown_choice_t> label_window_internal;
	void add_item( int32_t index,  std::string display_value);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_window_mode_label_t : public alice_ui::template_toggle_button {
// BEGIN graphics::window_mode_label::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_mouse_left_click_mode_label_t : public alice_ui::template_toggle_button {
// BEGIN graphics::mouse_left_click_mode_label::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_black_map_font_label_t : public alice_ui::template_toggle_button {
// BEGIN graphics::black_map_font_label::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_river_label_t : public alice_ui::template_toggle_button {
// BEGIN graphics::river_label::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_railroad_label_t : public alice_ui::template_toggle_button {
// BEGIN graphics::railroad_label::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_graphics_fonts_label_t : public alice_ui::template_toggle_button {
// BEGIN graphics::fonts_label::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_message_settings_message_notify_rebels_defeat_t : public alice_ui::template_toggle_button {
// BEGIN message_settings::message_notify_rebels_defeat::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_message_settings_reset_t : public alice_ui::template_text_button {
// BEGIN message_settings::reset::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_message_settings_message_settings_list_t : public layout_generator {
// BEGIN message_settings::message_settings_list::variables
// END
	struct message_setting_row_option { int32_t id; };
	std::vector<std::unique_ptr<ui::element_base>> message_setting_row_pool;
	int32_t message_setting_row_pool_used = 0;
	void add_message_setting_row( int32_t id);
	std::vector<std::unique_ptr<ui::element_base>> message_setting_header_pool;
	int32_t message_setting_header_pool_used = 0;
	std::vector<std::variant<std::monostate, message_setting_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct main_menu_message_setting_row_overlay_t : public ui::element_base {
// BEGIN message_setting_row::overlay::variables
// END
	int32_t template_id = -1;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_create(sys::state& state) noexcept override;
};
struct main_menu_message_setting_row_setting_name_t : public alice_ui::template_label {
// BEGIN message_setting_row::setting_name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_message_setting_row_self_icon_t : public alice_ui::template_icon_button {
// BEGIN message_setting_row::self_icon::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	bool button_right_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_message_setting_row_interesting_icon_t : public alice_ui::template_icon_button {
// BEGIN message_setting_row::interesting_icon::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	bool button_right_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_message_setting_row_other_icon_t : public alice_ui::template_icon_button {
// BEGIN message_setting_row::other_icon::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	bool button_right_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_message_setting_header_contents_t : public ui::element_base {
// BEGIN message_setting_header::contents::variables
// END
	int32_t template_id = -1;
	text::layout setting_name_internal_layout;
	std::string setting_name_cached_text;
	text::layout self_internal_layout;
	std::string self_cached_text;
	text::layout interesting_internal_layout;
	std::string interesting_cached_text;
	text::layout other_internal_layout;
	std::string other_cached_text;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click || type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::consumed;
		} else  {
			return ui::message_result::unseen;
		}
	}
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void on_reset_text(sys::state & state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct main_menu_base_t : public layout_window_element {
// BEGIN base::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_bg_graphic> bg_extension;
	std::unique_ptr<template_icon_graphic> logo;
	std::unique_ptr<main_menu_base_save_and_exit_t> save_and_exit;
	std::unique_ptr<main_menu_base_save_t> save;
	std::unique_ptr<main_menu_base_exit_t> exit;
	std::unique_ptr<main_menu_base_graphics_options_t> graphics_options;
	std::unique_ptr<main_menu_base_sound_options_t> sound_options;
	std::unique_ptr<main_menu_base_controls_options_t> controls_options;
	std::unique_ptr<main_menu_base_message_settings_options_t> message_settings_options;
	std::unique_ptr<template_icon_graphic> end_icon;
	std::unique_ptr<template_label> save_name_label;
	std::unique_ptr<main_menu_base_save_name_t> save_name;
	std::unique_ptr<main_menu_base_save_warning_icon_t> save_warning_icon;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto location_abs = get_absolute_location(state, *this);
		if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
			ui::xy_pair new_abs_pos = location_abs;
			new_abs_pos.x += int16_t(x - oldx);
			new_abs_pos.y += int16_t(y - oldy);
			if(ui::ui_width(state) > base_data.size.x)
				new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui::ui_width(state) - base_data.size.x));
			if(ui::ui_height(state) > base_data.size.y)
				new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui::ui_height(state) - base_data.size.y));
			if(state_is_rtl(state)) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_main_menu_base(sys::state& state);
struct main_menu_sound_t : public layout_window_element {
// BEGIN sound::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> window_title;
	std::unique_ptr<main_menu_sound_master_volume_minimum_t> master_volume_minimum;
	std::unique_ptr<main_menu_sound_master_volume_less_t> master_volume_less;
	std::unique_ptr<main_menu_sound_master_voume_amount_t> master_voume_amount;
	std::unique_ptr<main_menu_sound_master_volume_more_t> master_volume_more;
	std::unique_ptr<main_menu_sound_master_volume_maximum_t> master_volume_maximum;
	std::unique_ptr<template_label> music_volume_label;
	std::unique_ptr<template_label> master_volume_label;
	std::unique_ptr<main_menu_sound_music_volume_minimum_t> music_volume_minimum;
	std::unique_ptr<main_menu_sound_music_volume_less_t> music_volume_less;
	std::unique_ptr<main_menu_sound_music_volume_amount_t> music_volume_amount;
	std::unique_ptr<main_menu_sound_music_volume_more_t> music_volume_more;
	std::unique_ptr<main_menu_sound_music_volume_max_t> music_volume_max;
	std::unique_ptr<template_label> interface_volume_label;
	std::unique_ptr<main_menu_sound_interface_volume_min_t> interface_volume_min;
	std::unique_ptr<main_menu_sound_interface_volume_less_t> interface_volume_less;
	std::unique_ptr<main_menu_sound_interface_volume_amount_t> interface_volume_amount;
	std::unique_ptr<main_menu_sound_interface_volume_more_t> interface_volume_more;
	std::unique_ptr<main_menu_sound_interface_volume_max_t> interface_volume_max;
	std::unique_ptr<template_label> effect_volume_label;
	std::unique_ptr<main_menu_sound_effect_volume_min_t> effect_volume_min;
	std::unique_ptr<main_menu_sound_effect_volume_less_t> effect_volume_less;
	std::unique_ptr<main_menu_sound_effect_volume_amount_t> effect_volume_amount;
	std::unique_ptr<main_menu_sound_effect_volume_more_t> effect_volume_more;
	std::unique_ptr<main_menu_sound_effect_volume_max_t> effect_volume_max;
	std::unique_ptr<template_bg_graphic> divider;
	std::unique_ptr<main_menu_sound_mute_on_lose_focus_t> mute_on_lose_focus;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto location_abs = get_absolute_location(state, *this);
		if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
			ui::xy_pair new_abs_pos = location_abs;
			new_abs_pos.x += int16_t(x - oldx);
			new_abs_pos.y += int16_t(y - oldy);
			if(ui::ui_width(state) > base_data.size.x)
				new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui::ui_width(state) - base_data.size.x));
			if(ui::ui_height(state) > base_data.size.y)
				new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui::ui_height(state) - base_data.size.y));
			if(state_is_rtl(state)) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_main_menu_sound(sys::state& state);
struct main_menu_misc_t : public layout_window_element {
// BEGIN misc::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> window_title;
	std::unique_ptr<template_label> auto_save_label;
	std::unique_ptr<main_menu_misc_auto_save_dropdown_t> auto_save_dropdown;
	std::unique_ptr<template_label> language_label;
	std::unique_ptr<main_menu_misc_language_dropdown_t> language_dropdown;
	std::unique_ptr<template_label> zoom_mode_label;
	std::unique_ptr<main_menu_misc_zoom_mode_dropdown_t> zoom_mode_dropdown;
	std::unique_ptr<template_label> zoom_speed_label;
	std::unique_ptr<main_menu_misc_zoom_speed_dropdown_t> zoom_speed_dropdown;
	std::unique_ptr<main_menu_misc_floating_tooltip_t> floating_tooltip;
	std::unique_ptr<main_menu_misc_wasd_movement_t> wasd_movement;
	std::unique_ptr<main_menu_misc_edge_scrolling_t> edge_scrolling;
	std::unique_ptr<main_menu_misc_confirm_disband_t> confirm_disband;
	std::unique_ptr<main_menu_misc_messages_as_popups_t> messages_as_popups;
	std::unique_ptr<main_menu_misc_detailed_tooltips_t> detailed_tooltips;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto location_abs = get_absolute_location(state, *this);
		if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
			ui::xy_pair new_abs_pos = location_abs;
			new_abs_pos.x += int16_t(x - oldx);
			new_abs_pos.y += int16_t(y - oldy);
			if(ui::ui_width(state) > base_data.size.x)
				new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui::ui_width(state) - base_data.size.x));
			if(ui::ui_height(state) > base_data.size.y)
				new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui::ui_height(state) - base_data.size.y));
			if(state_is_rtl(state)) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_main_menu_misc(sys::state& state);
struct main_menu_string_dropdown_choice_t : public layout_window_element {
// BEGIN string_dropdown_choice::variables
// END
	int32_t index;
	std::string display_value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<main_menu_string_dropdown_choice_contents_t> contents;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "index") {
			return (void*)(&index);
		}
		if(name_parameter == "display_value") {
			return (void*)(&display_value);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_main_menu_string_dropdown_choice(sys::state& state);
struct main_menu_graphics_t : public layout_window_element {
// BEGIN graphics::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> title;
	std::unique_ptr<template_label> projection_label;
	std::unique_ptr<main_menu_graphics_projection_dropdown_t> projection_dropdown;
	std::unique_ptr<template_label> ui_scale_label;
	std::unique_ptr<main_menu_graphics_ui_scale_dropdown_t> ui_scale_dropdown;
	std::unique_ptr<template_label> map_label_label;
	std::unique_ptr<main_menu_graphics_map_label_dropdown_t> map_label_dropdown;
	std::unique_ptr<template_label> antialiasing_label;
	std::unique_ptr<main_menu_graphics_antialiasing_dropdown_t> antialiasing_dropdown;
	std::unique_ptr<template_label> gamma_label;
	std::unique_ptr<main_menu_graphics_gamma_dropdown_t> gamma_dropdown;
	std::unique_ptr<template_label> vassal_color_label;
	std::unique_ptr<main_menu_graphics_vassal_color_dropdown_t> vassal_color_dropdown;
	std::unique_ptr<template_label> graphics_details_label;
	std::unique_ptr<main_menu_graphics_graphics_details_dropdown_t> graphics_details_dropdown;
	std::unique_ptr<main_menu_graphics_window_mode_label_t> window_mode_label;
	std::unique_ptr<main_menu_graphics_mouse_left_click_mode_label_t> mouse_left_click_mode_label;
	std::unique_ptr<main_menu_graphics_black_map_font_label_t> black_map_font_label;
	std::unique_ptr<main_menu_graphics_river_label_t> river_label;
	std::unique_ptr<main_menu_graphics_railroad_label_t> railroad_label;
	std::unique_ptr<main_menu_graphics_fonts_label_t> fonts_label;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::consumed;
	}
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto location_abs = get_absolute_location(state, *this);
		if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
			ui::xy_pair new_abs_pos = location_abs;
			new_abs_pos.x += int16_t(x - oldx);
			new_abs_pos.y += int16_t(y - oldy);
			if(ui::ui_width(state) > base_data.size.x)
				new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui::ui_width(state) - base_data.size.x));
			if(ui::ui_height(state) > base_data.size.y)
				new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui::ui_height(state) - base_data.size.y));
			if(state_is_rtl(state)) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_main_menu_graphics(sys::state& state);
struct main_menu_message_settings_t : public layout_window_element {
// BEGIN message_settings::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> title;
	std::unique_ptr<main_menu_message_settings_message_notify_rebels_defeat_t> message_notify_rebels_defeat;
	std::unique_ptr<main_menu_message_settings_reset_t> reset;
	main_menu_message_settings_message_settings_list_t message_settings_list;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view message_settings_table_setting_name_header_text_key;
	uint8_t message_settings_table_setting_name_header_text_color = 1;
	uint8_t message_settings_table_setting_name_column_text_color = 1;
	text::alignment message_settings_table_setting_name_text_alignment = text::alignment::left;
	int16_t message_settings_table_setting_name_column_start = 0;
	int16_t message_settings_table_setting_name_column_width = 0;
	std::string_view message_settings_table_self_header_text_key;
	uint8_t message_settings_table_self_header_text_color = 1;
	uint8_t message_settings_table_self_column_text_color = 1;
	text::alignment message_settings_table_self_text_alignment = text::alignment::center;
	int16_t message_settings_table_self_column_start = 0;
	int16_t message_settings_table_self_column_width = 0;
	std::string_view message_settings_table_interesting_header_text_key;
	uint8_t message_settings_table_interesting_header_text_color = 1;
	uint8_t message_settings_table_interesting_column_text_color = 1;
	text::alignment message_settings_table_interesting_text_alignment = text::alignment::center;
	int16_t message_settings_table_interesting_column_start = 0;
	int16_t message_settings_table_interesting_column_width = 0;
	std::string_view message_settings_table_other_header_text_key;
	uint8_t message_settings_table_other_header_text_color = 1;
	uint8_t message_settings_table_other_column_text_color = 1;
	text::alignment message_settings_table_other_text_alignment = text::alignment::center;
	int16_t message_settings_table_other_column_start = 0;
	int16_t message_settings_table_other_column_width = 0;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::consumed;
	}
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto location_abs = get_absolute_location(state, *this);
		if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
			ui::xy_pair new_abs_pos = location_abs;
			new_abs_pos.x += int16_t(x - oldx);
			new_abs_pos.y += int16_t(y - oldy);
			if(ui::ui_width(state) > base_data.size.x)
				new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui::ui_width(state) - base_data.size.x));
			if(ui::ui_height(state) > base_data.size.y)
				new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui::ui_height(state) - base_data.size.y));
			if(state_is_rtl(state)) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_main_menu_message_settings(sys::state& state);
struct main_menu_message_setting_row_t : public layout_window_element {
// BEGIN message_setting_row::variables
// END
	int32_t id;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<main_menu_message_setting_row_overlay_t> overlay;
	std::unique_ptr<main_menu_message_setting_row_setting_name_t> setting_name;
	std::unique_ptr<main_menu_message_setting_row_self_icon_t> self_icon;
	std::unique_ptr<main_menu_message_setting_row_interesting_icon_t> interesting_icon;
	std::unique_ptr<main_menu_message_setting_row_other_icon_t> other_icon;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view message_settings_table_setting_name_header_text_key;
	uint8_t message_settings_table_setting_name_header_text_color = 1;
	uint8_t message_settings_table_setting_name_column_text_color = 1;
	text::alignment message_settings_table_setting_name_text_alignment = text::alignment::left;
	int16_t message_settings_table_setting_name_column_start = 0;
	int16_t message_settings_table_setting_name_column_width = 0;
	std::string_view message_settings_table_self_header_text_key;
	uint8_t message_settings_table_self_header_text_color = 1;
	uint8_t message_settings_table_self_column_text_color = 1;
	text::alignment message_settings_table_self_text_alignment = text::alignment::center;
	int16_t message_settings_table_self_column_start = 0;
	int16_t message_settings_table_self_column_width = 0;
	std::string_view message_settings_table_interesting_header_text_key;
	uint8_t message_settings_table_interesting_header_text_color = 1;
	uint8_t message_settings_table_interesting_column_text_color = 1;
	text::alignment message_settings_table_interesting_text_alignment = text::alignment::center;
	int16_t message_settings_table_interesting_column_start = 0;
	int16_t message_settings_table_interesting_column_width = 0;
	std::string_view message_settings_table_other_header_text_key;
	uint8_t message_settings_table_other_header_text_color = 1;
	uint8_t message_settings_table_other_column_text_color = 1;
	text::alignment message_settings_table_other_text_alignment = text::alignment::center;
	int16_t message_settings_table_other_column_start = 0;
	int16_t message_settings_table_other_column_width = 0;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "id") {
			return (void*)(&id);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_main_menu_message_setting_row(sys::state& state);
struct main_menu_message_setting_header_t : public layout_window_element {
// BEGIN message_setting_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<main_menu_message_setting_header_contents_t> contents;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view message_settings_table_setting_name_header_text_key;
	uint8_t message_settings_table_setting_name_header_text_color = 1;
	uint8_t message_settings_table_setting_name_column_text_color = 1;
	text::alignment message_settings_table_setting_name_text_alignment = text::alignment::left;
	int16_t message_settings_table_setting_name_column_start = 0;
	int16_t message_settings_table_setting_name_column_width = 0;
	std::string_view message_settings_table_self_header_text_key;
	uint8_t message_settings_table_self_header_text_color = 1;
	uint8_t message_settings_table_self_column_text_color = 1;
	text::alignment message_settings_table_self_text_alignment = text::alignment::center;
	int16_t message_settings_table_self_column_start = 0;
	int16_t message_settings_table_self_column_width = 0;
	std::string_view message_settings_table_interesting_header_text_key;
	uint8_t message_settings_table_interesting_header_text_color = 1;
	uint8_t message_settings_table_interesting_column_text_color = 1;
	text::alignment message_settings_table_interesting_text_alignment = text::alignment::center;
	int16_t message_settings_table_interesting_column_start = 0;
	int16_t message_settings_table_interesting_column_width = 0;
	std::string_view message_settings_table_other_header_text_key;
	uint8_t message_settings_table_other_header_text_color = 1;
	uint8_t message_settings_table_other_column_text_color = 1;
	text::alignment message_settings_table_other_text_alignment = text::alignment::center;
	int16_t message_settings_table_other_column_start = 0;
	int16_t message_settings_table_other_column_width = 0;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_main_menu_message_setting_header(sys::state& state);
void main_menu_base_save_and_exit_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::save_and_exit::update
	disabled = !state.current_scene.game_in_progress;
// END
}
bool main_menu_base_save_and_exit_t::button_action(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::save_and_exit::lbutton_action
	if(state.user_setting_changed)
		state.save_user_settings();
	state.user_setting_changed = false;
	command::save_game(state, state.local_player_nation, true, simple_fs::utf16_to_utf8(base.save_name->get_text(state)));
// END
	return true;
}
void main_menu_base_save_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::save::update
	disabled = !state.current_scene.game_in_progress;
// END
}
bool main_menu_base_save_t::button_action(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::save::lbutton_action
	command::save_game(state, state.local_player_nation, false, simple_fs::utf16_to_utf8(base.save_name->get_text(state)));
// END
	return true;
}
void main_menu_base_exit_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::exit::update
// END
}
bool main_menu_base_exit_t::button_action(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::exit::lbutton_action
	if(state.user_setting_changed)
		state.save_user_settings();
	state.user_setting_changed = false;
	window::close_window(state);
// END
	return true;
}
void main_menu_base_graphics_options_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::graphics_options::update
// END
}
bool main_menu_base_graphics_options_t::button_action(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::graphics_options::lbutton_action
	alice_ui::display_at_front<alice_ui::make_main_menu_graphics>(state);
	parent->set_visible(state, false);
// END
	return true;
}
void main_menu_base_sound_options_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::sound_options::update
// END
}
bool main_menu_base_sound_options_t::button_action(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::sound_options::lbutton_action
	alice_ui::display_at_front<alice_ui::make_main_menu_sound>(state);
	parent->set_visible(state, false);
// END
	return true;
}
void main_menu_base_controls_options_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::controls_options::update
// END
}
bool main_menu_base_controls_options_t::button_action(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::controls_options::lbutton_action
	alice_ui::display_at_front<alice_ui::make_main_menu_misc>(state);
	parent->set_visible(state, false);
// END
	return true;
}
void main_menu_base_message_settings_options_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::message_settings_options::update
// END
}
bool main_menu_base_message_settings_options_t::button_action(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::message_settings_options::lbutton_action
	alice_ui::display_at_front<alice_ui::make_main_menu_message_settings>(state);
	parent->set_visible(state, false);
// END
	return true;
}
void main_menu_base_save_name_t::edit_box_update(sys::state& state, std::u16string_view s) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::save_name::edit_update
	base.save_warning_icon->impl_on_update(state);
// END
}
void main_menu_base_save_name_t::on_create(sys::state& state) noexcept {
// BEGIN base::save_name::create
// END
}
void main_menu_base_save_name_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::save_name::update
	disabled = !state.current_scene.game_in_progress;
// END
}
void main_menu_base_save_warning_icon_t::on_update(sys::state& state) noexcept {
	main_menu_base_t& base = *((main_menu_base_t*)(parent)); 
// BEGIN base::save_warning_icon::update
	if(base.save_name->get_text(state).empty()) {
		set_visible(state, false);
		return;
	}
	auto sdir = simple_fs::get_or_create_save_game_directory(state.mod_save_dir);
	auto as_native = simple_fs::utf16_to_native(base.save_name->get_text(state));
	as_native += NATIVE(".bin");
	auto exists = simple_fs::peek_file(sdir, as_native);
	set_visible(state, bool(exists));
// END
}
ui::message_result main_menu_base_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result main_menu_base_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_base_t::on_hide(sys::state& state) noexcept {
// BEGIN base::on_hide
	if(state.user_setting_changed)
		state.save_user_settings();
	state.user_setting_changed = false;
// END
}
void main_menu_base_t::on_update(sys::state& state) noexcept {
// BEGIN base::update
// END
	remake_layout(state, true);
}
void main_menu_base_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "bg_extension") {
					temp.ptr = bg_extension.get();
				} else
				if(cname == "logo") {
					temp.ptr = logo.get();
				} else
				if(cname == "save_and_exit") {
					temp.ptr = save_and_exit.get();
				} else
				if(cname == "save") {
					temp.ptr = save.get();
				} else
				if(cname == "exit") {
					temp.ptr = exit.get();
				} else
				if(cname == "graphics_options") {
					temp.ptr = graphics_options.get();
				} else
				if(cname == "sound_options") {
					temp.ptr = sound_options.get();
				} else
				if(cname == "controls_options") {
					temp.ptr = controls_options.get();
				} else
				if(cname == "message_settings_options") {
					temp.ptr = message_settings_options.get();
				} else
				if(cname == "end_icon") {
					temp.ptr = end_icon.get();
				} else
				if(cname == "save_name_label") {
					temp.ptr = save_name_label.get();
				} else
				if(cname == "save_name") {
					temp.ptr = save_name.get();
				} else
				if(cname == "save_warning_icon") {
					temp.ptr = save_warning_icon.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_base_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::base"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "bg_extension") {
			bg_extension = std::make_unique<template_bg_graphic>();
			bg_extension->parent = this;
			auto cptr = bg_extension.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "logo") {
			logo = std::make_unique<template_icon_graphic>();
			logo->parent = this;
			auto cptr = logo.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "save_and_exit") {
			save_and_exit = std::make_unique<main_menu_base_save_and_exit_t>();
			save_and_exit->parent = this;
			auto cptr = save_and_exit.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "save") {
			save = std::make_unique<main_menu_base_save_t>();
			save->parent = this;
			auto cptr = save.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "exit") {
			exit = std::make_unique<main_menu_base_exit_t>();
			exit->parent = this;
			auto cptr = exit.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "graphics_options") {
			graphics_options = std::make_unique<main_menu_base_graphics_options_t>();
			graphics_options->parent = this;
			auto cptr = graphics_options.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "sound_options") {
			sound_options = std::make_unique<main_menu_base_sound_options_t>();
			sound_options->parent = this;
			auto cptr = sound_options.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "controls_options") {
			controls_options = std::make_unique<main_menu_base_controls_options_t>();
			controls_options->parent = this;
			auto cptr = controls_options.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "message_settings_options") {
			message_settings_options = std::make_unique<main_menu_base_message_settings_options_t>();
			message_settings_options->parent = this;
			auto cptr = message_settings_options.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "end_icon") {
			end_icon = std::make_unique<template_icon_graphic>();
			end_icon->parent = this;
			auto cptr = end_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "save_name_label") {
			save_name_label = std::make_unique<template_label>();
			save_name_label->parent = this;
			auto cptr = save_name_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "save_name") {
			save_name = std::make_unique<main_menu_base_save_name_t>();
			save_name->parent = this;
			auto cptr = save_name.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "save_warning_icon") {
			save_warning_icon = std::make_unique<main_menu_base_save_warning_icon_t>();
			save_warning_icon->parent = this;
			auto cptr = save_warning_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN base::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_base(sys::state& state) {
	auto ptr = std::make_unique<main_menu_base_t>();
	ptr->on_create(state);
	return ptr;
}
void main_menu_sound_master_volume_minimum_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_minimum::update
// END
}
bool main_menu_sound_master_volume_minimum_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_minimum::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.master_volume = 0.0f;
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
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_master_volume_less_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_less::update
// END
}
bool main_menu_sound_master_volume_less_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_less::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.master_volume = std::clamp(state.user_settings.master_volume - 0.1f, 0.0f, 1.0f);
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
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_master_volume_less_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_less::lbutton_shift_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.master_volume = std::clamp(state.user_settings.master_volume - 0.01f, 0.0f, 1.0f);
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
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_master_voume_amount_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_voume_amount::update
	set_text(state, std::to_string(int32_t(state.user_settings.master_volume * 100)));
// END
}
void main_menu_sound_master_volume_more_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_more::update
// END
}
bool main_menu_sound_master_volume_more_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_more::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.master_volume = std::clamp(state.user_settings.master_volume + 0.1f, 0.0f, 1.0f);
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
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_master_volume_more_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_more::lbutton_shift_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.master_volume = std::clamp(state.user_settings.master_volume + 0.01f, 0.0f, 1.0f);
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
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_master_volume_maximum_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_maximum::update
// END
}
bool main_menu_sound_master_volume_maximum_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::master_volume_maximum::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.master_volume = 1.0f;
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
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_music_volume_minimum_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_minimum::update
// END
}
bool main_menu_sound_music_volume_minimum_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_minimum::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.music_volume = 0.0f;
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}

	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_music_volume_less_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_less::update
// END
}
bool main_menu_sound_music_volume_less_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_less::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.music_volume = std::clamp(state.user_settings.music_volume - 0.1f, 0.0f, 1.0f);
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}

	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_music_volume_less_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_less::lbutton_shift_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.music_volume = std::clamp(state.user_settings.music_volume - 0.01f, 0.0f, 1.0f);
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}

	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_music_volume_amount_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_amount::update
	set_text(state, std::to_string(int32_t(state.user_settings.music_volume * 100.0f)));
// END
}
void main_menu_sound_music_volume_more_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_more::update
// END
}
bool main_menu_sound_music_volume_more_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_more::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.music_volume = std::clamp(state.user_settings.music_volume + 0.1f, 0.0f, 1.0f);
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}

	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_music_volume_more_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_more::lbutton_shift_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.music_volume = std::clamp(state.user_settings.music_volume + 0.01f, 0.0f, 1.0f);
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}

	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_music_volume_max_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_max::update
// END
}
bool main_menu_sound_music_volume_max_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::music_volume_max::lbutton_action
	bool music_was_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;

	state.user_settings.music_volume = 1.0f;
	sound::change_music_volume(state, state.user_settings.music_volume * state.user_settings.master_volume);

	bool music_is_playing = state.user_settings.music_volume * state.user_settings.master_volume > 0;
	if(music_was_playing != music_is_playing) {
		if(music_is_playing)
			sound::start_music(state, state.user_settings.music_volume * state.user_settings.master_volume);
		else
			sound::stop_music(state);
	}

	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_interface_volume_min_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_min::update
// END
}
bool main_menu_sound_interface_volume_min_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_min::lbutton_action
	state.user_settings.interface_volume = 0.0f;
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_interface_volume_less_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_less::update
// END
}
bool main_menu_sound_interface_volume_less_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_less::lbutton_action
	state.user_settings.interface_volume = std::clamp(state.user_settings.interface_volume - 0.1f, 0.0f, 1.0f);
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_interface_volume_less_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_less::lbutton_shift_action
	state.user_settings.interface_volume = std::clamp(state.user_settings.interface_volume - 0.01f, 0.0f, 1.0f);
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_interface_volume_amount_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_amount::update
	set_text(state, std::to_string(int32_t(state.user_settings.interface_volume * 100.0f)));
// END
}
void main_menu_sound_interface_volume_more_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_more::update
// END
}
bool main_menu_sound_interface_volume_more_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_more::lbutton_action
	state.user_settings.interface_volume = std::clamp(state.user_settings.interface_volume + 0.1f, 0.0f, 1.0f);
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_interface_volume_more_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_more::lbutton_shift_action
	state.user_settings.interface_volume = std::clamp(state.user_settings.interface_volume + 0.01f, 0.0f, 1.0f);
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_interface_volume_max_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_max::update
// END
}
bool main_menu_sound_interface_volume_max_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::interface_volume_max::lbutton_action
	state.user_settings.interface_volume = 1.0f;
	sound::change_interface_volume(state, state.user_settings.interface_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_effect_volume_min_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_min::update
// END
}
bool main_menu_sound_effect_volume_min_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_min::lbutton_action
	state.user_settings.effects_volume = 1.0f;
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_effect_volume_less_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_less::update
// END
}
bool main_menu_sound_effect_volume_less_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_less::lbutton_action
	state.user_settings.effects_volume = std::clamp(state.user_settings.effects_volume - 0.1f, 0.0f, 1.0f);
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_effect_volume_less_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_less::lbutton_shift_action
	state.user_settings.effects_volume = std::clamp(state.user_settings.effects_volume - 0.01f, 0.0f, 1.0f);
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_effect_volume_amount_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_amount::update
	set_text(state, std::to_string(int32_t(state.user_settings.effects_volume * 100.0f)));
// END
}
void main_menu_sound_effect_volume_more_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_more::update
// END
}
bool main_menu_sound_effect_volume_more_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_more::lbutton_action
	state.user_settings.effects_volume = std::clamp(state.user_settings.effects_volume + 0.1f, 0.0f, 1.0f);
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
bool main_menu_sound_effect_volume_more_t::button_shift_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_more::lbutton_shift_action
	state.user_settings.effects_volume = std::clamp(state.user_settings.effects_volume + 0.01f, 0.0f, 1.0f);
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_effect_volume_max_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_max::update
// END
}
bool main_menu_sound_effect_volume_max_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::effect_volume_max::lbutton_action
	state.user_settings.effects_volume = 1.0f;
	sound::change_effect_volume(state, state.user_settings.effects_volume * state.user_settings.master_volume);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_sound_mute_on_lose_focus_t::on_update(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::mute_on_lose_focus::update
	set_active(state, state.user_settings.mute_on_focus_lost);
// END
}
bool main_menu_sound_mute_on_lose_focus_t::button_action(sys::state& state) noexcept {
	main_menu_sound_t& sound = *((main_menu_sound_t*)(parent)); 
// BEGIN sound::mute_on_lose_focus::lbutton_action
	state.user_settings.mute_on_focus_lost = !state.user_settings.mute_on_focus_lost;
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
ui::message_result main_menu_sound_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result main_menu_sound_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_sound_t::on_hide(sys::state& state) noexcept {
// BEGIN sound::on_hide
	alice_ui::display_at_front<alice_ui::make_main_menu_base>(state);
// END
}
void main_menu_sound_t::on_update(sys::state& state) noexcept {
// BEGIN sound::update
// END
	remake_layout(state, true);
}
void main_menu_sound_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "window_title") {
					temp.ptr = window_title.get();
				} else
				if(cname == "master_volume_minimum") {
					temp.ptr = master_volume_minimum.get();
				} else
				if(cname == "master_volume_less") {
					temp.ptr = master_volume_less.get();
				} else
				if(cname == "master_voume_amount") {
					temp.ptr = master_voume_amount.get();
				} else
				if(cname == "master_volume_more") {
					temp.ptr = master_volume_more.get();
				} else
				if(cname == "master_volume_maximum") {
					temp.ptr = master_volume_maximum.get();
				} else
				if(cname == "music_volume_label") {
					temp.ptr = music_volume_label.get();
				} else
				if(cname == "master_volume_label") {
					temp.ptr = master_volume_label.get();
				} else
				if(cname == "music_volume_minimum") {
					temp.ptr = music_volume_minimum.get();
				} else
				if(cname == "music_volume_less") {
					temp.ptr = music_volume_less.get();
				} else
				if(cname == "music_volume_amount") {
					temp.ptr = music_volume_amount.get();
				} else
				if(cname == "music_volume_more") {
					temp.ptr = music_volume_more.get();
				} else
				if(cname == "music_volume_max") {
					temp.ptr = music_volume_max.get();
				} else
				if(cname == "interface_volume_label") {
					temp.ptr = interface_volume_label.get();
				} else
				if(cname == "interface_volume_min") {
					temp.ptr = interface_volume_min.get();
				} else
				if(cname == "interface_volume_less") {
					temp.ptr = interface_volume_less.get();
				} else
				if(cname == "interface_volume_amount") {
					temp.ptr = interface_volume_amount.get();
				} else
				if(cname == "interface_volume_more") {
					temp.ptr = interface_volume_more.get();
				} else
				if(cname == "interface_volume_max") {
					temp.ptr = interface_volume_max.get();
				} else
				if(cname == "effect_volume_label") {
					temp.ptr = effect_volume_label.get();
				} else
				if(cname == "effect_volume_min") {
					temp.ptr = effect_volume_min.get();
				} else
				if(cname == "effect_volume_less") {
					temp.ptr = effect_volume_less.get();
				} else
				if(cname == "effect_volume_amount") {
					temp.ptr = effect_volume_amount.get();
				} else
				if(cname == "effect_volume_more") {
					temp.ptr = effect_volume_more.get();
				} else
				if(cname == "effect_volume_max") {
					temp.ptr = effect_volume_max.get();
				} else
				if(cname == "divider") {
					temp.ptr = divider.get();
				} else
				if(cname == "mute_on_lose_focus") {
					temp.ptr = mute_on_lose_focus.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_sound_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::sound"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "window_title") {
			window_title = std::make_unique<template_label>();
			window_title->parent = this;
			auto cptr = window_title.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "master_volume_minimum") {
			master_volume_minimum = std::make_unique<main_menu_sound_master_volume_minimum_t>();
			master_volume_minimum->parent = this;
			auto cptr = master_volume_minimum.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "master_volume_less") {
			master_volume_less = std::make_unique<main_menu_sound_master_volume_less_t>();
			master_volume_less->parent = this;
			auto cptr = master_volume_less.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "master_voume_amount") {
			master_voume_amount = std::make_unique<main_menu_sound_master_voume_amount_t>();
			master_voume_amount->parent = this;
			auto cptr = master_voume_amount.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "master_volume_more") {
			master_volume_more = std::make_unique<main_menu_sound_master_volume_more_t>();
			master_volume_more->parent = this;
			auto cptr = master_volume_more.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "master_volume_maximum") {
			master_volume_maximum = std::make_unique<main_menu_sound_master_volume_maximum_t>();
			master_volume_maximum->parent = this;
			auto cptr = master_volume_maximum.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "music_volume_label") {
			music_volume_label = std::make_unique<template_label>();
			music_volume_label->parent = this;
			auto cptr = music_volume_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "master_volume_label") {
			master_volume_label = std::make_unique<template_label>();
			master_volume_label->parent = this;
			auto cptr = master_volume_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "music_volume_minimum") {
			music_volume_minimum = std::make_unique<main_menu_sound_music_volume_minimum_t>();
			music_volume_minimum->parent = this;
			auto cptr = music_volume_minimum.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "music_volume_less") {
			music_volume_less = std::make_unique<main_menu_sound_music_volume_less_t>();
			music_volume_less->parent = this;
			auto cptr = music_volume_less.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "music_volume_amount") {
			music_volume_amount = std::make_unique<main_menu_sound_music_volume_amount_t>();
			music_volume_amount->parent = this;
			auto cptr = music_volume_amount.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "music_volume_more") {
			music_volume_more = std::make_unique<main_menu_sound_music_volume_more_t>();
			music_volume_more->parent = this;
			auto cptr = music_volume_more.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "music_volume_max") {
			music_volume_max = std::make_unique<main_menu_sound_music_volume_max_t>();
			music_volume_max->parent = this;
			auto cptr = music_volume_max.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "interface_volume_label") {
			interface_volume_label = std::make_unique<template_label>();
			interface_volume_label->parent = this;
			auto cptr = interface_volume_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "interface_volume_min") {
			interface_volume_min = std::make_unique<main_menu_sound_interface_volume_min_t>();
			interface_volume_min->parent = this;
			auto cptr = interface_volume_min.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "interface_volume_less") {
			interface_volume_less = std::make_unique<main_menu_sound_interface_volume_less_t>();
			interface_volume_less->parent = this;
			auto cptr = interface_volume_less.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "interface_volume_amount") {
			interface_volume_amount = std::make_unique<main_menu_sound_interface_volume_amount_t>();
			interface_volume_amount->parent = this;
			auto cptr = interface_volume_amount.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "interface_volume_more") {
			interface_volume_more = std::make_unique<main_menu_sound_interface_volume_more_t>();
			interface_volume_more->parent = this;
			auto cptr = interface_volume_more.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "interface_volume_max") {
			interface_volume_max = std::make_unique<main_menu_sound_interface_volume_max_t>();
			interface_volume_max->parent = this;
			auto cptr = interface_volume_max.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "effect_volume_label") {
			effect_volume_label = std::make_unique<template_label>();
			effect_volume_label->parent = this;
			auto cptr = effect_volume_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "effect_volume_min") {
			effect_volume_min = std::make_unique<main_menu_sound_effect_volume_min_t>();
			effect_volume_min->parent = this;
			auto cptr = effect_volume_min.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "effect_volume_less") {
			effect_volume_less = std::make_unique<main_menu_sound_effect_volume_less_t>();
			effect_volume_less->parent = this;
			auto cptr = effect_volume_less.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "effect_volume_amount") {
			effect_volume_amount = std::make_unique<main_menu_sound_effect_volume_amount_t>();
			effect_volume_amount->parent = this;
			auto cptr = effect_volume_amount.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "effect_volume_more") {
			effect_volume_more = std::make_unique<main_menu_sound_effect_volume_more_t>();
			effect_volume_more->parent = this;
			auto cptr = effect_volume_more.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "effect_volume_max") {
			effect_volume_max = std::make_unique<main_menu_sound_effect_volume_max_t>();
			effect_volume_max->parent = this;
			auto cptr = effect_volume_max.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "divider") {
			divider = std::make_unique<template_bg_graphic>();
			divider->parent = this;
			auto cptr = divider.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "mute_on_lose_focus") {
			mute_on_lose_focus = std::make_unique<main_menu_sound_mute_on_lose_focus_t>();
			mute_on_lose_focus->parent = this;
			auto cptr = mute_on_lose_focus.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN sound::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_sound(sys::state& state) {
	auto ptr = std::make_unique<main_menu_sound_t>();
	ptr->on_create(state);
	return ptr;
}
void main_menu_misc_auto_save_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_misc_auto_save_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_misc_auto_save_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_misc_auto_save_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::auto_save_dropdown::on_selection
	state.user_settings.autosaves = sys::autosave_frequency(list_contents[id].index);
	state.user_setting_changed = true;
// END
}
void main_menu_misc_auto_save_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_misc_auto_save_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::auto_save_dropdown::update
	clear_list();
	add_item(int32_t(sys::autosave_frequency::none), text::produce_simple_string(state, "auto_save_0"));
	add_item(int32_t(sys::autosave_frequency::daily), text::produce_simple_string(state, "auto_save_1"));
	add_item(int32_t(sys::autosave_frequency::monthly), text::produce_simple_string(state, "auto_save_2"));
	add_item(int32_t(sys::autosave_frequency::yearly), text::produce_simple_string(state, "auto_save_3"));
	quiet_on_selection(state, int32_t(state.user_settings.autosaves));
// END
}
void main_menu_misc_auto_save_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN misc::auto_save_dropdown::create
// END
}
void main_menu_misc_language_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_misc_language_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_misc_language_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_misc_language_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::language_dropdown::on_selection
	window::change_cursor(state, window::cursor_type::busy);

	dcon::locale_id new_locale = dcon::locale_id{ dcon::locale_id::value_base_t(list_contents[id].index) };

	if(state.user_settings.use_classic_fonts
	&& state.world.locale_get_hb_script(new_locale) != HB_SCRIPT_LATIN) {
		state.user_settings.use_classic_fonts = false;
	}

	auto length = std::min(state.world.locale_get_locale_name(new_locale).size(), uint32_t(15));
	std::memcpy(state.user_settings.locale, state.world.locale_get_locale_name(new_locale).begin(), length);
	state.user_settings.locale[length] = 0;
	state.font_collection.change_locale(state, new_locale);


	state.ui_state.for_each_root([&](ui::element_base& elm) {
		elm.impl_on_reset_text(state);
	});

	state.province_ownership_changed.store(true, std::memory_order::release); //update map
	state.game_state_updated.store(true, std::memory_order::release); //update ui

	state.user_setting_changed = true;
	window::change_cursor(state, window::cursor_type::normal);
// END
}
void main_menu_misc_language_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_misc_language_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::language_dropdown::update
	clear_list();
	int32_t i = 0;
	for(auto loc : state.world.in_locale) {
		auto ln = state.world.locale_get_locale_name(loc);
		auto ln_sv = std::string_view{ (char const*)ln.begin(), ln.size() };
		add_item(loc.id.index(), std::string(ln_sv));
		if(state.font_collection.get_current_locale() == loc)
			quiet_on_selection(state, i);
		++i;
	}
// END
}
void main_menu_misc_language_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN misc::language_dropdown::create
// END
}
void main_menu_misc_zoom_mode_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_misc_zoom_mode_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_misc_zoom_mode_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_misc_zoom_mode_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::zoom_mode_dropdown::on_selection
	state.user_settings.zoom_mode = sys::map_zoom_mode(list_contents[id].index);
	state.user_setting_changed = true;
// END
}
void main_menu_misc_zoom_mode_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_misc_zoom_mode_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::zoom_mode_dropdown::update
	clear_list();
	add_item(int32_t(sys::map_zoom_mode::panning), text::produce_simple_string(state, "zoom_mode_panning"));
	add_item(int32_t(sys::map_zoom_mode::inverted), text::produce_simple_string(state, "zoom_mode_inverted"));
	add_item(int32_t(sys::map_zoom_mode::centered), text::produce_simple_string(state, "zoom_mode_centered"));
	add_item(int32_t(sys::map_zoom_mode::to_cursor), text::produce_simple_string(state, "zoom_mode_to_cursor"));
	add_item(int32_t(sys::map_zoom_mode::away_from_cursor), text::produce_simple_string(state, "zoom_mode_away_from_cursor"));
	quiet_on_selection(state, int32_t(state.user_settings.zoom_mode));
// END
}
void main_menu_misc_zoom_mode_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN misc::zoom_mode_dropdown::create
// END
}
void main_menu_misc_zoom_speed_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_misc_zoom_speed_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_misc_zoom_speed_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_misc_zoom_speed_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::zoom_speed_dropdown::on_selection
	if(list_contents[id].index == 0)
		state.user_settings.zoom_speed = 15.0f;
	else if(list_contents[id].index == 1)
		state.user_settings.zoom_speed = 17.5;
	else if(list_contents[id].index == 2)
		state.user_settings.zoom_speed = 20.0f;
	else if(list_contents[id].index == 3)
		state.user_settings.zoom_speed = 22.5f;
	else if(list_contents[id].index == 4)
		state.user_settings.zoom_speed = 25.0f;
	state.user_setting_changed = true;
// END
}
void main_menu_misc_zoom_speed_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_misc_zoom_speed_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::zoom_speed_dropdown::update
	if(state.user_settings.zoom_speed <= 15.0f)
		quiet_on_selection(state, 0);
	else if(state.user_settings.zoom_speed <= 17.5f)
		quiet_on_selection(state, 1);
	else if(state.user_settings.zoom_speed <= 20.0f)
		quiet_on_selection(state, 2);
	else if(state.user_settings.zoom_speed <= 22.5f)
		quiet_on_selection(state, 3);
	else
		quiet_on_selection(state, 4);
// END
}
void main_menu_misc_zoom_speed_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN misc::zoom_speed_dropdown::create
	add_item(0, text::produce_simple_string(state, "zoom_slowest"));
	add_item(1, text::produce_simple_string(state, "zoom_slow"));
	add_item(2, text::produce_simple_string(state, "zoom_medium"));
	add_item(3, text::produce_simple_string(state, "zoom_fast"));
	add_item(4, text::produce_simple_string(state, "zoom_fastest"));
// END
}
void main_menu_misc_floating_tooltip_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::floating_tooltip::update
	set_active(state, state.user_settings.bind_tooltip_mouse);
// END
}
bool main_menu_misc_floating_tooltip_t::button_action(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::floating_tooltip::lbutton_action
	state.user_settings.bind_tooltip_mouse = !state.user_settings.bind_tooltip_mouse;
	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_misc_wasd_movement_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::wasd_movement::update
	set_active(state, state.user_settings.wasd_for_map_movement);
// END
}
bool main_menu_misc_wasd_movement_t::button_action(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::wasd_movement::lbutton_action
	state.user_settings.wasd_for_map_movement = !state.user_settings.wasd_for_map_movement;
	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_misc_edge_scrolling_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::edge_scrolling::update
	set_active(state, state.user_settings.mouse_edge_scrolling);
// END
}
bool main_menu_misc_edge_scrolling_t::button_action(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::edge_scrolling::lbutton_action
	state.user_settings.mouse_edge_scrolling = !state.user_settings.mouse_edge_scrolling;
	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_misc_confirm_disband_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::confirm_disband::update
	set_active(state, state.user_settings.unit_disband_confirmation);
// END
}
bool main_menu_misc_confirm_disband_t::button_action(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::confirm_disband::lbutton_action
	state.user_settings.unit_disband_confirmation = !state.user_settings.unit_disband_confirmation;
	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_misc_messages_as_popups_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::messages_as_popups::update
	set_active(state, state.user_settings.diplomatic_message_popup);
// END
}
bool main_menu_misc_messages_as_popups_t::button_action(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::messages_as_popups::lbutton_action
	state.user_settings.diplomatic_message_popup = !state.user_settings.diplomatic_message_popup;
	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_misc_detailed_tooltips_t::on_update(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::detailed_tooltips::update
	set_active(state, state.user_settings.spoilers);
// END
}
bool main_menu_misc_detailed_tooltips_t::button_action(sys::state& state) noexcept {
	main_menu_misc_t& misc = *((main_menu_misc_t*)(parent)); 
// BEGIN misc::detailed_tooltips::lbutton_action
	state.user_settings.spoilers = !state.user_settings.spoilers;
	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
ui::message_result main_menu_misc_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result main_menu_misc_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_misc_t::on_hide(sys::state& state) noexcept {
// BEGIN misc::on_hide
	alice_ui::display_at_front<alice_ui::make_main_menu_base>(state);
// END
}
void main_menu_misc_t::on_update(sys::state& state) noexcept {
// BEGIN misc::update
// END
	remake_layout(state, true);
}
void main_menu_misc_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "window_title") {
					temp.ptr = window_title.get();
				} else
				if(cname == "auto_save_label") {
					temp.ptr = auto_save_label.get();
				} else
				if(cname == "auto_save_dropdown") {
					temp.ptr = auto_save_dropdown.get();
				} else
				if(cname == "language_label") {
					temp.ptr = language_label.get();
				} else
				if(cname == "language_dropdown") {
					temp.ptr = language_dropdown.get();
				} else
				if(cname == "zoom_mode_label") {
					temp.ptr = zoom_mode_label.get();
				} else
				if(cname == "zoom_mode_dropdown") {
					temp.ptr = zoom_mode_dropdown.get();
				} else
				if(cname == "zoom_speed_label") {
					temp.ptr = zoom_speed_label.get();
				} else
				if(cname == "zoom_speed_dropdown") {
					temp.ptr = zoom_speed_dropdown.get();
				} else
				if(cname == "floating_tooltip") {
					temp.ptr = floating_tooltip.get();
				} else
				if(cname == "wasd_movement") {
					temp.ptr = wasd_movement.get();
				} else
				if(cname == "edge_scrolling") {
					temp.ptr = edge_scrolling.get();
				} else
				if(cname == "confirm_disband") {
					temp.ptr = confirm_disband.get();
				} else
				if(cname == "messages_as_popups") {
					temp.ptr = messages_as_popups.get();
				} else
				if(cname == "detailed_tooltips") {
					temp.ptr = detailed_tooltips.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_misc_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::misc"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "window_title") {
			window_title = std::make_unique<template_label>();
			window_title->parent = this;
			auto cptr = window_title.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "auto_save_label") {
			auto_save_label = std::make_unique<template_label>();
			auto_save_label->parent = this;
			auto cptr = auto_save_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "auto_save_dropdown") {
			auto_save_dropdown = std::make_unique<main_menu_misc_auto_save_dropdown_t>();
			auto_save_dropdown->parent = this;
			auto cptr = auto_save_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "language_label") {
			language_label = std::make_unique<template_label>();
			language_label->parent = this;
			auto cptr = language_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "language_dropdown") {
			language_dropdown = std::make_unique<main_menu_misc_language_dropdown_t>();
			language_dropdown->parent = this;
			auto cptr = language_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "zoom_mode_label") {
			zoom_mode_label = std::make_unique<template_label>();
			zoom_mode_label->parent = this;
			auto cptr = zoom_mode_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "zoom_mode_dropdown") {
			zoom_mode_dropdown = std::make_unique<main_menu_misc_zoom_mode_dropdown_t>();
			zoom_mode_dropdown->parent = this;
			auto cptr = zoom_mode_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "zoom_speed_label") {
			zoom_speed_label = std::make_unique<template_label>();
			zoom_speed_label->parent = this;
			auto cptr = zoom_speed_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "zoom_speed_dropdown") {
			zoom_speed_dropdown = std::make_unique<main_menu_misc_zoom_speed_dropdown_t>();
			zoom_speed_dropdown->parent = this;
			auto cptr = zoom_speed_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "floating_tooltip") {
			floating_tooltip = std::make_unique<main_menu_misc_floating_tooltip_t>();
			floating_tooltip->parent = this;
			auto cptr = floating_tooltip.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "wasd_movement") {
			wasd_movement = std::make_unique<main_menu_misc_wasd_movement_t>();
			wasd_movement->parent = this;
			auto cptr = wasd_movement.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "edge_scrolling") {
			edge_scrolling = std::make_unique<main_menu_misc_edge_scrolling_t>();
			edge_scrolling->parent = this;
			auto cptr = edge_scrolling.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "confirm_disband") {
			confirm_disband = std::make_unique<main_menu_misc_confirm_disband_t>();
			confirm_disband->parent = this;
			auto cptr = confirm_disband.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "messages_as_popups") {
			messages_as_popups = std::make_unique<main_menu_misc_messages_as_popups_t>();
			messages_as_popups->parent = this;
			auto cptr = messages_as_popups.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "detailed_tooltips") {
			detailed_tooltips = std::make_unique<main_menu_misc_detailed_tooltips_t>();
			detailed_tooltips->parent = this;
			auto cptr = detailed_tooltips.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN misc::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_misc(sys::state& state) {
	auto ptr = std::make_unique<main_menu_misc_t>();
	ptr->on_create(state);
	return ptr;
}
void main_menu_string_dropdown_choice_contents_t::on_update(sys::state& state) noexcept {
	main_menu_string_dropdown_choice_t& string_dropdown_choice = *((main_menu_string_dropdown_choice_t*)(parent)); 
// BEGIN string_dropdown_choice::contents::update
	set_text(state, string_dropdown_choice.display_value);
// END
}
ui::message_result main_menu_string_dropdown_choice_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result main_menu_string_dropdown_choice_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_string_dropdown_choice_t::on_update(sys::state& state) noexcept {
// BEGIN string_dropdown_choice::update
// END
	remake_layout(state, true);
}
void main_menu_string_dropdown_choice_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "contents") {
					temp.ptr = contents.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_string_dropdown_choice_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::string_dropdown_choice"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "contents") {
			contents = std::make_unique<main_menu_string_dropdown_choice_contents_t>();
			contents->parent = this;
			auto cptr = contents.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN string_dropdown_choice::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_string_dropdown_choice(sys::state& state) {
	auto ptr = std::make_unique<main_menu_string_dropdown_choice_t>();
	ptr->on_create(state);
	return ptr;
}
void main_menu_graphics_projection_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_graphics_projection_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_graphics_projection_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_graphics_projection_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::projection_dropdown::on_selection
	state.user_settings.map_is_globe = sys::projection_mode(list_contents[id].index);
	state.user_setting_changed = true;
// END
}
void main_menu_graphics_projection_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_graphics_projection_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::projection_dropdown::update
	clear_list();
	add_item(int32_t(sys::projection_mode::globe_ortho), text::produce_simple_string(state, "map_projection_globe"));
	add_item(int32_t(sys::projection_mode::flat), text::produce_simple_string(state, "map_projection_flat"));
	add_item(int32_t(sys::projection_mode::globe_perpect), text::produce_simple_string(state, "map_projection_globe_perspective"));
	quiet_on_selection(state, int32_t(state.user_settings.map_is_globe));
// END
}
void main_menu_graphics_projection_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN graphics::projection_dropdown::create
// END
}
void main_menu_graphics_ui_scale_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_graphics_ui_scale_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_graphics_ui_scale_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_graphics_ui_scale_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::ui_scale_dropdown::on_selection
	state.update_ui_scale(sys::ui_scales[list_contents[id].index]);
	state.user_setting_changed = true;
// END
}
void main_menu_graphics_ui_scale_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_graphics_ui_scale_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::ui_scale_dropdown::update
	for(size_t i = 0; i < sys::ui_scales_count; ++i) {
		if(state.user_settings.ui_scale >= sys::ui_scales[i] && (i + 1 == sys::ui_scales_count || state.user_settings.ui_scale < sys::ui_scales[i + 1])) {
			quiet_on_selection(state, int32_t(i));
		}
	}
// END
}
void main_menu_graphics_ui_scale_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN graphics::ui_scale_dropdown::create
	for(size_t i = 0; i < sys::ui_scales_count; ++i) {
		add_item(int32_t(i), text::format_float(sys::ui_scales[i], 2));
	}
// END
}
void main_menu_graphics_map_label_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_graphics_map_label_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_graphics_map_label_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_graphics_map_label_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::map_label_dropdown::on_selection
	state.user_settings.map_label = sys::map_label_mode(list_contents[id].index);
	state.user_setting_changed = true;
	state.province_ownership_changed.store(true, std::memory_order::release);
// END
}
void main_menu_graphics_map_label_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_graphics_map_label_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::map_label_dropdown::update
	clear_list();
	add_item(int32_t(sys::map_label_mode::none), text::produce_simple_string(state, "map_label_0"));
	add_item(int32_t(sys::map_label_mode::linear), text::produce_simple_string(state, "map_label_1"));
	add_item(int32_t(sys::map_label_mode::quadratic), text::produce_simple_string(state, "map_label_2"));
	add_item(int32_t(sys::map_label_mode::cubic), text::produce_simple_string(state, "map_label_3"));
	quiet_on_selection(state, int32_t(state.user_settings.map_label));
// END
}
void main_menu_graphics_map_label_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN graphics::map_label_dropdown::create
// END
}
void main_menu_graphics_antialiasing_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_graphics_antialiasing_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_graphics_antialiasing_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_graphics_antialiasing_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::antialiasing_dropdown::on_selection
	state.user_settings.antialias_level = uint8_t(sys::ui_scales[list_contents[id].index]);
	ogl::deinitialize_msaa(state);
	ogl::initialize_msaa(state, state.x_size, state.y_size);
	state.user_setting_changed = true;
// END
}
void main_menu_graphics_antialiasing_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_graphics_antialiasing_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::antialiasing_dropdown::update
	quiet_on_selection(state, int32_t(state.user_settings.antialias_level));
// END
}
void main_menu_graphics_antialiasing_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN graphics::antialiasing_dropdown::create
	for(size_t i = 0; i < 17; ++i) {
		add_item(int32_t(i), std::to_string(i));
	}
// END
}
void main_menu_graphics_gamma_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_graphics_gamma_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_graphics_gamma_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_graphics_gamma_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::gamma_dropdown::on_selection
	state.user_settings.gamma = float(list_contents[id].index) * 0.1f + 0.5f;
	state.user_setting_changed = true;
// END
}
void main_menu_graphics_gamma_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_graphics_gamma_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::gamma_dropdown::update
	quiet_on_selection(state, int32_t(std::round((state.user_settings.gamma - 0.5f) * 10.0f)));
// END
}
void main_menu_graphics_gamma_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN graphics::gamma_dropdown::create
	for(size_t i = 0; i <= 20; ++i) {
		add_item(int32_t(i), text::format_float(0.5f + i * 0.1f, 1));
	}
// END
}
void main_menu_graphics_vassal_color_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_graphics_vassal_color_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_graphics_vassal_color_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_graphics_vassal_color_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::vassal_color_dropdown::on_selection
	state.user_settings.vassal_color = sys::map_vassal_color_mode(list_contents[id].index);
	state.user_setting_changed = true;
// END
}
void main_menu_graphics_vassal_color_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_graphics_vassal_color_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::vassal_color_dropdown::update
	clear_list();
	add_item(int32_t(sys::map_vassal_color_mode::inherit), text::produce_simple_string(state, "vassal_color_inherit"));
	add_item(int32_t(sys::map_vassal_color_mode::same), text::produce_simple_string(state, "vassal_color_same"));
	add_item(int32_t(sys::map_vassal_color_mode::none), text::produce_simple_string(state, "vassal_color_none"));
	quiet_on_selection(state, int32_t(state.user_settings.vassal_color));
// END
}
void main_menu_graphics_vassal_color_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN graphics::vassal_color_dropdown::create
// END
}
void main_menu_graphics_graphics_details_dropdown_t::add_item( int32_t index,  std::string display_value) {
	list_contents.emplace_back(string_dropdown_choice_option{index, display_value});
	++total_items;
}
ui::element_base* main_menu_graphics_graphics_details_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
	}
	list_pool[pool_id]->index = list_contents[id].index; 
	list_pool[pool_id]->display_value = list_contents[id].display_value; 
	return list_pool[pool_id].get();
}
void main_menu_graphics_graphics_details_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->index = list_contents[id].index; 
	label_window_internal->display_value = list_contents[id].display_value; 
	label_window_internal->impl_on_update(state); 
}
void main_menu_graphics_graphics_details_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::graphics_details_dropdown::on_selection
	state.user_settings.graphics_mode = sys::graphics_mode(list_contents[id].index);
	state.user_setting_changed = true;
// END
}
void main_menu_graphics_graphics_details_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void main_menu_graphics_graphics_details_dropdown_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::graphics_details_dropdown::update
	clear_list();
	add_item(int32_t(sys::graphics_mode::ugly), text::produce_simple_string(state, "graphics_details_0"));
	add_item(int32_t(sys::graphics_mode::classic), text::produce_simple_string(state, "graphics_details_1"));
	add_item(int32_t(sys::graphics_mode::modern), text::produce_simple_string(state, "graphics_details_2"));
	quiet_on_selection(state, int32_t(state.user_settings.graphics_mode));
// END
}
void main_menu_graphics_graphics_details_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN graphics::graphics_details_dropdown::create
// END
}
void main_menu_graphics_window_mode_label_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::window_mode_label::update
	set_active(state, state.user_settings.prefer_fullscreen);
// END
}
bool main_menu_graphics_window_mode_label_t::button_action(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::window_mode_label::lbutton_action
	state.user_settings.prefer_fullscreen = !state.user_settings.prefer_fullscreen;
	window::set_borderless_full_screen(state, state.user_settings.prefer_fullscreen);
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_graphics_mouse_left_click_mode_label_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::mouse_left_click_mode_label::update
	set_active(state, state.user_settings.left_mouse_click_hold_and_release);
// END
}
bool main_menu_graphics_mouse_left_click_mode_label_t::button_action(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::mouse_left_click_mode_label::lbutton_action
	state.user_settings.left_mouse_click_hold_and_release = !state.user_settings.left_mouse_click_hold_and_release;
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_graphics_black_map_font_label_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::black_map_font_label::update
	set_active(state, state.user_settings.black_map_font);
// END
}
bool main_menu_graphics_black_map_font_label_t::button_action(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::black_map_font_label::lbutton_action
	state.user_settings.black_map_font = !state.user_settings.black_map_font;
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_graphics_river_label_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::river_label::update
	set_active(state, state.user_settings.rivers_enabled);
// END
}
bool main_menu_graphics_river_label_t::button_action(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::river_label::lbutton_action
	state.user_settings.rivers_enabled = !state.user_settings.rivers_enabled;
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_graphics_railroad_label_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::railroad_label::update
	set_active(state, state.user_settings.railroads_enabled);
// END
}
bool main_menu_graphics_railroad_label_t::button_action(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::railroad_label::lbutton_action
	state.user_settings.railroads_enabled = !state.user_settings.railroads_enabled;
	parent->impl_on_update(state);
	state.user_setting_changed = true;
// END
	return true;
}
void main_menu_graphics_fonts_label_t::on_update(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::fonts_label::update
	set_active(state, state.user_settings.use_classic_fonts);
// END
}
bool main_menu_graphics_fonts_label_t::button_action(sys::state& state) noexcept {
	main_menu_graphics_t& graphics = *((main_menu_graphics_t*)(parent)); 
// BEGIN graphics::fonts_label::lbutton_action
	state.user_settings.use_classic_fonts = !state.user_settings.use_classic_fonts;

	window::change_cursor(state, window::cursor_type::busy);

	state.ui_state.for_each_root([&](ui::element_base& elm) {
		elm.impl_on_reset_text(state);
	});

	state.province_ownership_changed.store(true, std::memory_order::release); //update map
	state.game_state_updated.store(true, std::memory_order::release); //update ui
	state.ui_state.tooltip->set_visible(state, false);
	state.ui_state.last_tooltip = nullptr;
	window::change_cursor(state, window::cursor_type::normal);

	state.user_setting_changed = true;
// END
	return true;
}
ui::message_result main_menu_graphics_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result main_menu_graphics_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_graphics_t::on_hide(sys::state& state) noexcept {
// BEGIN graphics::on_hide
	alice_ui::display_at_front<alice_ui::make_main_menu_base>(state);
// END
}
void main_menu_graphics_t::on_update(sys::state& state) noexcept {
// BEGIN graphics::update
// END
	remake_layout(state, true);
}
void main_menu_graphics_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "title") {
					temp.ptr = title.get();
				} else
				if(cname == "projection_label") {
					temp.ptr = projection_label.get();
				} else
				if(cname == "projection_dropdown") {
					temp.ptr = projection_dropdown.get();
				} else
				if(cname == "ui_scale_label") {
					temp.ptr = ui_scale_label.get();
				} else
				if(cname == "ui_scale_dropdown") {
					temp.ptr = ui_scale_dropdown.get();
				} else
				if(cname == "map_label_label") {
					temp.ptr = map_label_label.get();
				} else
				if(cname == "map_label_dropdown") {
					temp.ptr = map_label_dropdown.get();
				} else
				if(cname == "antialiasing_label") {
					temp.ptr = antialiasing_label.get();
				} else
				if(cname == "antialiasing_dropdown") {
					temp.ptr = antialiasing_dropdown.get();
				} else
				if(cname == "gamma_label") {
					temp.ptr = gamma_label.get();
				} else
				if(cname == "gamma_dropdown") {
					temp.ptr = gamma_dropdown.get();
				} else
				if(cname == "vassal_color_label") {
					temp.ptr = vassal_color_label.get();
				} else
				if(cname == "vassal_color_dropdown") {
					temp.ptr = vassal_color_dropdown.get();
				} else
				if(cname == "graphics_details_label") {
					temp.ptr = graphics_details_label.get();
				} else
				if(cname == "graphics_details_dropdown") {
					temp.ptr = graphics_details_dropdown.get();
				} else
				if(cname == "window_mode_label") {
					temp.ptr = window_mode_label.get();
				} else
				if(cname == "mouse_left_click_mode_label") {
					temp.ptr = mouse_left_click_mode_label.get();
				} else
				if(cname == "black_map_font_label") {
					temp.ptr = black_map_font_label.get();
				} else
				if(cname == "river_label") {
					temp.ptr = river_label.get();
				} else
				if(cname == "railroad_label") {
					temp.ptr = railroad_label.get();
				} else
				if(cname == "fonts_label") {
					temp.ptr = fonts_label.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_graphics_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::graphics"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "title") {
			title = std::make_unique<template_label>();
			title->parent = this;
			auto cptr = title.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "projection_label") {
			projection_label = std::make_unique<template_label>();
			projection_label->parent = this;
			auto cptr = projection_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "projection_dropdown") {
			projection_dropdown = std::make_unique<main_menu_graphics_projection_dropdown_t>();
			projection_dropdown->parent = this;
			auto cptr = projection_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "ui_scale_label") {
			ui_scale_label = std::make_unique<template_label>();
			ui_scale_label->parent = this;
			auto cptr = ui_scale_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "ui_scale_dropdown") {
			ui_scale_dropdown = std::make_unique<main_menu_graphics_ui_scale_dropdown_t>();
			ui_scale_dropdown->parent = this;
			auto cptr = ui_scale_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "map_label_label") {
			map_label_label = std::make_unique<template_label>();
			map_label_label->parent = this;
			auto cptr = map_label_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "map_label_dropdown") {
			map_label_dropdown = std::make_unique<main_menu_graphics_map_label_dropdown_t>();
			map_label_dropdown->parent = this;
			auto cptr = map_label_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "antialiasing_label") {
			antialiasing_label = std::make_unique<template_label>();
			antialiasing_label->parent = this;
			auto cptr = antialiasing_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "antialiasing_dropdown") {
			antialiasing_dropdown = std::make_unique<main_menu_graphics_antialiasing_dropdown_t>();
			antialiasing_dropdown->parent = this;
			auto cptr = antialiasing_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "gamma_label") {
			gamma_label = std::make_unique<template_label>();
			gamma_label->parent = this;
			auto cptr = gamma_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "gamma_dropdown") {
			gamma_dropdown = std::make_unique<main_menu_graphics_gamma_dropdown_t>();
			gamma_dropdown->parent = this;
			auto cptr = gamma_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "vassal_color_label") {
			vassal_color_label = std::make_unique<template_label>();
			vassal_color_label->parent = this;
			auto cptr = vassal_color_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "vassal_color_dropdown") {
			vassal_color_dropdown = std::make_unique<main_menu_graphics_vassal_color_dropdown_t>();
			vassal_color_dropdown->parent = this;
			auto cptr = vassal_color_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "graphics_details_label") {
			graphics_details_label = std::make_unique<template_label>();
			graphics_details_label->parent = this;
			auto cptr = graphics_details_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "graphics_details_dropdown") {
			graphics_details_dropdown = std::make_unique<main_menu_graphics_graphics_details_dropdown_t>();
			graphics_details_dropdown->parent = this;
			auto cptr = graphics_details_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<main_menu_string_dropdown_choice_t>(static_cast<main_menu_string_dropdown_choice_t*>(make_main_menu_string_dropdown_choice(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "window_mode_label") {
			window_mode_label = std::make_unique<main_menu_graphics_window_mode_label_t>();
			window_mode_label->parent = this;
			auto cptr = window_mode_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "mouse_left_click_mode_label") {
			mouse_left_click_mode_label = std::make_unique<main_menu_graphics_mouse_left_click_mode_label_t>();
			mouse_left_click_mode_label->parent = this;
			auto cptr = mouse_left_click_mode_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "black_map_font_label") {
			black_map_font_label = std::make_unique<main_menu_graphics_black_map_font_label_t>();
			black_map_font_label->parent = this;
			auto cptr = black_map_font_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "river_label") {
			river_label = std::make_unique<main_menu_graphics_river_label_t>();
			river_label->parent = this;
			auto cptr = river_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "railroad_label") {
			railroad_label = std::make_unique<main_menu_graphics_railroad_label_t>();
			railroad_label->parent = this;
			auto cptr = railroad_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "fonts_label") {
			fonts_label = std::make_unique<main_menu_graphics_fonts_label_t>();
			fonts_label->parent = this;
			auto cptr = fonts_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN graphics::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_graphics(sys::state& state) {
	auto ptr = std::make_unique<main_menu_graphics_t>();
	ptr->on_create(state);
	return ptr;
}
void main_menu_message_settings_message_settings_list_t::add_message_setting_row(int32_t id) {
	values.emplace_back(message_setting_row_option{id});
}
void  main_menu_message_settings_message_settings_list_t::on_create(sys::state& state, layout_window_element* parent) {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent)); 
// BEGIN message_settings::message_settings_list::on_create
	for(uint32_t i = 0; i <= 104; ++i)
		add_message_setting_row(i);
// END
}
void  main_menu_message_settings_message_settings_list_t::update(sys::state& state, layout_window_element* parent) {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent)); 
// BEGIN message_settings::message_settings_list::update
// END
}
measure_result  main_menu_message_settings_message_settings_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<message_setting_row_option>(values[index])) {
		if(message_setting_header_pool.empty()) message_setting_header_pool.emplace_back(make_main_menu_message_setting_header(state));
		if(message_setting_row_pool.empty()) message_setting_row_pool.emplace_back(make_main_menu_message_setting_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<message_setting_row_option>(values[index - 1]))) {
			if(destination) {
				if(message_setting_header_pool.size() <= size_t(message_setting_header_pool_used)) message_setting_header_pool.emplace_back(make_main_menu_message_setting_header(state));
				if(message_setting_row_pool.size() <= size_t(message_setting_row_pool_used)) message_setting_row_pool.emplace_back(make_main_menu_message_setting_row(state));
				message_setting_header_pool[message_setting_header_pool_used]->base_data.position.x = int16_t(x);
				message_setting_header_pool[message_setting_header_pool_used]->base_data.position.y = int16_t(y);
				if(!message_setting_header_pool[message_setting_header_pool_used]->parent) {
					message_setting_header_pool[message_setting_header_pool_used]->parent = destination;
					message_setting_header_pool[message_setting_header_pool_used]->impl_on_update(state);
					message_setting_header_pool[message_setting_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(message_setting_header_pool[message_setting_header_pool_used].get());
			((main_menu_message_setting_header_t*)(message_setting_header_pool[message_setting_header_pool_used].get()))->set_alternate(alternate);
				message_setting_row_pool[message_setting_row_pool_used]->base_data.position.x = int16_t(x);
				message_setting_row_pool[message_setting_row_pool_used]->base_data.position.y = int16_t(y +  message_setting_row_pool[0]->base_data.size.y + 0);
				message_setting_row_pool[message_setting_row_pool_used]->parent = destination;
				destination->children.push_back(message_setting_row_pool[message_setting_row_pool_used].get());
				((main_menu_message_setting_row_t*)(message_setting_row_pool[message_setting_row_pool_used].get()))->id = std::get<message_setting_row_option>(values[index]).id;
			((main_menu_message_setting_row_t*)(message_setting_row_pool[message_setting_row_pool_used].get()))->set_alternate(!alternate);
				message_setting_row_pool[message_setting_row_pool_used]->impl_on_update(state);
				message_setting_header_pool_used++;
				message_setting_row_pool_used++;
			}
			return measure_result{std::max(message_setting_header_pool[0]->base_data.size.x, message_setting_row_pool[0]->base_data.size.x), message_setting_header_pool[0]->base_data.size.y + message_setting_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(message_setting_row_pool.size() <= size_t(message_setting_row_pool_used)) message_setting_row_pool.emplace_back(make_main_menu_message_setting_row(state));
			message_setting_row_pool[message_setting_row_pool_used]->base_data.position.x = int16_t(x);
			message_setting_row_pool[message_setting_row_pool_used]->base_data.position.y = int16_t(y);
			message_setting_row_pool[message_setting_row_pool_used]->parent = destination;
			destination->children.push_back(message_setting_row_pool[message_setting_row_pool_used].get());
			((main_menu_message_setting_row_t*)(message_setting_row_pool[message_setting_row_pool_used].get()))->id = std::get<message_setting_row_option>(values[index]).id;
			((main_menu_message_setting_row_t*)(message_setting_row_pool[message_setting_row_pool_used].get()))->set_alternate(alternate);
			message_setting_row_pool[message_setting_row_pool_used]->impl_on_update(state);
			message_setting_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ message_setting_row_pool[0]->base_data.size.x, message_setting_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  main_menu_message_settings_message_settings_list_t::reset_pools() {
	message_setting_header_pool_used = 0;
	message_setting_row_pool_used = 0;
}
void main_menu_message_settings_message_notify_rebels_defeat_t::on_update(sys::state& state) noexcept {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent)); 
// BEGIN message_settings::message_notify_rebels_defeat::update
	set_active(state, state.user_settings.notify_rebels_defeat);
// END
}
bool main_menu_message_settings_message_notify_rebels_defeat_t::button_action(sys::state& state) noexcept {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent)); 
// BEGIN message_settings::message_notify_rebels_defeat::lbutton_action
	state.user_settings.notify_rebels_defeat = !state.user_settings.notify_rebels_defeat;
	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_message_settings_reset_t::on_update(sys::state& state) noexcept {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent)); 
// BEGIN message_settings::reset::update
// END
}
bool main_menu_message_settings_reset_t::button_action(sys::state& state) noexcept {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent)); 
// BEGIN message_settings::reset::lbutton_action
	sys::user_settings_s d{};
	std::memcpy(state.user_settings.self_message_settings, d.self_message_settings, sizeof(d.self_message_settings));
	std::memcpy(state.user_settings.other_message_settings, d.other_message_settings, sizeof(d.other_message_settings));
	std::memcpy(state.user_settings.interesting_message_settings, d.interesting_message_settings, sizeof(d.interesting_message_settings));
	state.user_setting_changed = true;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
ui::message_result main_menu_message_settings_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result main_menu_message_settings_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_message_settings_t::on_hide(sys::state& state) noexcept {
// BEGIN message_settings::on_hide
	alice_ui::display_at_front<alice_ui::make_main_menu_base>(state);
// END
}
void main_menu_message_settings_t::on_update(sys::state& state) noexcept {
// BEGIN message_settings::update
// END
	message_settings_list.update(state, this);
	remake_layout(state, true);
}
void main_menu_message_settings_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "title") {
					temp.ptr = title.get();
				} else
				if(cname == "message_notify_rebels_defeat") {
					temp.ptr = message_notify_rebels_defeat.get();
				} else
				if(cname == "reset") {
					temp.ptr = reset.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				if(cname == "message_settings_list") {
					temp.generator = &message_settings_list;
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_message_settings_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::message_settings"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "title") {
			title = std::make_unique<template_label>();
			title->parent = this;
			auto cptr = title.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "message_notify_rebels_defeat") {
			message_notify_rebels_defeat = std::make_unique<main_menu_message_settings_message_notify_rebels_defeat_t>();
			message_notify_rebels_defeat->parent = this;
			auto cptr = message_notify_rebels_defeat.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "reset") {
			reset = std::make_unique<main_menu_message_settings_reset_t>();
			reset->parent = this;
			auto cptr = reset.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabmessage_settings_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			main_section.read<std::string_view>(); // discard
			main_section.read<std::string_view>(); // discard
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			message_settings_table_setting_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_setting_name_column_start = running_w_total;
			col_section.read(message_settings_table_setting_name_column_width);
			running_w_total += message_settings_table_setting_name_column_width;
			col_section.read(message_settings_table_setting_name_column_text_color);
			col_section.read(message_settings_table_setting_name_header_text_color);
			col_section.read(message_settings_table_setting_name_text_alignment);
			message_settings_table_self_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_self_column_start = running_w_total;
			col_section.read(message_settings_table_self_column_width);
			running_w_total += message_settings_table_self_column_width;
			col_section.read(message_settings_table_self_column_text_color);
			col_section.read(message_settings_table_self_header_text_color);
			col_section.read(message_settings_table_self_text_alignment);
			message_settings_table_interesting_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_interesting_column_start = running_w_total;
			col_section.read(message_settings_table_interesting_column_width);
			running_w_total += message_settings_table_interesting_column_width;
			col_section.read(message_settings_table_interesting_column_text_color);
			col_section.read(message_settings_table_interesting_header_text_color);
			col_section.read(message_settings_table_interesting_text_alignment);
			message_settings_table_other_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_other_column_start = running_w_total;
			col_section.read(message_settings_table_other_column_width);
			running_w_total += message_settings_table_other_column_width;
			col_section.read(message_settings_table_other_column_text_color);
			col_section.read(message_settings_table_other_header_text_color);
			col_section.read(message_settings_table_other_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	message_settings_list.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN message_settings::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_message_settings(sys::state& state) {
	auto ptr = std::make_unique<main_menu_message_settings_t>();
	ptr->on_create(state);
	return ptr;
}
void main_menu_message_setting_row_overlay_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto table_source = (main_menu_message_settings_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_setting_name = rel_mouse_x >= table_source->message_settings_table_setting_name_column_start && rel_mouse_x < (table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_setting_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_setting_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_setting_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_setting_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y), float(table_source->message_settings_table_setting_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_setting_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_self = rel_mouse_x >= table_source->message_settings_table_self_column_start && rel_mouse_x < (table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_self){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_self_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_self){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_self){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y), float(table_source->message_settings_table_self_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_self_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_interesting = rel_mouse_x >= table_source->message_settings_table_interesting_column_start && rel_mouse_x < (table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_interesting){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_interesting_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_interesting){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_interesting){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y), float(table_source->message_settings_table_interesting_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_interesting_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_other = rel_mouse_x >= table_source->message_settings_table_other_column_start && rel_mouse_x < (table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_other){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_other_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_other){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_other){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y), float(table_source->message_settings_table_other_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_other_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void main_menu_message_setting_row_overlay_t::on_create(sys::state& state) noexcept {
}
void main_menu_message_setting_row_setting_name_t::on_update(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::setting_name::update
	set_text(state, text::produce_simple_string(state, get_setting_text_key(message_setting_row.id)));
// END
}
void main_menu_message_setting_row_self_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::self_icon::tooltip
	uint8_t current =  state.user_settings.self_message_settings[message_setting_row.id];
	
	if((current & sys::message_response::pause) != 0) {
		text::add_line(state, contents, "msg_handling_pause");
	} else if((current & sys::message_response::popup) != 0) {
		text::add_line(state, contents, "msg_handling_popup");
	} else if((current & sys::message_response::log) != 0) {
		text::add_line(state, contents, "msg_handling_log");
	} else if((current & sys::message_response::sound) != 0) {
		text::add_line(state, contents, "msg_handling_sound");
	} else {
		text::add_line(state, contents, "msg_handling_ignore");
	}
// END
}
void main_menu_message_setting_row_self_icon_t::on_update(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::self_icon::update
	static const int32_t icon_levels[] = { template_project::icon_by_name(state.ui_templates, "ic_fluent_mail_prohibited_28_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_mail_48_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_card_ui_24_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_hourglass_24_regular.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_speaker_2_48_filled.svg") };
	uint8_t current = state.user_settings.self_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		icon = icon_levels[3];
	} else if((current & sys::message_response::popup) != 0) {
		icon = icon_levels[2];
	} else if((current & sys::message_response::log) != 0) {
		icon = icon_levels[1];
	} else if((current & sys::message_response::sound) != 0) {
		icon = icon_levels[4];
	} else {
		icon = icon_levels[0];
	}
// END
}
bool main_menu_message_setting_row_self_icon_t::button_action(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::self_icon::lbutton_action
	uint8_t current = state.user_settings.self_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		current = sys::message_response::ignore;
	} else if((current & sys::message_response::popup) != 0) {
		current = sys::message_response::standard_pause;
	} else if((current & sys::message_response::log) != 0) {
		current = sys::message_response::standard_popup;
	} else if((current & sys::message_response::sound) != 0) {
		current = sys::message_response::standard_log;
	} else {
		current = sys::message_response::standard_sound;
	}
	state.user_settings.self_message_settings[message_setting_row.id] = current;

	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
bool main_menu_message_setting_row_self_icon_t::button_right_action(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::self_icon::rbutton_action
	uint8_t current = state.user_settings.self_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		current = sys::message_response::standard_popup;
	} else if((current & sys::message_response::popup) != 0) {
		current = sys::message_response::standard_log;
	} else if((current & sys::message_response::log) != 0) {
		current = sys::message_response::sound;
	} else if((current & sys::message_response::sound) != 0) {
		current = sys::message_response::ignore;
	} else {
		current = sys::message_response::standard_pause;
	}
	state.user_settings.self_message_settings[message_setting_row.id] = current;

	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_message_setting_row_interesting_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::interesting_icon::tooltip
	uint8_t current = state.user_settings.interesting_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		text::add_line(state, contents, "msg_handling_pause");
	} else if((current & sys::message_response::popup) != 0) {
		text::add_line(state, contents, "msg_handling_popup");
	} else if((current & sys::message_response::log) != 0) {
		text::add_line(state, contents, "msg_handling_log");
	} else if((current & sys::message_response::sound) != 0) {
		text::add_line(state, contents, "msg_handling_sound");
	} else {
		text::add_line(state, contents, "msg_handling_ignore");
	}
// END
}
void main_menu_message_setting_row_interesting_icon_t::on_update(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::interesting_icon::update
	static const int32_t icon_levels[] = { template_project::icon_by_name(state.ui_templates, "ic_fluent_mail_prohibited_28_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_mail_48_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_card_ui_24_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_hourglass_24_regular.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_speaker_2_48_filled.svg") };
	uint8_t current = state.user_settings.interesting_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		icon = icon_levels[3];
	} else if((current & sys::message_response::popup) != 0) {
		icon = icon_levels[2];
	} else if((current & sys::message_response::log) != 0) {
		icon = icon_levels[1];
	} else if((current & sys::message_response::sound) != 0) {
		icon = icon_levels[4];
	} else {
		icon = icon_levels[0];
	}
// END
}
bool main_menu_message_setting_row_interesting_icon_t::button_action(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::interesting_icon::lbutton_action
	uint8_t current = state.user_settings.interesting_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		current = sys::message_response::ignore;
	} else if((current & sys::message_response::popup) != 0) {
		current = sys::message_response::standard_pause;
	} else if((current & sys::message_response::log) != 0) {
		current = sys::message_response::standard_popup;
	} else if((current & sys::message_response::sound) != 0) {
		current = sys::message_response::standard_log;
	} else {
		current = sys::message_response::standard_sound;
	}
	state.user_settings.interesting_message_settings[message_setting_row.id] = current;

	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
bool main_menu_message_setting_row_interesting_icon_t::button_right_action(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::interesting_icon::rbutton_action
	uint8_t current = state.user_settings.interesting_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		current = sys::message_response::standard_popup;
	} else if((current & sys::message_response::popup) != 0) {
		current = sys::message_response::standard_log;
	} else if((current & sys::message_response::log) != 0) {
		current = sys::message_response::sound;
	} else if((current & sys::message_response::sound) != 0) {
		current = sys::message_response::ignore;
	} else {
		current = sys::message_response::standard_pause;
	}
	state.user_settings.interesting_message_settings[message_setting_row.id] = current;

	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void main_menu_message_setting_row_other_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::other_icon::tooltip
	uint8_t current = state.user_settings.other_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		text::add_line(state, contents, "msg_handling_pause");
	} else if((current & sys::message_response::popup) != 0) {
		text::add_line(state, contents, "msg_handling_popup");
	} else if((current & sys::message_response::log) != 0) {
		text::add_line(state, contents, "msg_handling_log");
	} else if((current & sys::message_response::sound) != 0) {
		text::add_line(state, contents, "msg_handling_sound");
	} else {
		text::add_line(state, contents, "msg_handling_ignore");
	}
// END
}
void main_menu_message_setting_row_other_icon_t::on_update(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::other_icon::update
	static const int32_t icon_levels[] = { template_project::icon_by_name(state.ui_templates, "ic_fluent_mail_prohibited_28_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_mail_48_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_card_ui_24_filled.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_hourglass_24_regular.svg"), template_project::icon_by_name(state.ui_templates, "ic_fluent_speaker_2_48_filled.svg") };
	uint8_t current = state.user_settings.other_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		icon = icon_levels[3];
	} else if((current & sys::message_response::popup) != 0) {
		icon = icon_levels[2];
	} else if((current & sys::message_response::log) != 0) {
		icon = icon_levels[1];
	} else if((current & sys::message_response::sound) != 0) {
		icon = icon_levels[4];
	} else {
		icon = icon_levels[0];
	}
// END
}
bool main_menu_message_setting_row_other_icon_t::button_action(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::other_icon::lbutton_action
	uint8_t current = state.user_settings.other_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		current = sys::message_response::ignore;
	} else if((current & sys::message_response::popup) != 0) {
		current = sys::message_response::standard_pause;
	} else if((current & sys::message_response::log) != 0) {
		current = sys::message_response::standard_popup;
	} else if((current & sys::message_response::sound) != 0) {
		current = sys::message_response::standard_log;
	} else {
		current = sys::message_response::standard_sound;
	}
	state.user_settings.other_message_settings[message_setting_row.id] = current;

	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
bool main_menu_message_setting_row_other_icon_t::button_right_action(sys::state& state) noexcept {
	main_menu_message_setting_row_t& message_setting_row = *((main_menu_message_setting_row_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::other_icon::rbutton_action
	uint8_t current = state.user_settings.other_message_settings[message_setting_row.id];

	if((current & sys::message_response::pause) != 0) {
		current = sys::message_response::standard_popup;
	} else if((current & sys::message_response::popup) != 0) {
		current = sys::message_response::standard_log;
	} else if((current & sys::message_response::log) != 0) {
		current = sys::message_response::sound;
	} else if((current & sys::message_response::sound) != 0) {
		current = sys::message_response::ignore;
	} else {
		current = sys::message_response::standard_pause;
	}
	state.user_settings.other_message_settings[message_setting_row.id] = current;

	state.user_setting_changed = true;
	parent->impl_on_update(state);
// END
	return true;
}
void  main_menu_message_setting_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result main_menu_message_setting_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result main_menu_message_setting_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_message_setting_row_t::on_update(sys::state& state) noexcept {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_row::update
// END
	remake_layout(state, true);
}
void main_menu_message_setting_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "overlay") {
					temp.ptr = overlay.get();
				} else
				if(cname == "setting_name") {
					temp.ptr = setting_name.get();
				} else
				if(cname == "self_icon") {
					temp.ptr = self_icon.get();
				} else
				if(cname == "interesting_icon") {
					temp.ptr = interesting_icon.get();
				} else
				if(cname == "other_icon") {
					temp.ptr = other_icon.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_message_setting_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::message_setting_row"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "overlay") {
			overlay = std::make_unique<main_menu_message_setting_row_overlay_t>();
			overlay->parent = this;
			auto cptr = overlay.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "setting_name") {
			setting_name = std::make_unique<main_menu_message_setting_row_setting_name_t>();
			setting_name->parent = this;
			auto cptr = setting_name.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "self_icon") {
			self_icon = std::make_unique<main_menu_message_setting_row_self_icon_t>();
			self_icon->parent = this;
			auto cptr = self_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "interesting_icon") {
			interesting_icon = std::make_unique<main_menu_message_setting_row_interesting_icon_t>();
			interesting_icon->parent = this;
			auto cptr = interesting_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "other_icon") {
			other_icon = std::make_unique<main_menu_message_setting_row_other_icon_t>();
			other_icon->parent = this;
			auto cptr = other_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabmessage_settings_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			main_section.read<std::string_view>(); // discard
			main_section.read<std::string_view>(); // discard
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			message_settings_table_setting_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_setting_name_column_start = running_w_total;
			col_section.read(message_settings_table_setting_name_column_width);
			running_w_total += message_settings_table_setting_name_column_width;
			col_section.read(message_settings_table_setting_name_column_text_color);
			col_section.read(message_settings_table_setting_name_header_text_color);
			col_section.read(message_settings_table_setting_name_text_alignment);
			message_settings_table_self_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_self_column_start = running_w_total;
			col_section.read(message_settings_table_self_column_width);
			running_w_total += message_settings_table_self_column_width;
			col_section.read(message_settings_table_self_column_text_color);
			col_section.read(message_settings_table_self_header_text_color);
			col_section.read(message_settings_table_self_text_alignment);
			message_settings_table_interesting_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_interesting_column_start = running_w_total;
			col_section.read(message_settings_table_interesting_column_width);
			running_w_total += message_settings_table_interesting_column_width;
			col_section.read(message_settings_table_interesting_column_text_color);
			col_section.read(message_settings_table_interesting_header_text_color);
			col_section.read(message_settings_table_interesting_text_alignment);
			message_settings_table_other_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_other_column_start = running_w_total;
			col_section.read(message_settings_table_other_column_width);
			running_w_total += message_settings_table_other_column_width;
			col_section.read(message_settings_table_other_column_text_color);
			col_section.read(message_settings_table_other_header_text_color);
			col_section.read(message_settings_table_other_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN message_setting_row::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_message_setting_row(sys::state& state) {
	auto ptr = std::make_unique<main_menu_message_setting_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result main_menu_message_setting_header_contents_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (main_menu_message_settings_t*)(parent->parent);
	return ui::message_result::consumed;}
ui::message_result main_menu_message_setting_header_contents_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_message_setting_header_contents_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (main_menu_message_settings_t*)(parent->parent);
	if(x >= table_source->message_settings_table_setting_name_column_start && x < table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width) {
	}
	if(x >= table_source->message_settings_table_self_column_start && x < table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width) {
	}
	if(x >= table_source->message_settings_table_interesting_column_start && x < table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width) {
	}
	if(x >= table_source->message_settings_table_other_column_start && x < table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void main_menu_message_setting_header_contents_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (main_menu_message_settings_t*)(parent->parent);
	if(x >=  table_source->message_settings_table_setting_name_column_start && x <  table_source->message_settings_table_setting_name_column_start +  table_source->message_settings_table_setting_name_column_width) {
	}
	if(x >=  table_source->message_settings_table_self_column_start && x <  table_source->message_settings_table_self_column_start +  table_source->message_settings_table_self_column_width) {
	}
	if(x >=  table_source->message_settings_table_interesting_column_start && x <  table_source->message_settings_table_interesting_column_start +  table_source->message_settings_table_interesting_column_width) {
	}
	if(x >=  table_source->message_settings_table_other_column_start && x <  table_source->message_settings_table_other_column_start +  table_source->message_settings_table_other_column_width) {
	}
}
void main_menu_message_setting_header_contents_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (main_menu_message_settings_t*)(parent->parent);
	{
	setting_name_cached_text = text::produce_simple_string(state, table_source->message_settings_table_setting_name_header_text_key);
	 setting_name_internal_layout.contents.clear();
	 setting_name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  setting_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->message_settings_table_setting_name_column_width - 18), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 18), 0, table_source->message_settings_table_setting_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, setting_name_cached_text);
	}
	{
	self_cached_text = text::produce_simple_string(state, table_source->message_settings_table_self_header_text_key);
	 self_internal_layout.contents.clear();
	 self_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  self_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->message_settings_table_self_column_width - 18), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 18), 0, table_source->message_settings_table_self_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, self_cached_text);
	}
	{
	interesting_cached_text = text::produce_simple_string(state, table_source->message_settings_table_interesting_header_text_key);
	 interesting_internal_layout.contents.clear();
	 interesting_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  interesting_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->message_settings_table_interesting_column_width - 18), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 18), 0, table_source->message_settings_table_interesting_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, interesting_cached_text);
	}
	{
	other_cached_text = text::produce_simple_string(state, table_source->message_settings_table_other_header_text_key);
	 other_internal_layout.contents.clear();
	 other_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  other_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->message_settings_table_other_column_width - 18), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 18), 0, table_source->message_settings_table_other_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, other_cached_text);
	}
}
void main_menu_message_setting_header_contents_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 18);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (main_menu_message_settings_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_setting_name = rel_mouse_x >= table_source->message_settings_table_setting_name_column_start && rel_mouse_x < (table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_setting_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_setting_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_setting_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start + table_source->message_settings_table_setting_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_setting_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y), float(table_source->message_settings_table_setting_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_setting_name_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_setting_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_setting_name = state.ui_templates.colors[table_source->message_settings_table_setting_name_header_text_color]; 	if(!setting_name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : setting_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->message_settings_table_setting_name_column_start + 9, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_setting_name.r, col_color_setting_name.g, col_color_setting_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_self = rel_mouse_x >= table_source->message_settings_table_self_column_start && rel_mouse_x < (table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_self){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_self_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_self){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start + table_source->message_settings_table_self_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_self){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y), float(table_source->message_settings_table_self_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_self_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_self_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_self = state.ui_templates.colors[table_source->message_settings_table_self_header_text_color]; 	if(!self_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : self_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->message_settings_table_self_column_start + 9, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_self.r, col_color_self.g, col_color_self.b }, ogl::color_modification::none);
		}
	}
	bool col_um_interesting = rel_mouse_x >= table_source->message_settings_table_interesting_column_start && rel_mouse_x < (table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_interesting){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_interesting_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_interesting){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start + table_source->message_settings_table_interesting_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_interesting){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y), float(table_source->message_settings_table_interesting_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_interesting_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_interesting_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_interesting = state.ui_templates.colors[table_source->message_settings_table_interesting_header_text_color]; 	if(!interesting_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : interesting_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->message_settings_table_interesting_column_start + 9, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_interesting.r, col_color_interesting.g, col_color_interesting.b }, ogl::color_modification::none);
		}
	}
	bool col_um_other = rel_mouse_x >= table_source->message_settings_table_other_column_start && rel_mouse_x < (table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_other){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->message_settings_table_other_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_other){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start + table_source->message_settings_table_other_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_other){
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y), float(table_source->message_settings_table_other_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->message_settings_table_other_column_start), float(y + base_data.size.y - 2), float(table_source->message_settings_table_other_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_other = state.ui_templates.colors[table_source->message_settings_table_other_header_text_color]; 	if(!other_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : other_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->message_settings_table_other_column_start + 9, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_other.r, col_color_other.g, col_color_other.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void main_menu_message_setting_header_contents_t::on_update(sys::state& state) noexcept {
	main_menu_message_setting_header_t& message_setting_header = *((main_menu_message_setting_header_t*)(parent)); 
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_header::contents::update
// END
}
void main_menu_message_setting_header_contents_t::on_create(sys::state& state) noexcept {
// BEGIN message_setting_header::contents::create
// END
}
void  main_menu_message_setting_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result main_menu_message_setting_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result main_menu_message_setting_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void main_menu_message_setting_header_t::on_update(sys::state& state) noexcept {
	main_menu_message_settings_t& message_settings = *((main_menu_message_settings_t*)(parent->parent)); 
// BEGIN message_setting_header::update
// END
	remake_layout(state, true);
}
void main_menu_message_setting_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "contents") {
					temp.ptr = contents.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				if(cname == "base") {
					temp.ptr = make_main_menu_base(state);
				}
				if(cname == "sound") {
					temp.ptr = make_main_menu_sound(state);
				}
				if(cname == "misc") {
					temp.ptr = make_main_menu_misc(state);
				}
				if(cname == "string_dropdown_choice") {
					temp.ptr = make_main_menu_string_dropdown_choice(state);
				}
				if(cname == "graphics") {
					temp.ptr = make_main_menu_graphics(state);
				}
				if(cname == "message_settings") {
					temp.ptr = make_main_menu_message_settings(state);
				}
				if(cname == "message_setting_row") {
					temp.ptr = make_main_menu_message_setting_row(state);
				}
				if(cname == "message_setting_header") {
					temp.ptr = make_main_menu_message_setting_header(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void main_menu_message_setting_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("main_menu::message_setting_header"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "contents") {
			contents = std::make_unique<main_menu_message_setting_header_contents_t>();
			contents->parent = this;
			auto cptr = contents.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabmessage_settings_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			main_section.read<std::string_view>(); // discard
			main_section.read<std::string_view>(); // discard
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			message_settings_table_setting_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_setting_name_column_start = running_w_total;
			col_section.read(message_settings_table_setting_name_column_width);
			running_w_total += message_settings_table_setting_name_column_width;
			col_section.read(message_settings_table_setting_name_column_text_color);
			col_section.read(message_settings_table_setting_name_header_text_color);
			col_section.read(message_settings_table_setting_name_text_alignment);
			message_settings_table_self_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_self_column_start = running_w_total;
			col_section.read(message_settings_table_self_column_width);
			running_w_total += message_settings_table_self_column_width;
			col_section.read(message_settings_table_self_column_text_color);
			col_section.read(message_settings_table_self_header_text_color);
			col_section.read(message_settings_table_self_text_alignment);
			message_settings_table_interesting_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_interesting_column_start = running_w_total;
			col_section.read(message_settings_table_interesting_column_width);
			running_w_total += message_settings_table_interesting_column_width;
			col_section.read(message_settings_table_interesting_column_text_color);
			col_section.read(message_settings_table_interesting_header_text_color);
			col_section.read(message_settings_table_interesting_text_alignment);
			message_settings_table_other_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			message_settings_table_other_column_start = running_w_total;
			col_section.read(message_settings_table_other_column_width);
			running_w_total += message_settings_table_other_column_width;
			col_section.read(message_settings_table_other_column_text_color);
			col_section.read(message_settings_table_other_header_text_color);
			col_section.read(message_settings_table_other_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN message_setting_header::create
// END
}
std::unique_ptr<ui::element_base> make_main_menu_message_setting_header(sys::state& state) {
	auto ptr = std::make_unique<main_menu_message_setting_header_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
