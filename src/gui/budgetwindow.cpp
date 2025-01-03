namespace alice_ui {
struct budgetwindow_main_close_button_t;
struct budgetwindow_main_title_t;
struct budgetwindow_main_income_label_t;
struct budgetwindow_main_income_amount_t;
struct budgetwindow_main_income_page_left_t;
struct budgetwindow_main_income_page_right_t;
struct budgetwindow_main_income_page_number_t;
struct budgetwindow_main_income_table_t;
struct budgetwindow_main_expenses_page_left_t;
struct budgetwindow_main_expenses_page_number_t;
struct budgetwindow_main_expenses_page_right_t;
struct budgetwindow_main_expenses_label_t;
struct budgetwindow_main_expenses_amount_t;
struct budgetwindow_main_expenses_table_t;
struct budgetwindow_main_t;
struct budgetwindow_section_header_label_t;
struct budgetwindow_section_header_llbutton_t;
struct budgetwindow_section_header_lbutton_t;
struct budgetwindow_section_header_rbutton_t;
struct budgetwindow_section_header_rrbutton_t;
struct budgetwindow_section_header_setting_amount_t;
struct budgetwindow_section_header_expand_button_t;
struct budgetwindow_section_header_total_amount_t;
struct budgetwindow_section_header_min_setting_t;
struct budgetwindow_section_header_max_setting_t;
struct budgetwindow_section_header_t;
struct budgetwindow_main_close_button_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct budgetwindow_main_title_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 2.500000f; 
	bool text_is_header = true; 
	text::alignment text_alignment = text::alignment::center;
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
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_income_label_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 2.000000f; 
	bool text_is_header = true; 
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
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_income_amount_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 2.000000f; 
	bool text_is_header = true; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_income_page_left_t : public ui::element_base {
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
struct budgetwindow_main_income_page_right_t : public ui::element_base {
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
struct budgetwindow_main_income_page_number_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
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
struct budgetwindow_main_income_table_header_t : public ui::element_base {
	inline static int16_t lead_space_column_start = 0;
	inline static int16_t lead_space_column_width = 0;
	text::layout item_name_internal_layout;
	inline static text::text_color  item_name_text_color = text::text_color::black;
	std::string item_name_cached_text;
	inline static dcon::text_key item_name_text_key;
	inline static int8_t item_name_sort_direction = 0;
	inline static int16_t item_name_column_start = 0;
	inline static int16_t item_name_column_width = 0;
	text::layout item_value_internal_layout;
	inline static text::text_color  item_value_text_color = text::text_color::black;
	std::string item_value_cached_text;
	inline static dcon::text_key item_value_text_key;
	inline static int8_t item_value_sort_direction = 0;
	inline static int16_t item_value_column_start = 0;
	inline static int16_t item_value_column_width = 0;
	inline static std::string_view ascending_icon_key;
	inline static dcon::texture_id ascending_icon;
	inline static std::string_view descending_icon_key;
	inline static dcon::texture_id descending_icon;
	void on_reset_text(sys::state & state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	void on_create(sys::state& state) noexcept override;
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
};
struct budgetwindow_main_income_table_row_t : public ui::element_base {
	std::pair<std::string, float> value;
	bool alternate_row = false;
	text::layout item_name_internal_layout;
	inline static text::text_color  item_name_default_text_color = text::text_color::black;
	text::text_color  item_name_text_color = item_name_default_text_color ;
	std::string item_name_cached_text;
	inline static text::alignment item_name_text_alignment = text::alignment::right;
	void set_item_name_text(sys::state & state, std::string const& new_text);
	text::layout item_value_internal_layout;
	inline static text::text_color  item_value_default_text_color = text::text_color::black;
	text::text_color  item_value_text_color = item_value_default_text_color ;
	std::string item_value_cached_text;
	inline static text::alignment item_value_text_alignment = text::alignment::right;
	void set_item_value_text(sys::state & state, std::string const& new_text);
	inline static std::string_view row_texture_key;
	inline static dcon::texture_id row_background_texture;
	inline static std::string_view alt_row_texture_key;
	inline static dcon::texture_id alt_row_background_texture;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	void on_create(sys::state& state) noexcept override;
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
};
struct budgetwindow_main_income_table_t : public ui::non_owning_container_base {
	struct section_header_option { int32_t section_type; };
	std::vector<std::unique_ptr<ui::element_base>> section_header_pool;
	std::vector<std::unique_ptr<budgetwindow_main_income_table_row_t>> row_pool;
	std::vector<std::unique_ptr<budgetwindow_main_income_table_header_t>> header_pool;
	struct value_option { std::pair<std::string, float> value; };
	int32_t page = 0;
	inline static float row_height = 0;
	inline static ogl::color4f table_highlight_color{float(1.000000), float(1.000000), float(1.000000), float(0.156863)};
	inline static ogl::color3f table_divider_color{float(0.000000), float(0.000000), float(0.000000)};
	std::vector<std::variant<std::monostate, value_option, section_header_option>> values;
	std::vector<int32_t> page_starts;
	bool open_page = false;
	int16_t layout_space = 0;
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
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_scroll(sys::state & state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override; 
	void change_page(sys::state & state, int32_t new_page); 
	int32_t max_page(); 
	void clear_table(); 
	void impl_change_page(sys::state & state, int32_t new_page); 
	void add_value(std::pair<std::string, float> const& v); 
	void add_insert_section_header(sys::state& state, int32_t const& section_type); 
	std::unique_ptr<budgetwindow_main_income_table_row_t> make_row(sys::state& state);
	std::unique_ptr<budgetwindow_main_income_table_header_t> make_headers(sys::state& state);
	void apply_sort(sys::state& state); 
	void impl_apply_sort(sys::state& state); 
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_expenses_page_left_t : public ui::element_base {
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
struct budgetwindow_main_expenses_page_number_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
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
struct budgetwindow_main_expenses_page_right_t : public ui::element_base {
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
struct budgetwindow_main_expenses_label_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 2.000000f; 
	bool text_is_header = true; 
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
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_expenses_amount_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 2.000000f; 
	bool text_is_header = true; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_expenses_table_header_t : public ui::element_base {
	inline static int16_t lead_space_column_start = 0;
	inline static int16_t lead_space_column_width = 0;
	text::layout item_name_internal_layout;
	inline static text::text_color  item_name_text_color = text::text_color::black;
	std::string item_name_cached_text;
	inline static dcon::text_key item_name_text_key;
	inline static int8_t item_name_sort_direction = 0;
	inline static int16_t item_name_column_start = 0;
	inline static int16_t item_name_column_width = 0;
	text::layout item_value_internal_layout;
	inline static text::text_color  item_value_text_color = text::text_color::black;
	std::string item_value_cached_text;
	inline static dcon::text_key item_value_text_key;
	inline static int8_t item_value_sort_direction = 0;
	inline static int16_t item_value_column_start = 0;
	inline static int16_t item_value_column_width = 0;
	inline static std::string_view ascending_icon_key;
	inline static dcon::texture_id ascending_icon;
	inline static std::string_view descending_icon_key;
	inline static dcon::texture_id descending_icon;
	void on_reset_text(sys::state & state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	void on_create(sys::state& state) noexcept override;
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
};
struct budgetwindow_main_expenses_table_row_t : public ui::element_base {
	std::pair<std::string, float> value;
	bool alternate_row = false;
	text::layout item_name_internal_layout;
	inline static text::text_color  item_name_default_text_color = text::text_color::black;
	text::text_color  item_name_text_color = item_name_default_text_color ;
	std::string item_name_cached_text;
	inline static text::alignment item_name_text_alignment = text::alignment::right;
	void set_item_name_text(sys::state & state, std::string const& new_text);
	text::layout item_value_internal_layout;
	inline static text::text_color  item_value_default_text_color = text::text_color::black;
	text::text_color  item_value_text_color = item_value_default_text_color ;
	std::string item_value_cached_text;
	inline static text::alignment item_value_text_alignment = text::alignment::right;
	void set_item_value_text(sys::state & state, std::string const& new_text);
	inline static std::string_view row_texture_key;
	inline static dcon::texture_id row_background_texture;
	inline static std::string_view alt_row_texture_key;
	inline static dcon::texture_id alt_row_background_texture;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	void on_create(sys::state& state) noexcept override;
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
};
struct budgetwindow_main_expenses_table_t : public ui::non_owning_container_base {
	struct section_header_option { int32_t section_type; };
	std::vector<std::unique_ptr<ui::element_base>> section_header_pool;
	std::vector<std::unique_ptr<budgetwindow_main_expenses_table_row_t>> row_pool;
	std::vector<std::unique_ptr<budgetwindow_main_expenses_table_header_t>> header_pool;
	struct value_option { std::pair<std::string, float> value; };
	int32_t page = 0;
	inline static float row_height = 0;
	inline static ogl::color4f table_highlight_color{float(1.000000), float(1.000000), float(1.000000), float(0.156863)};
	inline static ogl::color3f table_divider_color{float(0.000000), float(0.000000), float(0.000000)};
	std::vector<std::variant<std::monostate, value_option, section_header_option>> values;
	std::vector<int32_t> page_starts;
	bool open_page = false;
	int16_t layout_space = 0;
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
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_scroll(sys::state & state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override; 
	void change_page(sys::state & state, int32_t new_page); 
	int32_t max_page(); 
	void clear_table(); 
	void impl_change_page(sys::state & state, int32_t new_page); 
	void add_value(std::pair<std::string, float> const& v); 
	void add_insert_section_header(sys::state& state, int32_t const& section_type); 
	std::unique_ptr<budgetwindow_main_expenses_table_row_t> make_row(sys::state& state);
	std::unique_ptr<budgetwindow_main_expenses_table_header_t> make_headers(sys::state& state);
	void apply_sort(sys::state& state); 
	void impl_apply_sort(sys::state& state); 
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_section_header_label_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.500000f; 
	bool text_is_header = true; 
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
struct budgetwindow_section_header_llbutton_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct budgetwindow_section_header_lbutton_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct budgetwindow_section_header_rbutton_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct budgetwindow_section_header_rrbutton_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct budgetwindow_section_header_setting_amount_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
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
struct budgetwindow_section_header_expand_button_t : public ui::element_base {
	std::string_view texture_key;
	std::string_view alt_texture_key;
	dcon::texture_id alt_background_texture;
	bool is_active = false;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct budgetwindow_section_header_total_amount_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
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
struct budgetwindow_section_header_min_setting_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::left;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
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
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_section_header_max_setting_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::left;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
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
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_t : public ui::container_base {
	budgetwindow_main_close_button_t* close_button = nullptr;
	budgetwindow_main_title_t* title = nullptr;
	budgetwindow_main_income_label_t* income_label = nullptr;
	budgetwindow_main_income_amount_t* income_amount = nullptr;
	budgetwindow_main_income_page_left_t* income_page_left = nullptr;
	budgetwindow_main_income_page_right_t* income_page_right = nullptr;
	budgetwindow_main_income_page_number_t* income_page_number = nullptr;
	budgetwindow_main_income_table_t* income_table = nullptr;
	budgetwindow_main_expenses_page_left_t* expenses_page_left = nullptr;
	budgetwindow_main_expenses_page_number_t* expenses_page_number = nullptr;
	budgetwindow_main_expenses_page_right_t* expenses_page_right = nullptr;
	budgetwindow_main_expenses_label_t* expenses_label = nullptr;
	budgetwindow_main_expenses_amount_t* expenses_amount = nullptr;
	budgetwindow_main_expenses_table_t* expenses_table = nullptr;
	std::string_view texture_key;
	dcon::texture_id background_texture;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return (type == ui::mouse_probe_type::scroll ? ui::message_result::unseen : ui::message_result::consumed);
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
			if(state.world.locale_get_native_rtl(state.font_collection.get_current_locale())) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_budgetwindow_main(sys::state& state);
struct budgetwindow_section_header_t : public ui::container_base {
	int32_t section_type;
	budgetwindow_section_header_label_t* label = nullptr;
	budgetwindow_section_header_llbutton_t* llbutton = nullptr;
	budgetwindow_section_header_lbutton_t* lbutton = nullptr;
	budgetwindow_section_header_rbutton_t* rbutton = nullptr;
	budgetwindow_section_header_rrbutton_t* rrbutton = nullptr;
	budgetwindow_section_header_setting_amount_t* setting_amount = nullptr;
	budgetwindow_section_header_expand_button_t* expand_button = nullptr;
	budgetwindow_section_header_total_amount_t* total_amount = nullptr;
	budgetwindow_section_header_min_setting_t* min_setting = nullptr;
	budgetwindow_section_header_max_setting_t* max_setting = nullptr;
	std::string_view texture_key;
	dcon::texture_id background_texture;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return (type == ui::mouse_probe_type::scroll ? ui::message_result::unseen : ui::message_result::consumed);
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name) noexcept override {
		if(name == "section_type") {
			return (void*)(&section_type);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_budgetwindow_section_header(sys::state& state);
ui::message_result budgetwindow_main_close_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::close_button::lbutton_action
	parent->set_visible(state, false);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_main_close_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_main_close_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void budgetwindow_main_close_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_main_close_button_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::close_button::update
// END
}
void budgetwindow_main_close_button_t::on_create(sys::state& state) noexcept {
// BEGIN main::close_button::create
// END
}
void budgetwindow_main_title_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_title_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_title_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_title_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::title::update
// END
}
void budgetwindow_main_title_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::title::create
// END
}
void budgetwindow_main_income_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_income_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_income_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_income_label_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_label::update
// END
}
void budgetwindow_main_income_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::income_label::create
// END
}
void budgetwindow_main_income_amount_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_income_amount_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_main_income_amount_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_income_amount_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_amount::update
	float total = 0.0f;
	total += economy::estimate_diplomatic_income(state, state.local_player_nation);
	total += economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::poor) * float(state.world.nation_get_poor_tax(state.local_player_nation)) / 100.0f;
	total += economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::middle) * float(state.world.nation_get_middle_tax(state.local_player_nation)) / 100.0f;
	total += economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::rich) * float(state.world.nation_get_rich_tax(state.local_player_nation)) / 100.0f;
	total += economy::estimate_tariff_income(state, state.local_player_nation) * float(state.world.nation_get_tariffs(state.local_player_nation)) / 100.0f;
	total += economy::estimate_gold_income(state, state.local_player_nation);
	set_text(state, text::prettify_currency(total));
// END
}
void budgetwindow_main_income_amount_t::on_create(sys::state& state) noexcept {
// BEGIN main::income_amount::create
// END
}
ui::message_result budgetwindow_main_income_page_left_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::income_page_left::lbutton_action
	main.income_table->change_page(state, main.income_table->page - 1);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_main_income_page_left_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_main_income_page_left_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_main_income_page_left_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_page_left::update
	set_visible(state, main.income_table->max_page() > 0);
// END
}
void budgetwindow_main_income_page_left_t::on_create(sys::state& state) noexcept {
// BEGIN main::income_page_left::create
// END
}
ui::message_result budgetwindow_main_income_page_right_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::income_page_right::lbutton_action
	main.income_table->change_page(state, main.income_table->page + 1);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_main_income_page_right_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_main_income_page_right_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_main_income_page_right_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_page_right::update
	set_visible(state, main.income_table->max_page() > 0);
// END
}
void budgetwindow_main_income_page_right_t::on_create(sys::state& state) noexcept {
// BEGIN main::income_page_right::create
// END
}
void budgetwindow_main_income_page_number_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_income_page_number_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_main_income_page_number_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_income_page_number_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_page_number::update
	if(main.income_table->max_page() > 0) {
		set_text(state, std::to_string(main.income_table->page + 1)  + "/" + std::to_string(main.income_table->max_page() + 1));
	} else {
		set_text(state, "");
	}
// END
}
void budgetwindow_main_income_page_number_t::on_create(sys::state& state) noexcept {
// BEGIN main::income_page_number::create
// END
}
void budgetwindow_main_income_table_header_t::on_reset_text(sys::state & state) noexcept {
	{
	item_name_cached_text = text::produce_simple_string(state, item_name_text_key);
	 item_name_internal_layout.contents.clear();
	 item_name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(item_name_column_width - 24 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, budgetwindow_main_income_table_row_t::item_name_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_name_cached_text);
	}
	{
	item_value_cached_text = text::produce_simple_string(state, item_value_text_key);
	 item_value_internal_layout.contents.clear();
	 item_value_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_value_internal_layout, text::layout_parameters{ 0, 0, int16_t(item_value_column_width - 24 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, budgetwindow_main_income_table_row_t::item_value_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_value_cached_text);
	}
}
void  budgetwindow_main_income_table_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
		ogl::render_textured_rect(state, ui::get_color_modification(false, false,  false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, budgetwindow_main_income_table_row_t::row_background_texture, budgetwindow_main_income_table_row_t::row_texture_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	bool col_um_lead_space = rel_mouse_x >= lead_space_column_start && rel_mouse_x < (lead_space_column_start + lead_space_column_width);
	bool col_um_item_name = rel_mouse_x >= item_name_column_start && rel_mouse_x < (item_name_column_start + item_name_column_width);
	if(col_um_item_name) {
		ogl::render_alpha_colored_rect(state, float(x + item_name_column_start), float(y), float(item_name_column_width), float(base_data.size.y),budgetwindow_main_income_table_t::table_highlight_color.r, budgetwindow_main_income_table_t::table_highlight_color.g, budgetwindow_main_income_table_t::table_highlight_color.b, budgetwindow_main_income_table_t::table_highlight_color.a);
	}
if(item_name_sort_direction > 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name, false, true), float(x + item_name_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, ascending_icon, ascending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
if(item_name_sort_direction < 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name, false, true), float(x + item_name_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, descending_icon, descending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name , false, true); 
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + item_name_column_start + 24 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_name_text_color), cmod);
		}
	}
	bool col_um_item_value = rel_mouse_x >= item_value_column_start && rel_mouse_x < (item_value_column_start + item_value_column_width);
	if(col_um_item_value) {
		ogl::render_alpha_colored_rect(state, float(x + item_value_column_start), float(y), float(item_value_column_width), float(base_data.size.y),budgetwindow_main_income_table_t::table_highlight_color.r, budgetwindow_main_income_table_t::table_highlight_color.g, budgetwindow_main_income_table_t::table_highlight_color.b, budgetwindow_main_income_table_t::table_highlight_color.a);
	}
if(item_value_sort_direction > 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value, false, true), float(x + item_value_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, ascending_icon, ascending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
if(item_value_sort_direction < 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value, false, true), float(x + item_value_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, descending_icon, descending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(!item_value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value , false, true); 
		for(auto& t : item_value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + item_value_column_start + 24 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_value_text_color), cmod);
		}
	}
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), budgetwindow_main_income_table_t::table_divider_color.r, budgetwindow_main_income_table_t::table_divider_color.g, budgetwindow_main_income_table_t::table_divider_color.b, 1.0f);
}
ui::message_result budgetwindow_main_income_table_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(x >= item_name_column_start && x < item_name_column_start + item_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = item_name_sort_direction;
		item_name_sort_direction = 0;
		item_value_sort_direction = 0;
		item_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		(( budgetwindow_main_income_table_t*)parent)->apply_sort(state);
	}
	if(x >= item_value_column_start && x < item_value_column_start + item_value_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = item_value_sort_direction;
		item_name_sort_direction = 0;
		item_value_sort_direction = 0;
		item_value_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		(( budgetwindow_main_income_table_t*)parent)->apply_sort(state);
	}
	return ui::message_result::consumed;}
