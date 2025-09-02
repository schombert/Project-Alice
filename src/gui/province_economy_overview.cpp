namespace alice_ui {
struct province_economy_overview_body_gdp_label_t;
struct province_economy_overview_body_gdp_share_label_t;
struct province_economy_overview_body_labor_label_t;
struct province_economy_overview_body_artisans_label_t;
struct province_economy_overview_body_prices_label_t;
struct province_economy_overview_body_prices_details_t;
struct province_economy_overview_body_margin_label_t;
struct province_economy_overview_body_payback_time_label_t;
struct province_economy_overview_body_trade_label_t;
struct province_economy_overview_body_trade_details_t;
struct province_economy_overview_body_import_value_icon_t;
struct province_economy_overview_body_export_value_icon_t;
struct province_economy_overview_body_import_structure_icon_t;
struct province_economy_overview_body_export_structure_icon_t;
struct province_economy_overview_body_import_value_graph_t;
struct province_economy_overview_body_export_value_graph_t;
struct province_economy_overview_body_import_structure_graph_t;
struct province_economy_overview_body_export_structure_graph_t;
struct province_economy_overview_body_rgo_label_t;
struct province_economy_overview_body_rgo_details_t;
struct province_economy_overview_body_rgo_employment_label_t;
struct province_economy_overview_body_rgo_profit_label_t;
struct province_economy_overview_body_bar_chart_overlay_1_t;
struct province_economy_overview_body_bar_chart_overlay_2_t;
struct province_economy_overview_body_bar_chart_overlay_3_t;
struct province_economy_overview_body_bar_chart_overlay_4_t;
struct province_economy_overview_body_per_nation_label_t;
struct province_economy_overview_body_per_commodity_label_t;
struct province_economy_overview_body_gdp_chart_t;
struct province_economy_overview_body_gdp_share_chart_t;
struct province_economy_overview_body_labor_chart_t;
struct province_economy_overview_body_artisan_chart_t;
struct province_economy_overview_body_t;
struct province_economy_overview_rgo_employment_name_t;
struct province_economy_overview_rgo_employment_employment_t;
struct province_economy_overview_rgo_employment_t;
struct province_economy_overview_rgo_profit_name_t;
struct province_economy_overview_rgo_profit_profit_t;
struct province_economy_overview_rgo_profit_t;
struct province_economy_overview_price_entry_name_t;
struct province_economy_overview_price_entry_local_price_t;
struct province_economy_overview_price_entry_ratio_t;
struct province_economy_overview_price_entry_t;
struct province_economy_overview_method_margin_name_t;
struct province_economy_overview_method_margin_margin_t;
struct province_economy_overview_method_margin_t;
struct province_economy_overview_method_payback_name_t;
struct province_economy_overview_method_payback_days_t;
struct province_economy_overview_method_payback_t;
struct province_economy_overview_body_gdp_label_t : public ui::element_base {
// BEGIN body::gdp_label::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
	std::string cached_text;
	dcon::text_key text_key;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	dcon::text_key tooltip_key;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::tooltip;
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
struct province_economy_overview_body_gdp_share_label_t : public ui::element_base {
// BEGIN body::gdp_share_label::variables
// END
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
struct province_economy_overview_body_labor_label_t : public ui::element_base {
// BEGIN body::labor_label::variables
// END
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
struct province_economy_overview_body_artisans_label_t : public ui::element_base {
// BEGIN body::artisans_label::variables
// END
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
struct province_economy_overview_body_prices_label_t : public ui::element_base {
// BEGIN body::prices_label::variables
// END
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
struct province_economy_overview_body_prices_details_t : public ui::element_base {
// BEGIN body::prices_details::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct province_economy_overview_body_margin_label_t : public ui::element_base {
// BEGIN body::margin_label::variables
// END
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
struct province_economy_overview_body_payback_time_label_t : public ui::element_base {
// BEGIN body::payback_time_label::variables
// END
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
struct province_economy_overview_body_trade_label_t : public ui::element_base {
// BEGIN body::trade_label::variables
// END
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
struct province_economy_overview_body_trade_details_t : public ui::element_base {
// BEGIN body::trade_details::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct province_economy_overview_body_import_value_icon_t : public ui::element_base {
// BEGIN body::import_value_icon::variables
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
struct province_economy_overview_body_export_value_icon_t : public ui::element_base {
// BEGIN body::export_value_icon::variables
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
struct province_economy_overview_body_import_structure_icon_t : public ui::element_base {
// BEGIN body::import_structure_icon::variables
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
struct province_economy_overview_body_export_structure_icon_t : public ui::element_base {
// BEGIN body::export_structure_icon::variables
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
struct province_economy_overview_body_import_value_graph_t : public ui::element_base {
// BEGIN body::import_value_graph::variables
	std::vector<float> per_nation_data{};
	struct entry_before_merge {
		dcon::nation_id key;
		float value;
	};
	std::vector<entry_before_merge> raw_content{ };
// END
	ogl::data_texture data_texture{ 300, 3 };
	struct graph_entry {dcon::nation_id key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_export_value_graph_t : public ui::element_base {
// BEGIN body::export_value_graph::variables
	std::vector<float> per_nation_data{};
	struct entry_before_merge {
		dcon::nation_id key;
		float value;
	};
	std::vector<entry_before_merge> raw_content{ };
// END
	ogl::data_texture data_texture{ 300, 3 };
	struct graph_entry {dcon::nation_id key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_import_structure_graph_t : public ui::element_base {
// BEGIN body::import_structure_graph::variables
	std::vector<float> per_commodity_data{};
	struct entry_before_merge {
		dcon::commodity_id key;
		float value;
	};
	std::vector<entry_before_merge> raw_content{ };
// END
	ogl::data_texture data_texture{ 300, 3 };
	struct graph_entry {dcon::commodity_id key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_export_structure_graph_t : public ui::element_base {
// BEGIN body::export_structure_graph::variables
	std::vector<float> per_commodity_data{};
	struct entry_before_merge {
		dcon::commodity_id key;
		float value;
	};
	std::vector<entry_before_merge> raw_content{ };
// END
	ogl::data_texture data_texture{ 300, 3 };
	struct graph_entry {dcon::commodity_id key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_rgo_label_t : public ui::element_base {
// BEGIN body::rgo_label::variables
// END
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
struct province_economy_overview_body_rgo_details_t : public ui::element_base {
// BEGIN body::rgo_details::variables
// END
	std::string_view texture_key;
	dcon::texture_id background_texture;
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
struct province_economy_overview_body_rgo_employment_label_t : public ui::element_base {
// BEGIN body::rgo_employment_label::variables
// END
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
struct province_economy_overview_body_rgo_profit_label_t : public ui::element_base {
// BEGIN body::rgo_profit_label::variables
// END
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
struct province_economy_overview_body_bar_chart_overlay_1_t : public ui::element_base {
// BEGIN body::bar_chart_overlay_1::variables
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
struct province_economy_overview_body_bar_chart_overlay_2_t : public ui::element_base {
// BEGIN body::bar_chart_overlay_2::variables
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
struct province_economy_overview_body_bar_chart_overlay_3_t : public ui::element_base {
// BEGIN body::bar_chart_overlay_3::variables
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
struct province_economy_overview_body_bar_chart_overlay_4_t : public ui::element_base {
// BEGIN body::bar_chart_overlay_4::variables
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
struct province_economy_overview_body_per_nation_label_t : public ui::element_base {
// BEGIN body::per_nation_label::variables
// END
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
struct province_economy_overview_body_per_commodity_label_t : public ui::element_base {
// BEGIN body::per_commodity_label::variables
// END
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
struct province_economy_overview_body_gdp_chart_t : public ui::element_base {
// BEGIN body::gdp_chart::variables
// END
	ogl::generic_ui_mesh_triangle_strip mesh{ 200 };
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {int8_t key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_gdp_share_chart_t : public ui::element_base {
// BEGIN body::gdp_share_chart::variables
// END
	ogl::generic_ui_mesh_triangle_strip mesh{ 200 };
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {int8_t key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_labor_chart_t : public ui::element_base {
// BEGIN body::labor_chart::variables
// END
	ogl::generic_ui_mesh_triangle_strip mesh{ 200 };
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {int32_t key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_artisan_chart_t : public ui::element_base {
// BEGIN body::artisan_chart::variables
// END
	ogl::generic_ui_mesh_triangle_strip mesh{ 200 };
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {dcon::commodity_id key; ogl::color3f color; float amount; };
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
struct province_economy_overview_body_high_prices_t : public layout_generator {
// BEGIN body::high_prices::variables
	std::vector<dcon::commodity_id> base_data;
// END
	struct price_entry_option { dcon::commodity_id commodity; };
	std::vector<std::unique_ptr<ui::element_base>> price_entry_pool;
	int32_t price_entry_pool_used = 0;
	void add_price_entry( dcon::commodity_id commodity);
	std::vector<std::variant<std::monostate, price_entry_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct province_economy_overview_body_low_prices_t : public layout_generator {
// BEGIN body::low_prices::variables
	std::vector<dcon::commodity_id> base_data;
// END
	struct price_entry_option { dcon::commodity_id commodity; };
	std::vector<std::unique_ptr<ui::element_base>> price_entry_pool;
	int32_t price_entry_pool_used = 0;
	void add_price_entry( dcon::commodity_id commodity);
	std::vector<std::variant<std::monostate, price_entry_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct province_economy_overview_body_margin_t : public layout_generator {
// BEGIN body::margin::variables
	std::vector<dcon::factory_type_id> base_data;
// END
	struct method_margin_option { dcon::factory_type_id top_factory; };
	std::vector<std::unique_ptr<ui::element_base>> method_margin_pool;
	int32_t method_margin_pool_used = 0;
	void add_method_margin( dcon::factory_type_id top_factory);
	std::vector<std::variant<std::monostate, method_margin_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct province_economy_overview_body_rgo_employment_t : public layout_generator {
// BEGIN body::rgo_employment::variables
	std::vector<dcon::commodity_id> top_commodities{};
// END
	struct rgo_employment_option { dcon::commodity_id top_commodity; };
	std::vector<std::unique_ptr<ui::element_base>> rgo_employment_pool;
	int32_t rgo_employment_pool_used = 0;
	void add_rgo_employment( dcon::commodity_id top_commodity);
	std::vector<std::variant<std::monostate, rgo_employment_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct province_economy_overview_body_rgo_profit_t : public layout_generator {
// BEGIN body::rgo_profit::variables
	std::vector<dcon::commodity_id> top_commodities{};
// END
	struct rgo_profit_option { dcon::commodity_id top_commodity; };
	std::vector<std::unique_ptr<ui::element_base>> rgo_profit_pool;
	int32_t rgo_profit_pool_used = 0;
	void add_rgo_profit( dcon::commodity_id top_commodity);
	std::vector<std::variant<std::monostate, rgo_profit_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct province_economy_overview_rgo_employment_name_t : public ui::element_base {
// BEGIN rgo_employment::name::variables
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
struct province_economy_overview_rgo_employment_employment_t : public ui::element_base {
// BEGIN rgo_employment::employment::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
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
struct province_economy_overview_rgo_profit_name_t : public ui::element_base {
// BEGIN rgo_profit::name::variables
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
struct province_economy_overview_rgo_profit_profit_t : public ui::element_base {
// BEGIN rgo_profit::profit::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
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
struct province_economy_overview_price_entry_name_t : public ui::element_base {
// BEGIN price_entry::name::variables
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
struct province_economy_overview_price_entry_local_price_t : public ui::element_base {
// BEGIN price_entry::local_price::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	dcon::text_key tooltip_key;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::tooltip;
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
struct province_economy_overview_price_entry_ratio_t : public ui::element_base {
// BEGIN price_entry::ratio::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::right;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	dcon::text_key tooltip_key;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::tooltip;
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
struct province_economy_overview_method_margin_name_t : public ui::element_base {
// BEGIN method_margin::name::variables
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
struct province_economy_overview_method_margin_margin_t : public ui::element_base {
// BEGIN method_margin::margin::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
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
struct province_economy_overview_method_payback_name_t : public ui::element_base {
// BEGIN method_payback::name::variables
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
struct province_economy_overview_method_payback_days_t : public ui::element_base {
// BEGIN method_payback::days::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
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
struct province_economy_overview_body_t : public layout_window_element {
// BEGIN body::variables
	ui::element_base * trade_details_window;
	ui::element_base * rgo_details_window;
	ui::element_base * price_details_window;
// END
	std::unique_ptr<province_economy_overview_body_gdp_label_t> gdp_label;
	std::unique_ptr<province_economy_overview_body_gdp_share_label_t> gdp_share_label;
	std::unique_ptr<province_economy_overview_body_labor_label_t> labor_label;
	std::unique_ptr<province_economy_overview_body_artisans_label_t> artisans_label;
	std::unique_ptr<province_economy_overview_body_prices_label_t> prices_label;
	std::unique_ptr<province_economy_overview_body_prices_details_t> prices_details;
	std::unique_ptr<province_economy_overview_body_margin_label_t> margin_label;
	std::unique_ptr<province_economy_overview_body_payback_time_label_t> payback_time_label;
	std::unique_ptr<province_economy_overview_body_trade_label_t> trade_label;
	std::unique_ptr<province_economy_overview_body_trade_details_t> trade_details;
	std::unique_ptr<province_economy_overview_body_import_value_icon_t> import_value_icon;
	std::unique_ptr<province_economy_overview_body_export_value_icon_t> export_value_icon;
	std::unique_ptr<province_economy_overview_body_import_structure_icon_t> import_structure_icon;
	std::unique_ptr<province_economy_overview_body_export_structure_icon_t> export_structure_icon;
	std::unique_ptr<province_economy_overview_body_import_value_graph_t> import_value_graph;
	std::unique_ptr<province_economy_overview_body_export_value_graph_t> export_value_graph;
	std::unique_ptr<province_economy_overview_body_import_structure_graph_t> import_structure_graph;
	std::unique_ptr<province_economy_overview_body_export_structure_graph_t> export_structure_graph;
	std::unique_ptr<province_economy_overview_body_rgo_label_t> rgo_label;
	std::unique_ptr<province_economy_overview_body_rgo_details_t> rgo_details;
	std::unique_ptr<province_economy_overview_body_rgo_employment_label_t> rgo_employment_label;
	std::unique_ptr<province_economy_overview_body_rgo_profit_label_t> rgo_profit_label;
	std::unique_ptr<province_economy_overview_body_bar_chart_overlay_1_t> bar_chart_overlay_1;
	std::unique_ptr<province_economy_overview_body_bar_chart_overlay_2_t> bar_chart_overlay_2;
	std::unique_ptr<province_economy_overview_body_bar_chart_overlay_3_t> bar_chart_overlay_3;
	std::unique_ptr<province_economy_overview_body_bar_chart_overlay_4_t> bar_chart_overlay_4;
	std::unique_ptr<province_economy_overview_body_per_nation_label_t> per_nation_label;
	std::unique_ptr<province_economy_overview_body_per_commodity_label_t> per_commodity_label;
	std::unique_ptr<province_economy_overview_body_gdp_chart_t> gdp_chart;
	std::unique_ptr<province_economy_overview_body_gdp_share_chart_t> gdp_share_chart;
	std::unique_ptr<province_economy_overview_body_labor_chart_t> labor_chart;
	std::unique_ptr<province_economy_overview_body_artisan_chart_t> artisan_chart;
	province_economy_overview_body_high_prices_t high_prices;
	province_economy_overview_body_low_prices_t low_prices;
	province_economy_overview_body_margin_t margin;
	province_economy_overview_body_rgo_employment_t rgo_employment;
	province_economy_overview_body_rgo_profit_t rgo_profit;
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
std::unique_ptr<ui::element_base> make_province_economy_overview_body(sys::state& state);
struct province_economy_overview_rgo_employment_t : public layout_window_element {
// BEGIN rgo_employment::variables
// END
	dcon::commodity_id top_commodity;
	std::unique_ptr<province_economy_overview_rgo_employment_name_t> name;
	std::unique_ptr<province_economy_overview_rgo_employment_employment_t> employment;
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
		if(name_parameter == "top_commodity") {
			return (void*)(&top_commodity);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_province_economy_overview_rgo_employment(sys::state& state);
struct province_economy_overview_rgo_profit_t : public layout_window_element {
// BEGIN rgo_profit::variables
// END
	dcon::commodity_id top_commodity;
	std::unique_ptr<province_economy_overview_rgo_profit_name_t> name;
	std::unique_ptr<province_economy_overview_rgo_profit_profit_t> profit;
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
		if(name_parameter == "top_commodity") {
			return (void*)(&top_commodity);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_province_economy_overview_rgo_profit(sys::state& state);
struct province_economy_overview_price_entry_t : public layout_window_element {
// BEGIN price_entry::variables
// END
	dcon::commodity_id commodity;
	std::unique_ptr<province_economy_overview_price_entry_name_t> name;
	std::unique_ptr<province_economy_overview_price_entry_local_price_t> local_price;
	std::unique_ptr<province_economy_overview_price_entry_ratio_t> ratio;
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
		if(name_parameter == "commodity") {
			return (void*)(&commodity);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_province_economy_overview_price_entry(sys::state& state);
struct province_economy_overview_method_margin_t : public layout_window_element {
// BEGIN method_margin::variables
// END
	dcon::factory_type_id top_factory;
	std::unique_ptr<province_economy_overview_method_margin_name_t> name;
	std::unique_ptr<province_economy_overview_method_margin_margin_t> margin;
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
		if(name_parameter == "top_factory") {
			return (void*)(&top_factory);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_province_economy_overview_method_margin(sys::state& state);
struct province_economy_overview_method_payback_t : public layout_window_element {
// BEGIN method_payback::variables
// END
	dcon::factory_type_id top_factory;
	std::unique_ptr<province_economy_overview_method_payback_name_t> name;
	std::unique_ptr<province_economy_overview_method_payback_days_t> days;
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
		if(name_parameter == "top_factory") {
			return (void*)(&top_factory);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_province_economy_overview_method_payback(sys::state& state);
void province_economy_overview_body_high_prices_t::add_price_entry(dcon::commodity_id commodity) {
	values.emplace_back(price_entry_option{commodity});
}
void  province_economy_overview_body_high_prices_t::on_create(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::high_prices::on_create
	base_data.clear();
	state.world.for_each_commodity([&](auto a) {
		base_data.push_back(a);
	});
// END
}
void  province_economy_overview_body_high_prices_t::update(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::high_prices::update
	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	std::sort(base_data.begin(), base_data.end(), [&](auto a, auto b) {
		auto value_a = economy::price(state, mid, a) / state.world.commodity_get_median_price(a);
		auto value_b = economy::price(state, mid, b) / state.world.commodity_get_median_price(b);
		if(value_a == value_b) {
			return a.value > b.value;
		}
		return value_a > value_b;
	});

	values.clear();
	for(size_t i = 0; i < std::min((size_t)4, base_data.size()); i++) {
		add_price_entry(base_data[i]);
	}
// END
}
measure_result  province_economy_overview_body_high_prices_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<price_entry_option>(values[index])) {
		if(price_entry_pool.empty()) price_entry_pool.emplace_back(make_province_economy_overview_price_entry(state));
		if(destination) {
			if(price_entry_pool.size() <= size_t(price_entry_pool_used)) price_entry_pool.emplace_back(make_province_economy_overview_price_entry(state));
			price_entry_pool[price_entry_pool_used]->base_data.position.x = int16_t(x);
			price_entry_pool[price_entry_pool_used]->base_data.position.y = int16_t(y);
			price_entry_pool[price_entry_pool_used]->parent = destination;
			destination->children.push_back(price_entry_pool[price_entry_pool_used].get());
			((province_economy_overview_price_entry_t*)(price_entry_pool[price_entry_pool_used].get()))->commodity = std::get<price_entry_option>(values[index]).commodity;
			((province_economy_overview_price_entry_t*)(price_entry_pool[price_entry_pool_used].get()))->is_active = alternate;
			price_entry_pool[price_entry_pool_used]->impl_on_update(state);
			price_entry_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ price_entry_pool[0]->base_data.size.x, price_entry_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  province_economy_overview_body_high_prices_t::reset_pools() {
	price_entry_pool_used = 0;
}
void province_economy_overview_body_low_prices_t::add_price_entry(dcon::commodity_id commodity) {
	values.emplace_back(price_entry_option{commodity});
}
void  province_economy_overview_body_low_prices_t::on_create(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::low_prices::on_create
	base_data.clear();
	state.world.for_each_commodity([&](auto a) {
		base_data.push_back(a);
	});
// END
}
void  province_economy_overview_body_low_prices_t::update(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::low_prices::update
	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	std::sort(base_data.begin(), base_data.end(), [&](auto a, auto b) {
		auto value_a = economy::price(state, mid, a) / state.world.commodity_get_median_price(a);
		auto value_b = economy::price(state, mid, b) / state.world.commodity_get_median_price(b);
		if(value_a == value_b) {
			return a.value > b.value;
		}
		return value_a > value_b;
	});

	values.clear();
	for(size_t i = base_data.size() - std::min((size_t)4, base_data.size()); i < base_data.size(); i++) {
		add_price_entry(base_data[i]);
	}
// END
}
measure_result  province_economy_overview_body_low_prices_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<price_entry_option>(values[index])) {
		if(price_entry_pool.empty()) price_entry_pool.emplace_back(make_province_economy_overview_price_entry(state));
		if(destination) {
			if(price_entry_pool.size() <= size_t(price_entry_pool_used)) price_entry_pool.emplace_back(make_province_economy_overview_price_entry(state));
			price_entry_pool[price_entry_pool_used]->base_data.position.x = int16_t(x);
			price_entry_pool[price_entry_pool_used]->base_data.position.y = int16_t(y);
			price_entry_pool[price_entry_pool_used]->parent = destination;
			destination->children.push_back(price_entry_pool[price_entry_pool_used].get());
			((province_economy_overview_price_entry_t*)(price_entry_pool[price_entry_pool_used].get()))->commodity = std::get<price_entry_option>(values[index]).commodity;
			((province_economy_overview_price_entry_t*)(price_entry_pool[price_entry_pool_used].get()))->is_active = alternate;
			price_entry_pool[price_entry_pool_used]->impl_on_update(state);
			price_entry_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ price_entry_pool[0]->base_data.size.x, price_entry_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  province_economy_overview_body_low_prices_t::reset_pools() {
	price_entry_pool_used = 0;
}
void province_economy_overview_body_margin_t::add_method_margin(dcon::factory_type_id top_factory) {
	values.emplace_back(method_margin_option{top_factory});
}
void  province_economy_overview_body_margin_t::on_create(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::margin::on_create
	base_data.clear();
	state.world.for_each_factory_type([&](auto a) {
		base_data.push_back(a);
	});
// END
}
void  province_economy_overview_body_margin_t::update(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::margin::update
	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto nid = state.world.province_get_nation_from_province_ownership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	std::sort(base_data.begin(), base_data.end(), [&](auto a, auto b) {
		auto value_a = economy::estimate_factory_profit_margin(state, pid, a);
		auto value_b = economy::estimate_factory_profit_margin(state, pid, b);

		if(value_a == value_b) {
			return a.value > b.value;
		}
		return value_a > value_b;
	});

	values.clear();
	for(size_t i = 0; i < std::min((size_t)6, base_data.size()); i++) {
		add_method_margin(base_data[i]);
	}
// END
}
measure_result  province_economy_overview_body_margin_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<method_margin_option>(values[index])) {
		if(method_margin_pool.empty()) method_margin_pool.emplace_back(make_province_economy_overview_method_margin(state));
		if(destination) {
			if(method_margin_pool.size() <= size_t(method_margin_pool_used)) method_margin_pool.emplace_back(make_province_economy_overview_method_margin(state));
			method_margin_pool[method_margin_pool_used]->base_data.position.x = int16_t(x);
			method_margin_pool[method_margin_pool_used]->base_data.position.y = int16_t(y);
			method_margin_pool[method_margin_pool_used]->parent = destination;
			destination->children.push_back(method_margin_pool[method_margin_pool_used].get());
			((province_economy_overview_method_margin_t*)(method_margin_pool[method_margin_pool_used].get()))->top_factory = std::get<method_margin_option>(values[index]).top_factory;
			((province_economy_overview_method_margin_t*)(method_margin_pool[method_margin_pool_used].get()))->is_active = alternate;
			method_margin_pool[method_margin_pool_used]->impl_on_update(state);
			method_margin_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ method_margin_pool[0]->base_data.size.x, method_margin_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  province_economy_overview_body_margin_t::reset_pools() {
	method_margin_pool_used = 0;
}
void province_economy_overview_body_rgo_employment_t::add_rgo_employment(dcon::commodity_id top_commodity) {
	values.emplace_back(rgo_employment_option{top_commodity});
}
void  province_economy_overview_body_rgo_employment_t::on_create(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_employment::on_create
	top_commodities.clear();
	state.world.for_each_commodity([&](auto cid) {
		top_commodities.push_back(cid);
	});
// END
}
void  province_economy_overview_body_rgo_employment_t::update(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_employment::update
	// display top employment rgos
	auto pid = state.map_state.selected_province;
	std::sort(top_commodities.begin(), top_commodities.end(), [&](auto a, auto b) {
		auto value_a = economy::rgo_employment(state, a, pid);
		auto value_b = economy::rgo_employment(state, b, pid);
		if(value_a == value_b) {
			return a.value > b.value;
		}

		return value_a > value_b;
	});
	values.clear();
	for(size_t i = 0; i < std::min((size_t)4, top_commodities.size()); i++) {
		if(economy::rgo_employment(state, top_commodities[i], pid) > 0.f) {
			add_rgo_employment(top_commodities[i]);
		}
	}
// END
}
measure_result  province_economy_overview_body_rgo_employment_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<rgo_employment_option>(values[index])) {
		if(rgo_employment_pool.empty()) rgo_employment_pool.emplace_back(make_province_economy_overview_rgo_employment(state));
		if(destination) {
			if(rgo_employment_pool.size() <= size_t(rgo_employment_pool_used)) rgo_employment_pool.emplace_back(make_province_economy_overview_rgo_employment(state));
			rgo_employment_pool[rgo_employment_pool_used]->base_data.position.x = int16_t(x);
			rgo_employment_pool[rgo_employment_pool_used]->base_data.position.y = int16_t(y);
			rgo_employment_pool[rgo_employment_pool_used]->parent = destination;
			destination->children.push_back(rgo_employment_pool[rgo_employment_pool_used].get());
			((province_economy_overview_rgo_employment_t*)(rgo_employment_pool[rgo_employment_pool_used].get()))->top_commodity = std::get<rgo_employment_option>(values[index]).top_commodity;
			((province_economy_overview_rgo_employment_t*)(rgo_employment_pool[rgo_employment_pool_used].get()))->is_active = alternate;
			rgo_employment_pool[rgo_employment_pool_used]->impl_on_update(state);
			rgo_employment_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ rgo_employment_pool[0]->base_data.size.x, rgo_employment_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  province_economy_overview_body_rgo_employment_t::reset_pools() {
	rgo_employment_pool_used = 0;
}
void province_economy_overview_body_rgo_profit_t::add_rgo_profit(dcon::commodity_id top_commodity) {
	values.emplace_back(rgo_profit_option{top_commodity});
}
void  province_economy_overview_body_rgo_profit_t::on_create(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_profit::on_create
	top_commodities.clear();
	state.world.for_each_commodity([&](auto cid) {
		top_commodities.push_back(cid);
	});
// END
}
void  province_economy_overview_body_rgo_profit_t::update(sys::state& state, layout_window_element* parent) {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_profit::update
	auto pid = state.map_state.selected_province;
	std::sort(top_commodities.begin(), top_commodities.end(), [&](auto a, auto b) {
		auto value_a = economy::rgo_income(state, a, pid);
		auto value_b = economy::rgo_income(state, b, pid);
		if(value_a == value_b) {
			return a.value > b.value;
		}

		return value_a > value_b;
	});
	values.clear();
	for(size_t i = 0; i < std::min((size_t)4, top_commodities.size()); i++) {
		if(economy::rgo_income(state, top_commodities[i], pid) > 0.f) {
			add_rgo_profit(top_commodities[i]);
		}
	}
// END
}
measure_result  province_economy_overview_body_rgo_profit_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<rgo_profit_option>(values[index])) {
		if(rgo_profit_pool.empty()) rgo_profit_pool.emplace_back(make_province_economy_overview_rgo_profit(state));
		if(destination) {
			if(rgo_profit_pool.size() <= size_t(rgo_profit_pool_used)) rgo_profit_pool.emplace_back(make_province_economy_overview_rgo_profit(state));
			rgo_profit_pool[rgo_profit_pool_used]->base_data.position.x = int16_t(x);
			rgo_profit_pool[rgo_profit_pool_used]->base_data.position.y = int16_t(y);
			rgo_profit_pool[rgo_profit_pool_used]->parent = destination;
			destination->children.push_back(rgo_profit_pool[rgo_profit_pool_used].get());
			((province_economy_overview_rgo_profit_t*)(rgo_profit_pool[rgo_profit_pool_used].get()))->top_commodity = std::get<rgo_profit_option>(values[index]).top_commodity;
			((province_economy_overview_rgo_profit_t*)(rgo_profit_pool[rgo_profit_pool_used].get()))->is_active = alternate;
			rgo_profit_pool[rgo_profit_pool_used]->impl_on_update(state);
			rgo_profit_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ rgo_profit_pool[0]->base_data.size.x, rgo_profit_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  province_economy_overview_body_rgo_profit_t::reset_pools() {
	rgo_profit_pool_used = 0;
}
void province_economy_overview_body_gdp_label_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void province_economy_overview_body_gdp_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_gdp_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_gdp_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_gdp_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::gdp_label::update
// END
}
void province_economy_overview_body_gdp_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::gdp_label::create
// END
}
void province_economy_overview_body_gdp_share_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_gdp_share_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_gdp_share_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_gdp_share_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::gdp_share_label::update
// END
}
void province_economy_overview_body_gdp_share_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::gdp_share_label::create
// END
}
void province_economy_overview_body_labor_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_labor_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_labor_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_labor_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::labor_label::update
// END
}
void province_economy_overview_body_labor_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::labor_label::create
// END
}
void province_economy_overview_body_artisans_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_artisans_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_artisans_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_artisans_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::artisans_label::update
// END
}
void province_economy_overview_body_artisans_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::artisans_label::create
// END
}
void province_economy_overview_body_prices_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_prices_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_prices_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_prices_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::prices_label::update
// END
}
void province_economy_overview_body_prices_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::prices_label::create
// END
}
ui::message_result province_economy_overview_body_prices_details_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN body::prices_details::lbutton_action
	auto target_is_visible = body.price_details_window->is_visible();
	body.price_details_window->set_visible(state, !target_is_visible);
	body.price_details_window->parent->move_child_to_front(body.price_details_window);
// END
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_body_prices_details_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_body_prices_details_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_prices_details_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_prices_details_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_prices_details_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::prices_details::update
// END
}
void province_economy_overview_body_prices_details_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::prices_details::create
// END
}
void province_economy_overview_body_margin_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_margin_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_margin_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_margin_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::margin_label::update
// END
}
void province_economy_overview_body_margin_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::margin_label::create
// END
}
void province_economy_overview_body_payback_time_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_payback_time_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_payback_time_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_payback_time_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::payback_time_label::update
// END
}
void province_economy_overview_body_payback_time_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::payback_time_label::create
// END
}
void province_economy_overview_body_trade_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_trade_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_trade_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_trade_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::trade_label::update
// END
}
void province_economy_overview_body_trade_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::trade_label::create
// END
}
ui::message_result province_economy_overview_body_trade_details_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN body::trade_details::lbutton_action
	auto target_is_visible = body.trade_details_window->is_visible();
	body.trade_details_window->set_visible(state, !target_is_visible);
	body.trade_details_window->parent->move_child_to_front(body.trade_details_window);
// END
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_body_trade_details_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_body_trade_details_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_trade_details_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_trade_details_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_trade_details_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::trade_details::update
// END
}
void province_economy_overview_body_trade_details_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::trade_details::create
// END
}
ui::message_result province_economy_overview_body_import_value_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_import_value_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_import_value_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_import_value_icon_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::import_value_icon::update
// END
}
void province_economy_overview_body_import_value_icon_t::on_create(sys::state& state) noexcept {
// BEGIN body::import_value_icon::create
// END
}
ui::message_result province_economy_overview_body_export_value_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_export_value_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_export_value_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_export_value_icon_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::export_value_icon::update
// END
}
void province_economy_overview_body_export_value_icon_t::on_create(sys::state& state) noexcept {
// BEGIN body::export_value_icon::create
// END
}
ui::message_result province_economy_overview_body_import_structure_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_import_structure_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_import_structure_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_import_structure_icon_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::import_structure_icon::update
// END
}
void province_economy_overview_body_import_structure_icon_t::on_create(sys::state& state) noexcept {
// BEGIN body::import_structure_icon::create
// END
}
ui::message_result province_economy_overview_body_export_structure_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_export_structure_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_export_structure_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_export_structure_icon_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::export_structure_icon::update
// END
}
void province_economy_overview_body_export_structure_icon_t::on_create(sys::state& state) noexcept {
// BEGIN body::export_structure_icon::create
// END
}
ui::message_result province_economy_overview_body_import_value_graph_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_import_value_graph_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_import_value_graph_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 300; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 300; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(300)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_import_value_graph_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::import_value_graph::tooltip
		auto pid = state.map_state.selected_province;
		auto sid = state.world.province_get_state_membership(pid);
		auto mid = state.world.state_instance_get_market_from_local_market(sid);

		if(selected_key) {
			{
				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, selected_key);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
				text::close_layout_box(contents, box);
			}

			state.world.for_each_market([&](auto other) {
				auto flow = economy::trade_value_flow(state, other, mid);
				auto other_sid = state.world.market_get_zone_from_local_market(other);
				auto other_nation = state.world.state_instance_get_nation_from_state_ownership(other_sid);
				auto capital = state.world.state_instance_get_capital(other_sid);

				if(other_nation != selected_key) return;
				if(flow < 0.5f) return;

				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, capital);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::fp_currency{ flow });
				text::close_layout_box(contents, box);
			});
		} else {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
			text::close_layout_box(contents, box);
		}
// END
	}
}
void province_economy_overview_body_import_value_graph_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void province_economy_overview_body_import_value_graph_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::import_value_graph::update
	per_nation_data.clear();
	per_nation_data.resize(state.world.nation_size());

	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	float total_left = 0.f;

	state.world.for_each_market([&](auto origin) {
		auto flow = economy::trade_value_flow(state, origin, mid);
		auto origin_sid = state.world.market_get_zone_from_local_market(origin);
		auto origin_nation = state.world.state_instance_get_nation_from_state_ownership(origin_sid);

		per_nation_data[origin_nation.index()] += flow;
		total_left += flow;
	});

	raw_content.clear();

	state.world.for_each_nation([&](auto origin) {
		raw_content.emplace_back(origin, per_nation_data[origin.index()]);
	});

	std::sort(raw_content.begin(), raw_content.end(), [&](auto a, auto b) {
		if(a.value == b.value) {
			return a.key.value > b.key.value;
		}
		return a.value > b.value;
	});

	// prepare data
	graph_content.clear();
	graph_content.resize(15);

	for(int32_t k = 0; k < 14; k++) {
		graph_content[k].key = raw_content[k].key;
		graph_content[k].amount = raw_content[k].value;

		auto c = state.world.nation_get_color(raw_content[k].key);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[k].color = c3f;

		total_left -= raw_content[k].value;
	}

	graph_content[14].color.r = 0.f;
	graph_content[14].color.g = 0.f;
	graph_content[14].color.b = 0.f;
	graph_content[14].key = { };
	graph_content[14].amount = total_left;

	update_chart(state);

// END
}
void province_economy_overview_body_import_value_graph_t::on_create(sys::state& state) noexcept {
// BEGIN body::import_value_graph::create
// END
}
ui::message_result province_economy_overview_body_export_value_graph_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_export_value_graph_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_export_value_graph_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 300; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 300; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(300)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_export_value_graph_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::export_value_graph::tooltip
		auto pid = state.map_state.selected_province;
		auto sid = state.world.province_get_state_membership(pid);
		auto mid = state.world.state_instance_get_market_from_local_market(sid);

		if(selected_key) {
			{
				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, selected_key);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
				text::close_layout_box(contents, box);
			}

			state.world.for_each_market([&](auto other) {
				auto flow = economy::trade_value_flow(state, mid, other);
				auto other_sid = state.world.market_get_zone_from_local_market(other);
				auto other_nation = state.world.state_instance_get_nation_from_state_ownership(other_sid);
				auto capital = state.world.state_instance_get_capital(other_sid);

				if(other_nation != selected_key) return;
				if(flow < 0.5f) return;

				auto box = text::open_layout_box(contents);
				text::add_to_layout_box(state, contents, box, capital);
				text::add_space_to_layout_box(state, contents, box);
				text::add_to_layout_box(state, contents, box, text::fp_currency{ flow });
				text::close_layout_box(contents, box);
			});
		} else {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
			text::close_layout_box(contents, box);
		}
// END
	}
}
void province_economy_overview_body_export_value_graph_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void province_economy_overview_body_export_value_graph_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::export_value_graph::update
    per_nation_data.clear();
	per_nation_data.resize(state.world.nation_size());

	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	float total_left = 0.f;

	state.world.for_each_market([&](auto target) {
		auto flow = economy::trade_value_flow(state, mid, target);
		auto target_sid = state.world.market_get_zone_from_local_market(target);
		auto target_nation = state.world.state_instance_get_nation_from_state_ownership(target_sid);

		per_nation_data[target_nation.index()] += flow;
		total_left += flow;
	});

	raw_content.clear();

	state.world.for_each_nation([&](auto target) {
		raw_content.emplace_back(target, per_nation_data[target.index()]);
	});

	std::sort(raw_content.begin(), raw_content.end(), [&](auto a, auto b) {
		if(a.value == b.value) {
			return a.key.value > b.key.value;
		}
		return a.value > b.value;
	});

	// prepare data
	graph_content.clear();
	graph_content.resize(15);

	for(int32_t k = 0; k < 14; k++) {
		graph_content[k].key = raw_content[k].key;
		graph_content[k].amount = raw_content[k].value;

		auto c = state.world.nation_get_color(raw_content[k].key);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[k].color = c3f;

		total_left -= raw_content[k].value;
	}

	graph_content[14].color.r = 0.f;
	graph_content[14].color.g = 0.f;
	graph_content[14].color.b = 0.f;
	graph_content[14].key = { };
	graph_content[14].amount = total_left;

	update_chart(state);
// END
}
void province_economy_overview_body_export_value_graph_t::on_create(sys::state& state) noexcept {
// BEGIN body::export_value_graph::create
// END
}
ui::message_result province_economy_overview_body_import_structure_graph_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_import_structure_graph_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_import_structure_graph_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 300; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 300; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(300)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_import_structure_graph_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::import_structure_graph::tooltip
		if(selected_key) {
			auto box = text::open_layout_box(contents);
			text::add_unparsed_text_to_layout_box(state, contents, box, text::get_commodity_name_with_icon(state, selected_key));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
			text::close_layout_box(contents, box);
		} else {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
			text::close_layout_box(contents, box);
		}
// END
	}
}
void province_economy_overview_body_import_structure_graph_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void province_economy_overview_body_import_structure_graph_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::import_structure_graph::update
	raw_content.clear();

	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	float total_left = 0.f;

	state.world.for_each_commodity([&](auto cid) {
		auto flow = economy::trade_influx(state, mid, cid);
		auto value_flow = flow * state.world.commodity_get_median_price(cid);
		raw_content.emplace_back(cid, value_flow);
		total_left += value_flow;
	});

	std::sort(raw_content.begin(), raw_content.end(), [&](auto a, auto b) {
		if(a.value == b.value) {
			return a.key.value > b.key.value;
		}
		return a.value > b.value;
	});

	// prepare data
	graph_content.clear();
	graph_content.resize(15);

	for(int32_t k = 0; k < 14; k++) {
		graph_content[k].key = raw_content[k].key;
		graph_content[k].amount = raw_content[k].value;

		auto c = state.world.commodity_get_color(raw_content[k].key);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[k].color = c3f;

		total_left -= raw_content[k].value;
	}

	graph_content[14].color.r = 0.f;
	graph_content[14].color.g = 0.f;
	graph_content[14].color.b = 0.f;
	graph_content[14].key = { };
	graph_content[14].amount = total_left;

	update_chart(state);
// END
}
void province_economy_overview_body_import_structure_graph_t::on_create(sys::state& state) noexcept {
// BEGIN body::import_structure_graph::create
// END
}
ui::message_result province_economy_overview_body_export_structure_graph_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_export_structure_graph_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_export_structure_graph_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 300; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 300; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(300)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_export_structure_graph_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::export_structure_graph::tooltip
		if(selected_key) {
			auto box = text::open_layout_box(contents);
			text::add_unparsed_text_to_layout_box(state, contents, box, text::get_commodity_name_with_icon(state, selected_key));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
			text::close_layout_box(contents, box);
		} else {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
			text::close_layout_box(contents, box);
		}
// END
	}
}
void province_economy_overview_body_export_structure_graph_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void province_economy_overview_body_export_structure_graph_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::export_structure_graph::update
	raw_content.clear();

	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);

	float total_left = 0.f;

	state.world.for_each_commodity([&](auto cid) {
		auto flow = economy::trade_outflux(state, mid, cid);
		auto value_flow = flow * state.world.commodity_get_median_price(cid);
		raw_content.emplace_back(cid, value_flow);
		total_left += value_flow;
	});

	std::sort(raw_content.begin(), raw_content.end(), [&](auto a, auto b) {
		if(a.value == b.value) {
			return a.key.value > b.key.value;
		}
		return a.value > b.value;
	});

	// prepare data
	graph_content.clear();
	graph_content.resize(15);

	for(int32_t k = 0; k < 14; k++) {
		graph_content[k].key = raw_content[k].key;
		graph_content[k].amount = raw_content[k].value;

		auto c = state.world.commodity_get_color(raw_content[k].key);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[k].color = c3f;

		total_left -= raw_content[k].value;
	}

	graph_content[14].color.r = 0.f;
	graph_content[14].color.g = 0.f;
	graph_content[14].color.b = 0.f;
	graph_content[14].key = { };
	graph_content[14].amount = total_left;

	update_chart(state);
// END
}
void province_economy_overview_body_export_structure_graph_t::on_create(sys::state& state) noexcept {
// BEGIN body::export_structure_graph::create
// END
}
void province_economy_overview_body_rgo_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_rgo_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_rgo_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_rgo_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_label::update
// END
}
void province_economy_overview_body_rgo_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::rgo_label::create
// END
}
ui::message_result province_economy_overview_body_rgo_details_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN body::rgo_details::lbutton_action
	auto target_is_visible = body.rgo_details_window->is_visible();
	body.rgo_details_window->set_visible(state, !target_is_visible);
	body.rgo_details_window->parent->move_child_to_front(body.rgo_details_window);
// END
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_body_rgo_details_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_body_rgo_details_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_rgo_details_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_rgo_details_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, true); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_rgo_details_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_details::update
// END
}
void province_economy_overview_body_rgo_details_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::rgo_details::create
// END
}
void province_economy_overview_body_rgo_employment_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_rgo_employment_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_rgo_employment_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_rgo_employment_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_employment_label::update
// END
}
void province_economy_overview_body_rgo_employment_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::rgo_employment_label::create
// END
}
void province_economy_overview_body_rgo_profit_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_rgo_profit_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_rgo_profit_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_rgo_profit_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::rgo_profit_label::update
// END
}
void province_economy_overview_body_rgo_profit_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::rgo_profit_label::create
// END
}
ui::message_result province_economy_overview_body_bar_chart_overlay_1_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_bar_chart_overlay_1_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_bar_chart_overlay_1_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_bar_chart_overlay_1_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::bar_chart_overlay_1::update
// END
}
void province_economy_overview_body_bar_chart_overlay_1_t::on_create(sys::state& state) noexcept {
// BEGIN body::bar_chart_overlay_1::create
// END
}
ui::message_result province_economy_overview_body_bar_chart_overlay_2_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_bar_chart_overlay_2_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_bar_chart_overlay_2_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_bar_chart_overlay_2_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::bar_chart_overlay_2::update
// END
}
void province_economy_overview_body_bar_chart_overlay_2_t::on_create(sys::state& state) noexcept {
// BEGIN body::bar_chart_overlay_2::create
// END
}
ui::message_result province_economy_overview_body_bar_chart_overlay_3_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_bar_chart_overlay_3_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_bar_chart_overlay_3_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_bar_chart_overlay_3_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::bar_chart_overlay_3::update
// END
}
void province_economy_overview_body_bar_chart_overlay_3_t::on_create(sys::state& state) noexcept {
// BEGIN body::bar_chart_overlay_3::create
// END
}
ui::message_result province_economy_overview_body_bar_chart_overlay_4_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_bar_chart_overlay_4_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_bar_chart_overlay_4_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void province_economy_overview_body_bar_chart_overlay_4_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::bar_chart_overlay_4::update
// END
}
void province_economy_overview_body_bar_chart_overlay_4_t::on_create(sys::state& state) noexcept {
// BEGIN body::bar_chart_overlay_4::create
// END
}
void province_economy_overview_body_per_nation_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_per_nation_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_per_nation_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_per_nation_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::per_nation_label::update
// END
}
void province_economy_overview_body_per_nation_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::per_nation_label::create
// END
}
void province_economy_overview_body_per_commodity_label_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_body_per_commodity_label_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void province_economy_overview_body_per_commodity_label_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_body_per_commodity_label_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::per_commodity_label::update
// END
}
void province_economy_overview_body_per_commodity_label_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN body::per_commodity_label::create
// END
}
ui::message_result province_economy_overview_body_gdp_chart_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_gdp_chart_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_gdp_chart_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 100; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 100; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_gdp_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float x_normal = float(x) / float(base_data.size.x) * 2.f - 1.f;
	float y_normal = float(y) / float(base_data.size.y) * 2.f - 1.f;
	float temp_offset = temp_total * (std::atan2f(-y_normal, -x_normal) / std::numbers::pi_v<float> / 2.f + 0.5f);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::gdp_chart::tooltip
		auto box = text::open_layout_box(contents);
		if(selected_key == 0) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "gdp_rgo"));
			text::add_space_to_layout_box(state, contents, box);
		} else if(selected_key == 1) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "gdp_factory"));
			text::add_space_to_layout_box(state, contents, box);
		} else if(selected_key == 2) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "gdp_artisans"));
			text::add_space_to_layout_box(state, contents, box);
		}

		text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_percentage(graph_content[temp_index].amount / temp_total));
		text::close_layout_box(contents, box);
// END
	}
}
void province_economy_overview_body_gdp_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_ui_mesh(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, mesh, data_texture);
}
void province_economy_overview_body_gdp_chart_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::gdp_chart::update
	auto pid = state.map_state.selected_province;

	// todo: add construction and services sectors

	// rgo
	float rgo_gdp = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		auto value_produced = economy::rgo_output(state, cid, pid) * state.world.commodity_get_median_price(cid);
		auto intermediate_consumption = economy::rgo_efficiency_spending(state, cid, pid);
		rgo_gdp += value_produced - intermediate_consumption;
	});

	// factories
	float factories_gdp = 0.f;
	state.world.province_for_each_factory_location_as_province(pid, [&](auto flid) {
		auto fid = state.world.factory_location_get_factory(flid);
		auto ftid = state.world.factory_get_building_type(fid);
		auto cid = state.world.factory_type_get_output(ftid);
		auto factory_details = economy::factory_operation::explain_everything(state, fid);
		
		auto value_produced = factory_details.output_actual_amount * state.world.commodity_get_median_price(cid);

		float intermediate_consumption = 0.f;

		for(uint32_t i = 0; i < factory_details.primary_inputs.set_size; i++) {
			auto cid_in = factory_details.primary_inputs.commodity_type[i];
			if(!cid_in) break;

			intermediate_consumption +=
				factory_details.primary_inputs.commodity_actual_amount[i]
				* state.world.commodity_get_median_price(cid_in);
		}

		if(factory_details.efficiency_inputs_worth_it) {
			for(uint32_t i = 0; i < factory_details.primary_inputs.set_size; i++) {
				auto cid_in = factory_details.primary_inputs.commodity_type[i];
				if(!cid_in) break;

				intermediate_consumption +=
					factory_details.primary_inputs.commodity_actual_amount[i]
					* state.world.commodity_get_median_price(cid_in);
			}
		}

		factories_gdp += value_produced - intermediate_consumption;
	});

	// artisans
	float artisans_gdp = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		auto value_produced = economy::artisan_output(state, cid, pid) * state.world.commodity_get_median_price(cid);
		auto intermediate_consumption = economy::estimate_artisan_gdp_intermediate_consumption(state, pid, cid);
		artisans_gdp += value_produced - intermediate_consumption;
	});

	graph_content[0].amount = std::max(0.f, rgo_gdp);
	graph_content[0].color = { 1.f, 0.f, 0.f };
	graph_content[0].key = 0;

	graph_content[1].amount = std::max(0.f, factories_gdp);
	graph_content[1].color = { 0.f, 1.f, 0.f };
	graph_content[1].key = 1;

	graph_content[2].amount = std::max(0.f, artisans_gdp);
	graph_content[2].color = { 0.f, 0.f, 1.f };
	graph_content[2].key = 2;

	update_chart(state);
