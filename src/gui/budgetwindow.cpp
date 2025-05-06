namespace alice_ui {
struct budgetwindow_main_close_button_t;
struct budgetwindow_main_title_t;
struct budgetwindow_main_income_label_t;
struct budgetwindow_main_income_amount_t;
struct budgetwindow_main_expenses_label_t;
struct budgetwindow_main_expenses_amount_t;
struct budgetwindow_main_admin_eff1_t;
struct budgetwindow_main_admin_eff2_t;
struct budgetwindow_main_admin_eff_amount_t;
struct budgetwindow_main_welfare_label_t;
struct budgetwindow_main_chart_max_a_t;
struct budgetwindow_main_chart_min_t;
struct budgetwindow_main_chart_max_b_t;
struct budgetwindow_main_chart_poplabel_t;
struct budgetwindow_main_chart_needslabel_t;
struct budgetwindow_main_welfare_chart_poor_t;
struct budgetwindow_main_welfare_chart_middle_t;
struct budgetwindow_main_welfare_chart_rich_t;
struct budgetwindow_main_hover_poor_t;
struct budgetwindow_main_hover_middle_t;
struct budgetwindow_main_hover_rich_t;
struct budgetwindow_main_debt_label_t;
struct budgetwindow_main_debt_enable_t;
struct budgetwindow_main_total_debt_label_t;
struct budgetwindow_main_max_debt_label_t;
struct budgetwindow_main_total_debt_amount_t;
struct budgetwindow_main_max_debt_amount_t;
struct budgetwindow_main_debt_chart_t;
struct budgetwindow_main_debt_overlay_t;
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
struct budgetwindow_neutral_spacer_t;
struct budgetwindow_top_spacer_t;
struct budgetwindow_bottom_spacer_t;
struct budgetwindow_budget_row_contents_t;
struct budgetwindow_budget_row_t;
struct budgetwindow_budget_header_contents_t;
struct budgetwindow_budget_header_t;
struct budgetwindow_main_close_button_t : public ui::element_base {
// BEGIN main::close_button::variables
// END
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
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_title_t : public ui::element_base {
// BEGIN main::title::variables
// END
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
// BEGIN main::income_label::variables
// END
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
// BEGIN main::income_amount::variables
// END
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
struct budgetwindow_main_expenses_label_t : public ui::element_base {
// BEGIN main::expenses_label::variables
// END
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
// BEGIN main::expenses_amount::variables
// END
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
struct budgetwindow_main_admin_eff1_t : public ui::element_base {
// BEGIN main::admin_eff1::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.500000f; 
	bool text_is_header = true; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_admin_eff2_t : public ui::element_base {
// BEGIN main::admin_eff2::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.500000f; 
	bool text_is_header = true; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_admin_eff_amount_t : public ui::element_base {
// BEGIN main::admin_eff_amount::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.500000f; 
	bool text_is_header = true; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_welfare_label_t : public ui::element_base {
// BEGIN main::welfare_label::variables
// END
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
struct budgetwindow_main_chart_max_a_t : public ui::element_base {
// BEGIN main::chart_max_a::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_chart_min_t : public ui::element_base {
// BEGIN main::chart_min::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_chart_max_b_t : public ui::element_base {
// BEGIN main::chart_max_b::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_chart_poplabel_t : public ui::element_base {
// BEGIN main::chart_poplabel::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
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
struct budgetwindow_main_chart_needslabel_t : public ui::element_base {
// BEGIN main::chart_needslabel::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
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
struct budgetwindow_main_welfare_chart_poor_t : public ui::element_base {
// BEGIN main::welfare_chart_poor::variables
// END
	ogl::lines lines{ 32 };
	ogl::color4f line_color{ 0.854902f, 0.254902f, 0.192157f, 1.000000f };
	void set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max);
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
struct budgetwindow_main_welfare_chart_middle_t : public ui::element_base {
// BEGIN main::welfare_chart_middle::variables
// END
	ogl::lines lines{ 32 };
	ogl::color4f line_color{ 0.462745f, 0.313726f, 1.000000f, 1.000000f };
	void set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max);
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
struct budgetwindow_main_welfare_chart_rich_t : public ui::element_base {
// BEGIN main::welfare_chart_rich::variables
// END
	ogl::lines lines{ 32 };
	ogl::color4f line_color{ 0.133333f, 0.658824f, 0.203922f, 1.000000f };
	void set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max);
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
struct budgetwindow_main_hover_poor_t : public ui::element_base {
// BEGIN main::hover_poor::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
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
	void on_hover(sys::state& state) noexcept override;
	void on_hover_end(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_hover_middle_t : public ui::element_base {
// BEGIN main::hover_middle::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
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
	void on_hover(sys::state& state) noexcept override;
	void on_hover_end(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_hover_rich_t : public ui::element_base {
// BEGIN main::hover_rich::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
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
	void on_hover(sys::state& state) noexcept override;
	void on_hover_end(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_main_debt_label_t : public ui::element_base {
// BEGIN main::debt_label::variables
// END
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
struct budgetwindow_main_debt_enable_t : public ui::element_base {
// BEGIN main::debt_enable::variables
// END
	std::string_view texture_key;
	std::string_view alt_texture_key;
	dcon::texture_id alt_background_texture;
	bool is_active = false;
	dcon::texture_id background_texture;
	bool disabled = false;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
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
struct budgetwindow_main_total_debt_label_t : public ui::element_base {
// BEGIN main::total_debt_label::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_max_debt_label_t : public ui::element_base {
// BEGIN main::max_debt_label::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_total_debt_amount_t : public ui::element_base {
// BEGIN main::total_debt_amount::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_max_debt_amount_t : public ui::element_base {
// BEGIN main::max_debt_amount::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
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
struct budgetwindow_main_debt_chart_t : public ui::element_base {
// BEGIN main::debt_chart::variables
// END
	ogl::data_texture data_texture{ 200, 3 };
	struct graph_entry {dcon::text_key key; ogl::color3f color; float amount; };
	std::vector<graph_entry> graph_content;
	void update_chart(sys::state& state);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::position_sensitive_tooltip;
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
struct budgetwindow_main_debt_overlay_t : public ui::element_base {
// BEGIN main::debt_overlay::variables
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
struct budgetwindow_main_income_table_t : public layout_generator {
// BEGIN main::income_table::variables
// END
	struct section_header_option { int32_t section_type; };
	std::vector<std::unique_ptr<ui::element_base>> section_header_pool;
	int32_t section_header_pool_used = 0;
	void add_section_header( int32_t section_type);
	struct neutral_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> neutral_spacer_pool;
	int32_t neutral_spacer_pool_used = 0;
	void add_neutral_spacer();
	struct top_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> top_spacer_pool;
	int32_t top_spacer_pool_used = 0;
	void add_top_spacer();
	struct bottom_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> bottom_spacer_pool;
	int32_t bottom_spacer_pool_used = 0;
	void add_bottom_spacer();
	struct budget_row_option { std::string name; float value; };
	std::vector<std::unique_ptr<ui::element_base>> budget_row_pool;
	int32_t budget_row_pool_used = 0;
	void add_budget_row( std::string name,  float value);
	std::vector<std::unique_ptr<ui::element_base>> budget_header_pool;
	int32_t budget_header_pool_used = 0;
	std::vector<std::variant<std::monostate, section_header_option, neutral_spacer_option, top_spacer_option, bottom_spacer_option, budget_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct budgetwindow_main_espenses_table_t : public layout_generator {
// BEGIN main::espenses_table::variables
// END
	struct section_header_option { int32_t section_type; };
	std::vector<std::unique_ptr<ui::element_base>> section_header_pool;
	int32_t section_header_pool_used = 0;
	void add_section_header( int32_t section_type);
	struct neutral_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> neutral_spacer_pool;
	int32_t neutral_spacer_pool_used = 0;
	void add_neutral_spacer();
	struct top_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> top_spacer_pool;
	int32_t top_spacer_pool_used = 0;
	void add_top_spacer();
	struct bottom_spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> bottom_spacer_pool;
	int32_t bottom_spacer_pool_used = 0;
	void add_bottom_spacer();
	struct budget_row_option { std::string name; float value; };
	std::vector<std::unique_ptr<ui::element_base>> budget_row_pool;
	int32_t budget_row_pool_used = 0;
	void add_budget_row( std::string name,  float value);
	std::vector<std::unique_ptr<ui::element_base>> budget_header_pool;
	int32_t budget_header_pool_used = 0;
	std::vector<std::variant<std::monostate, section_header_option, neutral_spacer_option, top_spacer_option, bottom_spacer_option, budget_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct budgetwindow_section_header_label_t : public ui::element_base {
// BEGIN section_header::label::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::gold;
	float text_scale = 1.400000f; 
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
struct budgetwindow_section_header_llbutton_t : public ui::element_base {
// BEGIN section_header::llbutton::variables
// END
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
// BEGIN section_header::lbutton::variables
// END
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
// BEGIN section_header::rbutton::variables
// END
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
// BEGIN section_header::rrbutton::variables
// END
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
// BEGIN section_header::setting_amount::variables
// END
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
// BEGIN section_header::expand_button::variables
// END
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
// BEGIN section_header::total_amount::variables
// END
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
// BEGIN section_header::min_setting::variables
// END
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
// BEGIN section_header::max_setting::variables
// END
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
struct budgetwindow_budget_row_contents_t : public ui::element_base {
// BEGIN budget_row::contents::variables
// END
	text::layout item_name_internal_layout;
	text::text_color  item_name_text_color = text::text_color::black;
	std::string item_name_cached_text;
	void set_item_name_text(sys::state & state, std::string const& new_text);
	text::layout item_value_internal_layout;
	text::text_color  item_value_text_color = text::text_color::black;
	std::string item_value_cached_text;
	void set_item_value_text(sys::state & state, std::string const& new_text);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
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
	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct budgetwindow_budget_header_contents_t : public ui::element_base {
// BEGIN budget_header::contents::variables
// END
	text::layout item_name_internal_layout;
	std::string item_name_cached_text;
	text::layout item_value_internal_layout;
	std::string item_value_cached_text;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
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
struct budgetwindow_main_t : public layout_window_element {
// BEGIN main::variables
// END
	std::unique_ptr<budgetwindow_main_close_button_t> close_button;
	std::unique_ptr<budgetwindow_main_title_t> title;
	std::unique_ptr<budgetwindow_main_income_label_t> income_label;
	std::unique_ptr<budgetwindow_main_income_amount_t> income_amount;
	std::unique_ptr<budgetwindow_main_expenses_label_t> expenses_label;
	std::unique_ptr<budgetwindow_main_expenses_amount_t> expenses_amount;
	std::unique_ptr<budgetwindow_main_admin_eff1_t> admin_eff1;
	std::unique_ptr<budgetwindow_main_admin_eff2_t> admin_eff2;
	std::unique_ptr<budgetwindow_main_admin_eff_amount_t> admin_eff_amount;
	std::unique_ptr<budgetwindow_main_welfare_label_t> welfare_label;
	std::unique_ptr<budgetwindow_main_chart_max_a_t> chart_max_a;
	std::unique_ptr<budgetwindow_main_chart_min_t> chart_min;
	std::unique_ptr<budgetwindow_main_chart_max_b_t> chart_max_b;
	std::unique_ptr<budgetwindow_main_chart_poplabel_t> chart_poplabel;
	std::unique_ptr<budgetwindow_main_chart_needslabel_t> chart_needslabel;
	std::unique_ptr<budgetwindow_main_welfare_chart_poor_t> welfare_chart_poor;
	std::unique_ptr<budgetwindow_main_welfare_chart_middle_t> welfare_chart_middle;
	std::unique_ptr<budgetwindow_main_welfare_chart_rich_t> welfare_chart_rich;
	std::unique_ptr<budgetwindow_main_hover_poor_t> hover_poor;
	std::unique_ptr<budgetwindow_main_hover_middle_t> hover_middle;
	std::unique_ptr<budgetwindow_main_hover_rich_t> hover_rich;
	std::unique_ptr<budgetwindow_main_debt_label_t> debt_label;
	std::unique_ptr<budgetwindow_main_debt_enable_t> debt_enable;
	std::unique_ptr<budgetwindow_main_total_debt_label_t> total_debt_label;
	std::unique_ptr<budgetwindow_main_max_debt_label_t> max_debt_label;
	std::unique_ptr<budgetwindow_main_total_debt_amount_t> total_debt_amount;
	std::unique_ptr<budgetwindow_main_max_debt_amount_t> max_debt_amount;
	std::unique_ptr<budgetwindow_main_debt_chart_t> debt_chart;
	std::unique_ptr<budgetwindow_main_debt_overlay_t> debt_overlay;
	budgetwindow_main_income_table_t income_table;
	budgetwindow_main_espenses_table_t espenses_table;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	int16_t income_table_lead_space_column_start = 0;
	int16_t income_table_lead_space_column_width = 0;
	std::string_view income_table_item_name_header_text_key;
	text::text_color income_table_item_name_header_text_color = text::text_color::black;
	text::text_color income_table_item_name_column_text_color = text::text_color::black;
	text::alignment income_table_item_name_text_alignment = text::alignment::right;
	int8_t income_table_item_name_sort_direction = 0;
	int16_t income_table_item_name_column_start = 0;
	int16_t income_table_item_name_column_width = 0;
	std::string_view income_table_item_value_header_text_key;
	text::text_color income_table_item_value_header_text_color = text::text_color::black;
	text::text_color income_table_item_value_column_text_color = text::text_color::black;
	text::alignment income_table_item_value_text_alignment = text::alignment::right;
	int8_t income_table_item_value_sort_direction = 0;
	int16_t income_table_item_value_column_start = 0;
	int16_t income_table_item_value_column_width = 0;
	std::string_view income_table_ascending_icon_key;
	dcon::texture_id income_table_ascending_icon;
	std::string_view income_table_descending_icon_key;
	dcon::texture_id income_table_descending_icon;
	ogl::color3f income_table_divider_color{float(0.200000), float(0.192157), float(0.192157)};
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
struct budgetwindow_section_header_t : public layout_window_element {
// BEGIN section_header::variables
// END
	int32_t section_type;
	std::unique_ptr<budgetwindow_section_header_label_t> label;
	std::unique_ptr<budgetwindow_section_header_llbutton_t> llbutton;
	std::unique_ptr<budgetwindow_section_header_lbutton_t> lbutton;
	std::unique_ptr<budgetwindow_section_header_rbutton_t> rbutton;
	std::unique_ptr<budgetwindow_section_header_rrbutton_t> rrbutton;
	std::unique_ptr<budgetwindow_section_header_setting_amount_t> setting_amount;
	std::unique_ptr<budgetwindow_section_header_expand_button_t> expand_button;
	std::unique_ptr<budgetwindow_section_header_total_amount_t> total_amount;
	std::unique_ptr<budgetwindow_section_header_min_setting_t> min_setting;
	std::unique_ptr<budgetwindow_section_header_max_setting_t> max_setting;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view texture_key;
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
		if(name_parameter == "section_type") {
			return (void*)(&section_type);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_budgetwindow_section_header(sys::state& state);
struct budgetwindow_neutral_spacer_t : public ui::non_owning_container_base {
// BEGIN neutral_spacer::variables
// END
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_budgetwindow_neutral_spacer(sys::state& state);
struct budgetwindow_top_spacer_t : public ui::non_owning_container_base {
// BEGIN top_spacer::variables
// END
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_budgetwindow_top_spacer(sys::state& state);
struct budgetwindow_bottom_spacer_t : public ui::non_owning_container_base {
// BEGIN bottom_spacer::variables
// END
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_budgetwindow_bottom_spacer(sys::state& state);
struct budgetwindow_budget_row_t : public layout_window_element {
// BEGIN budget_row::variables
// END
	std::string name;
	float value;
	std::unique_ptr<budgetwindow_budget_row_contents_t> contents;
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
		if(name_parameter == "name") {
			return (void*)(&name);
		}
		if(name_parameter == "value") {
			return (void*)(&value);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_budgetwindow_budget_row(sys::state& state);
struct budgetwindow_budget_header_t : public layout_window_element {
// BEGIN budget_header::variables
// END
	std::unique_ptr<budgetwindow_budget_header_contents_t> contents;
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
};
std::unique_ptr<ui::element_base> make_budgetwindow_budget_header(sys::state& state);
void budgetwindow_main_income_table_t::add_section_header(int32_t section_type) {
	values.emplace_back(section_header_option{section_type});
}
void budgetwindow_main_income_table_t::add_neutral_spacer() {
	values.emplace_back(neutral_spacer_option{});
}
void budgetwindow_main_income_table_t::add_top_spacer() {
	values.emplace_back(top_spacer_option{});
}
void budgetwindow_main_income_table_t::add_bottom_spacer() {
	values.emplace_back(bottom_spacer_option{});
}
void budgetwindow_main_income_table_t::add_budget_row(std::string name, float value) {
	values.emplace_back(budget_row_option{name, value});
}
void  budgetwindow_main_income_table_t::on_create(sys::state& state, layout_window_element* parent) {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_table::on_create
// END
}
void  budgetwindow_main_income_table_t::update(sys::state& state, layout_window_element* parent) {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::income_table::update
	values.clear();
	add_section_header(budget_categories::poor_tax);
	if(budget_categories::expanded[budget_categories::poor_tax]) {
		add_top_spacer();
		for(auto so : state.world.nation_get_province_ownership(state.local_player_nation)) {
			auto province = so.get_province();
			auto info = economy::explain_tax_income_local(state, state.local_player_nation, province);
			if (info.poor > 0.f)
				add_budget_row(
					text::produce_simple_string(state, province.get_name()),
					info.poor
				);
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::middle_tax);
	if(budget_categories::expanded[budget_categories::middle_tax]) {
		add_top_spacer();
		for(auto so : state.world.nation_get_province_ownership(state.local_player_nation)) {
			auto province = so.get_province();
			auto info = economy::explain_tax_income_local(state, state.local_player_nation, province);
			if (info.mid > 0.f)
				add_budget_row(
					text::produce_simple_string(state, province.get_name()),
					info.mid
				);
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::rich_tax);
	if(budget_categories::expanded[budget_categories::rich_tax]) {
		add_top_spacer();
		for(auto so : state.world.nation_get_province_ownership(state.local_player_nation)) {
			auto province = so.get_province();
			auto info = economy::explain_tax_income_local(state, state.local_player_nation, province);
			if (info.rich > 0.f)
				add_budget_row(
					text::produce_simple_string(state, province.get_name()),
					info.rich
				);
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::tariffs_import);
	if(budget_categories::expanded[budget_categories::tariffs_import]) {
		add_top_spacer();
		auto totals = economy::explain_national_tariff(state, state.local_player_nation, true, false);
		for(auto& item : totals) {
			add_budget_row(
				text::produce_simple_string(state,
					state.world.national_identity_get_name(
						state.world.nation_get_identity_from_identity_holder(item.trade_partner)
					)
				)
				+
				" ("
				+ 
				text::produce_simple_string(state,
					state.world.commodity_get_name(item.commodity)
				)
				+
				")",
				item.tariff
			);
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::tariffs_export);
	if(budget_categories::expanded[budget_categories::tariffs_export]) {
		add_top_spacer();
		auto totals = economy::explain_national_tariff(state, state.local_player_nation, false, true);
		for(auto& item : totals) {
			add_budget_row(
				text::produce_simple_string(state,
					state.world.national_identity_get_name(
						state.world.nation_get_identity_from_identity_holder(item.trade_partner)
					)
				)
				+
				" ("
				+
				text::produce_simple_string(state,
					state.world.commodity_get_name(item.commodity)
				)
				+
				")",
				item.tariff
			);
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::diplomatic_income);
	if(budget_categories::expanded[budget_categories::diplomatic_income]) {
		add_top_spacer();
		add_budget_row(text::produce_simple_string(state, "warsubsidies_button"), economy::estimate_war_subsidies_income(state, state.local_player_nation));
		add_budget_row(text::produce_simple_string(state, "alice_budget_indemnities"), economy::estimate_reparations_income(state, state.local_player_nation));
		for(auto n : state.world.nation_get_overlord_as_ruler(state.local_player_nation)) {
			auto transferamt = economy::estimate_subject_payments_paid(state, n.get_subject());
			add_budget_row(text::produce_simple_string(state, text::get_name(state, n.get_subject())), transferamt);
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::gold);
	if(budget_categories::expanded[budget_categories::gold]) {
		add_top_spacer();
		for(auto poid : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
			auto prov = poid.get_province();

			float amount = 0.0f;
			state.world.for_each_commodity([&](dcon::commodity_id c) {
				if(state.world.commodity_get_money_rgo(c)) {
					amount += economy::rgo_output(state, c, prov.id) * state.world.commodity_get_cost(c);
				}
			});

			if(amount > 0.0f) {
				add_budget_row(text::produce_simple_string(state, state.world.province_get_name(prov)), amount * state.defines.gold_to_cash_rate);
			}
		}
		add_bottom_spacer();
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (budgetwindow_main_t*)(parent);
	if(table_source->income_table_item_name_sort_direction != 0) work_to_do = true;
	if(table_source->income_table_item_value_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ++i) {
			if(std::holds_alternative<budget_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<budget_row_option>(values[i])) ++i;
				if(table_source->income_table_item_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<budget_row_option>(raw_a);
							auto const& b = std::get<budget_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::income_table::income_table::sort::item_name
						result = int8_t(std::clamp(a.name.compare(b.name), -1, 1));
// END
						return -result == table_source->income_table_item_name_sort_direction;
					});
				}
				if(table_source->income_table_item_value_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<budget_row_option>(raw_a);
							auto const& b = std::get<budget_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::income_table::income_table::sort::item_value
						if(a.value < b.value) result = -1;
						if(a.value > b.value) result = 1;
// END
						return -result == table_source->income_table_item_value_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  budgetwindow_main_income_table_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<section_header_option>(values[index])) {
		if(section_header_pool.empty()) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
		if(destination) {
			if(section_header_pool.size() <= size_t(section_header_pool_used)) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
			section_header_pool[section_header_pool_used]->base_data.position.x = int16_t(x);
			section_header_pool[section_header_pool_used]->base_data.position.y = int16_t(y);
			section_header_pool[section_header_pool_used]->parent = destination;
			destination->children.push_back(section_header_pool[section_header_pool_used].get());
			((budgetwindow_section_header_t*)(section_header_pool[section_header_pool_used].get()))->section_type = std::get<section_header_option>(values[index]).section_type;
			section_header_pool[section_header_pool_used]->impl_on_update(state);
			section_header_pool_used++;
		}
		alternate = true;
		return measure_result{ section_header_pool[0]->base_data.size.x, section_header_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<neutral_spacer_option>(values[index])) {
		if(neutral_spacer_pool.empty()) neutral_spacer_pool.emplace_back(make_budgetwindow_neutral_spacer(state));
		if(destination) {
			if(neutral_spacer_pool.size() <= size_t(neutral_spacer_pool_used)) neutral_spacer_pool.emplace_back(make_budgetwindow_neutral_spacer(state));
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.x = int16_t(x);
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.y = int16_t(y);
			neutral_spacer_pool[neutral_spacer_pool_used]->parent = destination;
			destination->children.push_back(neutral_spacer_pool[neutral_spacer_pool_used].get());
			neutral_spacer_pool[neutral_spacer_pool_used]->impl_on_update(state);
			neutral_spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ neutral_spacer_pool[0]->base_data.size.x, neutral_spacer_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<top_spacer_option>(values[index])) {
		if(top_spacer_pool.empty()) top_spacer_pool.emplace_back(make_budgetwindow_top_spacer(state));
		if(destination) {
			if(top_spacer_pool.size() <= size_t(top_spacer_pool_used)) top_spacer_pool.emplace_back(make_budgetwindow_top_spacer(state));
			top_spacer_pool[top_spacer_pool_used]->base_data.position.x = int16_t(x);
			top_spacer_pool[top_spacer_pool_used]->base_data.position.y = int16_t(y);
			top_spacer_pool[top_spacer_pool_used]->parent = destination;
			destination->children.push_back(top_spacer_pool[top_spacer_pool_used].get());
			top_spacer_pool[top_spacer_pool_used]->impl_on_update(state);
			top_spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ top_spacer_pool[0]->base_data.size.x, top_spacer_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<bottom_spacer_option>(values[index])) {
		if(bottom_spacer_pool.empty()) bottom_spacer_pool.emplace_back(make_budgetwindow_bottom_spacer(state));
		if(destination) {
			if(bottom_spacer_pool.size() <= size_t(bottom_spacer_pool_used)) bottom_spacer_pool.emplace_back(make_budgetwindow_bottom_spacer(state));
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.x = int16_t(x);
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.y = int16_t(y);
			bottom_spacer_pool[bottom_spacer_pool_used]->parent = destination;
			destination->children.push_back(bottom_spacer_pool[bottom_spacer_pool_used].get());
			bottom_spacer_pool[bottom_spacer_pool_used]->impl_on_update(state);
			bottom_spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ bottom_spacer_pool[0]->base_data.size.x, bottom_spacer_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<budget_row_option>(values[index])) {
		if(budget_header_pool.empty()) budget_header_pool.emplace_back(make_budgetwindow_budget_header(state));
		if(budget_row_pool.empty()) budget_row_pool.emplace_back(make_budgetwindow_budget_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<budget_row_option>(values[index - 1]))) {
			if(destination) {
				if(budget_header_pool.size() <= size_t(budget_header_pool_used)) budget_header_pool.emplace_back(make_budgetwindow_budget_header(state));
				if(budget_row_pool.size() <= size_t(budget_row_pool_used)) budget_row_pool.emplace_back(make_budgetwindow_budget_row(state));
				budget_header_pool[budget_header_pool_used]->base_data.position.x = int16_t(x);
				budget_header_pool[budget_header_pool_used]->base_data.position.y = int16_t(y);
				if(!budget_header_pool[budget_header_pool_used]->parent) {
					budget_header_pool[budget_header_pool_used]->parent = destination;
					budget_header_pool[budget_header_pool_used]->impl_on_update(state);
					budget_header_pool[budget_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(budget_header_pool[budget_header_pool_used].get());
			((budgetwindow_budget_header_t*)(budget_header_pool[budget_header_pool_used].get()))->is_active = alternate;
				budget_row_pool[budget_row_pool_used]->base_data.position.x = int16_t(x);
				budget_row_pool[budget_row_pool_used]->base_data.position.y = int16_t(y +  budget_row_pool[0]->base_data.size.y + 0);
				budget_row_pool[budget_row_pool_used]->parent = destination;
				destination->children.push_back(budget_row_pool[budget_row_pool_used].get());
				((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->name = std::get<budget_row_option>(values[index]).name;
				((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->value = std::get<budget_row_option>(values[index]).value;
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->is_active = !alternate;
				budget_row_pool[budget_row_pool_used]->impl_on_update(state);
				budget_header_pool_used++;
				budget_row_pool_used++;
			}
			return measure_result{std::max(budget_header_pool[0]->base_data.size.x, budget_row_pool[0]->base_data.size.x), budget_header_pool[0]->base_data.size.y + budget_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(budget_row_pool.size() <= size_t(budget_row_pool_used)) budget_row_pool.emplace_back(make_budgetwindow_budget_row(state));
			budget_row_pool[budget_row_pool_used]->base_data.position.x = int16_t(x);
			budget_row_pool[budget_row_pool_used]->base_data.position.y = int16_t(y);
			budget_row_pool[budget_row_pool_used]->parent = destination;
			destination->children.push_back(budget_row_pool[budget_row_pool_used].get());
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->name = std::get<budget_row_option>(values[index]).name;
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->value = std::get<budget_row_option>(values[index]).value;
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->is_active = alternate;
			budget_row_pool[budget_row_pool_used]->impl_on_update(state);
			budget_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ budget_row_pool[0]->base_data.size.x, budget_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  budgetwindow_main_income_table_t::reset_pools() {
	section_header_pool_used = 0;
	neutral_spacer_pool_used = 0;
	top_spacer_pool_used = 0;
	bottom_spacer_pool_used = 0;
	budget_header_pool_used = 0;
	budget_row_pool_used = 0;
}
void budgetwindow_main_espenses_table_t::add_section_header(int32_t section_type) {
	values.emplace_back(section_header_option{section_type});
}
void budgetwindow_main_espenses_table_t::add_neutral_spacer() {
	values.emplace_back(neutral_spacer_option{});
}
void budgetwindow_main_espenses_table_t::add_top_spacer() {
	values.emplace_back(top_spacer_option{});
}
void budgetwindow_main_espenses_table_t::add_bottom_spacer() {
	values.emplace_back(bottom_spacer_option{});
}
void budgetwindow_main_espenses_table_t::add_budget_row(std::string name, float value) {
	values.emplace_back(budget_row_option{name, value});
}
void  budgetwindow_main_espenses_table_t::on_create(sys::state& state, layout_window_element* parent) {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::espenses_table::on_create
// END
}
void  budgetwindow_main_espenses_table_t::update(sys::state& state, layout_window_element* parent) {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::espenses_table::update
	values.clear();
	add_section_header(budget_categories::construction);
	if(budget_categories::expanded[budget_categories::construction]) {
		auto explanation = economy::explain_construction_spending_now(state, state.local_player_nation);
		for(auto& data : explanation.factories) {
			auto building_type = state.world.factory_construction_get_type(data.construction);
			auto location = state.world.factory_construction_get_province(data.construction);
			add_budget_row(
				text::produce_simple_string(state, state.world.factory_type_get_name(building_type))
				+ "(" + text::produce_simple_string(state, state.world.province_get_name(location)) + ")",
				data.spending
			);
		}
		for(auto& data : explanation.province_buildings) {
			auto building_type = state.world.province_building_construction_get_type(data.construction);
			auto location = state.world.province_building_construction_get_province(data.construction);
			add_budget_row(
				text::produce_simple_string(state, state.world.province_get_name(location)) ,
				data.spending
			);
		}
		for(auto& data : explanation.land_units) {
			auto unit_type = state.world.province_land_construction_get_type(data.construction);
			auto location = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(data.construction));
			auto unit_name = state.military_definitions.unit_base_definitions[unit_type].name;
			add_budget_row(
				text::produce_simple_string(state, unit_name)
				+ "(" + text::produce_simple_string(state, state.world.province_get_name(location)) + ")",
				data.spending
			);
		}
		for(auto& data : explanation.naval_units) {
			auto unit_type = state.world.province_naval_construction_get_type(data.construction);
			auto location = state.world.province_naval_construction_get_province(data.construction);
			auto unit_name = state.military_definitions.unit_base_definitions[unit_type].name;
			add_budget_row(
				text::produce_simple_string(state, unit_name)
				+ "(" + text::produce_simple_string(state, state.world.province_get_name(location)) + ")",
				data.spending
			);
		}
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::army_upkeep);
	if(budget_categories::expanded[budget_categories::army_upkeep]) {
		add_top_spacer();
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
				add_budget_row(text::produce_simple_string(state, state.world.commodity_get_name(c)), goods.get(c) * fraction);
			}
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::navy_upkeep);
	if(budget_categories::expanded[budget_categories::navy_upkeep]) {
		add_top_spacer();
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
				add_budget_row(text::produce_simple_string(state, state.world.commodity_get_name(c)), goods.get(c) * fraction);
			}
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::admin);
	if(budget_categories::expanded[budget_categories::admin]) {
		add_top_spacer();
		auto fraction = float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.0f;

		add_budget_row(
			"capital",
			economy::estimate_spendings_administration_capital(
				state, state.local_player_nation, fraction
			)
		);

		/*
		auto in = culture::income_type::administration;
		state.world.nation_for_each_province_ownership(state.local_player_nation, [&](auto poid) {
			auto local_province = state.world.province_ownership_get_province(poid);
			add_budget_row(
					text::produce_simple_string(state, state.world.province_get_name(local_province)),
					economy::estimate_spendings_administration_local(
						state, state.local_player_nation, local_province, fraction
					)
			);
		});
		*/
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::education);
	if(budget_categories::expanded[budget_categories::education]) {
		add_top_spacer();
		auto fraction = float(state.world.nation_get_education_spending(state.local_player_nation)) / 100.0f;
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
				add_budget_row(text::get_dynamic_state_name(state, local_state), total * fraction);
			}
		});
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::social);
	if(budget_categories::expanded[budget_categories::social]) {
		add_top_spacer();
		auto population = state.world.nation_get_demographics(state.local_player_nation, demographics::total);
		auto unemployed = population - state.world.nation_get_demographics(state.local_player_nation, demographics::employed);
		auto const p_level = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::pension_level);
		auto const unemp_level = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::unemployment_benefit);
		auto fraction = float(state.world.nation_get_social_spending(state.local_player_nation)) / 100.0f;

		auto budget = state.world.nation_get_stockpiles(state.local_player_nation, economy::money);
		auto social_budget = budget * fraction;

		auto pension_ratio = p_level > 0.f ? p_level * population / (p_level * population + unemp_level * unemployed) : 0.f;
		auto unemployment_ratio = unemp_level > 0.f ? unemp_level * unemployed / (p_level * population + unemp_level * unemployed) : 0.f;

		auto const pension_per_person =
			pension_ratio
			* social_budget
			/ (population + 1.f);

		auto const benefits_per_person =
			unemployment_ratio
			* social_budget
			/ (unemployed + 1.f);

		state.world.nation_for_each_state_ownership(state.local_player_nation, [&](auto soid) {
			auto local_state = state.world.state_ownership_get_state(soid);
			auto market = state.world.state_instance_get_market_from_local_market(local_state);
			float total = 0.0f;

			state.world.for_each_pop_type([&](dcon::pop_type_id pt) {
				auto pop_of_type =
					state.world.state_instance_get_demographics(local_state, demographics::to_key(state, pt));
				if(pop_of_type <= 0)
					return;

				auto ln_type = culture::income_type(state.world.pop_type_get_life_needs_income_type(pt));
				if(ln_type == culture::income_type::administration || ln_type == culture::income_type::education || ln_type == culture::income_type::military) {
					//nothing
				} else { // unemployment, pensions
					total += pop_of_type * pension_per_person;
					if(state.world.pop_type_get_has_unemployment(pt)) {
						auto emp = state.world.state_instance_get_demographics(local_state, demographics::to_employment_key(state, pt));
						total += (pop_of_type - emp) * benefits_per_person;
					}
				}
			});
			if(total > 0.0f) {
				add_budget_row(text::get_dynamic_state_name(state, local_state), total * fraction);
			}
		});
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::military);
	if(budget_categories::expanded[budget_categories::military]) {
		add_top_spacer();
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
				add_budget_row(text::get_dynamic_state_name(state, local_state), total * fraction);
			}
		});
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::domestic_investment);
	if(budget_categories::expanded[budget_categories::domestic_investment]) {
		add_top_spacer();
		auto cap_total = 0.f;
		auto aristo_total = 0.f;

		auto capitalists_key = demographics::to_key(state, state.culture_definitions.capitalists);
		auto aristocracy_key = demographics::to_key(state, state.culture_definitions.aristocrat);
		auto capitalists = state.world.nation_get_demographics(state.local_player_nation, capitalists_key);
		auto aristocrats = state.world.nation_get_demographics(state.local_player_nation, aristocracy_key);
		auto investors = capitalists + aristocrats;
		if(investors > 0) {
			auto total_budget = state.world.nation_get_stockpiles(state.local_player_nation, economy::money)
				* float(state.world.nation_get_domestic_investment_spending(state.local_player_nation)) / 100.f;
			cap_total += capitalists / investors * total_budget;
			aristo_total += aristocrats / investors * total_budget;
		}

		add_budget_row(
			text::produce_simple_string(
				state, state.world.pop_type_get_name(state.culture_definitions.capitalists)
			),
			cap_total
		);
		add_budget_row(
			text::produce_simple_string(
				state, state.world.pop_type_get_name(state.culture_definitions.aristocrat)
			),
			aristo_total
		);
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::overseas_spending);
	if(budget_categories::expanded[budget_categories::overseas_spending]) {
		add_top_spacer();
		auto capital = state.world.nation_get_capital(state.local_player_nation);
		auto capital_state = state.world.province_get_state_membership(capital);
		auto market = state.world.state_instance_get_market_from_local_market(capital_state);

		auto overseas_factor = state.defines.province_overseas_penalty * float(state.world.nation_get_owned_province_count(state.local_player_nation) - state.world.nation_get_central_province_count(state.local_player_nation));
		uint32_t total_commodities = state.world.commodity_size();

		if(overseas_factor > 0) {
			for(uint32_t i = 1; i < total_commodities; ++i) {
				dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };

				if(state.world.commodity_get_overseas_penalty(cid) && (state.world.commodity_get_is_available_from_start(cid) || state.world.nation_get_unlocked_commodities(state.local_player_nation, cid))) {
					auto amount =
						overseas_factor * economy::price(state, market, cid) * state.world.market_get_demand_satisfaction(market, cid) * float(state.world.nation_get_overseas_spending(state.local_player_nation)) / 100.0f;
					add_budget_row(text::produce_simple_string(state, state.world.commodity_get_name(cid)), amount);
				}
			}
		}
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::diplomatic_expenses);
	if(budget_categories::expanded[budget_categories::diplomatic_expenses]) {
		add_top_spacer();
		add_budget_row(text::produce_simple_string(state, "warsubsidies_button"), economy::estimate_war_subsidies_spending(state, state.local_player_nation));
		add_budget_row(text::produce_simple_string(state, "alice_budget_indemnities"), economy::estimate_reparations_spending(state, state.local_player_nation));
		add_budget_row(text::produce_simple_string(state, "alice_budget_overlord"), economy::estimate_subject_payments_paid(state, state.local_player_nation));
		add_bottom_spacer();
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::debt_payment);
	if(budget_categories::expanded[budget_categories::debt_payment]) {
		// TODO?
	} else {
		add_neutral_spacer();
	}
	add_section_header(budget_categories::stockpile);
	if(budget_categories::expanded[budget_categories::stockpile]) {
		add_top_spacer();
		uint32_t total_commodities = state.world.commodity_size();

		auto capital = state.world.nation_get_capital(state.local_player_nation);
		auto capital_state = state.world.province_get_state_membership(capital);
		auto market = state.world.state_instance_get_market_from_local_market(capital_state);

		for(uint32_t i = 1; i < total_commodities; ++i) {
			dcon::commodity_id cid{ dcon::commodity_id::value_base_t(i) };
			auto difference =
				state.world.nation_get_stockpile_targets(state.local_player_nation, cid)
				- state.world.nation_get_stockpiles(state.local_player_nation, cid);

			if(difference > 0 && state.world.nation_get_drawing_on_stockpiles(state.local_player_nation, cid) == false) {
				auto amount = difference * economy::price(state, market, cid) * state.world.market_get_demand_satisfaction(market, cid);
				if(amount > 0) {
					add_budget_row(text::produce_simple_string(state, state.world.commodity_get_name(cid)), amount);
				}
			}
		}
		add_bottom_spacer();
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (budgetwindow_main_t*)(parent);
	if(table_source->income_table_item_name_sort_direction != 0) work_to_do = true;
	if(table_source->income_table_item_value_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ++i) {
			if(std::holds_alternative<budget_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<budget_row_option>(values[i])) ++i;
				if(table_source->income_table_item_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<budget_row_option>(raw_a);
							auto const& b = std::get<budget_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::espenses_table::income_table::sort::item_name
						result = int8_t(std::clamp(a.name.compare(b.name), -1, 1));
// END
						return -result == table_source->income_table_item_name_sort_direction;
					});
				}
				if(table_source->income_table_item_value_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<budget_row_option>(raw_a);
							auto const& b = std::get<budget_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::espenses_table::income_table::sort::item_value
						if(a.value < b.value) result = -1;
						if(a.value > b.value) result = 1;
// END
						return -result == table_source->income_table_item_value_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  budgetwindow_main_espenses_table_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<section_header_option>(values[index])) {
		if(section_header_pool.empty()) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
		if(destination) {
			if(section_header_pool.size() <= size_t(section_header_pool_used)) section_header_pool.emplace_back(make_budgetwindow_section_header(state));
			section_header_pool[section_header_pool_used]->base_data.position.x = int16_t(x);
			section_header_pool[section_header_pool_used]->base_data.position.y = int16_t(y);
			section_header_pool[section_header_pool_used]->parent = destination;
			destination->children.push_back(section_header_pool[section_header_pool_used].get());
			((budgetwindow_section_header_t*)(section_header_pool[section_header_pool_used].get()))->section_type = std::get<section_header_option>(values[index]).section_type;
			section_header_pool[section_header_pool_used]->impl_on_update(state);
			section_header_pool_used++;
		}
		alternate = true;
		return measure_result{ section_header_pool[0]->base_data.size.x, section_header_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<neutral_spacer_option>(values[index])) {
		if(neutral_spacer_pool.empty()) neutral_spacer_pool.emplace_back(make_budgetwindow_neutral_spacer(state));
		if(destination) {
			if(neutral_spacer_pool.size() <= size_t(neutral_spacer_pool_used)) neutral_spacer_pool.emplace_back(make_budgetwindow_neutral_spacer(state));
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.x = int16_t(x);
			neutral_spacer_pool[neutral_spacer_pool_used]->base_data.position.y = int16_t(y);
			neutral_spacer_pool[neutral_spacer_pool_used]->parent = destination;
			destination->children.push_back(neutral_spacer_pool[neutral_spacer_pool_used].get());
			neutral_spacer_pool[neutral_spacer_pool_used]->impl_on_update(state);
			neutral_spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ neutral_spacer_pool[0]->base_data.size.x, neutral_spacer_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<top_spacer_option>(values[index])) {
		if(top_spacer_pool.empty()) top_spacer_pool.emplace_back(make_budgetwindow_top_spacer(state));
		if(destination) {
			if(top_spacer_pool.size() <= size_t(top_spacer_pool_used)) top_spacer_pool.emplace_back(make_budgetwindow_top_spacer(state));
			top_spacer_pool[top_spacer_pool_used]->base_data.position.x = int16_t(x);
			top_spacer_pool[top_spacer_pool_used]->base_data.position.y = int16_t(y);
			top_spacer_pool[top_spacer_pool_used]->parent = destination;
			destination->children.push_back(top_spacer_pool[top_spacer_pool_used].get());
			top_spacer_pool[top_spacer_pool_used]->impl_on_update(state);
			top_spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ top_spacer_pool[0]->base_data.size.x, top_spacer_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<bottom_spacer_option>(values[index])) {
		if(bottom_spacer_pool.empty()) bottom_spacer_pool.emplace_back(make_budgetwindow_bottom_spacer(state));
		if(destination) {
			if(bottom_spacer_pool.size() <= size_t(bottom_spacer_pool_used)) bottom_spacer_pool.emplace_back(make_budgetwindow_bottom_spacer(state));
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.x = int16_t(x);
			bottom_spacer_pool[bottom_spacer_pool_used]->base_data.position.y = int16_t(y);
			bottom_spacer_pool[bottom_spacer_pool_used]->parent = destination;
			destination->children.push_back(bottom_spacer_pool[bottom_spacer_pool_used].get());
			bottom_spacer_pool[bottom_spacer_pool_used]->impl_on_update(state);
			bottom_spacer_pool_used++;
		}
		alternate = true;
		return measure_result{ bottom_spacer_pool[0]->base_data.size.x, bottom_spacer_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<budget_row_option>(values[index])) {
		if(budget_header_pool.empty()) budget_header_pool.emplace_back(make_budgetwindow_budget_header(state));
		if(budget_row_pool.empty()) budget_row_pool.emplace_back(make_budgetwindow_budget_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<budget_row_option>(values[index - 1]))) {
			if(destination) {
				if(budget_header_pool.size() <= size_t(budget_header_pool_used)) budget_header_pool.emplace_back(make_budgetwindow_budget_header(state));
				if(budget_row_pool.size() <= size_t(budget_row_pool_used)) budget_row_pool.emplace_back(make_budgetwindow_budget_row(state));
				budget_header_pool[budget_header_pool_used]->base_data.position.x = int16_t(x);
				budget_header_pool[budget_header_pool_used]->base_data.position.y = int16_t(y);
				if(!budget_header_pool[budget_header_pool_used]->parent) {
					budget_header_pool[budget_header_pool_used]->parent = destination;
					budget_header_pool[budget_header_pool_used]->impl_on_update(state);
					budget_header_pool[budget_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(budget_header_pool[budget_header_pool_used].get());
			((budgetwindow_budget_header_t*)(budget_header_pool[budget_header_pool_used].get()))->is_active = alternate;
				budget_row_pool[budget_row_pool_used]->base_data.position.x = int16_t(x);
				budget_row_pool[budget_row_pool_used]->base_data.position.y = int16_t(y +  budget_row_pool[0]->base_data.size.y + 0);
				budget_row_pool[budget_row_pool_used]->parent = destination;
				destination->children.push_back(budget_row_pool[budget_row_pool_used].get());
				((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->name = std::get<budget_row_option>(values[index]).name;
				((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->value = std::get<budget_row_option>(values[index]).value;
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->is_active = !alternate;
				budget_row_pool[budget_row_pool_used]->impl_on_update(state);
				budget_header_pool_used++;
				budget_row_pool_used++;
			}
			return measure_result{std::max(budget_header_pool[0]->base_data.size.x, budget_row_pool[0]->base_data.size.x), budget_header_pool[0]->base_data.size.y + budget_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(budget_row_pool.size() <= size_t(budget_row_pool_used)) budget_row_pool.emplace_back(make_budgetwindow_budget_row(state));
			budget_row_pool[budget_row_pool_used]->base_data.position.x = int16_t(x);
			budget_row_pool[budget_row_pool_used]->base_data.position.y = int16_t(y);
			budget_row_pool[budget_row_pool_used]->parent = destination;
			destination->children.push_back(budget_row_pool[budget_row_pool_used].get());
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->name = std::get<budget_row_option>(values[index]).name;
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->value = std::get<budget_row_option>(values[index]).value;
			((budgetwindow_budget_row_t*)(budget_row_pool[budget_row_pool_used].get()))->is_active = alternate;
			budget_row_pool[budget_row_pool_used]->impl_on_update(state);
			budget_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ budget_row_pool[0]->base_data.size.x, budget_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  budgetwindow_main_espenses_table_t::reset_pools() {
	section_header_pool_used = 0;
	neutral_spacer_pool_used = 0;
	top_spacer_pool_used = 0;
	bottom_spacer_pool_used = 0;
	budget_header_pool_used = 0;
	budget_row_pool_used = 0;
}
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
ui::message_result budgetwindow_main_close_button_t::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(key == sys::virtual_key::ESCAPE) {
		on_lbutton_down(state, 0, 0, mods);
		return ui::message_result::consumed;
	}
	return ui::message_result::unseen;
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
	auto tax_info = economy::explain_tax_income(state, state.local_player_nation);
	total += tax_info.poor;
	total += tax_info.mid;
	total += tax_info.rich;
	total += economy::estimate_tariff_import_income(state, state.local_player_nation);
	total += economy::estimate_tariff_export_income(state, state.local_player_nation);
	total += economy::estimate_gold_income(state, state.local_player_nation);
	set_text(state, text::prettify_currency(total));
// END
}
void budgetwindow_main_income_amount_t::on_create(sys::state& state) noexcept {
// BEGIN main::income_amount::create
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
	total += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military) * float(state.world.nation_get_military_spending(state.local_player_nation)) * float(state.world.nation_get_military_spending(state.local_player_nation)) / 10000.0f;
	total += economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education) * float(state.world.nation_get_education_spending(state.local_player_nation)) * float(state.world.nation_get_education_spending(state.local_player_nation)) / 10000.0f;
	total += economy::estimate_spendings_administration(state, state.local_player_nation, float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.f);
	total += economy::estimate_max_domestic_investment(state, state.local_player_nation) * float(state.world.nation_get_domestic_investment_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_overseas_penalty_spending(state, state.local_player_nation) * float(state.world.nation_get_overseas_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_subsidy_spending(state, state.local_player_nation);
	total += economy::estimate_construction_spending(state, state.local_player_nation);
	total += economy::estimate_land_spending(state, state.local_player_nation) * float(state.world.nation_get_land_spending(state.local_player_nation)) / 100.0f;
	total += economy::estimate_naval_spending(state, state.local_player_nation) * float(state.world.nation_get_naval_spending(state.local_player_nation)) / 100.0f;
	total += economy::interest_payment(state, state.local_player_nation);
	total += economy::estimate_stockpile_filling_spending(state, state.local_player_nation);
	set_text(state, text::prettify_currency(total));
// END
}
void budgetwindow_main_expenses_amount_t::on_create(sys::state& state) noexcept {
// BEGIN main::expenses_amount::create
// END
}
void budgetwindow_main_admin_eff1_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_admin_eff1_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_admin_eff1_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_admin_eff1_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::admin_eff1::update
// END
}
void budgetwindow_main_admin_eff1_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::admin_eff1::create
// END
}
void budgetwindow_main_admin_eff2_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_admin_eff2_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_admin_eff2_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_admin_eff2_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::admin_eff2::update
// END
}
void budgetwindow_main_admin_eff2_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::admin_eff2::create
// END
}
void budgetwindow_main_admin_eff_amount_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::admin_eff_amount::tooltip
	auto n = state.local_player_nation;

	{
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::val,
				text::fp_percentage{ 1.0f + state.world.nation_get_modifier_values(n, sys::national_mod_offsets::administrative_efficiency_modifier) });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, "admin_explain_1", m);
		text::close_layout_box(contents, box);
	}
	ui::active_modifiers_description(state, contents, n, 15, sys::national_mod_offsets::administrative_efficiency_modifier,
			false);
	{
		auto non_colonial = state.world.nation_get_non_colonial_population(n);
		auto total = non_colonial > 0.0f ? state.world.nation_get_non_colonial_bureaucrats(n) / non_colonial : 0.0f;

		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::val, text::fp_percentage{ total });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, "admin_explain_2", m);
		text::close_layout_box(contents, box);
	}
	{
		float issue_sum = 0.0f;
		for(auto i : state.culture_definitions.social_issues) {
			issue_sum = issue_sum + state.world.issue_option_get_administrative_multiplier(state.world.nation_get_issues(n, i));
		}
		auto from_issues = issue_sum * state.defines.bureaucracy_percentage_increment;

		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::val, text::fp_percentage{ (from_issues + state.defines.max_bureaucracy_percentage) });
		text::add_to_substitution_map(m, text::variable_type::x, text::fp_percentage{ state.defines.max_bureaucracy_percentage });
		text::add_to_substitution_map(m, text::variable_type::y, text::fp_percentage{ from_issues });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, "admin_explain_3", m);
		text::close_layout_box(contents, box);
	}
// END
}
void budgetwindow_main_admin_eff_amount_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_admin_eff_amount_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_main_admin_eff_amount_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_admin_eff_amount_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::admin_eff_amount::update
	set_text(state, text::format_percentage(state.world.nation_get_administrative_efficiency(state.local_player_nation)));
// END
}
void budgetwindow_main_admin_eff_amount_t::on_create(sys::state& state) noexcept {
// BEGIN main::admin_eff_amount::create
// END
}
void budgetwindow_main_welfare_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_welfare_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_welfare_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_welfare_label_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::welfare_label::update
// END
}
void budgetwindow_main_welfare_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::welfare_label::create
// END
}
void budgetwindow_main_chart_max_a_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_chart_max_a_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_chart_max_a_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_chart_max_a_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::chart_max_a::update
// END
}
void budgetwindow_main_chart_max_a_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::chart_max_a::create
// END
}
void budgetwindow_main_chart_min_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_chart_min_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_chart_min_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_chart_min_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::chart_min::update
// END
}
void budgetwindow_main_chart_min_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::chart_min::create
// END
}
void budgetwindow_main_chart_max_b_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_chart_max_b_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_chart_max_b_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_chart_max_b_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::chart_max_b::update
// END
}
void budgetwindow_main_chart_max_b_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::chart_max_b::create
// END
}
void budgetwindow_main_chart_poplabel_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_chart_poplabel_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_chart_poplabel_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_chart_poplabel_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::chart_poplabel::update
// END
}
void budgetwindow_main_chart_poplabel_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::chart_poplabel::create
// END
}
void budgetwindow_main_chart_needslabel_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_chart_needslabel_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_chart_needslabel_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_chart_needslabel_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::chart_needslabel::update
// END
}
void budgetwindow_main_chart_needslabel_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::chart_needslabel::create
// END
}
ui::message_result budgetwindow_main_welfare_chart_poor_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_welfare_chart_poor_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_welfare_chart_poor_t::set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max) {
	assert(datapoints.size() ==  32);
	float y_height = max - min;
	std::vector<float> scaled_datapoints = std::vector<float>(32);
	if(y_height == 0.f) {
		for(size_t i = 0; i < 32; i++) {
			scaled_datapoints[i] = .5f;
		}
	} else {
		for(size_t i = 0; i < 32; i++) {
			scaled_datapoints[i] = (datapoints[i] - min) / y_height;
		}
	}
	lines.set_y(scaled_datapoints.data());
}
void budgetwindow_main_welfare_chart_poor_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_linegraph(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, line_color.r, line_color.g, line_color.b, line_color.a, lines);
}
void budgetwindow_main_welfare_chart_poor_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::welfare_chart_poor::update
	float min = 0.f;
	float max = 0.f;
	for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {

		for(auto pop_loc : prov.get_province().get_pop_location()) {
			auto pop_id = pop_loc.get_pop();
			auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
			auto pop_size = pop_strata == uint8_t(culture::pop_strata::poor) ? state.world.pop_get_size(pop_id) : 0.f;

			max += pop_size;
		}
	}

	std::vector<float> datapoints(32);

	float integral = 0.f;
	float total_area = 0.f;

	for(uint32_t i = 0; i < 32; ++i) {
		float cutoff = (float)i / 32 + 0.01f;
		float value = 0.f;

		for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {

			for(auto pop_loc : prov.get_province().get_pop_location()) {
				auto pop_id = pop_loc.get_pop();
				auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
				auto pop_size = pop_strata == uint8_t(culture::pop_strata::poor) ? state.world.pop_get_size(pop_id) : 0.f;

				float total =
					pop_demographics::get_luxury_needs(state, pop_id)
					+ pop_demographics::get_everyday_needs(state, pop_id)
					+ pop_demographics::get_life_needs(state, pop_id);

				if(total / 3.f >= cutoff)
					value += pop_size;

				integral += total / 3.f * pop_size;
				total_area += pop_size;
			}
		}

		datapoints[i] = value;
	}
	set_data_points(state, datapoints, min, max);
// END
}
void budgetwindow_main_welfare_chart_poor_t::on_create(sys::state& state) noexcept {
// BEGIN main::welfare_chart_poor::create
// END
}
ui::message_result budgetwindow_main_welfare_chart_middle_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_welfare_chart_middle_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_welfare_chart_middle_t::set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max) {
	assert(datapoints.size() ==  32);
	float y_height = max - min;
	std::vector<float> scaled_datapoints = std::vector<float>(32);
	if(y_height == 0.f) {
		for(size_t i = 0; i < 32; i++) {
			scaled_datapoints[i] = .5f;
		}
	} else {
		for(size_t i = 0; i < 32; i++) {
			scaled_datapoints[i] = (datapoints[i] - min) / y_height;
		}
	}
	lines.set_y(scaled_datapoints.data());
}
void budgetwindow_main_welfare_chart_middle_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_linegraph(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, line_color.r, line_color.g, line_color.b, line_color.a, lines);
}
void budgetwindow_main_welfare_chart_middle_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::welfare_chart_middle::update
	float min = 0.f;
	float max = 0.f;
	for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {

		for(auto pop_loc : prov.get_province().get_pop_location()) {
			auto pop_id = pop_loc.get_pop();
			auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
			auto pop_size = pop_strata == uint8_t(culture::pop_strata::middle) ? state.world.pop_get_size(pop_id) : 0.f;

			max += pop_size;
		}
	}

	std::vector<float> datapoints(32);

	float integral = 0.f;
	float total_area = 0.f;

	for(uint32_t i = 0; i < 32; ++i) {
		float cutoff = (float)i / 32 + 0.01f;
		float value = 0.f;

		for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {

			for(auto pop_loc : prov.get_province().get_pop_location()) {
				auto pop_id = pop_loc.get_pop();
				auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
				auto pop_size = pop_strata == uint8_t(culture::pop_strata::middle) ? state.world.pop_get_size(pop_id) : 0.f;

				float total =
					pop_demographics::get_luxury_needs(state, pop_id)
					+ pop_demographics::get_everyday_needs(state, pop_id)
					+ pop_demographics::get_life_needs(state, pop_id);

				if(total / 3.f >= cutoff)
					value += pop_size;

				integral += total / 3.f * pop_size;
				total_area += pop_size;
			}
		}

		datapoints[i] = value;
	}
	set_data_points(state, datapoints, min, max);
// END
}
void budgetwindow_main_welfare_chart_middle_t::on_create(sys::state& state) noexcept {
// BEGIN main::welfare_chart_middle::create
// END
}
ui::message_result budgetwindow_main_welfare_chart_rich_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_welfare_chart_rich_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_welfare_chart_rich_t::set_data_points(sys::state& state, std::vector<float> const& datapoints, float min, float max) {
	assert(datapoints.size() ==  32);
	float y_height = max - min;
	std::vector<float> scaled_datapoints = std::vector<float>(32);
	if(y_height == 0.f) {
		for(size_t i = 0; i < 32; i++) {
			scaled_datapoints[i] = .5f;
		}
	} else {
		for(size_t i = 0; i < 32; i++) {
			scaled_datapoints[i] = (datapoints[i] - min) / y_height;
		}
	}
	lines.set_y(scaled_datapoints.data());
}
void budgetwindow_main_welfare_chart_rich_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_linegraph(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, line_color.r, line_color.g, line_color.b, line_color.a, lines);
}
void budgetwindow_main_welfare_chart_rich_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::welfare_chart_rich::update
	float min = 0.f;
	float max = 0.f;
	for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {

		for(auto pop_loc : prov.get_province().get_pop_location()) {
			auto pop_id = pop_loc.get_pop();
			auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
			auto pop_size = pop_strata == uint8_t(culture::pop_strata::rich) ? state.world.pop_get_size(pop_id) : 0.f;

			max += pop_size;
		}
	}

	std::vector<float> datapoints(32);

	float integral = 0.f;
	float total_area = 0.f;

	for(uint32_t i = 0; i < 32; ++i) {
		float cutoff = (float)i / 32 + 0.01f;
		float value = 0.f;

		for(auto prov : state.world.nation_get_province_ownership(state.local_player_nation)) {

			for(auto pop_loc : prov.get_province().get_pop_location()) {
				auto pop_id = pop_loc.get_pop();
				auto pop_strata = state.world.pop_type_get_strata(state.world.pop_get_poptype(pop_id));
				auto pop_size = pop_strata == uint8_t(culture::pop_strata::rich) ? state.world.pop_get_size(pop_id) : 0.f;

				float total =
					pop_demographics::get_luxury_needs(state, pop_id)
					+ pop_demographics::get_everyday_needs(state, pop_id)
					+ pop_demographics::get_life_needs(state, pop_id);

				if(total / 3.f >= cutoff)
					value += pop_size;

				integral += total / 3.f * pop_size;
				total_area += pop_size;
			}
		}

		datapoints[i] = value;
	}
	set_data_points(state, datapoints, min, max);
// END
}
void budgetwindow_main_welfare_chart_rich_t::on_create(sys::state& state) noexcept {
// BEGIN main::welfare_chart_rich::create
// END
}
void budgetwindow_main_hover_poor_t::on_hover(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_poor::on_hover
	main.welfare_chart_middle->line_color.a = 0.2f;
	main.welfare_chart_rich->line_color.a = 0.2f;
// END
}
void budgetwindow_main_hover_poor_t::on_hover_end(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_poor::on_hover_end
	main.welfare_chart_middle->line_color.a = 1.0f;
	main.welfare_chart_rich->line_color.a = 1.0f;
// END
}
ui::message_result budgetwindow_main_hover_poor_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_hover_poor_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_hover_poor_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_hover_poor_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_hover_poor_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
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
void budgetwindow_main_hover_poor_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_poor::update
// END
}
void budgetwindow_main_hover_poor_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::hover_poor::create
// END
}
void budgetwindow_main_hover_middle_t::on_hover(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_middle::on_hover
	main.welfare_chart_poor->line_color.a = 0.2f;
	main.welfare_chart_rich->line_color.a = 0.2f;
// END
}
void budgetwindow_main_hover_middle_t::on_hover_end(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_middle::on_hover_end
	main.welfare_chart_poor->line_color.a = 1.0f;
	main.welfare_chart_rich->line_color.a = 1.0f;
// END
}
ui::message_result budgetwindow_main_hover_middle_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_hover_middle_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_hover_middle_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_hover_middle_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_hover_middle_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
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
void budgetwindow_main_hover_middle_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_middle::update
// END
}
void budgetwindow_main_hover_middle_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::hover_middle::create
// END
}
void budgetwindow_main_hover_rich_t::on_hover(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_rich::on_hover
	main.welfare_chart_poor->line_color.a = 0.2f;
	main.welfare_chart_middle->line_color.a = 0.2f;
// END
}
void budgetwindow_main_hover_rich_t::on_hover_end(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_rich::on_hover_end
	main.welfare_chart_poor->line_color.a = 1.0f;
	main.welfare_chart_middle->line_color.a = 1.0f;
// END
}
ui::message_result budgetwindow_main_hover_rich_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_hover_rich_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_hover_rich_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_hover_rich_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_hover_rich_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
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
void budgetwindow_main_hover_rich_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::hover_rich::update
// END
}
void budgetwindow_main_hover_rich_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::hover_rich::create
// END
}
void budgetwindow_main_debt_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_debt_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_debt_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_debt_label_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::debt_label::update
// END
}
void budgetwindow_main_debt_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::debt_label::create
// END
}
ui::message_result budgetwindow_main_debt_enable_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::debt_enable::lbutton_action
	command::enable_debt(state, state.local_player_nation, !state.world.nation_get_is_debt_spending(state.local_player_nation));
// END
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_main_debt_enable_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_main_debt_enable_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::debt_enable::tooltip
	auto last_br = state.world.nation_get_bankrupt_until(state.local_player_nation);
	if(last_br && state.current_date < last_br) {
		text::add_line(state, contents, "alice_currently_bankrupt", text::variable_type::x, last_br);
	} else if(economy::max_loan(state, state.local_player_nation) <= 0.0f) {
		text::add_line(state, contents, "alice_no_loans_possible");
	} else {
		text::add_line(state, contents, "alice_debt_spending");
		text::add_line_break_to_layout(state, contents);
		text::add_line(state, contents, "alice_loan_size", text::variable_type::x, text::fp_currency{ economy::max_loan(state, state.local_player_nation) });

		auto mod = state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::max_loan_modifier);
		text::add_line(state, contents, "alice_loan_size_mod", text::variable_type::x, text::fp_percentage_one_place{ mod });
	}

	text::add_line_break_to_layout(state, contents);

	auto capitalists_savings_ratio = state.defines.alice_save_capitalist + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::capitalist_savings);
	auto middle_class_savings_ratio = state.defines.alice_save_middle_class + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::middle_class_savings);
	auto farmers_savings_ratio = state.defines.alice_save_farmer + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::farmers_savings);
	auto landowners_savings_ratio = state.defines.alice_save_aristocrat + state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::aristocrat_savings);

	if(capitalists_savings_ratio > 0.f) {
		text::add_line(state, contents, "capitalists_savings_ratio", text::variable_type::x, text::fp_percentage{ capitalists_savings_ratio });
	}
	if(landowners_savings_ratio > 0.f) {
		text::add_line(state, contents, "landowners_savings_ratio", text::variable_type::x, text::fp_percentage{ landowners_savings_ratio });
	}
	if(middle_class_savings_ratio > 0.f) {
		text::add_line(state, contents, "middle_class_savings_ratio", text::variable_type::x, text::fp_percentage{ middle_class_savings_ratio });
	}
	if(farmers_savings_ratio > 0.f) {
		text::add_line(state, contents, "farmers_savings_ratio", text::variable_type::x, text::fp_percentage{ farmers_savings_ratio });
	}
// END
}
void budgetwindow_main_debt_enable_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_debt_enable_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_debt_enable_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
	else
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void budgetwindow_main_debt_enable_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::debt_enable::update
	is_active = state.world.nation_get_is_debt_spending(state.local_player_nation);
	disabled = false;
	auto last_br = state.world.nation_get_bankrupt_until(state.local_player_nation);
	if(last_br && state.current_date < last_br)
		disabled = true;
	if(economy::max_loan(state, state.local_player_nation) <= 0.0f)
		disabled = true;
// END
}
void budgetwindow_main_debt_enable_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::debt_enable::create
// END
}
void budgetwindow_main_total_debt_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_total_debt_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_total_debt_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_total_debt_label_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::total_debt_label::update
// END
}
void budgetwindow_main_total_debt_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::total_debt_label::create
// END
}
void budgetwindow_main_max_debt_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_max_debt_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void budgetwindow_main_max_debt_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_max_debt_label_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::max_debt_label::update
// END
}
void budgetwindow_main_max_debt_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::max_debt_label::create
// END
}
void budgetwindow_main_total_debt_amount_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_total_debt_amount_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_main_total_debt_amount_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_total_debt_amount_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::total_debt_amount::update
	auto amount = state.world.nation_get_local_loan(state.local_player_nation);
	set_text(state, text::prettify_currency(amount));
// END
}
void budgetwindow_main_total_debt_amount_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::total_debt_amount::create
// END
}
void budgetwindow_main_max_debt_amount_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 16), 0, text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void budgetwindow_main_max_debt_amount_t::on_reset_text(sys::state& state) noexcept {
}
void budgetwindow_main_max_debt_amount_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void budgetwindow_main_max_debt_amount_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::max_debt_amount::update
	auto amount = economy::max_loan(state, state.local_player_nation);
	set_text(state, text::prettify_currency(amount));
