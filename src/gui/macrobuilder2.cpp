namespace alice_ui {
struct macrobuilder2_main_close_button_t : public ui::element_base {
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
struct macrobuilder2_main_header_text_t : public ui::element_base {
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.500000f; 
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
struct macrobuilder2_main_template_list_t : public ui::container_base {
	int32_t page = 0;
	std::vector<int32_t> values;
	std::vector<ui::element_base*> visible_items;
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
			return (values.size() > visible_items.size()) ? ui::message_result::consumed : ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_scroll(sys::state & state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override; 
	void change_page(sys::state & state, int32_t new_page); 
	int32_t max_page(); 
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_main_unit_grid_t : public ui::container_base {
	int32_t page = 0;
	std::vector<dcon::unit_type_id> values;
	std::vector<ui::element_base*> visible_items;
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
			return (values.size() > visible_items.size()) ? ui::message_result::consumed : ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_scroll(sys::state & state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override; 
	void change_page(sys::state & state, int32_t new_page); 
	int32_t max_page(); 
	void on_update(sys::state& state) noexcept override;
};
struct macrobuilder2_main_list_page_left_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct macrobuilder2_main_list_page_right_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct macrobuilder2_main_grid_page_left_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct macrobuilder2_main_grid_page_right_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct macrobuilder2_main_list_page_number_t : public ui::element_base {
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
struct macrobuilder2_main_grid_page_number_t : public ui::element_base {
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
struct macrobuilder2_main_apply_button_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
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
struct macrobuilder2_list_item_select_button_t : public ui::element_base {
	std::string_view texture_key;
	std::string_view alt_texture_key;
	dcon::texture_id alt_background_texture;
	bool is_active = false;
	dcon::texture_id background_texture;
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
struct macrobuilder2_list_item_delete_button_t : public ui::element_base {
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
struct macrobuilder2_grid_item_unit_icon_t : public ui::element_base {
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
struct macrobuilder2_grid_item_decrease_count_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct macrobuilder2_grid_item_increase_count_t : public ui::element_base {
	std::string_view texture_key;
	dcon::texture_id background_texture;
	bool disabled = false;
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
struct macrobuilder2_grid_item_current_count_t : public ui::element_base {
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
struct macrobuilder2_main_t : public ui::container_base {
	macrobuilder2_main_close_button_t* close_button = nullptr;
	macrobuilder2_main_header_text_t* header_text = nullptr;
	macrobuilder2_main_template_list_t* template_list = nullptr;
	macrobuilder2_main_unit_grid_t* unit_grid = nullptr;
	macrobuilder2_main_list_page_left_t* list_page_left = nullptr;
	macrobuilder2_main_list_page_right_t* list_page_right = nullptr;
	macrobuilder2_main_grid_page_left_t* grid_page_left = nullptr;
	macrobuilder2_main_grid_page_right_t* grid_page_right = nullptr;
	macrobuilder2_main_list_page_number_t* list_page_number = nullptr;
	macrobuilder2_main_grid_page_number_t* grid_page_number = nullptr;
	macrobuilder2_main_apply_button_t* apply_button = nullptr;
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
std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state);
struct macrobuilder2_list_item_t : public ui::container_base {
	int32_t value;
	macrobuilder2_list_item_select_button_t* select_button = nullptr;
	macrobuilder2_list_item_delete_button_t* delete_button = nullptr;
	void on_create(sys::state& state) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_macrobuilder2_list_item(sys::state& state);
struct macrobuilder2_grid_item_t : public ui::container_base {
	dcon::unit_type_id value;
	macrobuilder2_grid_item_unit_icon_t* unit_icon = nullptr;
	macrobuilder2_grid_item_decrease_count_t* decrease_count = nullptr;
	macrobuilder2_grid_item_increase_count_t* increase_count = nullptr;
	macrobuilder2_grid_item_current_count_t* current_count = nullptr;
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
};
std::unique_ptr<ui::element_base> make_macrobuilder2_grid_item(sys::state& state);
ui::message_result macrobuilder2_main_close_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::close_button::lbutton_action
	main.set_visible(state, false);
	auto sdir = simple_fs::get_or_create_templates_directory();
	simple_fs::write_file(sdir, state.loaded_scenario_file, reinterpret_cast<const char*>(state.ui_state.templates.data()), uint32_t(state.ui_state.templates.size()) * sizeof(sys::macro_builder_template));
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_close_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_close_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void macrobuilder2_main_close_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_main_close_button_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::close_button::update
// END
}
void macrobuilder2_main_close_button_t::on_create(sys::state& state) noexcept {
// BEGIN main::close_button::create
// END
}
void macrobuilder2_main_header_text_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void macrobuilder2_main_header_text_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void macrobuilder2_main_header_text_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 18);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void macrobuilder2_main_header_text_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::header_text::update
// END
}
void macrobuilder2_main_header_text_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::header_text::create
// END
}
int32_t macrobuilder2_main_template_list_t::max_page(){
	if(values.size() == 0) return 0;
	if(visible_items.size() == 0) return 0;
	return int32_t(values.size() - 1) / int32_t(visible_items.size());
}
void macrobuilder2_main_template_list_t::change_page(sys::state & state, int32_t new_page) {
	page = std::clamp(new_page, 0, max_page());
	state.game_state_updated.store(true, std::memory_order::release);
}
ui::message_result macrobuilder2_main_template_list_t::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	change_page(state, page + ((amount < 0) ? 1 : -1));
	return ui::message_result::consumed;
}
void macrobuilder2_main_template_list_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::template_list::update
// END
	page = std::clamp(page, 0, max_page());
	auto fill_count = visible_items.size();
	auto fill_start = size_t(page) * fill_count;
	for(size_t fill_position = 0; fill_position < fill_count; ++fill_position) {
		if(fill_position + fill_start < values.size()) {
			if( ((macrobuilder2_list_item_t*)visible_items[fill_position])->value != values[fill_position + fill_start] && visible_items[fill_position]->is_visible()) {
				((macrobuilder2_list_item_t*)visible_items[fill_position])->value = values[fill_position + fill_start];
				visible_items[fill_position]->impl_on_update(state);
			} else {
				((macrobuilder2_list_item_t*)visible_items[fill_position])->value = values[fill_position+ fill_start];
				visible_items[fill_position]->set_visible(state, true);
			}
		} else {
			visible_items[fill_position]->set_visible(state, false);
		}
	}
}
void macrobuilder2_main_template_list_t::on_create(sys::state& state) noexcept {
	auto ptr = make_macrobuilder2_list_item(state);
	int32_t item_y_size = ptr->base_data.size.y;
	visible_items.push_back(ptr.get());
	add_child_to_back(std::move(ptr));
	auto total_rows = int32_t(base_data.size.y) / item_y_size;
	for(int32_t i = 1; i < total_rows; ++i) {
		auto ptrb = make_macrobuilder2_list_item(state);
		ptrb->base_data.position.y = int16_t(i * item_y_size);
		visible_items.push_back(ptrb.get());
		add_child_to_back(std::move(ptrb));
	}
// BEGIN main::template_list::create
	auto sdir = simple_fs::get_or_create_templates_directory();
	auto f = simple_fs::open_file(sdir, state.loaded_scenario_file);
	if(f) {
		auto contents = simple_fs::view_contents(*f);
	
		uint32_t num_templates = contents.file_size / sizeof(sys::macro_builder_template);
		//Corruption protection
		if(num_templates >= 8192 * 4)
			num_templates = 8192 * 4;
		state.ui_state.templates.resize(num_templates);
		std::memcpy(state.ui_state.templates.data(), contents.data, num_templates * sizeof(sys::macro_builder_template));

		for(int32_t i = -1; i < int32_t(num_templates); ++i) {
			values.push_back(i);
		}
	} else {
		values.push_back(-1);
	}
// END
}
int32_t macrobuilder2_main_unit_grid_t::max_page(){
	if(values.size() == 0) return 0;
	if(visible_items.size() == 0) return 0;
	return int32_t(values.size() - 1) / int32_t(visible_items.size());
}
void macrobuilder2_main_unit_grid_t::change_page(sys::state & state, int32_t new_page) {
	page = std::clamp(new_page, 0, max_page());
	state.game_state_updated.store(true, std::memory_order::release);
}
ui::message_result macrobuilder2_main_unit_grid_t::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	change_page(state, page + ((amount < 0) ? 1 : -1));
	return ui::message_result::consumed;
}
void macrobuilder2_main_unit_grid_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::unit_grid::update
// END
	page = std::clamp(page, 0, max_page());
	auto fill_count = visible_items.size();
	auto fill_start = size_t(page) * fill_count;
	for(size_t fill_position = 0; fill_position < fill_count; ++fill_position) {
		if(fill_position + fill_start < values.size()) {
			if( ((macrobuilder2_grid_item_t*)visible_items[fill_position])->value != values[fill_position + fill_start] && visible_items[fill_position]->is_visible()) {
				((macrobuilder2_grid_item_t*)visible_items[fill_position])->value = values[fill_position + fill_start];
				visible_items[fill_position]->impl_on_update(state);
			} else {
				((macrobuilder2_grid_item_t*)visible_items[fill_position])->value = values[fill_position+ fill_start];
				visible_items[fill_position]->set_visible(state, true);
			}
		} else {
			visible_items[fill_position]->set_visible(state, false);
		}
	}
}
void macrobuilder2_main_unit_grid_t::on_create(sys::state& state) noexcept {
	auto ptr = make_macrobuilder2_grid_item(state);
	int32_t item_y_size = ptr->base_data.size.y;
	int32_t item_x_size = ptr->base_data.size.x;
	visible_items.push_back(ptr.get());
	add_child_to_back(std::move(ptr));
	auto total_rows = int32_t(base_data.size.y) / item_y_size;
	auto total_columns = int32_t(base_data.size.x) / item_x_size;
	for(int32_t i = 0; i < total_rows; ++i) {
		for(int32_t j = (i == 0 ? 1 : 0); j < total_columns; ++j) {
			auto ptrb = make_macrobuilder2_grid_item(state);
			ptrb->base_data.position.y = int16_t(i * item_y_size);
			ptrb->base_data.position.x = int16_t(j * item_x_size);
			visible_items.push_back(ptrb.get());
			add_child_to_back(std::move(ptrb));
		}
	}
// BEGIN main::unit_grid::create
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(state.military_definitions.unit_base_definitions[id].is_land) {
			values.push_back(id);
		}
	}
	while((values.size() % visible_items.size()) != 0) {
		values.push_back(dcon::unit_type_id{});
	}
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(!state.military_definitions.unit_base_definitions[id].is_land) {
			values.push_back(id);
		}
	}
// END
}
ui::message_result macrobuilder2_main_list_page_left_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::list_page_left::lbutton_action
	main.template_list->change_page(state, main.template_list->page - 1);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_list_page_left_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_list_page_left_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_main_list_page_left_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::list_page_left::update
	if(main.template_list->values.size() > main.template_list->visible_items.size()) {
		element_base::flags &= ~element_base::is_invisible_mask;
	} else {
		element_base::flags |= element_base::is_invisible_mask;
	}
// END
}
void macrobuilder2_main_list_page_left_t::on_create(sys::state& state) noexcept {
// BEGIN main::list_page_left::create
// END
}
ui::message_result macrobuilder2_main_list_page_right_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::list_page_right::lbutton_action
	main.template_list->change_page(state, main.template_list->page + 1);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_list_page_right_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_list_page_right_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_main_list_page_right_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::list_page_right::update
	if(main.template_list->values.size() > main.template_list->visible_items.size()) {
		element_base::flags &= ~element_base::is_invisible_mask;
	} else {
		element_base::flags |= element_base::is_invisible_mask;
	}
// END
}
void macrobuilder2_main_list_page_right_t::on_create(sys::state& state) noexcept {
// BEGIN main::list_page_right::create
// END
}
ui::message_result macrobuilder2_main_grid_page_left_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::grid_page_left::lbutton_action
	main.unit_grid->change_page(state, main.unit_grid->page - 1);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_grid_page_left_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_grid_page_left_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_main_grid_page_left_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::grid_page_left::update
	if(main.unit_grid->values.size() > main.unit_grid->visible_items.size()) {
		element_base::flags &= ~element_base::is_invisible_mask;
	} else {
		element_base::flags |= element_base::is_invisible_mask;
	}
// END
}
void macrobuilder2_main_grid_page_left_t::on_create(sys::state& state) noexcept {
// BEGIN main::grid_page_left::create
// END
}
ui::message_result macrobuilder2_main_grid_page_right_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::grid_page_right::lbutton_action
	main.unit_grid->change_page(state, main.unit_grid->page + 1);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_grid_page_right_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_grid_page_right_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_main_grid_page_right_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::grid_page_right::update
	if(main.unit_grid->values.size() > main.unit_grid->visible_items.size()) {
		element_base::flags &= ~element_base::is_invisible_mask;
	} else {
		element_base::flags |= element_base::is_invisible_mask;
	}
// END
}
void macrobuilder2_main_grid_page_right_t::on_create(sys::state& state) noexcept {
// BEGIN main::grid_page_right::create
// END
}
void macrobuilder2_main_list_page_number_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void macrobuilder2_main_list_page_number_t::on_reset_text(sys::state& state) noexcept {
}
void macrobuilder2_main_list_page_number_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 18);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void macrobuilder2_main_list_page_number_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::list_page_number::update
	if(main.template_list->values.size() > main.template_list->visible_items.size()) {
		set_text(state, std::to_string(main.template_list->page + 1) + "/" + std::to_string(1 + main.template_list->max_page()));
	} else {
		set_text(state, "");
	}