// END
}
void province_economy_overview_body_gdp_chart_t::on_create(sys::state& state) noexcept {
// BEGIN body::gdp_chart::create
	graph_content.resize(3);
// END
}
ui::message_result province_economy_overview_body_gdp_share_chart_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_gdp_share_chart_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_gdp_share_chart_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 100; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 100; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_gdp_share_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float x_normal = float(x) / float(base_data.size.x) * 2.f - 1.f;
	float y_normal = float(y) / float(base_data.size.y) * 2.f - 1.f;
	float temp_offset = temp_total * (std::atan2f(-y_normal, -x_normal) / std::numbers::pi_v<float> / 2.f + 0.5f);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::gdp_share_chart::tooltip
		auto box = text::open_layout_box(contents);
		if(selected_key == 0) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "gdp_selected"));
			text::add_space_to_layout_box(state, contents, box);
		} else if(selected_key == 1) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "gdp_rest"));
			text::add_space_to_layout_box(state, contents, box);
		}

		text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::format_percentage(graph_content[temp_index].amount / temp_total));
		text::close_layout_box(contents, box);
// END
	}
}
void province_economy_overview_body_gdp_share_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_ui_mesh(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, mesh, data_texture);
}
void province_economy_overview_body_gdp_share_chart_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::gdp_share_chart::update
	auto pid = state.map_state.selected_province;
	// rgo
	float rgo_gdp = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		auto value_produced = economy::rgo_output(state, cid, pid) * state.world.commodity_get_median_price(cid);
		auto intermediate_consumption = economy::rgo_efficiency_spending(state, cid, pid);
		rgo_gdp += value_produced - intermediate_consumption;
	});

	// factories
	float factories_gdp = 0.f;
	state.world.province_for_each_factory_location_as_province(pid, [&](auto flid) {
		auto fid = state.world.factory_location_get_factory(flid);
		auto ftid = state.world.factory_get_building_type(fid);
		auto cid = state.world.factory_type_get_output(ftid);
		auto factory_details = economy::factory_operation::explain_everything(state, fid);

		auto value_produced = factory_details.output_actual_amount * state.world.commodity_get_median_price(cid);

		float intermediate_consumption = 0.f;

		for(uint32_t i = 0; i < factory_details.primary_inputs.set_size; i++) {
			auto cid_in = factory_details.primary_inputs.commodity_type[i];
			if(!cid_in) break;

			intermediate_consumption +=
				factory_details.primary_inputs.commodity_actual_amount[i]
				* state.world.commodity_get_median_price(cid_in);
		}

		if(factory_details.efficiency_inputs_worth_it) {
			for(uint32_t i = 0; i < factory_details.primary_inputs.set_size; i++) {
				auto cid_in = factory_details.primary_inputs.commodity_type[i];
				if(!cid_in) break;

				intermediate_consumption +=
					factory_details.primary_inputs.commodity_actual_amount[i]
					* state.world.commodity_get_median_price(cid_in);
			}
		}

		factories_gdp += value_produced - intermediate_consumption;
	});

	// artisans
	float artisans_gdp = 0.f;
	state.world.for_each_commodity([&](auto cid) {
		auto value_produced = economy::artisan_output(state, cid, pid) * state.world.commodity_get_median_price(cid);
		auto intermediate_consumption = economy::estimate_artisan_gdp_intermediate_consumption(state, pid, cid);
		artisans_gdp += value_produced - intermediate_consumption;
	});

	auto local_gdp = artisans_gdp + rgo_gdp + factories_gdp;

	auto total_gdp = economy::gdp(state, state.world.province_get_nation_from_province_ownership(pid));

	graph_content[0].amount = std::max(0.f, local_gdp);
	graph_content[0].color = { 1.f, 1.f, 1.f };
	graph_content[0].key = 0;

	graph_content[1].amount = std::max(0.f, total_gdp - local_gdp);
	graph_content[1].color = { 0.f, 0.f, 0.f };
	graph_content[1].key = 1;

	update_chart(state);