ui::message_result budgetwindow_main_income_table_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;}
void budgetwindow_main_income_table_header_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(x >= lead_space_column_start && x < lead_space_column_start + lead_space_column_width) {
	}
	if(x >= item_name_column_start && x < item_name_column_start + item_name_column_width) {
	}
	if(x >= item_value_column_start && x < item_value_column_start + item_value_column_width) {
	}
}
void budgetwindow_main_income_table_header_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	if(x >= lead_space_column_start && x < lead_space_column_start + lead_space_column_width) {
	}
	if(x >= item_name_column_start && x < item_name_column_start + item_name_column_width) {
	}
	if(x >= item_value_column_start && x < item_value_column_start + item_value_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void budgetwindow_main_income_table_header_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
}
void  budgetwindow_main_income_table_row_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(alternate_row) {
		ogl::render_textured_rect(state, ui::get_color_modification(false, false,  false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_row_background_texture, alt_row_texture_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	} else {
		ogl::render_textured_rect(state, ui::get_color_modification(false, false,  false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, row_background_texture, row_texture_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	}
	if(this == state.ui_state.under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x ), float(y), float(base_data.size.x), float(base_data.size.y), budgetwindow_main_income_table_t::table_highlight_color.r, budgetwindow_main_income_table_t::table_highlight_color.g, budgetwindow_main_income_table_t::table_highlight_color.b, budgetwindow_main_income_table_t::table_highlight_color.a);
	}
	bool col_um_lead_space = rel_mouse_x >= budgetwindow_main_income_table_header_t::lead_space_column_start && rel_mouse_x < (budgetwindow_main_income_table_header_t::lead_space_column_start + budgetwindow_main_income_table_header_t::lead_space_column_width);
	bool col_um_item_name = rel_mouse_x >= budgetwindow_main_income_table_header_t::item_name_column_start && rel_mouse_x < (budgetwindow_main_income_table_header_t::item_name_column_start + budgetwindow_main_income_table_header_t::item_name_column_width);
	if(col_um_item_name && this != state.ui_state.under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + budgetwindow_main_income_table_header_t::item_name_column_start), float(y), float(budgetwindow_main_income_table_header_t::item_name_column_width), float(base_data.size.y), budgetwindow_main_income_table_t::table_highlight_color.r, budgetwindow_main_income_table_t::table_highlight_color.g, budgetwindow_main_income_table_t::table_highlight_color.b, budgetwindow_main_income_table_t::table_highlight_color.a);
	}
	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name , false, false); 
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + budgetwindow_main_income_table_header_t::item_name_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_name_text_color), cmod);
	}
	}
	bool col_um_item_value = rel_mouse_x >= budgetwindow_main_income_table_header_t::item_value_column_start && rel_mouse_x < (budgetwindow_main_income_table_header_t::item_value_column_start + budgetwindow_main_income_table_header_t::item_value_column_width);
	if(col_um_item_value && this != state.ui_state.under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + budgetwindow_main_income_table_header_t::item_value_column_start), float(y), float(budgetwindow_main_income_table_header_t::item_value_column_width), float(base_data.size.y), budgetwindow_main_income_table_t::table_highlight_color.r, budgetwindow_main_income_table_t::table_highlight_color.g, budgetwindow_main_income_table_t::table_highlight_color.b, budgetwindow_main_income_table_t::table_highlight_color.a);
	}
	if(!item_value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value , false, false); 
		for(auto& t : item_value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + budgetwindow_main_income_table_header_t::item_value_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_value_text_color), cmod);
	}
	}
}
ui::message_result budgetwindow_main_income_table_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;}
ui::message_result budgetwindow_main_income_table_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;}
void budgetwindow_main_income_table_row_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(x >= budgetwindow_main_income_table_header_t::lead_space_column_start && x < budgetwindow_main_income_table_header_t::lead_space_column_start + budgetwindow_main_income_table_header_t::lead_space_column_width) {
	}
	if(x >= budgetwindow_main_income_table_header_t::item_name_column_start && x < budgetwindow_main_income_table_header_t::item_name_column_start + budgetwindow_main_income_table_header_t::item_name_column_width) {
	}
	if(x >= budgetwindow_main_income_table_header_t::item_value_column_start && x < budgetwindow_main_income_table_header_t::item_value_column_start + budgetwindow_main_income_table_header_t::item_value_column_width) {
	}
}
void budgetwindow_main_income_table_row_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	if(x >= budgetwindow_main_income_table_header_t::lead_space_column_start && x < budgetwindow_main_income_table_header_t::lead_space_column_start + budgetwindow_main_income_table_header_t::lead_space_column_width) {
	}
	if(x >= budgetwindow_main_income_table_header_t::item_name_column_start && x < budgetwindow_main_income_table_header_t::item_name_column_start + budgetwindow_main_income_table_header_t::item_name_column_width) {
	}
	if(x >= budgetwindow_main_income_table_header_t::item_value_column_start && x < budgetwindow_main_income_table_header_t::item_value_column_start + budgetwindow_main_income_table_header_t::item_value_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void budgetwindow_main_income_table_row_t::on_create(sys::state& state) noexcept {
}
void budgetwindow_main_income_table_row_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_income_table_t& income_table = *((budgetwindow_main_income_table_t*)(parent)); 
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN main::income_table::row_update
	set_item_name_text(state, value.first);
	set_item_value_text(state, text::prettify_currency(value.second));
// END
}
void budgetwindow_main_income_table_row_t::set_item_name_text(sys::state & state, std::string const& new_text) {
	if(new_text !=  item_name_cached_text) {
		item_name_cached_text = new_text;
		item_name_internal_layout.contents.clear();
		item_name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(budgetwindow_main_income_table_header_t::item_name_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, item_name_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_name_cached_text);
		}
	} else {
	}
}
void budgetwindow_main_income_table_row_t::set_item_value_text(sys::state & state, std::string const& new_text) {
	if(new_text !=  item_value_cached_text) {
		item_value_cached_text = new_text;
		item_value_internal_layout.contents.clear();
		item_value_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_value_internal_layout, text::layout_parameters{ 0, 0, int16_t(budgetwindow_main_income_table_header_t::item_value_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, item_value_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_value_cached_text);
		}
	} else {
	}
}
void budgetwindow_main_income_table_t::impl_change_page(sys::state & state, int32_t new_page){
	size_t section_header_pool_used = 0;
	size_t row_pool_used = 0;
	size_t header_pool_used = 0;
	int16_t vert_used = 0;
	bool alt_bg  = true;
	page = std::clamp(new_page, 0, max_page());
	children.clear();
	if(page_starts.empty()) return;
	if(open_page) { page_starts.push_back(int32_t(values.size())); open_page = false; }
	for(int32_t i = page_starts[page]; i < page_starts[page + 1]; ++i) {
		if(std::holds_alternative<section_header_option>(values[i])) {
			if(section_header_pool_used >= section_header_pool.size()) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
			children.push_back(section_header_pool[section_header_pool_used].get());
			children.back()->parent = this;
			((budgetwindow_section_header_t*)(section_header_pool[section_header_pool_used].get()))->section_type = std::get<section_header_option>(values[i]).section_type;
			section_header_pool[section_header_pool_used]->base_data.position.y = vert_used;
			vert_used += section_header_pool[section_header_pool_used]->base_data.size.y;
			++section_header_pool_used;
			alt_bg  = false;
		}
		if(std::holds_alternative<value_option>(values[i])) {
			if(row_pool_used >= row_pool.size())row_pool.emplace_back(make_row(state));
			children.push_back(row_pool[row_pool_used].get());
			children.back()->parent = this;
			row_pool[row_pool_used]->value = std::get<value_option>(values[i]).value;
			row_pool[row_pool_used]->alternate_row = alt_bg;
			row_pool[row_pool_used]->base_data.position.y = vert_used;
			vert_used += row_pool[row_pool_used]->base_data.size.y;
			++row_pool_used;
			alt_bg  = !alt_bg;
		}
		if(std::holds_alternative<std::monostate>(values[i])) {
			if(header_pool_used >= header_pool.size()) header_pool.emplace_back(make_headers(state));
			children.push_back(header_pool[header_pool_used].get());
			children.back()->parent = this;
			header_pool[header_pool_used]->base_data.position.y = vert_used;
			vert_used += header_pool[header_pool_used]->base_data.size.y;
			++header_pool_used;
			alt_bg  = true;
		}
	}
}
void budgetwindow_main_income_table_t::change_page(sys::state & state, int32_t new_page){
	impl_change_page(state, new_page);	for(auto c : children) c->impl_on_update(state);
}
int32_t budgetwindow_main_income_table_t::max_page(){
	return (open_page ? 1 : 0) + ((page_starts.size() <= 2) ? 0 : int32_t(page_starts.size() - 2)); 
}
void budgetwindow_main_income_table_t::clear_table() {
	children.clear();
	page_starts.clear();
	values.clear();
	open_page = false;
	layout_space = 0;
}
void budgetwindow_main_income_table_t::add_value(std::pair<std::string, float> const& v) {
	if(!open_page) {
		page_starts.push_back(0);
		open_page = true;
	}
	if(values.empty() || !std::holds_alternative<value_option>(values.back())) {
		if(base_data.size.y - layout_space <= 2 * int32_t(row_height * 8)) {
			page_starts.push_back(int32_t(values.size()));
			layout_space = 0;
		}
		values.emplace_back(std::monostate{});
		layout_space += int16_t(row_height * 8);
	}
	if(base_data.size.y - layout_space <= int32_t(row_height * 8)) {
		page_starts.push_back(int32_t(values.size()));
		layout_space = 0;
		values.emplace_back(std::monostate{});
		layout_space += int16_t(row_height * 8);
	}
	values.emplace_back(value_option{v});
	layout_space += int16_t(row_height * 8);
}
void budgetwindow_main_income_table_t::add_insert_section_header(sys::state& state, int32_t const& section_type) {
	if(section_header_pool.empty()) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
	if(!open_page) {
		page_starts.push_back(0);
		open_page = true;
	}
	if(base_data.size.y - layout_space <=  section_header_pool[0]->base_data.size.y) {
		page_starts.push_back(int32_t(values.size()));
		layout_space = 0;
	}
	values.emplace_back(section_header_option{section_type});
	layout_space += int16_t(section_header_pool[0]->base_data.size.y);
}
std::unique_ptr<budgetwindow_main_income_table_row_t> budgetwindow_main_income_table_t::make_row(sys::state& state) {
	auto cptr = std::make_unique<budgetwindow_main_income_table_row_t>();
	cptr->parent = this;
	cptr->base_data.position.x = 0;
	cptr->base_data.position.y = 0;
	cptr->base_data.size.y = int16_t(row_height * 8);
	cptr->base_data.size.x = this->base_data.size.x;
	cptr->on_create(state);
	return cptr;
}
std::unique_ptr<budgetwindow_main_income_table_header_t> budgetwindow_main_income_table_t::make_headers(sys::state& state){
	auto cptr = std::make_unique<budgetwindow_main_income_table_header_t>();
	cptr->parent = this;
	cptr->base_data.position.x = 0;
	cptr->base_data.position.y = 0;
	cptr->base_data.size.y = int16_t(row_height * 8);
	cptr->base_data.size.x = this->base_data.size.x;
	cptr->on_create(state);
	return cptr;
}
void budgetwindow_main_income_table_t::impl_apply_sort(sys::state& state) {
	bool work_to_do = false;
	if(budgetwindow_main_income_table_header_t::item_name_sort_direction != 0) work_to_do = true;
	if(budgetwindow_main_income_table_header_t::item_value_sort_direction != 0) work_to_do = true;
	if(!work_to_do) return;
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	std::vector<std::pair<std::string, float>> val_temp;
	size_t i = 0;
	while(i < values.size()) {
		if(std::holds_alternative<value_option>(values[i])) {
			auto start_pos = i;
			do {
				if(std::holds_alternative<value_option>(values[i])) val_temp.push_back(std::get<value_option>(values[i]).value);
				++i;
			} while(i < values.size() && (std::holds_alternative<value_option>(values[i]) || std::holds_alternative<std::monostate>(values[i])));
			if(budgetwindow_main_income_table_header_t::item_name_sort_direction != 0) {
				sys::merge_sort(val_temp.begin(), val_temp.end(), [&](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b){
					int8_t result = 0;
// BEGIN main::income_table::sort::item_name
					result = int8_t(std::clamp(a.first.compare(b.first), -1, 1));
// END
					return result == budgetwindow_main_income_table_header_t::item_name_sort_direction;
				});
			}
			if(budgetwindow_main_income_table_header_t::item_value_sort_direction != 0) {
				sys::merge_sort(val_temp.begin(), val_temp.end(), [&](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b){
					int8_t result = 0;
// BEGIN main::income_table::sort::item_value
					if(a.second == b.second)
						result = 0;
					else
						result = (a.second < b.second) ? -1 : 1;
// END
					return result == budgetwindow_main_income_table_header_t::item_value_sort_direction;
				});
			}
			while(start_pos < i) {
				if(std::holds_alternative<value_option>(values[start_pos])) {
					std::get<value_option>(values[start_pos]).value = val_temp.back();
					val_temp.pop_back();
				}
				++start_pos;
			}
			val_temp.clear();
		} else {
			++i;
		}
	}
}
void budgetwindow_main_income_table_t::apply_sort(sys::state& state) {
	impl_apply_sort(state);
	impl_change_page(state, page);	for(auto c : children) c->impl_on_update(state);
}
ui::message_result budgetwindow_main_income_table_t::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	change_page(state, page + ((amount < 0) ? 1 : -1));
	return ui::message_result::consumed;
}
void budgetwindow_main_income_table_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_table::update
	clear_table();
	add_insert_section_header(state, budget_categories::poor_tax);
	if(budget_categories::expanded[budget_categories::poor_tax]) {
		auto total = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::poor) * float(state.world.nation_get_poor_tax(state.local_player_nation)) / 100.0f;
		auto type_totals = state.world.pop_type_make_vectorizable_float_buffer();
		auto all_totals = 0.0f;

		for(auto p : state.world.nation_get_province_ownership(state.local_player_nation)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				for(auto pl : province.get_pop_location()) {
					if(culture::pop_strata(pl.get_pop().get_poptype().get_strata()) == culture::pop_strata::poor) {
						all_totals += pl.get_pop().get_savings();
						type_totals.set(pl.get_pop().get_poptype(), type_totals.get(pl.get_pop().get_poptype()) + pl.get_pop().get_savings());
					}
				}
			}
		}

		for(auto pt : state.world.in_pop_type) {
			if(type_totals.get(pt) > 0.0f && all_totals > 0.0f) {
				add_value(std::pair<std::string, float>(text::produce_simple_string(state, pt.get_name()), total * type_totals.get(pt) / all_totals));
			}
		}
	}
	add_insert_section_header(state, budget_categories::middle_tax);
	if(budget_categories::expanded[budget_categories::middle_tax]) {
		auto total = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::middle) * float(state.world.nation_get_middle_tax(state.local_player_nation)) / 100.0f;
		auto type_totals = state.world.pop_type_make_vectorizable_float_buffer();
		auto all_totals = 0.0f;

		for(auto p : state.world.nation_get_province_ownership(state.local_player_nation)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				for(auto pl : province.get_pop_location()) {
					if(culture::pop_strata(pl.get_pop().get_poptype().get_strata()) == culture::pop_strata::middle) {
						all_totals += pl.get_pop().get_savings();
						type_totals.set(pl.get_pop().get_poptype(), type_totals.get(pl.get_pop().get_poptype()) + pl.get_pop().get_savings());
					}
				}
			}
		}

		for(auto pt : state.world.in_pop_type) {
			if(type_totals.get(pt) > 0.0f && all_totals > 0.0f) {
				add_value(std::pair<std::string, float>(text::produce_simple_string(state, pt.get_name()), total * type_totals.get(pt) / all_totals));
			}
		}
	}
	add_insert_section_header(state, budget_categories::rich_tax);
	if(budget_categories::expanded[budget_categories::rich_tax]) {
		auto total = economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::rich) * float(state.world.nation_get_rich_tax(state.local_player_nation)) / 100.0f;
		auto type_totals = state.world.pop_type_make_vectorizable_float_buffer();
		auto all_totals = 0.0f;

		for(auto p : state.world.nation_get_province_ownership(state.local_player_nation)) {
			auto province = p.get_province();
			if(state.world.province_get_nation_from_province_ownership(province) == state.world.province_get_nation_from_province_control(province)) {
				for(auto pl : province.get_pop_location()) {
					if(culture::pop_strata(pl.get_pop().get_poptype().get_strata()) == culture::pop_strata::rich) {
						all_totals += pl.get_pop().get_savings();
						type_totals.set(pl.get_pop().get_poptype(), type_totals.get(pl.get_pop().get_poptype()) + pl.get_pop().get_savings());
					}
				}
			}
		}

		for(auto pt : state.world.in_pop_type) {
			if(type_totals.get(pt) > 0.0f && all_totals > 0.0f) {
				add_value(std::pair<std::string, float>(text::produce_simple_string(state, pt.get_name()), total * type_totals.get(pt) / all_totals));
			}
		}
	}
	add_insert_section_header(state, budget_categories::tariffs);
	add_insert_section_header(state, budget_categories::diplomatic_income);
	if(budget_categories::expanded[budget_categories::diplomatic_income]) {
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, "warsubsidies_button"), economy::estimate_war_subsidies_income(state, state.local_player_nation)));
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, "alice_budget_indemnities"), economy::estimate_reparations_income(state, state.local_player_nation)));
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, "alice_budget_subjects"), economy::estimate_subject_payments_received(state, state.local_player_nation)));
	}
	add_insert_section_header(state, budget_categories::gold);
	if(budget_categories::expanded[budget_categories::gold]) {
		for(auto poid : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
			auto prov = poid.get_province();

			float amount = 0.0f;
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				if(state.world.commodity_get_money_rgo(c)) {
					amount += province::rgo_production_quantity(state, prov.id, c) * state.world.commodity_get_cost(c);
				}
			});

			if(amount > 0.0f) {
				add_value(std::pair<std::string, float>(text::produce_simple_string(state, state.world.province_get_name(prov)), amount * state.defines.gold_to_cash_rate));
			}
		}
	}
