// BEGIN prelude
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif
// END

namespace alice_ui {
struct logisticswindow_main_selected_commodity_icon_t;
struct logisticswindow_main_stockpile_target_input_t;
struct logisticswindow_main_confirm_target_button_t;
struct logisticswindow_main_stockpile_target_error_t;
struct logisticswindow_main_only_military_goods_toggle_t;
struct logisticswindow_main_prio_armies_in_battle_supply_toggle_t;
struct logisticswindow_main_prio_navies_in_battle_supply_toggle_t;
struct logisticswindow_main_t;
struct logisticswindow_section_header_label_t;
struct logisticswindow_section_header_llbutton_t;
struct logisticswindow_section_header_lbutton_t;
struct logisticswindow_section_header_rbutton_t;
struct logisticswindow_section_header_rrbutton_t;
struct logisticswindow_section_header_setting_amount_t;
struct logisticswindow_section_header_expand_button_t;
struct logisticswindow_section_header_satisfaction_t;
struct logisticswindow_section_header_min_setting_t;
struct logisticswindow_section_header_max_setting_t;
struct logisticswindow_section_header_info_section_header_t;
struct logisticswindow_section_header_supply_loss_t;
struct logisticswindow_section_header_supply_throughput_t;
struct logisticswindow_section_header_t;
struct logisticswindow_neutral_spacer_t;
struct logisticswindow_bottom_spacer_t;
struct logisticswindow_consumption_row_contents_t;
struct logisticswindow_consumption_row_t;
struct logisticswindow_consumption_header_contents_t;
struct logisticswindow_consumption_header_t;
struct logisticswindow_commodity_grid_item_commodity_icon_t;
struct logisticswindow_commodity_grid_item_current_stockpile_count_t;
struct logisticswindow_commodity_grid_item_stockpile_target_t;
struct logisticswindow_commodity_grid_item_daily_stockpile_change_t;
struct logisticswindow_commodity_grid_item_days_left_t;
struct logisticswindow_commodity_grid_item_t;
struct logisticswindow_commodity_spacer_t;
struct logisticswindow_main_selected_commodity_icon_t : public ui::element_base {
// BEGIN main::selected_commodity_icon::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_main_stockpile_target_input_t : public ui::edit_box_element_base {
// BEGIN main::stockpile_target_input::variables
// END
	void on_edit_command(sys::state& state, ui::edit_command command, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
};
struct logisticswindow_main_confirm_target_button_t : public alice_ui::template_mixed_button {
// BEGIN main::confirm_target_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_main_stockpile_target_error_t : public ui::element_base {
// BEGIN main::stockpile_target_error::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::left;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_main_only_military_goods_toggle_t : public alice_ui::template_toggle_button {
// BEGIN main::only_military_goods_toggle::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_main_prio_armies_in_battle_supply_toggle_t : public alice_ui::template_toggle_button {
// BEGIN main::prio_armies_in_battle_supply_toggle::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_main_prio_navies_in_battle_supply_toggle_t : public alice_ui::template_toggle_button {
// BEGIN main::prio_navies_in_battle_supply_toggle::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_main_military_table_t : public layout_generator {
// BEGIN main::military_table::variables
// END
	struct section_header_option { int32_t section_type; };
	std::vector<std::unique_ptr<ui::element_base>> section_header_pool;
	int32_t section_header_pool_used = 0;
	void add_section_header( int32_t section_type);
	struct neutral_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> neutral_spacer_pool;
	int32_t neutral_spacer_pool_used = 0;
	void add_neutral_spacer();
	struct bottom_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> bottom_spacer_pool;
	int32_t bottom_spacer_pool_used = 0;
	void add_bottom_spacer();
	struct consumption_row_option { std::string name; float fufilled; float required; float satisfaction; };
	std::vector<std::unique_ptr<ui::element_base>> consumption_row_pool;
	int32_t consumption_row_pool_used = 0;
	void add_consumption_row( std::string name,  float fufilled,  float required,  float satisfaction);
	std::vector<std::unique_ptr<ui::element_base>> consumption_header_pool;
	int32_t consumption_header_pool_used = 0;
	std::vector<std::variant<std::monostate, section_header_option, neutral_spacer_option, bottom_spacer_option, consumption_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct logisticswindow_main_consumption_table_t : public layout_generator {
// BEGIN main::consumption_table::variables
// END
	struct section_header_option { int32_t section_type; };
	std::vector<std::unique_ptr<ui::element_base>> section_header_pool;
	int32_t section_header_pool_used = 0;
	void add_section_header( int32_t section_type);
	struct neutral_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> neutral_spacer_pool;
	int32_t neutral_spacer_pool_used = 0;
	void add_neutral_spacer();
	struct bottom_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> bottom_spacer_pool;
	int32_t bottom_spacer_pool_used = 0;
	void add_bottom_spacer();
	struct consumption_row_option { std::string name; float fufilled; float required; float satisfaction; };
	std::vector<std::unique_ptr<ui::element_base>> consumption_row_pool;
	int32_t consumption_row_pool_used = 0;
	void add_consumption_row( std::string name,  float fufilled,  float required,  float satisfaction);
	std::vector<std::unique_ptr<ui::element_base>> consumption_header_pool;
	int32_t consumption_header_pool_used = 0;
	std::vector<std::variant<std::monostate, section_header_option, neutral_spacer_option, bottom_spacer_option, consumption_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct logisticswindow_main_commodity_grid_g_t : public layout_generator {
// BEGIN main::commodity_grid_g::variables
// END
	struct commodity_grid_item_option { dcon::commodity_id value; };
	std::vector<std::unique_ptr<ui::element_base>> commodity_grid_item_pool;
	int32_t commodity_grid_item_pool_used = 0;
	void add_commodity_grid_item( dcon::commodity_id value);
	struct commodity_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> commodity_spacer_pool;
	int32_t commodity_spacer_pool_used = 0;
	void add_commodity_spacer();
	std::vector<std::variant<std::monostate, commodity_grid_item_option, commodity_spacer_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct logisticswindow_section_header_label_t : public alice_ui::template_label {
// BEGIN section_header::label::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_llbutton_t : public alice_ui::template_icon_button {
// BEGIN section_header::llbutton::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_lbutton_t : public alice_ui::template_icon_button {
// BEGIN section_header::lbutton::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_rbutton_t : public alice_ui::template_icon_button {
// BEGIN section_header::rbutton::variables
// END
	bool button_action(sys::state& state) noexcept override;
	bool button_shift_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_rrbutton_t : public alice_ui::template_icon_button {
// BEGIN section_header::rrbutton::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_setting_amount_t : public alice_ui::template_label {
// BEGIN section_header::setting_amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_expand_button_t : public alice_ui::template_icon_button {
// BEGIN section_header::expand_button::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_satisfaction_t : public alice_ui::template_label {
// BEGIN section_header::satisfaction::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_min_setting_t : public alice_ui::template_label {
// BEGIN section_header::min_setting::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_max_setting_t : public alice_ui::template_label {
// BEGIN section_header::max_setting::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_info_section_header_t : public alice_ui::template_icon_graphic {
// BEGIN section_header::info_section_header::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_supply_loss_t : public alice_ui::template_label {
// BEGIN section_header::supply_loss::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_section_header_supply_throughput_t : public alice_ui::template_label {
// BEGIN section_header::supply_throughput::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_consumption_row_contents_t : public ui::element_base {
// BEGIN consumption_row::contents::variables
// END
	int32_t template_id = -1;
	text::layout item_name_internal_layout;
	int32_t  item_name_text_color = 1;
	std::string item_name_cached_text;
	void set_item_name_text(sys::state & state, std::string const& new_text);
	text::layout item_amount_internal_layout;
	int32_t  item_amount_text_color = 1;
	std::string item_amount_cached_text;
	void set_item_amount_text(sys::state & state, std::string const& new_text);
	text::layout fufillment_percent_internal_layout;
	int32_t  fufillment_percent_text_color = 1;
	std::string fufillment_percent_cached_text;
	void set_fufillment_percent_text(sys::state & state, std::string const& new_text);
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
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_consumption_header_contents_t : public ui::element_base {
// BEGIN consumption_header::contents::variables
// END
	int32_t template_id = -1;
	text::layout item_name_internal_layout;
	std::string item_name_cached_text;
	text::layout item_amount_internal_layout;
	std::string item_amount_cached_text;
	text::layout fufillment_percent_internal_layout;
	std::string fufillment_percent_cached_text;
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
struct logisticswindow_commodity_grid_item_commodity_icon_t : public ui::element_base {
// BEGIN commodity_grid_item::commodity_icon::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
	dcon::text_key tooltip_key;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_commodity_grid_item_current_stockpile_count_t : public alice_ui::template_label {
// BEGIN commodity_grid_item::current_stockpile_count::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_commodity_grid_item_stockpile_target_t : public alice_ui::template_label {
// BEGIN commodity_grid_item::stockpile_target::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_commodity_grid_item_daily_stockpile_change_t : public alice_ui::template_label {
// BEGIN commodity_grid_item::daily_stockpile_change::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_commodity_grid_item_days_left_t : public alice_ui::template_label {
// BEGIN commodity_grid_item::days_left::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct logisticswindow_main_t : public layout_window_element {
// BEGIN main::variables
// END
	dcon::commodity_id selected_stockpile_commodity;
	bool show_only_military_goods;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> military_consumption_title;
	std::unique_ptr<logisticswindow_main_selected_commodity_icon_t> selected_commodity_icon;
	std::unique_ptr<logisticswindow_main_stockpile_target_input_t> stockpile_target_input;
	std::unique_ptr<logisticswindow_main_confirm_target_button_t> confirm_target_button;
	std::unique_ptr<logisticswindow_main_stockpile_target_error_t> stockpile_target_error;
	std::unique_ptr<template_label> stockpile_target_editbox_label;
	std::unique_ptr<template_icon_graphic> info_stockpile_target_view;
	std::unique_ptr<template_icon_graphic> info_stockpile_view;
	std::unique_ptr<logisticswindow_main_only_military_goods_toggle_t> only_military_goods_toggle;
	std::unique_ptr<logisticswindow_main_prio_armies_in_battle_supply_toggle_t> prio_armies_in_battle_supply_toggle;
	std::unique_ptr<logisticswindow_main_prio_navies_in_battle_supply_toggle_t> prio_navies_in_battle_supply_toggle;
	std::unique_ptr<template_label> construction_consumption_title;
	std::unique_ptr<template_label> government_stockpiles_title;
	logisticswindow_main_military_table_t military_table;
	logisticswindow_main_consumption_table_t consumption_table;
	logisticswindow_main_commodity_grid_g_t commodity_grid_g;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	int16_t consumption_table_lead_space_column_start = 0;
	int16_t consumption_table_lead_space_column_width = 0;
	std::string_view consumption_table_item_name_header_text_key;
	uint8_t consumption_table_item_name_header_text_color = 1;
	uint8_t consumption_table_item_name_column_text_color = 1;
	text::alignment consumption_table_item_name_text_alignment = text::alignment::left;
	int8_t consumption_table_item_name_sort_direction = 0;
	int16_t consumption_table_item_name_column_start = 0;
	int16_t consumption_table_item_name_column_width = 0;
	std::string_view consumption_table_item_amount_header_text_key;
	std::string_view consumption_table_item_amount_header_tooltip_key;
	uint8_t consumption_table_item_amount_header_text_color = 1;
	uint8_t consumption_table_item_amount_column_text_color = 1;
	text::alignment consumption_table_item_amount_text_alignment = text::alignment::center;
	std::string_view consumption_table_item_amount_column_tooltip_key;
	int8_t consumption_table_item_amount_sort_direction = 0;
	int16_t consumption_table_item_amount_column_start = 0;
	int16_t consumption_table_item_amount_column_width = 0;
	std::string_view consumption_table_fufillment_percent_header_text_key;
	uint8_t consumption_table_fufillment_percent_header_text_color = 1;
	uint8_t consumption_table_fufillment_percent_column_text_color = 1;
	text::alignment consumption_table_fufillment_percent_text_alignment = text::alignment::center;
	int8_t consumption_table_fufillment_percent_sort_direction = 0;
	int16_t consumption_table_fufillment_percent_column_start = 0;
	int16_t consumption_table_fufillment_percent_column_width = 0;
	std::string_view consumption_table_ascending_icon_key;
	dcon::texture_id consumption_table_ascending_icon;
	std::string_view consumption_table_descending_icon_key;
	dcon::texture_id consumption_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
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
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "selected_stockpile_commodity") {
			return (void*)(&selected_stockpile_commodity);
		}
		if(name_parameter == "show_only_military_goods") {
			return (void*)(&show_only_military_goods);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_logisticswindow_main(sys::state& state);
struct logisticswindow_section_header_t : public layout_window_element {
// BEGIN section_header::variables
// END
	int32_t section_type;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<logisticswindow_section_header_label_t> label;
	std::unique_ptr<logisticswindow_section_header_llbutton_t> llbutton;
	std::unique_ptr<logisticswindow_section_header_lbutton_t> lbutton;
	std::unique_ptr<logisticswindow_section_header_rbutton_t> rbutton;
	std::unique_ptr<logisticswindow_section_header_rrbutton_t> rrbutton;
	std::unique_ptr<logisticswindow_section_header_setting_amount_t> setting_amount;
	std::unique_ptr<logisticswindow_section_header_expand_button_t> expand_button;
	std::unique_ptr<logisticswindow_section_header_satisfaction_t> satisfaction;
	std::unique_ptr<logisticswindow_section_header_min_setting_t> min_setting;
	std::unique_ptr<logisticswindow_section_header_max_setting_t> max_setting;
	std::unique_ptr<logisticswindow_section_header_info_section_header_t> info_section_header;
	std::unique_ptr<logisticswindow_section_header_supply_loss_t> supply_loss;
	std::unique_ptr<logisticswindow_section_header_supply_throughput_t> supply_throughput;
	std::unique_ptr<template_label> satisfaction_label;
	std::unique_ptr<template_label> supply_loss_label;
	std::unique_ptr<template_label> supply_throughput_label;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "section_type") {
			return (void*)(&section_type);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_logisticswindow_section_header(sys::state& state);
struct logisticswindow_neutral_spacer_t : public layout_window_element {
// BEGIN neutral_spacer::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_logisticswindow_neutral_spacer(sys::state& state);
struct logisticswindow_bottom_spacer_t : public layout_window_element {
// BEGIN bottom_spacer::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_logisticswindow_bottom_spacer(sys::state& state);
struct logisticswindow_consumption_row_t : public layout_window_element {
// BEGIN consumption_row::variables
// END
	std::string name;
	float fufilled;
	float required;
	float satisfaction;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<logisticswindow_consumption_row_contents_t> contents;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	int16_t consumption_table_lead_space_column_start = 0;
	int16_t consumption_table_lead_space_column_width = 0;
	std::string_view consumption_table_item_name_header_text_key;
	uint8_t consumption_table_item_name_header_text_color = 1;
	uint8_t consumption_table_item_name_column_text_color = 1;
	text::alignment consumption_table_item_name_text_alignment = text::alignment::left;
	int8_t consumption_table_item_name_sort_direction = 0;
	int16_t consumption_table_item_name_column_start = 0;
	int16_t consumption_table_item_name_column_width = 0;
	std::string_view consumption_table_item_amount_header_text_key;
	std::string_view consumption_table_item_amount_header_tooltip_key;
	uint8_t consumption_table_item_amount_header_text_color = 1;
	uint8_t consumption_table_item_amount_column_text_color = 1;
	text::alignment consumption_table_item_amount_text_alignment = text::alignment::center;
	std::string_view consumption_table_item_amount_column_tooltip_key;
	int8_t consumption_table_item_amount_sort_direction = 0;
	int16_t consumption_table_item_amount_column_start = 0;
	int16_t consumption_table_item_amount_column_width = 0;
	std::string_view consumption_table_fufillment_percent_header_text_key;
	uint8_t consumption_table_fufillment_percent_header_text_color = 1;
	uint8_t consumption_table_fufillment_percent_column_text_color = 1;
	text::alignment consumption_table_fufillment_percent_text_alignment = text::alignment::center;
	int8_t consumption_table_fufillment_percent_sort_direction = 0;
	int16_t consumption_table_fufillment_percent_column_start = 0;
	int16_t consumption_table_fufillment_percent_column_width = 0;
	std::string_view consumption_table_ascending_icon_key;
	dcon::texture_id consumption_table_ascending_icon;
	std::string_view consumption_table_descending_icon_key;
	dcon::texture_id consumption_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "name") {
			return (void*)(&name);
		}
		if(name_parameter == "fufilled") {
			return (void*)(&fufilled);
		}
		if(name_parameter == "required") {
			return (void*)(&required);
		}
		if(name_parameter == "satisfaction") {
			return (void*)(&satisfaction);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_logisticswindow_consumption_row(sys::state& state);
struct logisticswindow_consumption_header_t : public layout_window_element {
// BEGIN consumption_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<logisticswindow_consumption_header_contents_t> contents;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	int16_t consumption_table_lead_space_column_start = 0;
	int16_t consumption_table_lead_space_column_width = 0;
	std::string_view consumption_table_item_name_header_text_key;
	uint8_t consumption_table_item_name_header_text_color = 1;
	uint8_t consumption_table_item_name_column_text_color = 1;
	text::alignment consumption_table_item_name_text_alignment = text::alignment::left;
	int8_t consumption_table_item_name_sort_direction = 0;
	int16_t consumption_table_item_name_column_start = 0;
	int16_t consumption_table_item_name_column_width = 0;
	std::string_view consumption_table_item_amount_header_text_key;
	std::string_view consumption_table_item_amount_header_tooltip_key;
	uint8_t consumption_table_item_amount_header_text_color = 1;
	uint8_t consumption_table_item_amount_column_text_color = 1;
	text::alignment consumption_table_item_amount_text_alignment = text::alignment::center;
	std::string_view consumption_table_item_amount_column_tooltip_key;
	int8_t consumption_table_item_amount_sort_direction = 0;
	int16_t consumption_table_item_amount_column_start = 0;
	int16_t consumption_table_item_amount_column_width = 0;
	std::string_view consumption_table_fufillment_percent_header_text_key;
	uint8_t consumption_table_fufillment_percent_header_text_color = 1;
	uint8_t consumption_table_fufillment_percent_column_text_color = 1;
	text::alignment consumption_table_fufillment_percent_text_alignment = text::alignment::center;
	int8_t consumption_table_fufillment_percent_sort_direction = 0;
	int16_t consumption_table_fufillment_percent_column_start = 0;
	int16_t consumption_table_fufillment_percent_column_width = 0;
	std::string_view consumption_table_ascending_icon_key;
	dcon::texture_id consumption_table_ascending_icon;
	std::string_view consumption_table_descending_icon_key;
	dcon::texture_id consumption_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_logisticswindow_consumption_header(sys::state& state);
struct logisticswindow_commodity_grid_item_t : public layout_window_element {
// BEGIN commodity_grid_item::variables
// END
	dcon::commodity_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<logisticswindow_commodity_grid_item_commodity_icon_t> commodity_icon;
	std::unique_ptr<logisticswindow_commodity_grid_item_current_stockpile_count_t> current_stockpile_count;
	std::unique_ptr<logisticswindow_commodity_grid_item_stockpile_target_t> stockpile_target;
	std::unique_ptr<logisticswindow_commodity_grid_item_daily_stockpile_change_t> daily_stockpile_change;
	std::unique_ptr<template_label> value_divider;
	std::unique_ptr<logisticswindow_commodity_grid_item_days_left_t> days_left;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "value") {
			return (void*)(&value);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_logisticswindow_commodity_grid_item(sys::state& state);
struct logisticswindow_commodity_spacer_t : public ui::non_owning_container_base {
// BEGIN commodity_spacer::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_logisticswindow_commodity_spacer(sys::state& state);
void logisticswindow_main_military_table_t::add_section_header(int32_t section_type) {
	values.emplace_back(section_header_option{section_type});
}
void logisticswindow_main_military_table_t::add_neutral_spacer() {
	values.emplace_back(neutral_spacer_option{});
}
void logisticswindow_main_military_table_t::add_bottom_spacer() {
	values.emplace_back(bottom_spacer_option{});
}
void logisticswindow_main_military_table_t::add_consumption_row(std::string name, float fufilled, float required, float satisfaction) {
	values.emplace_back(consumption_row_option{name, fufilled, required, satisfaction});
}
void  logisticswindow_main_military_table_t::on_create(sys::state& state, layout_window_element* parent) {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::military_table::on_create
// END
}
void  logisticswindow_main_military_table_t::update(sys::state& state, layout_window_element* parent) {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::military_table::update
	values.clear();
	add_section_header(logistic_categories::land_supply);
	if(logistic_categories::expanded[logistic_categories::land_supply]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = military::nation_get_last_fufilled_goods_need<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = military::nation_get_last_required_goods_need<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
	add_neutral_spacer();
	add_section_header(logistic_categories::land_reinforcement);
	if(logistic_categories::expanded[logistic_categories::land_reinforcement]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = military::nation_get_last_fufilled_goods_need<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = military::nation_get_last_required_goods_need<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
	add_neutral_spacer();
	add_section_header(logistic_categories::naval_supply);
	if(logistic_categories::expanded[logistic_categories::naval_supply]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = military::nation_get_last_fufilled_goods_need<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = military::nation_get_last_required_goods_need<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
	add_neutral_spacer();
	add_section_header(logistic_categories::naval_reinforcement);
	if(logistic_categories::expanded[logistic_categories::naval_reinforcement]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = military::nation_get_last_fufilled_goods_need<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = military::nation_get_last_required_goods_need<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (logisticswindow_main_t*)(parent);
	if(table_source->consumption_table_item_name_sort_direction != 0) work_to_do = true;
	if(table_source->consumption_table_item_amount_sort_direction != 0) work_to_do = true;
	if(table_source->consumption_table_fufillment_percent_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<consumption_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<consumption_row_option>(values[i])) ++i;
				if(table_source->consumption_table_item_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<consumption_row_option>(raw_a);
						auto const& b = std::get<consumption_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::military_table::consumption_table::sort::item_name
						result = int8_t(std::clamp(a.name.compare(b.name), -1, 1));
// END
						return -result == table_source->consumption_table_item_name_sort_direction;
					});
				}
				if(table_source->consumption_table_item_amount_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<consumption_row_option>(raw_a);
						auto const& b = std::get<consumption_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::military_table::consumption_table::sort::item_amount
						if(a.fufilled < b.fufilled) result = -1;
						if(a.fufilled > b.fufilled) result = 1;
// END
						return -result == table_source->consumption_table_item_amount_sort_direction;
					});
				}
				if(table_source->consumption_table_fufillment_percent_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<consumption_row_option>(raw_a);
						auto const& b = std::get<consumption_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::military_table::consumption_table::sort::fufillment_percent
						float a_pct = (a.required == 0 ? 1.0f : a.fufilled / a.required);
						float b_pct = (b.required == 0 ? 1.0f : b.fufilled / b.required);
						if(a_pct < b_pct) result = -1;
						if(a_pct > b_pct) result = 1;
// END
						return -result == table_source->consumption_table_fufillment_percent_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  logisticswindow_main_military_table_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<section_header_option>(values[index])) {
		if(section_header_pool.empty()) section_header_pool.emplace_back(make_logisticswindow_section_header(state));
		if(destination) {
			if(section_header_pool.size() <= size_t(section_header_pool_used)) section_header_pool.emplace_back(make_logisticswindow_section_header(state));
			section_header_pool[section_header_pool_used]->base_data.position.x = int16_t(x);
			section_header_pool[section_header_pool_used]->base_data.position.y = int16_t(y);
			section_header_pool[section_header_pool_used]->parent = destination;
			destination->children.push_back(section_header_pool[section_header_pool_used].get());
			((logisticswindow_section_header_t*)(section_header_pool[section_header_pool_used].get()))->section_type = std::get<section_header_option>(values[index]).section_type;
			section_header_pool[section_header_pool_used]->impl_on_update(state);
			section_header_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ section_header_pool[0]->base_data.size.x, section_header_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<neutral_spacer_option>(values[index])) {
		if(neutral_spacer_pool.empty()) neutral_spacer_pool.emplace_back(make_logisticswindow_neutral_spacer(state));
		if(destination) {
			if(neutral_spacer_pool.size() <= size_t(neutral_spacer_pool_used)) neutral_spacer_pool.emplace_back(make_logisticswindow_neutral_spacer(state));
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.x = int16_t(x);
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.y = int16_t(y);
			neutral_spacer_pool[neutral_spacer_pool_used]->parent = destination;
			destination->children.push_back(neutral_spacer_pool[neutral_spacer_pool_used].get());
			neutral_spacer_pool[neutral_spacer_pool_used]->impl_on_update(state);
			neutral_spacer_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ neutral_spacer_pool[0]->base_data.size.x, neutral_spacer_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<bottom_spacer_option>(values[index])) {
		if(bottom_spacer_pool.empty()) bottom_spacer_pool.emplace_back(make_logisticswindow_bottom_spacer(state));
		if(destination) {
			if(bottom_spacer_pool.size() <= size_t(bottom_spacer_pool_used)) bottom_spacer_pool.emplace_back(make_logisticswindow_bottom_spacer(state));
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.x = int16_t(x);
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.y = int16_t(y);
			bottom_spacer_pool[bottom_spacer_pool_used]->parent = destination;
			destination->children.push_back(bottom_spacer_pool[bottom_spacer_pool_used].get());
			bottom_spacer_pool[bottom_spacer_pool_used]->impl_on_update(state);
			bottom_spacer_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ bottom_spacer_pool[0]->base_data.size.x, bottom_spacer_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<consumption_row_option>(values[index])) {
		if(consumption_header_pool.empty()) consumption_header_pool.emplace_back(make_logisticswindow_consumption_header(state));
		if(consumption_row_pool.empty()) consumption_row_pool.emplace_back(make_logisticswindow_consumption_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<consumption_row_option>(values[index - 1]))) {
			if(destination) {
				if(consumption_header_pool.size() <= size_t(consumption_header_pool_used)) consumption_header_pool.emplace_back(make_logisticswindow_consumption_header(state));
				if(consumption_row_pool.size() <= size_t(consumption_row_pool_used)) consumption_row_pool.emplace_back(make_logisticswindow_consumption_row(state));
				consumption_header_pool[consumption_header_pool_used]->base_data.position.x = int16_t(x);
				consumption_header_pool[consumption_header_pool_used]->base_data.position.y = int16_t(y);
				if(!consumption_header_pool[consumption_header_pool_used]->parent) {
					consumption_header_pool[consumption_header_pool_used]->parent = destination;
					consumption_header_pool[consumption_header_pool_used]->impl_on_update(state);
					consumption_header_pool[consumption_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(consumption_header_pool[consumption_header_pool_used].get());
			((logisticswindow_consumption_header_t*)(consumption_header_pool[consumption_header_pool_used].get()))->set_alternate(alternate);
				consumption_row_pool[consumption_row_pool_used]->base_data.position.x = int16_t(x);
				consumption_row_pool[consumption_row_pool_used]->base_data.position.y = int16_t(y +  consumption_header_pool[0]->base_data.size.y + 0);
				consumption_row_pool[consumption_row_pool_used]->parent = destination;
				destination->children.push_back(consumption_row_pool[consumption_row_pool_used].get());
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->name = std::get<consumption_row_option>(values[index]).name;
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->fufilled = std::get<consumption_row_option>(values[index]).fufilled;
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->required = std::get<consumption_row_option>(values[index]).required;
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->satisfaction = std::get<consumption_row_option>(values[index]).satisfaction;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->set_alternate(!alternate);
				consumption_row_pool[consumption_row_pool_used]->impl_on_update(state);
				consumption_header_pool_used++;
				consumption_row_pool_used++;
			}
	 	 	bool stick_to_next = false;
			return measure_result{std::max(consumption_header_pool[0]->base_data.size.x, consumption_row_pool[0]->base_data.size.x), consumption_header_pool[0]->base_data.size.y + consumption_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
		}
		if(destination) {
			if(consumption_row_pool.size() <= size_t(consumption_row_pool_used)) consumption_row_pool.emplace_back(make_logisticswindow_consumption_row(state));
			consumption_row_pool[consumption_row_pool_used]->base_data.position.x = int16_t(x);
			consumption_row_pool[consumption_row_pool_used]->base_data.position.y = int16_t(y);
			consumption_row_pool[consumption_row_pool_used]->parent = destination;
			destination->children.push_back(consumption_row_pool[consumption_row_pool_used].get());
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->name = std::get<consumption_row_option>(values[index]).name;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->fufilled = std::get<consumption_row_option>(values[index]).fufilled;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->required = std::get<consumption_row_option>(values[index]).required;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->satisfaction = std::get<consumption_row_option>(values[index]).satisfaction;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->set_alternate(alternate);
			consumption_row_pool[consumption_row_pool_used]->impl_on_update(state);
			consumption_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = false;
		return measure_result{ consumption_row_pool[0]->base_data.size.x, consumption_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  logisticswindow_main_military_table_t::reset_pools() {
	section_header_pool_used = 0;
	neutral_spacer_pool_used = 0;
	bottom_spacer_pool_used = 0;
	consumption_header_pool_used = 0;
	consumption_row_pool_used = 0;
}
void logisticswindow_main_consumption_table_t::add_section_header(int32_t section_type) {
	values.emplace_back(section_header_option{section_type});
}
void logisticswindow_main_consumption_table_t::add_neutral_spacer() {
	values.emplace_back(neutral_spacer_option{});
}
void logisticswindow_main_consumption_table_t::add_bottom_spacer() {
	values.emplace_back(bottom_spacer_option{});
}
void logisticswindow_main_consumption_table_t::add_consumption_row(std::string name, float fufilled, float required, float satisfaction) {
	values.emplace_back(consumption_row_option{name, fufilled, required, satisfaction});
}
void  logisticswindow_main_consumption_table_t::on_create(sys::state& state, layout_window_element* parent) {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::consumption_table::on_create
// END
}
void  logisticswindow_main_consumption_table_t::update(sys::state& state, layout_window_element* parent) {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::consumption_table::update
	values.clear();
	add_section_header(logistic_categories::army_construction);
	if(logistic_categories::expanded[logistic_categories::army_construction]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = economy::nation_get_last_fufilled_construction_need<dcon::province_land_construction_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = economy::nation_get_last_required_construction_need<dcon::province_land_construction_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
	add_neutral_spacer();
	add_section_header(logistic_categories::naval_construction);
	if(logistic_categories::expanded[logistic_categories::naval_construction]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = economy::nation_get_last_fufilled_construction_need<dcon::province_naval_construction_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = economy::nation_get_last_required_construction_need<dcon::province_naval_construction_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
	add_neutral_spacer();
	add_section_header(logistic_categories::factory_construction);
	if(logistic_categories::expanded[logistic_categories::factory_construction]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = economy::nation_get_last_fufilled_construction_need<dcon::factory_construction_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = economy::nation_get_last_required_construction_need<dcon::factory_construction_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
	add_neutral_spacer();
	add_section_header(logistic_categories::building_construction);
	if(logistic_categories::expanded[logistic_categories::building_construction]) {
		add_bottom_spacer();
		tagged_vector<float, dcon::commodity_id> fufilled_amounts = economy::nation_get_last_fufilled_construction_need<dcon::province_building_construction_id>(state, state.local_player_nation);
		tagged_vector<float, dcon::commodity_id> required_amounts = economy::nation_get_last_required_construction_need<dcon::province_building_construction_id>(state, state.local_player_nation);
		economy::for_each_commodity_no_money(state, [&](dcon::commodity_id com_id) {
			float required = required_amounts[com_id];
			float fufilled = fufilled_amounts[com_id];
			if(required > 0.0f) {
				float sat = (required == 0.0f ? 1.0f : fufilled / required);
				add_consumption_row(text::get_commodity_text_icon(state, com_id) + text::produce_simple_string(state, state.world.commodity_get_name(com_id)), fufilled, required, sat);
			}
		});
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (logisticswindow_main_t*)(parent);
	if(table_source->consumption_table_item_name_sort_direction != 0) work_to_do = true;
	if(table_source->consumption_table_item_amount_sort_direction != 0) work_to_do = true;
	if(table_source->consumption_table_fufillment_percent_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<consumption_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<consumption_row_option>(values[i])) ++i;
				if(table_source->consumption_table_item_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<consumption_row_option>(raw_a);
						auto const& b = std::get<consumption_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::consumption_table::consumption_table::sort::item_name
// END
						return -result == table_source->consumption_table_item_name_sort_direction;
					});
				}
				if(table_source->consumption_table_item_amount_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<consumption_row_option>(raw_a);
						auto const& b = std::get<consumption_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::consumption_table::consumption_table::sort::item_amount
// END
						return -result == table_source->consumption_table_item_amount_sort_direction;
					});
				}
				if(table_source->consumption_table_fufillment_percent_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<consumption_row_option>(raw_a);
						auto const& b = std::get<consumption_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::consumption_table::consumption_table::sort::fufillment_percent
// END
						return -result == table_source->consumption_table_fufillment_percent_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  logisticswindow_main_consumption_table_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<section_header_option>(values[index])) {
		if(section_header_pool.empty()) section_header_pool.emplace_back(make_logisticswindow_section_header(state));
		if(destination) {
			if(section_header_pool.size() <= size_t(section_header_pool_used)) section_header_pool.emplace_back(make_logisticswindow_section_header(state));
			section_header_pool[section_header_pool_used]->base_data.position.x = int16_t(x);
			section_header_pool[section_header_pool_used]->base_data.position.y = int16_t(y);
			section_header_pool[section_header_pool_used]->parent = destination;
			destination->children.push_back(section_header_pool[section_header_pool_used].get());
			((logisticswindow_section_header_t*)(section_header_pool[section_header_pool_used].get()))->section_type = std::get<section_header_option>(values[index]).section_type;
			section_header_pool[section_header_pool_used]->impl_on_update(state);
			section_header_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ section_header_pool[0]->base_data.size.x, section_header_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<neutral_spacer_option>(values[index])) {
		if(neutral_spacer_pool.empty()) neutral_spacer_pool.emplace_back(make_logisticswindow_neutral_spacer(state));
		if(destination) {
			if(neutral_spacer_pool.size() <= size_t(neutral_spacer_pool_used)) neutral_spacer_pool.emplace_back(make_logisticswindow_neutral_spacer(state));
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.x = int16_t(x);
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.y = int16_t(y);
			neutral_spacer_pool[neutral_spacer_pool_used]->parent = destination;
			destination->children.push_back(neutral_spacer_pool[neutral_spacer_pool_used].get());
			neutral_spacer_pool[neutral_spacer_pool_used]->impl_on_update(state);
			neutral_spacer_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ neutral_spacer_pool[0]->base_data.size.x, neutral_spacer_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<bottom_spacer_option>(values[index])) {
		if(bottom_spacer_pool.empty()) bottom_spacer_pool.emplace_back(make_logisticswindow_bottom_spacer(state));
		if(destination) {
			if(bottom_spacer_pool.size() <= size_t(bottom_spacer_pool_used)) bottom_spacer_pool.emplace_back(make_logisticswindow_bottom_spacer(state));
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.x = int16_t(x);
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.y = int16_t(y);
			bottom_spacer_pool[bottom_spacer_pool_used]->parent = destination;
			destination->children.push_back(bottom_spacer_pool[bottom_spacer_pool_used].get());
			bottom_spacer_pool[bottom_spacer_pool_used]->impl_on_update(state);
			bottom_spacer_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ bottom_spacer_pool[0]->base_data.size.x, bottom_spacer_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<consumption_row_option>(values[index])) {
		if(consumption_header_pool.empty()) consumption_header_pool.emplace_back(make_logisticswindow_consumption_header(state));
		if(consumption_row_pool.empty()) consumption_row_pool.emplace_back(make_logisticswindow_consumption_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<consumption_row_option>(values[index - 1]))) {
			if(destination) {
				if(consumption_header_pool.size() <= size_t(consumption_header_pool_used)) consumption_header_pool.emplace_back(make_logisticswindow_consumption_header(state));
				if(consumption_row_pool.size() <= size_t(consumption_row_pool_used)) consumption_row_pool.emplace_back(make_logisticswindow_consumption_row(state));
				consumption_header_pool[consumption_header_pool_used]->base_data.position.x = int16_t(x);
				consumption_header_pool[consumption_header_pool_used]->base_data.position.y = int16_t(y);
				if(!consumption_header_pool[consumption_header_pool_used]->parent) {
					consumption_header_pool[consumption_header_pool_used]->parent = destination;
					consumption_header_pool[consumption_header_pool_used]->impl_on_update(state);
					consumption_header_pool[consumption_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(consumption_header_pool[consumption_header_pool_used].get());
			((logisticswindow_consumption_header_t*)(consumption_header_pool[consumption_header_pool_used].get()))->set_alternate(alternate);
				consumption_row_pool[consumption_row_pool_used]->base_data.position.x = int16_t(x);
				consumption_row_pool[consumption_row_pool_used]->base_data.position.y = int16_t(y +  consumption_header_pool[0]->base_data.size.y + 0);
				consumption_row_pool[consumption_row_pool_used]->parent = destination;
				destination->children.push_back(consumption_row_pool[consumption_row_pool_used].get());
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->name = std::get<consumption_row_option>(values[index]).name;
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->fufilled = std::get<consumption_row_option>(values[index]).fufilled;
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->required = std::get<consumption_row_option>(values[index]).required;
				((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->satisfaction = std::get<consumption_row_option>(values[index]).satisfaction;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->set_alternate(!alternate);
				consumption_row_pool[consumption_row_pool_used]->impl_on_update(state);
				consumption_header_pool_used++;
				consumption_row_pool_used++;
			}
	 	 	bool stick_to_next = false;
			return measure_result{std::max(consumption_header_pool[0]->base_data.size.x, consumption_row_pool[0]->base_data.size.x), consumption_header_pool[0]->base_data.size.y + consumption_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
		}
		if(destination) {
			if(consumption_row_pool.size() <= size_t(consumption_row_pool_used)) consumption_row_pool.emplace_back(make_logisticswindow_consumption_row(state));
			consumption_row_pool[consumption_row_pool_used]->base_data.position.x = int16_t(x);
			consumption_row_pool[consumption_row_pool_used]->base_data.position.y = int16_t(y);
			consumption_row_pool[consumption_row_pool_used]->parent = destination;
			destination->children.push_back(consumption_row_pool[consumption_row_pool_used].get());
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->name = std::get<consumption_row_option>(values[index]).name;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->fufilled = std::get<consumption_row_option>(values[index]).fufilled;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->required = std::get<consumption_row_option>(values[index]).required;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->satisfaction = std::get<consumption_row_option>(values[index]).satisfaction;
			((logisticswindow_consumption_row_t*)(consumption_row_pool[consumption_row_pool_used].get()))->set_alternate(alternate);
			consumption_row_pool[consumption_row_pool_used]->impl_on_update(state);
			consumption_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = false;
		return measure_result{ consumption_row_pool[0]->base_data.size.x, consumption_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  logisticswindow_main_consumption_table_t::reset_pools() {
	section_header_pool_used = 0;
	neutral_spacer_pool_used = 0;
	bottom_spacer_pool_used = 0;
	consumption_header_pool_used = 0;
	consumption_row_pool_used = 0;
}
void logisticswindow_main_commodity_grid_g_t::add_commodity_grid_item(dcon::commodity_id value) {
	values.emplace_back(commodity_grid_item_option{value});
}
void logisticswindow_main_commodity_grid_g_t::add_commodity_spacer() {
	values.emplace_back(commodity_spacer_option{});
}
void  logisticswindow_main_commodity_grid_g_t::on_create(sys::state& state, layout_window_element* parent) {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::commodity_grid_g::on_create
// END
}
void  logisticswindow_main_commodity_grid_g_t::update(sys::state& state, layout_window_element* parent) {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::commodity_grid_g::update
	values.clear();
	if(main.show_only_military_goods) {
		// unit supply and build commodities encompass all good used in unit supply or build costs
		state.world.for_each_unit_supply_and_build_commodity([&](dcon::unit_supply_and_build_commodity_id com_id) {
			auto base_com_id = state.world.unit_supply_and_build_commodity_get_base_commodity(com_id);
			add_commodity_grid_item(base_com_id);
		});
	}
	else {
		state.world.for_each_commodity([&](dcon::commodity_id com_id) {
			add_commodity_grid_item(com_id);
		});
	}
// END
}
measure_result  logisticswindow_main_commodity_grid_g_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<commodity_grid_item_option>(values[index])) {
		if(commodity_grid_item_pool.empty()) commodity_grid_item_pool.emplace_back(make_logisticswindow_commodity_grid_item(state));
		if(destination) {
			if(commodity_grid_item_pool.size() <= size_t(commodity_grid_item_pool_used)) commodity_grid_item_pool.emplace_back(make_logisticswindow_commodity_grid_item(state));
			commodity_grid_item_pool[commodity_grid_item_pool_used]->base_data.position.x = int16_t(x);
			commodity_grid_item_pool[commodity_grid_item_pool_used]->base_data.position.y = int16_t(y);
			commodity_grid_item_pool[commodity_grid_item_pool_used]->parent = destination;
			destination->children.push_back(commodity_grid_item_pool[commodity_grid_item_pool_used].get());
			((logisticswindow_commodity_grid_item_t*)(commodity_grid_item_pool[commodity_grid_item_pool_used].get()))->value = std::get<commodity_grid_item_option>(values[index]).value;
			commodity_grid_item_pool[commodity_grid_item_pool_used]->impl_on_update(state);
			commodity_grid_item_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ commodity_grid_item_pool[0]->base_data.size.x, commodity_grid_item_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<commodity_spacer_option>(values[index])) {
		if(commodity_spacer_pool.empty()) commodity_spacer_pool.emplace_back(make_logisticswindow_commodity_spacer(state));
		if(destination) {
			if(commodity_spacer_pool.size() <= size_t(commodity_spacer_pool_used)) commodity_spacer_pool.emplace_back(make_logisticswindow_commodity_spacer(state));
			commodity_spacer_pool[commodity_spacer_pool_used]->base_data.position.x = int16_t(x);
			commodity_spacer_pool[commodity_spacer_pool_used]->base_data.position.y = int16_t(y);
			commodity_spacer_pool[commodity_spacer_pool_used]->parent = destination;
			destination->children.push_back(commodity_spacer_pool[commodity_spacer_pool_used].get());
			commodity_spacer_pool[commodity_spacer_pool_used]->impl_on_update(state);
			commodity_spacer_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ commodity_spacer_pool[0]->base_data.size.x, commodity_spacer_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::end_page};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  logisticswindow_main_commodity_grid_g_t::reset_pools() {
	commodity_grid_item_pool_used = 0;
	commodity_spacer_pool_used = 0;
}
ui::message_result logisticswindow_main_selected_commodity_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result logisticswindow_main_selected_commodity_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void logisticswindow_main_selected_commodity_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::selected_commodity_icon::tooltip
// END
}
void logisticswindow_main_selected_commodity_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(background_gid) {
		auto& gfx_def = state.ui_defs.gfx[background_gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), frame, gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else {
				ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			}
		}
	}
}
void logisticswindow_main_selected_commodity_icon_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::selected_commodity_icon::update
	if(!main.selected_stockpile_commodity) {
		set_visible(state, false);
	} else {
		set_visible(state, true);
		frame = state.world.commodity_get_icon(main.selected_stockpile_commodity);
	}
// END
}
void logisticswindow_main_selected_commodity_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN main::selected_commodity_icon::create
// END
}
void logisticswindow_main_stockpile_target_input_t::on_edit_command(sys::state& state, ui::edit_command command, sys::key_modifiers mods)  noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::stockpile_target_input::edit_command
	// if user presses enter, then press the confirm button. This can't be implemented as a normal shortcut since all on-key events are dispatched to the edit box when in focus instead of other shortcuts
	if(command == ui::edit_command::new_line) {
		main.confirm_target_button->button_action(state);
		return;
	}
// END
	ui::edit_box_element_base::on_edit_command(state, command, mods);
}
void logisticswindow_main_stockpile_target_input_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::stockpile_target_input::update
// END
}
void logisticswindow_main_stockpile_target_input_t::on_create(sys::state& state) noexcept {
// BEGIN main::stockpile_target_input::create
// END
}
void logisticswindow_main_confirm_target_button_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::confirm_target_button::update
// END
}
bool logisticswindow_main_confirm_target_button_t::button_action(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::confirm_target_button::lbutton_action
	if(!main.selected_stockpile_commodity) {
		return false;
	}
	auto raw_input = main.stockpile_target_input->get_text(state);
	auto utf8_str = simple_fs::utf16_to_utf8(raw_input);
	auto parsed_res = parsers::try_parse_float(utf8_str);
	if(parsed_res && command::can_change_stockpile_settings(state, state.local_player_nation, main.selected_stockpile_commodity, parsed_res.value(), false)) {
		command::change_stockpile_settings(state, state.local_player_nation, main.selected_stockpile_commodity, parsed_res.value(), false);
		main.stockpile_target_error->set_text(state, ""); // Clear any potential error if sucess
	} else {
		main.stockpile_target_error->set_text(state, text::produce_simple_string(state, "mlitary_stockpile_target_error"));
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void logisticswindow_main_stockpile_target_error_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void logisticswindow_main_stockpile_target_error_t::on_reset_text(sys::state& state) noexcept {
}
void logisticswindow_main_stockpile_target_error_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void logisticswindow_main_stockpile_target_error_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::stockpile_target_error::update
// END
}
void logisticswindow_main_stockpile_target_error_t::on_create(sys::state& state) noexcept {
// BEGIN main::stockpile_target_error::create
// END
}
void logisticswindow_main_only_military_goods_toggle_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::only_military_goods_toggle::update
	set_active(state, main.show_only_military_goods);
// END
}
bool logisticswindow_main_only_military_goods_toggle_t::button_action(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::only_military_goods_toggle::lbutton_action
	main.show_only_military_goods = !main.show_only_military_goods;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void logisticswindow_main_prio_armies_in_battle_supply_toggle_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::prio_armies_in_battle_supply_toggle::update
	bool is_checked = state.world.nation_get_armies_have_supply_prio_in_battle(state.local_player_nation);
	set_active(state, is_checked);
// END
}
bool logisticswindow_main_prio_armies_in_battle_supply_toggle_t::button_action(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::prio_armies_in_battle_supply_toggle::lbutton_action
	bool cur_setting = state.world.nation_get_armies_have_supply_prio_in_battle(state.local_player_nation);
	command::set_supply_priority_for_armies_in_battle(state, !cur_setting);
// END
	return true;
}
void logisticswindow_main_prio_navies_in_battle_supply_toggle_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::prio_navies_in_battle_supply_toggle::update
	bool is_checked = state.world.nation_get_navies_have_supply_prio_in_battle(state.local_player_nation);
	set_active(state, is_checked);
// END
}
bool logisticswindow_main_prio_navies_in_battle_supply_toggle_t::button_action(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent)); 
// BEGIN main::prio_navies_in_battle_supply_toggle::lbutton_action
	bool cur_setting = state.world.nation_get_navies_have_supply_prio_in_battle(state.local_player_nation);
	command::set_supply_priority_for_navies_in_battle(state, !cur_setting);
// END
	return true;
}
ui::message_result logisticswindow_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	military_table.update(state, this);
	consumption_table.update(state, this);
	commodity_grid_g.update(state, this);
	remake_layout(state, true);
}
void logisticswindow_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				temp.ptr = nullptr;
				if(cname == "military_consumption_title") {
					temp.ptr = military_consumption_title.get();
				} else
				if(cname == "selected_commodity_icon") {
					temp.ptr = selected_commodity_icon.get();
				} else
				if(cname == "stockpile_target_input") {
					temp.ptr = stockpile_target_input.get();
				} else
				if(cname == "confirm_target_button") {
					temp.ptr = confirm_target_button.get();
				} else
				if(cname == "stockpile_target_error") {
					temp.ptr = stockpile_target_error.get();
				} else
				if(cname == "stockpile_target_editbox_label") {
					temp.ptr = stockpile_target_editbox_label.get();
				} else
				if(cname == "info_stockpile_target_view") {
					temp.ptr = info_stockpile_target_view.get();
				} else
				if(cname == "info_stockpile_view") {
					temp.ptr = info_stockpile_view.get();
				} else
				if(cname == "only_military_goods_toggle") {
					temp.ptr = only_military_goods_toggle.get();
				} else
				if(cname == "prio_armies_in_battle_supply_toggle") {
					temp.ptr = prio_armies_in_battle_supply_toggle.get();
				} else
				if(cname == "prio_navies_in_battle_supply_toggle") {
					temp.ptr = prio_navies_in_battle_supply_toggle.get();
				} else
				if(cname == "construction_consumption_title") {
					temp.ptr = construction_consumption_title.get();
				} else
				if(cname == "government_stockpiles_title") {
					temp.ptr = government_stockpiles_title.get();
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
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_logisticswindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_logisticswindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_logisticswindow_neutral_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_logisticswindow_bottom_spacer(state);
				}
				if(cname == "consumption_row") {
					temp.ptr = make_logisticswindow_consumption_row(state);
				}
				if(cname == "consumption_header") {
					temp.ptr = make_logisticswindow_consumption_header(state);
				}
				if(cname == "commodity_grid_item") {
					temp.ptr = make_logisticswindow_commodity_grid_item(state);
				}
				if(cname == "commodity_spacer") {
					temp.ptr = make_logisticswindow_commodity_spacer(state);
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
			case layout_item_types::generator2:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				if(cname == "military_table") {
					temp.generator = &military_table;
				}
				if(cname == "consumption_table") {
					temp.generator = &consumption_table;
				}
				if(cname == "commodity_grid_g") {
					temp.generator = &commodity_grid_g;
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
void logisticswindow_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::main"));
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
		if(child_data.name == "military_consumption_title") {
			military_consumption_title = std::make_unique<template_label>();
			military_consumption_title->parent = this;
			auto cptr = military_consumption_title.get();
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
		if(child_data.name == "selected_commodity_icon") {
			selected_commodity_icon = std::make_unique<logisticswindow_main_selected_commodity_icon_t>();
			selected_commodity_icon->parent = this;
			auto cptr = selected_commodity_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "stockpile_target_input") {
			stockpile_target_input = std::make_unique<logisticswindow_main_stockpile_target_input_t>();
			stockpile_target_input->parent = this;
			auto cptr = stockpile_target_input.get();
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
		if(child_data.name == "confirm_target_button") {
			confirm_target_button = std::make_unique<logisticswindow_main_confirm_target_button_t>();
			confirm_target_button->parent = this;
			auto cptr = confirm_target_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon_id = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "stockpile_target_error") {
			stockpile_target_error = std::make_unique<logisticswindow_main_stockpile_target_error_t>();
			stockpile_target_error->parent = this;
			auto cptr = stockpile_target_error.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "stockpile_target_editbox_label") {
			stockpile_target_editbox_label = std::make_unique<template_label>();
			stockpile_target_editbox_label->parent = this;
			auto cptr = stockpile_target_editbox_label.get();
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
		if(child_data.name == "info_stockpile_target_view") {
			info_stockpile_target_view = std::make_unique<template_icon_graphic>();
			info_stockpile_target_view->parent = this;
			auto cptr = info_stockpile_target_view.get();
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
		if(child_data.name == "info_stockpile_view") {
			info_stockpile_view = std::make_unique<template_icon_graphic>();
			info_stockpile_view->parent = this;
			auto cptr = info_stockpile_view.get();
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
		if(child_data.name == "only_military_goods_toggle") {
			only_military_goods_toggle = std::make_unique<logisticswindow_main_only_military_goods_toggle_t>();
			only_military_goods_toggle->parent = this;
			auto cptr = only_military_goods_toggle.get();
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
		if(child_data.name == "prio_armies_in_battle_supply_toggle") {
			prio_armies_in_battle_supply_toggle = std::make_unique<logisticswindow_main_prio_armies_in_battle_supply_toggle_t>();
			prio_armies_in_battle_supply_toggle->parent = this;
			auto cptr = prio_armies_in_battle_supply_toggle.get();
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
		if(child_data.name == "prio_navies_in_battle_supply_toggle") {
			prio_navies_in_battle_supply_toggle = std::make_unique<logisticswindow_main_prio_navies_in_battle_supply_toggle_t>();
			prio_navies_in_battle_supply_toggle->parent = this;
			auto cptr = prio_navies_in_battle_supply_toggle.get();
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
		if(child_data.name == "construction_consumption_title") {
			construction_consumption_title = std::make_unique<template_label>();
			construction_consumption_title->parent = this;
			auto cptr = construction_consumption_title.get();
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
		if(child_data.name == "government_stockpiles_title") {
			government_stockpiles_title = std::make_unique<template_label>();
			government_stockpiles_title->parent = this;
			auto cptr = government_stockpiles_title.get();
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
		if(child_data.name == ".tabconsumption_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			consumption_table_ascending_icon_key = main_section.read<std::string_view>();
			consumption_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_lead_space_column_start = running_w_total;
			col_section.read(consumption_table_lead_space_column_width);
			running_w_total += consumption_table_lead_space_column_width;
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			consumption_table_item_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_item_name_column_start = running_w_total;
			col_section.read(consumption_table_item_name_column_width);
			running_w_total += consumption_table_item_name_column_width;
			col_section.read(consumption_table_item_name_column_text_color);
			col_section.read(consumption_table_item_name_header_text_color);
			col_section.read(consumption_table_item_name_text_alignment);
			consumption_table_item_amount_header_text_key = col_section.read<std::string_view>();
			consumption_table_item_amount_header_tooltip_key = col_section.read<std::string_view>();
			consumption_table_item_amount_column_tooltip_key = col_section.read<std::string_view>();
			consumption_table_item_amount_column_start = running_w_total;
			col_section.read(consumption_table_item_amount_column_width);
			running_w_total += consumption_table_item_amount_column_width;
			col_section.read(consumption_table_item_amount_column_text_color);
			col_section.read(consumption_table_item_amount_header_text_color);
			col_section.read(consumption_table_item_amount_text_alignment);
			consumption_table_fufillment_percent_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_fufillment_percent_column_start = running_w_total;
			col_section.read(consumption_table_fufillment_percent_column_width);
			running_w_total += consumption_table_fufillment_percent_column_width;
			col_section.read(consumption_table_fufillment_percent_column_text_color);
			col_section.read(consumption_table_fufillment_percent_header_text_color);
			col_section.read(consumption_table_fufillment_percent_text_alignment);
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
	military_table.on_create(state, this);
	consumption_table.on_create(state, this);
	commodity_grid_g.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_main(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_main_t>();
	ptr->on_create(state);
	return ptr;
}
void logisticswindow_section_header_label_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::label::update
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: set_text(state, text::produce_simple_string(state, "land_reinforcement_consumption")); break;
	case logistic_categories::land_supply: set_text(state, text::produce_simple_string(state, "land_supply_consumption")); break;
	case logistic_categories::naval_reinforcement: set_text(state, text::produce_simple_string(state, "naval_reinforcement_consumption")); break;
	case logistic_categories::naval_supply: set_text(state, text::produce_simple_string(state, "naval_supply_consumption")); break;
	case logistic_categories::army_construction: set_text(state, text::produce_simple_string(state, "army_construction_consumption")); break;
	case logistic_categories::naval_construction: set_text(state, text::produce_simple_string(state, "navy_construction_consumption")); break;
	case logistic_categories::factory_construction: set_text(state, text::produce_simple_string(state, "factory_construction_consumption")); break;
	case logistic_categories::building_construction: set_text(state, text::produce_simple_string(state, "building_construction_consumption")); break;
	default: set_text(state, ""); break;
	}
// END
}
void logisticswindow_section_header_llbutton_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::llbutton::update
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: set_visible(state, true);  break;
	case logistic_categories::land_supply: set_visible(state, true);  break;
	case logistic_categories::naval_reinforcement: set_visible(state, true);  break;
	case logistic_categories::naval_supply: set_visible(state, true);  break;
	case logistic_categories::army_construction: set_visible(state, true);  break;
	case logistic_categories::naval_construction: set_visible(state, true);  break;
	case logistic_categories::factory_construction: set_visible(state, true);  break;
	case logistic_categories::building_construction: set_visible(state, true);  break;
	default: set_visible(state, false); break;
	}
// END
}
bool logisticswindow_section_header_llbutton_t::button_action(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::llbutton::lbutton_action
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: command::change_army_reinforcement_consumption_setting(state, logistics::army_reinforcement_setting_min(state, state.local_player_nation)); break;
	case logistic_categories::land_supply: command::change_army_supply_consumption_setting(state, logistics::army_supply_setting_min(state, state.local_player_nation)); break;
	case logistic_categories::naval_reinforcement: command::change_navy_reinforcement_consumption_setting(state, logistics::navy_reinforcement_setting_min(state, state.local_player_nation)); break;
	case logistic_categories::naval_supply: command::change_navy_supply_consumption_setting(state, logistics::navy_supply_setting_min(state, state.local_player_nation)); break;
	case logistic_categories::army_construction: command::change_army_construction_consumption_setting(state, logistics::army_construction_setting_min(state, state.local_player_nation)); break;
	case logistic_categories::naval_construction: command::change_navy_construction_consumption_setting(state, logistics::navy_construction_setting_min(state, state.local_player_nation)); break;
	case logistic_categories::factory_construction: command::change_factory_construction_consumption_setting(state, logistics::factory_construction_setting_min(state, state.local_player_nation)); break;
	case logistic_categories::building_construction: command::change_building_construction_consumption_setting(state, logistics::building_construction_setting_min(state, state.local_player_nation)); break;
	default:  break;
	}
// END
	return true;
}
void logisticswindow_section_header_lbutton_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::lbutton::update
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: set_visible(state, true);  break;
	case logistic_categories::land_supply: set_visible(state, true);  break;
	case logistic_categories::naval_reinforcement: set_visible(state, true);  break;
	case logistic_categories::naval_supply: set_visible(state, true);  break;
	case logistic_categories::army_construction: set_visible(state, true);  break;
	case logistic_categories::naval_construction: set_visible(state, true);  break;
	case logistic_categories::factory_construction: set_visible(state, true);  break;
	case logistic_categories::building_construction: set_visible(state, true);  break;
	default: set_visible(state, false); break;
	}
// END
}
bool logisticswindow_section_header_lbutton_t::button_action(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::lbutton::lbutton_action
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: command::change_army_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_reinforcement_consumption(state.local_player_nation) - 10), logistics::army_reinforcement_setting_min(state, state.local_player_nation), logistics::army_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::land_supply: command::change_army_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_supply_consumption(state.local_player_nation) - 10), logistics::army_supply_setting_min(state, state.local_player_nation), logistics::army_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_reinforcement: command::change_navy_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_reinforcement_consumption(state.local_player_nation) - 10), logistics::navy_reinforcement_setting_min(state, state.local_player_nation), logistics::navy_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_supply: command::change_navy_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_supply_consumption(state.local_player_nation) - 10), logistics::navy_supply_setting_min(state, state.local_player_nation), logistics::navy_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::army_construction: command::change_army_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_army_construction_consumption(state.local_player_nation) - 10), logistics::army_construction_setting_min(state, state.local_player_nation), logistics::army_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_construction: command::change_navy_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_navy_construction_consumption(state.local_player_nation) - 10), logistics::navy_construction_setting_min(state, state.local_player_nation), logistics::navy_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::factory_construction: command::change_factory_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_factory_construction_consumption(state.local_player_nation) - 10), logistics::factory_construction_setting_min(state, state.local_player_nation), logistics::factory_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::building_construction: command::change_building_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_building_construction_consumption(state.local_player_nation) - 10), logistics::building_construction_setting_min(state, state.local_player_nation), logistics::building_construction_setting_max(state, state.local_player_nation))); break;
	default:  break;
	}
// END
	return true;
}
bool logisticswindow_section_header_lbutton_t::button_shift_action(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::lbutton::lbutton_shift_action
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: command::change_army_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_reinforcement_consumption(state.local_player_nation) - 1), logistics::army_reinforcement_setting_min(state, state.local_player_nation), logistics::army_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::land_supply: command::change_army_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_supply_consumption(state.local_player_nation) - 1), logistics::army_supply_setting_min(state, state.local_player_nation), logistics::army_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_reinforcement: command::change_navy_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_reinforcement_consumption(state.local_player_nation) - 1), logistics::navy_reinforcement_setting_min(state, state.local_player_nation), logistics::navy_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_supply: command::change_navy_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_supply_consumption(state.local_player_nation) - 1), logistics::navy_supply_setting_min(state, state.local_player_nation), logistics::navy_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::army_construction: command::change_army_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_army_construction_consumption(state.local_player_nation) - 1), logistics::army_construction_setting_min(state, state.local_player_nation), logistics::army_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_construction: command::change_navy_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_navy_construction_consumption(state.local_player_nation) - 1), logistics::navy_construction_setting_min(state, state.local_player_nation), logistics::navy_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::factory_construction: command::change_factory_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_factory_construction_consumption(state.local_player_nation) - 1), logistics::factory_construction_setting_min(state, state.local_player_nation), logistics::factory_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::building_construction: command::change_building_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_building_construction_consumption(state.local_player_nation) - 1), logistics::building_construction_setting_min(state, state.local_player_nation), logistics::building_construction_setting_max(state, state.local_player_nation))); break;
	default:  break;
	}
// END
	return true;
}
void logisticswindow_section_header_rbutton_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::rbutton::update
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: set_visible(state, true);  break;
	case logistic_categories::land_supply: set_visible(state, true);  break;
	case logistic_categories::naval_reinforcement: set_visible(state, true);  break;
	case logistic_categories::naval_supply: set_visible(state, true);  break;
	case logistic_categories::army_construction: set_visible(state, true);  break;
	case logistic_categories::naval_construction: set_visible(state, true);  break;
	case logistic_categories::factory_construction: set_visible(state, true);  break;
	case logistic_categories::building_construction: set_visible(state, true);  break;
	default: set_visible(state, false); break;
	}
// END
}
bool logisticswindow_section_header_rbutton_t::button_action(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::rbutton::lbutton_action
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: command::change_army_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_reinforcement_consumption(state.local_player_nation) + 10), logistics::army_reinforcement_setting_min(state, state.local_player_nation), logistics::army_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::land_supply: command::change_army_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_supply_consumption(state.local_player_nation) + 10), logistics::army_supply_setting_min(state, state.local_player_nation), logistics::army_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_reinforcement: command::change_navy_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_reinforcement_consumption(state.local_player_nation) + 10), logistics::navy_reinforcement_setting_min(state, state.local_player_nation), logistics::navy_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_supply: command::change_navy_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_supply_consumption(state.local_player_nation) + 10), logistics::navy_supply_setting_min(state, state.local_player_nation), logistics::navy_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::army_construction: command::change_army_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_army_construction_consumption(state.local_player_nation) + 10), logistics::army_construction_setting_min(state, state.local_player_nation), logistics::army_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_construction: command::change_navy_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_navy_construction_consumption(state.local_player_nation) + 10), logistics::navy_construction_setting_min(state, state.local_player_nation), logistics::navy_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::factory_construction: command::change_factory_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_factory_construction_consumption(state.local_player_nation) + 10), logistics::factory_construction_setting_min(state, state.local_player_nation), logistics::factory_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::building_construction: command::change_building_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_building_construction_consumption(state.local_player_nation) + 10), logistics::building_construction_setting_min(state, state.local_player_nation), logistics::building_construction_setting_max(state, state.local_player_nation))); break;
	default:  break;
	}