// END
}
void province_economy_overview_body_gdp_share_chart_t::on_create(sys::state& state) noexcept {
// BEGIN body::gdp_share_chart::create
	graph_content.resize(2);
// END
}
ui::message_result province_economy_overview_body_labor_chart_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_labor_chart_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_labor_chart_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 100; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 100; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_labor_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float x_normal = float(x) / float(base_data.size.x) * 2.f - 1.f;
	float y_normal = float(y) / float(base_data.size.y) * 2.f - 1.f;
	float temp_offset = temp_total * (std::atan2f(-y_normal, -x_normal) / std::numbers::pi_v<float> / 2.f + 0.5f);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::labor_chart::tooltip
	auto box = text::open_layout_box(contents);
	text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, ui::labour_type_to_employment_name_text_key(selected_key)));
	text::add_space_to_layout_box(state, contents, box);
	text::add_to_layout_box(state, contents, box, text::format_wholenum( (int32_t) graph_content[temp_index].amount ));
	text::add_space_to_layout_box(state, contents, box);
	text::add_to_layout_box(state, contents, box, text::format_percentage(graph_content[temp_index].amount / temp_total));
	text::close_layout_box(contents, box);
// END
	}
}
void province_economy_overview_body_labor_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_ui_mesh(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, mesh, data_texture);
}
void province_economy_overview_body_labor_chart_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::labor_chart::update
	auto pid = state.map_state.selected_province;

	for(int i = 0; i < economy::labor::total; i++) {
		graph_content[i].amount = state.world.province_get_labor_supply(pid, i);
		float t = (float)i / (float)(economy::labor::total);
		auto c = sys::hsv_to_rgb({ t * 360.f, 1.f, 1.f });
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[i].color = c3f;
		graph_content[i].key = i;
	}

	update_chart(state);