// END
	impl_apply_sort(state);
	impl_change_page(state, page);
}
void budgetwindow_main_income_table_t::on_create(sys::state& state) noexcept {
// BEGIN main::income_table::create
// END
}
ui::message_result budgetwindow_main_expenses_page_left_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::expenses_page_left::lbutton_action
	auto pos = ui::get_absolute_location(state, *main.expenses_table);
	state.ui_animation.start_animation(state, pos.x, pos.y, main.expenses_table->base_data.size.x, main.expenses_table->base_data.size.y, ogl::animation::type::page_flip_left_rev, 3000);
	main.expenses_table->change_page(state, main.expenses_table->page - 1);
	state.ui_animation.post_update_frame(state);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_main_expenses_page_left_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_main_expenses_page_left_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_main_expenses_page_left_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::expenses_page_left::update
	set_visible(state, main.expenses_table->max_page() > 0);
// END
}
void budgetwindow_main_expenses_page_left_t::on_create(sys::state& state) noexcept {
// BEGIN main::expenses_page_left::create
// END
}
void budgetwindow_main_expenses_page_number_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_expenses_page_number_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_main_expenses_page_number_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_expenses_page_number_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::expenses_page_number::update
	if(main.expenses_table->max_page() > 0) {
		set_text(state, std::to_string(main.expenses_table->page + 1) + "/" + std::to_string(main.expenses_table->max_page() + 1));
	} else {
		set_text(state, "");
	}
// END
}
void budgetwindow_main_expenses_page_number_t::on_create(sys::state& state) noexcept {
// BEGIN main::expenses_page_number::create
// END
}
ui::message_result budgetwindow_main_expenses_page_right_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::expenses_page_right::lbutton_action
	auto pos = ui::get_absolute_location(state, *main.expenses_table);
	state.ui_animation.start_animation(state, pos.x, pos.y, main.expenses_table->base_data.size.x, main.expenses_table->base_data.size.y, ogl::animation::type::page_flip_left, 3000);
	main.expenses_table->change_page(state, main.expenses_table->page + 1);
	state.ui_animation.post_update_frame(state);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_main_expenses_page_right_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_main_expenses_page_right_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_main_expenses_page_right_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::expenses_page_right::update
	set_visible(state, main.expenses_table->max_page() > 0);