// END
	return true;
}
bool logisticswindow_section_header_rbutton_t::button_shift_action(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::rbutton::lbutton_shift_action
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: command::change_army_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_reinforcement_consumption(state.local_player_nation) + 1), logistics::army_reinforcement_setting_min(state, state.local_player_nation), logistics::army_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::land_supply: command::change_army_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_land_supply_consumption(state.local_player_nation) + 1), logistics::army_supply_setting_min(state, state.local_player_nation), logistics::army_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_reinforcement: command::change_navy_reinforcement_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_reinforcement_consumption(state.local_player_nation) + 1), logistics::navy_reinforcement_setting_min(state, state.local_player_nation), logistics::navy_reinforcement_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_supply: command::change_navy_supply_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_naval_supply_consumption(state.local_player_nation) + 1), logistics::navy_supply_setting_min(state, state.local_player_nation), logistics::navy_supply_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::army_construction: command::change_army_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_army_construction_consumption(state.local_player_nation) + 1), logistics::army_construction_setting_min(state, state.local_player_nation), logistics::army_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::naval_construction: command::change_navy_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_navy_construction_consumption(state.local_player_nation) + 1), logistics::navy_construction_setting_min(state, state.local_player_nation), logistics::navy_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::factory_construction: command::change_factory_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_factory_construction_consumption(state.local_player_nation) + 1), logistics::factory_construction_setting_min(state, state.local_player_nation), logistics::factory_construction_setting_max(state, state.local_player_nation))); break;
	case logistic_categories::building_construction: command::change_building_construction_consumption_setting(state, std::clamp(int8_t(state.world.nation_get_building_construction_consumption(state.local_player_nation) + 1), logistics::building_construction_setting_min(state, state.local_player_nation), logistics::building_construction_setting_max(state, state.local_player_nation))); break;
	default:  break;
	}
