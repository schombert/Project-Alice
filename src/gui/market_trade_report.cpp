namespace alice_ui {
struct market_trade_report_body_report_header_t;
struct market_trade_report_body_close_button_t;
struct market_trade_report_body_pivot_volume_header_t;
struct market_trade_report_body_pivot_balance_top_header_t;
struct market_trade_report_body_pivot_balance_bottom_header_t;
struct market_trade_report_body_t;
struct market_trade_report_commodity_entry_t;
struct market_trade_report_commodity_t;
struct market_trade_report_trade_item_trade_item_c_t;
struct market_trade_report_trade_item_flag_real_t;
struct market_trade_report_trade_item_select_market_t;
struct market_trade_report_trade_item_t;
struct market_trade_report_nation_data_nation_name_t;
struct market_trade_report_nation_data_flag_t;
struct market_trade_report_nation_data_value_t;
struct market_trade_report_nation_data_t;
struct market_trade_report_trade_header_trade_header_c_t;
struct market_trade_report_trade_header_t;
struct market_trade_report_body_report_header_t : public ui::element_base {
// BEGIN body::report_header::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
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
struct market_trade_report_body_close_button_t : public ui::element_base {
// BEGIN body::close_button::variables
// END
	ogl::color4f color{ 1.000000f, 0.000000f, 0.000000f, 1.000000f };
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_body_pivot_volume_header_t : public ui::element_base {
// BEGIN body::pivot_volume_header::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::left;
	std::string cached_text;
	dcon::text_key text_key;
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
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_body_pivot_balance_top_header_t : public ui::element_base {
// BEGIN body::pivot_balance_top_header::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::left;
	std::string cached_text;
	dcon::text_key text_key;
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
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_body_pivot_balance_bottom_header_t : public ui::element_base {
// BEGIN body::pivot_balance_bottom_header::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::left;
	std::string cached_text;
	dcon::text_key text_key;
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
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_body_commodity_selector_t : public layout_generator {
// BEGIN body::commodity_selector::variables
// END
	struct commodity_option { dcon::commodity_id item; };
	std::vector<std::unique_ptr<ui::element_base>> commodity_pool;
	int32_t commodity_pool_used = 0;
	void add_commodity( dcon::commodity_id item);
	std::vector<std::variant<std::monostate, commodity_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct market_trade_report_body_list_t : public layout_generator {
// BEGIN body::list::variables
// END
	struct trade_item_option { dcon::market_id other; };
	std::vector<std::unique_ptr<ui::element_base>> trade_item_pool;
	int32_t trade_item_pool_used = 0;
	void add_trade_item( dcon::market_id other);
	std::vector<std::unique_ptr<ui::element_base>> trade_header_pool;
	int32_t trade_header_pool_used = 0;
	std::vector<std::variant<std::monostate, trade_item_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct market_trade_report_commodity_entry_t : public ui::element_base {
// BEGIN commodity::entry::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_trade_item_trade_item_c_t : public ui::element_base {
// BEGIN trade_item::trade_item_c::variables
// END
	text::layout flag_internal_layout;
	text::text_color  flag_text_color = text::text_color::black;
	std::string flag_cached_text;
	void set_flag_text(sys::state & state, std::string const& new_text);
	text::layout destination_internal_layout;
	text::text_color  destination_text_color = text::text_color::black;
	std::string destination_cached_text;
	void set_destination_text(sys::state & state, std::string const& new_text);
	text::layout distance_internal_layout;
	text::text_color  distance_text_color = text::text_color::black;
	std::string distance_cached_text;
	float distance_decimal_pos = 0.0f;	void set_distance_text(sys::state & state, std::string const& new_text);
	text::layout price_internal_layout;
	text::text_color  price_text_color = text::text_color::black;
	std::string price_cached_text;
	void set_price_text(sys::state & state, std::string const& new_text);
	text::layout volume_internal_layout;
	text::text_color  volume_text_color = text::text_color::black;
	std::string volume_cached_text;
	float volume_decimal_pos = 0.0f;	void set_volume_text(sys::state & state, std::string const& new_text);
	text::layout value_internal_layout;
	text::text_color  value_text_color = text::text_color::black;
	std::string value_cached_text;
	void set_value_text(sys::state & state, std::string const& new_text);
	text::layout empty_internal_layout;
	text::text_color  empty_text_color = text::text_color::black;
	std::string empty_cached_text;
	void set_empty_text(sys::state & state, std::string const& new_text);
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
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_trade_item_flag_real_t : public ui::element_base {
// BEGIN trade_item::flag_real::variables
// END
	std::variant<std::monostate, dcon::national_identity_id, dcon::rebel_faction_id, dcon::nation_id> flag;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
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
struct market_trade_report_trade_item_select_market_t : public ui::element_base {
// BEGIN trade_item::select_market::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_nation_data_nation_name_t : public ui::element_base {
// BEGIN nation_data::nation_name::variables
// END
	void on_create(sys::state& state) noexcept override;
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
struct market_trade_report_nation_data_flag_t : public ui::element_base {
// BEGIN nation_data::flag::variables
// END
	void on_create(sys::state& state) noexcept override;
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
struct market_trade_report_nation_data_value_t : public ui::element_base {
// BEGIN nation_data::value::variables
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
struct market_trade_report_trade_header_trade_header_c_t : public ui::element_base {
// BEGIN trade_header::trade_header_c::variables
// END
	text::layout destination_internal_layout;
	std::string destination_cached_text;
	text::layout distance_internal_layout;
	std::string distance_cached_text;
	text::layout price_internal_layout;
	std::string price_cached_text;
	text::layout volume_internal_layout;
	std::string volume_cached_text;
	text::layout value_internal_layout;
	std::string value_cached_text;
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
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	void on_reset_text(sys::state & state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct market_trade_report_body_t : public layout_window_element {
// BEGIN body::variables
// END
	dcon::commodity_id selected_commodity;
	std::unique_ptr<market_trade_report_body_report_header_t> report_header;
	std::unique_ptr<market_trade_report_body_close_button_t> close_button;
	std::unique_ptr<market_trade_report_body_pivot_volume_header_t> pivot_volume_header;
	std::unique_ptr<market_trade_report_body_pivot_balance_top_header_t> pivot_balance_top_header;
	std::unique_ptr<market_trade_report_body_pivot_balance_bottom_header_t> pivot_balance_bottom_header;
	market_trade_report_body_commodity_selector_t commodity_selector;
	market_trade_report_body_list_t list;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	text::text_color trade_flag_header_text_color = text::text_color::black;
	text::text_color trade_flag_column_text_color = text::text_color::black;
	text::alignment trade_flag_text_alignment = text::alignment::center;
	int16_t trade_flag_column_start = 0;
	int16_t trade_flag_column_width = 0;
	std::string_view trade_destination_header_text_key;
	text::text_color trade_destination_header_text_color = text::text_color::black;
	text::text_color trade_destination_column_text_color = text::text_color::black;
	text::alignment trade_destination_text_alignment = text::alignment::left;
	int16_t trade_destination_column_start = 0;
	int16_t trade_destination_column_width = 0;
	int16_t trade_selector_c_column_start = 0;
	int16_t trade_selector_c_column_width = 0;
	std::string_view trade_distance_header_text_key;
	text::text_color trade_distance_header_text_color = text::text_color::black;
	text::text_color trade_distance_column_text_color = text::text_color::black;
	text::alignment trade_distance_text_alignment = text::alignment::right;
	float trade_distance_decimal_pos = 0.0f;	int8_t trade_distance_sort_direction = 0;
	int16_t trade_distance_column_start = 0;
	int16_t trade_distance_column_width = 0;
	std::string_view trade_price_header_text_key;
	text::text_color trade_price_header_text_color = text::text_color::black;
	text::text_color trade_price_column_text_color = text::text_color::black;
	text::alignment trade_price_text_alignment = text::alignment::right;
	int8_t trade_price_sort_direction = 0;
	int16_t trade_price_column_start = 0;
	int16_t trade_price_column_width = 0;
	std::string_view trade_volume_header_text_key;
	text::text_color trade_volume_header_text_color = text::text_color::black;
	text::text_color trade_volume_column_text_color = text::text_color::black;
	text::alignment trade_volume_text_alignment = text::alignment::right;
	float trade_volume_decimal_pos = 0.0f;	int8_t trade_volume_sort_direction = 0;
	int16_t trade_volume_column_start = 0;
	int16_t trade_volume_column_width = 0;
	std::string_view trade_value_header_text_key;
	text::text_color trade_value_header_text_color = text::text_color::black;
	text::text_color trade_value_column_text_color = text::text_color::black;
	text::alignment trade_value_text_alignment = text::alignment::right;
	int8_t trade_value_sort_direction = 0;
	int16_t trade_value_column_start = 0;
	int16_t trade_value_column_width = 0;
	text::text_color trade_empty_header_text_color = text::text_color::black;
	text::text_color trade_empty_column_text_color = text::text_color::black;
	text::alignment trade_empty_text_alignment = text::alignment::center;
	int16_t trade_empty_column_start = 0;
	int16_t trade_empty_column_width = 0;
	std::string_view trade_ascending_icon_key;
	dcon::texture_id trade_ascending_icon;
	std::string_view trade_descending_icon_key;
	dcon::texture_id trade_descending_icon;
	ogl::color3f trade_divider_color{float(0.150579), float(0.129068), float(0.129068)};
	std::string_view texture_key;
	dcon::texture_id background_texture;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
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
		if(name_parameter == "selected_commodity") {
			return (void*)(&selected_commodity);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_market_trade_report_body(sys::state& state);
struct market_trade_report_commodity_t : public layout_window_element {
// BEGIN commodity::variables
// END
	dcon::commodity_id item;
	std::unique_ptr<market_trade_report_commodity_entry_t> entry;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view texture_key;
	std::string_view alt_texture_key;
	dcon::texture_id alt_background_texture;
	bool is_active = false;
	dcon::texture_id background_texture;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return (type == ui::mouse_probe_type::scroll ? ui::message_result::unseen : ui::message_result::consumed);
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "item") {
			return (void*)(&item);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_market_trade_report_commodity(sys::state& state);
struct market_trade_report_trade_item_t : public layout_window_element {
// BEGIN trade_item::variables
// END
	dcon::market_id other;
	std::unique_ptr<market_trade_report_trade_item_trade_item_c_t> trade_item_c;
	std::unique_ptr<market_trade_report_trade_item_flag_real_t> flag_real;
	std::unique_ptr<market_trade_report_trade_item_select_market_t> select_market;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view texture_key;
	std::string_view alt_texture_key;
	dcon::texture_id alt_background_texture;
	bool is_active = false;
	dcon::texture_id background_texture;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return (type == ui::mouse_probe_type::scroll ? ui::message_result::unseen : ui::message_result::consumed);
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "other") {
			return (void*)(&other);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_market_trade_report_trade_item(sys::state& state);
struct market_trade_report_nation_data_t : public layout_window_element {
// BEGIN nation_data::variables
// END
	dcon::nation_id trade_partner;
	std::unique_ptr<market_trade_report_nation_data_nation_name_t> nation_name;
	std::unique_ptr<market_trade_report_nation_data_flag_t> flag;
	std::unique_ptr<market_trade_report_nation_data_value_t> value;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "trade_partner") {
			return (void*)(&trade_partner);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_market_trade_report_nation_data(sys::state& state);
struct market_trade_report_trade_header_t : public layout_window_element {
// BEGIN trade_header::variables
// END
	std::unique_ptr<market_trade_report_trade_header_trade_header_c_t> trade_header_c;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_market_trade_report_trade_header(sys::state& state);
void market_trade_report_body_commodity_selector_t::add_commodity(dcon::commodity_id item) {
	values.emplace_back(commodity_option{item});
}
void  market_trade_report_body_commodity_selector_t::on_create(sys::state& state, layout_window_element* parent) {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::commodity_selector::on_create
	// sort by name
	std::vector<dcon::commodity_id> raw;
	state.world.for_each_commodity([&](auto cid) {
		raw.push_back(cid);
	});

	std::sort(raw.begin(), raw.end(), [&](auto a, auto b) {
		std::string a_name = text::produce_simple_string(state, state.world.commodity_get_name(a));
		std::string b_name = text::produce_simple_string(state, state.world.commodity_get_name(b));
		return b_name.compare(a_name) > 0;
	});

	for(auto & a : raw) {
		add_commodity(a);
	}
// END
}
void  market_trade_report_body_commodity_selector_t::update(sys::state& state, layout_window_element* parent) {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::commodity_selector::update
// END
}
measure_result  market_trade_report_body_commodity_selector_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<commodity_option>(values[index])) {
		if(commodity_pool.empty()) commodity_pool.emplace_back(make_market_trade_report_commodity(state));
		if(destination) {
			if(commodity_pool.size() <= size_t(commodity_pool_used)) commodity_pool.emplace_back(make_market_trade_report_commodity(state));
			commodity_pool[commodity_pool_used]->base_data.position.x = int16_t(x);
			commodity_pool[commodity_pool_used]->base_data.position.y = int16_t(y);
			commodity_pool[commodity_pool_used]->parent = destination;
			destination->children.push_back(commodity_pool[commodity_pool_used].get());
			((market_trade_report_commodity_t*)(commodity_pool[commodity_pool_used].get()))->item = std::get<commodity_option>(values[index]).item;
			((market_trade_report_commodity_t*)(commodity_pool[commodity_pool_used].get()))->is_active = alternate;
			commodity_pool[commodity_pool_used]->impl_on_update(state);
			commodity_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ commodity_pool[0]->base_data.size.x, commodity_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  market_trade_report_body_commodity_selector_t::reset_pools() {
	commodity_pool_used = 0;
}
void market_trade_report_body_list_t::add_trade_item(dcon::market_id other) {
	values.emplace_back(trade_item_option{other});
}
void  market_trade_report_body_list_t::on_create(sys::state& state, layout_window_element* parent) {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::list::on_create
// END
}
void  market_trade_report_body_list_t::update(sys::state& state, layout_window_element* parent) {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::list::update
	values.clear();

	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	state.world.market_for_each_trade_route(mid, [&](auto trid) {
		auto mid_0 = state.world.trade_route_get_connected_markets(trid, 0);
		auto mid_1 = state.world.trade_route_get_connected_markets(trid, 1);

		if(mid_0 == mid) {
			add_trade_item(mid_1);
		} else {
			add_trade_item(mid_0);
		}
	});

// END
	{
	bool work_to_do = false;
	auto table_source = (market_trade_report_body_t*)(parent);
	if(table_source->trade_distance_sort_direction != 0) work_to_do = true;
	if(table_source->trade_price_sort_direction != 0) work_to_do = true;
	if(table_source->trade_volume_sort_direction != 0) work_to_do = true;
	if(table_source->trade_value_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<trade_item_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<trade_item_option>(values[i])) ++i;
				if(table_source->trade_distance_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<trade_item_option>(raw_a);
						auto const& b = std::get<trade_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::list::trade::sort::distance
						auto trade_route_a = state.world.get_trade_route_by_province_pair(mid, a.other);
						auto trade_route_b = state.world.get_trade_route_by_province_pair(mid, b.other);
						auto distance_a = state.world.trade_route_get_distance(trade_route_a);
						auto distance_b = state.world.trade_route_get_distance(trade_route_b);
						result = cmp3(distance_a, distance_b);
// END
						return -result == table_source->trade_distance_sort_direction;
					});
				}
				if(table_source->trade_price_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<trade_item_option>(raw_a);
						auto const& b = std::get<trade_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::list::trade::sort::price
						if(state.selected_trade_good) {
							auto price_a = economy::price(state, a.other, state.selected_trade_good);
							auto price_b = economy::price(state, b.other, state.selected_trade_good);

							result = cmp3(price_a, price_b);
						} else {
							result = cmp3(0, 0);
						}
// END
						return -result == table_source->trade_price_sort_direction;
					});
				}
				if(table_source->trade_volume_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<trade_item_option>(raw_a);
						auto const& b = std::get<trade_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::list::trade::sort::volume
						auto trade_route_a = state.world.get_trade_route_by_province_pair(mid, a.other);
						auto trade_route_b = state.world.get_trade_route_by_province_pair(mid, b.other);

						int32_t index_a = 0;
						if(mid == state.world.trade_route_get_connected_markets(trade_route_a, index_a)) {
							index_a = 1;
						}
						int32_t index_b = 0;
						if(mid == state.world.trade_route_get_connected_markets(trade_route_b, index_b)) {
							index_b = 1;
						}

						if(state.selected_trade_good) {
							auto value_a = state.world.trade_route_get_volume(trade_route_a, state.selected_trade_good) * ((float)index_a - 0.5f);
							auto value_b = state.world.trade_route_get_volume(trade_route_b, state.selected_trade_good) * ((float)index_b - 0.5f);

							result = cmp3(value_a, value_b);
						} else {
							result = cmp3(0, 0);
						}
// END
						return -result == table_source->trade_volume_sort_direction;
					});
				}
				if(table_source->trade_value_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<trade_item_option>(raw_a);
						auto const& b = std::get<trade_item_option>(raw_b);
						int8_t result = 0;
// BEGIN body::list::trade::sort::value
						auto trade_route_a = state.world.get_trade_route_by_province_pair(mid, a.other);
						auto trade_route_b = state.world.get_trade_route_by_province_pair(mid, b.other);

						auto mult_a = 1.f;
						if(mid == state.world.trade_route_get_connected_markets(trade_route_a, 1)) {
							mult_a = -1.f;
						}

						auto mult_b = 1.f;
						if(mid == state.world.trade_route_get_connected_markets(trade_route_b, 1)) {
							mult_b = -1.f;
						}

						if(state.selected_trade_good) {
							auto value_a = state.world.trade_route_get_volume(trade_route_a, state.selected_trade_good) * mult_a;
							auto value_b = state.world.trade_route_get_volume(trade_route_b, state.selected_trade_good) * mult_b;

							auto price = state.world.commodity_get_median_price(state.selected_trade_good);

							result = cmp3(value_a * price, value_b * price);
						} else {
							auto value_a = 0.f;
							auto value_b = 0.f;

							state.world.for_each_commodity([&](auto cid) {
								auto price = state.world.commodity_get_median_price(cid);
								value_a += price * state.world.trade_route_get_volume(trade_route_a, cid) * mult_a;
								value_b += price * state.world.trade_route_get_volume(trade_route_b, cid) * mult_b;
							});

							result = cmp3(value_a, value_b);
						}
// END
						return -result == table_source->trade_value_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  market_trade_report_body_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<trade_item_option>(values[index])) {
		if(trade_header_pool.empty()) trade_header_pool.emplace_back(make_market_trade_report_trade_header(state));
		if(trade_item_pool.empty()) trade_item_pool.emplace_back(make_market_trade_report_trade_item(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<trade_item_option>(values[index - 1]))) {
			if(destination) {
				if(trade_header_pool.size() <= size_t(trade_header_pool_used)) trade_header_pool.emplace_back(make_market_trade_report_trade_header(state));
				if(trade_item_pool.size() <= size_t(trade_item_pool_used)) trade_item_pool.emplace_back(make_market_trade_report_trade_item(state));
				trade_header_pool[trade_header_pool_used]->base_data.position.x = int16_t(x);
				trade_header_pool[trade_header_pool_used]->base_data.position.y = int16_t(y);
				if(!trade_header_pool[trade_header_pool_used]->parent) {
					trade_header_pool[trade_header_pool_used]->parent = destination;
					trade_header_pool[trade_header_pool_used]->impl_on_update(state);
					trade_header_pool[trade_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(trade_header_pool[trade_header_pool_used].get());
				trade_item_pool[trade_item_pool_used]->base_data.position.x = int16_t(x);
				trade_item_pool[trade_item_pool_used]->base_data.position.y = int16_t(y +  trade_item_pool[0]->base_data.size.y + 0);
				trade_item_pool[trade_item_pool_used]->parent = destination;
				destination->children.push_back(trade_item_pool[trade_item_pool_used].get());
				((market_trade_report_trade_item_t*)(trade_item_pool[trade_item_pool_used].get()))->other = std::get<trade_item_option>(values[index]).other;
			((market_trade_report_trade_item_t*)(trade_item_pool[trade_item_pool_used].get()))->is_active = !alternate;
				trade_item_pool[trade_item_pool_used]->impl_on_update(state);
				trade_header_pool_used++;
				trade_item_pool_used++;
			}
			return measure_result{std::max(trade_header_pool[0]->base_data.size.x, trade_item_pool[0]->base_data.size.x), trade_header_pool[0]->base_data.size.y + trade_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(trade_item_pool.size() <= size_t(trade_item_pool_used)) trade_item_pool.emplace_back(make_market_trade_report_trade_item(state));
			trade_item_pool[trade_item_pool_used]->base_data.position.x = int16_t(x);
			trade_item_pool[trade_item_pool_used]->base_data.position.y = int16_t(y);
			trade_item_pool[trade_item_pool_used]->parent = destination;
			destination->children.push_back(trade_item_pool[trade_item_pool_used].get());
			((market_trade_report_trade_item_t*)(trade_item_pool[trade_item_pool_used].get()))->other = std::get<trade_item_option>(values[index]).other;
			((market_trade_report_trade_item_t*)(trade_item_pool[trade_item_pool_used].get()))->is_active = alternate;
			trade_item_pool[trade_item_pool_used]->impl_on_update(state);
			trade_item_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ trade_item_pool[0]->base_data.size.x, trade_item_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  market_trade_report_body_list_t::reset_pools() {
	trade_header_pool_used = 0;
	trade_item_pool_used = 0;
}
void market_trade_report_body_report_header_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void market_trade_report_body_report_header_t::on_reset_text(sys::state& state) noexcept {
}
void market_trade_report_body_report_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void market_trade_report_body_report_header_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::report_header::update
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::province, state.map_state.selected_province);
	auto result = text::resolve_string_substitution(state, "market_report", m);
	set_text(state, result);
// END
}
void market_trade_report_body_report_header_t::on_create(sys::state& state) noexcept {
// BEGIN body::report_header::create
// END
}
ui::message_result market_trade_report_body_close_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN body::close_button::lbutton_action
	body.set_visible(state, false);
// END
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_body_close_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_body_close_button_t::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(key == sys::virtual_key::ESCAPE) {
		on_lbutton_down(state, 0, 0, mods);
		return ui::message_result::consumed;
	}
	return ui::message_result::unseen;
}
void market_trade_report_body_close_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
		ogl::render_alpha_colored_rect(state, float(x ), float(y), float(base_data.size.x), float(base_data.size.y), color.r, color.g, color.b, color.a);
}
void market_trade_report_body_close_button_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::close_button::update
// END
}
void market_trade_report_body_close_button_t::on_create(sys::state& state) noexcept {
// BEGIN body::close_button::create
// END
}
ui::message_result market_trade_report_body_pivot_volume_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result market_trade_report_body_pivot_volume_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void market_trade_report_body_pivot_volume_header_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void market_trade_report_body_pivot_volume_header_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void market_trade_report_body_pivot_volume_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
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
void market_trade_report_body_pivot_volume_header_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::pivot_volume_header::update
// END
}
void market_trade_report_body_pivot_volume_header_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::pivot_volume_header::create
// END
}
ui::message_result market_trade_report_body_pivot_balance_top_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result market_trade_report_body_pivot_balance_top_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void market_trade_report_body_pivot_balance_top_header_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void market_trade_report_body_pivot_balance_top_header_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void market_trade_report_body_pivot_balance_top_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
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
void market_trade_report_body_pivot_balance_top_header_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::pivot_balance_top_header::update
// END
}
void market_trade_report_body_pivot_balance_top_header_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::pivot_balance_top_header::create
// END
}
ui::message_result market_trade_report_body_pivot_balance_bottom_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result market_trade_report_body_pivot_balance_bottom_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void market_trade_report_body_pivot_balance_bottom_header_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void market_trade_report_body_pivot_balance_bottom_header_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void market_trade_report_body_pivot_balance_bottom_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
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
void market_trade_report_body_pivot_balance_bottom_header_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent)); 
// BEGIN body::pivot_balance_bottom_header::update
// END
}
void market_trade_report_body_pivot_balance_bottom_header_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::pivot_balance_bottom_header::create
// END
}
ui::message_result market_trade_report_body_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_body_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_trade_report_body_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void market_trade_report_body_t::on_update(sys::state& state) noexcept {
// BEGIN body::update
// END
	{
	bool left_align = true == (state_is_rtl(state)); 
	if(left_align)
		trade_distance_decimal_pos = 1000000.0f;
	else
		trade_distance_decimal_pos = -1000000.0f;
	}
	{
	bool left_align = true == (state_is_rtl(state)); 
	if(left_align)
		trade_volume_decimal_pos = 1000000.0f;
	else
		trade_volume_decimal_pos = -1000000.0f;
	}
	commodity_selector.update(state, this);
	list.update(state, this);
	remake_layout(state, true);
}
void market_trade_report_body_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(80);
		lvl.page_controls->base_data.size.y = int16_t(16);
	}
	auto optional_section = buffer.read_section(); // nothing
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
				if(cname == "report_header") {
					temp.ptr = report_header.get();
				}
				if(cname == "close_button") {
					temp.ptr = close_button.get();
				}
				if(cname == "pivot_volume_header") {
					temp.ptr = pivot_volume_header.get();
				}
				if(cname == "pivot_balance_top_header") {
					temp.ptr = pivot_balance_top_header.get();
				}
				if(cname == "pivot_balance_bottom_header") {
					temp.ptr = pivot_balance_bottom_header.get();
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
				if(cname == "body") {
					temp.ptr = make_market_trade_report_body(state);
				}
				if(cname == "commodity") {
					temp.ptr = make_market_trade_report_commodity(state);
				}
				if(cname == "trade_item") {
					temp.ptr = make_market_trade_report_trade_item(state);
				}
				if(cname == "nation_data") {
					temp.ptr = make_market_trade_report_nation_data(state);
				}
				if(cname == "trade_header") {
					temp.ptr = make_market_trade_report_trade_header(state);
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
				if(cname == "commodity_selector") {
					temp.generator = &commodity_selector;
				}
				if(cname == "list") {
					temp.generator = &list;
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
void market_trade_report_body_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_trade_report::body"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "report_header") {
			report_header = std::make_unique<market_trade_report_body_report_header_t>();
			report_header->parent = this;
			auto cptr = report_header.get();
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
		}
		if(child_data.name == "close_button") {
			close_button = std::make_unique<market_trade_report_body_close_button_t>();
			close_button->parent = this;
			auto cptr = close_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->color = child_data.table_highlight_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "pivot_volume_header") {
			pivot_volume_header = std::make_unique<market_trade_report_body_pivot_volume_header_t>();
			pivot_volume_header->parent = this;
			auto cptr = pivot_volume_header.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "pivot_balance_top_header") {
			pivot_balance_top_header = std::make_unique<market_trade_report_body_pivot_balance_top_header_t>();
			pivot_balance_top_header->parent = this;
			auto cptr = pivot_balance_top_header.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "pivot_balance_bottom_header") {
			pivot_balance_bottom_header = std::make_unique<market_trade_report_body_pivot_balance_bottom_header_t>();
			pivot_balance_bottom_header->parent = this;
			auto cptr = pivot_balance_bottom_header.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == ".tabtrade") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			trade_ascending_icon_key = main_section.read<std::string_view>();
			trade_descending_icon_key = main_section.read<std::string_view>();
			main_section.read(trade_divider_color);
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			trade_flag_column_start = running_w_total;
			col_section.read(trade_flag_column_width);
			running_w_total += trade_flag_column_width;
			col_section.read(trade_flag_column_text_color);
			col_section.read(trade_flag_header_text_color);
			col_section.read(trade_flag_text_alignment);
			trade_destination_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			trade_destination_column_start = running_w_total;
			col_section.read(trade_destination_column_width);
			running_w_total += trade_destination_column_width;
			col_section.read(trade_destination_column_text_color);
			col_section.read(trade_destination_header_text_color);
			col_section.read(trade_destination_text_alignment);
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			running_w_total += col_section.read<int16_t>();
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			trade_distance_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			trade_distance_column_start = running_w_total;
			col_section.read(trade_distance_column_width);
			running_w_total += trade_distance_column_width;
			col_section.read(trade_distance_column_text_color);
			col_section.read(trade_distance_header_text_color);
			col_section.read(trade_distance_text_alignment);
			trade_price_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			trade_price_column_start = running_w_total;
			col_section.read(trade_price_column_width);
			running_w_total += trade_price_column_width;
			col_section.read(trade_price_column_text_color);
			col_section.read(trade_price_header_text_color);
			col_section.read(trade_price_text_alignment);
			trade_volume_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			trade_volume_column_start = running_w_total;
			col_section.read(trade_volume_column_width);
			running_w_total += trade_volume_column_width;
			col_section.read(trade_volume_column_text_color);
			col_section.read(trade_volume_header_text_color);
			col_section.read(trade_volume_text_alignment);
			trade_value_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			trade_value_column_start = running_w_total;
			col_section.read(trade_value_column_width);
			running_w_total += trade_value_column_width;
			col_section.read(trade_value_column_text_color);
			col_section.read(trade_value_header_text_color);
			col_section.read(trade_value_text_alignment);
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			trade_empty_column_start = running_w_total;
			col_section.read(trade_empty_column_width);
			running_w_total += trade_empty_column_width;
			col_section.read(trade_empty_column_text_color);
			col_section.read(trade_empty_header_text_color);
			col_section.read(trade_empty_text_alignment);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	commodity_selector.on_create(state, this);
	list.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN body::create
// END
}
std::unique_ptr<ui::element_base> make_market_trade_report_body(sys::state& state) {
	auto ptr = std::make_unique<market_trade_report_body_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result market_trade_report_commodity_entry_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	market_trade_report_commodity_t& commodity = *((market_trade_report_commodity_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN commodity::entry::lbutton_action
	if(state.selected_trade_good == commodity.item) {
		state.selected_trade_good = { };
	} else {
		state.selected_trade_good = commodity.item;
	}
	state.update_trade_flow.store(true, std::memory_order::release);
	body.on_update(state);
// END
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_commodity_entry_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_trade_report_commodity_entry_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void market_trade_report_commodity_entry_t::on_reset_text(sys::state& state) noexcept {
}
void market_trade_report_commodity_entry_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_rect_with_repeated_border(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(8), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void market_trade_report_commodity_entry_t::on_update(sys::state& state) noexcept {
	market_trade_report_commodity_t& commodity = *((market_trade_report_commodity_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN commodity::entry::update
	set_text(state, text::get_commodity_name_with_icon(state, commodity.item));
	if(state.selected_trade_good == commodity.item) {
		commodity.is_active = true;
	} else {
		commodity.is_active = false;
	}
// END
}
void market_trade_report_commodity_entry_t::on_create(sys::state& state) noexcept {
// BEGIN commodity::entry::create
// END
}
ui::message_result market_trade_report_commodity_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_commodity_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_trade_report_commodity_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void market_trade_report_commodity_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN commodity::update
// END
	remake_layout(state, true);
}
void market_trade_report_commodity_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(80);
		lvl.page_controls->base_data.size.y = int16_t(16);
	}
	auto optional_section = buffer.read_section(); // nothing
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
				if(cname == "entry") {
					temp.ptr = entry.get();
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
				if(cname == "body") {
					temp.ptr = make_market_trade_report_body(state);
				}
				if(cname == "commodity") {
					temp.ptr = make_market_trade_report_commodity(state);
				}
				if(cname == "trade_item") {
					temp.ptr = make_market_trade_report_trade_item(state);
				}
				if(cname == "nation_data") {
					temp.ptr = make_market_trade_report_nation_data(state);
				}
				if(cname == "trade_header") {
					temp.ptr = make_market_trade_report_trade_header(state);
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
void market_trade_report_commodity_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_trade_report::commodity"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	alt_texture_key = win_data.alt_texture;
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "entry") {
			entry = std::make_unique<market_trade_report_commodity_entry_t>();
			entry->parent = this;
			auto cptr = entry.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
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
// BEGIN commodity::create
// END
}
std::unique_ptr<ui::element_base> make_market_trade_report_commodity(sys::state& state) {
	auto ptr = std::make_unique<market_trade_report_commodity_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result market_trade_report_trade_item_trade_item_c_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result market_trade_report_trade_item_trade_item_c_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void market_trade_report_trade_item_trade_item_c_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(x >= table_source->trade_flag_column_start && x < table_source->trade_flag_column_start + table_source->trade_flag_column_width) {
	}
	if(x >= table_source->trade_destination_column_start && x < table_source->trade_destination_column_start + table_source->trade_destination_column_width) {
	}
	if(x >= table_source->trade_distance_column_start && x < table_source->trade_distance_column_start + table_source->trade_distance_column_width) {
	}
	if(x >= table_source->trade_price_column_start && x < table_source->trade_price_column_start + table_source->trade_price_column_width) {
	}
	if(x >= table_source->trade_volume_column_start && x < table_source->trade_volume_column_start + table_source->trade_volume_column_width) {
	}
	if(x >= table_source->trade_value_column_start && x < table_source->trade_value_column_start + table_source->trade_value_column_width) {
	}
	if(x >= table_source->trade_empty_column_start && x < table_source->trade_empty_column_start + table_source->trade_empty_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void market_trade_report_trade_item_trade_item_c_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(x >=  table_source->trade_flag_column_start && x <  table_source->trade_flag_column_start +  table_source->trade_flag_column_width) {
	}
	if(x >=  table_source->trade_destination_column_start && x <  table_source->trade_destination_column_start +  table_source->trade_destination_column_width) {
	}
	if(x >=  table_source->trade_distance_column_start && x <  table_source->trade_distance_column_start +  table_source->trade_distance_column_width) {
	}
	if(x >=  table_source->trade_price_column_start && x <  table_source->trade_price_column_start +  table_source->trade_price_column_width) {
	}
	if(x >=  table_source->trade_volume_column_start && x <  table_source->trade_volume_column_start +  table_source->trade_volume_column_width) {
	market_trade_report_trade_item_t& trade_item = *((market_trade_report_trade_item_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN trade_item::trade_item_c::volume::column_tooltip
	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);
	auto route = state.world.get_trade_route_by_province_pair(mid, trade_item.other);
	economy::make_trade_volume_tooltip(state, contents, route, state.selected_trade_good, mid);
// END
	}
	if(x >=  table_source->trade_value_column_start && x <  table_source->trade_value_column_start +  table_source->trade_value_column_width) {
	}
	if(x >=  table_source->trade_empty_column_start && x <  table_source->trade_empty_column_start +  table_source->trade_empty_column_width) {
	}
}
void market_trade_report_trade_item_trade_item_c_t::set_flag_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(new_text !=  flag_cached_text) {
		flag_cached_text = new_text;
		flag_internal_layout.contents.clear();
		flag_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ flag_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_flag_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_flag_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, flag_cached_text);
		}
	} else {
	}
}
void market_trade_report_trade_item_trade_item_c_t::set_destination_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(new_text !=  destination_cached_text) {
		destination_cached_text = new_text;
		destination_internal_layout.contents.clear();
		destination_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ destination_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_destination_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_destination_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, destination_cached_text);
		}
	} else {
	}
}
void market_trade_report_trade_item_trade_item_c_t::set_distance_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(new_text !=  distance_cached_text) {
		distance_cached_text = new_text;
		distance_internal_layout.contents.clear();
		distance_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ distance_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_distance_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_distance_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, distance_cached_text);
		}
	auto font_size_factor = float(text::size_from_font_id(text::make_font_id(state, false, 1.0f * 16))) / (float((1 << 6) * 64.0f * text::magnification_factor));
	float temp_decimal_pos = -1.0f;
	float running_total = 0.0f;
	auto best_cluster = std::string::npos;
	auto found_decimal_pos =  distance_cached_text.find_last_of('.');	bool left_align = true == (state_is_rtl(state)); 
	for(auto& t : distance_internal_layout.contents) { 
		running_total = float(t.x);
		for(auto& ch : t.unicodechars.glyph_info) {
			if(found_decimal_pos <= size_t(ch.cluster) && size_t(ch.cluster) < best_cluster) {
				temp_decimal_pos = running_total ;
				best_cluster = size_t(ch.cluster);
			}
			running_total += ch.x_advance * font_size_factor;
		}
	} 
	if(best_cluster == std::string::npos) {
		running_total = 0.0f;
		temp_decimal_pos = -1000000.0f;
		for(auto& t : distance_internal_layout.contents) {
			running_total = float(t.x);
			for(auto& ch : t.unicodechars.glyph_info) {
				temp_decimal_pos = std::max(temp_decimal_pos, running_total);
				running_total += ch.x_advance * font_size_factor;
			}
		}
	}
	distance_decimal_pos = temp_decimal_pos;
	if(left_align)
		table_source->trade_distance_decimal_pos = std::min(distance_decimal_pos, table_source->trade_distance_decimal_pos);
	else
		table_source->trade_distance_decimal_pos = std::max(distance_decimal_pos, table_source->trade_distance_decimal_pos);
	} else {
	bool left_align = true == (state_is_rtl(state)); 
	if(left_align)
		table_source->trade_distance_decimal_pos = std::min(distance_decimal_pos, table_source->trade_distance_decimal_pos);
	else
		table_source->trade_distance_decimal_pos = std::max(distance_decimal_pos, table_source->trade_distance_decimal_pos);
	}
}
void market_trade_report_trade_item_trade_item_c_t::set_price_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(new_text !=  price_cached_text) {
		price_cached_text = new_text;
		price_internal_layout.contents.clear();
		price_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_price_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, price_cached_text);
		}
	} else {
	}
}
void market_trade_report_trade_item_trade_item_c_t::set_volume_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(new_text !=  volume_cached_text) {
		volume_cached_text = new_text;
		volume_internal_layout.contents.clear();
		volume_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ volume_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_volume_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_volume_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, volume_cached_text);
		}
	auto font_size_factor = float(text::size_from_font_id(text::make_font_id(state, false, 1.0f * 16))) / (float((1 << 6) * 64.0f * text::magnification_factor));
	float temp_decimal_pos = -1.0f;
	float running_total = 0.0f;
	auto best_cluster = std::string::npos;
	auto found_decimal_pos =  volume_cached_text.find_last_of('.');	bool left_align = true == (state_is_rtl(state)); 
	for(auto& t : volume_internal_layout.contents) { 
		running_total = float(t.x);
		for(auto& ch : t.unicodechars.glyph_info) {
			if(found_decimal_pos <= size_t(ch.cluster) && size_t(ch.cluster) < best_cluster) {
				temp_decimal_pos = running_total ;
				best_cluster = size_t(ch.cluster);
			}
			running_total += ch.x_advance * font_size_factor;
		}
	} 
	if(best_cluster == std::string::npos) {
		running_total = 0.0f;
		temp_decimal_pos = -1000000.0f;
		for(auto& t : volume_internal_layout.contents) {
			running_total = float(t.x);
			for(auto& ch : t.unicodechars.glyph_info) {
				temp_decimal_pos = std::max(temp_decimal_pos, running_total);
				running_total += ch.x_advance * font_size_factor;
			}
		}
	}
	volume_decimal_pos = temp_decimal_pos;
	if(left_align)
		table_source->trade_volume_decimal_pos = std::min(volume_decimal_pos, table_source->trade_volume_decimal_pos);
	else
		table_source->trade_volume_decimal_pos = std::max(volume_decimal_pos, table_source->trade_volume_decimal_pos);
	} else {
	bool left_align = true == (state_is_rtl(state)); 
	if(left_align)
		table_source->trade_volume_decimal_pos = std::min(volume_decimal_pos, table_source->trade_volume_decimal_pos);
	else
		table_source->trade_volume_decimal_pos = std::max(volume_decimal_pos, table_source->trade_volume_decimal_pos);
	}
}
void market_trade_report_trade_item_trade_item_c_t::set_value_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(new_text !=  value_cached_text) {
		value_cached_text = new_text;
		value_internal_layout.contents.clear();
		value_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ value_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_value_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_value_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, value_cached_text);
		}
	} else {
	}
}
void market_trade_report_trade_item_trade_item_c_t::set_empty_text(sys::state & state, std::string const& new_text) {
		auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(new_text !=  empty_cached_text) {
		empty_cached_text = new_text;
		empty_internal_layout.contents.clear();
		empty_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ empty_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_empty_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_empty_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, empty_cached_text);
		}
	} else {
	}
}
void market_trade_report_trade_item_trade_item_c_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	bool col_um_flag = rel_mouse_x >= table_source->trade_flag_column_start && rel_mouse_x < (table_source->trade_flag_column_start + table_source->trade_flag_column_width);
	if(!flag_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_flag , false, false); 
		for(auto& t : flag_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_flag_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, flag_text_color), cmod);
		}
	}
	bool col_um_destination = rel_mouse_x >= table_source->trade_destination_column_start && rel_mouse_x < (table_source->trade_destination_column_start + table_source->trade_destination_column_width);
	if(!destination_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_destination , false, false); 
		for(auto& t : destination_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_destination_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, destination_text_color), cmod);
		}
	}
	bool col_um_distance = rel_mouse_x >= table_source->trade_distance_column_start && rel_mouse_x < (table_source->trade_distance_column_start + table_source->trade_distance_column_width);
	if(!distance_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_distance , false, false); 
		for(auto& t : distance_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_distance_column_start + 8 + table_source->trade_distance_decimal_pos - distance_decimal_pos, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, distance_text_color), cmod);
		}
	}
	bool col_um_price = rel_mouse_x >= table_source->trade_price_column_start && rel_mouse_x < (table_source->trade_price_column_start + table_source->trade_price_column_width);
	if(!price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_price , false, false); 
		for(auto& t : price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_price_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, price_text_color), cmod);
		}
	}
	bool col_um_volume = rel_mouse_x >= table_source->trade_volume_column_start && rel_mouse_x < (table_source->trade_volume_column_start + table_source->trade_volume_column_width);
	if(!volume_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_volume , false, false); 
		for(auto& t : volume_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_volume_column_start + 8 + table_source->trade_volume_decimal_pos - volume_decimal_pos, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, volume_text_color), cmod);
		}
	}
	bool col_um_value = rel_mouse_x >= table_source->trade_value_column_start && rel_mouse_x < (table_source->trade_value_column_start + table_source->trade_value_column_width);
	if(!value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_value , false, false); 
		for(auto& t : value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_value_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, value_text_color), cmod);
		}
	}
	bool col_um_empty = rel_mouse_x >= table_source->trade_empty_column_start && rel_mouse_x < (table_source->trade_empty_column_start + table_source->trade_empty_column_width);
	if(!empty_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_empty , false, false); 
		for(auto& t : empty_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_empty_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, empty_text_color), cmod);
		}
	}
}
void market_trade_report_trade_item_trade_item_c_t::on_update(sys::state& state) noexcept {
	market_trade_report_trade_item_t& trade_item = *((market_trade_report_trade_item_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN trade_item::trade_item_c::update
	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	auto sid_other = state.world.market_get_zone_from_local_market(trade_item.other);
	//auto nid_other = state.world.state_instance_get_nation_from_state_ownership(sid_other);
	//auto niid_other = state.world.nation_get_identity_from_identity_holder(nid_other);
	//auto ii_other = state.world.national_identity_get_identifying_int(niid_other);
	//auto tag_other = nations::int_to_tag(ii_other);
	//auto flag_other = "@" + tag_other;
	set_flag_text(state, "");

	set_destination_text(state,
		text::produce_simple_string(
			state,
			state.world.province_get_name(state.world.state_instance_get_capital(sid_other))
		)
	);

	auto route = state.world.get_trade_route_by_province_pair(mid, trade_item.other);

	auto distance = state.world.trade_route_get_distance(route);
	set_distance_text(state, text::format_float(distance, 1));

	auto route_0 = state.world.trade_route_get_connected_markets(route, 0);

	float mult = 1.f;

	if(route_0 == trade_item.other) {
		mult = -1.f;
	}

	if(state.selected_trade_good) {
		auto volume = state.world.trade_route_get_volume(route, state.selected_trade_good);
		set_volume_text(state, text::format_float(mult * volume, 2));

		auto median_price = state.world.commodity_get_median_price(state.selected_trade_good);
		set_value_text(state, text::format_money(median_price * volume * mult));

		set_price_text(state, text::format_money(economy::price(state, trade_item.other, state.selected_trade_good)));
	} else {
		set_volume_text(state, "");

		float total = 0.f;
		state.world.for_each_commodity([&](auto cid) {
			auto median_price = state.world.commodity_get_median_price(cid);
			auto volume = state.world.trade_route_get_volume(route, cid);
			total += median_price * volume;
		});
		set_value_text(state, text::format_money(total * mult));

		set_price_text(state, "");
	}
// END
}
void market_trade_report_trade_item_trade_item_c_t::on_create(sys::state& state) noexcept {
// BEGIN trade_item::trade_item_c::create
// END
}
ui::message_result market_trade_report_trade_item_flag_real_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(std::holds_alternative<dcon::nation_id>(flag)) {
		if(std::get<dcon::nation_id>(flag) && state.world.nation_get_owned_province_count(std::get<dcon::nation_id>(flag)) > 0) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
			state.open_diplomacy(std::get<dcon::nation_id>(flag));
		} 
	} else if(std::holds_alternative<dcon::national_identity_id>(flag)) {
		auto n_temp = state.world.national_identity_get_nation_from_identity_holder(std::get<dcon::national_identity_id>(flag));
		if(n_temp && state.world.nation_get_owned_province_count(n_temp) > 0) {
			sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
			state.open_diplomacy(n_temp);
		} 
	} 
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_trade_item_flag_real_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void market_trade_report_trade_item_flag_real_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(std::holds_alternative<dcon::nation_id>(flag)) {
		text::add_line(state, contents, text::get_name(state, std::get<dcon::nation_id>(flag)));
	} else if(std::holds_alternative<dcon::national_identity_id>(flag)) {
		text::add_line(state, contents, nations::name_from_tag(state, std::get<dcon::national_identity_id>(flag)));
	} else if(std::holds_alternative<dcon::rebel_faction_id>(flag)) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, rebel::rebel_name(state, std::get<dcon::rebel_faction_id>(flag)));
		text::close_layout_box(contents, box);
	} 
}
void market_trade_report_trade_item_flag_real_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(std::holds_alternative<dcon::nation_id>(flag)) {
		dcon::government_flag_id flag_type = dcon::government_flag_id{};
		auto h_temp = state.world.nation_get_identity_from_identity_holder(std::get<dcon::nation_id>(flag));
		if(bool(std::get<dcon::nation_id>(flag)) && state.world.nation_get_owned_province_count(std::get<dcon::nation_id>(flag)) != 0) {
			flag_type = culture::get_current_flag_type(state, std::get<dcon::nation_id>(flag));
		} else {
			flag_type = culture::get_current_flag_type(state, h_temp);
		}
		auto flag_texture_handle = ogl::get_flag_handle(state, h_temp, flag_type);
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, base_data.get_rotation(), false,  false);
	} else if(std::holds_alternative<dcon::national_identity_id>(flag)) {
		dcon::government_flag_id flag_type = dcon::government_flag_id{};
		auto n_temp = state.world.national_identity_get_nation_from_identity_holder(std::get<dcon::national_identity_id>(flag));
		if(bool(n_temp) && state.world.nation_get_owned_province_count(n_temp) != 0) {
			flag_type = culture::get_current_flag_type(state, n_temp);
		} else {
			flag_type = culture::get_current_flag_type(state, std::get<dcon::national_identity_id>(flag));
		}
		auto flag_texture_handle = ogl::get_flag_handle(state, std::get<dcon::national_identity_id>(flag), flag_type);
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, base_data.get_rotation(), false,  false);
	} else if(std::holds_alternative<dcon::rebel_faction_id>(flag)) {
		dcon::rebel_faction_id rf_temp = std::get<dcon::rebel_faction_id>(flag);
		if(state.world.rebel_faction_get_type(rf_temp).get_independence() != uint8_t(culture::rebel_independence::none) && state.world.rebel_faction_get_defection_target(rf_temp)) {
			dcon::government_flag_id flag_type = dcon::government_flag_id{};
			auto h_temp = state.world.rebel_faction_get_defection_target(rf_temp);
			auto n_temp = state.world.national_identity_get_nation_from_identity_holder(h_temp);
			if(bool(n_temp) && state.world.nation_get_owned_province_count(n_temp) != 0) {
				flag_type = culture::get_current_flag_type(state, n_temp);
			} else {
				flag_type = culture::get_current_flag_type(state, h_temp);
			}
			auto flag_texture_handle = ogl::get_flag_handle(state, h_temp, flag_type);
			ogl::render_textured_rect(state, ui::get_color_modification(false, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, base_data.get_rotation(), false,  false);
			ogl::render_textured_rect(state, ui::get_color_modification(false, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_rebel_flag_overlay(state), base_data.get_rotation(), false,  false);
		return;
		}
		ogl::render_textured_rect(state, ui::get_color_modification(false, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_rebel_flag_handle(state, rf_temp), base_data.get_rotation(), false,  false);
	} 
}
void market_trade_report_trade_item_flag_real_t::on_update(sys::state& state) noexcept {
	market_trade_report_trade_item_t& trade_item = *((market_trade_report_trade_item_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN trade_item::flag_real::update
	auto sid_other = state.world.market_get_zone_from_local_market(trade_item.other);
	flag = state.world.state_instance_get_nation_from_state_ownership(sid_other);
// END
}
void market_trade_report_trade_item_flag_real_t::on_create(sys::state& state) noexcept {
// BEGIN trade_item::flag_real::create
// END
}
ui::message_result market_trade_report_trade_item_select_market_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	market_trade_report_trade_item_t& trade_item = *((market_trade_report_trade_item_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN trade_item::select_market::lbutton_action
	auto sid_other = state.world.market_get_zone_from_local_market(trade_item.other);
	auto pid_other = state.world.state_instance_get_capital(sid_other);
	state.set_selected_province(pid_other);
	state.map_state.center_map_on_province(state, state.map_state.get_selected_province());
	state.update_trade_flow.store(true, std::memory_order::release);
	if(state.ui_state.province_window) {
		state.ui_state.province_window->impl_on_update(state);
	}
	body.on_update(state);
// END
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_trade_item_select_market_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_trade_report_trade_item_select_market_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void market_trade_report_trade_item_select_market_t::on_update(sys::state& state) noexcept {
	market_trade_report_trade_item_t& trade_item = *((market_trade_report_trade_item_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN trade_item::select_market::update
// END
}
void market_trade_report_trade_item_select_market_t::on_create(sys::state& state) noexcept {
// BEGIN trade_item::select_market::create
// END
}
ui::message_result market_trade_report_trade_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result market_trade_report_trade_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_trade_report_trade_item_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
	auto table_source = (market_trade_report_body_t*)(parent);
	auto under_mouse = [&](){auto p = state.ui_state.under_mouse; while(p){ if(p == this) return true; p = p->parent; } return false;}();
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_flag = rel_mouse_x >= table_source->trade_flag_column_start && rel_mouse_x < (table_source->trade_flag_column_start + table_source->trade_flag_column_width);
	if(col_um_flag && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_flag_column_start), float(y), float(table_source->trade_flag_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_destination = rel_mouse_x >= table_source->trade_destination_column_start && rel_mouse_x < (table_source->trade_destination_column_start + table_source->trade_destination_column_width);
	if(col_um_destination && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_destination_column_start), float(y), float(table_source->trade_destination_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_distance = rel_mouse_x >= table_source->trade_distance_column_start && rel_mouse_x < (table_source->trade_distance_column_start + table_source->trade_distance_column_width);
	if(col_um_distance && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_distance_column_start), float(y), float(table_source->trade_distance_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_price = rel_mouse_x >= table_source->trade_price_column_start && rel_mouse_x < (table_source->trade_price_column_start + table_source->trade_price_column_width);
	if(col_um_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_price_column_start), float(y), float(table_source->trade_price_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_volume = rel_mouse_x >= table_source->trade_volume_column_start && rel_mouse_x < (table_source->trade_volume_column_start + table_source->trade_volume_column_width);
	if(col_um_volume && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_volume_column_start), float(y), float(table_source->trade_volume_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_value = rel_mouse_x >= table_source->trade_value_column_start && rel_mouse_x < (table_source->trade_value_column_start + table_source->trade_value_column_width);
	if(col_um_value && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_value_column_start), float(y), float(table_source->trade_value_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_empty = rel_mouse_x >= table_source->trade_empty_column_start && rel_mouse_x < (table_source->trade_empty_column_start + table_source->trade_empty_column_width);
	if(col_um_empty && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_empty_column_start), float(y), float(table_source->trade_empty_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
}
void market_trade_report_trade_item_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN trade_item::update
// END
	remake_layout(state, true);
}
void market_trade_report_trade_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(80);
		lvl.page_controls->base_data.size.y = int16_t(16);
	}
	auto optional_section = buffer.read_section(); // nothing
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
				if(cname == "trade_item_c") {
					temp.ptr = trade_item_c.get();
				}
				if(cname == "flag_real") {
					temp.ptr = flag_real.get();
				}
				if(cname == "select_market") {
					temp.ptr = select_market.get();
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
				if(cname == "body") {
					temp.ptr = make_market_trade_report_body(state);
				}
				if(cname == "commodity") {
					temp.ptr = make_market_trade_report_commodity(state);
				}
				if(cname == "trade_item") {
					temp.ptr = make_market_trade_report_trade_item(state);
				}
				if(cname == "nation_data") {
					temp.ptr = make_market_trade_report_nation_data(state);
				}
				if(cname == "trade_header") {
					temp.ptr = make_market_trade_report_trade_header(state);
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
void market_trade_report_trade_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_trade_report::trade_item"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	alt_texture_key = win_data.alt_texture;
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "trade_item_c") {
			trade_item_c = std::make_unique<market_trade_report_trade_item_trade_item_c_t>();
			trade_item_c->parent = this;
			auto cptr = trade_item_c.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "flag_real") {
			flag_real = std::make_unique<market_trade_report_trade_item_flag_real_t>();
			flag_real->parent = this;
			auto cptr = flag_real.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "select_market") {
			select_market = std::make_unique<market_trade_report_trade_item_select_market_t>();
			select_market->parent = this;
			auto cptr = select_market.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
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
// BEGIN trade_item::create
// END
}
std::unique_ptr<ui::element_base> make_market_trade_report_trade_item(sys::state& state) {
	auto ptr = std::make_unique<market_trade_report_trade_item_t>();
	ptr->on_create(state);
	return ptr;
}
void market_trade_report_nation_data_nation_name_t::on_update(sys::state& state) noexcept {
	market_trade_report_nation_data_t& nation_data = *((market_trade_report_nation_data_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent->parent)); 
// BEGIN nation_data::nation_name::update
// END
}
void market_trade_report_nation_data_nation_name_t::on_create(sys::state& state) noexcept {
// BEGIN nation_data::nation_name::create
// END
}
void market_trade_report_nation_data_flag_t::on_update(sys::state& state) noexcept {
	market_trade_report_nation_data_t& nation_data = *((market_trade_report_nation_data_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent->parent)); 
// BEGIN nation_data::flag::update
// END
}
void market_trade_report_nation_data_flag_t::on_create(sys::state& state) noexcept {
// BEGIN nation_data::flag::create
// END
}
void market_trade_report_nation_data_value_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void market_trade_report_nation_data_value_t::on_reset_text(sys::state& state) noexcept {
}
void market_trade_report_nation_data_value_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void market_trade_report_nation_data_value_t::on_update(sys::state& state) noexcept {
	market_trade_report_nation_data_t& nation_data = *((market_trade_report_nation_data_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent->parent)); 
// BEGIN nation_data::value::update
// END
}
void market_trade_report_nation_data_value_t::on_create(sys::state& state) noexcept {
// BEGIN nation_data::value::create
// END
}
void market_trade_report_nation_data_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void market_trade_report_nation_data_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN nation_data::update
// END
	remake_layout(state, true);
}
void market_trade_report_nation_data_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(80);
		lvl.page_controls->base_data.size.y = int16_t(16);
	}
	auto optional_section = buffer.read_section(); // nothing
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
				if(cname == "nation_name") {
					temp.ptr = nation_name.get();
				}
				if(cname == "flag") {
					temp.ptr = flag.get();
				}
				if(cname == "value") {
					temp.ptr = value.get();
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
				if(cname == "body") {
					temp.ptr = make_market_trade_report_body(state);
				}
				if(cname == "commodity") {
					temp.ptr = make_market_trade_report_commodity(state);
				}
				if(cname == "trade_item") {
					temp.ptr = make_market_trade_report_trade_item(state);
				}
				if(cname == "nation_data") {
					temp.ptr = make_market_trade_report_nation_data(state);
				}
				if(cname == "trade_header") {
					temp.ptr = make_market_trade_report_trade_header(state);
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
void market_trade_report_nation_data_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_trade_report::nation_data"));
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
		if(child_data.name == "nation_name") {
			nation_name = std::make_unique<market_trade_report_nation_data_nation_name_t>();
			nation_name->parent = this;
			auto cptr = nation_name.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "flag") {
			flag = std::make_unique<market_trade_report_nation_data_flag_t>();
			flag->parent = this;
			auto cptr = flag.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "value") {
			value = std::make_unique<market_trade_report_nation_data_value_t>();
			value->parent = this;
			auto cptr = value.get();
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
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN nation_data::create
// END
}
std::unique_ptr<ui::element_base> make_market_trade_report_nation_data(sys::state& state) {
	auto ptr = std::make_unique<market_trade_report_nation_data_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result market_trade_report_trade_header_trade_header_c_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(x >= table_source->trade_distance_column_start && x < table_source->trade_distance_column_start + table_source->trade_distance_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->trade_distance_sort_direction;
		table_source->trade_distance_sort_direction = 0;
		table_source->trade_price_sort_direction = 0;
		table_source->trade_volume_sort_direction = 0;
		table_source->trade_value_sort_direction = 0;
		table_source->trade_distance_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->trade_price_column_start && x < table_source->trade_price_column_start + table_source->trade_price_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->trade_price_sort_direction;
		table_source->trade_distance_sort_direction = 0;
		table_source->trade_price_sort_direction = 0;
		table_source->trade_volume_sort_direction = 0;
		table_source->trade_value_sort_direction = 0;
		table_source->trade_price_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->trade_volume_column_start && x < table_source->trade_volume_column_start + table_source->trade_volume_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->trade_volume_sort_direction;
		table_source->trade_distance_sort_direction = 0;
		table_source->trade_price_sort_direction = 0;
		table_source->trade_volume_sort_direction = 0;
		table_source->trade_value_sort_direction = 0;
		table_source->trade_volume_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->trade_value_column_start && x < table_source->trade_value_column_start + table_source->trade_value_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->trade_value_sort_direction;
		table_source->trade_distance_sort_direction = 0;
		table_source->trade_price_sort_direction = 0;
		table_source->trade_volume_sort_direction = 0;
		table_source->trade_value_sort_direction = 0;
		table_source->trade_value_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result market_trade_report_trade_header_trade_header_c_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void market_trade_report_trade_header_trade_header_c_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(x >= table_source->trade_flag_column_start && x < table_source->trade_flag_column_start + table_source->trade_flag_column_width) {
	}
	if(x >= table_source->trade_destination_column_start && x < table_source->trade_destination_column_start + table_source->trade_destination_column_width) {
	}
	if(x >= table_source->trade_distance_column_start && x < table_source->trade_distance_column_start + table_source->trade_distance_column_width) {
	}
	if(x >= table_source->trade_price_column_start && x < table_source->trade_price_column_start + table_source->trade_price_column_width) {
	}
	if(x >= table_source->trade_volume_column_start && x < table_source->trade_volume_column_start + table_source->trade_volume_column_width) {
	}
	if(x >= table_source->trade_value_column_start && x < table_source->trade_value_column_start + table_source->trade_value_column_width) {
	}
	if(x >= table_source->trade_empty_column_start && x < table_source->trade_empty_column_start + table_source->trade_empty_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void market_trade_report_trade_header_trade_header_c_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	if(x >=  table_source->trade_flag_column_start && x <  table_source->trade_flag_column_start +  table_source->trade_flag_column_width) {
	}
	if(x >=  table_source->trade_destination_column_start && x <  table_source->trade_destination_column_start +  table_source->trade_destination_column_width) {
	}
	if(x >=  table_source->trade_distance_column_start && x <  table_source->trade_distance_column_start +  table_source->trade_distance_column_width) {
	}
	if(x >=  table_source->trade_price_column_start && x <  table_source->trade_price_column_start +  table_source->trade_price_column_width) {
	}
	if(x >=  table_source->trade_volume_column_start && x <  table_source->trade_volume_column_start +  table_source->trade_volume_column_width) {
	}
	if(x >=  table_source->trade_value_column_start && x <  table_source->trade_value_column_start +  table_source->trade_value_column_width) {
	}
	if(x >=  table_source->trade_empty_column_start && x <  table_source->trade_empty_column_start +  table_source->trade_empty_column_width) {
	}
}
void market_trade_report_trade_header_trade_header_c_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	{
	destination_cached_text = text::produce_simple_string(state, table_source->trade_destination_header_text_key);
	 destination_internal_layout.contents.clear();
	 destination_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  destination_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_destination_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_destination_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, destination_cached_text);
	}
	{
	distance_cached_text = text::produce_simple_string(state, table_source->trade_distance_header_text_key);
	 distance_internal_layout.contents.clear();
	 distance_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  distance_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_distance_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_distance_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, distance_cached_text);
	}
	{
	price_cached_text = text::produce_simple_string(state, table_source->trade_price_header_text_key);
	 price_internal_layout.contents.clear();
	 price_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  price_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_price_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_price_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, price_cached_text);
	}
	{
	volume_cached_text = text::produce_simple_string(state, table_source->trade_volume_header_text_key);
	 volume_internal_layout.contents.clear();
	 volume_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  volume_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_volume_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_volume_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, volume_cached_text);
	}
	{
	value_cached_text = text::produce_simple_string(state, table_source->trade_value_header_text_key);
	 value_internal_layout.contents.clear();
	 value_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  value_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->trade_value_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->trade_value_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, value_cached_text);
	}
}
void market_trade_report_trade_header_trade_header_c_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (market_trade_report_body_t*)(parent->parent);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	bool col_um_flag = rel_mouse_x >= table_source->trade_flag_column_start && rel_mouse_x < (table_source->trade_flag_column_start + table_source->trade_flag_column_width);
	bool col_um_destination = rel_mouse_x >= table_source->trade_destination_column_start && rel_mouse_x < (table_source->trade_destination_column_start + table_source->trade_destination_column_width);
	if(!destination_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_destination , false, false); 
		for(auto& t : destination_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_destination_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->trade_destination_header_text_color), cmod);
		}
	}
	bool col_um_distance = rel_mouse_x >= table_source->trade_distance_column_start && rel_mouse_x < (table_source->trade_distance_column_start + table_source->trade_distance_column_width);
	if(table_source->trade_distance_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_distance, false, true), float(x + table_source->trade_distance_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_ascending_icon, table_source->trade_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->trade_distance_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_distance, false, true), float(x + table_source->trade_distance_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_descending_icon, table_source->trade_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!distance_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_distance , false, true); 
		for(auto& t : distance_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_distance_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->trade_distance_header_text_color), cmod);
		}
	}
	bool col_um_price = rel_mouse_x >= table_source->trade_price_column_start && rel_mouse_x < (table_source->trade_price_column_start + table_source->trade_price_column_width);
	if(table_source->trade_price_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_price, false, true), float(x + table_source->trade_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_ascending_icon, table_source->trade_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->trade_price_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_price, false, true), float(x + table_source->trade_price_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_descending_icon, table_source->trade_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!price_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_price , false, true); 
		for(auto& t : price_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_price_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->trade_price_header_text_color), cmod);
		}
	}
	bool col_um_volume = rel_mouse_x >= table_source->trade_volume_column_start && rel_mouse_x < (table_source->trade_volume_column_start + table_source->trade_volume_column_width);
	if(table_source->trade_volume_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_volume, false, true), float(x + table_source->trade_volume_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_ascending_icon, table_source->trade_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->trade_volume_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_volume, false, true), float(x + table_source->trade_volume_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_descending_icon, table_source->trade_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!volume_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_volume , false, true); 
		for(auto& t : volume_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_volume_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->trade_volume_header_text_color), cmod);
		}
	}
	bool col_um_value = rel_mouse_x >= table_source->trade_value_column_start && rel_mouse_x < (table_source->trade_value_column_start + table_source->trade_value_column_width);
	if(table_source->trade_value_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_value, false, true), float(x + table_source->trade_value_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_ascending_icon, table_source->trade_ascending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(table_source->trade_value_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_value, false, true), float(x + table_source->trade_value_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), ogl::get_late_load_texture_handle(state, table_source->trade_descending_icon, table_source->trade_descending_icon_key), ui::rotation::upright, false, state_is_rtl(state));
	}
	if(!value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_value , false, true); 
		for(auto& t : value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->trade_value_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->trade_value_header_text_color), cmod);
		}
	}
	bool col_um_empty = rel_mouse_x >= table_source->trade_empty_column_start && rel_mouse_x < (table_source->trade_empty_column_start + table_source->trade_empty_column_width);
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), table_source->trade_divider_color.r, table_source->trade_divider_color.g, table_source->trade_divider_color.b, 1.0f);
}
void market_trade_report_trade_header_trade_header_c_t::on_update(sys::state& state) noexcept {
	market_trade_report_trade_header_t& trade_header = *((market_trade_report_trade_header_t*)(parent)); 
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN trade_header::trade_header_c::update
// END
}
void market_trade_report_trade_header_trade_header_c_t::on_create(sys::state& state) noexcept {
// BEGIN trade_header::trade_header_c::create
// END
}
void market_trade_report_trade_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(8), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
	auto table_source = (market_trade_report_body_t*)(parent);
	auto under_mouse = [&](){auto p = state.ui_state.under_mouse; while(p){ if(p == this) return true; p = p->parent; } return false;}();
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_flag = rel_mouse_x >= table_source->trade_flag_column_start && rel_mouse_x < (table_source->trade_flag_column_start + table_source->trade_flag_column_width);
	if(col_um_flag && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_flag_column_start), float(y), float(table_source->trade_flag_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_destination = rel_mouse_x >= table_source->trade_destination_column_start && rel_mouse_x < (table_source->trade_destination_column_start + table_source->trade_destination_column_width);
	if(col_um_destination && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_destination_column_start), float(y), float(table_source->trade_destination_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_distance = rel_mouse_x >= table_source->trade_distance_column_start && rel_mouse_x < (table_source->trade_distance_column_start + table_source->trade_distance_column_width);
	if(col_um_distance && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_distance_column_start), float(y), float(table_source->trade_distance_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_price = rel_mouse_x >= table_source->trade_price_column_start && rel_mouse_x < (table_source->trade_price_column_start + table_source->trade_price_column_width);
	if(col_um_price && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_price_column_start), float(y), float(table_source->trade_price_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_volume = rel_mouse_x >= table_source->trade_volume_column_start && rel_mouse_x < (table_source->trade_volume_column_start + table_source->trade_volume_column_width);
	if(col_um_volume && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_volume_column_start), float(y), float(table_source->trade_volume_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_value = rel_mouse_x >= table_source->trade_value_column_start && rel_mouse_x < (table_source->trade_value_column_start + table_source->trade_value_column_width);
	if(col_um_value && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_value_column_start), float(y), float(table_source->trade_value_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
	bool col_um_empty = rel_mouse_x >= table_source->trade_empty_column_start && rel_mouse_x < (table_source->trade_empty_column_start + table_source->trade_empty_column_width);
	if(col_um_empty && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->trade_empty_column_start), float(y), float(table_source->trade_empty_column_width), float(base_data.size.y), 0.949020f, 0.505882f, 0.505882f, 0.611765f);
	}
}
void market_trade_report_trade_header_t::on_update(sys::state& state) noexcept {
	market_trade_report_body_t& body = *((market_trade_report_body_t*)(parent->parent)); 
// BEGIN trade_header::update
// END
	remake_layout(state, true);
}
void market_trade_report_trade_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(80);
		lvl.page_controls->base_data.size.y = int16_t(16);
	}
	auto optional_section = buffer.read_section(); // nothing
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
				if(cname == "trade_header_c") {
					temp.ptr = trade_header_c.get();
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
				if(cname == "body") {
					temp.ptr = make_market_trade_report_body(state);
				}
				if(cname == "commodity") {
					temp.ptr = make_market_trade_report_commodity(state);
				}
				if(cname == "trade_item") {
					temp.ptr = make_market_trade_report_trade_item(state);
				}
				if(cname == "nation_data") {
					temp.ptr = make_market_trade_report_nation_data(state);
				}
				if(cname == "trade_header") {
					temp.ptr = make_market_trade_report_trade_header(state);
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
void market_trade_report_trade_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("market_trade_report::trade_header"));
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
		if(child_data.name == "trade_header_c") {
			trade_header_c = std::make_unique<market_trade_report_trade_header_trade_header_c_t>();
			trade_header_c->parent = this;
			auto cptr = trade_header_c.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
// BEGIN trade_header::create
// END
}
std::unique_ptr<ui::element_base> make_market_trade_report_trade_header(sys::state& state) {
	auto ptr = std::make_unique<market_trade_report_trade_header_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