// END
}
void budgetwindow_main_max_debt_amount_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::max_debt_amount::create
// END
}
ui::message_result budgetwindow_main_debt_chart_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_debt_chart_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_debt_chart_t::update_chart(sys::state& state) {
	std::sort(graph_content.begin(), graph_content.end(), [](auto const& a, auto const& b) { return a.amount > b.amount; });
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 200; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 200; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(200)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void budgetwindow_main_debt_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::debt_chart::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
		text::close_layout_box(contents, box);
// END
	}
}
void budgetwindow_main_debt_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void budgetwindow_main_debt_chart_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::debt_chart::update
	graph_content.clear();

	auto interest = economy::interest_payment(state, state.local_player_nation) > 0;
	set_visible(state,  interest);
	if(!interest)
		return;

	auto t = state.world.nation_get_local_loan(state.local_player_nation);

	if(t > 0.0f) {
		int32_t num_gp = 7;
		if(t <= 1000.0f) {
			num_gp = 1;
		} else if(t <= 2000.0f) {
			num_gp = 2;
		} else if(t <= 4000.0f) {
			num_gp = 4;
		}
		float total_share = 0.0f;
		float share = 10.0f;
		for(auto n : state.nations_by_rank) {
			if(state.world.nation_get_is_great_power(n) && n != state.local_player_nation) {
				auto c = state.world.nation_get_color(n);
				graph_content.emplace_back(graph_entry{ text::get_name(state, n), ogl::color3f{sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) }, share });
				total_share += share;
				share -= 1.0f;

				--num_gp;
				if(num_gp <= 0)
					break;
			}
		}
		for(auto& i : graph_content) {
			i.amount *= i.amount * t / total_share;
		}
	}
	update_chart(state);