// END
	return true;
}
void logisticswindow_section_header_rrbutton_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::rrbutton::update
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: set_visible(state, true);  break;
	case logistic_categories::land_supply: set_visible(state, true);  break;
	case logistic_categories::naval_reinforcement: set_visible(state, true);  break;
	case logistic_categories::naval_supply: set_visible(state, true);  break;
	case logistic_categories::army_construction: set_visible(state, true);  break;
	case logistic_categories::naval_construction: set_visible(state, true);  break;
	case logistic_categories::factory_construction: set_visible(state, true);  break;
	case logistic_categories::building_construction: set_visible(state, true);  break;
	default: set_visible(state, false); break;
	}
// END
}
bool logisticswindow_section_header_rrbutton_t::button_action(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::rrbutton::lbutton_action
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: command::change_army_reinforcement_consumption_setting(state, logistics::army_reinforcement_setting_max(state, state.local_player_nation)); break;
	case logistic_categories::land_supply: command::change_army_supply_consumption_setting(state, logistics::army_supply_setting_max(state, state.local_player_nation)); break;
	case logistic_categories::naval_reinforcement: command::change_navy_reinforcement_consumption_setting(state, logistics::navy_reinforcement_setting_max(state, state.local_player_nation)); break;
	case logistic_categories::naval_supply: command::change_navy_supply_consumption_setting(state, logistics::navy_supply_setting_max(state, state.local_player_nation)); break;
	case logistic_categories::army_construction: command::change_army_construction_consumption_setting(state, logistics::army_construction_setting_max(state, state.local_player_nation)); break;
	case logistic_categories::naval_construction: command::change_navy_construction_consumption_setting(state, logistics::navy_construction_setting_max(state, state.local_player_nation)); break;
	case logistic_categories::factory_construction: command::change_factory_construction_consumption_setting(state, logistics::factory_construction_setting_max(state, state.local_player_nation)); break;
	case logistic_categories::building_construction: command::change_building_construction_consumption_setting(state, logistics::building_construction_setting_max(state, state.local_player_nation)); break;
	default:  break;
	}
