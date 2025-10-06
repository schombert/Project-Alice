namespace alice_ui {
struct trade_dashboard_main_per_commodity_details_t;
struct trade_dashboard_main_per_nation_details_t;
struct trade_dashboard_main_nation_import_per_commodity_t;
struct trade_dashboard_main_nation_export_per_commodity_t;
struct trade_dashboard_main_nation_import_per_nation_t;
struct trade_dashboard_main_nation_export_per_nation_t;
struct trade_dashboard_main_icon_import_t;
struct trade_dashboard_main_icon_export_t;
struct trade_dashboard_main_label_gdp_top_t;
struct trade_dashboard_main_label_gdp_capita_top_t;
struct trade_dashboard_main_commodity_import_per_nation_t;
struct trade_dashboard_main_commodity_export_per_nation_t;
struct trade_dashboard_main_commodity_production_per_nation_t;
struct trade_dashboard_main_commodity_consumption_per_nation_t;
struct trade_dashboard_main_label_production_top_t;
struct trade_dashboard_main_label_consumption_top_t;
struct trade_dashboard_main_gdp_per_sector_t;
struct trade_dashboard_main_circle_t;
struct trade_dashboard_main_gdp_sphere_t;
struct trade_dashboard_main_gdp_nations_t;
struct trade_dashboard_main_icon_import_2_t;
struct trade_dashboard_main_icon_import_3_t;
struct trade_dashboard_main_icon_export_2_t;
struct trade_dashboard_main_icon_export_3_t;
struct trade_dashboard_main_close_t;
struct trade_dashboard_main_select_self_t;
struct trade_dashboard_main_icon_consumption_t;
struct trade_dashboard_main_icon_production_t;
struct trade_dashboard_main_old_window_t;
struct trade_dashboard_main_selected_commodity_top_provinces_t;
struct trade_dashboard_main_selected_commodity_breakdown_t;
struct trade_dashboard_main_selected_nation_breakdown_t;
struct trade_dashboard_main_t;
struct trade_dashboard_province_row_consumption_flag_t;
struct trade_dashboard_province_row_consumption_name_t;
struct trade_dashboard_province_row_consumption_value_t;
struct trade_dashboard_province_row_consumption_t;
struct trade_dashboard_province_row_production_flag_t;
struct trade_dashboard_province_row_production_name_t;
struct trade_dashboard_province_row_production_value_t;
struct trade_dashboard_province_row_production_t;
struct trade_dashboard_province_row_gdp_flag_t;
struct trade_dashboard_province_row_gdp_name_t;
struct trade_dashboard_province_row_gdp_value_t;
struct trade_dashboard_province_row_gdp_t;
struct trade_dashboard_province_row_gdp_per_capita_flag_t;
struct trade_dashboard_province_row_gdp_per_capita_name_t;
struct trade_dashboard_province_row_gdp_per_capita_value_t;
struct trade_dashboard_province_row_gdp_per_capita_t;
struct trade_dashboard_commodity_selector_icon_t;
struct trade_dashboard_commodity_selector_name_t;
struct trade_dashboard_commodity_selector_t;
struct trade_dashboard_nation_selector_flag_t;
struct trade_dashboard_nation_selector_name_t;
struct trade_dashboard_nation_selector_t;
struct trade_dashboard_main_per_commodity_details_t : public ui::element_base {
// BEGIN main::per_commodity_details::variables
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
struct trade_dashboard_main_per_nation_details_t : public ui::element_base {
// BEGIN main::per_nation_details::variables
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
struct trade_dashboard_main_nation_import_per_commodity_t : public ui::element_base {
// BEGIN main::nation_import_per_commodity::variables
// END
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
struct trade_dashboard_main_nation_export_per_commodity_t : public ui::element_base {
// BEGIN main::nation_export_per_commodity::variables
// END
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
struct trade_dashboard_main_nation_import_per_nation_t : public ui::element_base {
// BEGIN main::nation_import_per_nation::variables
// END
	ogl::data_texture data_texture{ 100, 3 };
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
struct trade_dashboard_main_nation_export_per_nation_t : public ui::element_base {
// BEGIN main::nation_export_per_nation::variables
// END
	ogl::data_texture data_texture{ 100, 3 };
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
struct trade_dashboard_main_icon_import_t : public ui::element_base {
// BEGIN main::icon_import::variables
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
struct trade_dashboard_main_icon_export_t : public ui::element_base {
// BEGIN main::icon_export::variables
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
struct trade_dashboard_main_label_gdp_top_t : public ui::element_base {
// BEGIN main::label_gdp_top::variables
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
struct trade_dashboard_main_label_gdp_capita_top_t : public ui::element_base {
// BEGIN main::label_gdp_capita_top::variables
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
struct trade_dashboard_main_commodity_import_per_nation_t : public ui::element_base {
// BEGIN main::commodity_import_per_nation::variables
// END
	ogl::data_texture data_texture{ 100, 3 };
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
struct trade_dashboard_main_commodity_export_per_nation_t : public ui::element_base {
// BEGIN main::commodity_export_per_nation::variables
// END
	ogl::data_texture data_texture{ 100, 3 };
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
struct trade_dashboard_main_commodity_production_per_nation_t : public ui::element_base {
// BEGIN main::commodity_production_per_nation::variables
// END
	ogl::data_texture data_texture{ 100, 3 };
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
struct trade_dashboard_main_commodity_consumption_per_nation_t : public ui::element_base {
// BEGIN main::commodity_consumption_per_nation::variables
// END
	ogl::data_texture data_texture{ 100, 3 };
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
struct trade_dashboard_main_label_production_top_t : public ui::element_base {
// BEGIN main::label_production_top::variables
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
struct trade_dashboard_main_label_consumption_top_t : public ui::element_base {
// BEGIN main::label_consumption_top::variables
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
struct trade_dashboard_main_gdp_per_sector_t : public ui::element_base {
// BEGIN main::gdp_per_sector::variables
// END
	ogl::generic_ui_mesh_triangle_strip mesh{ 200 };
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {int key; ogl::color3f color; float amount; };
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
			auto cx = (float)(x - base_data.size.x / 2) / (float)(base_data.size.x);
			auto cy = (float)(y - base_data.size.y / 2) / (float)(base_data.size.y);
			auto d = std::sqrt(cx * cx + cy * cy);
			if (d > 0.3f && d < 0.5f) return ui::message_result::consumed;
			else return ui::message_result::unseen;
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
struct trade_dashboard_main_circle_t : public ui::element_base {
// BEGIN main::circle::variables
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
struct trade_dashboard_main_gdp_sphere_t : public ui::element_base {
// BEGIN main::gdp_sphere::variables
// END
	ogl::generic_ui_mesh_triangle_strip mesh{ 600 };
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
			auto cx = (float)(x - base_data.size.x / 2) / (float)(base_data.size.x);
			auto cy = (float)(y - base_data.size.y / 2) / (float)(base_data.size.y);
			auto d = std::sqrt(cx * cx + cy * cy);
			if (d > 0.3f && d < 0.5f) return ui::message_result::consumed;
			else return ui::message_result::unseen;
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
struct trade_dashboard_main_gdp_nations_t : public ui::element_base {
// BEGIN main::gdp_nations::variables
// END
	ogl::generic_ui_mesh_triangle_strip mesh{ 600 };
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
			auto cx = (float)(x - base_data.size.x / 2) / (float)(base_data.size.x);
			auto cy = (float)(y - base_data.size.y / 2) / (float)(base_data.size.y);
			auto d = std::sqrt(cx * cx + cy * cy);
			if (d > 0.3f && d < 0.5f) return ui::message_result::consumed;
			else return ui::message_result::unseen;
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
struct trade_dashboard_main_icon_import_2_t : public ui::element_base {
// BEGIN main::icon_import_2::variables
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
struct trade_dashboard_main_icon_import_3_t : public ui::element_base {
// BEGIN main::icon_import_3::variables
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
struct trade_dashboard_main_icon_export_2_t : public ui::element_base {
// BEGIN main::icon_export_2::variables
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
struct trade_dashboard_main_icon_export_3_t : public ui::element_base {
// BEGIN main::icon_export_3::variables
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
struct trade_dashboard_main_close_t : public ui::element_base {
// BEGIN main::close::variables
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
	ui::message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct trade_dashboard_main_select_self_t : public ui::element_base {
// BEGIN main::select_self::variables
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
struct trade_dashboard_main_icon_consumption_t : public ui::element_base {
// BEGIN main::icon_consumption::variables
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
struct trade_dashboard_main_icon_production_t : public ui::element_base {
// BEGIN main::icon_production::variables
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
struct trade_dashboard_main_old_window_t : public ui::element_base {
// BEGIN main::old_window::variables
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
struct trade_dashboard_main_selected_commodity_top_provinces_t : public ui::element_base {
// BEGIN main::selected_commodity_top_provinces::variables
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
struct trade_dashboard_main_selected_commodity_breakdown_t : public ui::element_base {
// BEGIN main::selected_commodity_breakdown::variables
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
struct trade_dashboard_main_selected_nation_breakdown_t : public ui::element_base {
// BEGIN main::selected_nation_breakdown::variables
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
struct trade_dashboard_main_commodity_list_t : public layout_generator {
// BEGIN main::commodity_list::variables
// END
	struct commodity_selector_option { dcon::commodity_id cid; };
	std::vector<std::unique_ptr<ui::element_base>> commodity_selector_pool;
	int32_t commodity_selector_pool_used = 0;
	void add_commodity_selector( dcon::commodity_id cid);
	std::vector<std::variant<std::monostate, commodity_selector_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct trade_dashboard_main_top_production_t : public layout_generator {
// BEGIN main::top_production::variables
// END
	struct province_row_production_option { dcon::province_id pid; };
	std::vector<std::unique_ptr<ui::element_base>> province_row_production_pool;
	int32_t province_row_production_pool_used = 0;
	void add_province_row_production( dcon::province_id pid);
	std::vector<std::variant<std::monostate, province_row_production_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct trade_dashboard_main_top_consumption_t : public layout_generator {
// BEGIN main::top_consumption::variables
// END
	struct province_row_consumption_option { dcon::province_id pid; };
	std::vector<std::unique_ptr<ui::element_base>> province_row_consumption_pool;
	int32_t province_row_consumption_pool_used = 0;
	void add_province_row_consumption( dcon::province_id pid);
	std::vector<std::variant<std::monostate, province_row_consumption_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct trade_dashboard_main_nation_list_t : public layout_generator {
// BEGIN main::nation_list::variables
// END
	struct nation_selector_option { dcon::nation_id nid; };
	std::vector<std::unique_ptr<ui::element_base>> nation_selector_pool;
	int32_t nation_selector_pool_used = 0;
	void add_nation_selector( dcon::nation_id nid);
	std::vector<std::variant<std::monostate, nation_selector_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct trade_dashboard_main_top_gdp_t : public layout_generator {
// BEGIN main::top_gdp::variables
// END
	struct province_row_gdp_option { dcon::province_id pid; };
	std::vector<std::unique_ptr<ui::element_base>> province_row_gdp_pool;
	int32_t province_row_gdp_pool_used = 0;
	void add_province_row_gdp( dcon::province_id pid);
	std::vector<std::variant<std::monostate, province_row_gdp_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct trade_dashboard_main_top_gdp_capita_t : public layout_generator {
// BEGIN main::top_gdp_capita::variables
// END
	struct province_row_gdp_per_capita_option { dcon::province_id pid; };
	std::vector<std::unique_ptr<ui::element_base>> province_row_gdp_per_capita_pool;
	int32_t province_row_gdp_per_capita_pool_used = 0;
	void add_province_row_gdp_per_capita( dcon::province_id pid);
	std::vector<std::variant<std::monostate, province_row_gdp_per_capita_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct trade_dashboard_province_row_consumption_flag_t : public ui::element_base {
// BEGIN province_row_consumption::flag::variables
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
struct trade_dashboard_province_row_consumption_name_t : public ui::element_base {
// BEGIN province_row_consumption::name::variables
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
struct trade_dashboard_province_row_consumption_value_t : public ui::element_base {
// BEGIN province_row_consumption::value::variables
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
struct trade_dashboard_province_row_production_flag_t : public ui::element_base {
// BEGIN province_row_production::flag::variables
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
struct trade_dashboard_province_row_production_name_t : public ui::element_base {
// BEGIN province_row_production::name::variables
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
struct trade_dashboard_province_row_production_value_t : public ui::element_base {
// BEGIN province_row_production::value::variables
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
struct trade_dashboard_province_row_gdp_flag_t : public ui::element_base {
// BEGIN province_row_gdp::flag::variables
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
struct trade_dashboard_province_row_gdp_name_t : public ui::element_base {
// BEGIN province_row_gdp::name::variables
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
struct trade_dashboard_province_row_gdp_value_t : public ui::element_base {
// BEGIN province_row_gdp::value::variables
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
struct trade_dashboard_province_row_gdp_per_capita_flag_t : public ui::element_base {
// BEGIN province_row_gdp_per_capita::flag::variables
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
struct trade_dashboard_province_row_gdp_per_capita_name_t : public ui::element_base {
// BEGIN province_row_gdp_per_capita::name::variables
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
struct trade_dashboard_province_row_gdp_per_capita_value_t : public ui::element_base {
// BEGIN province_row_gdp_per_capita::value::variables
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
struct trade_dashboard_commodity_selector_icon_t : public ui::element_base {
// BEGIN commodity_selector::icon::variables
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
struct trade_dashboard_commodity_selector_name_t : public ui::element_base {
// BEGIN commodity_selector::name::variables
// END
	std::string_view texture_key;
	std::string_view alt_texture_key;
	dcon::texture_id alt_background_texture;
	bool is_active = false;
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
struct trade_dashboard_nation_selector_flag_t : public ui::element_base {
// BEGIN nation_selector::flag::variables
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
struct trade_dashboard_nation_selector_name_t : public ui::element_base {
// BEGIN nation_selector::name::variables
// END
	std::string_view texture_key;
	std::string_view alt_texture_key;
	dcon::texture_id alt_background_texture;
	bool is_active = false;
	dcon::texture_id background_texture;
	text::layout internal_layout;
	text::text_color text_color = text::text_color::black;
	float text_scale = 0.800000f; 
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
struct trade_dashboard_main_t : public layout_window_element {
// BEGIN main::variables
	dcon::nation_id nation_pov;
	ui::element_base * old_window_container;
	dcon::province_id::value_base_t current_index;
// END
	std::unique_ptr<trade_dashboard_main_per_commodity_details_t> per_commodity_details;
	std::unique_ptr<trade_dashboard_main_per_nation_details_t> per_nation_details;
	std::unique_ptr<trade_dashboard_main_nation_import_per_commodity_t> nation_import_per_commodity;
	std::unique_ptr<trade_dashboard_main_nation_export_per_commodity_t> nation_export_per_commodity;
	std::unique_ptr<trade_dashboard_main_nation_import_per_nation_t> nation_import_per_nation;
	std::unique_ptr<trade_dashboard_main_nation_export_per_nation_t> nation_export_per_nation;
	std::unique_ptr<trade_dashboard_main_icon_import_t> icon_import;
	std::unique_ptr<trade_dashboard_main_icon_export_t> icon_export;
	std::unique_ptr<trade_dashboard_main_label_gdp_top_t> label_gdp_top;
	std::unique_ptr<trade_dashboard_main_label_gdp_capita_top_t> label_gdp_capita_top;
	std::unique_ptr<trade_dashboard_main_commodity_import_per_nation_t> commodity_import_per_nation;
	std::unique_ptr<trade_dashboard_main_commodity_export_per_nation_t> commodity_export_per_nation;
	std::unique_ptr<trade_dashboard_main_commodity_production_per_nation_t> commodity_production_per_nation;
	std::unique_ptr<trade_dashboard_main_commodity_consumption_per_nation_t> commodity_consumption_per_nation;
	std::unique_ptr<trade_dashboard_main_label_production_top_t> label_production_top;
	std::unique_ptr<trade_dashboard_main_label_consumption_top_t> label_consumption_top;
	std::unique_ptr<trade_dashboard_main_gdp_per_sector_t> gdp_per_sector;
	std::unique_ptr<trade_dashboard_main_circle_t> circle;
	std::unique_ptr<trade_dashboard_main_gdp_sphere_t> gdp_sphere;
	std::unique_ptr<trade_dashboard_main_gdp_nations_t> gdp_nations;
	std::unique_ptr<trade_dashboard_main_icon_import_2_t> icon_import_2;
	std::unique_ptr<trade_dashboard_main_icon_import_3_t> icon_import_3;
	std::unique_ptr<trade_dashboard_main_icon_export_2_t> icon_export_2;
	std::unique_ptr<trade_dashboard_main_icon_export_3_t> icon_export_3;
	std::unique_ptr<trade_dashboard_main_close_t> close;
	std::unique_ptr<trade_dashboard_main_select_self_t> select_self;
	std::unique_ptr<trade_dashboard_main_icon_consumption_t> icon_consumption;
	std::unique_ptr<trade_dashboard_main_icon_production_t> icon_production;
	std::unique_ptr<trade_dashboard_main_old_window_t> old_window;
	std::unique_ptr<trade_dashboard_main_selected_commodity_top_provinces_t> selected_commodity_top_provinces;
	std::unique_ptr<trade_dashboard_main_selected_commodity_breakdown_t> selected_commodity_breakdown;
	std::unique_ptr<trade_dashboard_main_selected_nation_breakdown_t> selected_nation_breakdown;
	trade_dashboard_main_commodity_list_t commodity_list;
	trade_dashboard_main_top_production_t top_production;
	trade_dashboard_main_top_consumption_t top_consumption;
	trade_dashboard_main_nation_list_t nation_list;
	trade_dashboard_main_top_gdp_t top_gdp;
	trade_dashboard_main_top_gdp_capita_t top_gdp_capita;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
};
std::unique_ptr<ui::element_base> make_trade_dashboard_main(sys::state& state);
struct trade_dashboard_province_row_consumption_t : public layout_window_element {
// BEGIN province_row_consumption::variables
// END
	dcon::province_id pid;
	std::unique_ptr<trade_dashboard_province_row_consumption_flag_t> flag;
	std::unique_ptr<trade_dashboard_province_row_consumption_name_t> name;
	std::unique_ptr<trade_dashboard_province_row_consumption_value_t> value;
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
		if(name_parameter == "pid") {
			return (void*)(&pid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_consumption(sys::state& state);
struct trade_dashboard_province_row_production_t : public layout_window_element {
// BEGIN province_row_production::variables
// END
	dcon::province_id pid;
	std::unique_ptr<trade_dashboard_province_row_production_flag_t> flag;
	std::unique_ptr<trade_dashboard_province_row_production_name_t> name;
	std::unique_ptr<trade_dashboard_province_row_production_value_t> value;
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
		if(name_parameter == "pid") {
			return (void*)(&pid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_production(sys::state& state);
struct trade_dashboard_province_row_gdp_t : public layout_window_element {
// BEGIN province_row_gdp::variables
// END
	dcon::province_id pid;
	std::unique_ptr<trade_dashboard_province_row_gdp_flag_t> flag;
	std::unique_ptr<trade_dashboard_province_row_gdp_name_t> name;
	std::unique_ptr<trade_dashboard_province_row_gdp_value_t> value;
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
		if(name_parameter == "pid") {
			return (void*)(&pid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_gdp(sys::state& state);
struct trade_dashboard_province_row_gdp_per_capita_t : public layout_window_element {
// BEGIN province_row_gdp_per_capita::variables
// END
	dcon::province_id pid;
	std::unique_ptr<trade_dashboard_province_row_gdp_per_capita_flag_t> flag;
	std::unique_ptr<trade_dashboard_province_row_gdp_per_capita_name_t> name;
	std::unique_ptr<trade_dashboard_province_row_gdp_per_capita_value_t> value;
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
		if(name_parameter == "pid") {
			return (void*)(&pid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_gdp_per_capita(sys::state& state);
struct trade_dashboard_commodity_selector_t : public layout_window_element {
// BEGIN commodity_selector::variables
// END
	dcon::commodity_id cid;
	std::unique_ptr<trade_dashboard_commodity_selector_icon_t> icon;
	std::unique_ptr<trade_dashboard_commodity_selector_name_t> name;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "cid") {
			return (void*)(&cid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_trade_dashboard_commodity_selector(sys::state& state);
struct trade_dashboard_nation_selector_t : public layout_window_element {
// BEGIN nation_selector::variables
// END
	dcon::nation_id nid;
	std::unique_ptr<trade_dashboard_nation_selector_flag_t> flag;
	std::unique_ptr<trade_dashboard_nation_selector_name_t> name;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "nid") {
			return (void*)(&nid);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_trade_dashboard_nation_selector(sys::state& state);
void trade_dashboard_main_commodity_list_t::add_commodity_selector(dcon::commodity_id cid) {
	values.emplace_back(commodity_selector_option{cid});
}
void  trade_dashboard_main_commodity_list_t::on_create(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::commodity_list::on_create
	std::vector<dcon::commodity_id> raw;
	state.world.for_each_commodity([&](auto cid) {
		raw.push_back(cid);
	});

	std::sort(raw.begin(), raw.end(), [&](auto a, auto b) {
		std::string a_name = text::produce_simple_string(state, state.world.commodity_get_name(a));
		std::string b_name = text::produce_simple_string(state, state.world.commodity_get_name(b));
		return b_name.compare(a_name) > 0;
	});

	for(auto& a : raw) {
		add_commodity_selector(a);
	}
// END
}
void  trade_dashboard_main_commodity_list_t::update(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::commodity_list::update
// END
}
measure_result  trade_dashboard_main_commodity_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<commodity_selector_option>(values[index])) {
		if(commodity_selector_pool.empty()) commodity_selector_pool.emplace_back(make_trade_dashboard_commodity_selector(state));
		if(destination) {
			if(commodity_selector_pool.size() <= size_t(commodity_selector_pool_used)) commodity_selector_pool.emplace_back(make_trade_dashboard_commodity_selector(state));
			commodity_selector_pool[commodity_selector_pool_used]->base_data.position.x = int16_t(x);
			commodity_selector_pool[commodity_selector_pool_used]->base_data.position.y = int16_t(y);
			commodity_selector_pool[commodity_selector_pool_used]->parent = destination;
			destination->children.push_back(commodity_selector_pool[commodity_selector_pool_used].get());
			((trade_dashboard_commodity_selector_t*)(commodity_selector_pool[commodity_selector_pool_used].get()))->cid = std::get<commodity_selector_option>(values[index]).cid;
			commodity_selector_pool[commodity_selector_pool_used]->impl_on_update(state);
			commodity_selector_pool_used++;
		}
		alternate = true;
		return measure_result{ commodity_selector_pool[0]->base_data.size.x, commodity_selector_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  trade_dashboard_main_commodity_list_t::reset_pools() {
	commodity_selector_pool_used = 0;
}
void trade_dashboard_main_top_production_t::add_province_row_production(dcon::province_id pid) {
	values.emplace_back(province_row_production_option{pid});
}
void  trade_dashboard_main_top_production_t::on_create(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_production::on_create
// END
}
void  trade_dashboard_main_top_production_t::update(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_production::update
	auto size = state.ui_cached_data.commodity_per_province.production_volume.size();
	if(!size.has_value()) {
		return;
	}
	values.clear();
	size_t counter = 0;
	while(counter < size.value() && values.size() < 8) {
		auto pid = state.ui_cached_data.commodity_per_province.sorted_by_production[(int)counter];
		if(pid.has_value()) {
			if(!main.nation_pov || state.world.province_get_nation_from_province_ownership(pid.value()) == main.nation_pov) {
				add_province_row_production(pid.value());
			}
		}
		counter++;
	}
// END
}
measure_result  trade_dashboard_main_top_production_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<province_row_production_option>(values[index])) {
		if(province_row_production_pool.empty()) province_row_production_pool.emplace_back(make_trade_dashboard_province_row_production(state));
		if(destination) {
			if(province_row_production_pool.size() <= size_t(province_row_production_pool_used)) province_row_production_pool.emplace_back(make_trade_dashboard_province_row_production(state));
			province_row_production_pool[province_row_production_pool_used]->base_data.position.x = int16_t(x);
			province_row_production_pool[province_row_production_pool_used]->base_data.position.y = int16_t(y);
			province_row_production_pool[province_row_production_pool_used]->parent = destination;
			destination->children.push_back(province_row_production_pool[province_row_production_pool_used].get());
			((trade_dashboard_province_row_production_t*)(province_row_production_pool[province_row_production_pool_used].get()))->pid = std::get<province_row_production_option>(values[index]).pid;
			((trade_dashboard_province_row_production_t*)(province_row_production_pool[province_row_production_pool_used].get()))->is_active = alternate;
			province_row_production_pool[province_row_production_pool_used]->impl_on_update(state);
			province_row_production_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ province_row_production_pool[0]->base_data.size.x, province_row_production_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  trade_dashboard_main_top_production_t::reset_pools() {
	province_row_production_pool_used = 0;
}
void trade_dashboard_main_top_consumption_t::add_province_row_consumption(dcon::province_id pid) {
	values.emplace_back(province_row_consumption_option{pid});
}
void  trade_dashboard_main_top_consumption_t::on_create(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_consumption::on_create
// END
}
void  trade_dashboard_main_top_consumption_t::update(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_consumption::update
	auto size = state.ui_cached_data.commodity_per_province.sorted_by_consumption.size();
	if(!size.has_value()) {
		return;
	}
	values.clear();
	size_t counter = 0;
	while(counter < size.value() && values.size() < 8) {
		auto pid = state.ui_cached_data.commodity_per_province.sorted_by_consumption[(int)counter];
		if(pid.has_value()) {
			if(!main.nation_pov || state.world.province_get_nation_from_province_ownership(pid.value()) == main.nation_pov) {
				add_province_row_consumption(pid.value());
			}
		}
		counter++;
	}
// END
}
measure_result  trade_dashboard_main_top_consumption_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<province_row_consumption_option>(values[index])) {
		if(province_row_consumption_pool.empty()) province_row_consumption_pool.emplace_back(make_trade_dashboard_province_row_consumption(state));
		if(destination) {
			if(province_row_consumption_pool.size() <= size_t(province_row_consumption_pool_used)) province_row_consumption_pool.emplace_back(make_trade_dashboard_province_row_consumption(state));
			province_row_consumption_pool[province_row_consumption_pool_used]->base_data.position.x = int16_t(x);
			province_row_consumption_pool[province_row_consumption_pool_used]->base_data.position.y = int16_t(y);
			province_row_consumption_pool[province_row_consumption_pool_used]->parent = destination;
			destination->children.push_back(province_row_consumption_pool[province_row_consumption_pool_used].get());
			((trade_dashboard_province_row_consumption_t*)(province_row_consumption_pool[province_row_consumption_pool_used].get()))->pid = std::get<province_row_consumption_option>(values[index]).pid;
			((trade_dashboard_province_row_consumption_t*)(province_row_consumption_pool[province_row_consumption_pool_used].get()))->is_active = alternate;
			province_row_consumption_pool[province_row_consumption_pool_used]->impl_on_update(state);
			province_row_consumption_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ province_row_consumption_pool[0]->base_data.size.x, province_row_consumption_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  trade_dashboard_main_top_consumption_t::reset_pools() {
	province_row_consumption_pool_used = 0;
}
void trade_dashboard_main_nation_list_t::add_nation_selector(dcon::nation_id nid) {
	values.emplace_back(nation_selector_option{nid});
}
void  trade_dashboard_main_nation_list_t::on_create(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::nation_list::on_create
// END
}
void  trade_dashboard_main_nation_list_t::update(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::nation_list::update
	values.clear();
	std::vector<dcon::nation_id> raw;
	state.world.for_each_nation([&](auto nation) {
		if(state.world.nation_get_owned_province_count(nation) > 0) {
			raw.push_back(nation);
		}
	});

	std::sort(raw.begin(), raw.end(), [&](auto a, auto b) {
		std::string a_name = text::produce_simple_string(state, text::get_name(state, a));
		std::string b_name = text::produce_simple_string(state, text::get_name(state, b));
		return b_name.compare(a_name) > 0;
	});

	for(auto n : raw) {
		add_nation_selector(n);
	}
// END
}
measure_result  trade_dashboard_main_nation_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<nation_selector_option>(values[index])) {
		if(nation_selector_pool.empty()) nation_selector_pool.emplace_back(make_trade_dashboard_nation_selector(state));
		if(destination) {
			if(nation_selector_pool.size() <= size_t(nation_selector_pool_used)) nation_selector_pool.emplace_back(make_trade_dashboard_nation_selector(state));
			nation_selector_pool[nation_selector_pool_used]->base_data.position.x = int16_t(x);
			nation_selector_pool[nation_selector_pool_used]->base_data.position.y = int16_t(y);
			nation_selector_pool[nation_selector_pool_used]->parent = destination;
			destination->children.push_back(nation_selector_pool[nation_selector_pool_used].get());
			((trade_dashboard_nation_selector_t*)(nation_selector_pool[nation_selector_pool_used].get()))->nid = std::get<nation_selector_option>(values[index]).nid;
			nation_selector_pool[nation_selector_pool_used]->impl_on_update(state);
			nation_selector_pool_used++;
		}
		alternate = true;
		return measure_result{ nation_selector_pool[0]->base_data.size.x, nation_selector_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  trade_dashboard_main_nation_list_t::reset_pools() {
	nation_selector_pool_used = 0;
}
void trade_dashboard_main_top_gdp_t::add_province_row_gdp(dcon::province_id pid) {
	values.emplace_back(province_row_gdp_option{pid});
}
void  trade_dashboard_main_top_gdp_t::on_create(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_gdp::on_create
// END
}
void  trade_dashboard_main_top_gdp_t::update(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_gdp::update
	auto size = state.ui_cached_data.per_province.gdp.size();
	if(!size.has_value()) {
		return;
	}
	values.clear();
	size_t counter = 0;
	while(counter < size.value() && values.size() < 8) {
		auto pid = state.ui_cached_data.per_province.sorted_by_gdp[(int)counter];
		if(pid.has_value()) {
			if(!main.nation_pov || state.world.province_get_nation_from_province_ownership(pid.value()) == main.nation_pov) {
				add_province_row_gdp(pid.value());
			}
		}
		counter++;
	}
// END
}
measure_result  trade_dashboard_main_top_gdp_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<province_row_gdp_option>(values[index])) {
		if(province_row_gdp_pool.empty()) province_row_gdp_pool.emplace_back(make_trade_dashboard_province_row_gdp(state));
		if(destination) {
			if(province_row_gdp_pool.size() <= size_t(province_row_gdp_pool_used)) province_row_gdp_pool.emplace_back(make_trade_dashboard_province_row_gdp(state));
			province_row_gdp_pool[province_row_gdp_pool_used]->base_data.position.x = int16_t(x);
			province_row_gdp_pool[province_row_gdp_pool_used]->base_data.position.y = int16_t(y);
			province_row_gdp_pool[province_row_gdp_pool_used]->parent = destination;
			destination->children.push_back(province_row_gdp_pool[province_row_gdp_pool_used].get());
			((trade_dashboard_province_row_gdp_t*)(province_row_gdp_pool[province_row_gdp_pool_used].get()))->pid = std::get<province_row_gdp_option>(values[index]).pid;
			((trade_dashboard_province_row_gdp_t*)(province_row_gdp_pool[province_row_gdp_pool_used].get()))->is_active = alternate;
			province_row_gdp_pool[province_row_gdp_pool_used]->impl_on_update(state);
			province_row_gdp_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ province_row_gdp_pool[0]->base_data.size.x, province_row_gdp_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  trade_dashboard_main_top_gdp_t::reset_pools() {
	province_row_gdp_pool_used = 0;
}
void trade_dashboard_main_top_gdp_capita_t::add_province_row_gdp_per_capita(dcon::province_id pid) {
	values.emplace_back(province_row_gdp_per_capita_option{pid});
}
void  trade_dashboard_main_top_gdp_capita_t::on_create(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_gdp_capita::on_create
// END
}
void  trade_dashboard_main_top_gdp_capita_t::update(sys::state& state, layout_window_element* parent) {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::top_gdp_capita::update
	auto size = state.ui_cached_data.per_province.gdp.size();
	if(!size.has_value()) {
		return;
	}
	values.clear();
	size_t counter = 0;
	while(counter < size.value() && values.size() < 8) {
		auto pid = state.ui_cached_data.per_province.sorted_by_gdp[(int)counter];
		if(pid.has_value()) {
			if(!main.nation_pov || state.world.province_get_nation_from_province_ownership(pid.value()) == main.nation_pov) {
				add_province_row_gdp_per_capita(pid.value());
			}
		}
		counter++;
	}
// END
}
measure_result  trade_dashboard_main_top_gdp_capita_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<province_row_gdp_per_capita_option>(values[index])) {
		if(province_row_gdp_per_capita_pool.empty()) province_row_gdp_per_capita_pool.emplace_back(make_trade_dashboard_province_row_gdp_per_capita(state));
		if(destination) {
			if(province_row_gdp_per_capita_pool.size() <= size_t(province_row_gdp_per_capita_pool_used)) province_row_gdp_per_capita_pool.emplace_back(make_trade_dashboard_province_row_gdp_per_capita(state));
			province_row_gdp_per_capita_pool[province_row_gdp_per_capita_pool_used]->base_data.position.x = int16_t(x);
			province_row_gdp_per_capita_pool[province_row_gdp_per_capita_pool_used]->base_data.position.y = int16_t(y);
			province_row_gdp_per_capita_pool[province_row_gdp_per_capita_pool_used]->parent = destination;
			destination->children.push_back(province_row_gdp_per_capita_pool[province_row_gdp_per_capita_pool_used].get());
			((trade_dashboard_province_row_gdp_per_capita_t*)(province_row_gdp_per_capita_pool[province_row_gdp_per_capita_pool_used].get()))->pid = std::get<province_row_gdp_per_capita_option>(values[index]).pid;
			((trade_dashboard_province_row_gdp_per_capita_t*)(province_row_gdp_per_capita_pool[province_row_gdp_per_capita_pool_used].get()))->is_active = alternate;
			province_row_gdp_per_capita_pool[province_row_gdp_per_capita_pool_used]->impl_on_update(state);
			province_row_gdp_per_capita_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ province_row_gdp_per_capita_pool[0]->base_data.size.x, province_row_gdp_per_capita_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  trade_dashboard_main_top_gdp_capita_t::reset_pools() {
	province_row_gdp_per_capita_pool_used = 0;
}
void trade_dashboard_main_per_commodity_details_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::per_commodity_details::update
// END
}
void trade_dashboard_main_per_commodity_details_t::on_create(sys::state& state) noexcept {
// BEGIN main::per_commodity_details::create
// END
}
void trade_dashboard_main_per_nation_details_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::per_nation_details::update
// END
}
void trade_dashboard_main_per_nation_details_t::on_create(sys::state& state) noexcept {
// BEGIN main::per_nation_details::create
// END
}
ui::message_result trade_dashboard_main_nation_import_per_commodity_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_nation_import_per_commodity_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_nation_import_per_commodity_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_nation_import_per_commodity_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::nation_import_per_commodity::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "import"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_unparsed_text_to_layout_box(state, contents, box, text::get_commodity_name_with_icon(state, selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_nation_import_per_commodity_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_nation_import_per_commodity_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::nation_import_per_commodity::update
	auto size = state.ui_cached_data.nation_per_commodity.import_volume.size();
	if(!size.has_value()) return;

	graph_content.clear();
	graph_content.resize(state.world.commodity_size());

	state.world.for_each_commodity([&](auto cid) {
		graph_content[cid.index()].key = cid;
		auto median_price = state.world.commodity_get_median_price(cid);
		if(main.nation_pov) {
			auto value = state.ui_cached_data.nation_per_commodity.import_volume[cid.index()];
			if(value.has_value()) {
				graph_content[cid.index()].amount = value.value()  * median_price;
			} else {
				graph_content[cid.index()].amount = 0.f;
			}
		} else {
			graph_content[cid.index()].amount = 0.f;
		}
		auto c = state.world.commodity_get_color(cid);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[cid.index()].color = c3f;
	});

	update_chart(state);
// END
}
void trade_dashboard_main_nation_import_per_commodity_t::on_create(sys::state& state) noexcept {
// BEGIN main::nation_import_per_commodity::create
// END
}
ui::message_result trade_dashboard_main_nation_export_per_commodity_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_nation_export_per_commodity_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_nation_export_per_commodity_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_nation_export_per_commodity_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::nation_export_per_commodity::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "export"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_unparsed_text_to_layout_box(state, contents, box, text::get_commodity_name_with_icon(state, selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_nation_export_per_commodity_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_nation_export_per_commodity_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::nation_export_per_commodity::update
	graph_content.clear();
	graph_content.resize(state.world.commodity_size());

	state.world.for_each_commodity([&](auto cid) {
		graph_content[cid.index()].key = cid;
		auto median_price = state.world.commodity_get_median_price(cid);
		if(main.nation_pov) {
			graph_content[cid.index()].amount = state.ui_cached_data.nation_per_commodity.export_volume[cid.index()].value_or(0.f) * median_price;
		} else {
			graph_content[cid.index()].amount = 0.f;
		}
		auto c = state.world.commodity_get_color(cid);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[cid.index()].color = c3f;
	});

	update_chart(state);
// END
}
void trade_dashboard_main_nation_export_per_commodity_t::on_create(sys::state& state) noexcept {
// BEGIN main::nation_export_per_commodity::create
// END
}
ui::message_result trade_dashboard_main_nation_import_per_nation_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_nation_import_per_nation_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_nation_import_per_nation_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_nation_import_per_nation_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::nation_import_per_nation::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "import"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::prettify_currency(graph_content[temp_index].amount));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_nation_import_per_nation_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_nation_import_per_nation_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent));
// BEGIN main::nation_import_per_nation::update
	graph_content.clear();
	state.world.for_each_nation([&](auto target) {
		if(main.nation_pov && target != main.nation_pov) {
			auto c = state.world.nation_get_color(target);
			graph_entry entry = {
				target,
				ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) },
				state.ui_cached_data.nation_per_nation.import_value[target.index()].value_or(0.f)
			};
			graph_content.push_back(entry);
		}
	});
	update_chart(state);
// END
}
void trade_dashboard_main_nation_import_per_nation_t::on_create(sys::state& state) noexcept {
// BEGIN main::nation_import_per_nation::create
// END
}
ui::message_result trade_dashboard_main_nation_export_per_nation_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_nation_export_per_nation_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_nation_export_per_nation_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_nation_export_per_nation_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::nation_export_per_nation::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "export"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::prettify_currency(graph_content[temp_index].amount));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_nation_export_per_nation_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_nation_export_per_nation_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::nation_export_per_nation::update
	graph_content.clear();
	state.world.for_each_nation([&](auto target) {
		if(main.nation_pov && target != main.nation_pov) {
			auto c = state.world.nation_get_color(target);
			graph_entry entry = {
				target,
				ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) },
				state.ui_cached_data.nation_per_nation.export_value[target.index()].value_or(0.f)
			};
			graph_content.push_back(entry);
		}
	});
	update_chart(state);
// END
}
void trade_dashboard_main_nation_export_per_nation_t::on_create(sys::state& state) noexcept {
// BEGIN main::nation_export_per_nation::create
// END
}
ui::message_result trade_dashboard_main_icon_import_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_import_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_import_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_import_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_import::update
// END
}
void trade_dashboard_main_icon_import_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_import::create
// END
}
ui::message_result trade_dashboard_main_icon_export_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_export_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_export_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_export_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_export::update
// END
}
void trade_dashboard_main_icon_export_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_export::create
// END
}
void trade_dashboard_main_label_gdp_top_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_main_label_gdp_top_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void trade_dashboard_main_label_gdp_top_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_label_gdp_top_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::label_gdp_top::update
// END
}
void trade_dashboard_main_label_gdp_top_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::label_gdp_top::create
// END
}
void trade_dashboard_main_label_gdp_capita_top_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_main_label_gdp_capita_top_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void trade_dashboard_main_label_gdp_capita_top_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_label_gdp_capita_top_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::label_gdp_capita_top::update
// END
}
void trade_dashboard_main_label_gdp_capita_top_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::label_gdp_capita_top::create
// END
}
ui::message_result trade_dashboard_main_commodity_import_per_nation_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_commodity_import_per_nation_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_commodity_import_per_nation_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_commodity_import_per_nation_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::commodity_import_per_nation::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "import"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::prettify_currency(graph_content[temp_index].amount));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_commodity_import_per_nation_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_commodity_import_per_nation_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::commodity_import_per_nation::update
	graph_content.clear();
	graph_content.resize(state.world.nation_size());
	auto cid = state.selected_trade_good;
	auto median_price = state.world.commodity_get_median_price(cid);
	state.world.for_each_nation([&](auto nid) {
		graph_content[nid.index()].key = nid;
		graph_content[nid.index()].amount = state.ui_cached_data.commodity_per_nation.import_volume[nid.index()].value_or(0.f) * median_price;
		auto c = state.world.nation_get_color(nid);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[nid.index()].color = c3f;
	});
	update_chart(state);
// END
}
void trade_dashboard_main_commodity_import_per_nation_t::on_create(sys::state& state) noexcept {
// BEGIN main::commodity_import_per_nation::create
// END
}
ui::message_result trade_dashboard_main_commodity_export_per_nation_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_commodity_export_per_nation_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_commodity_export_per_nation_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_commodity_export_per_nation_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::commodity_export_per_nation::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "export"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::prettify_currency( graph_content[temp_index].amount ));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_commodity_export_per_nation_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_commodity_export_per_nation_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::commodity_export_per_nation::update
	graph_content.clear();
	graph_content.resize(state.world.nation_size());

	auto cid = state.selected_trade_good;
	auto median_price = state.world.commodity_get_median_price(cid);

	state.world.for_each_nation([&](auto nid) {
		graph_content[nid.index()].key = nid;
		graph_content[nid.index()].amount = state.ui_cached_data.commodity_per_nation.export_volume[nid.index()].value_or(0.f) * median_price;
		auto c = state.world.nation_get_color(nid);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[nid.index()].color = c3f;
	});

	update_chart(state);
// END
}
void trade_dashboard_main_commodity_export_per_nation_t::on_create(sys::state& state) noexcept {
// BEGIN main::commodity_export_per_nation::create
// END
}
ui::message_result trade_dashboard_main_commodity_production_per_nation_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_commodity_production_per_nation_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_commodity_production_per_nation_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_commodity_production_per_nation_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::commodity_production_per_nation::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "production"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::prettify_currency(graph_content[temp_index].amount));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_commodity_production_per_nation_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_commodity_production_per_nation_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::commodity_production_per_nation::update
	graph_content.clear();
	graph_content.resize(state.world.nation_size());

	auto cid = state.selected_trade_good;
	auto median_price = state.world.commodity_get_median_price(cid);

	state.world.for_each_nation([&](auto nid) {
		graph_content[nid.index()].key = nid;
		graph_content[nid.index()].amount = state.ui_cached_data.commodity_per_nation.production_volume[nid.index()].value_or(0.f) * median_price;
		auto c = state.world.nation_get_color(nid);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[nid.index()].color = c3f;
	});

	update_chart(state);
// END
}
void trade_dashboard_main_commodity_production_per_nation_t::on_create(sys::state& state) noexcept {
// BEGIN main::commodity_production_per_nation::create
// END
}
ui::message_result trade_dashboard_main_commodity_consumption_per_nation_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_commodity_consumption_per_nation_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_commodity_consumption_per_nation_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_commodity_consumption_per_nation_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x) / float(base_data.size.x);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::commodity_consumption_per_nation::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "consumption"));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::prettify_currency(graph_content[temp_index].amount));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_commodity_consumption_per_nation_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_stripchart(state, ogl::color_modification::none, float(x), float(y), float(base_data.size.x), float(base_data.size.y), data_texture);
}
void trade_dashboard_main_commodity_consumption_per_nation_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::commodity_consumption_per_nation::update
	graph_content.clear();
	graph_content.resize(state.world.nation_size());

	auto cid = state.selected_trade_good;
	auto median_price = state.world.commodity_get_median_price(cid);

	state.world.for_each_nation([&](auto nid) {
		graph_content[nid.index()].key = nid;
		graph_content[nid.index()].amount = state.ui_cached_data.commodity_per_nation.consumption_volume[nid.index()].value_or(0.f) * median_price;
		auto c = state.world.nation_get_color(nid);
		auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
		graph_content[nid.index()].color = c3f;
	});

	update_chart(state);
// END
}
void trade_dashboard_main_commodity_consumption_per_nation_t::on_create(sys::state& state) noexcept {
// BEGIN main::commodity_consumption_per_nation::create
// END
}
void trade_dashboard_main_label_production_top_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_main_label_production_top_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void trade_dashboard_main_label_production_top_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_label_production_top_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::label_production_top::update
// END
}
void trade_dashboard_main_label_production_top_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::label_production_top::create
// END
}
void trade_dashboard_main_label_consumption_top_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_main_label_consumption_top_t::on_reset_text(sys::state& state) noexcept {
	cached_text = text::produce_simple_string(state, text_key);
	internal_layout.contents.clear();
	internal_layout.number_of_lines = 0;
	text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cached_text);
}
void trade_dashboard_main_label_consumption_top_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_label_consumption_top_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::label_consumption_top::update
// END
}
void trade_dashboard_main_label_consumption_top_t::on_create(sys::state& state) noexcept {
	on_reset_text(state);
// BEGIN main::label_consumption_top::create
// END
}
ui::message_result trade_dashboard_main_gdp_per_sector_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_gdp_per_sector_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_gdp_per_sector_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(100)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_gdp_per_sector_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float x_normal = float(x) / float(base_data.size.x) * 2.f - 1.f;
	float y_normal = float(y) / float(base_data.size.y) * 2.f - 1.f;
	float temp_offset = temp_total * (std::atan2(-y_normal, -x_normal) / std::numbers::pi_v<float> / 2.f + 0.5f);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::gdp_per_sector::tooltip
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
void trade_dashboard_main_gdp_per_sector_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_ui_mesh(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, mesh, data_texture);
}
void trade_dashboard_main_gdp_per_sector_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::gdp_per_sector::update
	graph_content.resize(3);

	graph_content[0].amount = 0.f;
	graph_content[1].amount = 0.f;
	graph_content[2].amount = 0.f;

	state.world.for_each_province([&](auto pid) {
		if(main.nation_pov) {
			if(state.world.province_get_nation_from_province_ownership(pid) != main.nation_pov) {
				return;
			}
		}
		auto local_gdp = state.ui_cached_data.per_province.gdp[pid.index()];
		if(local_gdp.has_value()) {
			graph_content[0].amount += std::max(0.f, std::max(0.f, local_gdp.value().primary));
			graph_content[1].amount += std::max(0.f, std::max(0.f, local_gdp.value().secondary_factory));
			graph_content[2].amount += std::max(0.f, std::max(0.f, local_gdp.value().secondary_artisan));
		}
	});

	graph_content[0].color = { 1.f, 0.f, 0.f };
	graph_content[0].key = 0;
	graph_content[1].color = { 0.f, 1.f, 0.f };
	graph_content[1].key = 1;
	graph_content[2].color = { 0.f, 0.f, 1.f };
	graph_content[2].key = 2;

	update_chart(state);
// END
}
void trade_dashboard_main_gdp_per_sector_t::on_create(sys::state& state) noexcept {
// BEGIN main::gdp_per_sector::create
// END
}
ui::message_result trade_dashboard_main_circle_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_circle_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_circle_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_circle_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::circle::update
// END
}
void trade_dashboard_main_circle_t::on_create(sys::state& state) noexcept {
// BEGIN main::circle::create
// END
}
ui::message_result trade_dashboard_main_gdp_sphere_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_gdp_sphere_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_gdp_sphere_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(300)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_gdp_sphere_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float x_normal = float(x) / float(base_data.size.x) * 2.f - 1.f;
	float y_normal = float(y) / float(base_data.size.y) * 2.f - 1.f;
	float temp_offset = temp_total * (std::atan2(-y_normal, -x_normal) / std::numbers::pi_v<float> / 2.f + 0.5f);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::gdp_sphere::tooltip
		auto box = text::open_layout_box(contents);
		auto temp = selected_key;
		auto sphere = state.world.nation_get_in_sphere_of(temp);
		while(!sphere) {
			auto overlord = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(temp));
			if(overlord) {
				temp = overlord;
				sphere = state.world.nation_get_in_sphere_of(temp);
			} else {
				break;
			}
		}
		if(sphere) {
			text::add_to_layout_box(state, contents, box, text::get_adjective_as_string(state, sphere));
		} else if (temp != selected_key) {
			text::add_to_layout_box(state, contents, box, text::get_adjective_as_string(state, temp));
		}
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "gva"));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_gdp_sphere_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_ui_mesh(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, mesh, data_texture);
	auto time = std::chrono::steady_clock::now();

}
void trade_dashboard_main_gdp_sphere_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent));
// BEGIN main::gdp_sphere::update
	if(graph_content.size() != state.world.nation_size()) {
		graph_content.clear();
	}
	if(graph_content.size() == 0) {
		state.world.for_each_nation([&](auto nid) {
			auto c = state.world.nation_get_color(nid);
			auto c3f = ogl::color3f{ sys::red_from_int(c), sys::green_from_int(c), sys::blue_from_int(c) };
			graph_content.push_back({
				nid, c3f, 0.f
			});
		});
	}
	for(auto& item : graph_content) {
		item.amount = state.ui_cached_data.per_nation.sphere_gdp[item.key.index()].value_or(0.f);
	}
	std::sort(graph_content.begin(), graph_content.end(), [](auto const& a, auto const& b) {
		if(a.amount != b.amount) {
			return a.amount > b.amount;
		} else {
			return a.key.index() > b.key.index();
		}
	});
	update_chart(state);
// END
}
void trade_dashboard_main_gdp_sphere_t::on_create(sys::state& state) noexcept {
// BEGIN main::gdp_sphere::create
// END
}
ui::message_result trade_dashboard_main_gdp_nations_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_gdp_nations_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_gdp_nations_t::update_chart(sys::state& state) {
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
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(300)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void trade_dashboard_main_gdp_nations_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float x_normal = float(x) / float(base_data.size.x) * 2.f - 1.f;
	float y_normal = float(y) / float(base_data.size.y) * 2.f - 1.f;
	float temp_offset = temp_total * (std::atan2(-y_normal, -x_normal) / std::numbers::pi_v<float> / 2.f + 0.5f);
	int32_t temp_index = 0;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::gdp_nations::tooltip
		auto box = text::open_layout_box(contents);
		auto temp = selected_key;
		auto sphere = state.world.nation_get_in_sphere_of(temp);
		while(!sphere) {
			auto overlord = state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(temp));
			if(overlord) {
				temp = overlord;
				sphere = state.world.nation_get_in_sphere_of(temp);
			} else {
				break;
			}
		}
		if(sphere) {
			text::add_to_layout_box(state, contents, box, text::get_adjective_as_string(state, sphere));
		} else if(temp != selected_key) {
			text::add_to_layout_box(state, contents, box, text::get_adjective_as_string(state, temp));
		}
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, selected_key);
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_currency{ graph_content[temp_index].amount });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "gva"));
		text::close_layout_box(contents, box);
// END
	}
}
void trade_dashboard_main_gdp_nations_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_ui_mesh(state, ogl::color_modification::none, float(x), float(y), base_data.size.x, base_data.size.y, mesh, data_texture);
}
void trade_dashboard_main_gdp_nations_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::gdp_nations::update
	if(graph_content.size() != state.world.nation_size()) {
		graph_content.clear();
	}

	if(graph_content.size() == 0) {
		state.world.for_each_nation([&](auto nid) {
			auto parent = state.ui_cached_data.per_nation.sphere_parent[nid.index()].value_or(nid);
			auto c = state.world.nation_get_color(nid);
			auto pc = state.world.nation_get_color(parent);
			auto c3f = ogl::color3f{
				sys::red_from_int(pc) * 0.6f + sys::red_from_int(c) * 0.1f,
				sys::green_from_int(pc) * 0.6f + sys::green_from_int(c) * 0.1f,
				sys::blue_from_int(pc) * 0.6f + sys::blue_from_int(c) * 0.1f
			};
			graph_content.push_back({
				nid,
				c3f,
				0.f
			});
		});
	}

	for(auto& item : graph_content) {
		auto parent = state.ui_cached_data.per_nation.sphere_parent[item.key.index()].value_or(item.key);
		auto c = state.world.nation_get_color(item.key);
		auto pc = state.world.nation_get_color(parent);
		auto c3f = ogl::color3f{
			sys::red_from_int(pc) * 0.6f + sys::red_from_int(c) * 0.1f,
			sys::green_from_int(pc) * 0.6f + sys::green_from_int(c) * 0.1f,
			sys::blue_from_int(pc) * 0.6f + sys::blue_from_int(c) * 0.1f
		};
		item.amount = state.ui_cached_data.per_nation.national_gdp[item.key.index()].value_or(0.f);
		item.color = c3f;
	}

	std::sort(graph_content.begin(), graph_content.end(), [&](auto const& a, auto const& b) {
		auto parent_a = state.ui_cached_data.per_nation.sphere_parent[a.key.index()].value_or(a.key);
		auto parent_b = state.ui_cached_data.per_nation.sphere_parent[b.key.index()].value_or(b.key);

		if(parent_a == parent_b) {
			if(a.amount != b.amount) {
				return a.amount > b.amount;
			} else {
				return a.key.index() > b.key.index();
			}
		} else {
			float value_a = 0.f;
			float value_b = 0.f;

			if(parent_a) {
				value_a = state.ui_cached_data.per_nation.sphere_gdp[parent_a.index()].value_or(0.f);
			}

			if(parent_b) {
				value_b = state.ui_cached_data.per_nation.sphere_gdp[parent_b.index()].value_or(0.f);
			}

			if(value_a != value_b) {
				return value_a > value_b;
			} else {
				return a.key.index() > b.key.index();
			}
		}
	});
	update_chart(state);
// END
}
void trade_dashboard_main_gdp_nations_t::on_create(sys::state& state) noexcept {
// BEGIN main::gdp_nations::create
// END
}
ui::message_result trade_dashboard_main_icon_import_2_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_import_2_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_import_2_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_import_2_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_import_2::update
// END
}
void trade_dashboard_main_icon_import_2_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_import_2::create
// END
}
ui::message_result trade_dashboard_main_icon_import_3_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_import_3_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_import_3_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_import_3_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_import_3::update
// END
}
void trade_dashboard_main_icon_import_3_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_import_3::create
// END
}
ui::message_result trade_dashboard_main_icon_export_2_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_export_2_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_export_2_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_export_2_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_export_2::update
// END
}
void trade_dashboard_main_icon_export_2_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_export_2::create
// END
}
ui::message_result trade_dashboard_main_icon_export_3_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_export_3_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_export_3_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_export_3_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_export_3::update
// END
}
void trade_dashboard_main_icon_export_3_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_export_3::create
// END
}
ui::message_result trade_dashboard_main_close_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::close::lbutton_action
	main.set_visible(state, false);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_main_close_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_main_close_t::on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	if(key == sys::virtual_key::ESCAPE) {
		on_lbutton_down(state, 0, 0, mods);
		return ui::message_result::consumed;
	}
	return ui::message_result::unseen;
}
void trade_dashboard_main_close_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_close_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::close::update
// END
}
void trade_dashboard_main_close_t::on_create(sys::state& state) noexcept {
// BEGIN main::close::create
// END
}
ui::message_result trade_dashboard_main_select_self_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::select_self::lbutton_action
	if(main.nation_pov == state.local_player_nation) {
		main.nation_pov = { };
	} else {
		main.nation_pov = state.local_player_nation;
		state.ui_cached_data.set_nation(state, state.local_player_nation);
	}
	main.on_update(state);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_main_select_self_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_main_select_self_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void trade_dashboard_main_select_self_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_select_self_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::select_self::update
	flag = state.local_player_nation;
// END
}
void trade_dashboard_main_select_self_t::on_create(sys::state& state) noexcept {
// BEGIN main::select_self::create
// END
}
ui::message_result trade_dashboard_main_icon_consumption_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_consumption_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_consumption_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_consumption_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_consumption::update
// END
}
void trade_dashboard_main_icon_consumption_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_consumption::create
// END
}
ui::message_result trade_dashboard_main_icon_production_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result trade_dashboard_main_icon_production_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_icon_production_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_icon_production_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::icon_production::update
// END
}
void trade_dashboard_main_icon_production_t::on_create(sys::state& state) noexcept {
// BEGIN main::icon_production::create
// END
}
ui::message_result trade_dashboard_main_old_window_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN main::old_window::lbutton_action
	main.old_window_container->set_visible(state, !main.old_window_container->is_visible());
	main.old_window_container->parent->move_child_to_front(main.old_window_container);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_main_old_window_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_main_old_window_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void trade_dashboard_main_old_window_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::old_window::update
// END
}
void trade_dashboard_main_old_window_t::on_create(sys::state& state) noexcept {
// BEGIN main::old_window::create
// END
}
void trade_dashboard_main_selected_commodity_top_provinces_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_main_selected_commodity_top_provinces_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_main_selected_commodity_top_provinces_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_selected_commodity_top_provinces_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::selected_commodity_top_provinces::update
	if(state.selected_trade_good) {
		set_text(state, text::get_commodity_text_icon(state, state.selected_trade_good));
	} else {
		set_text(state, "");
	}
// END
}
void trade_dashboard_main_selected_commodity_top_provinces_t::on_create(sys::state& state) noexcept {
// BEGIN main::selected_commodity_top_provinces::create
// END
}
void trade_dashboard_main_selected_commodity_breakdown_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_main_selected_commodity_breakdown_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_main_selected_commodity_breakdown_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_selected_commodity_breakdown_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::selected_commodity_breakdown::update
	if(state.selected_trade_good) {
		set_text(state, text::get_commodity_text_icon(state, state.selected_trade_good));
	} else {
		set_text(state, "");
	}
// END
}
void trade_dashboard_main_selected_commodity_breakdown_t::on_create(sys::state& state) noexcept {
// BEGIN main::selected_commodity_breakdown::create
// END
}
ui::message_result trade_dashboard_main_selected_nation_breakdown_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
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
ui::message_result trade_dashboard_main_selected_nation_breakdown_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void trade_dashboard_main_selected_nation_breakdown_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void trade_dashboard_main_selected_nation_breakdown_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_main_selected_nation_breakdown_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent)); 
// BEGIN main::selected_nation_breakdown::update
	flag = main.nation_pov;
// END
}
void trade_dashboard_main_selected_nation_breakdown_t::on_create(sys::state& state) noexcept {
// BEGIN main::selected_nation_breakdown::create
// END
}
ui::message_result trade_dashboard_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_main_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void trade_dashboard_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	commodity_list.update(state, this);
	top_production.update(state, this);
	top_consumption.update(state, this);
	nation_list.update(state, this);
	top_gdp.update(state, this);
	top_gdp_capita.update(state, this);
	remake_layout(state, true);
}
void trade_dashboard_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "per_commodity_details") {
					temp.ptr = per_commodity_details.get();
				}
				if(cname == "per_nation_details") {
					temp.ptr = per_nation_details.get();
				}
				if(cname == "nation_import_per_commodity") {
					temp.ptr = nation_import_per_commodity.get();
				}
				if(cname == "nation_export_per_commodity") {
					temp.ptr = nation_export_per_commodity.get();
				}
				if(cname == "nation_import_per_nation") {
					temp.ptr = nation_import_per_nation.get();
				}
				if(cname == "nation_export_per_nation") {
					temp.ptr = nation_export_per_nation.get();
				}
				if(cname == "icon_import") {
					temp.ptr = icon_import.get();
				}
				if(cname == "icon_export") {
					temp.ptr = icon_export.get();
				}
				if(cname == "label_gdp_top") {
					temp.ptr = label_gdp_top.get();
				}
				if(cname == "label_gdp_capita_top") {
					temp.ptr = label_gdp_capita_top.get();
				}
				if(cname == "commodity_import_per_nation") {
					temp.ptr = commodity_import_per_nation.get();
				}
				if(cname == "commodity_export_per_nation") {
					temp.ptr = commodity_export_per_nation.get();
				}
				if(cname == "commodity_production_per_nation") {
					temp.ptr = commodity_production_per_nation.get();
				}
				if(cname == "commodity_consumption_per_nation") {
					temp.ptr = commodity_consumption_per_nation.get();
				}
				if(cname == "label_production_top") {
					temp.ptr = label_production_top.get();
				}
				if(cname == "label_consumption_top") {
					temp.ptr = label_consumption_top.get();
				}
				if(cname == "gdp_per_sector") {
					temp.ptr = gdp_per_sector.get();
				}
				if(cname == "circle") {
					temp.ptr = circle.get();
				}
				if(cname == "gdp_sphere") {
					temp.ptr = gdp_sphere.get();
				}
				if(cname == "gdp_nations") {
					temp.ptr = gdp_nations.get();
				}
				if(cname == "icon_import_2") {
					temp.ptr = icon_import_2.get();
				}
				if(cname == "icon_import_3") {
					temp.ptr = icon_import_3.get();
				}
				if(cname == "icon_export_2") {
					temp.ptr = icon_export_2.get();
				}
				if(cname == "icon_export_3") {
					temp.ptr = icon_export_3.get();
				}
				if(cname == "close") {
					temp.ptr = close.get();
				}
				if(cname == "select_self") {
					temp.ptr = select_self.get();
				}
				if(cname == "icon_consumption") {
					temp.ptr = icon_consumption.get();
				}
				if(cname == "icon_production") {
					temp.ptr = icon_production.get();
				}
				if(cname == "old_window") {
					temp.ptr = old_window.get();
				}
				if(cname == "selected_commodity_top_provinces") {
					temp.ptr = selected_commodity_top_provinces.get();
				}
				if(cname == "selected_commodity_breakdown") {
					temp.ptr = selected_commodity_breakdown.get();
				}
				if(cname == "selected_nation_breakdown") {
					temp.ptr = selected_nation_breakdown.get();
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
					temp.ptr = make_trade_dashboard_main(state);
				}
				if(cname == "province_row_consumption") {
					temp.ptr = make_trade_dashboard_province_row_consumption(state);
				}
				if(cname == "province_row_production") {
					temp.ptr = make_trade_dashboard_province_row_production(state);
				}
				if(cname == "province_row_gdp") {
					temp.ptr = make_trade_dashboard_province_row_gdp(state);
				}
				if(cname == "province_row_gdp_per_capita") {
					temp.ptr = make_trade_dashboard_province_row_gdp_per_capita(state);
				}
				if(cname == "commodity_selector") {
					temp.ptr = make_trade_dashboard_commodity_selector(state);
				}
				if(cname == "nation_selector") {
					temp.ptr = make_trade_dashboard_nation_selector(state);
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
				if(cname == "commodity_list") {
					temp.generator = &commodity_list;
				}
				if(cname == "top_production") {
					temp.generator = &top_production;
				}
				if(cname == "top_consumption") {
					temp.generator = &top_consumption;
				}
				if(cname == "nation_list") {
					temp.generator = &nation_list;
				}
				if(cname == "top_gdp") {
					temp.generator = &top_gdp;
				}
				if(cname == "top_gdp_capita") {
					temp.generator = &top_gdp_capita;
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
void trade_dashboard_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("trade_dashboard::main"));
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
		if(child_data.name == "per_commodity_details") {
			per_commodity_details = std::make_unique<trade_dashboard_main_per_commodity_details_t>();
			per_commodity_details->parent = this;
			auto cptr = per_commodity_details.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "per_nation_details") {
			per_nation_details = std::make_unique<trade_dashboard_main_per_nation_details_t>();
			per_nation_details->parent = this;
			auto cptr = per_nation_details.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "nation_import_per_commodity") {
			nation_import_per_commodity = std::make_unique<trade_dashboard_main_nation_import_per_commodity_t>();
			nation_import_per_commodity->parent = this;
			auto cptr = nation_import_per_commodity.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "nation_export_per_commodity") {
			nation_export_per_commodity = std::make_unique<trade_dashboard_main_nation_export_per_commodity_t>();
			nation_export_per_commodity->parent = this;
			auto cptr = nation_export_per_commodity.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "nation_import_per_nation") {
			nation_import_per_nation = std::make_unique<trade_dashboard_main_nation_import_per_nation_t>();
			nation_import_per_nation->parent = this;
			auto cptr = nation_import_per_nation.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "nation_export_per_nation") {
			nation_export_per_nation = std::make_unique<trade_dashboard_main_nation_export_per_nation_t>();
			nation_export_per_nation->parent = this;
			auto cptr = nation_export_per_nation.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "icon_import") {
			icon_import = std::make_unique<trade_dashboard_main_icon_import_t>();
			icon_import->parent = this;
			auto cptr = icon_import.get();
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
		if(child_data.name == "icon_export") {
			icon_export = std::make_unique<trade_dashboard_main_icon_export_t>();
			icon_export->parent = this;
			auto cptr = icon_export.get();
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
		if(child_data.name == "label_gdp_top") {
			label_gdp_top = std::make_unique<trade_dashboard_main_label_gdp_top_t>();
			label_gdp_top->parent = this;
			auto cptr = label_gdp_top.get();
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
		if(child_data.name == "label_gdp_capita_top") {
			label_gdp_capita_top = std::make_unique<trade_dashboard_main_label_gdp_capita_top_t>();
			label_gdp_capita_top->parent = this;
			auto cptr = label_gdp_capita_top.get();
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
		if(child_data.name == "commodity_import_per_nation") {
			commodity_import_per_nation = std::make_unique<trade_dashboard_main_commodity_import_per_nation_t>();
			commodity_import_per_nation->parent = this;
			auto cptr = commodity_import_per_nation.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "commodity_export_per_nation") {
			commodity_export_per_nation = std::make_unique<trade_dashboard_main_commodity_export_per_nation_t>();
			commodity_export_per_nation->parent = this;
			auto cptr = commodity_export_per_nation.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "commodity_production_per_nation") {
			commodity_production_per_nation = std::make_unique<trade_dashboard_main_commodity_production_per_nation_t>();
			commodity_production_per_nation->parent = this;
			auto cptr = commodity_production_per_nation.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "commodity_consumption_per_nation") {
			commodity_consumption_per_nation = std::make_unique<trade_dashboard_main_commodity_consumption_per_nation_t>();
			commodity_consumption_per_nation->parent = this;
			auto cptr = commodity_consumption_per_nation.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "label_production_top") {
			label_production_top = std::make_unique<trade_dashboard_main_label_production_top_t>();
			label_production_top->parent = this;
			auto cptr = label_production_top.get();
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
		if(child_data.name == "label_consumption_top") {
			label_consumption_top = std::make_unique<trade_dashboard_main_label_consumption_top_t>();
			label_consumption_top->parent = this;
			auto cptr = label_consumption_top.get();
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
		if(child_data.name == "gdp_per_sector") {
			gdp_per_sector = std::make_unique<trade_dashboard_main_gdp_per_sector_t>();
			gdp_per_sector->parent = this;
			auto cptr = gdp_per_sector.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "circle") {
			circle = std::make_unique<trade_dashboard_main_circle_t>();
			circle->parent = this;
			auto cptr = circle.get();
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
		if(child_data.name == "gdp_sphere") {
			gdp_sphere = std::make_unique<trade_dashboard_main_gdp_sphere_t>();
			gdp_sphere->parent = this;
			auto cptr = gdp_sphere.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "gdp_nations") {
			gdp_nations = std::make_unique<trade_dashboard_main_gdp_nations_t>();
			gdp_nations->parent = this;
			auto cptr = gdp_nations.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "icon_import_2") {
			icon_import_2 = std::make_unique<trade_dashboard_main_icon_import_2_t>();
			icon_import_2->parent = this;
			auto cptr = icon_import_2.get();
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
		if(child_data.name == "icon_import_3") {
			icon_import_3 = std::make_unique<trade_dashboard_main_icon_import_3_t>();
			icon_import_3->parent = this;
			auto cptr = icon_import_3.get();
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
		if(child_data.name == "icon_export_2") {
			icon_export_2 = std::make_unique<trade_dashboard_main_icon_export_2_t>();
			icon_export_2->parent = this;
			auto cptr = icon_export_2.get();
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
		if(child_data.name == "icon_export_3") {
			icon_export_3 = std::make_unique<trade_dashboard_main_icon_export_3_t>();
			icon_export_3->parent = this;
			auto cptr = icon_export_3.get();
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
		if(child_data.name == "close") {
			close = std::make_unique<trade_dashboard_main_close_t>();
			close->parent = this;
			auto cptr = close.get();
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
		if(child_data.name == "select_self") {
			select_self = std::make_unique<trade_dashboard_main_select_self_t>();
			select_self->parent = this;
			auto cptr = select_self.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "icon_consumption") {
			icon_consumption = std::make_unique<trade_dashboard_main_icon_consumption_t>();
			icon_consumption->parent = this;
			auto cptr = icon_consumption.get();
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
		if(child_data.name == "icon_production") {
			icon_production = std::make_unique<trade_dashboard_main_icon_production_t>();
			icon_production->parent = this;
			auto cptr = icon_production.get();
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
		if(child_data.name == "old_window") {
			old_window = std::make_unique<trade_dashboard_main_old_window_t>();
			old_window->parent = this;
			auto cptr = old_window.get();
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
		if(child_data.name == "selected_commodity_top_provinces") {
			selected_commodity_top_provinces = std::make_unique<trade_dashboard_main_selected_commodity_top_provinces_t>();
			selected_commodity_top_provinces->parent = this;
			auto cptr = selected_commodity_top_provinces.get();
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
		if(child_data.name == "selected_commodity_breakdown") {
			selected_commodity_breakdown = std::make_unique<trade_dashboard_main_selected_commodity_breakdown_t>();
			selected_commodity_breakdown->parent = this;
			auto cptr = selected_commodity_breakdown.get();
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
		if(child_data.name == "selected_nation_breakdown") {
			selected_nation_breakdown = std::make_unique<trade_dashboard_main_selected_nation_breakdown_t>();
			selected_nation_breakdown->parent = this;
			auto cptr = selected_nation_breakdown.get();
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
	commodity_list.on_create(state, this);
	top_production.on_create(state, this);
	top_consumption.on_create(state, this);
	nation_list.on_create(state, this);
	top_gdp.on_create(state, this);
	top_gdp_capita.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
	auto it = state.ui_state.defs_by_name.find(state.lookup_key("alice_country_trade"));
	if(it != state.ui_state.defs_by_name.end()) {
		auto uptr = std::make_unique<ui::trade_window>();
		uptr->parent = this;
		auto cptr = uptr.get();
		std::memcpy(&(cptr->base_data), &(state.ui_defs.gui[it->second.definition]), sizeof(ui::element_data));
		make_size_from_graphics(state, cptr->base_data);
		cptr->on_create(state);
		cptr->set_visible(state, false);
		old_window_container = cptr;
		state.ui_state.root->add_child_to_back(std::move(uptr));
	}
// END
}
std::unique_ptr<ui::element_base> make_trade_dashboard_main(sys::state& state) {
	auto ptr = std::make_unique<trade_dashboard_main_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result trade_dashboard_province_row_consumption_flag_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_province_row_consumption_t& province_row_consumption = *((trade_dashboard_province_row_consumption_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN province_row_consumption::flag::lbutton_action
	main.nation_pov = state.world.province_get_nation_from_province_ownership(province_row_consumption.pid);
	state.ui_cached_data.set_nation(state, main.nation_pov);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_consumption_flag_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_consumption_flag_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void trade_dashboard_province_row_consumption_flag_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_consumption_flag_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_consumption_t& province_row_consumption = *((trade_dashboard_province_row_consumption_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_consumption::flag::update
	flag = state.world.province_get_nation_from_province_ownership(province_row_consumption.pid);
// END
}
void trade_dashboard_province_row_consumption_flag_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_consumption::flag::create
// END
}
void trade_dashboard_province_row_consumption_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_consumption_name_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_consumption_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_consumption_name_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_consumption_t& province_row_consumption = *((trade_dashboard_province_row_consumption_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_consumption::name::update
	set_text(state, text::produce_simple_string(state, state.world.province_get_name(province_row_consumption.pid)));
// END
}
void trade_dashboard_province_row_consumption_name_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_consumption::name::create
// END
}
void trade_dashboard_province_row_consumption_value_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_consumption_value_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_consumption_value_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_consumption_value_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_consumption_t& province_row_consumption = *((trade_dashboard_province_row_consumption_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_consumption::value::update
	set_text(state, text::prettify_float(state.ui_cached_data.commodity_per_province.consumption_volume[province_row_consumption.pid.index()].value_or(0.f)));
// END
}
void trade_dashboard_province_row_consumption_value_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_consumption::value::create
// END
}
ui::message_result trade_dashboard_province_row_consumption_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_consumption_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_consumption_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void trade_dashboard_province_row_consumption_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_consumption::update
// END
	remake_layout(state, true);
}
void trade_dashboard_province_row_consumption_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "flag") {
					temp.ptr = flag.get();
				}
				if(cname == "name") {
					temp.ptr = name.get();
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
				if(cname == "main") {
					temp.ptr = make_trade_dashboard_main(state);
				}
				if(cname == "province_row_consumption") {
					temp.ptr = make_trade_dashboard_province_row_consumption(state);
				}
				if(cname == "province_row_production") {
					temp.ptr = make_trade_dashboard_province_row_production(state);
				}
				if(cname == "province_row_gdp") {
					temp.ptr = make_trade_dashboard_province_row_gdp(state);
				}
				if(cname == "province_row_gdp_per_capita") {
					temp.ptr = make_trade_dashboard_province_row_gdp_per_capita(state);
				}
				if(cname == "commodity_selector") {
					temp.ptr = make_trade_dashboard_commodity_selector(state);
				}
				if(cname == "nation_selector") {
					temp.ptr = make_trade_dashboard_nation_selector(state);
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
void trade_dashboard_province_row_consumption_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("trade_dashboard::province_row_consumption"));
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
		if(child_data.name == "flag") {
			flag = std::make_unique<trade_dashboard_province_row_consumption_flag_t>();
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
		if(child_data.name == "name") {
			name = std::make_unique<trade_dashboard_province_row_consumption_name_t>();
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
		if(child_data.name == "value") {
			value = std::make_unique<trade_dashboard_province_row_consumption_value_t>();
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
// BEGIN province_row_consumption::create
// END
}
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_consumption(sys::state& state) {
	auto ptr = std::make_unique<trade_dashboard_province_row_consumption_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result trade_dashboard_province_row_production_flag_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_province_row_production_t& province_row_production = *((trade_dashboard_province_row_production_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN province_row_production::flag::lbutton_action
	main.nation_pov = state.world.province_get_nation_from_province_ownership(province_row_production.pid);
	state.ui_cached_data.set_nation(state, main.nation_pov);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_production_flag_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_production_flag_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void trade_dashboard_province_row_production_flag_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_production_flag_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_production_t& province_row_production = *((trade_dashboard_province_row_production_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_production::flag::update
	flag = state.world.province_get_nation_from_province_ownership(province_row_production.pid);
// END
}
void trade_dashboard_province_row_production_flag_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_production::flag::create
// END
}
void trade_dashboard_province_row_production_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_production_name_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_production_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_production_name_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_production_t& province_row_production = *((trade_dashboard_province_row_production_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_production::name::update
	set_text(state, text::produce_simple_string(state, state.world.province_get_name(province_row_production.pid)));
// END
}
void trade_dashboard_province_row_production_name_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_production::name::create
// END
}
void trade_dashboard_province_row_production_value_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_production_value_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_production_value_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_production_value_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_production_t& province_row_production = *((trade_dashboard_province_row_production_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_production::value::update
	set_text(state, text::prettify_float(economy::estimate_production(state, state.selected_trade_good, province_row_production.pid)));
// END
}
void trade_dashboard_province_row_production_value_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_production::value::create
// END
}
ui::message_result trade_dashboard_province_row_production_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_production_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_production_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void trade_dashboard_province_row_production_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_production::update
// END
	remake_layout(state, true);
}
void trade_dashboard_province_row_production_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "flag") {
					temp.ptr = flag.get();
				}
				if(cname == "name") {
					temp.ptr = name.get();
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
				if(cname == "main") {
					temp.ptr = make_trade_dashboard_main(state);
				}
				if(cname == "province_row_consumption") {
					temp.ptr = make_trade_dashboard_province_row_consumption(state);
				}
				if(cname == "province_row_production") {
					temp.ptr = make_trade_dashboard_province_row_production(state);
				}
				if(cname == "province_row_gdp") {
					temp.ptr = make_trade_dashboard_province_row_gdp(state);
				}
				if(cname == "province_row_gdp_per_capita") {
					temp.ptr = make_trade_dashboard_province_row_gdp_per_capita(state);
				}
				if(cname == "commodity_selector") {
					temp.ptr = make_trade_dashboard_commodity_selector(state);
				}
				if(cname == "nation_selector") {
					temp.ptr = make_trade_dashboard_nation_selector(state);
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
void trade_dashboard_province_row_production_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("trade_dashboard::province_row_production"));
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
		if(child_data.name == "flag") {
			flag = std::make_unique<trade_dashboard_province_row_production_flag_t>();
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
		if(child_data.name == "name") {
			name = std::make_unique<trade_dashboard_province_row_production_name_t>();
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
		if(child_data.name == "value") {
			value = std::make_unique<trade_dashboard_province_row_production_value_t>();
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
// BEGIN province_row_production::create
// END
}
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_production(sys::state& state) {
	auto ptr = std::make_unique<trade_dashboard_province_row_production_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result trade_dashboard_province_row_gdp_flag_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_province_row_gdp_t& province_row_gdp = *((trade_dashboard_province_row_gdp_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN province_row_gdp::flag::lbutton_action
	main.nation_pov = state.world.province_get_nation_from_province_ownership(province_row_gdp.pid);
	state.ui_cached_data.set_nation(state, main.nation_pov);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_gdp_flag_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_gdp_flag_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void trade_dashboard_province_row_gdp_flag_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_gdp_flag_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_gdp_t& province_row_gdp = *((trade_dashboard_province_row_gdp_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp::flag::update
	flag = state.world.province_get_nation_from_province_ownership(province_row_gdp.pid);
// END
}
void trade_dashboard_province_row_gdp_flag_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_gdp::flag::create
// END
}
void trade_dashboard_province_row_gdp_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_gdp_name_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_gdp_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_gdp_name_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_gdp_t& province_row_gdp = *((trade_dashboard_province_row_gdp_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp::name::update
	set_text(state, text::produce_simple_string(state, state.world.province_get_name(province_row_gdp.pid)));
// END
}
void trade_dashboard_province_row_gdp_name_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_gdp::name::create
// END
}
void trade_dashboard_province_row_gdp_value_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_gdp_value_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_gdp_value_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_gdp_value_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_gdp_t& province_row_gdp = *((trade_dashboard_province_row_gdp_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp::value::update
	set_text(state, text::format_money(economy::gdp::breakdown_province(state, province_row_gdp.pid).total_non_negative));
// END
}
void trade_dashboard_province_row_gdp_value_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_gdp::value::create
// END
}
ui::message_result trade_dashboard_province_row_gdp_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_gdp_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_gdp_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void trade_dashboard_province_row_gdp_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp::update
// END
	remake_layout(state, true);
}
void trade_dashboard_province_row_gdp_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "flag") {
					temp.ptr = flag.get();
				}
				if(cname == "name") {
					temp.ptr = name.get();
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
				if(cname == "main") {
					temp.ptr = make_trade_dashboard_main(state);
				}
				if(cname == "province_row_consumption") {
					temp.ptr = make_trade_dashboard_province_row_consumption(state);
				}
				if(cname == "province_row_production") {
					temp.ptr = make_trade_dashboard_province_row_production(state);
				}
				if(cname == "province_row_gdp") {
					temp.ptr = make_trade_dashboard_province_row_gdp(state);
				}
				if(cname == "province_row_gdp_per_capita") {
					temp.ptr = make_trade_dashboard_province_row_gdp_per_capita(state);
				}
				if(cname == "commodity_selector") {
					temp.ptr = make_trade_dashboard_commodity_selector(state);
				}
				if(cname == "nation_selector") {
					temp.ptr = make_trade_dashboard_nation_selector(state);
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
void trade_dashboard_province_row_gdp_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("trade_dashboard::province_row_gdp"));
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
		if(child_data.name == "flag") {
			flag = std::make_unique<trade_dashboard_province_row_gdp_flag_t>();
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
		if(child_data.name == "name") {
			name = std::make_unique<trade_dashboard_province_row_gdp_name_t>();
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
		if(child_data.name == "value") {
			value = std::make_unique<trade_dashboard_province_row_gdp_value_t>();
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
// BEGIN province_row_gdp::create
// END
}
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_gdp(sys::state& state) {
	auto ptr = std::make_unique<trade_dashboard_province_row_gdp_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result trade_dashboard_province_row_gdp_per_capita_flag_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_province_row_gdp_per_capita_t& province_row_gdp_per_capita = *((trade_dashboard_province_row_gdp_per_capita_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN province_row_gdp_per_capita::flag::lbutton_action
	main.nation_pov = state.world.province_get_nation_from_province_ownership(province_row_gdp_per_capita.pid);
	state.ui_cached_data.set_nation(state, main.nation_pov);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_gdp_per_capita_flag_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_gdp_per_capita_flag_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void trade_dashboard_province_row_gdp_per_capita_flag_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_gdp_per_capita_flag_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_gdp_per_capita_t& province_row_gdp_per_capita = *((trade_dashboard_province_row_gdp_per_capita_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp_per_capita::flag::update
	flag = state.world.province_get_nation_from_province_ownership(province_row_gdp_per_capita.pid);
// END
}
void trade_dashboard_province_row_gdp_per_capita_flag_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_gdp_per_capita::flag::create
// END
}
void trade_dashboard_province_row_gdp_per_capita_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_gdp_per_capita_name_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_gdp_per_capita_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_gdp_per_capita_name_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_gdp_per_capita_t& province_row_gdp_per_capita = *((trade_dashboard_province_row_gdp_per_capita_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp_per_capita::name::update
	set_text(state, text::produce_simple_string(state, state.world.province_get_name(province_row_gdp_per_capita.pid)));
// END
}
void trade_dashboard_province_row_gdp_per_capita_name_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_gdp_per_capita::name::create
// END
}
void trade_dashboard_province_row_gdp_per_capita_value_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_province_row_gdp_per_capita_value_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_province_row_gdp_per_capita_value_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_province_row_gdp_per_capita_value_t::on_update(sys::state& state) noexcept {
	trade_dashboard_province_row_gdp_per_capita_t& province_row_gdp_per_capita = *((trade_dashboard_province_row_gdp_per_capita_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp_per_capita::value::update
	auto pop = state.world.province_get_demographics(province_row_gdp_per_capita.pid, demographics::total);
	if(pop > 0) {
		set_text(state, text::format_money(economy::gdp::breakdown_province(state, province_row_gdp_per_capita.pid).total_non_negative / pop * 1000.f));
	} else {
		set_text(state, text::format_money(0.f));
	}
// END
}
void trade_dashboard_province_row_gdp_per_capita_value_t::on_create(sys::state& state) noexcept {
// BEGIN province_row_gdp_per_capita::value::create
// END
}
ui::message_result trade_dashboard_province_row_gdp_per_capita_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_province_row_gdp_per_capita_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_province_row_gdp_per_capita_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void trade_dashboard_province_row_gdp_per_capita_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN province_row_gdp_per_capita::update
// END
	remake_layout(state, true);
}
void trade_dashboard_province_row_gdp_per_capita_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "flag") {
					temp.ptr = flag.get();
				}
				if(cname == "name") {
					temp.ptr = name.get();
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
				if(cname == "main") {
					temp.ptr = make_trade_dashboard_main(state);
				}
				if(cname == "province_row_consumption") {
					temp.ptr = make_trade_dashboard_province_row_consumption(state);
				}
				if(cname == "province_row_production") {
					temp.ptr = make_trade_dashboard_province_row_production(state);
				}
				if(cname == "province_row_gdp") {
					temp.ptr = make_trade_dashboard_province_row_gdp(state);
				}
				if(cname == "province_row_gdp_per_capita") {
					temp.ptr = make_trade_dashboard_province_row_gdp_per_capita(state);
				}
				if(cname == "commodity_selector") {
					temp.ptr = make_trade_dashboard_commodity_selector(state);
				}
				if(cname == "nation_selector") {
					temp.ptr = make_trade_dashboard_nation_selector(state);
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
void trade_dashboard_province_row_gdp_per_capita_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("trade_dashboard::province_row_gdp_per_capita"));
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
		if(child_data.name == "flag") {
			flag = std::make_unique<trade_dashboard_province_row_gdp_per_capita_flag_t>();
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
		if(child_data.name == "name") {
			name = std::make_unique<trade_dashboard_province_row_gdp_per_capita_name_t>();
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
		if(child_data.name == "value") {
			value = std::make_unique<trade_dashboard_province_row_gdp_per_capita_value_t>();
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
// BEGIN province_row_gdp_per_capita::create
// END
}
std::unique_ptr<ui::element_base> make_trade_dashboard_province_row_gdp_per_capita(sys::state& state) {
	auto ptr = std::make_unique<trade_dashboard_province_row_gdp_per_capita_t>();
	ptr->on_create(state);
	return ptr;
}
void trade_dashboard_commodity_selector_icon_t::on_update(sys::state& state) noexcept {
	trade_dashboard_commodity_selector_t& commodity_selector = *((trade_dashboard_commodity_selector_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN commodity_selector::icon::update
// END
}
void trade_dashboard_commodity_selector_icon_t::on_create(sys::state& state) noexcept {
// BEGIN commodity_selector::icon::create
// END
}
ui::message_result trade_dashboard_commodity_selector_name_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_commodity_selector_t& commodity_selector = *((trade_dashboard_commodity_selector_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN commodity_selector::name::lbutton_action
	if(state.selected_trade_good == commodity_selector.cid) {
		state.selected_trade_good = { };
		state.ui_cached_data.set_commodity(state, { });
	} else {
		state.selected_trade_good = commodity_selector.cid;
		state.ui_cached_data.set_commodity(state, commodity_selector.cid);
	}
	state.update_trade_flow.store(true, std::memory_order::release);
	main.on_update(state);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_commodity_selector_name_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_commodity_selector_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_commodity_selector_name_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_commodity_selector_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
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
void trade_dashboard_commodity_selector_name_t::on_update(sys::state& state) noexcept {
	trade_dashboard_commodity_selector_t& commodity_selector = *((trade_dashboard_commodity_selector_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN commodity_selector::name::update
	set_text(state, text::get_commodity_text_icon(state, commodity_selector.cid));
	if(state.selected_trade_good == commodity_selector.cid) {
		is_active = true;
	} else {
		is_active = false;
	}
// END
}
void trade_dashboard_commodity_selector_name_t::on_create(sys::state& state) noexcept {
// BEGIN commodity_selector::name::create
// END
}
void trade_dashboard_commodity_selector_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void trade_dashboard_commodity_selector_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN commodity_selector::update
// END
	remake_layout(state, true);
}
void trade_dashboard_commodity_selector_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "icon") {
					temp.ptr = icon.get();
				}
				if(cname == "name") {
					temp.ptr = name.get();
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
					temp.ptr = make_trade_dashboard_main(state);
				}
				if(cname == "province_row_consumption") {
					temp.ptr = make_trade_dashboard_province_row_consumption(state);
				}
				if(cname == "province_row_production") {
					temp.ptr = make_trade_dashboard_province_row_production(state);
				}
				if(cname == "province_row_gdp") {
					temp.ptr = make_trade_dashboard_province_row_gdp(state);
				}
				if(cname == "province_row_gdp_per_capita") {
					temp.ptr = make_trade_dashboard_province_row_gdp_per_capita(state);
				}
				if(cname == "commodity_selector") {
					temp.ptr = make_trade_dashboard_commodity_selector(state);
				}
				if(cname == "nation_selector") {
					temp.ptr = make_trade_dashboard_nation_selector(state);
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
void trade_dashboard_commodity_selector_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("trade_dashboard::commodity_selector"));
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
		if(child_data.name == "icon") {
			icon = std::make_unique<trade_dashboard_commodity_selector_icon_t>();
			icon->parent = this;
			auto cptr = icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		if(child_data.name == "name") {
			name = std::make_unique<trade_dashboard_commodity_selector_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
// BEGIN commodity_selector::create
// END
}
std::unique_ptr<ui::element_base> make_trade_dashboard_commodity_selector(sys::state& state) {
	auto ptr = std::make_unique<trade_dashboard_commodity_selector_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result trade_dashboard_nation_selector_flag_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_nation_selector_t& nation_selector = *((trade_dashboard_nation_selector_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN nation_selector::flag::lbutton_action
	if(main.nation_pov == nation_selector.nid) {
		main.nation_pov = { };
	} else {
		main.nation_pov = nation_selector.nid;
		state.ui_cached_data.set_nation(state, main.nation_pov);
	}
	main.on_update(state);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_nation_selector_flag_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_nation_selector_flag_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void trade_dashboard_nation_selector_flag_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void trade_dashboard_nation_selector_flag_t::on_update(sys::state& state) noexcept {
	trade_dashboard_nation_selector_t& nation_selector = *((trade_dashboard_nation_selector_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN nation_selector::flag::update
	flag = nation_selector.nid;
// END
}
void trade_dashboard_nation_selector_flag_t::on_create(sys::state& state) noexcept {
// BEGIN nation_selector::flag::create
// END
}
ui::message_result trade_dashboard_nation_selector_name_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	trade_dashboard_nation_selector_t& nation_selector = *((trade_dashboard_nation_selector_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN nation_selector::name::lbutton_action
	if(main.nation_pov == nation_selector.nid) {
		main.nation_pov = { };
	} else {
		main.nation_pov = nation_selector.nid;
		state.ui_cached_data.set_nation(state, main.nation_pov);
	}
	main.on_update(state);
// END
	return ui::message_result::consumed;
}
ui::message_result trade_dashboard_nation_selector_name_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void trade_dashboard_nation_selector_name_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 20), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void trade_dashboard_nation_selector_name_t::on_reset_text(sys::state& state) noexcept {
}
void trade_dashboard_nation_selector_name_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(is_active)
		ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, alt_background_texture, alt_texture_key), base_data.get_rotation(), false, state_is_rtl(state)); 
	else
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
void trade_dashboard_nation_selector_name_t::on_update(sys::state& state) noexcept {
	trade_dashboard_nation_selector_t& nation_selector = *((trade_dashboard_nation_selector_t*)(parent)); 
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN nation_selector::name::update
	auto identity = state.world.nation_get_identity_from_identity_holder(nation_selector.nid);
	auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(identity));
	set_text(state, curr);
	if(main.nation_pov == nation_selector.nid) {
		is_active = true;
	} else {
		is_active = false;
	}
// END
}
void trade_dashboard_nation_selector_name_t::on_create(sys::state& state) noexcept {
// BEGIN nation_selector::name::create
// END
}
void trade_dashboard_nation_selector_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto cmod = ui::get_color_modification(false, false,  false);
	for (auto& _item : textures_to_render) {
		if (_item.texture_type == background_type::texture)
			ogl::render_textured_rect(state, cmod, float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::border_texture_repeat)
			ogl::render_rect_with_repeated_border(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
		else if (_item.texture_type == background_type::textured_corners)
			ogl::render_rect_with_repeated_corner(state, cmod, float(10), float(x + _item.x), float(y + _item.y), float(_item.w), float(_item.h), ogl::get_late_load_texture_handle(state, _item.texture_id, _item.texture), base_data.get_rotation(), false, state_is_rtl(state));
	}
}
void trade_dashboard_nation_selector_t::on_update(sys::state& state) noexcept {
	trade_dashboard_main_t& main = *((trade_dashboard_main_t*)(parent->parent)); 
// BEGIN nation_selector::update
// END
	remake_layout(state, true);
}
void trade_dashboard_nation_selector_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "flag") {
					temp.ptr = flag.get();
				}
				if(cname == "name") {
					temp.ptr = name.get();
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
					temp.ptr = make_trade_dashboard_main(state);
				}
				if(cname == "province_row_consumption") {
					temp.ptr = make_trade_dashboard_province_row_consumption(state);
				}
				if(cname == "province_row_production") {
					temp.ptr = make_trade_dashboard_province_row_production(state);
				}
				if(cname == "province_row_gdp") {
					temp.ptr = make_trade_dashboard_province_row_gdp(state);
				}
				if(cname == "province_row_gdp_per_capita") {
					temp.ptr = make_trade_dashboard_province_row_gdp_per_capita(state);
				}
				if(cname == "commodity_selector") {
					temp.ptr = make_trade_dashboard_commodity_selector(state);
				}
				if(cname == "nation_selector") {
					temp.ptr = make_trade_dashboard_nation_selector(state);
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
void trade_dashboard_nation_selector_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("trade_dashboard::nation_selector"));
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
		if(child_data.name == "flag") {
			flag = std::make_unique<trade_dashboard_nation_selector_flag_t>();
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
		if(child_data.name == "name") {
			name = std::make_unique<trade_dashboard_nation_selector_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
// BEGIN nation_selector::create
// END
}
std::unique_ptr<ui::element_base> make_trade_dashboard_nation_selector(sys::state& state) {
	auto ptr = std::make_unique<trade_dashboard_nation_selector_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