// END
}
void budgetwindow_main_debt_chart_t::on_create(sys::state& state) noexcept {
// BEGIN main::debt_chart::create
// END
}
ui::message_result budgetwindow_main_debt_overlay_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_main_debt_overlay_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_main_debt_overlay_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
void budgetwindow_main_debt_overlay_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent)); 
// BEGIN main::debt_overlay::update
	set_visible(state, economy::interest_payment(state, state.local_player_nation) > 0);
// END
}
void budgetwindow_main_debt_overlay_t::on_create(sys::state& state) noexcept {
// BEGIN main::debt_overlay::create
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
	income_table.update(state, this);
	espenses_table.update(state, this);
	remake_layout(state, true);
}
void budgetwindow_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "close_button") {
					temp.ptr = close_button.get();
				}
				if(cname == "title") {
					temp.ptr = title.get();
				}
				if(cname == "income_label") {
					temp.ptr = income_label.get();
				}
				if(cname == "income_amount") {
					temp.ptr = income_amount.get();
				}
				if(cname == "expenses_label") {
					temp.ptr = expenses_label.get();
				}
				if(cname == "expenses_amount") {
					temp.ptr = expenses_amount.get();
				}
				if(cname == "admin_eff1") {
					temp.ptr = admin_eff1.get();
				}
				if(cname == "admin_eff2") {
					temp.ptr = admin_eff2.get();
				}
				if(cname == "admin_eff_amount") {
					temp.ptr = admin_eff_amount.get();
				}
				if(cname == "welfare_label") {
					temp.ptr = welfare_label.get();
				}
				if(cname == "chart_max_a") {
					temp.ptr = chart_max_a.get();
				}
				if(cname == "chart_min") {
					temp.ptr = chart_min.get();
				}
				if(cname == "chart_max_b") {
					temp.ptr = chart_max_b.get();
				}
				if(cname == "chart_poplabel") {
					temp.ptr = chart_poplabel.get();
				}
				if(cname == "chart_needslabel") {
					temp.ptr = chart_needslabel.get();
				}
				if(cname == "welfare_chart_poor") {
					temp.ptr = welfare_chart_poor.get();
				}
				if(cname == "welfare_chart_middle") {
					temp.ptr = welfare_chart_middle.get();
				}
				if(cname == "welfare_chart_rich") {
					temp.ptr = welfare_chart_rich.get();
				}
				if(cname == "hover_poor") {
					temp.ptr = hover_poor.get();
				}
				if(cname == "hover_middle") {
					temp.ptr = hover_middle.get();
				}
				if(cname == "hover_rich") {
					temp.ptr = hover_rich.get();
				}
				if(cname == "debt_label") {
					temp.ptr = debt_label.get();
				}
				if(cname == "debt_enable") {
					temp.ptr = debt_enable.get();
				}
				if(cname == "total_debt_label") {
					temp.ptr = total_debt_label.get();
				}
				if(cname == "max_debt_label") {
					temp.ptr = max_debt_label.get();
				}
				if(cname == "total_debt_amount") {
					temp.ptr = total_debt_amount.get();
				}
				if(cname == "max_debt_amount") {
					temp.ptr = max_debt_amount.get();
				}
				if(cname == "debt_chart") {
					temp.ptr = debt_chart.get();
				}
				if(cname == "debt_overlay") {
					temp.ptr = debt_overlay.get();
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
				if(cname == "main") {
					temp.ptr = make_budgetwindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_budgetwindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_budgetwindow_neutral_spacer(state);
				}
				if(cname == "top_spacer") {
					temp.ptr = make_budgetwindow_top_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_budgetwindow_bottom_spacer(state);
				}
				if(cname == "budget_row") {
					temp.ptr = make_budgetwindow_budget_row(state);
				}
				if(cname == "budget_header") {
					temp.ptr = make_budgetwindow_budget_header(state);
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
				if(cname == "income_table") {
					temp.generator = &income_table;
				}
				if(cname == "espenses_table") {
					temp.generator = &espenses_table;
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
				children.push_back(ch_res.get());
				gui_inserts.push_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "close_button") {
			close_button = std::make_unique<budgetwindow_main_close_button_t>();
			close_button->parent = this;
			auto cptr = close_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "title") {
			title = std::make_unique<budgetwindow_main_title_t>();
			title->parent = this;
			auto cptr = title.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "income_label") {
			income_label = std::make_unique<budgetwindow_main_income_label_t>();
			income_label->parent = this;
			auto cptr = income_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "income_amount") {
			income_amount = std::make_unique<budgetwindow_main_income_amount_t>();
			income_amount->parent = this;
			auto cptr = income_amount.get();
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
		if(child_data.name == "expenses_label") {
			expenses_label = std::make_unique<budgetwindow_main_expenses_label_t>();
			expenses_label->parent = this;
			auto cptr = expenses_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "expenses_amount") {
			expenses_amount = std::make_unique<budgetwindow_main_expenses_amount_t>();
			expenses_amount->parent = this;
			auto cptr = expenses_amount.get();
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
		if(child_data.name == "admin_eff1") {
			admin_eff1 = std::make_unique<budgetwindow_main_admin_eff1_t>();
			admin_eff1->parent = this;
			auto cptr = admin_eff1.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "admin_eff2") {
			admin_eff2 = std::make_unique<budgetwindow_main_admin_eff2_t>();
			admin_eff2->parent = this;
			auto cptr = admin_eff2.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "admin_eff_amount") {
			admin_eff_amount = std::make_unique<budgetwindow_main_admin_eff_amount_t>();
			admin_eff_amount->parent = this;
			auto cptr = admin_eff_amount.get();
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
		if(child_data.name == "welfare_label") {
			welfare_label = std::make_unique<budgetwindow_main_welfare_label_t>();
			welfare_label->parent = this;
			auto cptr = welfare_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "chart_max_a") {
			chart_max_a = std::make_unique<budgetwindow_main_chart_max_a_t>();
			chart_max_a->parent = this;
			auto cptr = chart_max_a.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "chart_min") {
			chart_min = std::make_unique<budgetwindow_main_chart_min_t>();
			chart_min->parent = this;
			auto cptr = chart_min.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "chart_max_b") {
			chart_max_b = std::make_unique<budgetwindow_main_chart_max_b_t>();
			chart_max_b->parent = this;
			auto cptr = chart_max_b.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "chart_poplabel") {
			chart_poplabel = std::make_unique<budgetwindow_main_chart_poplabel_t>();
			chart_poplabel->parent = this;
			auto cptr = chart_poplabel.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "chart_needslabel") {
			chart_needslabel = std::make_unique<budgetwindow_main_chart_needslabel_t>();
			chart_needslabel->parent = this;
			auto cptr = chart_needslabel.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "welfare_chart_poor") {
			welfare_chart_poor = std::make_unique<budgetwindow_main_welfare_chart_poor_t>();
			welfare_chart_poor->parent = this;
			auto cptr = welfare_chart_poor.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->line_color = child_data.table_highlight_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "welfare_chart_middle") {
			welfare_chart_middle = std::make_unique<budgetwindow_main_welfare_chart_middle_t>();
			welfare_chart_middle->parent = this;
			auto cptr = welfare_chart_middle.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->line_color = child_data.table_highlight_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "welfare_chart_rich") {
			welfare_chart_rich = std::make_unique<budgetwindow_main_welfare_chart_rich_t>();
			welfare_chart_rich->parent = this;
			auto cptr = welfare_chart_rich.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->line_color = child_data.table_highlight_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "hover_poor") {
			hover_poor = std::make_unique<budgetwindow_main_hover_poor_t>();
			hover_poor->parent = this;
			auto cptr = hover_poor.get();
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
		if(child_data.name == "hover_middle") {
			hover_middle = std::make_unique<budgetwindow_main_hover_middle_t>();
			hover_middle->parent = this;
			auto cptr = hover_middle.get();
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
		if(child_data.name == "hover_rich") {
			hover_rich = std::make_unique<budgetwindow_main_hover_rich_t>();
			hover_rich->parent = this;
			auto cptr = hover_rich.get();
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
		if(child_data.name == "debt_label") {
			debt_label = std::make_unique<budgetwindow_main_debt_label_t>();
			debt_label->parent = this;
			auto cptr = debt_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "debt_enable") {
			debt_enable = std::make_unique<budgetwindow_main_debt_enable_t>();
			debt_enable->parent = this;
			auto cptr = debt_enable.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->alt_texture_key = child_data.alt_texture;
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
		if(child_data.name == "total_debt_label") {
			total_debt_label = std::make_unique<budgetwindow_main_total_debt_label_t>();
			total_debt_label->parent = this;
			auto cptr = total_debt_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "max_debt_label") {
			max_debt_label = std::make_unique<budgetwindow_main_max_debt_label_t>();
			max_debt_label->parent = this;
			auto cptr = max_debt_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "total_debt_amount") {
			total_debt_amount = std::make_unique<budgetwindow_main_total_debt_amount_t>();
			total_debt_amount->parent = this;
			auto cptr = total_debt_amount.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "max_debt_amount") {
			max_debt_amount = std::make_unique<budgetwindow_main_max_debt_amount_t>();
			max_debt_amount->parent = this;
			auto cptr = max_debt_amount.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
		if(child_data.name == "debt_chart") {
			debt_chart = std::make_unique<budgetwindow_main_debt_chart_t>();
			debt_chart->parent = this;
			auto cptr = debt_chart.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "debt_overlay") {
			debt_overlay = std::make_unique<budgetwindow_main_debt_overlay_t>();
			debt_overlay->parent = this;
			auto cptr = debt_overlay.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == ".tabincome_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			income_table_ascending_icon_key = main_section.read<std::string_view>();
			income_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read(income_table_divider_color);
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			running_w_total += col_section.read<int16_t>();
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			income_table_item_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			income_table_item_name_column_start = running_w_total;
			col_section.read(income_table_item_name_column_width);
			running_w_total += income_table_item_name_column_width;
			col_section.read(income_table_item_name_column_text_color);
			col_section.read(income_table_item_name_header_text_color);
			col_section.read(income_table_item_name_text_alignment);
			income_table_item_value_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			income_table_item_value_column_start = running_w_total;
			col_section.read(income_table_item_value_column_width);
			running_w_total += income_table_item_value_column_width;
			col_section.read(income_table_item_value_column_text_color);
			col_section.read(income_table_item_value_header_text_color);
			col_section.read(income_table_item_value_text_alignment);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	income_table.on_create(state, this);
	espenses_table.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::label::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_text(state, text::produce_simple_string(state, "alice_budget_diplo_income")); break;
	case budget_categories::poor_tax: set_text(state, text::produce_simple_string(state, "budget_tax_poor")); break;
	case budget_categories::middle_tax: set_text(state, text::produce_simple_string(state, "budget_tax_middle")); break;
	case budget_categories::rich_tax: set_text(state, text::produce_simple_string(state, "budget_tax_rich")); break;
	case budget_categories::tariffs_import: set_text(state, text::produce_simple_string(state, "alice_budget_tariffs_import")); break;
	case budget_categories::tariffs_export: set_text(state, text::produce_simple_string(state, "alice_budget_tariffs_export")); break;
	case budget_categories::gold: set_text(state, text::produce_simple_string(state, "alice_budget_gold")); break;
	case budget_categories::diplomatic_expenses: set_text(state, text::produce_simple_string(state, "alice_budget_diplo_expenses")); break;
	case budget_categories::social: set_text(state, text::produce_simple_string(state, "alice_budget_social_spending")); break;
	case budget_categories::military: set_text(state, text::produce_simple_string(state, "military")); break;
	case budget_categories::education: set_text(state, text::produce_simple_string(state, "education")); break;
	case budget_categories::admin: set_text(state, text::produce_simple_string(state, "administration")); break;
	case budget_categories::domestic_investment: set_text(state, text::produce_simple_string(state, "alice_budget_domestic_investment")); break;
	case budget_categories::overseas_spending: set_text(state, text::produce_simple_string(state, "alice_budget_overseas_maintanance")); break;
	case budget_categories::subsidies: set_text(state, text::produce_simple_string(state, "budget_industrial_subsidies")); break;
	case budget_categories::construction: set_text(state, text::produce_simple_string(state, "alice_budget_construction")); break;
	case budget_categories::army_upkeep: set_text(state, text::produce_simple_string(state, "alice_budget_army_upkeep")); break;
	case budget_categories::navy_upkeep: set_text(state, text::produce_simple_string(state, "alice_budget_navy_upkeep")); break;
	case budget_categories::debt_payment: set_text(state, text::produce_simple_string(state, "alice_budget_debt_service")); break;
	case budget_categories::stockpile: set_text(state, text::produce_simple_string(state, "national_stockpile")); break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN section_header::llbutton::lbutton_action
	auto vals = command::make_empty_budget_settings();
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: vals.poor_tax = economy::budget_minimums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: vals.middle_tax = economy::budget_minimums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: vals.rich_tax = economy::budget_minimums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs_import: vals.tariffs_import = economy::budget_minimums(state, state.local_player_nation).tariffs_import; break;
	case budget_categories::tariffs_export: vals.tariffs_export = economy::budget_minimums(state, state.local_player_nation).tariffs_export; break;
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
	case budget_categories::stockpile: break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::llbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs_import: set_visible(state, true); break;
	case budget_categories::tariffs_export: set_visible(state, true); break;
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
	case budget_categories::stockpile: set_visible(state, false);  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
	if(mods == sys::key_modifiers::modifiers_shift) {
// BEGIN section_header::lbutton::lbutton_shift_action
		auto vals = command::make_empty_budget_settings();
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: vals.poor_tax = int8_t(std::clamp(state.world.nation_get_poor_tax(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::middle_tax: vals.middle_tax = int8_t(std::clamp(state.world.nation_get_middle_tax(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::rich_tax: vals.rich_tax = int8_t(std::clamp(state.world.nation_get_rich_tax(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::tariffs_import: vals.tariffs_import = int8_t(std::clamp(state.world.nation_get_tariffs_import(state.local_player_nation) - 1, 0, 100)); break;
		case budget_categories::tariffs_export: vals.tariffs_export = int8_t(std::clamp(state.world.nation_get_tariffs_export(state.local_player_nation) - 1, 0, 100)); break;
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
		case budget_categories::stockpile:  break;
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
	case budget_categories::tariffs_import: vals.tariffs_import = int8_t(std::clamp(state.world.nation_get_tariffs_import(state.local_player_nation) - 10, 0, 100)); break;
	case budget_categories::tariffs_export: vals.tariffs_export = int8_t(std::clamp(state.world.nation_get_tariffs_export(state.local_player_nation) - 10, 0, 100)); break;
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
	case budget_categories::stockpile:  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::lbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs_import: set_visible(state, true); break;
	case budget_categories::tariffs_export: set_visible(state, true); break;
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
	case budget_categories::stockpile: set_visible(state, false);  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
	if(mods == sys::key_modifiers::modifiers_shift) {
// BEGIN section_header::rbutton::lbutton_shift_action
		auto vals = command::make_empty_budget_settings();
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: vals.poor_tax = int8_t(std::clamp(state.world.nation_get_poor_tax(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::middle_tax: vals.middle_tax = int8_t(std::clamp(state.world.nation_get_middle_tax(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::rich_tax: vals.rich_tax = int8_t(std::clamp(state.world.nation_get_rich_tax(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::tariffs_import: vals.tariffs_import = int8_t(std::clamp(state.world.nation_get_tariffs_import(state.local_player_nation) + 1, 0, 100)); break;
		case budget_categories::tariffs_export: vals.tariffs_export = int8_t(std::clamp(state.world.nation_get_tariffs_export(state.local_player_nation) + 1, 0, 100)); break;
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
		case budget_categories::stockpile: break;
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
	case budget_categories::tariffs_import: vals.tariffs_import = int8_t(std::clamp(state.world.nation_get_tariffs_import(state.local_player_nation) + 10, 0, 100)); break;
	case budget_categories::tariffs_export: vals.tariffs_export = int8_t(std::clamp(state.world.nation_get_tariffs_export(state.local_player_nation) + 10, 0, 100)); break;
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
	case budget_categories::stockpile: break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::rbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs_import: set_visible(state, true); break;
	case budget_categories::tariffs_export: set_visible(state, true); break;
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
	case budget_categories::stockpile: set_visible(state, false);  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN section_header::rrbutton::lbutton_action
	auto vals = command::make_empty_budget_settings();
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: vals.poor_tax = economy::budget_maximums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: vals.middle_tax = economy::budget_maximums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: vals.rich_tax = economy::budget_maximums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs_import: vals.tariffs_import = economy::budget_maximums(state, state.local_player_nation).tariffs_import; break;
	case budget_categories::tariffs_export: vals.tariffs_export = economy::budget_maximums(state, state.local_player_nation).tariffs_export; break;
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
	case budget_categories::stockpile:  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::rrbutton::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_visible(state, false); break;
	case budget_categories::poor_tax: set_visible(state, true); break;
	case budget_categories::middle_tax: set_visible(state, true); break;
	case budget_categories::rich_tax: set_visible(state, true); break;
	case budget_categories::tariffs_import: set_visible(state, true); break;
	case budget_categories::tariffs_export: set_visible(state, true); break;
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
	case budget_categories::stockpile: set_visible(state, false);  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::setting_amount::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_text(state, ""); break;
	case budget_categories::poor_tax: set_text(state, std::to_string(state.world.nation_get_poor_tax(state.local_player_nation))); break;
	case budget_categories::middle_tax: set_text(state, std::to_string(state.world.nation_get_middle_tax(state.local_player_nation))); break;
	case budget_categories::rich_tax: set_text(state, std::to_string(state.world.nation_get_rich_tax(state.local_player_nation))); break;
	case budget_categories::tariffs_import: set_text(state, std::to_string(state.world.nation_get_tariffs_import(state.local_player_nation))); break;
	case budget_categories::tariffs_export: set_text(state, std::to_string(state.world.nation_get_tariffs_export(state.local_player_nation))); break;
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
	case budget_categories::stockpile: set_text(state, "");  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::expand_button::update
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: disabled = (economy::estimate_diplomatic_income(state, state.local_player_nation) <= 0); break;
	case budget_categories::poor_tax: disabled = false; break;
	case budget_categories::middle_tax: disabled = false; break;
	case budget_categories::rich_tax: disabled = false; break;
	case budget_categories::tariffs_import: disabled = (economy::estimate_tariff_import_income(state, state.local_player_nation) <= 0); break;
	case budget_categories::tariffs_export: disabled = (economy::estimate_tariff_export_income(state, state.local_player_nation) <= 0); break;
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
	case budget_categories::stockpile: disabled = (economy::estimate_stockpile_filling_spending(state, state.local_player_nation) <= 0);  break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::total_amount::update
	auto info = economy::explain_tax_income(state, state.local_player_nation);
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income: set_text(state, text::prettify_currency(economy::estimate_diplomatic_income(state, state.local_player_nation))); break;
	case budget_categories::poor_tax: set_text(state, text::prettify_currency(info.poor)); break;
	case budget_categories::middle_tax: set_text(state, text::prettify_currency(info.mid)); break;
	case budget_categories::rich_tax: set_text(state, text::prettify_currency(info.rich)); break;
	case budget_categories::tariffs_import: set_text(state, text::prettify_currency(economy::estimate_tariff_import_income(state, state.local_player_nation))); break;
	case budget_categories::tariffs_export: set_text(state, text::prettify_currency(economy::estimate_tariff_export_income(state, state.local_player_nation))); break;
	case budget_categories::gold: set_text(state, text::prettify_currency(economy::estimate_gold_income(state, state.local_player_nation))); break;
	case budget_categories::diplomatic_expenses: set_text(state, text::prettify_currency(economy::estimate_diplomatic_expenses(state, state.local_player_nation))); break;
	case budget_categories::social: set_text(state,  text::prettify_currency(economy::estimate_social_spending(state, state.local_player_nation) * float(state.world.nation_get_social_spending(state.local_player_nation)) / 100.0f)); break;
	case budget_categories::military: set_text(state, text::prettify_currency(economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::military) * float(state.world.nation_get_military_spending(state.local_player_nation)) * float(state.world.nation_get_military_spending(state.local_player_nation)) / 10000.0f)); break;
	case budget_categories::education: set_text(state, text::prettify_currency(economy::estimate_pop_payouts_by_income_type(state, state.local_player_nation, culture::income_type::education) * float(state.world.nation_get_education_spending(state.local_player_nation)) * float(state.world.nation_get_education_spending(state.local_player_nation)) / 10000.0f)); break;
	case budget_categories::admin: set_text(state, text::prettify_currency(economy::estimate_spendings_administration(state, state.local_player_nation, float(state.world.nation_get_administrative_spending(state.local_player_nation)) / 100.f))); break;
	case budget_categories::domestic_investment: set_text(state, text::prettify_currency(economy::estimate_max_domestic_investment(state, state.local_player_nation) * float(state.world.nation_get_domestic_investment_spending(state.local_player_nation)) / 100.0f)); break;
	case budget_categories::overseas_spending: set_text(state, text::prettify_currency(economy::estimate_overseas_penalty_spending(state, state.local_player_nation) * float(state.world.nation_get_overseas_spending(state.local_player_nation)) / 100.0f)); break;
	case budget_categories::subsidies: set_text(state, text::prettify_currency(economy::estimate_subsidy_spending(state, state.local_player_nation))); break;
	case budget_categories::construction: set_text(state, text::prettify_currency(economy::estimate_construction_spending(state, state.local_player_nation))); break;
	case budget_categories::army_upkeep: set_text(state, text::prettify_currency(economy::estimate_land_spending(state, state.local_player_nation) * float(state.world.nation_get_land_spending(state.local_player_nation)) / 100.0f)); break;
	case budget_categories::navy_upkeep: set_text(state, text::prettify_currency(economy::estimate_naval_spending(state, state.local_player_nation) * float(state.world.nation_get_naval_spending(state.local_player_nation)) / 100.0f)); break;
	case budget_categories::debt_payment: set_text(state, text::prettify_currency(economy::interest_payment(state, state.local_player_nation))); break;
	case budget_categories::stockpile: set_text(state, text::prettify_currency(economy::estimate_stockpile_filling_spending(state, state.local_player_nation))); break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::min_setting::tooltip
	auto value = 0;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_minimums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_minimums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_minimums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs_export: value = economy::budget_minimums(state, state.local_player_nation).tariffs_export; break;
	case budget_categories::tariffs_import: value = economy::budget_minimums(state, state.local_player_nation).tariffs_import; break;
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
	case budget_categories::stockpile: break;
	default:  break;
	}
	if(value == 0) {

	} else {
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tax, true); break;
		case budget_categories::middle_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tax, true); break;
		case budget_categories::rich_tax:  ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tax, true); break;
		case budget_categories::tariffs_import: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tariff, true); break;
		case budget_categories::tariffs_export: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::min_tariff, true); break;
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
		case budget_categories::stockpile: break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::min_setting::update
	auto value = 0;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_minimums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_minimums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_minimums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs_import: value = economy::budget_minimums(state, state.local_player_nation).tariffs_import; break;
	case budget_categories::tariffs_export: value = economy::budget_minimums(state, state.local_player_nation).tariffs_export; break;
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
	case budget_categories::stockpile: break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::max_setting::tooltip
	auto value = 100;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_maximums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_maximums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_maximums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs_import: value = economy::budget_maximums(state, state.local_player_nation).tariffs_import; break;
	case budget_categories::tariffs_export: value = economy::budget_maximums(state, state.local_player_nation).tariffs_export; break;
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
	case budget_categories::stockpile: break;
	default:  break;
	}
	if(value == 100) {

	} else {
		switch(section_header.section_type) {
		case budget_categories::diplomatic_income:  break;
		case budget_categories::poor_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tax, true); break;
		case budget_categories::middle_tax: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tax, true); break;
		case budget_categories::rich_tax:  ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tax, true); break;
		case budget_categories::tariffs_import: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tariff, true); break;
		case budget_categories::tariffs_export: ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::max_tariff, true); break;
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
		case budget_categories::stockpile: break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::max_setting::update
	auto value = 100;
	switch(section_header.section_type) {
	case budget_categories::diplomatic_income:  break;
	case budget_categories::poor_tax: value = economy::budget_maximums(state, state.local_player_nation).poor_tax; break;
	case budget_categories::middle_tax: value = economy::budget_maximums(state, state.local_player_nation).middle_tax; break;
	case budget_categories::rich_tax: value = economy::budget_maximums(state, state.local_player_nation).rich_tax; break;
	case budget_categories::tariffs_import: value = economy::budget_maximums(state, state.local_player_nation).tariffs_import; break;
	case budget_categories::tariffs_export: value = economy::budget_maximums(state, state.local_player_nation).tariffs_export; break;
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
	case budget_categories::stockpile: break;
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
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN section_header::update
// END
	remake_layout(state, true);
}
void budgetwindow_section_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
			case layout_item_types::control:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				temp.ptr = nullptr;
				if(cname == "label") {
					temp.ptr = label.get();
				}
				if(cname == "llbutton") {
					temp.ptr = llbutton.get();
				}
				if(cname == "lbutton") {
					temp.ptr = lbutton.get();
				}
				if(cname == "rbutton") {
					temp.ptr = rbutton.get();
				}
				if(cname == "rrbutton") {
					temp.ptr = rrbutton.get();
				}
				if(cname == "setting_amount") {
					temp.ptr = setting_amount.get();
				}
				if(cname == "expand_button") {
					temp.ptr = expand_button.get();
				}
				if(cname == "total_amount") {
					temp.ptr = total_amount.get();
				}
				if(cname == "min_setting") {
					temp.ptr = min_setting.get();
				}
				if(cname == "max_setting") {
					temp.ptr = max_setting.get();
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
				if(cname == "main") {
					temp.ptr = make_budgetwindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_budgetwindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_budgetwindow_neutral_spacer(state);
				}
				if(cname == "top_spacer") {
					temp.ptr = make_budgetwindow_top_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_budgetwindow_bottom_spacer(state);
				}
				if(cname == "budget_row") {
					temp.ptr = make_budgetwindow_budget_row(state);
				}
				if(cname == "budget_header") {
					temp.ptr = make_budgetwindow_budget_header(state);
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
				children.push_back(ch_res.get());
				gui_inserts.push_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "label") {
			label = std::make_unique<budgetwindow_section_header_label_t>();
			label->parent = this;
			auto cptr = label.get();
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
		if(child_data.name == "llbutton") {
			llbutton = std::make_unique<budgetwindow_section_header_llbutton_t>();
			llbutton->parent = this;
			auto cptr = llbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "lbutton") {
			lbutton = std::make_unique<budgetwindow_section_header_lbutton_t>();
			lbutton->parent = this;
			auto cptr = lbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "rbutton") {
			rbutton = std::make_unique<budgetwindow_section_header_rbutton_t>();
			rbutton->parent = this;
			auto cptr = rbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "rrbutton") {
			rrbutton = std::make_unique<budgetwindow_section_header_rrbutton_t>();
			rrbutton->parent = this;
			auto cptr = rrbutton.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->texture_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "setting_amount") {
			setting_amount = std::make_unique<budgetwindow_section_header_setting_amount_t>();
			setting_amount->parent = this;
			auto cptr = setting_amount.get();
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
		if(child_data.name == "expand_button") {
			expand_button = std::make_unique<budgetwindow_section_header_expand_button_t>();
			expand_button->parent = this;
			auto cptr = expand_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->alt_texture_key = child_data.alt_texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "total_amount") {
			total_amount = std::make_unique<budgetwindow_section_header_total_amount_t>();
			total_amount->parent = this;
			auto cptr = total_amount.get();
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
		if(child_data.name == "min_setting") {
			min_setting = std::make_unique<budgetwindow_section_header_min_setting_t>();
			min_setting->parent = this;
			auto cptr = min_setting.get();
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
		if(child_data.name == "max_setting") {
			max_setting = std::make_unique<budgetwindow_section_header_max_setting_t>();
			max_setting->parent = this;
			auto cptr = max_setting.get();
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
// BEGIN section_header::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_section_header(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_section_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result budgetwindow_neutral_spacer_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_neutral_spacer_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_neutral_spacer_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
}
void budgetwindow_neutral_spacer_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN neutral_spacer::update
// END
}
void budgetwindow_neutral_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("budgetwindow::neutral_spacer"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	auto name_key = state.lookup_key("budgetwindow::neutral_spacer");
	for(auto ex : state.ui_defs.extensions) {
		if(name_key && ex.window == name_key) {
			auto ch_res = ui::make_element_immediate(state, ex.child);
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
				children.push_back(ch_res.get());
				gui_inserts.push_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		pending_children.pop_back();
	}
// BEGIN neutral_spacer::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_neutral_spacer(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_neutral_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result budgetwindow_top_spacer_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_top_spacer_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_top_spacer_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
}
void budgetwindow_top_spacer_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN top_spacer::update
// END
}
void budgetwindow_top_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("budgetwindow::top_spacer"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	auto name_key = state.lookup_key("budgetwindow::top_spacer");
	for(auto ex : state.ui_defs.extensions) {
		if(name_key && ex.window == name_key) {
			auto ch_res = ui::make_element_immediate(state, ex.child);
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
				children.push_back(ch_res.get());
				gui_inserts.push_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		pending_children.pop_back();
	}
// BEGIN top_spacer::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_top_spacer(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_top_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result budgetwindow_bottom_spacer_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_bottom_spacer_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_bottom_spacer_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
}
void budgetwindow_bottom_spacer_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN bottom_spacer::update
// END
}
void budgetwindow_bottom_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("budgetwindow::bottom_spacer"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	texture_key = win_data.texture;
	auto name_key = state.lookup_key("budgetwindow::bottom_spacer");
	for(auto ex : state.ui_defs.extensions) {
		if(name_key && ex.window == name_key) {
			auto ch_res = ui::make_element_immediate(state, ex.child);
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
				children.push_back(ch_res.get());
				gui_inserts.push_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		pending_children.pop_back();
	}
// BEGIN bottom_spacer::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_bottom_spacer(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_bottom_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result budgetwindow_budget_row_contents_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result budgetwindow_budget_row_contents_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void budgetwindow_budget_row_contents_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	if(x >= table_source->income_table_item_name_column_start && x < table_source->income_table_item_name_column_start + table_source->income_table_item_name_column_width) {
	}
	if(x >= table_source->income_table_item_value_column_start && x < table_source->income_table_item_value_column_start + table_source->income_table_item_value_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void budgetwindow_budget_row_contents_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	if(x >=  table_source->income_table_item_name_column_start && x <  table_source->income_table_item_name_column_start +  table_source->income_table_item_name_column_width) {
	}
	if(x >=  table_source->income_table_item_value_column_start && x <  table_source->income_table_item_value_column_start +  table_source->income_table_item_value_column_width) {
	}
}
void budgetwindow_budget_row_contents_t::set_item_name_text(sys::state & state, std::string const& new_text) {
		auto table_source = (budgetwindow_main_t*)(parent->parent);
	if(new_text !=  item_name_cached_text) {
		item_name_cached_text = new_text;
		item_name_internal_layout.contents.clear();
		item_name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->income_table_item_name_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->income_table_item_name_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_name_cached_text);
		}
	} else {
	}
}
void budgetwindow_budget_row_contents_t::set_item_value_text(sys::state & state, std::string const& new_text) {
		auto table_source = (budgetwindow_main_t*)(parent->parent);
	if(new_text !=  item_value_cached_text) {
		item_value_cached_text = new_text;
		item_value_internal_layout.contents.clear();
		item_value_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_value_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->income_table_item_value_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->income_table_item_value_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_value_cached_text);
		}
	} else {
	}
}
void budgetwindow_budget_row_contents_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	bool col_um_item_name = rel_mouse_x >= table_source->income_table_item_name_column_start && rel_mouse_x < (table_source->income_table_item_name_column_start + table_source->income_table_item_name_column_width);
	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name , false, false); 
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->income_table_item_name_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_name_text_color), cmod);
		}
	}
	bool col_um_item_value = rel_mouse_x >= table_source->income_table_item_value_column_start && rel_mouse_x < (table_source->income_table_item_value_column_start + table_source->income_table_item_value_column_width);
	if(!item_value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value , false, false); 
		for(auto& t : item_value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->income_table_item_value_column_start + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, item_value_text_color), cmod);
		}
	}
}
void budgetwindow_budget_row_contents_t::on_update(sys::state& state) noexcept {
	budgetwindow_budget_row_t& budget_row = *((budgetwindow_budget_row_t*)(parent)); 
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN budget_row::contents::update
	set_item_name_text(state, budget_row.name);
	set_item_value_text(state, text::prettify_currency(budget_row.value));
// END
}
void budgetwindow_budget_row_contents_t::on_create(sys::state& state) noexcept {
// BEGIN budget_row::contents::create
// END
}
ui::message_result budgetwindow_budget_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_budget_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_budget_row_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
	auto table_source = (budgetwindow_main_t*)(parent);
	auto under_mouse = [&](){auto p = state.ui_state.under_mouse; while(p){ if(p == this) return true; p = p->parent; } return false;}();
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), 1.000000f, 1.000000f, 1.000000f, 0.156863f);
	}
	bool col_um_item_name = rel_mouse_x >= table_source->income_table_item_name_column_start && rel_mouse_x < (table_source->income_table_item_name_column_start + table_source->income_table_item_name_column_width);
	if(col_um_item_name && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->income_table_item_name_column_start), float(y), float(table_source->income_table_item_name_column_width), float(base_data.size.y), 1.000000f, 1.000000f, 1.000000f, 0.156863f);
	}
	bool col_um_item_value = rel_mouse_x >= table_source->income_table_item_value_column_start && rel_mouse_x < (table_source->income_table_item_value_column_start + table_source->income_table_item_value_column_width);
	if(col_um_item_value && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->income_table_item_value_column_start), float(y), float(table_source->income_table_item_value_column_width), float(base_data.size.y), 1.000000f, 1.000000f, 1.000000f, 0.156863f);
	}
}
void budgetwindow_budget_row_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN budget_row::update
// END
	remake_layout(state, true);
}
void budgetwindow_budget_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "main") {
					temp.ptr = make_budgetwindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_budgetwindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_budgetwindow_neutral_spacer(state);
				}
				if(cname == "top_spacer") {
					temp.ptr = make_budgetwindow_top_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_budgetwindow_bottom_spacer(state);
				}
				if(cname == "budget_row") {
					temp.ptr = make_budgetwindow_budget_row(state);
				}
				if(cname == "budget_header") {
					temp.ptr = make_budgetwindow_budget_header(state);
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
void budgetwindow_budget_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("budgetwindow::budget_row"));
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
	auto name_key = state.lookup_key("budgetwindow::budget_row");
	for(auto ex : state.ui_defs.extensions) {
		if(name_key && ex.window == name_key) {
			auto ch_res = ui::make_element_immediate(state, ex.child);
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
				children.push_back(ch_res.get());
				gui_inserts.push_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "contents") {
			contents = std::make_unique<budgetwindow_budget_row_contents_t>();
			contents->parent = this;
			auto cptr = contents.get();
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
// BEGIN budget_row::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_budget_row(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_budget_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result budgetwindow_budget_header_contents_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	if(x >= table_source->income_table_item_name_column_start && x < table_source->income_table_item_name_column_start + table_source->income_table_item_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->income_table_item_name_sort_direction;
		table_source->income_table_item_name_sort_direction = 0;
		table_source->income_table_item_value_sort_direction = 0;
		table_source->income_table_item_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->income_table_item_value_column_start && x < table_source->income_table_item_value_column_start + table_source->income_table_item_value_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->income_table_item_value_sort_direction;
		table_source->income_table_item_name_sort_direction = 0;
		table_source->income_table_item_value_sort_direction = 0;
		table_source->income_table_item_value_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result budgetwindow_budget_header_contents_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_budget_header_contents_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	if(x >= table_source->income_table_item_name_column_start && x < table_source->income_table_item_name_column_start + table_source->income_table_item_name_column_width) {
	}
	if(x >= table_source->income_table_item_value_column_start && x < table_source->income_table_item_value_column_start + table_source->income_table_item_value_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void budgetwindow_budget_header_contents_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	if(x >=  table_source->income_table_item_name_column_start && x <  table_source->income_table_item_name_column_start +  table_source->income_table_item_name_column_width) {
	}
	if(x >=  table_source->income_table_item_value_column_start && x <  table_source->income_table_item_value_column_start +  table_source->income_table_item_value_column_width) {
	}
}
void budgetwindow_budget_header_contents_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	{
	item_name_cached_text = text::produce_simple_string(state, table_source->income_table_item_name_header_text_key);
	 item_name_internal_layout.contents.clear();
	 item_name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->income_table_item_name_column_width - 24 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->income_table_item_name_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_name_cached_text);
	}
	{
	item_value_cached_text = text::produce_simple_string(state, table_source->income_table_item_value_header_text_key);
	 item_value_internal_layout.contents.clear();
	 item_value_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_value_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->income_table_item_value_column_width - 24 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->income_table_item_value_text_alignment, text::text_color::black, true, true }, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_value_cached_text);
	}
}
void budgetwindow_budget_header_contents_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (budgetwindow_main_t*)(parent->parent);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	bool col_um_item_name = rel_mouse_x >= table_source->income_table_item_name_column_start && rel_mouse_x < (table_source->income_table_item_name_column_start + table_source->income_table_item_name_column_width);
	if(table_source->income_table_item_name_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name, false, true), float(x + table_source->income_table_item_name_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, table_source->income_table_ascending_icon, table_source->income_table_ascending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(table_source->income_table_item_name_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name, false, true), float(x + table_source->income_table_item_name_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, table_source->income_table_descending_icon, table_source->income_table_descending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(!item_name_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_name , false, true); 
		for(auto& t : item_name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->income_table_item_name_column_start + 24 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->income_table_item_name_header_text_color), cmod);
		}
	}
	bool col_um_item_value = rel_mouse_x >= table_source->income_table_item_value_column_start && rel_mouse_x < (table_source->income_table_item_value_column_start + table_source->income_table_item_value_column_width);
	if(table_source->income_table_item_value_sort_direction > 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value, false, true), float(x + table_source->income_table_item_value_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, table_source->income_table_ascending_icon, table_source->income_table_ascending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(table_source->income_table_item_value_sort_direction < 0) {
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value, false, true), float(x + table_source->income_table_item_value_column_start + 8), float(y + base_data.size.y / 2 - 8), float(16), float(16), ogl::get_late_load_texture_handle(state, table_source->income_table_descending_icon, table_source->income_table_descending_icon_key), ui::rotation::upright, false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
}
	if(!item_value_internal_layout.contents.empty() && linesz > 0.0f) {
		auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse && col_um_item_value , false, true); 
		for(auto& t : item_value_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->income_table_item_value_column_start + 24 + 8, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, table_source->income_table_item_value_header_text_color), cmod);
		}
	}
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), table_source->income_table_divider_color.r, table_source->income_table_divider_color.g, table_source->income_table_divider_color.b, 1.0f);
}
void budgetwindow_budget_header_contents_t::on_update(sys::state& state) noexcept {
	budgetwindow_budget_header_t& budget_header = *((budgetwindow_budget_header_t*)(parent)); 
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN budget_header::contents::update
// END
}
void budgetwindow_budget_header_contents_t::on_create(sys::state& state) noexcept {
// BEGIN budget_header::contents::create
// END
}
ui::message_result budgetwindow_budget_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result budgetwindow_budget_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void budgetwindow_budget_header_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state.world.locale_get_native_rtl(state.font_collection.get_current_locale())); 
	auto table_source = (budgetwindow_main_t*)(parent);
	auto under_mouse = [&](){auto p = state.ui_state.under_mouse; while(p){ if(p == this) return true; p = p->parent; } return false;}();
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - ui::get_absolute_location(state, *this).x;
	if(under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), 1.000000f, 1.000000f, 1.000000f, 0.156863f);
	}
	bool col_um_item_name = rel_mouse_x >= table_source->income_table_item_name_column_start && rel_mouse_x < (table_source->income_table_item_name_column_start + table_source->income_table_item_name_column_width);
	if(col_um_item_name && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->income_table_item_name_column_start), float(y), float(table_source->income_table_item_name_column_width), float(base_data.size.y), 1.000000f, 1.000000f, 1.000000f, 0.156863f);
	}
	bool col_um_item_value = rel_mouse_x >= table_source->income_table_item_value_column_start && rel_mouse_x < (table_source->income_table_item_value_column_start + table_source->income_table_item_value_column_width);
	if(col_um_item_value && !under_mouse) {
		ogl::render_alpha_colored_rect(state, float(x + table_source->income_table_item_value_column_start), float(y), float(table_source->income_table_item_value_column_width), float(base_data.size.y), 1.000000f, 1.000000f, 1.000000f, 0.156863f);
	}
}
void budgetwindow_budget_header_t::on_update(sys::state& state) noexcept {
	budgetwindow_main_t& main = *((budgetwindow_main_t*)(parent->parent)); 
// BEGIN budget_header::update
// END
	remake_layout(state, true);
}
void budgetwindow_budget_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "main") {
					temp.ptr = make_budgetwindow_main(state);
				}
				if(cname == "section_header") {
					temp.ptr = make_budgetwindow_section_header(state);
				}
				if(cname == "neutral_spacer") {
					temp.ptr = make_budgetwindow_neutral_spacer(state);
				}
				if(cname == "top_spacer") {
					temp.ptr = make_budgetwindow_top_spacer(state);
				}
				if(cname == "bottom_spacer") {
					temp.ptr = make_budgetwindow_bottom_spacer(state);
				}
				if(cname == "budget_row") {
					temp.ptr = make_budgetwindow_budget_row(state);
				}
				if(cname == "budget_header") {
					temp.ptr = make_budgetwindow_budget_header(state);
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
void budgetwindow_budget_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("budgetwindow::budget_header"));
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
	auto name_key = state.lookup_key("budgetwindow::budget_header");
	for(auto ex : state.ui_defs.extensions) {
		if(name_key && ex.window == name_key) {
			auto ch_res = ui::make_element_immediate(state, ex.child);
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
				children.push_back(ch_res.get());
				gui_inserts.push_back(std::move(ch_res));
			}
		}
	}
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "contents") {
			contents = std::make_unique<budgetwindow_budget_header_contents_t>();
			contents->parent = this;
			auto cptr = contents.get();
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
// BEGIN budget_header::create
// END
}
std::unique_ptr<ui::element_base> make_budgetwindow_budget_header(sys::state& state) {
	auto ptr = std::make_unique<budgetwindow_budget_header_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