// END
	return true;
}
void logisticswindow_section_header_setting_amount_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::setting_amount::update
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: set_text(state, std::to_string(state.world.nation_get_land_reinforcement_consumption(state.local_player_nation)));  break;
	case logistic_categories::land_supply: set_text(state, std::to_string(state.world.nation_get_land_supply_consumption(state.local_player_nation)));  break;
	case logistic_categories::naval_reinforcement: set_text(state, std::to_string(state.world.nation_get_naval_reinforcement_consumption(state.local_player_nation)));  break;
	case logistic_categories::naval_supply: set_text(state, std::to_string(state.world.nation_get_naval_supply_consumption(state.local_player_nation)));  break;
	case logistic_categories::army_construction: set_text(state, std::to_string(state.world.nation_get_army_construction_consumption(state.local_player_nation)));  break;
	case logistic_categories::naval_construction: set_text(state, std::to_string(state.world.nation_get_navy_construction_consumption(state.local_player_nation)));  break;
	case logistic_categories::factory_construction: set_text(state, std::to_string(state.world.nation_get_factory_construction_consumption(state.local_player_nation)));  break;
	case logistic_categories::building_construction: set_text(state, std::to_string(state.world.nation_get_building_construction_consumption(state.local_player_nation)));  break;
	default: set_text(state, ""); break;
	}