// END
}
void budgetwindow_main_expenses_page_right_t::on_create(sys::state& state) noexcept {
// BEGIN main::expenses_page_right::create
// END
}
void budgetwindow_main_expenses_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_expenses_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_expenses_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_expenses_label_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::expenses_label::update
// END
}
void budgetwindow_main_expenses_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::expenses_label::create
// END
}
void budgetwindow_main_expenses_amount_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_expenses_amount_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_main_expenses_amount_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_expenses_amount_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::expenses_amount::update
	float total = 0.0f;
	total += economy::estimate_diplomatic_expenses(state, state.local_player_nation); 
	total += economy::estimate_social_spending(state, state.local_player_nation) * float(state.world.nation_get_social_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military) * float(state.world.nation_get_military_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education) * float(state.world.nation_get_education_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::administration) * float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_domestic_investment(state, state.local_player_nation) * float(state.world.nation_get_domestic_investment_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_overseas_penalty_spending(state, state.local_player_nation) * float(state.world.nation_get_overseas_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_subsidy_spending(state, state.local_player_nation);
	total += economy::estimate_construction_spending(state, state.local_player_nation) * float(state.world.nation_get_construction_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_land_spending(state, state.local_player_nation) * float(state.world.nation_get_land_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_naval_spending(state, state.local_player_nation) * float(state.world.nation_get_naval_spending(state.local_player_nation)) / 100.0f;
	total += economy::interest_payment(state, state.local_player_nation);
	set_text(state, text::prettify_currency(total));
// END
}
void budgetwindow_main_expenses_amount_t::on_create(sys::state& state) noexcept {
// BEGIN main::expenses_amount::create
// END
}
void budgetwindow_main_expenses_table_header_t::on_reset_text(sys::state & state) noexcept {
	{
	item_name_cached_text = text::produce_simple_string(state, item_name_text_key);
	 item_name_internal_layout.contents.clear();
	 item_name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(item_name_column_width - 24 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, budgetwindow_main_expenses_table_row_t::item_name_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_name_cached_text);
	}
	{
	item_value_cached_text = text::produce_simple_string(state, item_value_text_key);
	 item_value_internal_layout.contents.clear();
	 item_value_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_value_internal_layout, text::layout_parameters{ 0, 0, int16_t(item_value_column_width - 24 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, budgetwindow_main_expenses_table_row_t::item_value_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_value_cached_text);
	}
}
void  budgetwindow_main_expenses_table_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
		ogl::render_textured_rect(state, ui::get_color_modification(false, false,  false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, budgetwindow_main_expenses_table_row_t::row_background_texture, budgetwindow_main_expenses_table_row_t::row_texture_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	bool col_um_lead_space = rel_mouse_x >= lead_space_column_start && rel_mouse_x < (lead_space_column_start + lead_space_column_width);
	bool col_um_item_name = rel_mouse_x >= item_name_column_start && rel_mouse_x < (item_name_column_start + item_name_column_width);
	if(col_um_item_name) {
		ogl::render_alpha_colored_rect(state, float(x + item_name_column_start), float(y), float(item_name_column_width), float(base_data.size.y),budgetwindow_main_expenses_table_t::table_highlight_color.r, budgetwindow_main_expenses_table_t::table_highlight_color.g, budgetwindow_main_expenses_table_t::table_highlight_color.b, budgetwindow_main_expenses_table_t::table_highlight_color.a);
	}
if(item_name_sort_direction > 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name, false, true), float(x + item_name_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, ascending_icon, ascending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
if(item_name_sort_direction < 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name, false, true), float(x + item_name_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, descending_icon, descending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name , false, true); 
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + item_name_column_start + 24 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_name_text_color), cmod);
		}
	}
	bool col_um_item_value = rel_mouse_x >= item_value_column_start && rel_mouse_x < (item_value_column_start + item_value_column_width);
	if(col_um_item_value) {
		ogl::render_alpha_colored_rect(state, float(x + item_value_column_start), float(y), float(item_value_column_width), float(base_data.size.y),budgetwindow_main_expenses_table_t::table_highlight_color.r, budgetwindow_main_expenses_table_t::table_highlight_color.g, budgetwindow_main_expenses_table_t::table_highlight_color.b, budgetwindow_main_expenses_table_t::table_highlight_color.a);
	}
if(item_value_sort_direction > 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value, false, true), float(x + item_value_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, ascending_icon, ascending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
if(item_value_sort_direction < 0) {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value, false, true), float(x + item_value_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, descending_icon, descending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(!item_value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value , false, true); 
		for(auto& t : item_value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + item_value_column_start + 24 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_value_text_color), cmod);
		}
	}
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), budgetwindow_main_expenses_table_t::table_divider_color.r, budgetwindow_main_expenses_table_t::table_divider_color.g, budgetwindow_main_expenses_table_t::table_divider_color.b, 1.0f);
}
ui::message_result budgetwindow_main_expenses_table_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(x >= item_name_column_start && x < item_name_column_start + item_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = item_name_sort_direction;
		item_name_sort_direction = 0;
		item_value_sort_direction = 0;
		item_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		(( budgetwindow_main_expenses_table_t*)parent)->apply_sort(state);
	}
	if(x >= item_value_column_start && x < item_value_column_start + item_value_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = item_value_sort_direction;
		item_name_sort_direction = 0;
		item_value_sort_direction = 0;
		item_value_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		(( budgetwindow_main_expenses_table_t*)parent)->apply_sort(state);
	}
	return ui::message_result::consumed;}