// END
}
void province_economy_overview_body_labor_chart_t::on_create(sys::state& state) noexcept {
// BEGIN body::labor_chart::create
	graph_content.resize(economy::labor::total);
// END
}
ui::message_result province_economy_overview_body_artisan_chart_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result province_economy_overview_body_artisan_chart_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void province_economy_overview_body_artisan_chart_t::update_chart(sys::state& state) {
	std::sort(graph_content.begin(), graph_content.end(), [](auto const& a, auto const& b) { return a.amount > b.amount; });
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 100; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 100; k++) {
		if(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void province_economy_overview_body_artisan_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float x_normal = float(x) / float(base_data.size.x) * 2.f - 1.f;
	float y_normal = float(y) / float(base_data.size.y) * 2.f - 1.f;
	float temp_offset = temp_total * (std::atan2f(-y_normal, -x_normal) / std::numbers::pi_v<float> / 2.f + 0.5f);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN body::artisan_chart::tooltip
	auto box = text::open_layout_box(contents);
	text::add_unparsed_text_to_layout_box(state, contents, box, text::get_commodity_name_with_icon(state, selected_key));
	text::add_space_to_layout_box(state, contents, box);
	text::add_to_layout_box(state, contents, box, text::format_wholenum((int32_t)graph_content[temp_index].amount));
	text::add_space_to_layout_box(state, contents, box);
	text::add_to_layout_box(state, contents, box, text::format_percentage(graph_content[temp_index].amount / temp_total));
	text::close_layout_box(contents, box);
// END
	}
}
void province_economy_overview_body_artisan_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_ui_mesh(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, mesh, data_texture);
}
void province_economy_overview_body_artisan_chart_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent)); 
// BEGIN body::artisan_chart::update
	auto pid = state.map_state.selected_province;
	state.world.for_each_commodity([&](auto cid) {
		if(economy::valid_artisan_good(state, state.world.province_get_nation_from_province_ownership(pid), cid)) {
			graph_content[cid.index()].amount = economy::artisan_employment_target(state, cid, pid);
		} else {
			graph_content[cid.index()].amount = 0.f;
		}
		auto c = state.world.commodity_get_color(cid);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[cid.index()].color = c3f;
		graph_content[cid.index()].key = cid;
	});
	update_chart(state);