// END
}
void logisticswindow_section_header_expand_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::expand_button::tooltip
	if(!logistic_categories::expanded[section_header.section_type])
		text::add_line(state, contents, "alice_budget_expand_tt");
	else
		text::add_line(state, contents, "alice_budget_contract_tt");
// END
}
void logisticswindow_section_header_expand_button_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::expand_button::update
	static auto open_icon = template_project::icon_by_name(state.ui_templates, "list_open.svg");
	static auto closed_icon = template_project::icon_by_name(state.ui_templates, "list_closed.svg");

	auto n = state.local_player_nation;
	uint32_t total_armies = uint32_t(state.world.nation_get_army_control(state.local_player_nation).end() - state.world.nation_get_army_control(state.local_player_nation).begin());
	uint32_t total_navies = uint32_t(state.world.nation_get_navy_control(state.local_player_nation).end() - state.world.nation_get_navy_control(state.local_player_nation).begin());

	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: disabled = (total_armies == 0); break;
	case logistic_categories::land_supply: disabled = (total_armies == 0); break;
	case logistic_categories::naval_reinforcement: disabled = (total_navies == 0); break;
	case logistic_categories::naval_supply: disabled = (total_navies == 0); break;
	case logistic_categories::army_construction: disabled = false; break;
	case logistic_categories::naval_construction: disabled = false; break;
	case logistic_categories::factory_construction: disabled = false; break;
	case logistic_categories::building_construction: disabled = false; break;
	default: disabled = false; break;
	}

	icon = (!disabled && logistic_categories::expanded[section_header.section_type]) ? open_icon : closed_icon;