// END
}
void macrobuilder2_main_list_page_number_t::on_create(sys::state& state) noexcept {
// BEGIN main::list_page_number::create
// END
}
void macrobuilder2_main_grid_page_number_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void macrobuilder2_main_grid_page_number_t::on_reset_text(sys::state& state) noexcept {
}
void macrobuilder2_main_grid_page_number_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 18);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void macrobuilder2_main_grid_page_number_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::grid_page_number::update
	if(main.unit_grid->values.size() > main.unit_grid->visible_items.size()) {
		set_text(state, std::to_string(main.unit_grid->page + 1) + "/" + std::to_string(1 + main.unit_grid->max_page()));
	} else {
		set_text(state, "");
	}
// END
}
void macrobuilder2_main_grid_page_number_t::on_create(sys::state& state) noexcept {
// BEGIN main::grid_page_number::create
// END
}
ui::message_result macrobuilder2_main_apply_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::apply_button::lbutton_action
	if(state.ui_state.current_template < 0 || state.ui_state.current_template >= int32_t(state.ui_state.templates.size()))
		return ui::message_result::consumed;

	auto const& t = state.ui_state.templates[state.ui_state.current_template];

	bool is_land = true;
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(!state.military_definitions.unit_base_definitions[id].is_land && t.amounts[i] > 0) {
			is_land = false;
		}
	}

	const auto template_province = state.map_state.selected_province;

	std::vector<dcon::province_id> provinces;
	state.fill_vector_of_connected_provinces(state.map_state.selected_province, is_land, provinces);
	if(provinces.empty())
		return ui::message_result::consumed;

	if(is_land) {
		std::array<uint8_t, sys::macro_builder_template::max_types> current_distribution;
		current_distribution.fill(0);
		state.build_up_to_template_land(
			t,
			state.map_state.selected_province,
			provinces,
			current_distribution
		);
	} else {
		uint8_t rem_to_build[sys::macro_builder_template::max_types];
		std::memcpy(rem_to_build, t.amounts, sizeof(rem_to_build));

		std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
			auto ab = state.world.province_get_province_naval_construction_as_province(a);
			auto bb = state.world.province_get_province_naval_construction_as_province(b);
			int32_t asz = int32_t(ab.end() - ab.begin());
			int32_t bsz = int32_t(bb.end() - bb.begin());
			if(asz == bsz)
				return a.index() < b.index();
			return asz < bsz;
		});
		for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
			dcon::unit_type_id utid = dcon::unit_type_id(i);
			if(rem_to_build[i] > 0
			&& is_land == state.military_definitions.unit_base_definitions[utid].is_land
			&& (state.military_definitions.unit_base_definitions[utid].active || state.world.nation_get_active_unit(state.local_player_nation, utid))) {
				for(const auto prov : provinces) {
					auto const port_level = state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::naval_base));
					if(port_level >= state.military_definitions.unit_base_definitions[utid].min_port_level
					&& command::can_start_naval_unit_construction(state, state.local_player_nation, prov, utid, template_province)) {
						command::start_naval_unit_construction(state, state.local_player_nation, prov, utid, template_province);
						rem_to_build[i]--;
						if(rem_to_build[i] == 0)
							break;
					}
				}
				// sort provinces again so that the ships can be built on parallel
				std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
					auto ab = state.world.province_get_province_naval_construction_as_province(a);
					auto bb = state.world.province_get_province_naval_construction_as_province(b);
					int32_t asz = int32_t(ab.end() - ab.begin());
					int32_t bsz = int32_t(bb.end() - bb.begin());
					if(asz == bsz)
						return a.index() < b.index();
					return asz < bsz;
				});
			}
		}
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_apply_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_apply_button_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void macrobuilder2_main_apply_button_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void macrobuilder2_main_apply_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 18);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void macrobuilder2_main_apply_button_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent)); 
// BEGIN main::apply_button::update
	disabled = (state.ui_state.current_template == -1 || state.map_state.selected_province == dcon::province_id{} || state.world.province_get_nation_from_province_ownership(state.map_state.selected_province) != state.local_player_nation || state.world.province_get_nation_from_province_control(state.map_state.selected_province) != state.local_player_nation);