ui::message_result budgetwindow_main_expenses_table_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;}
void budgetwindow_main_expenses_table_header_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(x >= lead_space_column_start && x < lead_space_column_start + lead_space_column_width) {
	}
	if(x >= item_name_column_start && x < item_name_column_start + item_name_column_width) {
	}
	if(x >= item_value_column_start && x < item_value_column_start + item_value_column_width) {
	}
}
void budgetwindow_main_expenses_table_header_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	if(x >= lead_space_column_start && x < lead_space_column_start + lead_space_column_width) {
	}
	if(x >= item_name_column_start && x < item_name_column_start + item_name_column_width) {
	}
	if(x >= item_value_column_start && x < item_value_column_start + item_value_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void budgetwindow_main_expenses_table_header_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
}
void  budgetwindow_main_expenses_table_row_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(alternate_row) {
		ogl::render_textured_rect(state, ui::get_color_modification(false, false,  false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_row_background_texture, alt_row_texture_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	} else {
		ogl::render_textured_rect(state, ui::get_color_modification(false, false,  false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, row_background_texture, row_texture_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	}
	if(this == state.ui_state.under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x ), float(y), float(base_data.size.x), float(base_data.size.y), budgetwindow_main_expenses_table_t::table_highlight_color.r, budgetwindow_main_expenses_table_t::table_highlight_color.g, budgetwindow_main_expenses_table_t::table_highlight_color.b, budgetwindow_main_expenses_table_t::table_highlight_color.a);
	}
	bool col_um_lead_space = rel_mouse_x >= budgetwindow_main_expenses_table_header_t::lead_space_column_start && rel_mouse_x < (budgetwindow_main_expenses_table_header_t::lead_space_column_start + budgetwindow_main_expenses_table_header_t::lead_space_column_width);
	bool col_um_item_name = rel_mouse_x >= budgetwindow_main_expenses_table_header_t::item_name_column_start && rel_mouse_x < (budgetwindow_main_expenses_table_header_t::item_name_column_start + budgetwindow_main_expenses_table_header_t::item_name_column_width);
	if(col_um_item_name && this != state.ui_state.under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + budgetwindow_main_expenses_table_header_t::item_name_column_start), float(y), float(budgetwindow_main_expenses_table_header_t::item_name_column_width), float(base_data.size.y), budgetwindow_main_expenses_table_t::table_highlight_color.r, budgetwindow_main_expenses_table_t::table_highlight_color.g, budgetwindow_main_expenses_table_t::table_highlight_color.b, budgetwindow_main_expenses_table_t::table_highlight_color.a);
	}
	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name , false, false); 
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + budgetwindow_main_expenses_table_header_t::item_name_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_name_text_color), cmod);
	}
	}
	bool col_um_item_value = rel_mouse_x >= budgetwindow_main_expenses_table_header_t::item_value_column_start && rel_mouse_x < (budgetwindow_main_expenses_table_header_t::item_value_column_start + budgetwindow_main_expenses_table_header_t::item_value_column_width);
	if(col_um_item_value && this != state.ui_state.under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + budgetwindow_main_expenses_table_header_t::item_value_column_start), float(y), float(budgetwindow_main_expenses_table_header_t::item_value_column_width), float(base_data.size.y), budgetwindow_main_expenses_table_t::table_highlight_color.r, budgetwindow_main_expenses_table_t::table_highlight_color.g, budgetwindow_main_expenses_table_t::table_highlight_color.b, budgetwindow_main_expenses_table_t::table_highlight_color.a);
	}
	if(!item_value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value , false, false); 
		for(auto& t : item_value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + budgetwindow_main_expenses_table_header_t::item_value_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_value_text_color), cmod);
	}
	}
}
ui::message_result budgetwindow_main_expenses_table_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;}
ui::message_result budgetwindow_main_expenses_table_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;}
void budgetwindow_main_expenses_table_row_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	if(x >= budgetwindow_main_expenses_table_header_t::lead_space_column_start && x < budgetwindow_main_expenses_table_header_t::lead_space_column_start + budgetwindow_main_expenses_table_header_t::lead_space_column_width) {
	}
	if(x >= budgetwindow_main_expenses_table_header_t::item_name_column_start && x < budgetwindow_main_expenses_table_header_t::item_name_column_start + budgetwindow_main_expenses_table_header_t::item_name_column_width) {
	}
	if(x >= budgetwindow_main_expenses_table_header_t::item_value_column_start && x < budgetwindow_main_expenses_table_header_t::item_value_column_start + budgetwindow_main_expenses_table_header_t::item_value_column_width) {
	}
}
void budgetwindow_main_expenses_table_row_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	if(x >= budgetwindow_main_expenses_table_header_t::lead_space_column_start && x < budgetwindow_main_expenses_table_header_t::lead_space_column_start + budgetwindow_main_expenses_table_header_t::lead_space_column_width) {
	}
	if(x >= budgetwindow_main_expenses_table_header_t::item_name_column_start && x < budgetwindow_main_expenses_table_header_t::item_name_column_start + budgetwindow_main_expenses_table_header_t::item_name_column_width) {
	}
	if(x >= budgetwindow_main_expenses_table_header_t::item_value_column_start && x < budgetwindow_main_expenses_table_header_t::item_value_column_start + budgetwindow_main_expenses_table_header_t::item_value_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void budgetwindow_main_expenses_table_row_t::on_create(sys::state& state) noexcept {
}
void budgetwindow_main_expenses_table_row_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_expenses_table_t& expenses_table = *((budgetwindow_main_expenses_table_t*)(parent)); 
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN main::expenses_table::row_update
	set_item_name_text(state, value.first);
	set_item_value_text(state, text::prettify_currency(value.second));
// END
}
void budgetwindow_main_expenses_table_row_t::set_item_name_text(sys::state & state, std::string const& new_text) {
	if(new_text !=  item_name_cached_text) {
		item_name_cached_text = new_text;
		item_name_internal_layout.contents.clear();
		item_name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(budgetwindow_main_expenses_table_header_t::item_name_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, item_name_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_name_cached_text);
		}
	} else {
	}
}
void budgetwindow_main_expenses_table_row_t::set_item_value_text(sys::state & state, std::string const& new_text) {
	if(new_text !=  item_value_cached_text) {
		item_value_cached_text = new_text;
		item_value_internal_layout.contents.clear();
		item_value_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_value_internal_layout, text::layout_parameters{ 0, 0, int16_t(budgetwindow_main_expenses_table_header_t::item_value_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, item_value_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_value_cached_text);
		}
	} else {
	}
}
void budgetwindow_main_expenses_table_t::impl_change_page(sys::state & state, int32_t new_page){
	size_t section_header_pool_used = 0;
	size_t row_pool_used = 0;
	size_t header_pool_used = 0;
	int16_t vert_used = 0;
	bool alt_bg  = true;
	page = std::clamp(new_page, 0, max_page());
	children.clear();
	if(page_starts.empty()) return;
	if(open_page) { page_starts.push_back(int32_t(values.size())); open_page = false; }
	for(int32_t i = page_starts[page]; i < page_starts[page + 1]; ++i) {
		if(std::holds_alternative<section_header_option>(values[i])) {
			if(section_header_pool_used >= section_header_pool.size()) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
			children.push_back(section_header_pool[section_header_pool_used].get());
			children.back()->parent = this;
			((budgetwindow_section_header_t*)(section_header_pool[section_header_pool_used].get()))->section_type = std::get<section_header_option>(values[i]).section_type;
			section_header_pool[section_header_pool_used]->base_data.position.y = vert_used;
			vert_used += section_header_pool[section_header_pool_used]->base_data.size.y;
			++section_header_pool_used;
			alt_bg  = false;
		}
		if(std::holds_alternative<value_option>(values[i])) {
			if(row_pool_used >= row_pool.size())row_pool.emplace_back(make_row(state));
			children.push_back(row_pool[row_pool_used].get());
			children.back()->parent = this;
			row_pool[row_pool_used]->value = std::get<value_option>(values[i]).value;
			row_pool[row_pool_used]->alternate_row = alt_bg;
			row_pool[row_pool_used]->base_data.position.y = vert_used;
			vert_used += row_pool[row_pool_used]->base_data.size.y;
			++row_pool_used;
			alt_bg  = !alt_bg;
		}
		if(std::holds_alternative<std::monostate>(values[i])) {
			if(header_pool_used >= header_pool.size()) header_pool.emplace_back(make_headers(state));
			children.push_back(header_pool[header_pool_used].get());
			children.back()->parent = this;
			header_pool[header_pool_used]->base_data.position.y = vert_used;
			vert_used += header_pool[header_pool_used]->base_data.size.y;
			++header_pool_used;
			alt_bg  = true;
		}
	}
}
void budgetwindow_main_expenses_table_t::change_page(sys::state & state, int32_t new_page){
	impl_change_page(state, new_page);	for(auto c : children) c->impl_on_update(state);
}
int32_t budgetwindow_main_expenses_table_t::max_page(){
	return (open_page ? 1 : 0) + ((page_starts.size() <= 2) ? 0 : int32_t(page_starts.size() - 2)); 
}
void budgetwindow_main_expenses_table_t::clear_table() {
	children.clear();
	page_starts.clear();
	values.clear();
	open_page = false;
	layout_space = 0;
}
void budgetwindow_main_expenses_table_t::add_value(std::pair<std::string, float> const& v) {
	if(!open_page) {
		page_starts.push_back(0);
		open_page = true;
	}
	if(values.empty() || !std::holds_alternative<value_option>(values.back())) {
		if(base_data.size.y - layout_space <= 2 * int32_t(row_height * 8)) {
			page_starts.push_back(int32_t(values.size()));
			layout_space = 0;
		}
		values.emplace_back(std::monostate{});
		layout_space += int16_t(row_height * 8);
	}
	if(base_data.size.y - layout_space <= int32_t(row_height * 8)) {
		page_starts.push_back(int32_t(values.size()));
		layout_space = 0;
		values.emplace_back(std::monostate{});
		layout_space += int16_t(row_height * 8);
	}
	values.emplace_back(value_option{v});
	layout_space += int16_t(row_height * 8);
}
void budgetwindow_main_expenses_table_t::add_insert_section_header(sys::state& state, int32_t const& section_type) {
	if(section_header_pool.empty()) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
	if(!open_page) {
		page_starts.push_back(0);
		open_page = true;
	}
	if(base_data.size.y - layout_space <=  section_header_pool[0]->base_data.size.y) {
		page_starts.push_back(int32_t(values.size()));
		layout_space = 0;
	}
	values.emplace_back(section_header_option{section_type});
	layout_space += int16_t(section_header_pool[0]->base_data.size.y);
}
std::unique_ptr<budgetwindow_main_expenses_table_row_t> budgetwindow_main_expenses_table_t::make_row(sys::state& state) {
	auto cptr = std::make_unique<budgetwindow_main_expenses_table_row_t>();
	cptr->parent = this;
	cptr->base_data.position.x = 0;
	cptr->base_data.position.y = 0;
	cptr->base_data.size.y = int16_t(row_height * 8);
	cptr->base_data.size.x = this->base_data.size.x;
	cptr->on_create(state);
	return cptr;
}
std::unique_ptr<budgetwindow_main_expenses_table_header_t> budgetwindow_main_expenses_table_t::make_headers(sys::state& state){
	auto cptr = std::make_unique<budgetwindow_main_expenses_table_header_t>();
	cptr->parent = this;
	cptr->base_data.position.x = 0;
	cptr->base_data.position.y = 0;
	cptr->base_data.size.y = int16_t(row_height * 8);
	cptr->base_data.size.x = this->base_data.size.x;
	cptr->on_create(state);
	return cptr;
}
void budgetwindow_main_expenses_table_t::impl_apply_sort(sys::state& state) {
	bool work_to_do = false;
	if(budgetwindow_main_expenses_table_header_t::item_name_sort_direction != 0) work_to_do = true;
	if(budgetwindow_main_expenses_table_header_t::item_value_sort_direction != 0) work_to_do = true;
	if(!work_to_do) return;
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	std::vector<std::pair<std::string, float>> val_temp;
	size_t i = 0;
	while(i < values.size()) {
		if(std::holds_alternative<value_option>(values[i])) {
			auto start_pos = i;
			do {
				if(std::holds_alternative<value_option>(values[i])) val_temp.push_back(std::get<value_option>(values[i]).value);
				++i;
			} while(i < values.size() && (std::holds_alternative<value_option>(values[i]) || std::holds_alternative<std::monostate>(values[i])));
			if(budgetwindow_main_expenses_table_header_t::item_name_sort_direction != 0) {
				sys::merge_sort(val_temp.begin(), val_temp.end(), [&](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b){
					int8_t result = 0;
// BEGIN main::expenses_table::sort::item_name
					result = int8_t(std::clamp(a.first.compare(b.first), -1, 1));
// END
					return result == budgetwindow_main_expenses_table_header_t::item_name_sort_direction;
				});
			}
			if(budgetwindow_main_expenses_table_header_t::item_value_sort_direction != 0) {
				sys::merge_sort(val_temp.begin(), val_temp.end(), [&](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b){
					int8_t result = 0;
// BEGIN main::expenses_table::sort::item_value
					if(a.second == b.second)
						result = 0;
					else
						result = (a.second < b.second) ? -1 : 1;
// END
					return result == budgetwindow_main_expenses_table_header_t::item_value_sort_direction;
				});
			}
			while(start_pos < i) {
				if(std::holds_alternative<value_option>(values[start_pos])) {
					std::get<value_option>(values[start_pos]).value = val_temp.back();
					val_temp.pop_back();
				}
				++start_pos;
			}
			val_temp.clear();
		} else {
			++i;
		}
	}
}
void budgetwindow_main_expenses_table_t::apply_sort(sys::state& state) {
	impl_apply_sort(state);
	impl_change_page(state, page);	for(auto c : children) c->impl_on_update(state);
}
ui::message_result budgetwindow_main_expenses_table_t::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	change_page(state, page + ((amount < 0) ? 1 : -1));
	return ui::message_result::consumed;
}
void budgetwindow_main_expenses_table_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::expenses_table::update
	clear_table();
	add_insert_section_header(state, budget_categories::construction);
	if(budget_categories::expanded[budget_categories::construction]) {
		// TODO
	}
	add_insert_section_header(state, budget_categories::army_upkeep);
	if(budget_categories::expanded[budget_categories::army_upkeep]) {
		auto fraction = float(state.world.nation_get_land_spending(state.local_player_nation)) / 100.0f;
		auto goods = state.world.commodity_make_vectorizable_float_buffer();

		uint32_t total_commodities = state.world.commodity_size();
		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
				goods.set(cid, goods.get(cid) + state.world.market_get_army_demand(market, cid)
					* economy::price(state, market, cid)
					* state.world.market_get_demand_satisfaction(market, cid));
			}
		});
		for(auto c : state.world.in_commodity) {
			if(goods.get(c) > 0.0f) {
				add_value(std::pair<std::string, float>(text::produce_simple_string(state, state.world.commodity_get_name(c)), goods.get(c) * fraction));
			}
		}
	}
	add_insert_section_header(state, budget_categories::navy_upkeep);
	if(budget_categories::expanded[budget_categories::navy_upkeep]) {
		auto fraction = float(state.world.nation_get_naval_spending(state.local_player_nation)) / 100.0f;
		auto goods = state.world.commodity_make_vectorizable_float_buffer();

		uint32_t total_commodities = state.world.commodity_size();
		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
				goods.set(cid, goods.get(cid) + state.world.market_get_navy_demand(market, cid)
					* economy::price(state, market, cid)
					* state.world.market_get_demand_satisfaction(market, cid));
			}
		});
		for(auto c : state.world.in_commodity) {
			if(goods.get(c) > 0.0f) {
				add_value(std::pair<std::string, float>(text::produce_simple_string(state, state.world.commodity_get_name(c)), goods.get(c) * fraction));
			}
		}
	}
	add_insert_section_header(state, budget_categories::admin);
	if(budget_categories::expanded[budget_categories::admin]) {
		auto fraction = float(state.world.nation_get_military_spending(state.local_player_nation)) / 100.0f;
		auto in = culture::income_type::administration;
		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			float total = 0.0f;
			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				auto adj_pop_of_type =
					state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt)) / state.defines.alice_needs_scaling_factor;

				if(adj_pop_of_type <= 0)
					return;

				auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
				if(ln_type == in) {
					total += adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
				}
				auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
				if(en_type == in) {
					total += adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
				}
				auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
				if(lx_type == in) {
					total += adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
				}
			});
			if(total > 0.0f) {
				add_value(std::pair<std::string, float>(text::get_dynamic_state_name(state, local_state), total * fraction));
			}
		});
	}
	add_insert_section_header(state, budget_categories::education);
	if(budget_categories::expanded[budget_categories::education]) {
		auto fraction = float(state.world.nation_get_military_spending(state.local_player_nation)) / 100.0f;
		auto in = culture::income_type::education;
		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			float total = 0.0f;
			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				auto adj_pop_of_type =
					state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt)) / state.defines.alice_needs_scaling_factor;

				if(adj_pop_of_type <= 0)
					return;

				auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
				if(ln_type == in) {
					total += adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
				}
				auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
				if(en_type == in) {
					total += adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
				}
				auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
				if(lx_type == in) {
					total += adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
				}
			});
			if(total > 0.0f) {
				add_value(std::pair<std::string, float>(text::get_dynamic_state_name(state, local_state), total * fraction));
			}
		});
	}
	add_insert_section_header(state, budget_categories::social);
	if(budget_categories::expanded[budget_categories::social]) {
		auto const p_level = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::unemployment_benefit);
		auto fraction = float(state.world.nation_get_social_spending(state.local_player_nation)) / 100.0f;

		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			float total = 0.0f;

			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				auto adj_pop_of_type =
					state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt))
					/ state.defines.alice_needs_scaling_factor;
				if(adj_pop_of_type <= 0)
					return;

				auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
				if(ln_type == culture::income_type::administration || ln_type == culture::income_type::education || ln_type == culture::income_type::military) {
					//nothing
				} else { // unemployment, pensions
					total += adj_pop_of_type * p_level * state.world.market_get_life_needs_costs(market, pt);
					if(state.world.pop_type_get_has_unemployment(pt)) {
						auto emp = state.world.state_instance_get_demographics(local_state, demographics::to_employment_key(state, pt)) / state.defines.alice_needs_scaling_factor;
						total +=
							(adj_pop_of_type - emp)
							* unemp_level
							* state.world.market_get_life_needs_costs(market, pt);
					}
				}
			});
			if(total > 0.0f) {
				add_value(std::pair<std::string, float>(text::get_dynamic_state_name(state, local_state), total * fraction));
			}
		});
	}
	add_insert_section_header(state, budget_categories::military);
	if(budget_categories::expanded[budget_categories::military]) {
		auto fraction = float(state.world.nation_get_military_spending(state.local_player_nation)) / 100.0f;
		auto in = culture::income_type::military;
		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			float total = 0.0f;
			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				auto adj_pop_of_type =
					state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt)) / state.defines.alice_needs_scaling_factor;

				if(adj_pop_of_type <= 0)
					return;

				auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
				if(ln_type == in) {
					total += adj_pop_of_type * state.world.market_get_life_needs_costs(market, pt);
				}
				auto en_type = culture::income_type(state.world.pop_type_get_everyday_needs_income_type(pt));
				if(en_type == in) {
					total += adj_pop_of_type * state.world.market_get_everyday_needs_costs(market, pt);
				}
				auto lx_type = culture::income_type(state.world.pop_type_get_luxury_needs_income_type(pt));
				if(lx_type == in) {
					total += adj_pop_of_type * state.world.market_get_luxury_needs_costs(market, pt);
				}
			});
			if(total > 0.0f) {
				add_value(std::pair<std::string, float>(text::get_dynamic_state_name(state, local_state), total * fraction));
			}
		});
	}
	add_insert_section_header(state, budget_categories::domestic_investment);
	if(budget_categories::expanded[budget_categories::domestic_investment]) {
		auto cap_total = 0.f;
		auto aristo_total = 0.f;
		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			auto adj_pop_of_type_capis = (state.world.state_instance_get_demographics(
				local_state, demographics::to_key(state, state.culture_definitions.capitalists))
				) / state.defines.alice_needs_scaling_factor;
			auto adj_pop_of_type_arist = (state.world.state_instance_get_demographics(
				local_state, demographics::to_key(state, state.culture_definitions.aristocrat))
				) / state.defines.alice_needs_scaling_factor;
			float arist_costs =
				state.world.market_get_life_needs_costs(market, state.culture_definitions.aristocrat)
				+ state.world.market_get_everyday_needs_costs(market, state.culture_definitions.aristocrat)
				+ state.world.market_get_luxury_needs_costs(market, state.culture_definitions.aristocrat);
			float capis_costs =
				state.world.market_get_life_needs_costs(market, state.culture_definitions.capitalists)
				+ state.world.market_get_everyday_needs_costs(market, state.culture_definitions.capitalists)
				+ state.world.market_get_luxury_needs_costs(market, state.culture_definitions.capitalists);

			cap_total += state.defines.alice_domestic_investment_multiplier * ( adj_pop_of_type_capis * capis_costs );
			aristo_total += state.defines.alice_domestic_investment_multiplier * (adj_pop_of_type_arist * arist_costs );
		});
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.capitalists)), cap_total * float(state.world.nation_get_domestic_investment_spending(state.local_player_nation)) / 100.0f));
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, state.world.pop_type_get_name(state.culture_definitions.aristocrat)), aristo_total * float(state.world.nation_get_domestic_investment_spending(state.local_player_nation)) / 100.0f));
	}
	add_insert_section_header(state, budget_categories::overseas_spending);
	if(budget_categories::expanded[budget_categories::overseas_spending]) {
		auto capital = state.world.nation_get_capital(state.local_player_nation);
		auto capital_state = state.world.province_get_state_membership(capital);
		auto market = state.world.state_instance_get_market_from_local_market(capital_state);

		auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(state.local_player_nation) - state.world.nation_get_central_province_count(state.local_player_nation));
		uint32_t total_commodities = state.world.commodity_size();

		if(overseas_factor > 0) {
			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

				auto kf = state.world.commodity_get_key_factory(cid);
				if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || (kf && state.world.nation_get_active_building(state.local_player_nation, kf)))) {
					auto amount =
						overseas_factor * economy::price(state, market, cid) * state.world.market_get_demand_satisfaction(market, cid) * float(state.world.nation_get_social_spending(state.local_player_nation)) / 100.0f;
					add_value(std::pair<std::string, float>(text::produce_simple_string(state, state.world.commodity_get_name(cid)), amount));
				}
			}
		}
	}
	add_insert_section_header(state, budget_categories::diplomatic_expenses);
	if(budget_categories::expanded[budget_categories::diplomatic_expenses]) {
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, "warsubsidies_button"), economy::estimate_war_subsidies_spending(state, state.local_player_nation)));
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, "alice_budget_indemnities"), economy::estimate_reparations_spending(state, state.local_player_nation)));
		add_value(std::pair<std::string, float>(text::produce_simple_string(state, "alice_budget_overlord"), economy::estimate_subject_payments_paid(state, state.local_player_nation)));
	}
	add_insert_section_header(state, budget_categories::debt_payment);
	if(budget_categories::expanded[budget_categories::debt_payment]) {
		// TODO?
	}