// END
}
void province_economy_overview_body_artisan_chart_t::on_create(sys::state& state) noexcept {
// BEGIN body::artisan_chart::create
 	   graph_content.resize(state.world.commodity_size());
// END
}
ui::message_result province_economy_overview_body_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_body_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_body_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void province_economy_overview_body_t::on_update(sys::state& state) noexcept {
// BEGIN body::update
// END
	high_prices.update(state, this);
	low_prices.update(state, this);
	margin.update(state, this);
	rgo_employment.update(state, this);
	rgo_profit.update(state, this);
	remake_layout(state, true);
}
void province_economy_overview_body_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(100);
		lvl.page_controls->base_data.size.y = int16_t(20);
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
				if(cname == "gdp_label") {
					temp.ptr = gdp_label.get();
				}
				if(cname == "gdp_share_label") {
					temp.ptr = gdp_share_label.get();
				}
				if(cname == "labor_label") {
					temp.ptr = labor_label.get();
				}
				if(cname == "artisans_label") {
					temp.ptr = artisans_label.get();
				}
				if(cname == "prices_label") {
					temp.ptr = prices_label.get();
				}
				if(cname == "prices_details") {
					temp.ptr = prices_details.get();
				}
				if(cname == "margin_label") {
					temp.ptr = margin_label.get();
				}
				if(cname == "payback_time_label") {
					temp.ptr = payback_time_label.get();
				}
				if(cname == "trade_label") {
					temp.ptr = trade_label.get();
				}
				if(cname == "trade_details") {
					temp.ptr = trade_details.get();
				}
				if(cname == "import_value_icon") {
					temp.ptr = import_value_icon.get();
				}
				if(cname == "export_value_icon") {
					temp.ptr = export_value_icon.get();
				}
				if(cname == "import_structure_icon") {
					temp.ptr = import_structure_icon.get();
				}
				if(cname == "export_structure_icon") {
					temp.ptr = export_structure_icon.get();
				}
				if(cname == "import_value_graph") {
					temp.ptr = import_value_graph.get();
				}
				if(cname == "export_value_graph") {
					temp.ptr = export_value_graph.get();
				}
				if(cname == "import_structure_graph") {
					temp.ptr = import_structure_graph.get();
				}
				if(cname == "export_structure_graph") {
					temp.ptr = export_structure_graph.get();
				}
				if(cname == "rgo_label") {
					temp.ptr = rgo_label.get();
				}
				if(cname == "rgo_details") {
					temp.ptr = rgo_details.get();
				}
				if(cname == "rgo_employment_label") {
					temp.ptr = rgo_employment_label.get();
				}
				if(cname == "rgo_profit_label") {
					temp.ptr = rgo_profit_label.get();
				}
				if(cname == "bar_chart_overlay_1") {
					temp.ptr = bar_chart_overlay_1.get();
				}
				if(cname == "bar_chart_overlay_2") {
					temp.ptr = bar_chart_overlay_2.get();
				}
				if(cname == "bar_chart_overlay_3") {
					temp.ptr = bar_chart_overlay_3.get();
				}
				if(cname == "bar_chart_overlay_4") {
					temp.ptr = bar_chart_overlay_4.get();
				}
				if(cname == "per_nation_label") {
					temp.ptr = per_nation_label.get();
				}
				if(cname == "per_commodity_label") {
					temp.ptr = per_commodity_label.get();
				}
				if(cname == "gdp_chart") {
					temp.ptr = gdp_chart.get();
				}
				if(cname == "gdp_share_chart") {
					temp.ptr = gdp_share_chart.get();
				}
				if(cname == "labor_chart") {
					temp.ptr = labor_chart.get();
				}
				if(cname == "artisan_chart") {
					temp.ptr = artisan_chart.get();
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
					temp.ptr = make_province_economy_overview_body(state);
				}
				if(cname == "rgo_employment") {
					temp.ptr = make_province_economy_overview_rgo_employment(state);
				}
				if(cname == "rgo_profit") {
					temp.ptr = make_province_economy_overview_rgo_profit(state);
				}
				if(cname == "price_entry") {
					temp.ptr = make_province_economy_overview_price_entry(state);
				}
				if(cname == "method_margin") {
					temp.ptr = make_province_economy_overview_method_margin(state);
				}
				if(cname == "method_payback") {
					temp.ptr = make_province_economy_overview_method_payback(state);
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
				if(cname == "high_prices") {
					temp.generator = &high_prices;
				}
				if(cname == "low_prices") {
					temp.generator = &low_prices;
				}
				if(cname == "margin") {
					temp.generator = &margin;
				}
				if(cname == "rgo_employment") {
					temp.generator = &rgo_employment;
				}
				if(cname == "rgo_profit") {
					temp.generator = &rgo_profit;
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
void province_economy_overview_body_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_economy_overview::body"));
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
		if(child_data.name == "gdp_label") {
			gdp_label = std::make_unique<province_economy_overview_body_gdp_label_t>();
			gdp_label->parent = this;
			auto cptr = gdp_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_key = state.lookup_key(child_data.text_key);
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "gdp_share_label") {
			gdp_share_label = std::make_unique<province_economy_overview_body_gdp_share_label_t>();
			gdp_share_label->parent = this;
			auto cptr = gdp_share_label.get();
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
		if(child_data.name == "labor_label") {
			labor_label = std::make_unique<province_economy_overview_body_labor_label_t>();
			labor_label->parent = this;
			auto cptr = labor_label.get();
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
		if(child_data.name == "artisans_label") {
			artisans_label = std::make_unique<province_economy_overview_body_artisans_label_t>();
			artisans_label->parent = this;
			auto cptr = artisans_label.get();
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
		if(child_data.name == "prices_label") {
			prices_label = std::make_unique<province_economy_overview_body_prices_label_t>();
			prices_label->parent = this;
			auto cptr = prices_label.get();
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
		if(child_data.name == "prices_details") {
			prices_details = std::make_unique<province_economy_overview_body_prices_details_t>();
			prices_details->parent = this;
			auto cptr = prices_details.get();
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
		if(child_data.name == "margin_label") {
			margin_label = std::make_unique<province_economy_overview_body_margin_label_t>();
			margin_label->parent = this;
			auto cptr = margin_label.get();
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
		if(child_data.name == "payback_time_label") {
			payback_time_label = std::make_unique<province_economy_overview_body_payback_time_label_t>();
			payback_time_label->parent = this;
			auto cptr = payback_time_label.get();
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
		if(child_data.name == "trade_label") {
			trade_label = std::make_unique<province_economy_overview_body_trade_label_t>();
			trade_label->parent = this;
			auto cptr = trade_label.get();
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
		if(child_data.name == "trade_details") {
			trade_details = std::make_unique<province_economy_overview_body_trade_details_t>();
			trade_details->parent = this;
			auto cptr = trade_details.get();
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
		if(child_data.name == "import_value_icon") {
			import_value_icon = std::make_unique<province_economy_overview_body_import_value_icon_t>();
			import_value_icon->parent = this;
			auto cptr = import_value_icon.get();
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
		if(child_data.name == "export_value_icon") {
			export_value_icon = std::make_unique<province_economy_overview_body_export_value_icon_t>();
			export_value_icon->parent = this;
			auto cptr = export_value_icon.get();
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
		if(child_data.name == "import_structure_icon") {
			import_structure_icon = std::make_unique<province_economy_overview_body_import_structure_icon_t>();
			import_structure_icon->parent = this;
			auto cptr = import_structure_icon.get();
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
		if(child_data.name == "export_structure_icon") {
			export_structure_icon = std::make_unique<province_economy_overview_body_export_structure_icon_t>();
			export_structure_icon->parent = this;
			auto cptr = export_structure_icon.get();
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
		if(child_data.name == "import_value_graph") {
			import_value_graph = std::make_unique<province_economy_overview_body_import_value_graph_t>();
			import_value_graph->parent = this;
			auto cptr = import_value_graph.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "export_value_graph") {
			export_value_graph = std::make_unique<province_economy_overview_body_export_value_graph_t>();
			export_value_graph->parent = this;
			auto cptr = export_value_graph.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "import_structure_graph") {
			import_structure_graph = std::make_unique<province_economy_overview_body_import_structure_graph_t>();
			import_structure_graph->parent = this;
			auto cptr = import_structure_graph.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "export_structure_graph") {
			export_structure_graph = std::make_unique<province_economy_overview_body_export_structure_graph_t>();
			export_structure_graph->parent = this;
			auto cptr = export_structure_graph.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "rgo_label") {
			rgo_label = std::make_unique<province_economy_overview_body_rgo_label_t>();
			rgo_label->parent = this;
			auto cptr = rgo_label.get();
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
		if(child_data.name == "rgo_details") {
			rgo_details = std::make_unique<province_economy_overview_body_rgo_details_t>();
			rgo_details->parent = this;
			auto cptr = rgo_details.get();
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
		if(child_data.name == "rgo_employment_label") {
			rgo_employment_label = std::make_unique<province_economy_overview_body_rgo_employment_label_t>();
			rgo_employment_label->parent = this;
			auto cptr = rgo_employment_label.get();
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
		if(child_data.name == "rgo_profit_label") {
			rgo_profit_label = std::make_unique<province_economy_overview_body_rgo_profit_label_t>();
			rgo_profit_label->parent = this;
			auto cptr = rgo_profit_label.get();
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
		if(child_data.name == "bar_chart_overlay_1") {
			bar_chart_overlay_1 = std::make_unique<province_economy_overview_body_bar_chart_overlay_1_t>();
			bar_chart_overlay_1->parent = this;
			auto cptr = bar_chart_overlay_1.get();
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
		if(child_data.name == "bar_chart_overlay_2") {
			bar_chart_overlay_2 = std::make_unique<province_economy_overview_body_bar_chart_overlay_2_t>();
			bar_chart_overlay_2->parent = this;
			auto cptr = bar_chart_overlay_2.get();
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
		if(child_data.name == "bar_chart_overlay_3") {
			bar_chart_overlay_3 = std::make_unique<province_economy_overview_body_bar_chart_overlay_3_t>();
			bar_chart_overlay_3->parent = this;
			auto cptr = bar_chart_overlay_3.get();
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
		if(child_data.name == "bar_chart_overlay_4") {
			bar_chart_overlay_4 = std::make_unique<province_economy_overview_body_bar_chart_overlay_4_t>();
			bar_chart_overlay_4->parent = this;
			auto cptr = bar_chart_overlay_4.get();
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
		if(child_data.name == "per_nation_label") {
			per_nation_label = std::make_unique<province_economy_overview_body_per_nation_label_t>();
			per_nation_label->parent = this;
			auto cptr = per_nation_label.get();
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
		if(child_data.name == "per_commodity_label") {
			per_commodity_label = std::make_unique<province_economy_overview_body_per_commodity_label_t>();
			per_commodity_label->parent = this;
			auto cptr = per_commodity_label.get();
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
		if(child_data.name == "gdp_chart") {
			gdp_chart = std::make_unique<province_economy_overview_body_gdp_chart_t>();
			gdp_chart->parent = this;
			auto cptr = gdp_chart.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "gdp_share_chart") {
			gdp_share_chart = std::make_unique<province_economy_overview_body_gdp_share_chart_t>();
			gdp_share_chart->parent = this;
			auto cptr = gdp_share_chart.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "labor_chart") {
			labor_chart = std::make_unique<province_economy_overview_body_labor_chart_t>();
			labor_chart->parent = this;
			auto cptr = labor_chart.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "artisan_chart") {
			artisan_chart = std::make_unique<province_economy_overview_body_artisan_chart_t>();
			artisan_chart->parent = this;
			auto cptr = artisan_chart.get();
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
	high_prices.on_create(state, this);
	low_prices.on_create(state, this);
	margin.on_create(state, this);
	rgo_employment.on_create(state, this);
	rgo_profit.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN body::create
	{
		auto u_ptr = make_market_trade_report_body(state);
		trade_details_window = u_ptr.get();
		trade_details_window->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(u_ptr));
	}
	{
		auto u_ptr = make_rgo_report_body(state);
		rgo_details_window = u_ptr.get();
		rgo_details_window->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(u_ptr));
	}
	{
		auto u_ptr = make_market_prices_report_body(state);
		price_details_window = u_ptr.get();
		price_details_window->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(u_ptr));
	}
// END
}
std::unique_ptr<ui::element_base> make_province_economy_overview_body(sys::state& state) {
	auto ptr = std::make_unique<province_economy_overview_body_t>();
	ptr->on_create(state);
	return ptr;
}
void province_economy_overview_rgo_employment_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_rgo_employment_name_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_rgo_employment_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_rgo_employment_name_t::on_update(sys::state& state) noexcept {
	province_economy_overview_rgo_employment_t& rgo_employment = *((province_economy_overview_rgo_employment_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN rgo_employment::name::update
	set_text(state, text::get_commodity_name_with_icon(state, rgo_employment.top_commodity));
// END
}
void province_economy_overview_rgo_employment_name_t::on_create(sys::state& state) noexcept {
// BEGIN rgo_employment::name::create
// END
}
void province_economy_overview_rgo_employment_employment_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_rgo_employment_employment_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_rgo_employment_employment_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_rgo_employment_employment_t::on_update(sys::state& state) noexcept {
	province_economy_overview_rgo_employment_t& rgo_employment = *((province_economy_overview_rgo_employment_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN rgo_employment::employment::update
	auto cid = rgo_employment.top_commodity;
	auto pid = state.map_state.selected_province;
	set_text(state, text::format_wholenum((int32_t)economy::rgo_employment(state, cid, pid)));
// END
}
void province_economy_overview_rgo_employment_employment_t::on_create(sys::state& state) noexcept {
// BEGIN rgo_employment::employment::create
// END
}
ui::message_result province_economy_overview_rgo_employment_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_rgo_employment_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_rgo_employment_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void province_economy_overview_rgo_employment_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN rgo_employment::update
// END
	remake_layout(state, true);
}
void province_economy_overview_rgo_employment_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(100);
		lvl.page_controls->base_data.size.y = int16_t(20);
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
				if(cname == "name") {
					temp.ptr = name.get();
				}
				if(cname == "employment") {
					temp.ptr = employment.get();
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
					temp.ptr = make_province_economy_overview_body(state);
				}
				if(cname == "rgo_employment") {
					temp.ptr = make_province_economy_overview_rgo_employment(state);
				}
				if(cname == "rgo_profit") {
					temp.ptr = make_province_economy_overview_rgo_profit(state);
				}
				if(cname == "price_entry") {
					temp.ptr = make_province_economy_overview_price_entry(state);
				}
				if(cname == "method_margin") {
					temp.ptr = make_province_economy_overview_method_margin(state);
				}
				if(cname == "method_payback") {
					temp.ptr = make_province_economy_overview_method_payback(state);
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
void province_economy_overview_rgo_employment_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_economy_overview::rgo_employment"));
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
		if(child_data.name == "name") {
			name = std::make_unique<province_economy_overview_rgo_employment_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
		if(child_data.name == "employment") {
			employment = std::make_unique<province_economy_overview_rgo_employment_employment_t>();
			employment->parent = this;
			auto cptr = employment.get();
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
// BEGIN rgo_employment::create
// END
}
std::unique_ptr<ui::element_base> make_province_economy_overview_rgo_employment(sys::state& state) {
	auto ptr = std::make_unique<province_economy_overview_rgo_employment_t>();
	ptr->on_create(state);
	return ptr;
}
void province_economy_overview_rgo_profit_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_rgo_profit_name_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_rgo_profit_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_rgo_profit_name_t::on_update(sys::state& state) noexcept {
	province_economy_overview_rgo_profit_t& rgo_profit = *((province_economy_overview_rgo_profit_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN rgo_profit::name::update
	set_text(state, text::get_commodity_name_with_icon(state, rgo_profit.top_commodity));
// END
}
void province_economy_overview_rgo_profit_name_t::on_create(sys::state& state) noexcept {
// BEGIN rgo_profit::name::create
// END
}
void province_economy_overview_rgo_profit_profit_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_rgo_profit_profit_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_rgo_profit_profit_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_rgo_profit_profit_t::on_update(sys::state& state) noexcept {
	province_economy_overview_rgo_profit_t& rgo_profit = *((province_economy_overview_rgo_profit_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN rgo_profit::profit::update
	auto cid = rgo_profit.top_commodity;
	auto pid = state.map_state.selected_province;
	set_text(state, text::format_money(economy::rgo_income(state, cid, pid)));
// END
}
void province_economy_overview_rgo_profit_profit_t::on_create(sys::state& state) noexcept {
// BEGIN rgo_profit::profit::create
// END
}
ui::message_result province_economy_overview_rgo_profit_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_rgo_profit_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_rgo_profit_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void province_economy_overview_rgo_profit_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN rgo_profit::update
// END
	remake_layout(state, true);
}
void province_economy_overview_rgo_profit_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(100);
		lvl.page_controls->base_data.size.y = int16_t(20);
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
				if(cname == "name") {
					temp.ptr = name.get();
				}
				if(cname == "profit") {
					temp.ptr = profit.get();
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
					temp.ptr = make_province_economy_overview_body(state);
				}
				if(cname == "rgo_employment") {
					temp.ptr = make_province_economy_overview_rgo_employment(state);
				}
				if(cname == "rgo_profit") {
					temp.ptr = make_province_economy_overview_rgo_profit(state);
				}
				if(cname == "price_entry") {
					temp.ptr = make_province_economy_overview_price_entry(state);
				}
				if(cname == "method_margin") {
					temp.ptr = make_province_economy_overview_method_margin(state);
				}
				if(cname == "method_payback") {
					temp.ptr = make_province_economy_overview_method_payback(state);
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
void province_economy_overview_rgo_profit_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_economy_overview::rgo_profit"));
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
		if(child_data.name == "name") {
			name = std::make_unique<province_economy_overview_rgo_profit_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
		if(child_data.name == "profit") {
			profit = std::make_unique<province_economy_overview_rgo_profit_profit_t>();
			profit->parent = this;
			auto cptr = profit.get();
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
// BEGIN rgo_profit::create
// END
}
std::unique_ptr<ui::element_base> make_province_economy_overview_rgo_profit(sys::state& state) {
	auto ptr = std::make_unique<province_economy_overview_rgo_profit_t>();
	ptr->on_create(state);
	return ptr;
}
void province_economy_overview_price_entry_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_price_entry_name_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_price_entry_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_price_entry_name_t::on_update(sys::state& state) noexcept {
	province_economy_overview_price_entry_t& price_entry = *((province_economy_overview_price_entry_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN price_entry::name::update
	set_text(state, text::get_commodity_name_with_icon(state, price_entry.commodity));
// END
}
void province_economy_overview_price_entry_name_t::on_create(sys::state& state) noexcept {
// BEGIN price_entry::name::create
// END
}
void province_economy_overview_price_entry_local_price_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void province_economy_overview_price_entry_local_price_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_price_entry_local_price_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_price_entry_local_price_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_price_entry_local_price_t::on_update(sys::state& state) noexcept {
	province_economy_overview_price_entry_t& price_entry = *((province_economy_overview_price_entry_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN price_entry::local_price::update
	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);
	auto cid = price_entry.commodity;
	set_text(state, text::format_money(economy::price(state, mid, cid)));
// END
}
void province_economy_overview_price_entry_local_price_t::on_create(sys::state& state) noexcept {
// BEGIN price_entry::local_price::create
// END
}
void province_economy_overview_price_entry_ratio_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void province_economy_overview_price_entry_ratio_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_price_entry_ratio_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_price_entry_ratio_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_price_entry_ratio_t::on_update(sys::state& state) noexcept {
	province_economy_overview_price_entry_t& price_entry = *((province_economy_overview_price_entry_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN price_entry::ratio::update
	auto pid = state.map_state.selected_province;
	auto sid = state.world.province_get_state_membership(pid);
	auto mid = state.world.state_instance_get_market_from_local_market(sid);
	auto cid = price_entry.commodity;
	set_text(state, text::format_percentage(economy::price(state, mid, cid) / state.world.commodity_get_median_price(price_entry.commodity)));
// END
}
void province_economy_overview_price_entry_ratio_t::on_create(sys::state& state) noexcept {
// BEGIN price_entry::ratio::create
// END
}
ui::message_result province_economy_overview_price_entry_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_price_entry_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_price_entry_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void province_economy_overview_price_entry_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN price_entry::update
// END
	remake_layout(state, true);
}
void province_economy_overview_price_entry_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(100);
		lvl.page_controls->base_data.size.y = int16_t(20);
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
				if(cname == "name") {
					temp.ptr = name.get();
				}
				if(cname == "local_price") {
					temp.ptr = local_price.get();
				}
				if(cname == "ratio") {
					temp.ptr = ratio.get();
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
					temp.ptr = make_province_economy_overview_body(state);
				}
				if(cname == "rgo_employment") {
					temp.ptr = make_province_economy_overview_rgo_employment(state);
				}
				if(cname == "rgo_profit") {
					temp.ptr = make_province_economy_overview_rgo_profit(state);
				}
				if(cname == "price_entry") {
					temp.ptr = make_province_economy_overview_price_entry(state);
				}
				if(cname == "method_margin") {
					temp.ptr = make_province_economy_overview_method_margin(state);
				}
				if(cname == "method_payback") {
					temp.ptr = make_province_economy_overview_method_payback(state);
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
void province_economy_overview_price_entry_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_economy_overview::price_entry"));
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
		if(child_data.name == "name") {
			name = std::make_unique<province_economy_overview_price_entry_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
		if(child_data.name == "local_price") {
			local_price = std::make_unique<province_economy_overview_price_entry_local_price_t>();
			local_price->parent = this;
			auto cptr = local_price.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "ratio") {
			ratio = std::make_unique<province_economy_overview_price_entry_ratio_t>();
			ratio->parent = this;
			auto cptr = ratio.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
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
// BEGIN price_entry::create
// END
}
std::unique_ptr<ui::element_base> make_province_economy_overview_price_entry(sys::state& state) {
	auto ptr = std::make_unique<province_economy_overview_price_entry_t>();
	ptr->on_create(state);
	return ptr;
}
void province_economy_overview_method_margin_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_method_margin_name_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_method_margin_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_method_margin_name_t::on_update(sys::state& state) noexcept {
	province_economy_overview_method_margin_t& method_margin = *((province_economy_overview_method_margin_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN method_margin::name::update
	auto ftid = method_margin.top_factory;
	set_text(state, text::produce_simple_string(state, state.world.factory_type_get_name(ftid)));
// END
}
void province_economy_overview_method_margin_name_t::on_create(sys::state& state) noexcept {
// BEGIN method_margin::name::create
// END
}
void province_economy_overview_method_margin_margin_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_method_margin_margin_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_method_margin_margin_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_method_margin_margin_t::on_update(sys::state& state) noexcept {
	province_economy_overview_method_margin_t& method_margin = *((province_economy_overview_method_margin_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN method_margin::margin::update
	auto pid = state.map_state.selected_province;
	auto ftid = method_margin.top_factory;
	set_text(state, text::format_percentage(economy::estimate_factory_profit_margin(state, pid, ftid)));
// END
}
void province_economy_overview_method_margin_margin_t::on_create(sys::state& state) noexcept {
// BEGIN method_margin::margin::create
// END
}
ui::message_result province_economy_overview_method_margin_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_method_margin_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_method_margin_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void province_economy_overview_method_margin_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN method_margin::update
// END
	remake_layout(state, true);
}
void province_economy_overview_method_margin_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(100);
		lvl.page_controls->base_data.size.y = int16_t(20);
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
				if(cname == "name") {
					temp.ptr = name.get();
				}
				if(cname == "margin") {
					temp.ptr = margin.get();
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
					temp.ptr = make_province_economy_overview_body(state);
				}
				if(cname == "rgo_employment") {
					temp.ptr = make_province_economy_overview_rgo_employment(state);
				}
				if(cname == "rgo_profit") {
					temp.ptr = make_province_economy_overview_rgo_profit(state);
				}
				if(cname == "price_entry") {
					temp.ptr = make_province_economy_overview_price_entry(state);
				}
				if(cname == "method_margin") {
					temp.ptr = make_province_economy_overview_method_margin(state);
				}
				if(cname == "method_payback") {
					temp.ptr = make_province_economy_overview_method_payback(state);
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
void province_economy_overview_method_margin_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_economy_overview::method_margin"));
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
		if(child_data.name == "name") {
			name = std::make_unique<province_economy_overview_method_margin_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
		if(child_data.name == "margin") {
			margin = std::make_unique<province_economy_overview_method_margin_margin_t>();
			margin->parent = this;
			auto cptr = margin.get();
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
// BEGIN method_margin::create
// END
}
std::unique_ptr<ui::element_base> make_province_economy_overview_method_margin(sys::state& state) {
	auto ptr = std::make_unique<province_economy_overview_method_margin_t>();
	ptr->on_create(state);
	return ptr;
}
void province_economy_overview_method_payback_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_method_payback_name_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_method_payback_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_method_payback_name_t::on_update(sys::state& state) noexcept {
	province_economy_overview_method_payback_t& method_payback = *((province_economy_overview_method_payback_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent->parent)); 
// BEGIN method_payback::name::update
	auto ftid = method_payback.top_factory;
	set_text(state, text::produce_simple_string(state, state.world.factory_type_get_name(ftid)));
// END
}
void province_economy_overview_method_payback_name_t::on_create(sys::state& state) noexcept {
// BEGIN method_payback::name::create
// END
}
void province_economy_overview_method_payback_days_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void province_economy_overview_method_payback_days_t::on_reset_text(sys::state& state) noexcept {
}
void province_economy_overview_method_payback_days_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void province_economy_overview_method_payback_days_t::on_update(sys::state& state) noexcept {
	province_economy_overview_method_payback_t& method_payback = *((province_economy_overview_method_payback_t*)(parent)); 
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent->parent)); 
// BEGIN method_payback::days::update
	auto pid = state.map_state.selected_province;
	auto ftid = method_payback.top_factory;
	set_text(state, text::format_wholenum((int32_t)(economy::estimate_factory_payback_time(state, pid, ftid))));
// END
}
void province_economy_overview_method_payback_days_t::on_create(sys::state& state) noexcept {
// BEGIN method_payback::days::create
// END
}
ui::message_result province_economy_overview_method_payback_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_economy_overview_method_payback_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_economy_overview_method_payback_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& __item : textures_to_render) {
		if (__item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (__item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + __item.x), float(y + __item.y), float(__item.w), float(__item.h), ogl::get_late_load_texture_handle(state, __item.texture_id, __item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void province_economy_overview_method_payback_t::on_update(sys::state& state) noexcept {
	province_economy_overview_body_t& body = *((province_economy_overview_body_t*)(parent->parent)); 
// BEGIN method_payback::update
// END
	remake_layout(state, true);
}
void province_economy_overview_method_payback_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
		lvl.page_controls->base_data.size.x = int16_t(100);
		lvl.page_controls->base_data.size.y = int16_t(20);
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
				if(cname == "name") {
					temp.ptr = name.get();
				}
				if(cname == "days") {
					temp.ptr = days.get();
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
					temp.ptr = make_province_economy_overview_body(state);
				}
				if(cname == "rgo_employment") {
					temp.ptr = make_province_economy_overview_rgo_employment(state);
				}
				if(cname == "rgo_profit") {
					temp.ptr = make_province_economy_overview_rgo_profit(state);
				}
				if(cname == "price_entry") {
					temp.ptr = make_province_economy_overview_price_entry(state);
				}
				if(cname == "method_margin") {
					temp.ptr = make_province_economy_overview_method_margin(state);
				}
				if(cname == "method_payback") {
					temp.ptr = make_province_economy_overview_method_payback(state);
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
void province_economy_overview_method_payback_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_economy_overview::method_payback"));
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
		if(child_data.name == "name") {
			name = std::make_unique<province_economy_overview_method_payback_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
		if(child_data.name == "days") {
			days = std::make_unique<province_economy_overview_method_payback_days_t>();
			days->parent = this;
			auto cptr = days.get();
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
// BEGIN method_payback::create
// END
}
std::unique_ptr<ui::element_base> make_province_economy_overview_method_payback(sys::state& state) {
	auto ptr = std::make_unique<province_economy_overview_method_payback_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
// BEGIN body::payback::on_create
////	base_data.clear();
////	state.world.for_each_factory_type([&](auto a) {
////		base_data.push_back(a);
////	});
// END
// BEGIN body::payback::update
////	auto pid = state.map_state.selected_province;
////	auto sid = state.world.province_get_state_membership(pid);
////	auto nid = state.world.province_get_nation_from_province_ownership(pid);
////	auto mid = state.world.state_instance_get_market_from_local_market(sid);

////	std::sort(base_data.begin(), base_data.end(), [&](auto a, auto b) {
////		auto value_a = economy::estimate_factory_payback_time(state, pid, a);
////		auto value_b = economy::estimate_factory_payback_time(state, pid, b);

////		if(value_a == value_b) {
////			return a.value > b.value;
////		}
////		return value_a < value_b;
////	});

////	values.clear();
////	for(size_t i = 0; i < std::min((size_t)4, base_data.size()); i++) {
////		if(std::isfinite(economy::estimate_factory_payback_time(state, pid, base_data[i]))) {
////			add_method_payback(base_data[i]);
////		}
////	}
// END
// BEGIN price_entry::median_price::update
////	set_text(state, text::format_money(state.world.commodity_get_median_price(price_entry.commodity)));
// END
// BEGIN body::payback::variables
////	std::vector<dcon::factory_type_id> base_data;
// END
}