// END
}
void macrobuilder2_main_apply_button_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::apply_button::create
// END
}
ui::message_result macrobuilder2_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_main_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
}
void macrobuilder2_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
}
void macrobuilder2_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("macrobuilder2::main"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	auto name_key = state.lookup_key("macrobuilder2::main");
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
			auto cptr = std::make_unique<macrobuilder2_main_close_button_t>();
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
		if(child_data.name == "header_text") {
			auto cptr = std::make_unique<macrobuilder2_main_header_text_t>();
			cptr->parent = this;
			header_text = cptr.get();
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
		if(child_data.name == "template_list") {
			auto cptr = std::make_unique<macrobuilder2_main_template_list_t>();
			cptr->parent = this;
			template_list = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "unit_grid") {
			auto cptr = std::make_unique<macrobuilder2_main_unit_grid_t>();
			cptr->parent = this;
			unit_grid = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "list_page_left") {
			auto cptr = std::make_unique<macrobuilder2_main_list_page_left_t>();
			cptr->parent = this;
			list_page_left = cptr.get();
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
		if(child_data.name == "list_page_right") {
			auto cptr = std::make_unique<macrobuilder2_main_list_page_right_t>();
			cptr->parent = this;
			list_page_right = cptr.get();
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
		if(child_data.name == "grid_page_left") {
			auto cptr = std::make_unique<macrobuilder2_main_grid_page_left_t>();
			cptr->parent = this;
			grid_page_left = cptr.get();
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
		if(child_data.name == "grid_page_right") {
			auto cptr = std::make_unique<macrobuilder2_main_grid_page_right_t>();
			cptr->parent = this;
			grid_page_right = cptr.get();
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
		if(child_data.name == "list_page_number") {
			auto cptr = std::make_unique<macrobuilder2_main_list_page_number_t>();
			cptr->parent = this;
			list_page_number = cptr.get();
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
		if(child_data.name == "grid_page_number") {
			auto cptr = std::make_unique<macrobuilder2_main_grid_page_number_t>();
			cptr->parent = this;
			grid_page_number = cptr.get();
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
		if(child_data.name == "apply_button") {
			auto cptr = std::make_unique<macrobuilder2_main_apply_button_t>();
			cptr->parent = this;
			apply_button = cptr.get();
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
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
	}
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state) {
	auto ptr = std::make_unique<macrobuilder2_main_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result macrobuilder2_list_item_select_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN list_item::select_button::lbutton_action
	state.ui_state.current_template = list_item.value;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_list_item_select_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_list_item_select_button_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void macrobuilder2_list_item_select_button_t::on_reset_text(sys::state& state) noexcept {
}
void macrobuilder2_list_item_select_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
	else
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 18);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void macrobuilder2_list_item_select_button_t::on_update(sys::state& state) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
// BEGIN list_item::select_button::update
	if(list_item.value == state.ui_state.current_template) {
		is_active = true;
		text_color = text::text_color::black;
	} else {
		is_active = false;
		text_color = text::text_color::gold;
	}
	if(list_item.value == -1) {
		set_text(state, text::produce_simple_string(state, "macro_new_template"));
		return;
	}
	if(list_item.value >= int32_t(state.ui_state.templates.size()))
		return;

	std::string accumulated;
	for(dcon::unit_type_id::value_base_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; i++) {
		auto amount = state.ui_state.templates[list_item.value].amounts[i];
		if(amount < 1) {
			continue;
		}
		accumulated += std::to_string(amount);
		accumulated += "@*" + std::string(i < 10 ? "0" : "") + std::to_string(i);
	}
	set_text(state, accumulated);
// END
}
void macrobuilder2_list_item_select_button_t::on_create(sys::state& state) noexcept {
// BEGIN list_item::select_button::create
// END
}
ui::message_result macrobuilder2_list_item_delete_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN list_item::delete_button::lbutton_action
	if(list_item.value != -1) {
		if(list_item.value == state.ui_state.current_template) {
			state.ui_state.current_template = -1;
		}
		state.ui_state.templates.erase(state.ui_state.templates.begin() + list_item.value);
		main.template_list->values.pop_back();
		state.game_state_updated.store(true, std::memory_order::release);
	}
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_list_item_delete_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_list_item_delete_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void macrobuilder2_list_item_delete_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_list_item_delete_button_t::on_update(sys::state& state) noexcept {
	macrobuilder2_list_item_t& list_item = *((macrobuilder2_list_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
// BEGIN list_item::delete_button::update
	if(list_item.value != -1) {
		element_base::flags &= ~element_base::is_invisible_mask;
	} else {
		element_base::flags |= element_base::is_invisible_mask;
	}
// END
}
void macrobuilder2_list_item_delete_button_t::on_create(sys::state& state) noexcept {
// BEGIN list_item::delete_button::create
// END
}
void macrobuilder2_list_item_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN list_item::update
// END
}
void macrobuilder2_list_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("macrobuilder2::list_item"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	auto name_key = state.lookup_key("macrobuilder2::list_item");
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
		if(child_data.name == "select_button") {
			auto cptr = std::make_unique<macrobuilder2_list_item_select_button_t>();
			cptr->parent = this;
			select_button = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->alt_texture_key = child_data.alt_texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "delete_button") {
			auto cptr = std::make_unique<macrobuilder2_list_item_delete_button_t>();
			cptr->parent = this;
			delete_button = cptr.get();
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
	}
// BEGIN list_item::create
// END
}
std::unique_ptr<ui::element_base> make_macrobuilder2_list_item(sys::state& state) {
	auto ptr = std::make_unique<macrobuilder2_list_item_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result macrobuilder2_grid_item_unit_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_grid_item_unit_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_grid_item_unit_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
// BEGIN grid_item::unit_icon::tooltip
	text::add_line(state, contents, state.military_definitions.unit_base_definitions[grid_item.value].name);
	text::add_line_break_to_layout(state, contents);
	auto utid = grid_item.value;
	auto is_land = state.military_definitions.unit_base_definitions[utid].is_land;

	float reconnaissance_or_fire_range = 0.f;
	float siege_or_torpedo_attack = 0.f;
	float attack_or_gun_power = 0.f;
	float defence_or_hull = 0.f;
	float discipline_or_evasion = std::numeric_limits<float>::max();
	float support = 0.f;
	float supply_consumption = 0.f;
	float maximum_speed = std::numeric_limits<float>::max();
	float maneuver = std::numeric_limits<float>::max();
	int32_t supply_consumption_score = 0;
	bool warn_overseas = false;
	bool warn_culture = false;
	bool warn_active = false;

	

		if(!state.military_definitions.unit_base_definitions[utid].active && !state.world.nation_get_active_unit(state.local_player_nation, utid))
			warn_active = true;
		if(state.military_definitions.unit_base_definitions[utid].primary_culture)
			warn_culture = true;
		if(!state.military_definitions.unit_base_definitions[utid].can_build_overseas)
			warn_overseas = true;

		reconnaissance_or_fire_range += state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range;
		siege_or_torpedo_attack += state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack;
		attack_or_gun_power += state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power;
		defence_or_hull += state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull ;
		discipline_or_evasion += std::min(discipline_or_evasion, state.world.nation_get_unit_stats(state.local_player_nation, utid).discipline_or_evasion);
		supply_consumption += state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption;
		maximum_speed = std::min(maximum_speed, state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed);
		if(is_land) {
			support += state.world.nation_get_unit_stats(state.local_player_nation, utid).support ;
			maneuver += std::min(maneuver, state.military_definitions.unit_base_definitions[utid].maneuver);
		} else {
			supply_consumption_score += state.military_definitions.unit_base_definitions[utid].supply_consumption_score;
		}
	

	if(warn_overseas)
		text::add_line(state, contents, "macro_warn_overseas");
	if(warn_culture)
		text::add_line(state, contents, "macro_warn_culture");
	if(warn_active)
		text::add_line(state, contents, "macro_warn_unlocked");
	
	if(maximum_speed == std::numeric_limits<float>::max()) maximum_speed = 0.f;
	if(discipline_or_evasion == std::numeric_limits<float>::max()) discipline_or_evasion = 0.f;
	if(maneuver == std::numeric_limits<float>::max()) maneuver = 0.f;
	if(is_land) {
		if(reconnaissance_or_fire_range > 0.f) {
			text::add_line(state, contents, "unit_recon", text::variable_type::x, text::format_float(reconnaissance_or_fire_range, 2));
		}
		if(siege_or_torpedo_attack > 0.f) {
			text::add_line(state, contents, "unit_siege", text::variable_type::x, text::format_float(siege_or_torpedo_attack, 2));
		}
		text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(attack_or_gun_power, 2));
		text::add_line(state, contents, "unit_defence", text::variable_type::x, text::format_float(defence_or_hull, 2));
		text::add_line(state, contents, "unit_discipline", text::variable_type::x, text::format_percentage(discipline_or_evasion, 0));
		if(support > 0.f) {
			text::add_line(state, contents, "unit_support", text::variable_type::x, text::format_float(support, 0));
		}
		text::add_line(state, contents, "unit_maneuver", text::variable_type::x, text::format_float(maneuver, 0));
		text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(maximum_speed, 2));
		text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_percentage(supply_consumption, 0));
	} else {
		text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(maximum_speed, 2));
		text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(attack_or_gun_power, 2));
		if(siege_or_torpedo_attack > 0.f) {
			text::add_line(state, contents, "unit_torpedo_attack", text::variable_type::x, text::format_float(siege_or_torpedo_attack, 2));
		}
		text::add_line(state, contents, "unit_hull", text::variable_type::x, text::format_float(defence_or_hull, 2));
		text::add_line(state, contents, "unit_fire_range", text::variable_type::x, text::format_float(reconnaissance_or_fire_range, 2));
		if(discipline_or_evasion > 0.f) {
			text::add_line(state, contents, "unit_evasion", text::variable_type::x, text::format_percentage(discipline_or_evasion, 0));
		}
		text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_percentage(supply_consumption, 0));
		text::add_line(state, contents, "unit_supply_load", text::variable_type::x, supply_consumption_score);
	}
// END
}
void macrobuilder2_grid_item_unit_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(background_gid) {
		auto& gfx_def = state.ui_defs.gfx[background_gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), frame, gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
			} else {
				ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
			}
		}
	}
}
void macrobuilder2_grid_item_unit_icon_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
// BEGIN grid_item::unit_icon::update
	if(!grid_item.value)
		return;
	frame = state.military_definitions.unit_base_definitions[grid_item.value].icon - 1;