// END
}
bool logisticswindow_section_header_expand_button_t::button_action(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::expand_button::lbutton_action
	logistic_categories::expanded[section_header.section_type] = !logistic_categories::expanded[section_header.section_type];
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void logisticswindow_section_header_satisfaction_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::satisfaction::tooltip
	std::string_view tooltip_key = "logistics_satisfaction_tooltip";

	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation) }); break;
	case logistic_categories::land_supply:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_reinforcement: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_supply: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation) }); break;
	case logistic_categories::army_construction:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ economy::nation_average_construction_satisfaction_by_type<dcon::province_land_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_construction:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ economy::nation_average_construction_satisfaction_by_type<dcon::province_naval_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::factory_construction: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ economy::nation_average_construction_satisfaction_by_type<dcon::factory_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::building_construction: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ economy::nation_average_construction_satisfaction_by_type<dcon::province_building_construction_id>(state, state.local_player_nation) }); break;
	default: set_text(state, ""); break;
	}

// END
}
void logisticswindow_section_header_satisfaction_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::satisfaction::update
	auto adjust_percent_value = [&](float value) {
		return text::format_percentage(value, 2);
	};
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement:  set_text(state, adjust_percent_value(military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation))); break;
	case logistic_categories::land_supply:  set_text(state, adjust_percent_value(military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_reinforcement:  set_text(state, adjust_percent_value(military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_supply: set_text(state, adjust_percent_value(military::nation_average_military_satisfaction_by_type<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation))); break;
	case logistic_categories::army_construction:  set_text(state, adjust_percent_value(economy::nation_average_construction_satisfaction_by_type<dcon::province_land_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_construction:  set_text(state, adjust_percent_value(economy::nation_average_construction_satisfaction_by_type<dcon::province_naval_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::factory_construction:set_text(state, adjust_percent_value(economy::nation_average_construction_satisfaction_by_type<dcon::factory_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::building_construction: set_text(state, adjust_percent_value(economy::nation_average_construction_satisfaction_by_type<dcon::province_building_construction_id>(state, state.local_player_nation))); break;
	default: set_text(state, ""); break;
	}
// END
}
void logisticswindow_section_header_min_setting_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::min_setting::tooltip
	auto value = 0;
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: value = logistics::army_reinforcement_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::land_supply: value = logistics::army_supply_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::naval_reinforcement: value = logistics::navy_reinforcement_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::naval_supply: value = logistics::navy_supply_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::army_construction: value = logistics::army_construction_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::naval_construction: value = logistics::navy_construction_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::factory_construction: value = logistics::factory_construction_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::building_construction: value = logistics::building_construction_setting_min(state, state.local_player_nation);  break;
	default:  break;
	}
	if(value == 0) {

	} else {
		switch(section_header.section_type) {
		case logistic_categories::land_reinforcement: break;
		case logistic_categories::land_supply: break;
		case logistic_categories::naval_reinforcement: break;
		case logistic_categories::naval_supply: break;
		case logistic_categories::army_construction: break;
		case logistic_categories::naval_construction:  break;
		case logistic_categories::factory_construction: break;
		case logistic_categories::building_construction: break;
		default:  break;
		}
	}
// END
}
void logisticswindow_section_header_min_setting_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::min_setting::update
	auto value = 0;
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: value = logistics::army_reinforcement_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::land_supply: value = logistics::army_supply_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::naval_reinforcement: value = logistics::navy_reinforcement_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::naval_supply: value = logistics::navy_supply_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::army_construction: value = logistics::army_construction_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::naval_construction: value = logistics::navy_construction_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::factory_construction: value = logistics::factory_construction_setting_min(state, state.local_player_nation);  break;
	case logistic_categories::building_construction: value = logistics::building_construction_setting_min(state, state.local_player_nation);  break;
	default:  break;
	}
	if(value == 0) {
		set_text(state, "");
	} else {
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::x, value);
		set_text(state, text::resolve_string_substitution(state, "alice_budget_min", m));
	}
// END
}
void logisticswindow_section_header_max_setting_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::max_setting::tooltip
	auto value = 100;
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: value = logistics::army_reinforcement_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::land_supply: value = logistics::army_supply_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::naval_reinforcement: value = logistics::navy_reinforcement_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::naval_supply: value = logistics::navy_supply_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::army_construction: value = logistics::army_construction_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::naval_construction: value = logistics::navy_construction_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::factory_construction: value = logistics::factory_construction_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::building_construction: value = logistics::building_construction_setting_max(state, state.local_player_nation);  break;
	default:  break;
	}
	if(value == 100) {

	} else {
		switch(section_header.section_type) {
		case logistic_categories::land_reinforcement: break;
		case logistic_categories::land_supply: break;
		case logistic_categories::naval_reinforcement: break;
		case logistic_categories::naval_supply: break;
		case logistic_categories::army_construction: break;
		case logistic_categories::naval_construction:  break;
		case logistic_categories::factory_construction: break;
		case logistic_categories::building_construction: break;
		default:  break;
		}
	}
// END
}
void logisticswindow_section_header_max_setting_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::max_setting::update
	auto value = 100;
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: value = logistics::army_reinforcement_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::land_supply: value = logistics::army_supply_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::naval_reinforcement: value = logistics::navy_reinforcement_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::naval_supply: value = logistics::navy_supply_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::army_construction: value = logistics::army_construction_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::naval_construction: value = logistics::navy_construction_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::factory_construction: value = logistics::factory_construction_setting_max(state, state.local_player_nation);  break;
	case logistic_categories::building_construction: value = logistics::building_construction_setting_max(state, state.local_player_nation);  break;
	default:  break;
	}
	if(value == 100) {
		set_text(state, "");
	} else {
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::x, value);
		set_text(state, text::resolve_string_substitution(state, "alice_budget_max", m));
	}
// END
}
void logisticswindow_section_header_info_section_header_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::info_section_header::tooltip
	std::string_view tooltip_key = [&]() {
		switch(section_header.section_type) {
		case logistic_categories::land_reinforcement: return "consumption_section_army_reinforcement_info_tooltip";
		case logistic_categories::land_supply: return "consumption_section_army_supply_info_tooltip";
		case logistic_categories::naval_reinforcement: return "consumption_section_navy_reinforcement_info_tooltip";
		case logistic_categories::naval_supply: return "consumption_section_navy_supply_info_tooltip";
		case logistic_categories::army_construction: return "consumption_section_army_construction_info_tooltip";
		case logistic_categories::naval_construction: return "consumption_section_navy_construction_info_tooltip";
		case logistic_categories::factory_construction: return "consumption_section_factory_construction_info_tooltip";
		case logistic_categories::building_construction: return "consumption_section_building_construction_info_tooltip";
		default: return "";
		}
	}();
	text::add_line(state, contents, tooltip_key);
// END
}
void logisticswindow_section_header_info_section_header_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::info_section_header::update
// END
}
void logisticswindow_section_header_supply_loss_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::supply_loss::tooltip
	std::string_view tooltip_key = "logistics_supply_loss_tooltip";

	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation) }); break;
	case logistic_categories::land_supply:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_reinforcement: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_supply: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation) }); break;
	case logistic_categories::army_construction:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_loss_by_type<dcon::province_land_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_construction:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_loss_by_type<dcon::province_naval_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::factory_construction: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_loss_by_type<dcon::factory_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::building_construction: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_loss_by_type<dcon::province_building_construction_id>(state, state.local_player_nation) }); break;
	default: set_text(state, ""); break;
	}
// END
}
void logisticswindow_section_header_supply_loss_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::supply_loss::update
	auto adjust_percent_value = [&](float value) {
		return text::format_percentage(value, 2);
	};
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement:  set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation))); break;
	case logistic_categories::land_supply:  set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_reinforcement:  set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_supply: set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_loss_by_type<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation))); break;
	case logistic_categories::army_construction:  set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_loss_by_type<dcon::province_land_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_construction:  set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_loss_by_type<dcon::province_naval_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::factory_construction: set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_loss_by_type<dcon::factory_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::building_construction: set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_loss_by_type<dcon::province_building_construction_id>(state, state.local_player_nation))); break;
	default: set_text(state, ""); break;
	}
// END
}
void logisticswindow_section_header_supply_throughput_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::supply_throughput::tooltip
	std::string_view tooltip_key = "logistics_supply_throughput_tooltip";

	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation) }); break;
	case logistic_categories::land_supply:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_reinforcement: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_supply: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation) }); break;
	case logistic_categories::army_construction:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::province_land_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::naval_construction:  text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::province_naval_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::factory_construction: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::factory_construction_id>(state, state.local_player_nation) }); break;
	case logistic_categories::building_construction: text::add_line(state, contents, tooltip_key, text::variable_type::val, text::fp_percentage_two_places{ logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::province_building_construction_id>(state, state.local_player_nation) }); break;
	default: set_text(state, ""); break;
	}
// END
}
void logisticswindow_section_header_supply_throughput_t::on_update(sys::state& state) noexcept {
	logisticswindow_section_header_t& section_header = *((logisticswindow_section_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::supply_throughput::update
	auto adjust_percent_value = [&](float value) {
		return text::format_percentage(value, 2);
	};
	switch(section_header.section_type) {
	case logistic_categories::land_reinforcement:  set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::reinforcement, dcon::army_id>(state, state.local_player_nation))); break;
	case logistic_categories::land_supply:  set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::supply, dcon::army_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_reinforcement:  set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::reinforcement, dcon::navy_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_supply: set_text(state, adjust_percent_value(logistics::nation_get_avg_military_supply_throughput_by_type<military::unit_consumption_type::supply, dcon::navy_id>(state, state.local_player_nation))); break;
	case logistic_categories::army_construction:  set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::province_land_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::naval_construction:  set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::province_naval_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::factory_construction: set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::factory_construction_id>(state, state.local_player_nation))); break;
	case logistic_categories::building_construction: set_text(state, adjust_percent_value(logistics::nation_get_avg_construction_supply_throughput_by_type<dcon::province_building_construction_id>(state, state.local_player_nation))); break;
	default: set_text(state, ""); break;
	}