// END
	impl_apply_sort(state);
	impl_change_page(state, page);
}
void budgetwindow_main_expenses_table_t::on_create(sys::state& state) noexcept {
// BEGIN main::expenses_table::create
// END
}
ui::message_result budgetwindow_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_main_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
}
void budgetwindow_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
}
void budgetwindow_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("budgetwindow::main"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	auto name_key = state.lookup_key("budgetwindow::main");
	for(auto ex : state.ui_defs.extensions) {
		if(name_key && ex.window == name_key) {
			auto ch_res = ui::make_element_immediate(state, ex.child);
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		pending_children.pop_back();
		if(child_data.name == "close_button") {
			auto cptr = std::make_unique<budgetwindow_main_close_button_t>();
			cptr->parent = this;
			close_button = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "title") {
			auto cptr = std::make_unique<budgetwindow_main_title_t>();
			cptr->parent = this;
			title = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "income_label") {
			auto cptr = std::make_unique<budgetwindow_main_income_label_t>();
			cptr->parent = this;
			income_label = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "income_amount") {
			auto cptr = std::make_unique<budgetwindow_main_income_amount_t>();
			cptr->parent = this;
			income_amount = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "income_page_left") {
			auto cptr = std::make_unique<budgetwindow_main_income_page_left_t>();
			cptr->parent = this;
			income_page_left = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "income_page_right") {
			auto cptr = std::make_unique<budgetwindow_main_income_page_right_t>();
			cptr->parent = this;
			income_page_right = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "income_page_number") {
			auto cptr = std::make_unique<budgetwindow_main_income_page_number_t>();
			cptr->parent = this;
			income_page_number = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "income_table") {
			auto cptr = std::make_unique<budgetwindow_main_income_table_t>();
			cptr->parent = this;
			income_table = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			int16_t col_sz_used = 0;
			budgetwindow_main_income_table_header_t::lead_space_column_start = col_sz_used;
			budgetwindow_main_income_table_header_t::lead_space_column_width = child_data.table_columns[0].width;
			col_sz_used += child_data.table_columns[0].width; 
			budgetwindow_main_income_table_header_t::item_name_text_color = child_data.table_columns[1].header_text_color;
			budgetwindow_main_income_table_header_t::item_name_text_key = state.lookup_key(child_data.table_columns[1].header_key);
			budgetwindow_main_income_table_row_t::item_name_default_text_color = child_data.table_columns[1].cell_text_color;
			budgetwindow_main_income_table_row_t::item_name_text_alignment = child_data.table_columns[1].text_alignment;
			budgetwindow_main_income_table_header_t::item_name_column_start = col_sz_used;
			budgetwindow_main_income_table_header_t::item_name_column_width = child_data.table_columns[1].width;
			col_sz_used += child_data.table_columns[1].width; 
			budgetwindow_main_income_table_header_t::item_value_text_color = child_data.table_columns[2].header_text_color;
			budgetwindow_main_income_table_header_t::item_value_text_key = state.lookup_key(child_data.table_columns[2].header_key);
			budgetwindow_main_income_table_row_t::item_value_default_text_color = child_data.table_columns[2].cell_text_color;
			budgetwindow_main_income_table_row_t::item_value_text_alignment = child_data.table_columns[2].text_alignment;
			budgetwindow_main_income_table_header_t::item_value_column_start = col_sz_used;
			budgetwindow_main_income_table_header_t::item_value_column_width = child_data.table_columns[2].width;
			col_sz_used += child_data.table_columns[2].width; 
			budgetwindow_main_income_table_header_t::ascending_icon_key = child_data.ascending_sort_icon;
			budgetwindow_main_income_table_header_t::descending_icon_key = child_data.descending_sort_icon;
			budgetwindow_main_income_table_row_t::row_texture_key = child_data.row_background_a;
			budgetwindow_main_income_table_row_t::alt_row_texture_key = child_data.row_background_b;
			budgetwindow_main_income_table_t::row_height = child_data.row_height;
			budgetwindow_main_income_table_t::table_divider_color = child_data.table_divider_color;
			budgetwindow_main_income_table_t::table_highlight_color = child_data.table_highlight_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "expenses_page_left") {
			auto cptr = std::make_unique<budgetwindow_main_expenses_page_left_t>();
			cptr->parent = this;
			expenses_page_left = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "expenses_page_number") {
			auto cptr = std::make_unique<budgetwindow_main_expenses_page_number_t>();
			cptr->parent = this;
			expenses_page_number = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "expenses_page_right") {
			auto cptr = std::make_unique<budgetwindow_main_expenses_page_right_t>();
			cptr->parent = this;
			expenses_page_right = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "expenses_label") {
			auto cptr = std::make_unique<budgetwindow_main_expenses_label_t>();
			cptr->parent = this;
			expenses_label = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "expenses_amount") {
			auto cptr = std::make_unique<budgetwindow_main_expenses_amount_t>();
			cptr->parent = this;
			expenses_amount = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "expenses_table") {
			auto cptr = std::make_unique<budgetwindow_main_expenses_table_t>();
			cptr->parent = this;
			expenses_table = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			int16_t col_sz_used = 0;
			budgetwindow_main_expenses_table_header_t::lead_space_column_start = col_sz_used;
			budgetwindow_main_expenses_table_header_t::lead_space_column_width = child_data.table_columns[0].width;
			col_sz_used += child_data.table_columns[0].width; 
			budgetwindow_main_expenses_table_header_t::item_name_text_color = child_data.table_columns[1].header_text_color;
			budgetwindow_main_expenses_table_header_t::item_name_text_key = state.lookup_key(child_data.table_columns[1].header_key);
			budgetwindow_main_expenses_table_row_t::item_name_default_text_color = child_data.table_columns[1].cell_text_color;
			budgetwindow_main_expenses_table_row_t::item_name_text_alignment = child_data.table_columns[1].text_alignment;
			budgetwindow_main_expenses_table_header_t::item_name_column_start = col_sz_used;
			budgetwindow_main_expenses_table_header_t::item_name_column_width = child_data.table_columns[1].width;
			col_sz_used += child_data.table_columns[1].width; 
			budgetwindow_main_expenses_table_header_t::item_value_text_color = child_data.table_columns[2].header_text_color;
			budgetwindow_main_expenses_table_header_t::item_value_text_key = state.lookup_key(child_data.table_columns[2].header_key);
			budgetwindow_main_expenses_table_row_t::item_value_default_text_color = child_data.table_columns[2].cell_text_color;
			budgetwindow_main_expenses_table_row_t::item_value_text_alignment = child_data.table_columns[2].text_alignment;
			budgetwindow_main_expenses_table_header_t::item_value_column_start = col_sz_used;
			budgetwindow_main_expenses_table_header_t::item_value_column_width = child_data.table_columns[2].width;
			col_sz_used += child_data.table_columns[2].width; 
			budgetwindow_main_expenses_table_header_t::ascending_icon_key = child_data.ascending_sort_icon;
			budgetwindow_main_expenses_table_header_t::descending_icon_key = child_data.descending_sort_icon;
			budgetwindow_main_expenses_table_row_t::row_texture_key = child_data.row_background_a;
			budgetwindow_main_expenses_table_row_t::alt_row_texture_key = child_data.row_background_b;
			budgetwindow_main_expenses_table_t::row_height = child_data.row_height;
			budgetwindow_main_expenses_table_t::table_divider_color = child_data.table_divider_color;
			budgetwindow_main_expenses_table_t::table_highlight_color = child_data.table_highlight_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
	}
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_main(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_main_t>();
	ptr->on_create(state);
	return ptr;
}
void budgetwindow_section_header_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_section_header_label_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_section_header_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_section_header_label_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::label::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_text(state, text::produce_simple_string(state, "alice_budget_diplo_income")); break;
	case budget_categories::poor_tax: set_text(state, text::produce_simple_string(state, "budget_tax_poor")); break;
	case budget_categories::middle_tax: set_text(state, text::produce_simple_string(state, "budget_tax_middle")); break;
	case budget_categories::rich_tax: set_text(state, text::produce_simple_string(state, "budget_tax_rich")); break;
	case budget_categories::tariffs: set_text(state, text::produce_simple_string(state, "budget_tariffs")); break;
	case budget_categories::gold: set_text(state, text::produce_simple_string(state, "alice_budget_gold")); break;
	case budget_categories::diplomatic_expenses: set_text(state, text::produce_simple_string(state, "alice_budget_diplo_expenses")); break;
	case budget_categories::social: set_text(state, text::produce_simple_string(state, "social_spending")); break;
	case budget_categories::military: set_text(state, text::produce_simple_string(state, "military_spending")); break;
	case budget_categories::education: set_text(state, text::produce_simple_string(state, "education")); break;
	case budget_categories::admin: set_text(state, text::produce_simple_string(state, "administration")); break;
	case budget_categories::domestic_investment: set_text(state, text::produce_simple_string(state, "alice_budget_domestic_investment")); break;
	case budget_categories::overseas_spending: set_text(state, text::produce_simple_string(state, "budget_overseas")); break;
	case budget_categories::subsidies: set_text(state, text::produce_simple_string(state, "budget_industrial_subsidies")); break;
	case budget_categories::construction: set_text(state, text::produce_simple_string(state, "alice_budget_construction")); break;
	case budget_categories::army_upkeep: set_text(state, text::produce_simple_string(state, "alice_budget_army_upkeep")); break;
	case budget_categories::navy_upkeep: set_text(state, text::produce_simple_string(state, "alice_budget_navy_upkeep")); break;
	case budget_categories::debt_payment: set_text(state, text::produce_simple_string(state, "alice_budget_debt_service")); break;
	default: set_text(state, ""); break;
	}
// END
}
void budgetwindow_section_header_label_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::label::create
// END
}
ui::message_result budgetwindow_section_header_llbutton_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN section_header::llbutton::lbutton_action
	auto vals = command::make_empty_budget_settings();
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: vals.poor_tax = economy::budget_minimums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: vals.middle_tax = economy::budget_minimums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: vals.rich_tax = economy::budget_minimums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs: vals.tariffs = economy::budget_minimums(state, state.local_player_nation).tariffs; break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: vals.social_spending = economy::budget_minimums(state, state.local_player_nation).social_spending; break;
	case budget_categories::military: vals.military_spending = economy::budget_minimums(state, state.local_player_nation).military_spending; break;
	case budget_categories::education: vals.education_spending = economy::budget_minimums(state, state.local_player_nation).education_spending; break;
	case budget_categories::admin: vals.administrative_spending = economy::budget_minimums(state, state.local_player_nation).administrative_spending; break;
	case budget_categories::domestic_investment: vals.domestic_investment = economy::budget_minimums(state, state.local_player_nation).domestic_investment; break;
	case budget_categories::overseas_spending: vals.overseas = economy::budget_minimums(state, state.local_player_nation).overseas; break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: vals.construction_spending = economy::budget_minimums(state, state.local_player_nation).construction_spending; break;
	case budget_categories::army_upkeep: vals.land_spending = economy::budget_minimums(state, state.local_player_nation).land_spending; break;
	case budget_categories::navy_upkeep: vals.naval_spending = economy::budget_minimums(state, state.local_player_nation).naval_spending; break;
	case budget_categories::debt_payment: break;
	default:  break;
	}
	command::change_budget_settings(state, state.local_player_nation, vals);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_section_header_llbutton_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_section_header_llbutton_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void budgetwindow_section_header_llbutton_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_section_header_llbutton_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::llbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs: set_visible(state, true); break;
	case budget_categories::gold: set_visible(state, false); break;
	case budget_categories::diplomatic_expenses: set_visible(state, false); break;
	case budget_categories::social: set_visible(state, true); break;
	case budget_categories::military: set_visible(state, true); break;
	case budget_categories::education: set_visible(state, true); break;
	case budget_categories::admin: set_visible(state, true); break;
	case budget_categories::domestic_investment: set_visible(state, true); break;
	case budget_categories::overseas_spending: set_visible(state, true); break;
	case budget_categories::subsidies: set_visible(state, false); break;
	case budget_categories::construction: set_visible(state, true); break;
	case budget_categories::army_upkeep: set_visible(state, true); break;
	case budget_categories::navy_upkeep: set_visible(state, true); break;
	case budget_categories::debt_payment: set_visible(state, false); break;
	default: set_visible(state, false); break;
	}