// END
}
void macrobuilder2_grid_item_unit_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN grid_item::unit_icon::create
// END
}
ui::message_result macrobuilder2_grid_item_decrease_count_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
	if(mods == sys::key_modifiers::modifiers_shift) {
// BEGIN grid_item::decrease_count::lbutton_shift_action
		if(state.ui_state.current_template != -1) {
			auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
			v = uint8_t(std::clamp(v - 10, 0, 255));
		}
		state.game_state_updated.store(true, std::memory_order::release);
// END
		return ui::message_result::consumed;
	}
// BEGIN grid_item::decrease_count::lbutton_action
	if(state.ui_state.current_template != -1) {
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		v = uint8_t(std::clamp(v - 1, 0, 255));
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_grid_item_decrease_count_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_grid_item_decrease_count_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_grid_item_decrease_count_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
// BEGIN grid_item::decrease_count::update
	if(!grid_item.value)
		return;

	if(state.ui_state.current_template == -1 || state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()] == 0) {
		disabled = true;
	} else {
		disabled = false;
	}
// END
}
void macrobuilder2_grid_item_decrease_count_t::on_create(sys::state& state) noexcept {
// BEGIN grid_item::decrease_count::create
// END
}
ui::message_result macrobuilder2_grid_item_increase_count_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
	if(mods == sys::key_modifiers::modifiers_shift) {
// BEGIN grid_item::increase_count::lbutton_shift_action
		if(state.ui_state.current_template != -1) {
			auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
			v = uint8_t(std::clamp(v + 10, 0, 255));
		} else {
			state.ui_state.templates.emplace_back();
			state.ui_state.current_template = int32_t(state.ui_state.templates.size() - 1);
			main.template_list->values.push_back(int32_t(state.ui_state.templates.size() - 1));
			auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
			v = uint8_t(std::clamp(v + 10, 0, 255));
			main.template_list->page = main.template_list->max_page();
		}
		bool is_land = state.military_definitions.unit_base_definitions[grid_item.value].is_land;
		for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
			dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
			if(is_land != state.military_definitions.unit_base_definitions[id].is_land) {
				state.ui_state.templates[state.ui_state.current_template].amounts[i] = 0;
			}
		}
		state.game_state_updated.store(true, std::memory_order::release);