// END
}
ui::message_result logisticswindow_section_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_section_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_section_header_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN section_header::update
// END
	remake_layout(state, true);
}
void logisticswindow_section_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				temp.ptr = nullptr;
				if(cname == "label") {
					temp.ptr = label.get();
				} else
				if(cname == "llbutton") {
					temp.ptr = llbutton.get();
				} else
				if(cname == "lbutton") {
					temp.ptr = lbutton.get();
				} else
				if(cname == "rbutton") {
					temp.ptr = rbutton.get();
				} else
				if(cname == "rrbutton") {
					temp.ptr = rrbutton.get();
				} else
				if(cname == "setting_amount") {
					temp.ptr = setting_amount.get();
				} else
				if(cname == "expand_button") {
					temp.ptr = expand_button.get();
				} else
				if(cname == "satisfaction") {
					temp.ptr = satisfaction.get();
				} else
				if(cname == "min_setting") {
					temp.ptr = min_setting.get();
				} else
				if(cname == "max_setting") {
					temp.ptr = max_setting.get();
				} else
				if(cname == "info_section_header") {
					temp.ptr = info_section_header.get();
				} else
				if(cname == "supply_loss") {
					temp.ptr = supply_loss.get();
				} else
				if(cname == "supply_throughput") {
					temp.ptr = supply_throughput.get();
				} else
				if(cname == "satisfaction_label") {
					temp.ptr = satisfaction_label.get();
				} else
				if(cname == "supply_loss_label") {
					temp.ptr = supply_loss_label.get();
				} else
				if(cname == "supply_throughput_label") {
					temp.ptr = supply_throughput_label.get();
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
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_logisticswindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_logisticswindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_logisticswindow_neutral_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_logisticswindow_bottom_spacer(state);
				}
				if(cname == "consumption_row") {
					temp.ptr = make_logisticswindow_consumption_row(state);
				}
				if(cname == "consumption_header") {
					temp.ptr = make_logisticswindow_consumption_header(state);
				}
				if(cname == "commodity_grid_item") {
					temp.ptr = make_logisticswindow_commodity_grid_item(state);
				}
				if(cname == "commodity_spacer") {
					temp.ptr = make_logisticswindow_commodity_spacer(state);
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
			case layout_item_types::generator2:
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
void logisticswindow_section_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::section_header"));
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
		if(child_data.name == "label") {
			label = std::make_unique<logisticswindow_section_header_label_t>();
			label->parent = this;
			auto cptr = label.get();
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
		if(child_data.name == "llbutton") {
			llbutton = std::make_unique<logisticswindow_section_header_llbutton_t>();
			llbutton->parent = this;
			auto cptr = llbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "lbutton") {
			lbutton = std::make_unique<logisticswindow_section_header_lbutton_t>();
			lbutton->parent = this;
			auto cptr = lbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "rbutton") {
			rbutton = std::make_unique<logisticswindow_section_header_rbutton_t>();
			rbutton->parent = this;
			auto cptr = rbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "rrbutton") {
			rrbutton = std::make_unique<logisticswindow_section_header_rrbutton_t>();
			rrbutton->parent = this;
			auto cptr = rrbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->template_id = child_data.template_id;
			cptr->icon = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "setting_amount") {
			setting_amount = std::make_unique<logisticswindow_section_header_setting_amount_t>();
			setting_amount->parent = this;
			auto cptr = setting_amount.get();
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
		if(child_data.name == "expand_button") {
			expand_button = std::make_unique<logisticswindow_section_header_expand_button_t>();
			expand_button->parent = this;
			auto cptr = expand_button.get();
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
		if(child_data.name == "satisfaction") {
			satisfaction = std::make_unique<logisticswindow_section_header_satisfaction_t>();
			satisfaction->parent = this;
			auto cptr = satisfaction.get();
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
		if(child_data.name == "min_setting") {
			min_setting = std::make_unique<logisticswindow_section_header_min_setting_t>();
			min_setting->parent = this;
			auto cptr = min_setting.get();
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
		if(child_data.name == "max_setting") {
			max_setting = std::make_unique<logisticswindow_section_header_max_setting_t>();
			max_setting->parent = this;
			auto cptr = max_setting.get();
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
		if(child_data.name == "info_section_header") {
			info_section_header = std::make_unique<logisticswindow_section_header_info_section_header_t>();
			info_section_header->parent = this;
			auto cptr = info_section_header.get();
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
		if(child_data.name == "supply_loss") {
			supply_loss = std::make_unique<logisticswindow_section_header_supply_loss_t>();
			supply_loss->parent = this;
			auto cptr = supply_loss.get();
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
		if(child_data.name == "supply_throughput") {
			supply_throughput = std::make_unique<logisticswindow_section_header_supply_throughput_t>();
			supply_throughput->parent = this;
			auto cptr = supply_throughput.get();
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
		if(child_data.name == "satisfaction_label") {
			satisfaction_label = std::make_unique<template_label>();
			satisfaction_label->parent = this;
			auto cptr = satisfaction_label.get();
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
		if(child_data.name == "supply_loss_label") {
			supply_loss_label = std::make_unique<template_label>();
			supply_loss_label->parent = this;
			auto cptr = supply_loss_label.get();
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
		if(child_data.name == "supply_throughput_label") {
			supply_throughput_label = std::make_unique<template_label>();
			supply_throughput_label->parent = this;
			auto cptr = supply_throughput_label.get();
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
// BEGIN section_header::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_section_header(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_section_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result logisticswindow_neutral_spacer_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_neutral_spacer_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_neutral_spacer_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN neutral_spacer::update
// END
}
void logisticswindow_neutral_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::neutral_spacer"));
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
// BEGIN neutral_spacer::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_neutral_spacer(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_neutral_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result logisticswindow_bottom_spacer_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_bottom_spacer_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_bottom_spacer_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN bottom_spacer::update
// END
}
void logisticswindow_bottom_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::bottom_spacer"));
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
// BEGIN bottom_spacer::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_bottom_spacer(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_bottom_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result logisticswindow_consumption_row_contents_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_consumption_row_contents_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_consumption_row_contents_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(x >= table_source->consumption_table_item_name_column_start && x < table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width) {
	}
	if(x >= table_source->consumption_table_item_amount_column_start && x < table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width) {
		ident = 1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->consumption_table_item_amount_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->consumption_table_item_amount_column_width);
		return;
	}
	if(x >= table_source->consumption_table_fufillment_percent_column_start && x < table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void logisticswindow_consumption_row_contents_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(x >=  table_source->consumption_table_item_name_column_start && x <  table_source->consumption_table_item_name_column_start +  table_source->consumption_table_item_name_column_width) {
	}
	if(x >=  table_source->consumption_table_item_amount_column_start && x <  table_source->consumption_table_item_amount_column_start +  table_source->consumption_table_item_amount_column_width) {
	text::add_line(state, contents, table_source->consumption_table_item_amount_column_tooltip_key);
	}
	if(x >=  table_source->consumption_table_fufillment_percent_column_start && x <  table_source->consumption_table_fufillment_percent_column_start +  table_source->consumption_table_fufillment_percent_column_width) {
	}
}
void logisticswindow_consumption_row_contents_t::set_item_name_text(sys::state & state, std::string const& new_text) {
		auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(new_text !=  item_name_cached_text) {
		item_name_cached_text = new_text;
		item_name_internal_layout.contents.clear();
		item_name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->consumption_table_item_name_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->consumption_table_item_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_name_cached_text);
		}
	} else {
	}
}
void logisticswindow_consumption_row_contents_t::set_item_amount_text(sys::state & state, std::string const& new_text) {
		auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(new_text !=  item_amount_cached_text) {
		item_amount_cached_text = new_text;
		item_amount_internal_layout.contents.clear();
		item_amount_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_amount_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->consumption_table_item_amount_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->consumption_table_item_amount_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_amount_cached_text);
		}
	} else {
	}
}
void logisticswindow_consumption_row_contents_t::set_fufillment_percent_text(sys::state & state, std::string const& new_text) {
		auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(new_text !=  fufillment_percent_cached_text) {
		fufillment_percent_cached_text = new_text;
		fufillment_percent_internal_layout.contents.clear();
		fufillment_percent_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ fufillment_percent_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->consumption_table_fufillment_percent_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->consumption_table_fufillment_percent_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, fufillment_percent_cached_text);
		}
	} else {
	}
}
void logisticswindow_consumption_row_contents_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_lead_space = rel_mouse_x >= table_source->consumption_table_lead_space_column_start && rel_mouse_x < (table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_lead_space){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_lead_space_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_lead_space){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_lead_space){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y), float(table_source->consumption_table_lead_space_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_lead_space_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_item_name = rel_mouse_x >= table_source->consumption_table_item_name_column_start && rel_mouse_x < (table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_item_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_item_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_item_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y), float(table_source->consumption_table_item_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_item_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_item_name = state.ui_templates.colors[item_name_text_color]; 	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->consumption_table_item_name_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_item_name.r, col_color_item_name.g, col_color_item_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_item_amount = rel_mouse_x >= table_source->consumption_table_item_amount_column_start && rel_mouse_x < (table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_item_amount_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_item_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_item_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y), float(table_source->consumption_table_item_amount_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_item_amount_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_item_amount = state.ui_templates.colors[item_amount_text_color]; 	if(!item_amount_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : item_amount_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->consumption_table_item_amount_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_item_amount.r, col_color_item_amount.g, col_color_item_amount.b }, ogl::color_modification::none);
		}
	}
	bool col_um_fufillment_percent = rel_mouse_x >= table_source->consumption_table_fufillment_percent_column_start && rel_mouse_x < (table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_fufillment_percent){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_fufillment_percent_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_fufillment_percent){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_fufillment_percent){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y), float(table_source->consumption_table_fufillment_percent_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_fufillment_percent_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_fufillment_percent = state.ui_templates.colors[fufillment_percent_text_color]; 	if(!fufillment_percent_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : fufillment_percent_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->consumption_table_fufillment_percent_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_fufillment_percent.r, col_color_fufillment_percent.g, col_color_fufillment_percent.b }, ogl::color_modification::none);
		}
	}
}
void logisticswindow_consumption_row_contents_t::on_update(sys::state& state) noexcept {
	logisticswindow_consumption_row_t& consumption_row = *((logisticswindow_consumption_row_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN consumption_row::contents::update
	set_item_name_text(state, consumption_row.name);
	set_item_amount_text(state, text::prettify_float(consumption_row.fufilled) + "/" + text::prettify_float(consumption_row.required));
	set_fufillment_percent_text(state, text::format_percentage(consumption_row.satisfaction, 0));
// END
}
void logisticswindow_consumption_row_contents_t::on_create(sys::state& state) noexcept {
// BEGIN consumption_row::contents::create
// END
}
void  logisticswindow_consumption_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result logisticswindow_consumption_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_consumption_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_consumption_row_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN consumption_row::update
// END
	remake_layout(state, true);
}
void logisticswindow_consumption_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
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
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_logisticswindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_logisticswindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_logisticswindow_neutral_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_logisticswindow_bottom_spacer(state);
				}
				if(cname == "consumption_row") {
					temp.ptr = make_logisticswindow_consumption_row(state);
				}
				if(cname == "consumption_header") {
					temp.ptr = make_logisticswindow_consumption_header(state);
				}
				if(cname == "commodity_grid_item") {
					temp.ptr = make_logisticswindow_commodity_grid_item(state);
				}
				if(cname == "commodity_spacer") {
					temp.ptr = make_logisticswindow_commodity_spacer(state);
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
			case layout_item_types::generator2:
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
void logisticswindow_consumption_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::consumption_row"));
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
			contents = std::make_unique<logisticswindow_consumption_row_contents_t>();
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
		if(child_data.name == ".tabconsumption_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			consumption_table_ascending_icon_key = main_section.read<std::string_view>();
			consumption_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_lead_space_column_start = running_w_total;
			col_section.read(consumption_table_lead_space_column_width);
			running_w_total += consumption_table_lead_space_column_width;
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			consumption_table_item_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_item_name_column_start = running_w_total;
			col_section.read(consumption_table_item_name_column_width);
			running_w_total += consumption_table_item_name_column_width;
			col_section.read(consumption_table_item_name_column_text_color);
			col_section.read(consumption_table_item_name_header_text_color);
			col_section.read(consumption_table_item_name_text_alignment);
			consumption_table_item_amount_header_text_key = col_section.read<std::string_view>();
			consumption_table_item_amount_header_tooltip_key = col_section.read<std::string_view>();
			consumption_table_item_amount_column_tooltip_key = col_section.read<std::string_view>();
			consumption_table_item_amount_column_start = running_w_total;
			col_section.read(consumption_table_item_amount_column_width);
			running_w_total += consumption_table_item_amount_column_width;
			col_section.read(consumption_table_item_amount_column_text_color);
			col_section.read(consumption_table_item_amount_header_text_color);
			col_section.read(consumption_table_item_amount_text_alignment);
			consumption_table_fufillment_percent_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_fufillment_percent_column_start = running_w_total;
			col_section.read(consumption_table_fufillment_percent_column_width);
			running_w_total += consumption_table_fufillment_percent_column_width;
			col_section.read(consumption_table_fufillment_percent_column_text_color);
			col_section.read(consumption_table_fufillment_percent_header_text_color);
			col_section.read(consumption_table_fufillment_percent_text_alignment);
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
// BEGIN consumption_row::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_consumption_row(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_consumption_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result logisticswindow_consumption_header_contents_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(x >= table_source->consumption_table_item_name_column_start && x < table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->consumption_table_item_name_sort_direction;
		table_source->consumption_table_item_name_sort_direction = 0;
		table_source->consumption_table_item_amount_sort_direction = 0;
		table_source->consumption_table_fufillment_percent_sort_direction = 0;
		table_source->consumption_table_item_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->consumption_table_item_amount_column_start && x < table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->consumption_table_item_amount_sort_direction;
		table_source->consumption_table_item_name_sort_direction = 0;
		table_source->consumption_table_item_amount_sort_direction = 0;
		table_source->consumption_table_fufillment_percent_sort_direction = 0;
		table_source->consumption_table_item_amount_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->consumption_table_fufillment_percent_column_start && x < table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->consumption_table_fufillment_percent_sort_direction;
		table_source->consumption_table_item_name_sort_direction = 0;
		table_source->consumption_table_item_amount_sort_direction = 0;
		table_source->consumption_table_fufillment_percent_sort_direction = 0;
		table_source->consumption_table_fufillment_percent_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result logisticswindow_consumption_header_contents_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_consumption_header_contents_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(x >= table_source->consumption_table_item_name_column_start && x < table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width) {
	}
	if(x >= table_source->consumption_table_item_amount_column_start && x < table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width) {
		ident = 1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->consumption_table_item_amount_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->consumption_table_item_amount_column_width);
		return;
	}
	if(x >= table_source->consumption_table_fufillment_percent_column_start && x < table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void logisticswindow_consumption_header_contents_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	if(x >=  table_source->consumption_table_item_name_column_start && x <  table_source->consumption_table_item_name_column_start +  table_source->consumption_table_item_name_column_width) {
	}
	if(x >=  table_source->consumption_table_item_amount_column_start && x <  table_source->consumption_table_item_amount_column_start +  table_source->consumption_table_item_amount_column_width) {
	text::add_line(state, contents, table_source->consumption_table_item_amount_header_tooltip_key);
	}
	if(x >=  table_source->consumption_table_fufillment_percent_column_start && x <  table_source->consumption_table_fufillment_percent_column_start +  table_source->consumption_table_fufillment_percent_column_width) {
	}
}
void logisticswindow_consumption_header_contents_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	{
	item_name_cached_text = text::produce_simple_string(state, table_source->consumption_table_item_name_header_text_key);
	 item_name_internal_layout.contents.clear();
	 item_name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->consumption_table_item_name_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->consumption_table_item_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_name_cached_text);
	}
	{
	item_amount_cached_text = text::produce_simple_string(state, table_source->consumption_table_item_amount_header_text_key);
	 item_amount_internal_layout.contents.clear();
	 item_amount_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_amount_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->consumption_table_item_amount_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->consumption_table_item_amount_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_amount_cached_text);
	}
	{
	fufillment_percent_cached_text = text::produce_simple_string(state, table_source->consumption_table_fufillment_percent_header_text_key);
	 fufillment_percent_internal_layout.contents.clear();
	 fufillment_percent_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  fufillment_percent_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->consumption_table_fufillment_percent_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->consumption_table_fufillment_percent_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, fufillment_percent_cached_text);
	}
}
void logisticswindow_consumption_header_contents_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (logisticswindow_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_lead_space = rel_mouse_x >= table_source->consumption_table_lead_space_column_start && rel_mouse_x < (table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_lead_space){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_lead_space_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_lead_space){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start + table_source->consumption_table_lead_space_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_lead_space){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y), float(table_source->consumption_table_lead_space_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_lead_space_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_lead_space_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_item_name = rel_mouse_x >= table_source->consumption_table_item_name_column_start && rel_mouse_x < (table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item_name) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_item_name_column_start), float(y), float(table_source->consumption_table_item_name_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->consumption_table_item_name_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_item_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_item_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start + table_source->consumption_table_item_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_item_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y), float(table_source->consumption_table_item_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_name_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_item_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_item_name = state.ui_templates.colors[table_source->consumption_table_item_name_header_text_color]; 	if(table_source->consumption_table_item_name_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_item_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->consumption_table_item_name_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_item_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->consumption_table_item_name_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_item_name.r, col_color_item_name.g, col_color_item_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_item_amount = rel_mouse_x >= table_source->consumption_table_item_amount_column_start && rel_mouse_x < (table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item_amount) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_item_amount_column_start), float(y), float(table_source->consumption_table_item_amount_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->consumption_table_item_amount_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_item_amount_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_item_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start + table_source->consumption_table_item_amount_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_item_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y), float(table_source->consumption_table_item_amount_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_item_amount_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_item_amount_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_item_amount = state.ui_templates.colors[table_source->consumption_table_item_amount_header_text_color]; 	if(table_source->consumption_table_item_amount_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_item_amount_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->consumption_table_item_amount_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_item_amount_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!item_amount_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : item_amount_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->consumption_table_item_amount_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_item_amount.r, col_color_item_amount.g, col_color_item_amount.b }, ogl::color_modification::none);
		}
	}
	bool col_um_fufillment_percent = rel_mouse_x >= table_source->consumption_table_fufillment_percent_column_start && rel_mouse_x < (table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_fufillment_percent) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y), float(table_source->consumption_table_fufillment_percent_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->consumption_table_fufillment_percent_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_fufillment_percent){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->consumption_table_fufillment_percent_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_fufillment_percent){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start + table_source->consumption_table_fufillment_percent_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_fufillment_percent){
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y), float(table_source->consumption_table_fufillment_percent_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->consumption_table_fufillment_percent_column_start), float(y + base_data.size.y - 2), float(table_source->consumption_table_fufillment_percent_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_fufillment_percent = state.ui_templates.colors[table_source->consumption_table_fufillment_percent_header_text_color]; 	if(table_source->consumption_table_fufillment_percent_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_fufillment_percent_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->consumption_table_fufillment_percent_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->consumption_table_fufillment_percent_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!fufillment_percent_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : fufillment_percent_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->consumption_table_fufillment_percent_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_fufillment_percent.r, col_color_fufillment_percent.g, col_color_fufillment_percent.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void logisticswindow_consumption_header_contents_t::on_update(sys::state& state) noexcept {
	logisticswindow_consumption_header_t& consumption_header = *((logisticswindow_consumption_header_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN consumption_header::contents::update
// END
}
void logisticswindow_consumption_header_contents_t::on_create(sys::state& state) noexcept {
// BEGIN consumption_header::contents::create
// END
}
void  logisticswindow_consumption_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result logisticswindow_consumption_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_consumption_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_consumption_header_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN consumption_header::update
// END
	remake_layout(state, true);
}
void logisticswindow_consumption_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
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
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_logisticswindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_logisticswindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_logisticswindow_neutral_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_logisticswindow_bottom_spacer(state);
				}
				if(cname == "consumption_row") {
					temp.ptr = make_logisticswindow_consumption_row(state);
				}
				if(cname == "consumption_header") {
					temp.ptr = make_logisticswindow_consumption_header(state);
				}
				if(cname == "commodity_grid_item") {
					temp.ptr = make_logisticswindow_commodity_grid_item(state);
				}
				if(cname == "commodity_spacer") {
					temp.ptr = make_logisticswindow_commodity_spacer(state);
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
			case layout_item_types::generator2:
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
void logisticswindow_consumption_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::consumption_header"));
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
			contents = std::make_unique<logisticswindow_consumption_header_contents_t>();
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
		if(child_data.name == ".tabconsumption_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			consumption_table_ascending_icon_key = main_section.read<std::string_view>();
			consumption_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_lead_space_column_start = running_w_total;
			col_section.read(consumption_table_lead_space_column_width);
			running_w_total += consumption_table_lead_space_column_width;
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			consumption_table_item_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_item_name_column_start = running_w_total;
			col_section.read(consumption_table_item_name_column_width);
			running_w_total += consumption_table_item_name_column_width;
			col_section.read(consumption_table_item_name_column_text_color);
			col_section.read(consumption_table_item_name_header_text_color);
			col_section.read(consumption_table_item_name_text_alignment);
			consumption_table_item_amount_header_text_key = col_section.read<std::string_view>();
			consumption_table_item_amount_header_tooltip_key = col_section.read<std::string_view>();
			consumption_table_item_amount_column_tooltip_key = col_section.read<std::string_view>();
			consumption_table_item_amount_column_start = running_w_total;
			col_section.read(consumption_table_item_amount_column_width);
			running_w_total += consumption_table_item_amount_column_width;
			col_section.read(consumption_table_item_amount_column_text_color);
			col_section.read(consumption_table_item_amount_header_text_color);
			col_section.read(consumption_table_item_amount_text_alignment);
			consumption_table_fufillment_percent_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			consumption_table_fufillment_percent_column_start = running_w_total;
			col_section.read(consumption_table_fufillment_percent_column_width);
			running_w_total += consumption_table_fufillment_percent_column_width;
			col_section.read(consumption_table_fufillment_percent_column_text_color);
			col_section.read(consumption_table_fufillment_percent_header_text_color);
			col_section.read(consumption_table_fufillment_percent_text_alignment);
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
// BEGIN consumption_header::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_consumption_header(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_consumption_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result logisticswindow_commodity_grid_item_commodity_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN commodity_grid_item::commodity_icon::lbutton_action
	main.stockpile_target_input->set_text(state, std::u16string{ }); // Reset the input field when switching between commodities
	main.selected_stockpile_commodity = commodity_grid_item.value;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_commodity_grid_item_commodity_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN commodity_grid_item::commodity_icon::rbutton_action
	main.stockpile_target_input->set_text(state, std::u16string{ }); // Reset the input field when switching between commodities
	main.selected_stockpile_commodity = dcon::commodity_id{ };
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
void logisticswindow_commodity_grid_item_commodity_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void logisticswindow_commodity_grid_item_commodity_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(background_gid) {
		auto& gfx_def = state.ui_defs.gfx[background_gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), frame, gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else {
				ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			}
		}
	}
}
void logisticswindow_commodity_grid_item_commodity_icon_t::on_update(sys::state& state) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::commodity_icon::update
	if(!commodity_grid_item.value) {
		return;
	}
	frame = state.world.commodity_get_icon(commodity_grid_item.value);
// END
}
void logisticswindow_commodity_grid_item_commodity_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN commodity_grid_item::commodity_icon::create
// END
}
void logisticswindow_commodity_grid_item_current_stockpile_count_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::current_stockpile_count::tooltip
	float current_amount = state.world.nation_get_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	text::add_line(state, contents, "current_stockpile_count_tooltip", text::variable_type::val, text::fp_one_place{ current_amount });