// END
}
void budgetwindow_section_header_llbutton_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::llbutton::create
// END
}
ui::message_result budgetwindow_section_header_lbutton_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
	if(mods == sys::key_modifiers::modifiers_shift) {
// BEGIN section_header::lbutton::lbutton_shift_action
		auto vals = command::make_empty_budget_settings();
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: vals.poor_tax = int8_t(std::clamp(state.world.nation_get_poor_tax(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::middle_tax: vals.middle_tax = int8_t(std::clamp(state.world.nation_get_middle_tax(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::rich_tax: vals.rich_tax = int8_t(std::clamp(state.world.nation_get_rich_tax(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::tariffs: vals.tariffs = int8_t(std::clamp(state.world.nation_get_tariffs(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::gold:  break;
		case budget_categories::diplomatic_expenses:  break;
		case budget_categories::social: vals.social_spending = int8_t(std::clamp(state.world.nation_get_social_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::military: vals.military_spending = int8_t(std::clamp(state.world.nation_get_military_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::education: vals.education_spending = int8_t(std::clamp(state.world.nation_get_education_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::admin: vals.administrative_spending = int8_t(std::clamp(state.world.nation_get_administrative_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::domestic_investment: vals.domestic_investment = int8_t(std::clamp(state.world.nation_get_domestic_investment_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::overseas_spending: vals.overseas = int8_t(std::clamp(state.world.nation_get_overseas_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::subsidies: break;
		case budget_categories::construction: vals.construction_spending = int8_t(std::clamp(state.world.nation_get_construction_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::army_upkeep: vals.land_spending = int8_t(std::clamp(state.world.nation_get_land_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::navy_upkeep: vals.naval_spending = int8_t(std::clamp(state.world.nation_get_naval_spending(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::debt_payment: break;
		default:  break;
		}
		command::change_budget_settings(state, state.local_player_nation, vals);
// END
		return ui::message_result::consumed;
	}
// BEGIN section_header::lbutton::lbutton_action
	auto vals = command::make_empty_budget_settings();
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: vals.poor_tax = int8_t(std::clamp(state.world.nation_get_poor_tax(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::middle_tax: vals.middle_tax = int8_t(std::clamp(state.world.nation_get_middle_tax(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::rich_tax: vals.rich_tax = int8_t(std::clamp(state.world.nation_get_rich_tax(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::tariffs: vals.tariffs = int8_t(std::clamp(state.world.nation_get_tariffs(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: vals.social_spending = int8_t(std::clamp(state.world.nation_get_social_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::military: vals.military_spending = int8_t(std::clamp(state.world.nation_get_military_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::education: vals.education_spending = int8_t(std::clamp(state.world.nation_get_education_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::admin: vals.administrative_spending = int8_t(std::clamp(state.world.nation_get_administrative_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::domestic_investment: vals.domestic_investment = int8_t(std::clamp(state.world.nation_get_domestic_investment_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::overseas_spending: vals.overseas = int8_t(std::clamp(state.world.nation_get_overseas_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: vals.construction_spending = int8_t(std::clamp(state.world.nation_get_construction_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::army_upkeep: vals.land_spending = int8_t(std::clamp(state.world.nation_get_land_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::navy_upkeep: vals.naval_spending = int8_t(std::clamp(state.world.nation_get_naval_spending(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::debt_payment: break;
	default:  break;
	}
	command::change_budget_settings(state, state.local_player_nation, vals);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_section_header_lbutton_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_section_header_lbutton_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void budgetwindow_section_header_lbutton_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_section_header_lbutton_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::lbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs: set_visible(state, true); break;
	case budget_categories::gold: set_visible(state, false); break;
	case budget_categories::diplomatic_expenses: set_visible(state, false); break;
	case budget_categories::social: set_visible(state, true); break;
	case budget_categories::military: set_visible(state, true); break;
	case budget_categories::education: set_visible(state, true); break;
	case budget_categories::admin: set_visible(state, true); break;
	case budget_categories::domestic_investment: set_visible(state, true); break;
	case budget_categories::overseas_spending: set_visible(state, true); break;
	case budget_categories::subsidies: set_visible(state, false); break;
	case budget_categories::construction: set_visible(state, true); break;
	case budget_categories::army_upkeep: set_visible(state, true); break;
	case budget_categories::navy_upkeep: set_visible(state, true); break;
	case budget_categories::debt_payment: set_visible(state, false); break;
	default: set_visible(state, false); break;
	}
// END
}
void budgetwindow_section_header_lbutton_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::lbutton::create
// END
}
ui::message_result budgetwindow_section_header_rbutton_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
	if(mods == sys::key_modifiers::modifiers_shift) {
// BEGIN section_header::rbutton::lbutton_shift_action
		auto vals = command::make_empty_budget_settings();
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: vals.poor_tax = int8_t(std::clamp(state.world.nation_get_poor_tax(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::middle_tax: vals.middle_tax = int8_t(std::clamp(state.world.nation_get_middle_tax(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::rich_tax: vals.rich_tax = int8_t(std::clamp(state.world.nation_get_rich_tax(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::tariffs: vals.tariffs = int8_t(std::clamp(state.world.nation_get_tariffs(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::gold:  break;
		case budget_categories::diplomatic_expenses:  break;
		case budget_categories::social: vals.social_spending = int8_t(std::clamp(state.world.nation_get_social_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::military: vals.military_spending = int8_t(std::clamp(state.world.nation_get_military_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::education: vals.education_spending = int8_t(std::clamp(state.world.nation_get_education_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::admin: vals.administrative_spending = int8_t(std::clamp(state.world.nation_get_administrative_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::domestic_investment: vals.domestic_investment = int8_t(std::clamp(state.world.nation_get_domestic_investment_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::overseas_spending: vals.overseas = int8_t(std::clamp(state.world.nation_get_overseas_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::subsidies: break;
		case budget_categories::construction: vals.construction_spending = int8_t(std::clamp(state.world.nation_get_construction_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::army_upkeep: vals.land_spending = int8_t(std::clamp(state.world.nation_get_land_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::navy_upkeep: vals.naval_spending = int8_t(std::clamp(state.world.nation_get_naval_spending(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::debt_payment: break;
		default:  break;
		}
		command::change_budget_settings(state, state.local_player_nation, vals);
// END
		return ui::message_result::consumed;
	}
// BEGIN section_header::rbutton::lbutton_action
	auto vals = command::make_empty_budget_settings();
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: vals.poor_tax = int8_t(std::clamp(state.world.nation_get_poor_tax(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::middle_tax: vals.middle_tax = int8_t(std::clamp(state.world.nation_get_middle_tax(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::rich_tax: vals.rich_tax = int8_t(std::clamp(state.world.nation_get_rich_tax(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::tariffs: vals.tariffs = int8_t(std::clamp(state.world.nation_get_tariffs(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: vals.social_spending = int8_t(std::clamp(state.world.nation_get_social_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::military: vals.military_spending = int8_t(std::clamp(state.world.nation_get_military_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::education: vals.education_spending = int8_t(std::clamp(state.world.nation_get_education_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::admin: vals.administrative_spending = int8_t(std::clamp(state.world.nation_get_administrative_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::domestic_investment: vals.domestic_investment = int8_t(std::clamp(state.world.nation_get_domestic_investment_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::overseas_spending: vals.overseas = int8_t(std::clamp(state.world.nation_get_overseas_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: vals.construction_spending = int8_t(std::clamp(state.world.nation_get_construction_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::army_upkeep: vals.land_spending = int8_t(std::clamp(state.world.nation_get_land_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::navy_upkeep: vals.naval_spending = int8_t(std::clamp(state.world.nation_get_naval_spending(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::debt_payment: break;
	default:  break;
	}
	command::change_budget_settings(state, state.local_player_nation, vals);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_section_header_rbutton_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_section_header_rbutton_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void budgetwindow_section_header_rbutton_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_section_header_rbutton_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::rbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs: set_visible(state, true); break;
	case budget_categories::gold: set_visible(state, false); break;
	case budget_categories::diplomatic_expenses: set_visible(state, false); break;
	case budget_categories::social: set_visible(state, true); break;
	case budget_categories::military: set_visible(state, true); break;
	case budget_categories::education: set_visible(state, true); break;
	case budget_categories::admin: set_visible(state, true); break;
	case budget_categories::domestic_investment: set_visible(state, true); break;
	case budget_categories::overseas_spending: set_visible(state, true); break;
	case budget_categories::subsidies: set_visible(state, false); break;
	case budget_categories::construction: set_visible(state, true); break;
	case budget_categories::army_upkeep: set_visible(state, true); break;
	case budget_categories::navy_upkeep: set_visible(state, true); break;
	case budget_categories::debt_payment: set_visible(state, false); break;
	default: set_visible(state, false); break;
	}
// END
}
void budgetwindow_section_header_rbutton_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::rbutton::create
// END
}
ui::message_result budgetwindow_section_header_rrbutton_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN section_header::rrbutton::lbutton_action
	auto vals = command::make_empty_budget_settings();
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: vals.poor_tax = economy::budget_maximums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: vals.middle_tax = economy::budget_maximums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: vals.rich_tax = economy::budget_maximums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs: vals.tariffs = economy::budget_maximums(state, state.local_player_nation).tariffs; break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: vals.social_spending = economy::budget_maximums(state, state.local_player_nation).social_spending; break;
	case budget_categories::military: vals.military_spending = economy::budget_maximums(state, state.local_player_nation).military_spending; break;
	case budget_categories::education: vals.education_spending = economy::budget_maximums(state, state.local_player_nation).education_spending; break;
	case budget_categories::admin: vals.administrative_spending = economy::budget_maximums(state, state.local_player_nation).administrative_spending; break;
	case budget_categories::domestic_investment: vals.domestic_investment = economy::budget_maximums(state, state.local_player_nation).domestic_investment; break;
	case budget_categories::overseas_spending: vals.overseas = economy::budget_maximums(state, state.local_player_nation).overseas; break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: vals.construction_spending = economy::budget_maximums(state, state.local_player_nation).construction_spending; break;
	case budget_categories::army_upkeep: vals.land_spending = economy::budget_maximums(state, state.local_player_nation).land_spending; break;
	case budget_categories::navy_upkeep: vals.naval_spending = economy::budget_maximums(state, state.local_player_nation).naval_spending; break;
	case budget_categories::debt_payment: break;
	default:  break;
	}
	command::change_budget_settings(state, state.local_player_nation, vals);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_section_header_rrbutton_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_section_header_rrbutton_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void budgetwindow_section_header_rrbutton_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_section_header_rrbutton_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::rrbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs: set_visible(state, true); break;
	case budget_categories::gold: set_visible(state, false); break;
	case budget_categories::diplomatic_expenses: set_visible(state, false); break;
	case budget_categories::social: set_visible(state, true); break;
	case budget_categories::military: set_visible(state, true); break;
	case budget_categories::education: set_visible(state, true); break;
	case budget_categories::admin: set_visible(state, true); break;
	case budget_categories::domestic_investment: set_visible(state, true); break;
	case budget_categories::overseas_spending: set_visible(state, true); break;
	case budget_categories::subsidies: set_visible(state, false); break;
	case budget_categories::construction: set_visible(state, true); break;
	case budget_categories::army_upkeep: set_visible(state, true); break;
	case budget_categories::navy_upkeep: set_visible(state, true); break;
	case budget_categories::debt_payment: set_visible(state, false); break;
	default: set_visible(state, false); break;
	}
// END
}
void budgetwindow_section_header_rrbutton_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::rrbutton::create
// END
}
void budgetwindow_section_header_setting_amount_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_section_header_setting_amount_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_section_header_setting_amount_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_section_header_setting_amount_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::setting_amount::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_text(state, ""); break;
	case budget_categories::poor_tax: set_text(state, std::to_string(state.world.nation_get_poor_tax(state.local_player_nation))); break;
	case budget_categories::middle_tax: set_text(state, std::to_string(state.world.nation_get_middle_tax(state.local_player_nation))); break;
	case budget_categories::rich_tax: set_text(state, std::to_string(state.world.nation_get_rich_tax(state.local_player_nation))); break;
	case budget_categories::tariffs: set_text(state, std::to_string(state.world.nation_get_tariffs(state.local_player_nation))); break;
	case budget_categories::gold: set_text(state, ""); break;
	case budget_categories::diplomatic_expenses: set_text(state, ""); break;
	case budget_categories::social: set_text(state, std::to_string(state.world.nation_get_social_spending(state.local_player_nation))); break;
	case budget_categories::military: set_text(state, std::to_string(state.world.nation_get_military_spending(state.local_player_nation))); break;
	case budget_categories::education: set_text(state, std::to_string(state.world.nation_get_education_spending(state.local_player_nation))); break;
	case budget_categories::admin: set_text(state, std::to_string(state.world.nation_get_administrative_spending(state.local_player_nation))); break;
	case budget_categories::domestic_investment: set_text(state, std::to_string(state.world.nation_get_domestic_investment_spending(state.local_player_nation))); break;
	case budget_categories::overseas_spending: set_text(state, std::to_string(state.world.nation_get_overseas_spending(state.local_player_nation))); break;
	case budget_categories::subsidies: set_text(state, ""); break;
	case budget_categories::construction: set_text(state, std::to_string(state.world.nation_get_construction_spending(state.local_player_nation))); break;
	case budget_categories::army_upkeep: set_text(state, std::to_string(state.world.nation_get_land_spending(state.local_player_nation))); break;
	case budget_categories::navy_upkeep: set_text(state, std::to_string(state.world.nation_get_naval_spending(state.local_player_nation))); break;
	case budget_categories::debt_payment: set_text(state, ""); break;
	default: set_text(state, ""); break;
	}
// END
}
void budgetwindow_section_header_setting_amount_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::setting_amount::create
// END
}
ui::message_result budgetwindow_section_header_expand_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN section_header::expand_button::lbutton_action
	budget_categories::expanded[section_header.section_type] = !budget_categories::expanded[section_header.section_type];
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_section_header_expand_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_section_header_expand_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::expand_button::tooltip
	if(!budget_categories::expanded[section_header.section_type])
		text::add_line(state, contents, "alice_budget_expand_tt");
	else
		text::add_line(state, contents, "alice_budget_contract_tt");
// END
}
void budgetwindow_section_header_expand_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
	else
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_section_header_expand_button_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::expand_button::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: disabled = (economy::estimate_diplomatic_income(state, state.local_player_nation) <= 0); break;
	case budget_categories::poor_tax: disabled = false; break;
	case budget_categories::middle_tax: disabled = false; break;
	case budget_categories::rich_tax: disabled = false; break;
	case budget_categories::tariffs: disabled = (economy::estimate_tariff_income(state, state.local_player_nation) <= 0); break;
	case budget_categories::gold: disabled = (economy::estimate_gold_income(state, state.local_player_nation) <= 0); break;
	case budget_categories::diplomatic_expenses: disabled = (economy::estimate_diplomatic_expenses(state, state.local_player_nation) <= 0); break;
	case budget_categories::social: disabled = (economy::estimate_social_spending(state, state.local_player_nation) <= 0); break;
	case budget_categories::military: disabled = false; break;
	case budget_categories::education: disabled = false; break;
	case budget_categories::admin: disabled = false; break;
	case budget_categories::domestic_investment: disabled = false; break;
	case budget_categories::overseas_spending: disabled = (economy::estimate_overseas_penalty_spending(state, state.local_player_nation) <= 0); break;
	case budget_categories::subsidies: disabled = (economy::estimate_subsidy_spending(state, state.local_player_nation) <= 0); break;
	case budget_categories::construction: disabled = (economy::estimate_construction_spending(state, state.local_player_nation) <= 0); break;
	case budget_categories::army_upkeep: disabled = (economy::estimate_land_spending(state, state.local_player_nation) <= 0); break;
	case budget_categories::navy_upkeep:disabled = (economy::estimate_naval_spending(state, state.local_player_nation) <= 0); break;
	case budget_categories::debt_payment: disabled = (economy::interest_payment(state, state.local_player_nation) <= 0); break;
	default: disabled = false; break;
	}

	is_active = !(!disabled && budget_categories::expanded[section_header.section_type]);

// END
}
void budgetwindow_section_header_expand_button_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::expand_button::create
// END
}
void budgetwindow_section_header_total_amount_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_section_header_total_amount_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_section_header_total_amount_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_section_header_total_amount_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::total_amount::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_text(state, text::prettify_currency(economy::estimate_diplomatic_income(state, state.local_player_nation))); break;
	case budget_categories::poor_tax: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::poor) * float(state.world.nation_get_poor_tax(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::middle_tax: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::middle) * float(state.world.nation_get_middle_tax(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::rich_tax: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_tax_income_by_strata(state, state.local_player_nation, culture::pop_strata::rich) * float(state.world.nation_get_rich_tax(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::tariffs: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_tariff_income(state, state.local_player_nation) * float(state.world.nation_get_tariffs(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::gold: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_gold_income(state, state.local_player_nation)))); break;
	case budget_categories::diplomatic_expenses: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_diplomatic_expenses(state, state.local_player_nation)))); break;
	case budget_categories::social: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_social_spending(state, state.local_player_nation) * float(state.world.nation_get_social_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::military: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military) * float(state.world.nation_get_military_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::education: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education) * float(state.world.nation_get_education_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::admin: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::administration) * float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::domestic_investment: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_domestic_investment(state, state.local_player_nation) * float(state.world.nation_get_domestic_investment_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::overseas_spending: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_overseas_penalty_spending(state, state.local_player_nation) * float(state.world.nation_get_overseas_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::subsidies: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_subsidy_spending(state, state.local_player_nation)))); break;
	case budget_categories::construction: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_construction_spending(state, state.local_player_nation) * float(state.world.nation_get_construction_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::army_upkeep: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_land_spending(state, state.local_player_nation) * float(state.world.nation_get_land_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::navy_upkeep: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::estimate_naval_spending(state, state.local_player_nation) * float(state.world.nation_get_naval_spending(state.local_player_nation)) / 100.0f))); break;
	case budget_categories::debt_payment: set_text(state, text::produce_simple_string(state, text::prettify_currency(economy::interest_payment(state, state.local_player_nation)))); break;
	default: set_text(state, ""); break;
	}
// END
}
void budgetwindow_section_header_total_amount_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::total_amount::create
// END
}
void budgetwindow_section_header_min_setting_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::min_setting::tooltip
	auto value = 0;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_minimums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_minimums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_minimums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs: value = economy::budget_minimums(state, state.local_player_nation).tariffs; break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: value = economy::budget_minimums(state, state.local_player_nation).social_spending; break;
	case budget_categories::military: value = economy::budget_minimums(state, state.local_player_nation).military_spending; break;
	case budget_categories::education: value = economy::budget_minimums(state, state.local_player_nation).education_spending; break;
	case budget_categories::admin: value = economy::budget_minimums(state, state.local_player_nation).administrative_spending; break;
	case budget_categories::domestic_investment: value = economy::budget_minimums(state, state.local_player_nation).domestic_investment; break;
	case budget_categories::overseas_spending: value = economy::budget_minimums(state, state.local_player_nation).overseas; break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: value = economy::budget_minimums(state, state.local_player_nation).construction_spending; break;
	case budget_categories::army_upkeep: value = economy::budget_minimums(state, state.local_player_nation).land_spending; break;
	case budget_categories::navy_upkeep: value = economy::budget_minimums(state, state.local_player_nation).naval_spending; break;
	case budget_categories::debt_payment: break;
	default:  break;
	}
	if(value == 0) {

	} else {
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tax, true); break;
		case budget_categories::middle_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tax, true); break;
		case budget_categories::rich_tax:  ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tax, true); break;
		case budget_categories::tariffs: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tariff, true); break;
		case budget_categories::gold:  break;
		case budget_categories::diplomatic_expenses:  break;
		case budget_categories::social: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_social_spending, true); break;
		case budget_categories::military: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_military_spending, true); break;
		case budget_categories::education:  break;
		case budget_categories::admin:  break;
		case budget_categories::domestic_investment: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_domestic_investment, true); break;
		case budget_categories::overseas_spending: break;
		case budget_categories::subsidies: break;
		case budget_categories::construction: break;
		case budget_categories::army_upkeep: break;
		case budget_categories::navy_upkeep: break;
		case budget_categories::debt_payment: break;
		default:  break;
		}
	}
// END
}
void budgetwindow_section_header_min_setting_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_section_header_min_setting_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_section_header_min_setting_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_section_header_min_setting_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::min_setting::update
	auto value = 0;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_minimums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_minimums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_minimums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs: value = economy::budget_minimums(state, state.local_player_nation).tariffs; break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: value = economy::budget_minimums(state, state.local_player_nation).social_spending; break;
	case budget_categories::military: value = economy::budget_minimums(state, state.local_player_nation).military_spending; break;
	case budget_categories::education: value = economy::budget_minimums(state, state.local_player_nation).education_spending; break;
	case budget_categories::admin: value = economy::budget_minimums(state, state.local_player_nation).administrative_spending; break;
	case budget_categories::domestic_investment: value = economy::budget_minimums(state, state.local_player_nation).domestic_investment; break;
	case budget_categories::overseas_spending: value = economy::budget_minimums(state, state.local_player_nation).overseas; break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: value = economy::budget_minimums(state, state.local_player_nation).construction_spending; break;
	case budget_categories::army_upkeep: value = economy::budget_minimums(state, state.local_player_nation).land_spending; break;
	case budget_categories::navy_upkeep: value = economy::budget_minimums(state, state.local_player_nation).naval_spending; break;
	case budget_categories::debt_payment: break;
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
void budgetwindow_section_header_min_setting_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::min_setting::create
// END
}
void budgetwindow_section_header_max_setting_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::max_setting::tooltip
	auto value = 100;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_maximums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_maximums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_maximums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs: value = economy::budget_maximums(state, state.local_player_nation).tariffs; break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: value = economy::budget_maximums(state, state.local_player_nation).social_spending; break;
	case budget_categories::military: value = economy::budget_maximums(state, state.local_player_nation).military_spending; break;
	case budget_categories::education: value = economy::budget_maximums(state, state.local_player_nation).education_spending; break;
	case budget_categories::admin: value = economy::budget_maximums(state, state.local_player_nation).administrative_spending; break;
	case budget_categories::domestic_investment: value = economy::budget_maximums(state, state.local_player_nation).domestic_investment; break;
	case budget_categories::overseas_spending: value = economy::budget_maximums(state, state.local_player_nation).overseas; break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: value = economy::budget_maximums(state, state.local_player_nation).construction_spending; break;
	case budget_categories::army_upkeep: value = economy::budget_maximums(state, state.local_player_nation).land_spending; break;
	case budget_categories::navy_upkeep: value = economy::budget_maximums(state, state.local_player_nation).naval_spending; break;
	case budget_categories::debt_payment: break;
	default:  break;
	}
	if(value == 100) {

	} else {
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tax, true); break;
		case budget_categories::middle_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tax, true); break;
		case budget_categories::rich_tax:  ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tax, true); break;
		case budget_categories::tariffs: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tariff, true); break;
		case budget_categories::gold:  break;
		case budget_categories::diplomatic_expenses:  break;
		case budget_categories::social: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_social_spending, true); break;
		case budget_categories::military: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_military_spending, true); break;
		case budget_categories::education:  break;
		case budget_categories::admin:  break;
		case budget_categories::domestic_investment: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_domestic_investment, true); break;
		case budget_categories::overseas_spending: break;
		case budget_categories::subsidies: break;
		case budget_categories::construction: break;
		case budget_categories::army_upkeep: break;
		case budget_categories::navy_upkeep: break;
		case budget_categories::debt_payment: break;
		default:  break;
		}
	}
// END
}
void budgetwindow_section_header_max_setting_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_section_header_max_setting_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_section_header_max_setting_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_section_header_max_setting_t::on_update(sys::state& state) noexcept {
	budgetwindow_section_header_t& section_header = *((budgetwindow_section_header_t*)(parent)); 
// BEGIN section_header::max_setting::update
	auto value = 100;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_maximums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_maximums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_maximums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs: value = economy::budget_maximums(state, state.local_player_nation).tariffs; break;
	case budget_categories::gold:  break;
	case budget_categories::diplomatic_expenses:  break;
	case budget_categories::social: value = economy::budget_maximums(state, state.local_player_nation).social_spending; break;
	case budget_categories::military: value = economy::budget_maximums(state, state.local_player_nation).military_spending; break;
	case budget_categories::education: value = economy::budget_maximums(state, state.local_player_nation).education_spending; break;
	case budget_categories::admin: value = economy::budget_maximums(state, state.local_player_nation).administrative_spending; break;
	case budget_categories::domestic_investment: value = economy::budget_maximums(state, state.local_player_nation).domestic_investment; break;
	case budget_categories::overseas_spending: value = economy::budget_maximums(state, state.local_player_nation).overseas; break;
	case budget_categories::subsidies: break;
	case budget_categories::construction: value = economy::budget_maximums(state, state.local_player_nation).construction_spending; break;
	case budget_categories::army_upkeep: value = economy::budget_maximums(state, state.local_player_nation).land_spending; break;
	case budget_categories::navy_upkeep: value = economy::budget_maximums(state, state.local_player_nation).naval_spending; break;
	case budget_categories::debt_payment: break;
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
void budgetwindow_section_header_max_setting_t::on_create(sys::state& state) noexcept {
// BEGIN section_header::max_setting::create
// END
}
ui::message_result budgetwindow_section_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_section_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_section_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
}
void budgetwindow_section_header_t::on_update(sys::state& state) noexcept {
// BEGIN section_header::update
// END
}
void budgetwindow_section_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("budgetwindow::section_header"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	auto name_key = state.lookup_key("budgetwindow::section_header");
	for(auto ex : state.ui_defs.extensions) {
		if(name_key && ex.window == name_key) {
			auto ch_res = ui::make_element_immediate(state, ex.child);
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		pending_children.pop_back();
		if(child_data.name == "label") {
			auto cptr = std::make_unique<budgetwindow_section_header_label_t>();
			cptr->parent = this;
			label = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "llbutton") {
			auto cptr = std::make_unique<budgetwindow_section_header_llbutton_t>();
			cptr->parent = this;
			llbutton = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "lbutton") {
			auto cptr = std::make_unique<budgetwindow_section_header_lbutton_t>();
			cptr->parent = this;
			lbutton = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "rbutton") {
			auto cptr = std::make_unique<budgetwindow_section_header_rbutton_t>();
			cptr->parent = this;
			rbutton = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "rrbutton") {
			auto cptr = std::make_unique<budgetwindow_section_header_rrbutton_t>();
			cptr->parent = this;
			rrbutton = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "setting_amount") {
			auto cptr = std::make_unique<budgetwindow_section_header_setting_amount_t>();
			cptr->parent = this;
			setting_amount = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "expand_button") {
			auto cptr = std::make_unique<budgetwindow_section_header_expand_button_t>();
			cptr->parent = this;
			expand_button = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->alt_texture_key = child_data.alt_texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "total_amount") {
			auto cptr = std::make_unique<budgetwindow_section_header_total_amount_t>();
			cptr->parent = this;
			total_amount = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "min_setting") {
			auto cptr = std::make_unique<budgetwindow_section_header_min_setting_t>();
			cptr->parent = this;
			min_setting = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "max_setting") {
			auto cptr = std::make_unique<budgetwindow_section_header_max_setting_t>();
			cptr->parent = this;
			max_setting = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
	}
// BEGIN section_header::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_section_header(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_section_header_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