// END
		return ui::message_result::consumed;
	}
// BEGIN grid_item::increase_count::lbutton_action
	if(state.ui_state.current_template != -1) {
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		v = uint8_t(std::clamp(v + 1, 0, 255));
	} else {
		state.ui_state.templates.emplace_back();
		state.ui_state.current_template = int32_t(state.ui_state.templates.size() - 1);
		main.template_list->values.push_back(int32_t(state.ui_state.templates.size() - 1));
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		v = uint8_t(std::clamp(v + 1, 0, 255));
		main.template_list->page = main.template_list->max_page();
	}
	bool is_land = state.military_definitions.unit_base_definitions[grid_item.value].is_land;
	for(uint32_t i = 2; i < state.military_definitions.unit_base_definitions.size() && i < sys::macro_builder_template::max_types; ++i) {
		dcon::unit_type_id id{ dcon::unit_type_id::value_base_t(i) };
		if(is_land != state.military_definitions.unit_base_definitions[id].is_land) {
			state.ui_state.templates[state.ui_state.current_template].amounts[i] = 0;
		}
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_grid_item_increase_count_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_grid_item_increase_count_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void macrobuilder2_grid_item_increase_count_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
// BEGIN grid_item::increase_count::update
// END
}
void macrobuilder2_grid_item_increase_count_t::on_create(sys::state& state) noexcept {
// BEGIN grid_item::increase_count::create
// END
}
void macrobuilder2_grid_item_current_count_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 18), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void macrobuilder2_grid_item_current_count_t::on_reset_text(sys::state& state) noexcept {
}
void macrobuilder2_grid_item_current_count_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 18);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void macrobuilder2_grid_item_current_count_t::on_update(sys::state& state) noexcept {
	macrobuilder2_grid_item_t& grid_item = *((macrobuilder2_grid_item_t*)(parent)); 
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent->parent)); 
// BEGIN grid_item::current_count::update
	if(!grid_item.value)
		return;
	if(state.ui_state.current_template != -1) {
		auto& v = state.ui_state.templates[state.ui_state.current_template].amounts[grid_item.value.index()];
		set_text(state, std::to_string(v));
	} else {
		set_text(state, "0");
	}