// END
}
void logisticswindow_commodity_grid_item_current_stockpile_count_t::on_update(sys::state& state) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::current_stockpile_count::update
	if(!commodity_grid_item.value) {
		return;
	}
	float current_amount = state.world.nation_get_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	set_text(state, text::prettify_float(current_amount));
// END
}
void logisticswindow_commodity_grid_item_stockpile_target_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::stockpile_target::tooltip
	float current_amount = state.world.nation_get_stockpile_targets(state.local_player_nation, commodity_grid_item.value);
	text::add_line(state, contents, "stockpile_target_tooltip", text::variable_type::val, text::fp_one_place{ current_amount });
// END
}
void logisticswindow_commodity_grid_item_stockpile_target_t::on_update(sys::state& state) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::stockpile_target::update
	if(!commodity_grid_item.value) {
		return;
	}
	float current_amount = state.world.nation_get_stockpile_targets(state.local_player_nation, commodity_grid_item.value);
	set_text(state, text::prettify_float(current_amount));
// END
}
void logisticswindow_commodity_grid_item_daily_stockpile_change_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::daily_stockpile_change::tooltip
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	text::add_line(state, contents, "stockpile_change_tooltip", text::variable_type::val, text::fp_two_places{ current_stockpile - last_stockpile });
// END
}
void logisticswindow_commodity_grid_item_daily_stockpile_change_t::on_update(sys::state& state) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::daily_stockpile_change::update
	text::substitution_map sub{ };
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{ current_stockpile - last_stockpile });
	set_text(state, text::resolve_string_substitution(state, "stockpile_change", sub));
// END
}
void logisticswindow_commodity_grid_item_days_left_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::days_left::tooltip
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	float stockpile_change = current_stockpile - last_stockpile;
	float days_supplies_left = (stockpile_change >= 0.0f ? INFINITY : current_stockpile / std::abs(stockpile_change));
	text::add_line(state, contents, "stockpile_days_left_tooltip", text::variable_type::days, text::fp_one_place{ days_supplies_left });
// END
}
void logisticswindow_commodity_grid_item_days_left_t::on_update(sys::state& state) noexcept {
	logisticswindow_commodity_grid_item_t& commodity_grid_item = *((logisticswindow_commodity_grid_item_t*)(parent)); 
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::days_left::update
	text::substitution_map sub{ };
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, commodity_grid_item.value);
	float stockpile_change = current_stockpile - last_stockpile;
	float days_supplies_left = (stockpile_change >= 0.0f ? INFINITY : current_stockpile / std::abs(stockpile_change));
	text::add_to_substitution_map(sub, text::variable_type::days, text::fp_one_place{ days_supplies_left });
	set_text(state, text::resolve_string_substitution(state, "stockpile_days_left", sub));
// END
}
ui::message_result logisticswindow_commodity_grid_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result logisticswindow_commodity_grid_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void logisticswindow_commodity_grid_item_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_grid_item::update
	if(!value) {
		flags |= element_base::is_invisible_mask;
	}
// END
	remake_layout(state, true);
}
void logisticswindow_commodity_grid_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				temp.ptr = nullptr;
				if(cname == "commodity_icon") {
					temp.ptr = commodity_icon.get();
				} else
				if(cname == "current_stockpile_count") {
					temp.ptr = current_stockpile_count.get();
				} else
				if(cname == "stockpile_target") {
					temp.ptr = stockpile_target.get();
				} else
				if(cname == "daily_stockpile_change") {
					temp.ptr = daily_stockpile_change.get();
				} else
				if(cname == "value_divider") {
					temp.ptr = value_divider.get();
				} else
				if(cname == "days_left") {
					temp.ptr = days_left.get();
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
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_logisticswindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_logisticswindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_logisticswindow_neutral_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_logisticswindow_bottom_spacer(state);
				}
				if(cname == "consumption_row") {
					temp.ptr = make_logisticswindow_consumption_row(state);
				}
				if(cname == "consumption_header") {
					temp.ptr = make_logisticswindow_consumption_header(state);
				}
				if(cname == "commodity_grid_item") {
					temp.ptr = make_logisticswindow_commodity_grid_item(state);
				}
				if(cname == "commodity_spacer") {
					temp.ptr = make_logisticswindow_commodity_spacer(state);
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
			case layout_item_types::generator2:
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
void logisticswindow_commodity_grid_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::commodity_grid_item"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	ui::element_base::flags |= ui::element_base::wants_update_when_hidden_mask;
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "commodity_icon") {
			commodity_icon = std::make_unique<logisticswindow_commodity_grid_item_commodity_icon_t>();
			commodity_icon->parent = this;
			auto cptr = commodity_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "current_stockpile_count") {
			current_stockpile_count = std::make_unique<logisticswindow_commodity_grid_item_current_stockpile_count_t>();
			current_stockpile_count->parent = this;
			auto cptr = current_stockpile_count.get();
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
		if(child_data.name == "stockpile_target") {
			stockpile_target = std::make_unique<logisticswindow_commodity_grid_item_stockpile_target_t>();
			stockpile_target->parent = this;
			auto cptr = stockpile_target.get();
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
		if(child_data.name == "daily_stockpile_change") {
			daily_stockpile_change = std::make_unique<logisticswindow_commodity_grid_item_daily_stockpile_change_t>();
			daily_stockpile_change->parent = this;
			auto cptr = daily_stockpile_change.get();
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
		if(child_data.name == "value_divider") {
			value_divider = std::make_unique<template_label>();
			value_divider->parent = this;
			auto cptr = value_divider.get();
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
		if(child_data.name == "days_left") {
			days_left = std::make_unique<logisticswindow_commodity_grid_item_days_left_t>();
			days_left->parent = this;
			auto cptr = days_left.get();
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
// BEGIN commodity_grid_item::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_commodity_grid_item(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_commodity_grid_item_t>();
	ptr->on_create(state);
	return ptr;
}
void logisticswindow_commodity_spacer_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
}
void logisticswindow_commodity_spacer_t::on_update(sys::state& state) noexcept {
	logisticswindow_main_t& main = *((logisticswindow_main_t*)(parent->parent)); 
// BEGIN commodity_spacer::update
// END
}
void logisticswindow_commodity_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("logisticswindow::commodity_spacer"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
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
// BEGIN commodity_spacer::create
// END
}
std::unique_ptr<ui::element_base> make_logisticswindow_commodity_spacer(sys::state& state) {
	auto ptr = std::make_unique<logisticswindow_commodity_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