// END
}
void macrobuilder2_grid_item_current_count_t::on_create(sys::state& state) noexcept {
// BEGIN grid_item::current_count::create
// END
}
ui::message_result macrobuilder2_grid_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result macrobuilder2_grid_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void macrobuilder2_grid_item_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
}
void macrobuilder2_grid_item_t::on_update(sys::state& state) noexcept {
	macrobuilder2_main_t& main = *((macrobuilder2_main_t*)(parent->parent)); 
// BEGIN grid_item::update
	if(!value) {
		flags |= element_base::is_invisible_mask;
	}
// END
}
void macrobuilder2_grid_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("macrobuilder2::grid_item"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	ui::element_base::flags |= ui::element_base::wants_update_when_hidden_mask;
	auto name_key = state.lookup_key("macrobuilder2::grid_item");
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
		if(child_data.name == "unit_icon") {
			auto cptr = std::make_unique<macrobuilder2_grid_item_unit_icon_t>();
			cptr->parent = this;
			unit_icon = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "decrease_count") {
			auto cptr = std::make_unique<macrobuilder2_grid_item_decrease_count_t>();
			cptr->parent = this;
			decrease_count = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "increase_count") {
			auto cptr = std::make_unique<macrobuilder2_grid_item_increase_count_t>();
			cptr->parent = this;
			increase_count = cptr.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->on_create(state);
			this->add_child_to_back(std::move(cptr));
		continue;
		}
		if(child_data.name == "current_count") {
			auto cptr = std::make_unique<macrobuilder2_grid_item_current_count_t>();
			cptr->parent = this;
			current_count = cptr.get();
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
// BEGIN grid_item::create
// END
}
std::unique_ptr<ui::element_base> make_macrobuilder2_grid_item(sys::state& state) {
	auto ptr = std::make_unique<macrobuilder2_grid_item_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
