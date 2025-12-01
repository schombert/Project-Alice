namespace alice_ui {
struct pop_details_main_location_value_t;
struct pop_details_main_job_value_t;
struct pop_details_main_mil_value_t;
struct pop_details_main_con_value_t;
struct pop_details_main_lit_value_t;
struct pop_details_main_religion_value_t;
struct pop_details_main_culture_value_t;
struct pop_details_main_employment_value_t;
struct pop_details_main_income_value_t;
struct pop_details_main_growth_value_t;
struct pop_details_main_assim_value_t;
struct pop_details_main_ln_total_t;
struct pop_details_main_en_total_t;
struct pop_details_main_lx_total_t;
struct pop_details_main_prom_value_t;
struct pop_details_main_prom_bar_chart_t;
struct pop_details_main_dem_value_t;
struct pop_details_main_dem_bar_chart_t;
struct pop_details_main_ideo_bar_chart_t;
struct pop_details_main_iss_bar_chart_t;
struct pop_details_main_emm_value_t;
struct pop_details_main_i_mig_value_t;
struct pop_details_main_c_mig_value_t;
struct pop_details_main_to_pop_budget_t;
struct pop_details_main_t;
struct pop_details_needs_row_content_t;
struct pop_details_needs_row_need_icon_t;
struct pop_details_needs_row_t;
struct pop_details_needs_header_content_t;
struct pop_details_needs_header_t;
struct pop_details_prom_header_content_t;
struct pop_details_prom_header_t;
struct pop_details_prom_row_content_t;
struct pop_details_prom_row_job_icon_t;
struct pop_details_prom_row_t;
struct pop_details_weights_header_content_t;
struct pop_details_weights_header_t;
struct pop_details_weights_row_content_t;
struct pop_details_weights_row_t;
struct pop_details_emm_header_content_t;
struct pop_details_emm_header_t;
struct pop_details_emm_row_content_t;
struct pop_details_emm_row_dest_flag_t;
struct pop_details_emm_row_t;
struct pop_details_mig_header_content_t;
struct pop_details_mig_header_t;
struct pop_details_mig_row_content_t;
struct pop_details_mig_row_t;
struct pop_details_fancy_separator_1_t;
struct pop_details_fancy_separator_2_t;
struct pop_details_main_location_value_t : public alice_ui::template_label {
// BEGIN main::location_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_job_value_t : public alice_ui::template_label {
// BEGIN main::job_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_mil_value_t : public alice_ui::template_label {
// BEGIN main::mil_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_con_value_t : public alice_ui::template_label {
// BEGIN main::con_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_lit_value_t : public alice_ui::template_label {
// BEGIN main::lit_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_religion_value_t : public alice_ui::template_label {
// BEGIN main::religion_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_culture_value_t : public alice_ui::template_label {
// BEGIN main::culture_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_employment_value_t : public alice_ui::template_label {
// BEGIN main::employment_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_income_value_t : public alice_ui::template_label {
// BEGIN main::income_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_growth_value_t : public alice_ui::template_label {
// BEGIN main::growth_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_assim_value_t : public alice_ui::template_label {
// BEGIN main::assim_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_ln_total_t : public alice_ui::template_label {
// BEGIN main::ln_total::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_en_total_t : public alice_ui::template_label {
// BEGIN main::en_total::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_lx_total_t : public alice_ui::template_label {
// BEGIN main::lx_total::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_prom_value_t : public alice_ui::template_label {
// BEGIN main::prom_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_prom_bar_chart_t : public ui::element_base {
// BEGIN main::prom_bar_chart::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 264, 3 };
	struct graph_entry {dcon::pop_type_id key; ogl::color3f color; float amount; };
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
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_dem_value_t : public alice_ui::template_label {
// BEGIN main::dem_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_dem_bar_chart_t : public ui::element_base {
// BEGIN main::dem_bar_chart::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 264, 3 };
	struct graph_entry {dcon::pop_type_id key; ogl::color3f color; float amount; };
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
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_ideo_bar_chart_t : public ui::element_base {
// BEGIN main::ideo_bar_chart::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 264, 3 };
	struct graph_entry {dcon::ideology_id key; ogl::color3f color; float amount; };
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
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_iss_bar_chart_t : public ui::element_base {
// BEGIN main::iss_bar_chart::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 264, 3 };
	struct graph_entry {dcon::issue_option_id key; ogl::color3f color; float amount; };
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
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_emm_value_t : public alice_ui::template_label {
// BEGIN main::emm_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_i_mig_value_t : public alice_ui::template_label {
// BEGIN main::i_mig_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_c_mig_value_t : public alice_ui::template_label {
// BEGIN main::c_mig_value::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_to_pop_budget_t : public alice_ui::template_text_button {
// BEGIN main::to_pop_budget::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct pop_details_main_ln_list_t : public layout_generator {
// BEGIN main::ln_list::variables
// END
	struct needs_row_option { dcon::commodity_id commodity; float amount; float weight; };
	std::vector<std::unique_ptr<ui::element_base>> needs_row_pool;
	int32_t needs_row_pool_used = 0;
	void add_needs_row( dcon::commodity_id commodity,  float amount,  float weight);
	std::vector<std::unique_ptr<ui::element_base>> needs_header_pool;
	int32_t needs_header_pool_used = 0;
	std::vector<std::variant<std::monostate, needs_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_en_list_t : public layout_generator {
// BEGIN main::en_list::variables
// END
	struct needs_row_option { dcon::commodity_id commodity; float amount; float weight; };
	std::vector<std::unique_ptr<ui::element_base>> needs_row_pool;
	int32_t needs_row_pool_used = 0;
	void add_needs_row( dcon::commodity_id commodity,  float amount,  float weight);
	std::vector<std::unique_ptr<ui::element_base>> needs_header_pool;
	int32_t needs_header_pool_used = 0;
	std::vector<std::variant<std::monostate, needs_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_lx_list_t : public layout_generator {
// BEGIN main::lx_list::variables
// END
	struct needs_row_option { dcon::commodity_id commodity; float amount; float weight; };
	std::vector<std::unique_ptr<ui::element_base>> needs_row_pool;
	int32_t needs_row_pool_used = 0;
	void add_needs_row( dcon::commodity_id commodity,  float amount,  float weight);
	std::vector<std::unique_ptr<ui::element_base>> needs_header_pool;
	int32_t needs_header_pool_used = 0;
	std::vector<std::variant<std::monostate, needs_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_prom_list_t : public layout_generator {
// BEGIN main::prom_list::variables
// END
	struct prom_row_option { dcon::pop_type_id value; };
	std::vector<std::unique_ptr<ui::element_base>> prom_row_pool;
	int32_t prom_row_pool_used = 0;
	void add_prom_row( dcon::pop_type_id value);
	std::vector<std::unique_ptr<ui::element_base>> prom_header_pool;
	int32_t prom_header_pool_used = 0;
	std::vector<std::variant<std::monostate, prom_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_dem_list_t : public layout_generator {
// BEGIN main::dem_list::variables
// END
	struct prom_row_option { dcon::pop_type_id value; };
	std::vector<std::unique_ptr<ui::element_base>> prom_row_pool;
	int32_t prom_row_pool_used = 0;
	void add_prom_row( dcon::pop_type_id value);
	std::vector<std::unique_ptr<ui::element_base>> prom_header_pool;
	int32_t prom_header_pool_used = 0;
	std::vector<std::variant<std::monostate, prom_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_ideo_list_t : public layout_generator {
// BEGIN main::ideo_list::variables
// END
	struct weights_row_option { std::string item_name; float weight_value; std::variant<dcon::ideology_id, dcon::issue_option_id> source; };
	std::vector<std::unique_ptr<ui::element_base>> weights_row_pool;
	int32_t weights_row_pool_used = 0;
	void add_weights_row( std::string item_name,  float weight_value,  std::variant<dcon::ideology_id, dcon::issue_option_id> source);
	std::vector<std::unique_ptr<ui::element_base>> weights_header_pool;
	int32_t weights_header_pool_used = 0;
	std::vector<std::variant<std::monostate, weights_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_iss_list_t : public layout_generator {
// BEGIN main::iss_list::variables
// END
	struct weights_row_option { std::string item_name; float weight_value; std::variant<dcon::ideology_id, dcon::issue_option_id> source; };
	std::vector<std::unique_ptr<ui::element_base>> weights_row_pool;
	int32_t weights_row_pool_used = 0;
	void add_weights_row( std::string item_name,  float weight_value,  std::variant<dcon::ideology_id, dcon::issue_option_id> source);
	std::vector<std::unique_ptr<ui::element_base>> weights_header_pool;
	int32_t weights_header_pool_used = 0;
	std::vector<std::variant<std::monostate, weights_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_emm_list_t : public layout_generator {
// BEGIN main::emm_list::variables
// END
	struct emm_row_option { dcon::nation_id destination; };
	std::vector<std::unique_ptr<ui::element_base>> emm_row_pool;
	int32_t emm_row_pool_used = 0;
	void add_emm_row( dcon::nation_id destination);
	std::vector<std::unique_ptr<ui::element_base>> emm_header_pool;
	int32_t emm_header_pool_used = 0;
	std::vector<std::variant<std::monostate, emm_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_main_mig_list_t : public layout_generator {
// BEGIN main::mig_list::variables
// END
	struct mig_row_option { dcon::province_id destination; };
	std::vector<std::unique_ptr<ui::element_base>> mig_row_pool;
	int32_t mig_row_pool_used = 0;
	void add_mig_row( dcon::province_id destination);
	std::vector<std::unique_ptr<ui::element_base>> mig_header_pool;
	int32_t mig_header_pool_used = 0;
	std::vector<std::variant<std::monostate, mig_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct pop_details_needs_row_content_t : public ui::element_base {
// BEGIN needs_row::content::variables
// END
	int32_t template_id = -1;
	text::layout icon_internal_layout;
	int32_t  icon_text_color = 0;
	std::string icon_cached_text;
	void set_icon_text(sys::state & state, std::string const& new_text);
	text::layout name_internal_layout;
	int32_t  name_text_color = 0;
	std::string name_cached_text;
	void set_name_text(sys::state & state, std::string const& new_text);
	text::layout amount_internal_layout;
	int32_t  amount_text_color = 0;
	std::string amount_cached_text;
	void set_amount_text(sys::state & state, std::string const& new_text);
	text::layout cost_internal_layout;
	int32_t  cost_text_color = 0;
	std::string cost_cached_text;
	void set_cost_text(sys::state & state, std::string const& new_text);
	text::layout weight_internal_layout;
	int32_t  weight_text_color = 0;
	std::string weight_cached_text;
	void set_weight_text(sys::state & state, std::string const& new_text);
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
struct pop_details_needs_row_need_icon_t : public ui::element_base {
// BEGIN needs_row::need_icon::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
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
struct pop_details_needs_header_content_t : public ui::element_base {
// BEGIN needs_header::content::variables
// END
	int32_t template_id = -1;
	text::layout name_internal_layout;
	std::string name_cached_text;
	text::layout amount_internal_layout;
	std::string amount_cached_text;
	text::layout cost_internal_layout;
	std::string cost_cached_text;
	text::layout weight_internal_layout;
	std::string weight_cached_text;
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
struct pop_details_prom_header_content_t : public ui::element_base {
// BEGIN prom_header::content::variables
// END
	int32_t template_id = -1;
	text::layout name_internal_layout;
	std::string name_cached_text;
	text::layout weight_internal_layout;
	std::string weight_cached_text;
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
struct pop_details_prom_row_content_t : public ui::element_base {
// BEGIN prom_row::content::variables
// END
	int32_t template_id = -1;
	text::layout icon_internal_layout;
	int32_t  icon_text_color = 0;
	std::string icon_cached_text;
	void set_icon_text(sys::state & state, std::string const& new_text);
	text::layout name_internal_layout;
	int32_t  name_text_color = 0;
	std::string name_cached_text;
	void set_name_text(sys::state & state, std::string const& new_text);
	text::layout weight_internal_layout;
	int32_t  weight_text_color = 0;
	std::string weight_cached_text;
	void set_weight_text(sys::state & state, std::string const& new_text);
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
struct pop_details_prom_row_job_icon_t : public ui::element_base {
// BEGIN prom_row::job_icon::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
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
struct pop_details_weights_header_content_t : public ui::element_base {
// BEGIN weights_header::content::variables
// END
	int32_t template_id = -1;
	text::layout item_internal_layout;
	std::string item_cached_text;
	text::layout weight_internal_layout;
	std::string weight_cached_text;
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
struct pop_details_weights_row_content_t : public ui::element_base {
// BEGIN weights_row::content::variables
// END
	int32_t template_id = -1;
	text::layout item_internal_layout;
	int32_t  item_text_color = 0;
	std::string item_cached_text;
	void set_item_text(sys::state & state, std::string const& new_text);
	text::layout weight_internal_layout;
	int32_t  weight_text_color = 0;
	std::string weight_cached_text;
	void set_weight_text(sys::state & state, std::string const& new_text);
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
struct pop_details_emm_header_content_t : public ui::element_base {
// BEGIN emm_header::content::variables
// END
	int32_t template_id = -1;
	text::layout destination_internal_layout;
	std::string destination_cached_text;
	text::layout weight_internal_layout;
	std::string weight_cached_text;
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
struct pop_details_emm_row_content_t : public ui::element_base {
// BEGIN emm_row::content::variables
// END
	int32_t template_id = -1;
	text::layout destination_internal_layout;
	int32_t  destination_text_color = 0;
	std::string destination_cached_text;
	void set_destination_text(sys::state & state, std::string const& new_text);
	text::layout weight_internal_layout;
	int32_t  weight_text_color = 0;
	std::string weight_cached_text;
	void set_weight_text(sys::state & state, std::string const& new_text);
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
struct pop_details_emm_row_dest_flag_t : public ui::element_base {
// BEGIN emm_row::dest_flag::variables
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
struct pop_details_mig_header_content_t : public ui::element_base {
// BEGIN mig_header::content::variables
// END
	int32_t template_id = -1;
	text::layout destination_internal_layout;
	std::string destination_cached_text;
	text::layout weight_internal_layout;
	std::string weight_cached_text;
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
struct pop_details_mig_row_content_t : public ui::element_base {
// BEGIN mig_row::content::variables
// END
	int32_t template_id = -1;
	text::layout destination_internal_layout;
	int32_t  destination_text_color = 0;
	std::string destination_cached_text;
	void set_destination_text(sys::state & state, std::string const& new_text);
	text::layout weight_internal_layout;
	int32_t  weight_text_color = 0;
	std::string weight_cached_text;
	void set_weight_text(sys::state & state, std::string const& new_text);
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
struct pop_details_main_t : public layout_window_element {
// BEGIN main::variables
	ui::element_base* budget_w;
// END
	dcon::pop_id for_pop;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> window_title;
	std::unique_ptr<template_label> location_header;
	std::unique_ptr<pop_details_main_location_value_t> location_value;
	std::unique_ptr<template_label> job_header;
	std::unique_ptr<pop_details_main_job_value_t> job_value;
	std::unique_ptr<template_label> mil_header;
	std::unique_ptr<pop_details_main_mil_value_t> mil_value;
	std::unique_ptr<template_label> con_header;
	std::unique_ptr<pop_details_main_con_value_t> con_value;
	std::unique_ptr<template_label> lit_header;
	std::unique_ptr<pop_details_main_lit_value_t> lit_value;
	std::unique_ptr<template_label> religion_header;
	std::unique_ptr<pop_details_main_religion_value_t> religion_value;
	std::unique_ptr<template_label> culture_header;
	std::unique_ptr<pop_details_main_culture_value_t> culture_value;
	std::unique_ptr<template_label> employment_header;
	std::unique_ptr<pop_details_main_employment_value_t> employment_value;
	std::unique_ptr<template_label> income_header;
	std::unique_ptr<pop_details_main_income_value_t> income_value;
	std::unique_ptr<template_label> growth_header;
	std::unique_ptr<pop_details_main_growth_value_t> growth_value;
	std::unique_ptr<template_label> assim_header;
	std::unique_ptr<pop_details_main_assim_value_t> assim_value;
	std::unique_ptr<pop_details_main_ln_total_t> ln_total;
	std::unique_ptr<pop_details_main_en_total_t> en_total;
	std::unique_ptr<pop_details_main_lx_total_t> lx_total;
	std::unique_ptr<template_label> prom_header;
	std::unique_ptr<template_label> prom_amount_label;
	std::unique_ptr<pop_details_main_prom_value_t> prom_value;
	std::unique_ptr<pop_details_main_prom_bar_chart_t> prom_bar_chart;
	std::unique_ptr<template_label> dem_header;
	std::unique_ptr<template_label> dem_amount_label;
	std::unique_ptr<pop_details_main_dem_value_t> dem_value;
	std::unique_ptr<pop_details_main_dem_bar_chart_t> dem_bar_chart;
	std::unique_ptr<template_label> ideo_header;
	std::unique_ptr<pop_details_main_ideo_bar_chart_t> ideo_bar_chart;
	std::unique_ptr<template_label> iss_header;
	std::unique_ptr<pop_details_main_iss_bar_chart_t> iss_bar_chart;
	std::unique_ptr<template_label> emm_header;
	std::unique_ptr<template_label> emm_amount_label;
	std::unique_ptr<pop_details_main_emm_value_t> emm_value;
	std::unique_ptr<template_label> mig_header;
	std::unique_ptr<template_label> i_mig_amount_label;
	std::unique_ptr<pop_details_main_i_mig_value_t> i_mig_value;
	std::unique_ptr<template_label> c_mig_amount_label;
	std::unique_ptr<pop_details_main_c_mig_value_t> c_mig_value;
	std::unique_ptr<template_bg_graphic> fancy_bar_1;
	std::unique_ptr<template_bg_graphic> fancy_bar_2;
	std::unique_ptr<pop_details_main_to_pop_budget_t> to_pop_budget;
	std::unique_ptr<template_bg_graphic> fancy_bar_3;
	std::unique_ptr<template_label> ln_header;
	std::unique_ptr<template_label> en_header;
	std::unique_ptr<template_label> lx_header;
	std::unique_ptr<template_icon_graphic> ln_icon;
	std::unique_ptr<template_icon_graphic> en_icon;
	std::unique_ptr<template_icon_graphic> lx_icon;
	pop_details_main_ln_list_t ln_list;
	pop_details_main_en_list_t en_list;
	pop_details_main_lx_list_t lx_list;
	pop_details_main_prom_list_t prom_list;
	pop_details_main_dem_list_t dem_list;
	pop_details_main_ideo_list_t ideo_list;
	pop_details_main_iss_list_t iss_list;
	pop_details_main_emm_list_t emm_list;
	pop_details_main_mig_list_t mig_list;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t needs_table_icon_header_text_color = 0;
	uint8_t needs_table_icon_column_text_color = 0;
	text::alignment needs_table_icon_text_alignment = text::alignment::center;
	int16_t needs_table_icon_column_start = 0;
	int16_t needs_table_icon_column_width = 0;
	std::string_view needs_table_name_header_text_key;
	uint8_t needs_table_name_header_text_color = 0;
	uint8_t needs_table_name_column_text_color = 0;
	text::alignment needs_table_name_text_alignment = text::alignment::left;
	int8_t needs_table_name_sort_direction = 0;
	int16_t needs_table_name_column_start = 0;
	int16_t needs_table_name_column_width = 0;
	std::string_view needs_table_amount_header_text_key;
	uint8_t needs_table_amount_header_text_color = 0;
	uint8_t needs_table_amount_column_text_color = 0;
	text::alignment needs_table_amount_text_alignment = text::alignment::left;
	int8_t needs_table_amount_sort_direction = 0;
	int16_t needs_table_amount_column_start = 0;
	int16_t needs_table_amount_column_width = 0;
	std::string_view needs_table_cost_header_text_key;
	uint8_t needs_table_cost_header_text_color = 0;
	uint8_t needs_table_cost_column_text_color = 0;
	text::alignment needs_table_cost_text_alignment = text::alignment::left;
	int8_t needs_table_cost_sort_direction = 0;
	int16_t needs_table_cost_column_start = 0;
	int16_t needs_table_cost_column_width = 0;
	std::string_view needs_table_weight_header_text_key;
	std::string_view needs_table_weight_header_tooltip_key;
	uint8_t needs_table_weight_header_text_color = 0;
	uint8_t needs_table_weight_column_text_color = 0;
	text::alignment needs_table_weight_text_alignment = text::alignment::center;
	int8_t needs_table_weight_sort_direction = 0;
	int16_t needs_table_weight_column_start = 0;
	int16_t needs_table_weight_column_width = 0;
	std::string_view needs_table_ascending_icon_key;
	dcon::texture_id needs_table_ascending_icon;
	std::string_view needs_table_descending_icon_key;
	dcon::texture_id needs_table_descending_icon;
	uint8_t prom_table_icon_header_text_color = 0;
	uint8_t prom_table_icon_column_text_color = 0;
	text::alignment prom_table_icon_text_alignment = text::alignment::center;
	int16_t prom_table_icon_column_start = 0;
	int16_t prom_table_icon_column_width = 0;
	std::string_view prom_table_name_header_text_key;
	uint8_t prom_table_name_header_text_color = 0;
	uint8_t prom_table_name_column_text_color = 0;
	text::alignment prom_table_name_text_alignment = text::alignment::right;
	int8_t prom_table_name_sort_direction = 0;
	int16_t prom_table_name_column_start = 0;
	int16_t prom_table_name_column_width = 0;
	std::string_view prom_table_weight_header_text_key;
	uint8_t prom_table_weight_header_text_color = 0;
	uint8_t prom_table_weight_column_text_color = 0;
	text::alignment prom_table_weight_text_alignment = text::alignment::right;
	int8_t prom_table_weight_sort_direction = 0;
	int16_t prom_table_weight_column_start = 0;
	int16_t prom_table_weight_column_width = 0;
	std::string_view prom_table_ascending_icon_key;
	dcon::texture_id prom_table_ascending_icon;
	std::string_view prom_table_descending_icon_key;
	dcon::texture_id prom_table_descending_icon;
	std::string_view weights_table_item_header_text_key;
	uint8_t weights_table_item_header_text_color = 0;
	uint8_t weights_table_item_column_text_color = 0;
	text::alignment weights_table_item_text_alignment = text::alignment::right;
	int8_t weights_table_item_sort_direction = 0;
	int16_t weights_table_item_column_start = 0;
	int16_t weights_table_item_column_width = 0;
	std::string_view weights_table_weight_header_text_key;
	uint8_t weights_table_weight_header_text_color = 0;
	uint8_t weights_table_weight_column_text_color = 0;
	text::alignment weights_table_weight_text_alignment = text::alignment::right;
	int8_t weights_table_weight_sort_direction = 0;
	int16_t weights_table_weight_column_start = 0;
	int16_t weights_table_weight_column_width = 0;
	std::string_view weights_table_ascending_icon_key;
	dcon::texture_id weights_table_ascending_icon;
	std::string_view weights_table_descending_icon_key;
	dcon::texture_id weights_table_descending_icon;
	int16_t emm_table_flag_column_start = 0;
	int16_t emm_table_flag_column_width = 0;
	std::string_view emm_table_destination_header_text_key;
	uint8_t emm_table_destination_header_text_color = 0;
	uint8_t emm_table_destination_column_text_color = 0;
	text::alignment emm_table_destination_text_alignment = text::alignment::right;
	int8_t emm_table_destination_sort_direction = 0;
	int16_t emm_table_destination_column_start = 0;
	int16_t emm_table_destination_column_width = 0;
	std::string_view emm_table_weight_header_text_key;
	uint8_t emm_table_weight_header_text_color = 0;
	uint8_t emm_table_weight_column_text_color = 0;
	text::alignment emm_table_weight_text_alignment = text::alignment::right;
	int8_t emm_table_weight_sort_direction = 0;
	int16_t emm_table_weight_column_start = 0;
	int16_t emm_table_weight_column_width = 0;
	std::string_view emm_table_ascending_icon_key;
	dcon::texture_id emm_table_ascending_icon;
	std::string_view emm_table_descending_icon_key;
	dcon::texture_id emm_table_descending_icon;
	std::string_view mig_table_destination_header_text_key;
	uint8_t mig_table_destination_header_text_color = 0;
	uint8_t mig_table_destination_column_text_color = 0;
	text::alignment mig_table_destination_text_alignment = text::alignment::right;
	int8_t mig_table_destination_sort_direction = 0;
	int16_t mig_table_destination_column_start = 0;
	int16_t mig_table_destination_column_width = 0;
	std::string_view mig_table_weight_header_text_key;
	uint8_t mig_table_weight_header_text_color = 0;
	uint8_t mig_table_weight_column_text_color = 0;
	text::alignment mig_table_weight_text_alignment = text::alignment::right;
	int8_t mig_table_weight_sort_direction = 0;
	int16_t mig_table_weight_column_start = 0;
	int16_t mig_table_weight_column_width = 0;
	std::string_view mig_table_ascending_icon_key;
	dcon::texture_id mig_table_ascending_icon;
	std::string_view mig_table_descending_icon_key;
	dcon::texture_id mig_table_descending_icon;
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
		if(name_parameter == "for_pop") {
			return (void*)(&for_pop);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_details_main(sys::state& state);
struct pop_details_needs_row_t : public layout_window_element {
// BEGIN needs_row::variables
// END
	dcon::commodity_id commodity;
	float amount;
	float weight;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_needs_row_content_t> content;
	std::unique_ptr<pop_details_needs_row_need_icon_t> need_icon;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t needs_table_icon_header_text_color = 0;
	uint8_t needs_table_icon_column_text_color = 0;
	text::alignment needs_table_icon_text_alignment = text::alignment::center;
	int16_t needs_table_icon_column_start = 0;
	int16_t needs_table_icon_column_width = 0;
	std::string_view needs_table_name_header_text_key;
	uint8_t needs_table_name_header_text_color = 0;
	uint8_t needs_table_name_column_text_color = 0;
	text::alignment needs_table_name_text_alignment = text::alignment::left;
	int8_t needs_table_name_sort_direction = 0;
	int16_t needs_table_name_column_start = 0;
	int16_t needs_table_name_column_width = 0;
	std::string_view needs_table_amount_header_text_key;
	uint8_t needs_table_amount_header_text_color = 0;
	uint8_t needs_table_amount_column_text_color = 0;
	text::alignment needs_table_amount_text_alignment = text::alignment::left;
	int8_t needs_table_amount_sort_direction = 0;
	int16_t needs_table_amount_column_start = 0;
	int16_t needs_table_amount_column_width = 0;
	std::string_view needs_table_cost_header_text_key;
	uint8_t needs_table_cost_header_text_color = 0;
	uint8_t needs_table_cost_column_text_color = 0;
	text::alignment needs_table_cost_text_alignment = text::alignment::left;
	int8_t needs_table_cost_sort_direction = 0;
	int16_t needs_table_cost_column_start = 0;
	int16_t needs_table_cost_column_width = 0;
	std::string_view needs_table_weight_header_text_key;
	std::string_view needs_table_weight_header_tooltip_key;
	uint8_t needs_table_weight_header_text_color = 0;
	uint8_t needs_table_weight_column_text_color = 0;
	text::alignment needs_table_weight_text_alignment = text::alignment::center;
	int8_t needs_table_weight_sort_direction = 0;
	int16_t needs_table_weight_column_start = 0;
	int16_t needs_table_weight_column_width = 0;
	std::string_view needs_table_ascending_icon_key;
	dcon::texture_id needs_table_ascending_icon;
	std::string_view needs_table_descending_icon_key;
	dcon::texture_id needs_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "commodity") {
			return (void*)(&commodity);
		}
		if(name_parameter == "amount") {
			return (void*)(&amount);
		}
		if(name_parameter == "weight") {
			return (void*)(&weight);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_details_needs_row(sys::state& state);
struct pop_details_needs_header_t : public layout_window_element {
// BEGIN needs_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_needs_header_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t needs_table_icon_header_text_color = 0;
	uint8_t needs_table_icon_column_text_color = 0;
	text::alignment needs_table_icon_text_alignment = text::alignment::center;
	int16_t needs_table_icon_column_start = 0;
	int16_t needs_table_icon_column_width = 0;
	std::string_view needs_table_name_header_text_key;
	uint8_t needs_table_name_header_text_color = 0;
	uint8_t needs_table_name_column_text_color = 0;
	text::alignment needs_table_name_text_alignment = text::alignment::left;
	int8_t needs_table_name_sort_direction = 0;
	int16_t needs_table_name_column_start = 0;
	int16_t needs_table_name_column_width = 0;
	std::string_view needs_table_amount_header_text_key;
	uint8_t needs_table_amount_header_text_color = 0;
	uint8_t needs_table_amount_column_text_color = 0;
	text::alignment needs_table_amount_text_alignment = text::alignment::left;
	int8_t needs_table_amount_sort_direction = 0;
	int16_t needs_table_amount_column_start = 0;
	int16_t needs_table_amount_column_width = 0;
	std::string_view needs_table_cost_header_text_key;
	uint8_t needs_table_cost_header_text_color = 0;
	uint8_t needs_table_cost_column_text_color = 0;
	text::alignment needs_table_cost_text_alignment = text::alignment::left;
	int8_t needs_table_cost_sort_direction = 0;
	int16_t needs_table_cost_column_start = 0;
	int16_t needs_table_cost_column_width = 0;
	std::string_view needs_table_weight_header_text_key;
	std::string_view needs_table_weight_header_tooltip_key;
	uint8_t needs_table_weight_header_text_color = 0;
	uint8_t needs_table_weight_column_text_color = 0;
	text::alignment needs_table_weight_text_alignment = text::alignment::center;
	int8_t needs_table_weight_sort_direction = 0;
	int16_t needs_table_weight_column_start = 0;
	int16_t needs_table_weight_column_width = 0;
	std::string_view needs_table_ascending_icon_key;
	dcon::texture_id needs_table_ascending_icon;
	std::string_view needs_table_descending_icon_key;
	dcon::texture_id needs_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_details_needs_header(sys::state& state);
struct pop_details_prom_header_t : public layout_window_element {
// BEGIN prom_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_prom_header_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t prom_table_icon_header_text_color = 0;
	uint8_t prom_table_icon_column_text_color = 0;
	text::alignment prom_table_icon_text_alignment = text::alignment::center;
	int16_t prom_table_icon_column_start = 0;
	int16_t prom_table_icon_column_width = 0;
	std::string_view prom_table_name_header_text_key;
	uint8_t prom_table_name_header_text_color = 0;
	uint8_t prom_table_name_column_text_color = 0;
	text::alignment prom_table_name_text_alignment = text::alignment::right;
	int8_t prom_table_name_sort_direction = 0;
	int16_t prom_table_name_column_start = 0;
	int16_t prom_table_name_column_width = 0;
	std::string_view prom_table_weight_header_text_key;
	uint8_t prom_table_weight_header_text_color = 0;
	uint8_t prom_table_weight_column_text_color = 0;
	text::alignment prom_table_weight_text_alignment = text::alignment::right;
	int8_t prom_table_weight_sort_direction = 0;
	int16_t prom_table_weight_column_start = 0;
	int16_t prom_table_weight_column_width = 0;
	std::string_view prom_table_ascending_icon_key;
	dcon::texture_id prom_table_ascending_icon;
	std::string_view prom_table_descending_icon_key;
	dcon::texture_id prom_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_details_prom_header(sys::state& state);
struct pop_details_prom_row_t : public layout_window_element {
// BEGIN prom_row::variables
// END
	dcon::pop_type_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_prom_row_content_t> content;
	std::unique_ptr<pop_details_prom_row_job_icon_t> job_icon;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t prom_table_icon_header_text_color = 0;
	uint8_t prom_table_icon_column_text_color = 0;
	text::alignment prom_table_icon_text_alignment = text::alignment::center;
	int16_t prom_table_icon_column_start = 0;
	int16_t prom_table_icon_column_width = 0;
	std::string_view prom_table_name_header_text_key;
	uint8_t prom_table_name_header_text_color = 0;
	uint8_t prom_table_name_column_text_color = 0;
	text::alignment prom_table_name_text_alignment = text::alignment::right;
	int8_t prom_table_name_sort_direction = 0;
	int16_t prom_table_name_column_start = 0;
	int16_t prom_table_name_column_width = 0;
	std::string_view prom_table_weight_header_text_key;
	uint8_t prom_table_weight_header_text_color = 0;
	uint8_t prom_table_weight_column_text_color = 0;
	text::alignment prom_table_weight_text_alignment = text::alignment::right;
	int8_t prom_table_weight_sort_direction = 0;
	int16_t prom_table_weight_column_start = 0;
	int16_t prom_table_weight_column_width = 0;
	std::string_view prom_table_ascending_icon_key;
	dcon::texture_id prom_table_ascending_icon;
	std::string_view prom_table_descending_icon_key;
	dcon::texture_id prom_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
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
std::unique_ptr<ui::element_base> make_pop_details_prom_row(sys::state& state);
struct pop_details_weights_header_t : public layout_window_element {
// BEGIN weights_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_weights_header_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view weights_table_item_header_text_key;
	uint8_t weights_table_item_header_text_color = 0;
	uint8_t weights_table_item_column_text_color = 0;
	text::alignment weights_table_item_text_alignment = text::alignment::right;
	int8_t weights_table_item_sort_direction = 0;
	int16_t weights_table_item_column_start = 0;
	int16_t weights_table_item_column_width = 0;
	std::string_view weights_table_weight_header_text_key;
	uint8_t weights_table_weight_header_text_color = 0;
	uint8_t weights_table_weight_column_text_color = 0;
	text::alignment weights_table_weight_text_alignment = text::alignment::right;
	int8_t weights_table_weight_sort_direction = 0;
	int16_t weights_table_weight_column_start = 0;
	int16_t weights_table_weight_column_width = 0;
	std::string_view weights_table_ascending_icon_key;
	dcon::texture_id weights_table_ascending_icon;
	std::string_view weights_table_descending_icon_key;
	dcon::texture_id weights_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_details_weights_header(sys::state& state);
struct pop_details_weights_row_t : public layout_window_element {
// BEGIN weights_row::variables
// END
	std::string item_name;
	float weight_value;
	std::variant<dcon::ideology_id, dcon::issue_option_id> source;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_weights_row_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view weights_table_item_header_text_key;
	uint8_t weights_table_item_header_text_color = 0;
	uint8_t weights_table_item_column_text_color = 0;
	text::alignment weights_table_item_text_alignment = text::alignment::right;
	int8_t weights_table_item_sort_direction = 0;
	int16_t weights_table_item_column_start = 0;
	int16_t weights_table_item_column_width = 0;
	std::string_view weights_table_weight_header_text_key;
	uint8_t weights_table_weight_header_text_color = 0;
	uint8_t weights_table_weight_column_text_color = 0;
	text::alignment weights_table_weight_text_alignment = text::alignment::right;
	int8_t weights_table_weight_sort_direction = 0;
	int16_t weights_table_weight_column_start = 0;
	int16_t weights_table_weight_column_width = 0;
	std::string_view weights_table_ascending_icon_key;
	dcon::texture_id weights_table_ascending_icon;
	std::string_view weights_table_descending_icon_key;
	dcon::texture_id weights_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "item_name") {
			return (void*)(&item_name);
		}
		if(name_parameter == "weight_value") {
			return (void*)(&weight_value);
		}
		if(name_parameter == "source") {
			return (void*)(&source);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_details_weights_row(sys::state& state);
struct pop_details_emm_header_t : public layout_window_element {
// BEGIN emm_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_emm_header_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	int16_t emm_table_flag_column_start = 0;
	int16_t emm_table_flag_column_width = 0;
	std::string_view emm_table_destination_header_text_key;
	uint8_t emm_table_destination_header_text_color = 0;
	uint8_t emm_table_destination_column_text_color = 0;
	text::alignment emm_table_destination_text_alignment = text::alignment::right;
	int8_t emm_table_destination_sort_direction = 0;
	int16_t emm_table_destination_column_start = 0;
	int16_t emm_table_destination_column_width = 0;
	std::string_view emm_table_weight_header_text_key;
	uint8_t emm_table_weight_header_text_color = 0;
	uint8_t emm_table_weight_column_text_color = 0;
	text::alignment emm_table_weight_text_alignment = text::alignment::right;
	int8_t emm_table_weight_sort_direction = 0;
	int16_t emm_table_weight_column_start = 0;
	int16_t emm_table_weight_column_width = 0;
	std::string_view emm_table_ascending_icon_key;
	dcon::texture_id emm_table_ascending_icon;
	std::string_view emm_table_descending_icon_key;
	dcon::texture_id emm_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_details_emm_header(sys::state& state);
struct pop_details_emm_row_t : public layout_window_element {
// BEGIN emm_row::variables
// END
	dcon::nation_id destination;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_emm_row_content_t> content;
	std::unique_ptr<pop_details_emm_row_dest_flag_t> dest_flag;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	int16_t emm_table_flag_column_start = 0;
	int16_t emm_table_flag_column_width = 0;
	std::string_view emm_table_destination_header_text_key;
	uint8_t emm_table_destination_header_text_color = 0;
	uint8_t emm_table_destination_column_text_color = 0;
	text::alignment emm_table_destination_text_alignment = text::alignment::right;
	int8_t emm_table_destination_sort_direction = 0;
	int16_t emm_table_destination_column_start = 0;
	int16_t emm_table_destination_column_width = 0;
	std::string_view emm_table_weight_header_text_key;
	uint8_t emm_table_weight_header_text_color = 0;
	uint8_t emm_table_weight_column_text_color = 0;
	text::alignment emm_table_weight_text_alignment = text::alignment::right;
	int8_t emm_table_weight_sort_direction = 0;
	int16_t emm_table_weight_column_start = 0;
	int16_t emm_table_weight_column_width = 0;
	std::string_view emm_table_ascending_icon_key;
	dcon::texture_id emm_table_ascending_icon;
	std::string_view emm_table_descending_icon_key;
	dcon::texture_id emm_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "destination") {
			return (void*)(&destination);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_details_emm_row(sys::state& state);
struct pop_details_mig_header_t : public layout_window_element {
// BEGIN mig_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_mig_header_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view mig_table_destination_header_text_key;
	uint8_t mig_table_destination_header_text_color = 0;
	uint8_t mig_table_destination_column_text_color = 0;
	text::alignment mig_table_destination_text_alignment = text::alignment::right;
	int8_t mig_table_destination_sort_direction = 0;
	int16_t mig_table_destination_column_start = 0;
	int16_t mig_table_destination_column_width = 0;
	std::string_view mig_table_weight_header_text_key;
	uint8_t mig_table_weight_header_text_color = 0;
	uint8_t mig_table_weight_column_text_color = 0;
	text::alignment mig_table_weight_text_alignment = text::alignment::right;
	int8_t mig_table_weight_sort_direction = 0;
	int16_t mig_table_weight_column_start = 0;
	int16_t mig_table_weight_column_width = 0;
	std::string_view mig_table_ascending_icon_key;
	dcon::texture_id mig_table_ascending_icon;
	std::string_view mig_table_descending_icon_key;
	dcon::texture_id mig_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_details_mig_header(sys::state& state);
struct pop_details_mig_row_t : public layout_window_element {
// BEGIN mig_row::variables
// END
	dcon::province_id destination;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<pop_details_mig_row_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	std::string_view mig_table_destination_header_text_key;
	uint8_t mig_table_destination_header_text_color = 0;
	uint8_t mig_table_destination_column_text_color = 0;
	text::alignment mig_table_destination_text_alignment = text::alignment::right;
	int8_t mig_table_destination_sort_direction = 0;
	int16_t mig_table_destination_column_start = 0;
	int16_t mig_table_destination_column_width = 0;
	std::string_view mig_table_weight_header_text_key;
	uint8_t mig_table_weight_header_text_color = 0;
	uint8_t mig_table_weight_column_text_color = 0;
	text::alignment mig_table_weight_text_alignment = text::alignment::right;
	int8_t mig_table_weight_sort_direction = 0;
	int16_t mig_table_weight_column_start = 0;
	int16_t mig_table_weight_column_width = 0;
	std::string_view mig_table_ascending_icon_key;
	dcon::texture_id mig_table_ascending_icon;
	std::string_view mig_table_descending_icon_key;
	dcon::texture_id mig_table_descending_icon;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "destination") {
			return (void*)(&destination);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_pop_details_mig_row(sys::state& state);
struct pop_details_fancy_separator_1_t : public layout_window_element {
// BEGIN fancy_separator_1::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_details_fancy_separator_1(sys::state& state);
struct pop_details_fancy_separator_2_t : public layout_window_element {
// BEGIN fancy_separator_2::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_pop_details_fancy_separator_2(sys::state& state);
void pop_details_main_ln_list_t::add_needs_row(dcon::commodity_id commodity, float amount, float weight) {
	values.emplace_back(needs_row_option{commodity, amount, weight});
}
void  pop_details_main_ln_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::ln_list::on_create
// END
}
void  pop_details_main_ln_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::ln_list::update
	auto l = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto s = state.world.province_get_state_membership(l);
	auto m = state.world.state_instance_get_market_from_local_market(s);

	values.clear();
	auto type = state.world.pop_get_poptype(main.for_pop);
	for(auto c : state.world.in_commodity) {
		auto v = state.world.pop_type_get_life_needs(type, c);
		if(v > 0) {
			add_needs_row(c.id, v, state.world.market_get_life_needs_weights(m, c));
		}
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->needs_table_name_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_amount_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_cost_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<needs_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<needs_row_option>(values[i])) ++i;
				if(table_source->needs_table_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::ln_list::needs_table::sort::name
						result = cmp3(text::produce_simple_string(state, state.world.commodity_get_name(a.commodity)), text::produce_simple_string(state, state.world.commodity_get_name(b.commodity)));
// END
						return -result == table_source->needs_table_name_sort_direction;
					});
				}
				if(table_source->needs_table_amount_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::ln_list::needs_table::sort::amount
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.pop_type_get_life_needs(type, a.commodity), state.world.pop_type_get_life_needs(type, b.commodity));
// END
						return -result == table_source->needs_table_amount_sort_direction;
					});
				}
				if(table_source->needs_table_cost_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::ln_list::needs_table::sort::cost
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.market_get_price(m, a.commodity) * state.world.pop_type_get_life_needs(type, a.commodity), state.world.market_get_price(m, b.commodity) * state.world.pop_type_get_life_needs(type, b.commodity));
// END
						return -result == table_source->needs_table_cost_sort_direction;
					});
				}
				if(table_source->needs_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::ln_list::needs_table::sort::weight
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.market_get_life_needs_weights(m, a.commodity), state.world.market_get_life_needs_weights(m, b.commodity));
// END
						return -result == table_source->needs_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_ln_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<needs_row_option>(values[index])) {
		if(needs_header_pool.empty()) needs_header_pool.emplace_back(make_pop_details_needs_header(state));
		if(needs_row_pool.empty()) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<needs_row_option>(values[index - 1]))) {
			if(destination) {
				if(needs_header_pool.size() <= size_t(needs_header_pool_used)) needs_header_pool.emplace_back(make_pop_details_needs_header(state));
				if(needs_row_pool.size() <= size_t(needs_row_pool_used)) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
				needs_header_pool[needs_header_pool_used]->base_data.position.x = int16_t(x);
				needs_header_pool[needs_header_pool_used]->base_data.position.y = int16_t(y);
				if(!needs_header_pool[needs_header_pool_used]->parent) {
					needs_header_pool[needs_header_pool_used]->parent = destination;
					needs_header_pool[needs_header_pool_used]->impl_on_update(state);
					needs_header_pool[needs_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(needs_header_pool[needs_header_pool_used].get());
			((pop_details_needs_header_t*)(needs_header_pool[needs_header_pool_used].get()))->set_alternate(alternate);
				needs_row_pool[needs_row_pool_used]->base_data.position.x = int16_t(x);
				needs_row_pool[needs_row_pool_used]->base_data.position.y = int16_t(y +  needs_row_pool[0]->base_data.size.y + 0);
				needs_row_pool[needs_row_pool_used]->parent = destination;
				destination->children.push_back(needs_row_pool[needs_row_pool_used].get());
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->commodity = std::get<needs_row_option>(values[index]).commodity;
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->amount = std::get<needs_row_option>(values[index]).amount;
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->weight = std::get<needs_row_option>(values[index]).weight;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->set_alternate(!alternate);
				needs_row_pool[needs_row_pool_used]->impl_on_update(state);
				needs_header_pool_used++;
				needs_row_pool_used++;
			}
			return measure_result{std::max(needs_header_pool[0]->base_data.size.x, needs_row_pool[0]->base_data.size.x), needs_header_pool[0]->base_data.size.y + needs_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(needs_row_pool.size() <= size_t(needs_row_pool_used)) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
			needs_row_pool[needs_row_pool_used]->base_data.position.x = int16_t(x);
			needs_row_pool[needs_row_pool_used]->base_data.position.y = int16_t(y);
			needs_row_pool[needs_row_pool_used]->parent = destination;
			destination->children.push_back(needs_row_pool[needs_row_pool_used].get());
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->commodity = std::get<needs_row_option>(values[index]).commodity;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->amount = std::get<needs_row_option>(values[index]).amount;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->weight = std::get<needs_row_option>(values[index]).weight;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->set_alternate(alternate);
			needs_row_pool[needs_row_pool_used]->impl_on_update(state);
			needs_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ needs_row_pool[0]->base_data.size.x, needs_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_ln_list_t::reset_pools() {
	needs_header_pool_used = 0;
	needs_row_pool_used = 0;
}
void pop_details_main_en_list_t::add_needs_row(dcon::commodity_id commodity, float amount, float weight) {
	values.emplace_back(needs_row_option{commodity, amount, weight});
}
void  pop_details_main_en_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::en_list::on_create
// END
}
void  pop_details_main_en_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::en_list::update
	auto l = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto s = state.world.province_get_state_membership(l);
	auto m = state.world.state_instance_get_market_from_local_market(s);

	values.clear();
	auto type = state.world.pop_get_poptype(main.for_pop);
	for(auto c : state.world.in_commodity) {
		auto v = state.world.pop_type_get_everyday_needs(type, c);
		if(v > 0) {
			add_needs_row(c.id, v, state.world.market_get_everyday_needs_weights(m, c));
		}
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->needs_table_name_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_amount_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_cost_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<needs_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<needs_row_option>(values[i])) ++i;
				if(table_source->needs_table_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::en_list::needs_table::sort::name
						result = cmp3(text::produce_simple_string(state, state.world.commodity_get_name(a.commodity)), text::produce_simple_string(state, state.world.commodity_get_name(b.commodity)));
// END
						return -result == table_source->needs_table_name_sort_direction;
					});
				}
				if(table_source->needs_table_amount_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::en_list::needs_table::sort::amount
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.pop_type_get_everyday_needs(type, a.commodity), state.world.pop_type_get_everyday_needs(type, b.commodity));
// END
						return -result == table_source->needs_table_amount_sort_direction;
					});
				}
				if(table_source->needs_table_cost_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::en_list::needs_table::sort::cost
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.market_get_price(m, a.commodity) * state.world.pop_type_get_everyday_needs(type, a.commodity), state.world.market_get_price(m, b.commodity) * state.world.pop_type_get_everyday_needs(type, b.commodity));
// END
						return -result == table_source->needs_table_cost_sort_direction;
					});
				}
				if(table_source->needs_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::en_list::needs_table::sort::weight
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.market_get_everyday_needs_weights(m, a.commodity), state.world.market_get_everyday_needs_weights(m, b.commodity));
// END
						return -result == table_source->needs_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_en_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<needs_row_option>(values[index])) {
		if(needs_header_pool.empty()) needs_header_pool.emplace_back(make_pop_details_needs_header(state));
		if(needs_row_pool.empty()) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<needs_row_option>(values[index - 1]))) {
			if(destination) {
				if(needs_header_pool.size() <= size_t(needs_header_pool_used)) needs_header_pool.emplace_back(make_pop_details_needs_header(state));
				if(needs_row_pool.size() <= size_t(needs_row_pool_used)) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
				needs_header_pool[needs_header_pool_used]->base_data.position.x = int16_t(x);
				needs_header_pool[needs_header_pool_used]->base_data.position.y = int16_t(y);
				if(!needs_header_pool[needs_header_pool_used]->parent) {
					needs_header_pool[needs_header_pool_used]->parent = destination;
					needs_header_pool[needs_header_pool_used]->impl_on_update(state);
					needs_header_pool[needs_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(needs_header_pool[needs_header_pool_used].get());
			((pop_details_needs_header_t*)(needs_header_pool[needs_header_pool_used].get()))->set_alternate(alternate);
				needs_row_pool[needs_row_pool_used]->base_data.position.x = int16_t(x);
				needs_row_pool[needs_row_pool_used]->base_data.position.y = int16_t(y +  needs_row_pool[0]->base_data.size.y + 0);
				needs_row_pool[needs_row_pool_used]->parent = destination;
				destination->children.push_back(needs_row_pool[needs_row_pool_used].get());
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->commodity = std::get<needs_row_option>(values[index]).commodity;
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->amount = std::get<needs_row_option>(values[index]).amount;
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->weight = std::get<needs_row_option>(values[index]).weight;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->set_alternate(!alternate);
				needs_row_pool[needs_row_pool_used]->impl_on_update(state);
				needs_header_pool_used++;
				needs_row_pool_used++;
			}
			return measure_result{std::max(needs_header_pool[0]->base_data.size.x, needs_row_pool[0]->base_data.size.x), needs_header_pool[0]->base_data.size.y + needs_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(needs_row_pool.size() <= size_t(needs_row_pool_used)) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
			needs_row_pool[needs_row_pool_used]->base_data.position.x = int16_t(x);
			needs_row_pool[needs_row_pool_used]->base_data.position.y = int16_t(y);
			needs_row_pool[needs_row_pool_used]->parent = destination;
			destination->children.push_back(needs_row_pool[needs_row_pool_used].get());
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->commodity = std::get<needs_row_option>(values[index]).commodity;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->amount = std::get<needs_row_option>(values[index]).amount;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->weight = std::get<needs_row_option>(values[index]).weight;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->set_alternate(alternate);
			needs_row_pool[needs_row_pool_used]->impl_on_update(state);
			needs_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ needs_row_pool[0]->base_data.size.x, needs_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_en_list_t::reset_pools() {
	needs_header_pool_used = 0;
	needs_row_pool_used = 0;
}
void pop_details_main_lx_list_t::add_needs_row(dcon::commodity_id commodity, float amount, float weight) {
	values.emplace_back(needs_row_option{commodity, amount, weight});
}
void  pop_details_main_lx_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::lx_list::on_create
// END
}
void  pop_details_main_lx_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::lx_list::update
	auto l = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto s = state.world.province_get_state_membership(l);
	auto m = state.world.state_instance_get_market_from_local_market(s);

	values.clear();
	auto type = state.world.pop_get_poptype(main.for_pop);
	for(auto c : state.world.in_commodity) {
		auto v = state.world.pop_type_get_luxury_needs(type, c);
		if(v > 0) {
			add_needs_row(c.id, v, state.world.market_get_luxury_needs_weights(m, c));
		}
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->needs_table_name_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_amount_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_cost_sort_direction != 0) work_to_do = true;
	if(table_source->needs_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<needs_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<needs_row_option>(values[i])) ++i;
				if(table_source->needs_table_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::lx_list::needs_table::sort::name
						result = cmp3(text::produce_simple_string(state, state.world.commodity_get_name(a.commodity)), text::produce_simple_string(state, state.world.commodity_get_name(b.commodity)));
// END
						return -result == table_source->needs_table_name_sort_direction;
					});
				}
				if(table_source->needs_table_amount_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::lx_list::needs_table::sort::amount
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.pop_type_get_luxury_needs(type, a.commodity), state.world.pop_type_get_luxury_needs(type, b.commodity));
// END
						return -result == table_source->needs_table_amount_sort_direction;
					});
				}
				if(table_source->needs_table_cost_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::lx_list::needs_table::sort::cost
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.market_get_price(m, a.commodity) * state.world.pop_type_get_luxury_needs(type, a.commodity), state.world.market_get_price(m, b.commodity) * state.world.pop_type_get_luxury_needs(type, b.commodity));
// END
						return -result == table_source->needs_table_cost_sort_direction;
					});
				}
				if(table_source->needs_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<needs_row_option>(raw_a);
						auto const& b = std::get<needs_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::lx_list::needs_table::sort::weight
						auto type = state.world.pop_get_poptype(main.for_pop);
						result = cmp3(state.world.market_get_luxury_needs_weights(m, a.commodity), state.world.market_get_luxury_needs_weights(m, b.commodity));
// END
						return -result == table_source->needs_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_lx_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<needs_row_option>(values[index])) {
		if(needs_header_pool.empty()) needs_header_pool.emplace_back(make_pop_details_needs_header(state));
		if(needs_row_pool.empty()) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<needs_row_option>(values[index - 1]))) {
			if(destination) {
				if(needs_header_pool.size() <= size_t(needs_header_pool_used)) needs_header_pool.emplace_back(make_pop_details_needs_header(state));
				if(needs_row_pool.size() <= size_t(needs_row_pool_used)) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
				needs_header_pool[needs_header_pool_used]->base_data.position.x = int16_t(x);
				needs_header_pool[needs_header_pool_used]->base_data.position.y = int16_t(y);
				if(!needs_header_pool[needs_header_pool_used]->parent) {
					needs_header_pool[needs_header_pool_used]->parent = destination;
					needs_header_pool[needs_header_pool_used]->impl_on_update(state);
					needs_header_pool[needs_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(needs_header_pool[needs_header_pool_used].get());
			((pop_details_needs_header_t*)(needs_header_pool[needs_header_pool_used].get()))->set_alternate(alternate);
				needs_row_pool[needs_row_pool_used]->base_data.position.x = int16_t(x);
				needs_row_pool[needs_row_pool_used]->base_data.position.y = int16_t(y +  needs_row_pool[0]->base_data.size.y + 0);
				needs_row_pool[needs_row_pool_used]->parent = destination;
				destination->children.push_back(needs_row_pool[needs_row_pool_used].get());
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->commodity = std::get<needs_row_option>(values[index]).commodity;
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->amount = std::get<needs_row_option>(values[index]).amount;
				((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->weight = std::get<needs_row_option>(values[index]).weight;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->set_alternate(!alternate);
				needs_row_pool[needs_row_pool_used]->impl_on_update(state);
				needs_header_pool_used++;
				needs_row_pool_used++;
			}
			return measure_result{std::max(needs_header_pool[0]->base_data.size.x, needs_row_pool[0]->base_data.size.x), needs_header_pool[0]->base_data.size.y + needs_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(needs_row_pool.size() <= size_t(needs_row_pool_used)) needs_row_pool.emplace_back(make_pop_details_needs_row(state));
			needs_row_pool[needs_row_pool_used]->base_data.position.x = int16_t(x);
			needs_row_pool[needs_row_pool_used]->base_data.position.y = int16_t(y);
			needs_row_pool[needs_row_pool_used]->parent = destination;
			destination->children.push_back(needs_row_pool[needs_row_pool_used].get());
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->commodity = std::get<needs_row_option>(values[index]).commodity;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->amount = std::get<needs_row_option>(values[index]).amount;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->weight = std::get<needs_row_option>(values[index]).weight;
			((pop_details_needs_row_t*)(needs_row_pool[needs_row_pool_used].get()))->set_alternate(alternate);
			needs_row_pool[needs_row_pool_used]->impl_on_update(state);
			needs_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ needs_row_pool[0]->base_data.size.x, needs_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_lx_list_t::reset_pools() {
	needs_header_pool_used = 0;
	needs_row_pool_used = 0;
}
void pop_details_main_prom_list_t::add_prom_row(dcon::pop_type_id value) {
	values.emplace_back(prom_row_option{value});
}
void  pop_details_main_prom_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::prom_list::on_create
// END
}
void  pop_details_main_prom_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::prom_list::update
	values.clear();
	auto type = state.world.pop_get_poptype(main.for_pop);
	for(auto t : state.world.in_pop_type) {
		if(t.get_strata() >= type.get_strata() && t != type) {
			auto r = state.world.pop_type_get_promotion(type, t);
			if(r)
				add_prom_row(t.id);
		}
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->prom_table_name_sort_direction != 0) work_to_do = true;
	if(table_source->prom_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<prom_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<prom_row_option>(values[i])) ++i;
				if(table_source->prom_table_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<prom_row_option>(raw_a);
						auto const& b = std::get<prom_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::prom_list::prom_table::sort::name
						result = cmp3(text::produce_simple_string(state, state.world.pop_type_get_name(a.value)), text::produce_simple_string(state, state.world.pop_type_get_name(b.value)));
// END
						return -result == table_source->prom_table_name_sort_direction;
					});
				}
				if(table_source->prom_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<prom_row_option>(raw_a);
						auto const& b = std::get<prom_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::prom_list::prom_table::sort::weight
						auto type = state.world.pop_get_poptype(main.for_pop);
						auto aw = std::max(0.0f, trigger::evaluate_additive_modifier(state, state.world.pop_type_get_promotion(type, a.value), trigger::to_generic(main.for_pop), trigger::to_generic(main.for_pop), 0));
						auto bw = std::max(0.0f, trigger::evaluate_additive_modifier(state, state.world.pop_type_get_promotion(type, b.value), trigger::to_generic(main.for_pop), trigger::to_generic(main.for_pop), 0));
						result = cmp3(aw, bw);
// END
						return -result == table_source->prom_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_prom_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<prom_row_option>(values[index])) {
		if(prom_header_pool.empty()) prom_header_pool.emplace_back(make_pop_details_prom_header(state));
		if(prom_row_pool.empty()) prom_row_pool.emplace_back(make_pop_details_prom_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<prom_row_option>(values[index - 1]))) {
			if(destination) {
				if(prom_header_pool.size() <= size_t(prom_header_pool_used)) prom_header_pool.emplace_back(make_pop_details_prom_header(state));
				if(prom_row_pool.size() <= size_t(prom_row_pool_used)) prom_row_pool.emplace_back(make_pop_details_prom_row(state));
				prom_header_pool[prom_header_pool_used]->base_data.position.x = int16_t(x);
				prom_header_pool[prom_header_pool_used]->base_data.position.y = int16_t(y);
				if(!prom_header_pool[prom_header_pool_used]->parent) {
					prom_header_pool[prom_header_pool_used]->parent = destination;
					prom_header_pool[prom_header_pool_used]->impl_on_update(state);
					prom_header_pool[prom_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(prom_header_pool[prom_header_pool_used].get());
			((pop_details_prom_header_t*)(prom_header_pool[prom_header_pool_used].get()))->set_alternate(alternate);
				prom_row_pool[prom_row_pool_used]->base_data.position.x = int16_t(x);
				prom_row_pool[prom_row_pool_used]->base_data.position.y = int16_t(y +  prom_row_pool[0]->base_data.size.y + 0);
				prom_row_pool[prom_row_pool_used]->parent = destination;
				destination->children.push_back(prom_row_pool[prom_row_pool_used].get());
				((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->value = std::get<prom_row_option>(values[index]).value;
			((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->set_alternate(!alternate);
				prom_row_pool[prom_row_pool_used]->impl_on_update(state);
				prom_header_pool_used++;
				prom_row_pool_used++;
			}
			return measure_result{std::max(prom_header_pool[0]->base_data.size.x, prom_row_pool[0]->base_data.size.x), prom_header_pool[0]->base_data.size.y + prom_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(prom_row_pool.size() <= size_t(prom_row_pool_used)) prom_row_pool.emplace_back(make_pop_details_prom_row(state));
			prom_row_pool[prom_row_pool_used]->base_data.position.x = int16_t(x);
			prom_row_pool[prom_row_pool_used]->base_data.position.y = int16_t(y);
			prom_row_pool[prom_row_pool_used]->parent = destination;
			destination->children.push_back(prom_row_pool[prom_row_pool_used].get());
			((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->value = std::get<prom_row_option>(values[index]).value;
			((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->set_alternate(alternate);
			prom_row_pool[prom_row_pool_used]->impl_on_update(state);
			prom_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ prom_row_pool[0]->base_data.size.x, prom_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_prom_list_t::reset_pools() {
	prom_header_pool_used = 0;
	prom_row_pool_used = 0;
}
void pop_details_main_dem_list_t::add_prom_row(dcon::pop_type_id value) {
	values.emplace_back(prom_row_option{value});
}
void  pop_details_main_dem_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::dem_list::on_create
// END
}
void  pop_details_main_dem_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::dem_list::update
	values.clear();
	auto type = state.world.pop_get_poptype(main.for_pop);
	for(auto t : state.world.in_pop_type) {
		if(t.get_strata() <= type.get_strata() && t != type) {
			auto r = state.world.pop_type_get_promotion(type, t);
			if(r)
				add_prom_row(t.id);
		}
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->prom_table_name_sort_direction != 0) work_to_do = true;
	if(table_source->prom_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<prom_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<prom_row_option>(values[i])) ++i;
				if(table_source->prom_table_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<prom_row_option>(raw_a);
						auto const& b = std::get<prom_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::dem_list::prom_table::sort::name
						result = cmp3(text::produce_simple_string(state, state.world.pop_type_get_name(a.value)), text::produce_simple_string(state, state.world.pop_type_get_name(b.value)));
// END
						return -result == table_source->prom_table_name_sort_direction;
					});
				}
				if(table_source->prom_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<prom_row_option>(raw_a);
						auto const& b = std::get<prom_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::dem_list::prom_table::sort::weight
						auto type = state.world.pop_get_poptype(main.for_pop);
						auto aw = std::max(0.0f, trigger::evaluate_additive_modifier(state, state.world.pop_type_get_promotion(type, a.value), trigger::to_generic(main.for_pop), trigger::to_generic(main.for_pop), 0));
						auto bw = std::max(0.0f, trigger::evaluate_additive_modifier(state, state.world.pop_type_get_promotion(type, b.value), trigger::to_generic(main.for_pop), trigger::to_generic(main.for_pop), 0));
						result = cmp3(aw, bw);
// END
						return -result == table_source->prom_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_dem_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<prom_row_option>(values[index])) {
		if(prom_header_pool.empty()) prom_header_pool.emplace_back(make_pop_details_prom_header(state));
		if(prom_row_pool.empty()) prom_row_pool.emplace_back(make_pop_details_prom_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<prom_row_option>(values[index - 1]))) {
			if(destination) {
				if(prom_header_pool.size() <= size_t(prom_header_pool_used)) prom_header_pool.emplace_back(make_pop_details_prom_header(state));
				if(prom_row_pool.size() <= size_t(prom_row_pool_used)) prom_row_pool.emplace_back(make_pop_details_prom_row(state));
				prom_header_pool[prom_header_pool_used]->base_data.position.x = int16_t(x);
				prom_header_pool[prom_header_pool_used]->base_data.position.y = int16_t(y);
				if(!prom_header_pool[prom_header_pool_used]->parent) {
					prom_header_pool[prom_header_pool_used]->parent = destination;
					prom_header_pool[prom_header_pool_used]->impl_on_update(state);
					prom_header_pool[prom_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(prom_header_pool[prom_header_pool_used].get());
			((pop_details_prom_header_t*)(prom_header_pool[prom_header_pool_used].get()))->set_alternate(alternate);
				prom_row_pool[prom_row_pool_used]->base_data.position.x = int16_t(x);
				prom_row_pool[prom_row_pool_used]->base_data.position.y = int16_t(y +  prom_row_pool[0]->base_data.size.y + 0);
				prom_row_pool[prom_row_pool_used]->parent = destination;
				destination->children.push_back(prom_row_pool[prom_row_pool_used].get());
				((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->value = std::get<prom_row_option>(values[index]).value;
			((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->set_alternate(!alternate);
				prom_row_pool[prom_row_pool_used]->impl_on_update(state);
				prom_header_pool_used++;
				prom_row_pool_used++;
			}
			return measure_result{std::max(prom_header_pool[0]->base_data.size.x, prom_row_pool[0]->base_data.size.x), prom_header_pool[0]->base_data.size.y + prom_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(prom_row_pool.size() <= size_t(prom_row_pool_used)) prom_row_pool.emplace_back(make_pop_details_prom_row(state));
			prom_row_pool[prom_row_pool_used]->base_data.position.x = int16_t(x);
			prom_row_pool[prom_row_pool_used]->base_data.position.y = int16_t(y);
			prom_row_pool[prom_row_pool_used]->parent = destination;
			destination->children.push_back(prom_row_pool[prom_row_pool_used].get());
			((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->value = std::get<prom_row_option>(values[index]).value;
			((pop_details_prom_row_t*)(prom_row_pool[prom_row_pool_used].get()))->set_alternate(alternate);
			prom_row_pool[prom_row_pool_used]->impl_on_update(state);
			prom_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ prom_row_pool[0]->base_data.size.x, prom_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_dem_list_t::reset_pools() {
	prom_header_pool_used = 0;
	prom_row_pool_used = 0;
}
void pop_details_main_ideo_list_t::add_weights_row(std::string item_name, float weight_value, std::variant<dcon::ideology_id, dcon::issue_option_id> source) {
	values.emplace_back(weights_row_option{item_name, weight_value, source});
}
void  pop_details_main_ideo_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::ideo_list::on_create
// END
}
void  pop_details_main_ideo_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::ideo_list::update
	values.clear();
	auto type = state.world.pop_get_poptype(main.for_pop);
	for(auto i : state.world.in_ideology) {
		auto ptrigger = state.world.pop_type_get_ideology(type, i);
		if(ptrigger && (!i.get_is_civilized_only() || state.world.nation_get_is_civilized(nations::owner_of_pop(state, main.for_pop))) && i.get_enabled())
			add_weights_row(text::produce_simple_string(state, i.get_name()), trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(main.for_pop), trigger::to_generic(main.for_pop), 0), i.id);
		else
			add_weights_row(text::produce_simple_string(state, i.get_name()), 0.0f, i.id);
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->weights_table_item_sort_direction != 0) work_to_do = true;
	if(table_source->weights_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<weights_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<weights_row_option>(values[i])) ++i;
				if(table_source->weights_table_item_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<weights_row_option>(raw_a);
						auto const& b = std::get<weights_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::ideo_list::weights_table::sort::item
						result = cmp3(a.item_name, b.item_name);
// END
						return -result == table_source->weights_table_item_sort_direction;
					});
				}
				if(table_source->weights_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<weights_row_option>(raw_a);
						auto const& b = std::get<weights_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::ideo_list::weights_table::sort::weight
						result = cmp3(a.weight_value, b.weight_value);
// END
						return -result == table_source->weights_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_ideo_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<weights_row_option>(values[index])) {
		if(weights_header_pool.empty()) weights_header_pool.emplace_back(make_pop_details_weights_header(state));
		if(weights_row_pool.empty()) weights_row_pool.emplace_back(make_pop_details_weights_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<weights_row_option>(values[index - 1]))) {
			if(destination) {
				if(weights_header_pool.size() <= size_t(weights_header_pool_used)) weights_header_pool.emplace_back(make_pop_details_weights_header(state));
				if(weights_row_pool.size() <= size_t(weights_row_pool_used)) weights_row_pool.emplace_back(make_pop_details_weights_row(state));
				weights_header_pool[weights_header_pool_used]->base_data.position.x = int16_t(x);
				weights_header_pool[weights_header_pool_used]->base_data.position.y = int16_t(y);
				if(!weights_header_pool[weights_header_pool_used]->parent) {
					weights_header_pool[weights_header_pool_used]->parent = destination;
					weights_header_pool[weights_header_pool_used]->impl_on_update(state);
					weights_header_pool[weights_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(weights_header_pool[weights_header_pool_used].get());
			((pop_details_weights_header_t*)(weights_header_pool[weights_header_pool_used].get()))->set_alternate(alternate);
				weights_row_pool[weights_row_pool_used]->base_data.position.x = int16_t(x);
				weights_row_pool[weights_row_pool_used]->base_data.position.y = int16_t(y +  weights_row_pool[0]->base_data.size.y + 0);
				weights_row_pool[weights_row_pool_used]->parent = destination;
				destination->children.push_back(weights_row_pool[weights_row_pool_used].get());
				((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->item_name = std::get<weights_row_option>(values[index]).item_name;
				((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->weight_value = std::get<weights_row_option>(values[index]).weight_value;
				((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->source = std::get<weights_row_option>(values[index]).source;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->set_alternate(!alternate);
				weights_row_pool[weights_row_pool_used]->impl_on_update(state);
				weights_header_pool_used++;
				weights_row_pool_used++;
			}
			return measure_result{std::max(weights_header_pool[0]->base_data.size.x, weights_row_pool[0]->base_data.size.x), weights_header_pool[0]->base_data.size.y + weights_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(weights_row_pool.size() <= size_t(weights_row_pool_used)) weights_row_pool.emplace_back(make_pop_details_weights_row(state));
			weights_row_pool[weights_row_pool_used]->base_data.position.x = int16_t(x);
			weights_row_pool[weights_row_pool_used]->base_data.position.y = int16_t(y);
			weights_row_pool[weights_row_pool_used]->parent = destination;
			destination->children.push_back(weights_row_pool[weights_row_pool_used].get());
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->item_name = std::get<weights_row_option>(values[index]).item_name;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->weight_value = std::get<weights_row_option>(values[index]).weight_value;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->source = std::get<weights_row_option>(values[index]).source;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->set_alternate(alternate);
			weights_row_pool[weights_row_pool_used]->impl_on_update(state);
			weights_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ weights_row_pool[0]->base_data.size.x, weights_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_ideo_list_t::reset_pools() {
	weights_header_pool_used = 0;
	weights_row_pool_used = 0;
}
void pop_details_main_iss_list_t::add_weights_row(std::string item_name, float weight_value, std::variant<dcon::ideology_id, dcon::issue_option_id> source) {
	values.emplace_back(weights_row_option{item_name, weight_value, source});
}
void  pop_details_main_iss_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::iss_list::on_create
// END
}
void  pop_details_main_iss_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::iss_list::update
	values.clear();
	auto ptid = state.world.pop_get_poptype(main.for_pop);
	auto owner = nations::owner_of_pop(state, main.for_pop);

	for(auto opt : state.world.in_issue_option) {
		auto allow = opt.get_allow();
		auto parent_issue = opt.get_parent_issue();
		auto const i_key = pop_demographics::to_key(state, opt);
		auto is_party_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::party);
		auto is_social_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::social);
		auto is_political_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::political);
		auto has_modifier = is_social_issue || is_political_issue;
		auto modifier_key =
			is_social_issue ? sys::national_mod_offsets::social_reform_desire : sys::national_mod_offsets::political_reform_desire;

		auto current_issue_setting = state.world.nation_get_issues(owner, parent_issue);
		auto allowed_by_owner =
			(state.world.nation_get_is_civilized(owner) || is_party_issue)
			&& (!state.world.issue_get_is_next_step_only(parent_issue) ||
					(current_issue_setting.id.index() == opt.id.index()) ||
					(current_issue_setting.id.index() == opt.id.index() - 1) ||
					(current_issue_setting.id.index() == opt.id.index() + 1));
		auto owner_modifier =
			has_modifier ? (state.world.nation_get_modifier_values(owner, modifier_key) + 1.0f) : 1.0f;
		auto mtrigger = state.world.pop_type_get_issues(ptid, opt);
		auto amount = std::max(0.0f, owner_modifier * (allowed_by_owner ? (mtrigger ? trigger::evaluate_multiplicative_modifier(state, mtrigger, trigger::to_generic(main.for_pop), trigger::to_generic(main.for_pop), 0) : 0.0f) : 0.0f));

		add_weights_row(text::produce_simple_string(state, parent_issue.get_name()) + " " + text::produce_simple_string(state, opt.get_name()), amount, opt.id);
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->weights_table_item_sort_direction != 0) work_to_do = true;
	if(table_source->weights_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<weights_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<weights_row_option>(values[i])) ++i;
				if(table_source->weights_table_item_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<weights_row_option>(raw_a);
						auto const& b = std::get<weights_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::iss_list::weights_table::sort::item
						result = cmp3(a.item_name, b.item_name);
// END
						return -result == table_source->weights_table_item_sort_direction;
					});
				}
				if(table_source->weights_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<weights_row_option>(raw_a);
						auto const& b = std::get<weights_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::iss_list::weights_table::sort::weight
						result = cmp3(a.weight_value, b.weight_value);
// END
						return -result == table_source->weights_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_iss_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<weights_row_option>(values[index])) {
		if(weights_header_pool.empty()) weights_header_pool.emplace_back(make_pop_details_weights_header(state));
		if(weights_row_pool.empty()) weights_row_pool.emplace_back(make_pop_details_weights_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<weights_row_option>(values[index - 1]))) {
			if(destination) {
				if(weights_header_pool.size() <= size_t(weights_header_pool_used)) weights_header_pool.emplace_back(make_pop_details_weights_header(state));
				if(weights_row_pool.size() <= size_t(weights_row_pool_used)) weights_row_pool.emplace_back(make_pop_details_weights_row(state));
				weights_header_pool[weights_header_pool_used]->base_data.position.x = int16_t(x);
				weights_header_pool[weights_header_pool_used]->base_data.position.y = int16_t(y);
				if(!weights_header_pool[weights_header_pool_used]->parent) {
					weights_header_pool[weights_header_pool_used]->parent = destination;
					weights_header_pool[weights_header_pool_used]->impl_on_update(state);
					weights_header_pool[weights_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(weights_header_pool[weights_header_pool_used].get());
			((pop_details_weights_header_t*)(weights_header_pool[weights_header_pool_used].get()))->set_alternate(alternate);
				weights_row_pool[weights_row_pool_used]->base_data.position.x = int16_t(x);
				weights_row_pool[weights_row_pool_used]->base_data.position.y = int16_t(y +  weights_row_pool[0]->base_data.size.y + 0);
				weights_row_pool[weights_row_pool_used]->parent = destination;
				destination->children.push_back(weights_row_pool[weights_row_pool_used].get());
				((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->item_name = std::get<weights_row_option>(values[index]).item_name;
				((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->weight_value = std::get<weights_row_option>(values[index]).weight_value;
				((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->source = std::get<weights_row_option>(values[index]).source;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->set_alternate(!alternate);
				weights_row_pool[weights_row_pool_used]->impl_on_update(state);
				weights_header_pool_used++;
				weights_row_pool_used++;
			}
			return measure_result{std::max(weights_header_pool[0]->base_data.size.x, weights_row_pool[0]->base_data.size.x), weights_header_pool[0]->base_data.size.y + weights_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(weights_row_pool.size() <= size_t(weights_row_pool_used)) weights_row_pool.emplace_back(make_pop_details_weights_row(state));
			weights_row_pool[weights_row_pool_used]->base_data.position.x = int16_t(x);
			weights_row_pool[weights_row_pool_used]->base_data.position.y = int16_t(y);
			weights_row_pool[weights_row_pool_used]->parent = destination;
			destination->children.push_back(weights_row_pool[weights_row_pool_used].get());
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->item_name = std::get<weights_row_option>(values[index]).item_name;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->weight_value = std::get<weights_row_option>(values[index]).weight_value;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->source = std::get<weights_row_option>(values[index]).source;
			((pop_details_weights_row_t*)(weights_row_pool[weights_row_pool_used].get()))->set_alternate(alternate);
			weights_row_pool[weights_row_pool_used]->impl_on_update(state);
			weights_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ weights_row_pool[0]->base_data.size.x, weights_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_iss_list_t::reset_pools() {
	weights_header_pool_used = 0;
	weights_row_pool_used = 0;
}
void pop_details_main_emm_list_t::add_emm_row(dcon::nation_id destination) {
	values.emplace_back(emm_row_option{destination});
}
void  pop_details_main_emm_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::emm_list::on_create
// END
}
void  pop_details_main_emm_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::emm_list::update
	values.clear();
	auto owner = nations::owner_of_pop(state, main.for_pop);
	auto pt = state.world.pop_get_poptype(main.for_pop);
	auto modifier = state.world.pop_type_get_country_migration_target(pt);

	if(!state.world.nation_get_is_civilized(owner) || !modifier)
		return;

	auto home_continent = state.world.province_get_continent(state.world.nation_get_capital(owner));

	for(auto n : state.world.in_nation) {
		if(n.get_owned_province_count() > 0 && n != owner && n.get_is_civilized() && (state.world.province_get_continent(state.world.nation_get_capital(n)) != home_continent || state.world.get_nation_adjacency_by_nation_adjacency_pair(owner, n))) {
			add_emm_row(n);

			//float interp_result = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(n), trigger::to_generic(main.for_pop), 0);
			//float weight = std::max(0.0f, interp_result * std::max(0.f, (state.world.nation_get_modifier_values(n, sys::national_mod_offsets::global_immigrant_attract) + 1.0f)));
		}
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->emm_table_destination_sort_direction != 0) work_to_do = true;
	if(table_source->emm_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<emm_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<emm_row_option>(values[i])) ++i;
				if(table_source->emm_table_destination_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<emm_row_option>(raw_a);
						auto const& b = std::get<emm_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::emm_list::emm_table::sort::destination
						result = cmp3(text::produce_simple_string(state, text::get_name(state, a.destination)), text::produce_simple_string(state, text::get_name(state, b.destination)));
// END
						return -result == table_source->emm_table_destination_sort_direction;
					});
				}
				if(table_source->emm_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<emm_row_option>(raw_a);
						auto const& b = std::get<emm_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::emm_list::emm_table::sort::weight
						auto owner = nations::owner_of_pop(state, main.for_pop);
						auto pt = state.world.pop_get_poptype(main.for_pop);
						auto modifier = state.world.pop_type_get_country_migration_target(pt);

						float ainterp_result = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(a.destination), trigger::to_generic(main.for_pop), 0);
						float aweight = std::max(0.0f, ainterp_result * std::max(0.f, (state.world.nation_get_modifier_values(a.destination, sys::national_mod_offsets::global_immigrant_attract) + 1.0f)));

						float binterp_result = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(b.destination), trigger::to_generic(main.for_pop), 0);
						float bweight = std::max(0.0f, binterp_result * std::max(0.f, (state.world.nation_get_modifier_values(b.destination, sys::national_mod_offsets::global_immigrant_attract) + 1.0f)));

						result = cmp3(aweight, bweight);
// END
						return -result == table_source->emm_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_emm_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<emm_row_option>(values[index])) {
		if(emm_header_pool.empty()) emm_header_pool.emplace_back(make_pop_details_emm_header(state));
		if(emm_row_pool.empty()) emm_row_pool.emplace_back(make_pop_details_emm_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<emm_row_option>(values[index - 1]))) {
			if(destination) {
				if(emm_header_pool.size() <= size_t(emm_header_pool_used)) emm_header_pool.emplace_back(make_pop_details_emm_header(state));
				if(emm_row_pool.size() <= size_t(emm_row_pool_used)) emm_row_pool.emplace_back(make_pop_details_emm_row(state));
				emm_header_pool[emm_header_pool_used]->base_data.position.x = int16_t(x);
				emm_header_pool[emm_header_pool_used]->base_data.position.y = int16_t(y);
				if(!emm_header_pool[emm_header_pool_used]->parent) {
					emm_header_pool[emm_header_pool_used]->parent = destination;
					emm_header_pool[emm_header_pool_used]->impl_on_update(state);
					emm_header_pool[emm_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(emm_header_pool[emm_header_pool_used].get());
			((pop_details_emm_header_t*)(emm_header_pool[emm_header_pool_used].get()))->set_alternate(alternate);
				emm_row_pool[emm_row_pool_used]->base_data.position.x = int16_t(x);
				emm_row_pool[emm_row_pool_used]->base_data.position.y = int16_t(y +  emm_row_pool[0]->base_data.size.y + 0);
				emm_row_pool[emm_row_pool_used]->parent = destination;
				destination->children.push_back(emm_row_pool[emm_row_pool_used].get());
				((pop_details_emm_row_t*)(emm_row_pool[emm_row_pool_used].get()))->destination = std::get<emm_row_option>(values[index]).destination;
			((pop_details_emm_row_t*)(emm_row_pool[emm_row_pool_used].get()))->set_alternate(!alternate);
				emm_row_pool[emm_row_pool_used]->impl_on_update(state);
				emm_header_pool_used++;
				emm_row_pool_used++;
			}
			return measure_result{std::max(emm_header_pool[0]->base_data.size.x, emm_row_pool[0]->base_data.size.x), emm_header_pool[0]->base_data.size.y + emm_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(emm_row_pool.size() <= size_t(emm_row_pool_used)) emm_row_pool.emplace_back(make_pop_details_emm_row(state));
			emm_row_pool[emm_row_pool_used]->base_data.position.x = int16_t(x);
			emm_row_pool[emm_row_pool_used]->base_data.position.y = int16_t(y);
			emm_row_pool[emm_row_pool_used]->parent = destination;
			destination->children.push_back(emm_row_pool[emm_row_pool_used].get());
			((pop_details_emm_row_t*)(emm_row_pool[emm_row_pool_used].get()))->destination = std::get<emm_row_option>(values[index]).destination;
			((pop_details_emm_row_t*)(emm_row_pool[emm_row_pool_used].get()))->set_alternate(alternate);
			emm_row_pool[emm_row_pool_used]->impl_on_update(state);
			emm_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ emm_row_pool[0]->base_data.size.x, emm_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_emm_list_t::reset_pools() {
	emm_header_pool_used = 0;
	emm_row_pool_used = 0;
}
void pop_details_main_mig_list_t::add_mig_row(dcon::province_id destination) {
	values.emplace_back(mig_row_option{destination});
}
void  pop_details_main_mig_list_t::on_create(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::mig_list::on_create
// END
}
void  pop_details_main_mig_list_t::update(sys::state& state, layout_window_element* parent) {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::mig_list::update
	values.clear();

	auto pt = state.world.pop_get_poptype(main.for_pop);
	auto modifier = state.world.pop_type_get_migration_target(pt);
	if(!modifier)
		return;

	bool limit_to_capitals = state.world.pop_type_get_state_capital_only(pt);
	auto overseas_culture = state.world.culture_get_group_from_culture_group_membership(state.world.pop_get_culture(main.for_pop));
	auto home_continent = state.world.province_get_continent(state.world.pop_get_province_from_pop_location(main.for_pop));

	for(auto loc : state.world.nation_get_province_ownership(nations::owner_of_pop(state, main.for_pop))) {
		if(loc.get_province().get_is_colonial() == false) {
			if(!limit_to_capitals || loc.get_province().get_state_membership().get_capital().id == loc.get_province().id) {
				//float interp_result = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(loc.get_province().id), trigger::to_generic(main.for_pop), 0);
				//float weight = std::max(0.0f, interp_result * (loc.get_province().get_modifier_values(sys::provincial_mod_offsets::immigrant_attract) + 1.0f));
				add_mig_row(loc.get_province());
			}
		} else {
			if((overseas_culture || loc.get_province().get_continent() == home_continent) && (!limit_to_capitals || loc.get_province().get_state_membership().get_capital().id == loc.get_province().id)) {
				//float interp_result = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(loc.get_province().id), trigger::to_generic(main.for_pop), 0);
				//float weight = std::max(0.0f, interp_result * (loc.get_province().get_modifier_values(sys::provincial_mod_offsets::immigrant_attract) + 1.0f));
				add_mig_row(loc.get_province());
			}
		}
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (pop_details_main_t*)(parent);
	if(table_source->mig_table_destination_sort_direction != 0) work_to_do = true;
	if(table_source->mig_table_weight_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<mig_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<mig_row_option>(values[i])) ++i;
				if(table_source->mig_table_destination_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<mig_row_option>(raw_a);
						auto const& b = std::get<mig_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::mig_list::mig_table::sort::destination
						result = cmp3(text::produce_simple_string(state, state.world.province_get_name(a.destination)), text::produce_simple_string(state, state.world.province_get_name(b.destination)));
// END
						return -result == table_source->mig_table_destination_sort_direction;
					});
				}
				if(table_source->mig_table_weight_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<mig_row_option>(raw_a);
						auto const& b = std::get<mig_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::mig_list::mig_table::sort::weight
						float aweight = demographics::explain_province_internal_migration_weight(state, main.for_pop, a.destination).result;
						float bweight = demographics::explain_province_internal_migration_weight(state, main.for_pop, b.destination).result;

						result = cmp3(aweight, bweight);
// END
						return -result == table_source->mig_table_weight_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  pop_details_main_mig_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<mig_row_option>(values[index])) {
		if(mig_header_pool.empty()) mig_header_pool.emplace_back(make_pop_details_mig_header(state));
		if(mig_row_pool.empty()) mig_row_pool.emplace_back(make_pop_details_mig_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<mig_row_option>(values[index - 1]))) {
			if(destination) {
				if(mig_header_pool.size() <= size_t(mig_header_pool_used)) mig_header_pool.emplace_back(make_pop_details_mig_header(state));
				if(mig_row_pool.size() <= size_t(mig_row_pool_used)) mig_row_pool.emplace_back(make_pop_details_mig_row(state));
				mig_header_pool[mig_header_pool_used]->base_data.position.x = int16_t(x);
				mig_header_pool[mig_header_pool_used]->base_data.position.y = int16_t(y);
				if(!mig_header_pool[mig_header_pool_used]->parent) {
					mig_header_pool[mig_header_pool_used]->parent = destination;
					mig_header_pool[mig_header_pool_used]->impl_on_update(state);
					mig_header_pool[mig_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(mig_header_pool[mig_header_pool_used].get());
			((pop_details_mig_header_t*)(mig_header_pool[mig_header_pool_used].get()))->set_alternate(alternate);
				mig_row_pool[mig_row_pool_used]->base_data.position.x = int16_t(x);
				mig_row_pool[mig_row_pool_used]->base_data.position.y = int16_t(y +  mig_row_pool[0]->base_data.size.y + 0);
				mig_row_pool[mig_row_pool_used]->parent = destination;
				destination->children.push_back(mig_row_pool[mig_row_pool_used].get());
				((pop_details_mig_row_t*)(mig_row_pool[mig_row_pool_used].get()))->destination = std::get<mig_row_option>(values[index]).destination;
			((pop_details_mig_row_t*)(mig_row_pool[mig_row_pool_used].get()))->set_alternate(!alternate);
				mig_row_pool[mig_row_pool_used]->impl_on_update(state);
				mig_header_pool_used++;
				mig_row_pool_used++;
			}
			return measure_result{std::max(mig_header_pool[0]->base_data.size.x, mig_row_pool[0]->base_data.size.x), mig_header_pool[0]->base_data.size.y + mig_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(mig_row_pool.size() <= size_t(mig_row_pool_used)) mig_row_pool.emplace_back(make_pop_details_mig_row(state));
			mig_row_pool[mig_row_pool_used]->base_data.position.x = int16_t(x);
			mig_row_pool[mig_row_pool_used]->base_data.position.y = int16_t(y);
			mig_row_pool[mig_row_pool_used]->parent = destination;
			destination->children.push_back(mig_row_pool[mig_row_pool_used].get());
			((pop_details_mig_row_t*)(mig_row_pool[mig_row_pool_used].get()))->destination = std::get<mig_row_option>(values[index]).destination;
			((pop_details_mig_row_t*)(mig_row_pool[mig_row_pool_used].get()))->set_alternate(alternate);
			mig_row_pool[mig_row_pool_used]->impl_on_update(state);
			mig_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ mig_row_pool[0]->base_data.size.x, mig_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  pop_details_main_mig_list_t::reset_pools() {
	mig_header_pool_used = 0;
	mig_row_pool_used = 0;
}
void pop_details_main_location_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::location_value::update
	set_text(state, text::produce_simple_string(state, state.world.province_get_name(state.world.pop_get_province_from_pop_location(main.for_pop))));
// END
}
void pop_details_main_job_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::job_value::update
	set_text(state, text::produce_simple_string(state, state.world.pop_get_poptype(main.for_pop).get_name()));
// END
}
void pop_details_main_mil_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::mil_value::tooltip
	describe_mil(state, contents, main.for_pop);
// END
}
void pop_details_main_mil_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::mil_value::update
	set_text(state, text::format_float(pop_demographics::get_militancy(state, main.for_pop), 1));
// END
}
void pop_details_main_con_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::con_value::tooltip
	describe_con(state, contents, main.for_pop);
// END
}
void pop_details_main_con_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::con_value::update
	set_text(state, text::format_float(pop_demographics::get_consciousness(state, main.for_pop), 1));
// END
}
void pop_details_main_lit_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::lit_value::tooltip
	describe_lit(state, contents, main.for_pop);
// END
}
void pop_details_main_lit_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::lit_value::update
	set_text(state, text::format_percentage(pop_demographics::get_literacy(state, main.for_pop), 0));
// END
}
void pop_details_main_religion_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::religion_value::update
	set_text(state, text::produce_simple_string(state, state.world.pop_get_religion(main.for_pop).get_name()));
// END
}
void pop_details_main_culture_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::culture_value::update
	set_text(state, text::produce_simple_string(state, state.world.pop_get_culture(main.for_pop).get_name()));
// END
}
void pop_details_main_employment_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::employment_value::update
	if(state.world.pop_get_poptype(main.for_pop).get_has_unemployment()) {
		set_text(state, text::format_percentage(pop_demographics::get_raw_employment(state, main.for_pop), 0));
	} else {
		set_text(state, "–");
	}
// END
}
void pop_details_main_income_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::income_value::update
	set_text(state, text::format_money(state.world.pop_get_savings(main.for_pop)));
// END
}
void pop_details_main_growth_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::growth_value::tooltip
	describe_growth(state, contents, main.for_pop);
// END
}
void pop_details_main_growth_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::growth_value::update
	set_text(state, std::to_string(int64_t(demographics::get_monthly_pop_increase(state, main.for_pop))));
// END
}
void pop_details_main_assim_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::assim_value::tooltip
	describe_assimilation(state, contents, main.for_pop);
// END
}
void pop_details_main_assim_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::assim_value::update
	set_text(state, std::to_string(int64_t(demographics::get_estimated_assimilation(state, main.for_pop))));
// END
}
void pop_details_main_ln_total_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::ln_total::update
	float total = 0.0f;
	auto type = state.world.pop_get_poptype(main.for_pop);
	auto l = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto s = state.world.province_get_state_membership(l);
	auto m = state.world.state_instance_get_market_from_local_market(s);
	for(auto c : state.world.in_commodity) {
		total += state.world.market_get_price(m, c) * state.world.pop_type_get_life_needs(type, c);
	}
	set_text(state, text::format_money(total));
// END
}
void pop_details_main_en_total_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::en_total::update
	float total = 0.0f;
	auto type = state.world.pop_get_poptype(main.for_pop);
	auto l = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto s = state.world.province_get_state_membership(l);
	auto m = state.world.state_instance_get_market_from_local_market(s);
	for(auto c : state.world.in_commodity) {
		total += state.world.market_get_price(m, c) * state.world.pop_type_get_everyday_needs(type, c);
	}
	set_text(state, text::format_money(total));
// END
}
void pop_details_main_lx_total_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::lx_total::update
	float total = 0.0f;
	auto type = state.world.pop_get_poptype(main.for_pop);
	auto l = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto s = state.world.province_get_state_membership(l);
	auto m = state.world.state_instance_get_market_from_local_market(s);
	for(auto c : state.world.in_commodity) {
		total += state.world.market_get_price(m, c) * state.world.pop_type_get_luxury_needs(type, c);
	}
	set_text(state, text::format_money(total));
// END
}
void pop_details_main_prom_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::prom_value::tooltip
	describe_promotion(state, contents, main.for_pop);
// END
}
void pop_details_main_prom_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::prom_value::update
	set_text(state, std::to_string(int64_t(demographics::get_estimated_promotion(state, main.for_pop))));
// END
}
void pop_details_main_prom_bar_chart_t::on_create(sys::state& state) noexcept {
// BEGIN main::prom_bar_chart::create
// END
}
void pop_details_main_prom_bar_chart_t::update_chart(sys::state& state) {
	std::sort(graph_content.begin(), graph_content.end(), [](auto const& a, auto const& b) { return a.amount > b.amount; });
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 264; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 264; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(264)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void pop_details_main_prom_bar_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x - par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin ) / float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin));
	int32_t temp_index = 0;
	if(temp_offset < 0.0f || temp_offset > temp_total) return;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::prom_bar_chart::tooltip
		if(temp_total > 0) {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ graph_content[temp_index].amount / temp_total });
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(selected_key));
			text::close_layout_box(contents, box);
		}
// END
	}
}
void pop_details_main_prom_bar_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void pop_details_main_prom_bar_chart_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::prom_bar_chart::update
	graph_content.clear();

	auto promotion_weights = demographics::get_pop_promotion_demotion_weights<demographics::promotion_type::promotion>(state, main.for_pop);

	for(auto t : state.world.in_pop_type) {
		auto amount = promotion_weights.pop_weights[t.id];
		if(amount > 0) {
			graph_content.push_back(graph_entry{ t.id, ogl::unpack_color(t.get_color()), amount });
		}
	}

	update_chart(state);
// END
}
void pop_details_main_dem_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::dem_value::tooltip
	describe_demotion(state, contents, main.for_pop);
// END
}
void pop_details_main_dem_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::dem_value::update
	set_text(state, std::to_string(int64_t(demographics::get_estimated_demotion(state, main.for_pop))));
// END
}
void pop_details_main_dem_bar_chart_t::on_create(sys::state& state) noexcept {
// BEGIN main::dem_bar_chart::create
// END
}
void pop_details_main_dem_bar_chart_t::update_chart(sys::state& state) {
	std::sort(graph_content.begin(), graph_content.end(), [](auto const& a, auto const& b) { return a.amount > b.amount; });
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 264; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 264; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(264)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void pop_details_main_dem_bar_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x - par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin ) / float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin));
	int32_t temp_index = 0;
	if(temp_offset < 0.0f || temp_offset > temp_total) return;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::dem_bar_chart::tooltip
		if(temp_total > 0) {
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ graph_content[temp_index].amount / temp_total });
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(selected_key));
			text::close_layout_box(contents, box);
		}
// END
	}
}
void pop_details_main_dem_bar_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void pop_details_main_dem_bar_chart_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::dem_bar_chart::update
	graph_content.clear();

	auto promotion_weights = demographics::get_pop_promotion_demotion_weights<demographics::promotion_type::demotion>(state, main.for_pop);

	for(auto t : state.world.in_pop_type) {
		auto amount = promotion_weights.pop_weights[t.id];
		if(amount > 0) {
			graph_content.push_back(graph_entry{ t.id, ogl::unpack_color(t.get_color()), amount });
		}
	}

	update_chart(state);
// END
}
void pop_details_main_ideo_bar_chart_t::on_create(sys::state& state) noexcept {
// BEGIN main::ideo_bar_chart::create
// END
}
void pop_details_main_ideo_bar_chart_t::update_chart(sys::state& state) {
	std::sort(graph_content.begin(), graph_content.end(), [](auto const& a, auto const& b) { return a.amount > b.amount; });
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 264; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 264; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(264)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void pop_details_main_ideo_bar_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x - par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin ) / float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin));
	int32_t temp_index = 0;
	if(temp_offset < 0.0f || temp_offset > temp_total) return;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::ideo_bar_chart::tooltip
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ graph_content[temp_index].amount });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, state.world.ideology_get_name(selected_key));
		text::close_layout_box(contents, box);
// END
	}
}
void pop_details_main_ideo_bar_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void pop_details_main_ideo_bar_chart_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::ideo_bar_chart::update
	graph_content.clear();
	for(auto i : state.world.in_ideology) {
		auto v = pop_demographics::get_demo(state, main.for_pop, pop_demographics::to_key(state, i));
		if(v > 0)
			graph_content.push_back(graph_entry{ i.id, ogl::unpack_color(i.get_color()), v });
	}
	update_chart(state);
// END
}
void pop_details_main_iss_bar_chart_t::on_create(sys::state& state) noexcept {
// BEGIN main::iss_bar_chart::create
// END
}
void pop_details_main_iss_bar_chart_t::update_chart(sys::state& state) {
	std::sort(graph_content.begin(), graph_content.end(), [](auto const& a, auto const& b) { return a.amount > b.amount; });
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 264; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 264; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(264)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void pop_details_main_iss_bar_chart_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	float temp_total = 0.0f;
	for(auto& p : graph_content) { temp_total += p.amount; }
	float temp_offset = temp_total * float(x - par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin ) / float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin));
	int32_t temp_index = 0;
	if(temp_offset < 0.0f || temp_offset > temp_total) return;
	for(auto& p : graph_content) { if(temp_offset <= p.amount) break; temp_offset -= p.amount; ++temp_index; }
	if(temp_index < int32_t(graph_content.size())) {
		auto& selected_key = graph_content[temp_index].key;
// BEGIN main::iss_bar_chart::tooltip
		auto p = state.world.issue_option_get_parent_issue(selected_key);
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ graph_content[temp_index].amount });
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, p.get_name());
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, state.world.issue_option_get_name(selected_key));
		text::close_layout_box(contents, box);
// END
	}
}
void pop_details_main_iss_bar_chart_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void pop_details_main_iss_bar_chart_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::iss_bar_chart::update
	graph_content.clear();
	for(auto i : state.world.in_issue_option) {
		auto v = pop_demographics::get_demo(state, main.for_pop, pop_demographics::to_key(state, i));
		if(v > 0)
			graph_content.push_back(graph_entry{ i.id, ogl::unpack_color(ogl::get_ui_color(state, i.id)), v });
	}
	update_chart(state);
// END
}
void pop_details_main_emm_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::emm_value::tooltip
	describe_emigration(state, contents, main.for_pop);
// END
}
void pop_details_main_emm_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::emm_value::update
	set_text(state, std::to_string(int64_t(demographics::get_estimated_emigration(state, main.for_pop))));
// END
}
void pop_details_main_i_mig_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::i_mig_value::tooltip
	describe_migration(state, contents, main.for_pop);
// END
}
void pop_details_main_i_mig_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::i_mig_value::update
	set_text(state, std::to_string(int64_t(demographics::get_estimated_internal_migration(state, main.for_pop))));
// END
}
void pop_details_main_c_mig_value_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::c_mig_value::tooltip
	describe_colonial_migration(state, contents, main.for_pop);
// END
}
void pop_details_main_c_mig_value_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::c_mig_value::update
	set_text(state, std::to_string(int64_t(demographics::get_estimated_colonial_migration(state, main.for_pop))));
// END
}
void pop_details_main_to_pop_budget_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::to_pop_budget::update
// END
}
bool pop_details_main_to_pop_budget_t::button_action(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent)); 
// BEGIN main::to_pop_budget::lbutton_action
	main.budget_w->set_visible(state, true);

	assert(main.budget_w);
	auto pop_ptr = (dcon::pop_id*)main.budget_w->get_by_name(state, "for_pop");
	*pop_ptr = main.for_pop;

	if(!main.budget_w->is_visible())
		main.budget_w->set_visible(state, true);
	else
		main.budget_w->impl_on_update(state);
	main.budget_w->parent->move_child_to_front(main.budget_w);

// END
	return true;
}
ui::message_result pop_details_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result pop_details_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	ln_list.update(state, this);
	en_list.update(state, this);
	lx_list.update(state, this);
	prom_list.update(state, this);
	dem_list.update(state, this);
	ideo_list.update(state, this);
	iss_list.update(state, this);
	emm_list.update(state, this);
	mig_list.update(state, this);
	remake_layout(state, true);
}
void pop_details_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "window_title") {
					temp.ptr = window_title.get();
				} else
				if(cname == "location_header") {
					temp.ptr = location_header.get();
				} else
				if(cname == "location_value") {
					temp.ptr = location_value.get();
				} else
				if(cname == "job_header") {
					temp.ptr = job_header.get();
				} else
				if(cname == "job_value") {
					temp.ptr = job_value.get();
				} else
				if(cname == "mil_header") {
					temp.ptr = mil_header.get();
				} else
				if(cname == "mil_value") {
					temp.ptr = mil_value.get();
				} else
				if(cname == "con_header") {
					temp.ptr = con_header.get();
				} else
				if(cname == "con_value") {
					temp.ptr = con_value.get();
				} else
				if(cname == "lit_header") {
					temp.ptr = lit_header.get();
				} else
				if(cname == "lit_value") {
					temp.ptr = lit_value.get();
				} else
				if(cname == "religion_header") {
					temp.ptr = religion_header.get();
				} else
				if(cname == "religion_value") {
					temp.ptr = religion_value.get();
				} else
				if(cname == "culture_header") {
					temp.ptr = culture_header.get();
				} else
				if(cname == "culture_value") {
					temp.ptr = culture_value.get();
				} else
				if(cname == "employment_header") {
					temp.ptr = employment_header.get();
				} else
				if(cname == "employment_value") {
					temp.ptr = employment_value.get();
				} else
				if(cname == "income_header") {
					temp.ptr = income_header.get();
				} else
				if(cname == "income_value") {
					temp.ptr = income_value.get();
				} else
				if(cname == "growth_header") {
					temp.ptr = growth_header.get();
				} else
				if(cname == "growth_value") {
					temp.ptr = growth_value.get();
				} else
				if(cname == "assim_header") {
					temp.ptr = assim_header.get();
				} else
				if(cname == "assim_value") {
					temp.ptr = assim_value.get();
				} else
				if(cname == "ln_total") {
					temp.ptr = ln_total.get();
				} else
				if(cname == "en_total") {
					temp.ptr = en_total.get();
				} else
				if(cname == "lx_total") {
					temp.ptr = lx_total.get();
				} else
				if(cname == "prom_header") {
					temp.ptr = prom_header.get();
				} else
				if(cname == "prom_amount_label") {
					temp.ptr = prom_amount_label.get();
				} else
				if(cname == "prom_value") {
					temp.ptr = prom_value.get();
				} else
				if(cname == "prom_bar_chart") {
					temp.ptr = prom_bar_chart.get();
				} else
				if(cname == "dem_header") {
					temp.ptr = dem_header.get();
				} else
				if(cname == "dem_amount_label") {
					temp.ptr = dem_amount_label.get();
				} else
				if(cname == "dem_value") {
					temp.ptr = dem_value.get();
				} else
				if(cname == "dem_bar_chart") {
					temp.ptr = dem_bar_chart.get();
				} else
				if(cname == "ideo_header") {
					temp.ptr = ideo_header.get();
				} else
				if(cname == "ideo_bar_chart") {
					temp.ptr = ideo_bar_chart.get();
				} else
				if(cname == "iss_header") {
					temp.ptr = iss_header.get();
				} else
				if(cname == "iss_bar_chart") {
					temp.ptr = iss_bar_chart.get();
				} else
				if(cname == "emm_header") {
					temp.ptr = emm_header.get();
				} else
				if(cname == "emm_amount_label") {
					temp.ptr = emm_amount_label.get();
				} else
				if(cname == "emm_value") {
					temp.ptr = emm_value.get();
				} else
				if(cname == "mig_header") {
					temp.ptr = mig_header.get();
				} else
				if(cname == "i_mig_amount_label") {
					temp.ptr = i_mig_amount_label.get();
				} else
				if(cname == "i_mig_value") {
					temp.ptr = i_mig_value.get();
				} else
				if(cname == "c_mig_amount_label") {
					temp.ptr = c_mig_amount_label.get();
				} else
				if(cname == "c_mig_value") {
					temp.ptr = c_mig_value.get();
				} else
				if(cname == "fancy_bar_1") {
					temp.ptr = fancy_bar_1.get();
				} else
				if(cname == "fancy_bar_2") {
					temp.ptr = fancy_bar_2.get();
				} else
				if(cname == "to_pop_budget") {
					temp.ptr = to_pop_budget.get();
				} else
				if(cname == "fancy_bar_3") {
					temp.ptr = fancy_bar_3.get();
				} else
				if(cname == "ln_header") {
					temp.ptr = ln_header.get();
				} else
				if(cname == "en_header") {
					temp.ptr = en_header.get();
				} else
				if(cname == "lx_header") {
					temp.ptr = lx_header.get();
				} else
				if(cname == "ln_icon") {
					temp.ptr = ln_icon.get();
				} else
				if(cname == "en_icon") {
					temp.ptr = en_icon.get();
				} else
				if(cname == "lx_icon") {
					temp.ptr = lx_icon.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
				if(cname == "ln_list") {
					temp.generator = &ln_list;
				}
				if(cname == "en_list") {
					temp.generator = &en_list;
				}
				if(cname == "lx_list") {
					temp.generator = &lx_list;
				}
				if(cname == "prom_list") {
					temp.generator = &prom_list;
				}
				if(cname == "dem_list") {
					temp.generator = &dem_list;
				}
				if(cname == "ideo_list") {
					temp.generator = &ideo_list;
				}
				if(cname == "iss_list") {
					temp.generator = &iss_list;
				}
				if(cname == "emm_list") {
					temp.generator = &emm_list;
				}
				if(cname == "mig_list") {
					temp.generator = &mig_list;
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
void pop_details_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::main"));
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
		if(child_data.name == "location_header") {
			location_header = std::make_unique<template_label>();
			location_header->parent = this;
			auto cptr = location_header.get();
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
		if(child_data.name == "location_value") {
			location_value = std::make_unique<pop_details_main_location_value_t>();
			location_value->parent = this;
			auto cptr = location_value.get();
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
		if(child_data.name == "job_header") {
			job_header = std::make_unique<template_label>();
			job_header->parent = this;
			auto cptr = job_header.get();
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
		if(child_data.name == "job_value") {
			job_value = std::make_unique<pop_details_main_job_value_t>();
			job_value->parent = this;
			auto cptr = job_value.get();
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
		if(child_data.name == "mil_header") {
			mil_header = std::make_unique<template_label>();
			mil_header->parent = this;
			auto cptr = mil_header.get();
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
		if(child_data.name == "mil_value") {
			mil_value = std::make_unique<pop_details_main_mil_value_t>();
			mil_value->parent = this;
			auto cptr = mil_value.get();
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
		if(child_data.name == "con_header") {
			con_header = std::make_unique<template_label>();
			con_header->parent = this;
			auto cptr = con_header.get();
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
		if(child_data.name == "con_value") {
			con_value = std::make_unique<pop_details_main_con_value_t>();
			con_value->parent = this;
			auto cptr = con_value.get();
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
		if(child_data.name == "lit_header") {
			lit_header = std::make_unique<template_label>();
			lit_header->parent = this;
			auto cptr = lit_header.get();
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
		if(child_data.name == "lit_value") {
			lit_value = std::make_unique<pop_details_main_lit_value_t>();
			lit_value->parent = this;
			auto cptr = lit_value.get();
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
		if(child_data.name == "religion_header") {
			religion_header = std::make_unique<template_label>();
			religion_header->parent = this;
			auto cptr = religion_header.get();
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
		if(child_data.name == "religion_value") {
			religion_value = std::make_unique<pop_details_main_religion_value_t>();
			religion_value->parent = this;
			auto cptr = religion_value.get();
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
		if(child_data.name == "culture_header") {
			culture_header = std::make_unique<template_label>();
			culture_header->parent = this;
			auto cptr = culture_header.get();
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
		if(child_data.name == "culture_value") {
			culture_value = std::make_unique<pop_details_main_culture_value_t>();
			culture_value->parent = this;
			auto cptr = culture_value.get();
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
		if(child_data.name == "employment_header") {
			employment_header = std::make_unique<template_label>();
			employment_header->parent = this;
			auto cptr = employment_header.get();
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
		if(child_data.name == "employment_value") {
			employment_value = std::make_unique<pop_details_main_employment_value_t>();
			employment_value->parent = this;
			auto cptr = employment_value.get();
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
		if(child_data.name == "income_header") {
			income_header = std::make_unique<template_label>();
			income_header->parent = this;
			auto cptr = income_header.get();
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
		if(child_data.name == "income_value") {
			income_value = std::make_unique<pop_details_main_income_value_t>();
			income_value->parent = this;
			auto cptr = income_value.get();
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
		if(child_data.name == "growth_header") {
			growth_header = std::make_unique<template_label>();
			growth_header->parent = this;
			auto cptr = growth_header.get();
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
		if(child_data.name == "growth_value") {
			growth_value = std::make_unique<pop_details_main_growth_value_t>();
			growth_value->parent = this;
			auto cptr = growth_value.get();
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
		if(child_data.name == "assim_header") {
			assim_header = std::make_unique<template_label>();
			assim_header->parent = this;
			auto cptr = assim_header.get();
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
		if(child_data.name == "assim_value") {
			assim_value = std::make_unique<pop_details_main_assim_value_t>();
			assim_value->parent = this;
			auto cptr = assim_value.get();
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
		if(child_data.name == "ln_total") {
			ln_total = std::make_unique<pop_details_main_ln_total_t>();
			ln_total->parent = this;
			auto cptr = ln_total.get();
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
		if(child_data.name == "en_total") {
			en_total = std::make_unique<pop_details_main_en_total_t>();
			en_total->parent = this;
			auto cptr = en_total.get();
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
		if(child_data.name == "lx_total") {
			lx_total = std::make_unique<pop_details_main_lx_total_t>();
			lx_total->parent = this;
			auto cptr = lx_total.get();
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
		if(child_data.name == "prom_header") {
			prom_header = std::make_unique<template_label>();
			prom_header->parent = this;
			auto cptr = prom_header.get();
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
		if(child_data.name == "prom_amount_label") {
			prom_amount_label = std::make_unique<template_label>();
			prom_amount_label->parent = this;
			auto cptr = prom_amount_label.get();
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
		if(child_data.name == "prom_value") {
			prom_value = std::make_unique<pop_details_main_prom_value_t>();
			prom_value->parent = this;
			auto cptr = prom_value.get();
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
		if(child_data.name == "prom_bar_chart") {
			prom_bar_chart = std::make_unique<pop_details_main_prom_bar_chart_t>();
			prom_bar_chart->parent = this;
			auto cptr = prom_bar_chart.get();
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
		if(child_data.name == "dem_header") {
			dem_header = std::make_unique<template_label>();
			dem_header->parent = this;
			auto cptr = dem_header.get();
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
		if(child_data.name == "dem_amount_label") {
			dem_amount_label = std::make_unique<template_label>();
			dem_amount_label->parent = this;
			auto cptr = dem_amount_label.get();
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
		if(child_data.name == "dem_value") {
			dem_value = std::make_unique<pop_details_main_dem_value_t>();
			dem_value->parent = this;
			auto cptr = dem_value.get();
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
		if(child_data.name == "dem_bar_chart") {
			dem_bar_chart = std::make_unique<pop_details_main_dem_bar_chart_t>();
			dem_bar_chart->parent = this;
			auto cptr = dem_bar_chart.get();
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
		if(child_data.name == "ideo_header") {
			ideo_header = std::make_unique<template_label>();
			ideo_header->parent = this;
			auto cptr = ideo_header.get();
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
		if(child_data.name == "ideo_bar_chart") {
			ideo_bar_chart = std::make_unique<pop_details_main_ideo_bar_chart_t>();
			ideo_bar_chart->parent = this;
			auto cptr = ideo_bar_chart.get();
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
		if(child_data.name == "iss_header") {
			iss_header = std::make_unique<template_label>();
			iss_header->parent = this;
			auto cptr = iss_header.get();
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
		if(child_data.name == "iss_bar_chart") {
			iss_bar_chart = std::make_unique<pop_details_main_iss_bar_chart_t>();
			iss_bar_chart->parent = this;
			auto cptr = iss_bar_chart.get();
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
		if(child_data.name == "emm_header") {
			emm_header = std::make_unique<template_label>();
			emm_header->parent = this;
			auto cptr = emm_header.get();
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
		if(child_data.name == "emm_amount_label") {
			emm_amount_label = std::make_unique<template_label>();
			emm_amount_label->parent = this;
			auto cptr = emm_amount_label.get();
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
		if(child_data.name == "emm_value") {
			emm_value = std::make_unique<pop_details_main_emm_value_t>();
			emm_value->parent = this;
			auto cptr = emm_value.get();
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
		if(child_data.name == "mig_header") {
			mig_header = std::make_unique<template_label>();
			mig_header->parent = this;
			auto cptr = mig_header.get();
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
		if(child_data.name == "i_mig_amount_label") {
			i_mig_amount_label = std::make_unique<template_label>();
			i_mig_amount_label->parent = this;
			auto cptr = i_mig_amount_label.get();
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
		if(child_data.name == "i_mig_value") {
			i_mig_value = std::make_unique<pop_details_main_i_mig_value_t>();
			i_mig_value->parent = this;
			auto cptr = i_mig_value.get();
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
		if(child_data.name == "c_mig_amount_label") {
			c_mig_amount_label = std::make_unique<template_label>();
			c_mig_amount_label->parent = this;
			auto cptr = c_mig_amount_label.get();
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
		if(child_data.name == "c_mig_value") {
			c_mig_value = std::make_unique<pop_details_main_c_mig_value_t>();
			c_mig_value->parent = this;
			auto cptr = c_mig_value.get();
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
		if(child_data.name == "fancy_bar_1") {
			fancy_bar_1 = std::make_unique<template_bg_graphic>();
			fancy_bar_1->parent = this;
			auto cptr = fancy_bar_1.get();
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
		if(child_data.name == "fancy_bar_2") {
			fancy_bar_2 = std::make_unique<template_bg_graphic>();
			fancy_bar_2->parent = this;
			auto cptr = fancy_bar_2.get();
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
		if(child_data.name == "to_pop_budget") {
			to_pop_budget = std::make_unique<pop_details_main_to_pop_budget_t>();
			to_pop_budget->parent = this;
			auto cptr = to_pop_budget.get();
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
		if(child_data.name == "fancy_bar_3") {
			fancy_bar_3 = std::make_unique<template_bg_graphic>();
			fancy_bar_3->parent = this;
			auto cptr = fancy_bar_3.get();
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
		if(child_data.name == "ln_header") {
			ln_header = std::make_unique<template_label>();
			ln_header->parent = this;
			auto cptr = ln_header.get();
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
		if(child_data.name == "en_header") {
			en_header = std::make_unique<template_label>();
			en_header->parent = this;
			auto cptr = en_header.get();
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
		if(child_data.name == "lx_header") {
			lx_header = std::make_unique<template_label>();
			lx_header->parent = this;
			auto cptr = lx_header.get();
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
		if(child_data.name == "ln_icon") {
			ln_icon = std::make_unique<template_icon_graphic>();
			ln_icon->parent = this;
			auto cptr = ln_icon.get();
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
		if(child_data.name == "en_icon") {
			en_icon = std::make_unique<template_icon_graphic>();
			en_icon->parent = this;
			auto cptr = en_icon.get();
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
		if(child_data.name == "lx_icon") {
			lx_icon = std::make_unique<template_icon_graphic>();
			lx_icon->parent = this;
			auto cptr = lx_icon.get();
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
		if(child_data.name == ".tabneeds_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			needs_table_ascending_icon_key = main_section.read<std::string_view>();
			needs_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_icon_column_start = running_w_total;
			col_section.read(needs_table_icon_column_width);
			running_w_total += needs_table_icon_column_width;
			col_section.read(needs_table_icon_column_text_color);
			col_section.read(needs_table_icon_header_text_color);
			col_section.read(needs_table_icon_text_alignment);
			needs_table_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_name_column_start = running_w_total;
			col_section.read(needs_table_name_column_width);
			running_w_total += needs_table_name_column_width;
			col_section.read(needs_table_name_column_text_color);
			col_section.read(needs_table_name_header_text_color);
			col_section.read(needs_table_name_text_alignment);
			needs_table_amount_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_amount_column_start = running_w_total;
			col_section.read(needs_table_amount_column_width);
			running_w_total += needs_table_amount_column_width;
			col_section.read(needs_table_amount_column_text_color);
			col_section.read(needs_table_amount_header_text_color);
			col_section.read(needs_table_amount_text_alignment);
			needs_table_cost_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_cost_column_start = running_w_total;
			col_section.read(needs_table_cost_column_width);
			running_w_total += needs_table_cost_column_width;
			col_section.read(needs_table_cost_column_text_color);
			col_section.read(needs_table_cost_header_text_color);
			col_section.read(needs_table_cost_text_alignment);
			needs_table_weight_header_text_key = col_section.read<std::string_view>();
			needs_table_weight_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			needs_table_weight_column_start = running_w_total;
			col_section.read(needs_table_weight_column_width);
			running_w_total += needs_table_weight_column_width;
			col_section.read(needs_table_weight_column_text_color);
			col_section.read(needs_table_weight_header_text_color);
			col_section.read(needs_table_weight_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabprom_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			prom_table_ascending_icon_key = main_section.read<std::string_view>();
			prom_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_icon_column_start = running_w_total;
			col_section.read(prom_table_icon_column_width);
			running_w_total += prom_table_icon_column_width;
			col_section.read(prom_table_icon_column_text_color);
			col_section.read(prom_table_icon_header_text_color);
			col_section.read(prom_table_icon_text_alignment);
			prom_table_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_name_column_start = running_w_total;
			col_section.read(prom_table_name_column_width);
			running_w_total += prom_table_name_column_width;
			col_section.read(prom_table_name_column_text_color);
			col_section.read(prom_table_name_header_text_color);
			col_section.read(prom_table_name_text_alignment);
			prom_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_weight_column_start = running_w_total;
			col_section.read(prom_table_weight_column_width);
			running_w_total += prom_table_weight_column_width;
			col_section.read(prom_table_weight_column_text_color);
			col_section.read(prom_table_weight_header_text_color);
			col_section.read(prom_table_weight_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabweights_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			weights_table_ascending_icon_key = main_section.read<std::string_view>();
			weights_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			weights_table_item_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			weights_table_item_column_start = running_w_total;
			col_section.read(weights_table_item_column_width);
			running_w_total += weights_table_item_column_width;
			col_section.read(weights_table_item_column_text_color);
			col_section.read(weights_table_item_header_text_color);
			col_section.read(weights_table_item_text_alignment);
			weights_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			weights_table_weight_column_start = running_w_total;
			col_section.read(weights_table_weight_column_width);
			running_w_total += weights_table_weight_column_width;
			col_section.read(weights_table_weight_column_text_color);
			col_section.read(weights_table_weight_header_text_color);
			col_section.read(weights_table_weight_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabemm_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			emm_table_ascending_icon_key = main_section.read<std::string_view>();
			emm_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_flag_column_start = running_w_total;
			col_section.read(emm_table_flag_column_width);
			running_w_total += emm_table_flag_column_width;
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			emm_table_destination_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_destination_column_start = running_w_total;
			col_section.read(emm_table_destination_column_width);
			running_w_total += emm_table_destination_column_width;
			col_section.read(emm_table_destination_column_text_color);
			col_section.read(emm_table_destination_header_text_color);
			col_section.read(emm_table_destination_text_alignment);
			emm_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_weight_column_start = running_w_total;
			col_section.read(emm_table_weight_column_width);
			running_w_total += emm_table_weight_column_width;
			col_section.read(emm_table_weight_column_text_color);
			col_section.read(emm_table_weight_header_text_color);
			col_section.read(emm_table_weight_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabmig_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			mig_table_ascending_icon_key = main_section.read<std::string_view>();
			mig_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			mig_table_destination_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			mig_table_destination_column_start = running_w_total;
			col_section.read(mig_table_destination_column_width);
			running_w_total += mig_table_destination_column_width;
			col_section.read(mig_table_destination_column_text_color);
			col_section.read(mig_table_destination_header_text_color);
			col_section.read(mig_table_destination_text_alignment);
			mig_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			mig_table_weight_column_start = running_w_total;
			col_section.read(mig_table_weight_column_width);
			running_w_total += mig_table_weight_column_width;
			col_section.read(mig_table_weight_column_text_color);
			col_section.read(mig_table_weight_header_text_color);
			col_section.read(mig_table_weight_text_alignment);
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
	ln_list.on_create(state, this);
	en_list.on_create(state, this);
	lx_list.on_create(state, this);
	prom_list.on_create(state, this);
	dem_list.on_create(state, this);
	ideo_list.on_create(state, this);
	iss_list.on_create(state, this);
	emm_list.on_create(state, this);
	mig_list.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
	{
		auto ptr = make_pop_budget_details_main(state);
		budget_w = ptr.get();
		budget_w->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(ptr));
	}
// END
}
std::unique_ptr<ui::element_base> make_pop_details_main(sys::state& state) {
	auto ptr = std::make_unique<pop_details_main_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_needs_row_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_needs_row_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_needs_row_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->needs_table_icon_column_start && x < table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width) {
	}
	if(x >= table_source->needs_table_name_column_start && x < table_source->needs_table_name_column_start + table_source->needs_table_name_column_width) {
	}
	if(x >= table_source->needs_table_amount_column_start && x < table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width) {
	}
	if(x >= table_source->needs_table_cost_column_start && x < table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width) {
	}
	if(x >= table_source->needs_table_weight_column_start && x < table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_needs_row_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->needs_table_icon_column_start && x <  table_source->needs_table_icon_column_start +  table_source->needs_table_icon_column_width) {
	}
	if(x >=  table_source->needs_table_name_column_start && x <  table_source->needs_table_name_column_start +  table_source->needs_table_name_column_width) {
	}
	if(x >=  table_source->needs_table_amount_column_start && x <  table_source->needs_table_amount_column_start +  table_source->needs_table_amount_column_width) {
	}
	if(x >=  table_source->needs_table_cost_column_start && x <  table_source->needs_table_cost_column_start +  table_source->needs_table_cost_column_width) {
	}
	if(x >=  table_source->needs_table_weight_column_start && x <  table_source->needs_table_weight_column_start +  table_source->needs_table_weight_column_width) {
	}
}
void pop_details_needs_row_content_t::set_icon_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  icon_cached_text) {
		icon_cached_text = new_text;
		icon_internal_layout.contents.clear();
		icon_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ icon_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_icon_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_icon_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, icon_cached_text);
		}
	} else {
	}
}
void pop_details_needs_row_content_t::set_name_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  name_cached_text) {
		name_cached_text = new_text;
		name_internal_layout.contents.clear();
		name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_name_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, name_cached_text);
		}
	} else {
	}
}
void pop_details_needs_row_content_t::set_amount_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  amount_cached_text) {
		amount_cached_text = new_text;
		amount_internal_layout.contents.clear();
		amount_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ amount_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_amount_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_amount_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, amount_cached_text);
		}
	} else {
	}
}
void pop_details_needs_row_content_t::set_cost_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  cost_cached_text) {
		cost_cached_text = new_text;
		cost_internal_layout.contents.clear();
		cost_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ cost_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_cost_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_cost_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, cost_cached_text);
		}
	} else {
	}
}
void pop_details_needs_row_content_t::set_weight_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  weight_cached_text) {
		weight_cached_text = new_text;
		weight_internal_layout.contents.clear();
		weight_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_weight_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, weight_cached_text);
		}
	} else {
	}
}
void pop_details_needs_row_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_icon = rel_mouse_x >= table_source->needs_table_icon_column_start && rel_mouse_x < (table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_icon_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y), float(table_source->needs_table_icon_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_icon_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_icon = state.ui_templates.colors[icon_text_color]; 	if(!icon_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : icon_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_icon_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_icon.r, col_color_icon.g, col_color_icon.b }, ogl::color_modification::none);
		}
	}
	bool col_um_name = rel_mouse_x >= table_source->needs_table_name_column_start && rel_mouse_x < (table_source->needs_table_name_column_start + table_source->needs_table_name_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y), float(table_source->needs_table_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_name = state.ui_templates.colors[name_text_color]; 	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_name_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_name.r, col_color_name.g, col_color_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_amount = rel_mouse_x >= table_source->needs_table_amount_column_start && rel_mouse_x < (table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_amount_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y), float(table_source->needs_table_amount_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_amount_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_amount = state.ui_templates.colors[amount_text_color]; 	if(!amount_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : amount_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_amount_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_amount.r, col_color_amount.g, col_color_amount.b }, ogl::color_modification::none);
		}
	}
	bool col_um_cost = rel_mouse_x >= table_source->needs_table_cost_column_start && rel_mouse_x < (table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_cost){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_cost_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_cost){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_cost){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y), float(table_source->needs_table_cost_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_cost_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_cost = state.ui_templates.colors[cost_text_color]; 	if(!cost_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : cost_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_cost_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_cost.r, col_color_cost.g, col_color_cost.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->needs_table_weight_column_start && rel_mouse_x < (table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y), float(table_source->needs_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[weight_text_color]; 	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_weight_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
}
void pop_details_needs_row_content_t::on_update(sys::state& state) noexcept {
	pop_details_needs_row_t& needs_row = *((pop_details_needs_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN needs_row::content::update
	set_amount_text(state, text::format_float(needs_row.amount, 1));
	set_name_text(state, text::produce_simple_string(state, state.world.commodity_get_name(needs_row.commodity)));

	auto l = state.world.pop_get_province_from_pop_location(main.for_pop);
	auto s = state.world.province_get_state_membership(l);
	auto m = state.world.state_instance_get_market_from_local_market(s);
	set_cost_text(state, text::format_money(needs_row.amount * state.world.market_get_price(m, needs_row.commodity)));
	set_weight_text(state, text::format_float(needs_row.weight * 100.f, 0));
// END
}
void pop_details_needs_row_content_t::on_create(sys::state& state) noexcept {
// BEGIN needs_row::content::create
// END
}
ui::message_result pop_details_needs_row_need_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result pop_details_needs_row_need_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void pop_details_needs_row_need_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void pop_details_needs_row_need_icon_t::on_update(sys::state& state) noexcept {
	pop_details_needs_row_t& needs_row = *((pop_details_needs_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN needs_row::need_icon::update
	frame = int32_t(state.world.commodity_get_icon(needs_row.commodity));
// END
}
void pop_details_needs_row_need_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN needs_row::need_icon::create
// END
}
void  pop_details_needs_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_needs_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_needs_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_needs_row_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN needs_row::update
// END
	remake_layout(state, true);
}
void pop_details_needs_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
				} else
				if(cname == "need_icon") {
					temp.ptr = need_icon.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_needs_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::needs_row"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_needs_row_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == "need_icon") {
			need_icon = std::make_unique<pop_details_needs_row_need_icon_t>();
			need_icon->parent = this;
			auto cptr = need_icon.get();
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
		if(child_data.name == ".tabneeds_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			needs_table_ascending_icon_key = main_section.read<std::string_view>();
			needs_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_icon_column_start = running_w_total;
			col_section.read(needs_table_icon_column_width);
			running_w_total += needs_table_icon_column_width;
			col_section.read(needs_table_icon_column_text_color);
			col_section.read(needs_table_icon_header_text_color);
			col_section.read(needs_table_icon_text_alignment);
			needs_table_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_name_column_start = running_w_total;
			col_section.read(needs_table_name_column_width);
			running_w_total += needs_table_name_column_width;
			col_section.read(needs_table_name_column_text_color);
			col_section.read(needs_table_name_header_text_color);
			col_section.read(needs_table_name_text_alignment);
			needs_table_amount_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_amount_column_start = running_w_total;
			col_section.read(needs_table_amount_column_width);
			running_w_total += needs_table_amount_column_width;
			col_section.read(needs_table_amount_column_text_color);
			col_section.read(needs_table_amount_header_text_color);
			col_section.read(needs_table_amount_text_alignment);
			needs_table_cost_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_cost_column_start = running_w_total;
			col_section.read(needs_table_cost_column_width);
			running_w_total += needs_table_cost_column_width;
			col_section.read(needs_table_cost_column_text_color);
			col_section.read(needs_table_cost_header_text_color);
			col_section.read(needs_table_cost_text_alignment);
			needs_table_weight_header_text_key = col_section.read<std::string_view>();
			needs_table_weight_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			needs_table_weight_column_start = running_w_total;
			col_section.read(needs_table_weight_column_width);
			running_w_total += needs_table_weight_column_width;
			col_section.read(needs_table_weight_column_text_color);
			col_section.read(needs_table_weight_header_text_color);
			col_section.read(needs_table_weight_text_alignment);
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
// BEGIN needs_row::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_needs_row(sys::state& state) {
	auto ptr = std::make_unique<pop_details_needs_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_needs_header_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->needs_table_name_column_start && x < table_source->needs_table_name_column_start + table_source->needs_table_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->needs_table_name_sort_direction;
		table_source->needs_table_name_sort_direction = 0;
		table_source->needs_table_amount_sort_direction = 0;
		table_source->needs_table_cost_sort_direction = 0;
		table_source->needs_table_weight_sort_direction = 0;
		table_source->needs_table_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->needs_table_amount_column_start && x < table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->needs_table_amount_sort_direction;
		table_source->needs_table_name_sort_direction = 0;
		table_source->needs_table_amount_sort_direction = 0;
		table_source->needs_table_cost_sort_direction = 0;
		table_source->needs_table_weight_sort_direction = 0;
		table_source->needs_table_amount_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->needs_table_cost_column_start && x < table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->needs_table_cost_sort_direction;
		table_source->needs_table_name_sort_direction = 0;
		table_source->needs_table_amount_sort_direction = 0;
		table_source->needs_table_cost_sort_direction = 0;
		table_source->needs_table_weight_sort_direction = 0;
		table_source->needs_table_cost_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->needs_table_weight_column_start && x < table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->needs_table_weight_sort_direction;
		table_source->needs_table_name_sort_direction = 0;
		table_source->needs_table_amount_sort_direction = 0;
		table_source->needs_table_cost_sort_direction = 0;
		table_source->needs_table_weight_sort_direction = 0;
		table_source->needs_table_weight_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result pop_details_needs_header_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_needs_header_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->needs_table_icon_column_start && x < table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width) {
	}
	if(x >= table_source->needs_table_name_column_start && x < table_source->needs_table_name_column_start + table_source->needs_table_name_column_width) {
	}
	if(x >= table_source->needs_table_amount_column_start && x < table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width) {
	}
	if(x >= table_source->needs_table_cost_column_start && x < table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width) {
	}
	if(x >= table_source->needs_table_weight_column_start && x < table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width) {
		ident = 4;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->needs_table_weight_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->needs_table_weight_column_width);
		return;
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_needs_header_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->needs_table_icon_column_start && x <  table_source->needs_table_icon_column_start +  table_source->needs_table_icon_column_width) {
	}
	if(x >=  table_source->needs_table_name_column_start && x <  table_source->needs_table_name_column_start +  table_source->needs_table_name_column_width) {
	}
	if(x >=  table_source->needs_table_amount_column_start && x <  table_source->needs_table_amount_column_start +  table_source->needs_table_amount_column_width) {
	}
	if(x >=  table_source->needs_table_cost_column_start && x <  table_source->needs_table_cost_column_start +  table_source->needs_table_cost_column_width) {
	}
	if(x >=  table_source->needs_table_weight_column_start && x <  table_source->needs_table_weight_column_start +  table_source->needs_table_weight_column_width) {
	text::add_line(state, contents, table_source->needs_table_weight_header_tooltip_key);
	}
}
void pop_details_needs_header_content_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	{
	name_cached_text = text::produce_simple_string(state, table_source->needs_table_name_header_text_key);
	 name_internal_layout.contents.clear();
	 name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_name_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, name_cached_text);
	}
	{
	amount_cached_text = text::produce_simple_string(state, table_source->needs_table_amount_header_text_key);
	 amount_internal_layout.contents.clear();
	 amount_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  amount_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_amount_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_amount_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, amount_cached_text);
	}
	{
	cost_cached_text = text::produce_simple_string(state, table_source->needs_table_cost_header_text_key);
	 cost_internal_layout.contents.clear();
	 cost_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  cost_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_cost_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_cost_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, cost_cached_text);
	}
	{
	weight_cached_text = text::produce_simple_string(state, table_source->needs_table_weight_header_text_key);
	 weight_internal_layout.contents.clear();
	 weight_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->needs_table_weight_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->needs_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, weight_cached_text);
	}
}
void pop_details_needs_header_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_icon = rel_mouse_x >= table_source->needs_table_icon_column_start && rel_mouse_x < (table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_icon_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start + table_source->needs_table_icon_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y), float(table_source->needs_table_icon_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_icon_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_icon_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_icon = state.ui_templates.colors[table_source->needs_table_icon_header_text_color]; 	bool col_um_name = rel_mouse_x >= table_source->needs_table_name_column_start && rel_mouse_x < (table_source->needs_table_name_column_start + table_source->needs_table_name_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_name_column_start), float(y), float(table_source->needs_table_name_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->needs_table_name_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start + table_source->needs_table_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y), float(table_source->needs_table_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_name_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_name = state.ui_templates.colors[table_source->needs_table_name_header_text_color]; 	if(table_source->needs_table_name_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->needs_table_name_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_name_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_name.r, col_color_name.g, col_color_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_amount = rel_mouse_x >= table_source->needs_table_amount_column_start && rel_mouse_x < (table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_amount) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_amount_column_start), float(y), float(table_source->needs_table_amount_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->needs_table_amount_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_amount_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start + table_source->needs_table_amount_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_amount){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y), float(table_source->needs_table_amount_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_amount_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_amount_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_amount = state.ui_templates.colors[table_source->needs_table_amount_header_text_color]; 	if(table_source->needs_table_amount_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_amount_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->needs_table_amount_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_amount_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!amount_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : amount_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_amount_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_amount.r, col_color_amount.g, col_color_amount.b }, ogl::color_modification::none);
		}
	}
	bool col_um_cost = rel_mouse_x >= table_source->needs_table_cost_column_start && rel_mouse_x < (table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_cost) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_cost_column_start), float(y), float(table_source->needs_table_cost_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->needs_table_cost_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_cost){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_cost_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_cost){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start + table_source->needs_table_cost_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_cost){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y), float(table_source->needs_table_cost_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_cost_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_cost_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_cost = state.ui_templates.colors[table_source->needs_table_cost_header_text_color]; 	if(table_source->needs_table_cost_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_cost_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->needs_table_cost_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_cost_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!cost_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : cost_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_cost_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_cost.r, col_color_cost.g, col_color_cost.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->needs_table_weight_column_start && rel_mouse_x < (table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_weight_column_start), float(y), float(table_source->needs_table_weight_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->needs_table_weight_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->needs_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start + table_source->needs_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y), float(table_source->needs_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->needs_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->needs_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[table_source->needs_table_weight_header_text_color]; 	if(table_source->needs_table_weight_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->needs_table_weight_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->needs_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->needs_table_weight_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void pop_details_needs_header_content_t::on_update(sys::state& state) noexcept {
	pop_details_needs_header_t& needs_header = *((pop_details_needs_header_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN needs_header::content::update
// END
}
void pop_details_needs_header_content_t::on_create(sys::state& state) noexcept {
// BEGIN needs_header::content::create
// END
}
void  pop_details_needs_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_needs_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_needs_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_needs_header_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN needs_header::update
// END
	remake_layout(state, true);
}
void pop_details_needs_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_needs_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::needs_header"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_needs_header_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == ".tabneeds_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			needs_table_ascending_icon_key = main_section.read<std::string_view>();
			needs_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_icon_column_start = running_w_total;
			col_section.read(needs_table_icon_column_width);
			running_w_total += needs_table_icon_column_width;
			col_section.read(needs_table_icon_column_text_color);
			col_section.read(needs_table_icon_header_text_color);
			col_section.read(needs_table_icon_text_alignment);
			needs_table_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_name_column_start = running_w_total;
			col_section.read(needs_table_name_column_width);
			running_w_total += needs_table_name_column_width;
			col_section.read(needs_table_name_column_text_color);
			col_section.read(needs_table_name_header_text_color);
			col_section.read(needs_table_name_text_alignment);
			needs_table_amount_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_amount_column_start = running_w_total;
			col_section.read(needs_table_amount_column_width);
			running_w_total += needs_table_amount_column_width;
			col_section.read(needs_table_amount_column_text_color);
			col_section.read(needs_table_amount_header_text_color);
			col_section.read(needs_table_amount_text_alignment);
			needs_table_cost_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			needs_table_cost_column_start = running_w_total;
			col_section.read(needs_table_cost_column_width);
			running_w_total += needs_table_cost_column_width;
			col_section.read(needs_table_cost_column_text_color);
			col_section.read(needs_table_cost_header_text_color);
			col_section.read(needs_table_cost_text_alignment);
			needs_table_weight_header_text_key = col_section.read<std::string_view>();
			needs_table_weight_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			needs_table_weight_column_start = running_w_total;
			col_section.read(needs_table_weight_column_width);
			running_w_total += needs_table_weight_column_width;
			col_section.read(needs_table_weight_column_text_color);
			col_section.read(needs_table_weight_header_text_color);
			col_section.read(needs_table_weight_text_alignment);
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
// BEGIN needs_header::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_needs_header(sys::state& state) {
	auto ptr = std::make_unique<pop_details_needs_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_prom_header_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->prom_table_name_column_start && x < table_source->prom_table_name_column_start + table_source->prom_table_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->prom_table_name_sort_direction;
		table_source->prom_table_name_sort_direction = 0;
		table_source->prom_table_weight_sort_direction = 0;
		table_source->prom_table_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->prom_table_weight_column_start && x < table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->prom_table_weight_sort_direction;
		table_source->prom_table_name_sort_direction = 0;
		table_source->prom_table_weight_sort_direction = 0;
		table_source->prom_table_weight_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result pop_details_prom_header_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_prom_header_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->prom_table_icon_column_start && x < table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width) {
	}
	if(x >= table_source->prom_table_name_column_start && x < table_source->prom_table_name_column_start + table_source->prom_table_name_column_width) {
	}
	if(x >= table_source->prom_table_weight_column_start && x < table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_prom_header_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->prom_table_icon_column_start && x <  table_source->prom_table_icon_column_start +  table_source->prom_table_icon_column_width) {
	}
	if(x >=  table_source->prom_table_name_column_start && x <  table_source->prom_table_name_column_start +  table_source->prom_table_name_column_width) {
	}
	if(x >=  table_source->prom_table_weight_column_start && x <  table_source->prom_table_weight_column_start +  table_source->prom_table_weight_column_width) {
	}
}
void pop_details_prom_header_content_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	{
	name_cached_text = text::produce_simple_string(state, table_source->prom_table_name_header_text_key);
	 name_internal_layout.contents.clear();
	 name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->prom_table_name_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->prom_table_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, name_cached_text);
	}
	{
	weight_cached_text = text::produce_simple_string(state, table_source->prom_table_weight_header_text_key);
	 weight_internal_layout.contents.clear();
	 weight_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->prom_table_weight_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->prom_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, weight_cached_text);
	}
}
void pop_details_prom_header_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_icon = rel_mouse_x >= table_source->prom_table_icon_column_start && rel_mouse_x < (table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->prom_table_icon_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y), float(table_source->prom_table_icon_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y + base_data.size.y - 2), float(table_source->prom_table_icon_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_icon = state.ui_templates.colors[table_source->prom_table_icon_header_text_color]; 	bool col_um_name = rel_mouse_x >= table_source->prom_table_name_column_start && rel_mouse_x < (table_source->prom_table_name_column_start + table_source->prom_table_name_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->prom_table_name_column_start), float(y), float(table_source->prom_table_name_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->prom_table_name_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->prom_table_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y), float(table_source->prom_table_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y + base_data.size.y - 2), float(table_source->prom_table_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_name = state.ui_templates.colors[table_source->prom_table_name_header_text_color]; 	if(table_source->prom_table_name_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->prom_table_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->prom_table_name_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->prom_table_name_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->prom_table_name_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_name.r, col_color_name.g, col_color_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->prom_table_weight_column_start && rel_mouse_x < (table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->prom_table_weight_column_start), float(y), float(table_source->prom_table_weight_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->prom_table_weight_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->prom_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y), float(table_source->prom_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->prom_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[table_source->prom_table_weight_header_text_color]; 	if(table_source->prom_table_weight_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->prom_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->prom_table_weight_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->prom_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->prom_table_weight_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void pop_details_prom_header_content_t::on_update(sys::state& state) noexcept {
	pop_details_prom_header_t& prom_header = *((pop_details_prom_header_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN prom_header::content::update
// END
}
void pop_details_prom_header_content_t::on_create(sys::state& state) noexcept {
// BEGIN prom_header::content::create
// END
}
void  pop_details_prom_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_prom_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_prom_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_prom_header_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN prom_header::update
// END
	remake_layout(state, true);
}
void pop_details_prom_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_prom_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::prom_header"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_prom_header_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == ".tabprom_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			prom_table_ascending_icon_key = main_section.read<std::string_view>();
			prom_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_icon_column_start = running_w_total;
			col_section.read(prom_table_icon_column_width);
			running_w_total += prom_table_icon_column_width;
			col_section.read(prom_table_icon_column_text_color);
			col_section.read(prom_table_icon_header_text_color);
			col_section.read(prom_table_icon_text_alignment);
			prom_table_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_name_column_start = running_w_total;
			col_section.read(prom_table_name_column_width);
			running_w_total += prom_table_name_column_width;
			col_section.read(prom_table_name_column_text_color);
			col_section.read(prom_table_name_header_text_color);
			col_section.read(prom_table_name_text_alignment);
			prom_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_weight_column_start = running_w_total;
			col_section.read(prom_table_weight_column_width);
			running_w_total += prom_table_weight_column_width;
			col_section.read(prom_table_weight_column_text_color);
			col_section.read(prom_table_weight_header_text_color);
			col_section.read(prom_table_weight_text_alignment);
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
// BEGIN prom_header::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_prom_header(sys::state& state) {
	auto ptr = std::make_unique<pop_details_prom_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_prom_row_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_prom_row_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_prom_row_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->prom_table_icon_column_start && x < table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width) {
	}
	if(x >= table_source->prom_table_name_column_start && x < table_source->prom_table_name_column_start + table_source->prom_table_name_column_width) {
	}
	if(x >= table_source->prom_table_weight_column_start && x < table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width) {
		ident = 2;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->prom_table_weight_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->prom_table_weight_column_width);
		return;
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_prom_row_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->prom_table_icon_column_start && x <  table_source->prom_table_icon_column_start +  table_source->prom_table_icon_column_width) {
	}
	if(x >=  table_source->prom_table_name_column_start && x <  table_source->prom_table_name_column_start +  table_source->prom_table_name_column_width) {
	}
	if(x >=  table_source->prom_table_weight_column_start && x <  table_source->prom_table_weight_column_start +  table_source->prom_table_weight_column_width) {
	pop_details_prom_row_t& prom_row = *((pop_details_prom_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN prom_row::content::weight::column_tooltip
	auto type = state.world.pop_get_poptype(main.for_pop);
	ui::additive_value_modifier_description(state, contents, state.world.pop_type_get_promotion(type, prom_row.value), trigger::to_generic(main.for_pop), trigger::to_generic(main.for_pop), 0);
// END
	}
}
void pop_details_prom_row_content_t::set_icon_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  icon_cached_text) {
		icon_cached_text = new_text;
		icon_internal_layout.contents.clear();
		icon_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ icon_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->prom_table_icon_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->prom_table_icon_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, icon_cached_text);
		}
	} else {
	}
}
void pop_details_prom_row_content_t::set_name_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  name_cached_text) {
		name_cached_text = new_text;
		name_internal_layout.contents.clear();
		name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->prom_table_name_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->prom_table_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, name_cached_text);
		}
	} else {
	}
}
void pop_details_prom_row_content_t::set_weight_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  weight_cached_text) {
		weight_cached_text = new_text;
		weight_internal_layout.contents.clear();
		weight_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->prom_table_weight_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->prom_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, weight_cached_text);
		}
	} else {
	}
}
void pop_details_prom_row_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_icon = rel_mouse_x >= table_source->prom_table_icon_column_start && rel_mouse_x < (table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->prom_table_icon_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start + table_source->prom_table_icon_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y), float(table_source->prom_table_icon_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_icon_column_start), float(y + base_data.size.y - 2), float(table_source->prom_table_icon_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_icon = state.ui_templates.colors[icon_text_color]; 	if(!icon_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : icon_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->prom_table_icon_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_icon.r, col_color_icon.g, col_color_icon.b }, ogl::color_modification::none);
		}
	}
	bool col_um_name = rel_mouse_x >= table_source->prom_table_name_column_start && rel_mouse_x < (table_source->prom_table_name_column_start + table_source->prom_table_name_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->prom_table_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start + table_source->prom_table_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y), float(table_source->prom_table_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_name_column_start), float(y + base_data.size.y - 2), float(table_source->prom_table_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_name = state.ui_templates.colors[name_text_color]; 	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->prom_table_name_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_name.r, col_color_name.g, col_color_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->prom_table_weight_column_start && rel_mouse_x < (table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->prom_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start + table_source->prom_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y), float(table_source->prom_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->prom_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->prom_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[weight_text_color]; 	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->prom_table_weight_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
}
void pop_details_prom_row_content_t::on_update(sys::state& state) noexcept {
	pop_details_prom_row_t& prom_row = *((pop_details_prom_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN prom_row::content::update
	auto type = state.world.pop_get_poptype(main.for_pop);

	auto this_pop_strata = state.world.pop_type_get_strata(type);
	auto row_pop_strata = state.world.pop_type_get_strata(prom_row.value);

	float weight;
	// display promotions
	if(row_pop_strata >= this_pop_strata) {
		weight = demographics::get_single_promotion_demotion_target_weight<demographics::promotion_type::promotion>(state, main.for_pop, prom_row.value);
	}
	// display demotions
	else {
		weight = demographics::get_single_promotion_demotion_target_weight<demographics::promotion_type::demotion>(state, main.for_pop, prom_row.value);
	}

	set_name_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(prom_row.value)));
	set_weight_text(state, text::format_float(weight, 2));
// END
}
void pop_details_prom_row_content_t::on_create(sys::state& state) noexcept {
// BEGIN prom_row::content::create
// END
}
ui::message_result pop_details_prom_row_job_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result pop_details_prom_row_job_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void pop_details_prom_row_job_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void pop_details_prom_row_job_icon_t::on_update(sys::state& state) noexcept {
	pop_details_prom_row_t& prom_row = *((pop_details_prom_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN prom_row::job_icon::update
	frame = int32_t(state.world.pop_type_get_sprite(prom_row.value)) - 1;
// END
}
void pop_details_prom_row_job_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN prom_row::job_icon::create
// END
}
void  pop_details_prom_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_prom_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_prom_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_prom_row_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN prom_row::update
// END
	remake_layout(state, true);
}
void pop_details_prom_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
				} else
				if(cname == "job_icon") {
					temp.ptr = job_icon.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_prom_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::prom_row"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_prom_row_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == "job_icon") {
			job_icon = std::make_unique<pop_details_prom_row_job_icon_t>();
			job_icon->parent = this;
			auto cptr = job_icon.get();
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
		if(child_data.name == ".tabprom_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			prom_table_ascending_icon_key = main_section.read<std::string_view>();
			prom_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_icon_column_start = running_w_total;
			col_section.read(prom_table_icon_column_width);
			running_w_total += prom_table_icon_column_width;
			col_section.read(prom_table_icon_column_text_color);
			col_section.read(prom_table_icon_header_text_color);
			col_section.read(prom_table_icon_text_alignment);
			prom_table_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_name_column_start = running_w_total;
			col_section.read(prom_table_name_column_width);
			running_w_total += prom_table_name_column_width;
			col_section.read(prom_table_name_column_text_color);
			col_section.read(prom_table_name_header_text_color);
			col_section.read(prom_table_name_text_alignment);
			prom_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			prom_table_weight_column_start = running_w_total;
			col_section.read(prom_table_weight_column_width);
			running_w_total += prom_table_weight_column_width;
			col_section.read(prom_table_weight_column_text_color);
			col_section.read(prom_table_weight_header_text_color);
			col_section.read(prom_table_weight_text_alignment);
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
// BEGIN prom_row::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_prom_row(sys::state& state) {
	auto ptr = std::make_unique<pop_details_prom_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_weights_header_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->weights_table_item_column_start && x < table_source->weights_table_item_column_start + table_source->weights_table_item_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->weights_table_item_sort_direction;
		table_source->weights_table_item_sort_direction = 0;
		table_source->weights_table_weight_sort_direction = 0;
		table_source->weights_table_item_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->weights_table_weight_column_start && x < table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->weights_table_weight_sort_direction;
		table_source->weights_table_item_sort_direction = 0;
		table_source->weights_table_weight_sort_direction = 0;
		table_source->weights_table_weight_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result pop_details_weights_header_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_weights_header_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->weights_table_item_column_start && x < table_source->weights_table_item_column_start + table_source->weights_table_item_column_width) {
	}
	if(x >= table_source->weights_table_weight_column_start && x < table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_weights_header_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->weights_table_item_column_start && x <  table_source->weights_table_item_column_start +  table_source->weights_table_item_column_width) {
	}
	if(x >=  table_source->weights_table_weight_column_start && x <  table_source->weights_table_weight_column_start +  table_source->weights_table_weight_column_width) {
	}
}
void pop_details_weights_header_content_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	{
	item_cached_text = text::produce_simple_string(state, table_source->weights_table_item_header_text_key);
	 item_internal_layout.contents.clear();
	 item_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  item_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->weights_table_item_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->weights_table_item_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, item_cached_text);
	}
	{
	weight_cached_text = text::produce_simple_string(state, table_source->weights_table_weight_header_text_key);
	 weight_internal_layout.contents.clear();
	 weight_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->weights_table_weight_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->weights_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, weight_cached_text);
	}
}
void pop_details_weights_header_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_item = rel_mouse_x >= table_source->weights_table_item_column_start && rel_mouse_x < (table_source->weights_table_item_column_start + table_source->weights_table_item_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->weights_table_item_column_start), float(y), float(table_source->weights_table_item_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->weights_table_item_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->weights_table_item_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_item){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_item){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y), float(table_source->weights_table_item_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y + base_data.size.y - 2), float(table_source->weights_table_item_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_item = state.ui_templates.colors[table_source->weights_table_item_header_text_color]; 	if(table_source->weights_table_item_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->weights_table_item_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->weights_table_item_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->weights_table_item_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!item_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : item_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->weights_table_item_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_item.r, col_color_item.g, col_color_item.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->weights_table_weight_column_start && rel_mouse_x < (table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->weights_table_weight_column_start), float(y), float(table_source->weights_table_weight_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->weights_table_weight_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->weights_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y), float(table_source->weights_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->weights_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[table_source->weights_table_weight_header_text_color]; 	if(table_source->weights_table_weight_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->weights_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->weights_table_weight_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->weights_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->weights_table_weight_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void pop_details_weights_header_content_t::on_update(sys::state& state) noexcept {
	pop_details_weights_header_t& weights_header = *((pop_details_weights_header_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN weights_header::content::update
// END
}
void pop_details_weights_header_content_t::on_create(sys::state& state) noexcept {
// BEGIN weights_header::content::create
// END
}
void  pop_details_weights_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_weights_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_weights_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_weights_header_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN weights_header::update
// END
	remake_layout(state, true);
}
void pop_details_weights_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_weights_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::weights_header"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_weights_header_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == ".tabweights_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			weights_table_ascending_icon_key = main_section.read<std::string_view>();
			weights_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			weights_table_item_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			weights_table_item_column_start = running_w_total;
			col_section.read(weights_table_item_column_width);
			running_w_total += weights_table_item_column_width;
			col_section.read(weights_table_item_column_text_color);
			col_section.read(weights_table_item_header_text_color);
			col_section.read(weights_table_item_text_alignment);
			weights_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			weights_table_weight_column_start = running_w_total;
			col_section.read(weights_table_weight_column_width);
			running_w_total += weights_table_weight_column_width;
			col_section.read(weights_table_weight_column_text_color);
			col_section.read(weights_table_weight_header_text_color);
			col_section.read(weights_table_weight_text_alignment);
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
// BEGIN weights_header::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_weights_header(sys::state& state) {
	auto ptr = std::make_unique<pop_details_weights_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_weights_row_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_weights_row_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_weights_row_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->weights_table_item_column_start && x < table_source->weights_table_item_column_start + table_source->weights_table_item_column_width) {
	}
	if(x >= table_source->weights_table_weight_column_start && x < table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width) {
		ident = 1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->weights_table_weight_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->weights_table_weight_column_width);
		return;
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_weights_row_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->weights_table_item_column_start && x <  table_source->weights_table_item_column_start +  table_source->weights_table_item_column_width) {
	}
	if(x >=  table_source->weights_table_weight_column_start && x <  table_source->weights_table_weight_column_start +  table_source->weights_table_weight_column_width) {
	pop_details_weights_row_t& weights_row = *((pop_details_weights_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN weights_row::content::weight::column_tooltip
	if(holds_alternative<dcon::ideology_id>(weights_row.source)) {
		auto i = std::get<dcon::ideology_id>(weights_row.source);

		auto ids = main.for_pop;
		auto type = state.world.pop_get_poptype(ids);

		if(state.world.ideology_get_enabled(i)) {
			if(state.world.ideology_get_is_civilized_only(i)) {
				if(state.world.nation_get_is_civilized(nations::owner_of_pop(state, ids))) {
					auto ptrigger = state.world.pop_type_get_ideology(type, i);
					if(ptrigger) {
						auto amount = trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(ids),
								trigger::to_generic(ids), 0);

						text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ amount });
						text::add_line_break_to_layout(state, contents);
						ui::multiplicative_value_modifier_description(state, contents, ptrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
					} else {
						text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ 0.0f });
						text::add_line_break_to_layout(state, contents);
						text::substitution_map map{};
						text::add_to_substitution_map(map, text::variable_type::val, text::substitution(int64_t(0)));
						auto box = text::open_layout_box(contents, 15);
						text::localised_format_box(state, contents, box, std::string_view("comwid_base"), map);
						text::close_layout_box(contents, box);
					}
				} else {
					text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, int64_t(0));
					text::add_line_break_to_layout(state, contents);
					text::add_line(state, contents, "pop_ideology_attraction_2");
				}
			} else {
				auto ptrigger = state.world.pop_type_get_ideology(type, i);
				if(ptrigger) {
					auto amount = trigger::evaluate_multiplicative_modifier(state, ptrigger, trigger::to_generic(ids),
							trigger::to_generic(ids), 0);

					text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ amount });
					text::add_line_break_to_layout(state, contents);
					ui::multiplicative_value_modifier_description(state, contents, ptrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
				} else {
					text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, text::fp_two_places{ 0.0f });
					text::add_line_break_to_layout(state, contents);
					text::substitution_map map{};
					text::add_to_substitution_map(map, text::variable_type::val, text::substitution(int64_t(0)));
					auto box = text::open_layout_box(contents, 15);
					text::localised_format_box(state, contents, box, std::string_view("comwid_base"), map);
					text::close_layout_box(contents, box);
				}
			}
		} else {
			text::add_line(state, contents, "pop_ideology_attraction_1", text::variable_type::x, int64_t(0));
			text::add_line_break_to_layout(state, contents);
			text::add_line(state, contents, "pop_ideology_attraction_2");
		}
	} else {
		auto issue = std::get<dcon::issue_option_id>(weights_row.source);
		auto ids = main.for_pop;

		auto opt = fatten(state.world, issue);
		auto allow = opt.get_allow();
		auto parent_issue = opt.get_parent_issue();
		auto is_party_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::party);
		auto is_social_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::social);
		auto is_political_issue = state.world.issue_get_issue_type(parent_issue) == uint8_t(culture::issue_type::political);
		auto has_modifier = is_social_issue || is_political_issue;

		auto modifier_key = is_social_issue ? sys::national_mod_offsets::social_reform_desire : sys::national_mod_offsets::political_reform_desire;

		auto owner = nations::owner_of_pop(state, ids);
		auto current_issue_setting = state.world.nation_get_issues(owner, parent_issue).id;
		auto allowed_by_owner = (state.world.nation_get_is_civilized(owner) || is_party_issue) &&
			(allow ? trigger::evaluate(state, allow, trigger::to_generic(owner), trigger::to_generic(owner), 0) : true) && (current_issue_setting != issue || is_party_issue) && (!state.world.issue_get_is_next_step_only(parent_issue) || (current_issue_setting.index() == issue.index() - 1) || (current_issue_setting.index() == issue.index() + 1));
		auto owner_modifier = has_modifier ? (state.world.nation_get_modifier_values(owner, modifier_key) + 1.0f) : 1.0f;
		auto pop_type = state.world.pop_get_poptype(ids);

		if(!allowed_by_owner) {
			text::add_line(state, contents, "pop_issue_attraction_1", text::variable_type::x, int64_t(0));
			text::add_line_break_to_layout(state, contents);
			text::add_line(state, contents, "pop_issue_attraction_2");
			return;
		}

		auto attraction_factor = [&]() {
			if(auto mtrigger = state.world.pop_type_get_issues(pop_type, issue); mtrigger) {
				return trigger::evaluate_multiplicative_modifier(state, mtrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
			} else {
				return 0.0f;
			}
			}();


		text::add_line(state, contents, "pop_issue_attraction_1", text::variable_type::x, text::fp_two_places{ attraction_factor * owner_modifier });
		text::add_line_break_to_layout(state, contents);
		if(has_modifier) {
			text::add_line(state, contents, "pop_issue_attraction_3");
			text::add_line(state, contents, "pop_issue_attraction_4", text::variable_type::x, text::fp_percentage{ owner_modifier });
			if(is_social_issue) {
				ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::social_reform_desire, false);
			} else {
				ui::active_modifiers_description(state, contents, owner, 15, sys::national_mod_offsets::political_reform_desire, false);
			}
			text::add_line(state, contents, "pop_issue_attraction_5", text::variable_type::x, text::fp_two_places{ attraction_factor });
			if(auto mtrigger = state.world.pop_type_get_issues(pop_type, issue); mtrigger) {
				ui::multiplicative_value_modifier_description(state, contents, mtrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
			}
		} else {
			if(auto mtrigger = state.world.pop_type_get_issues(pop_type, issue); mtrigger) {
				ui::multiplicative_value_modifier_description(state, contents, mtrigger, trigger::to_generic(ids), trigger::to_generic(ids), 0);
			}
		}
	}
// END
	}
}
void pop_details_weights_row_content_t::set_item_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  item_cached_text) {
		item_cached_text = new_text;
		item_internal_layout.contents.clear();
		item_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ item_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->weights_table_item_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->weights_table_item_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, item_cached_text);
		}
	} else {
	}
}
void pop_details_weights_row_content_t::set_weight_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  weight_cached_text) {
		weight_cached_text = new_text;
		weight_internal_layout.contents.clear();
		weight_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->weights_table_weight_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->weights_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, weight_cached_text);
		}
	} else {
	}
}
void pop_details_weights_row_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_item = rel_mouse_x >= table_source->weights_table_item_column_start && rel_mouse_x < (table_source->weights_table_item_column_start + table_source->weights_table_item_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_item){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->weights_table_item_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_item){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start + table_source->weights_table_item_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_item){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y), float(table_source->weights_table_item_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_item_column_start), float(y + base_data.size.y - 2), float(table_source->weights_table_item_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_item = state.ui_templates.colors[item_text_color]; 	if(!item_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : item_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->weights_table_item_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_item.r, col_color_item.g, col_color_item.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->weights_table_weight_column_start && rel_mouse_x < (table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->weights_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start + table_source->weights_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y), float(table_source->weights_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->weights_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->weights_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[weight_text_color]; 	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->weights_table_weight_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
}
void pop_details_weights_row_content_t::on_update(sys::state& state) noexcept {
	pop_details_weights_row_t& weights_row = *((pop_details_weights_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN weights_row::content::update
	set_item_text(state, weights_row.item_name);
	set_weight_text(state, text::format_float(weights_row.weight_value, 2));
// END
}
void pop_details_weights_row_content_t::on_create(sys::state& state) noexcept {
// BEGIN weights_row::content::create
// END
}
void  pop_details_weights_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_weights_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_weights_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_weights_row_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN weights_row::update
// END
	remake_layout(state, true);
}
void pop_details_weights_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_weights_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::weights_row"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_weights_row_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == ".tabweights_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			weights_table_ascending_icon_key = main_section.read<std::string_view>();
			weights_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			weights_table_item_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			weights_table_item_column_start = running_w_total;
			col_section.read(weights_table_item_column_width);
			running_w_total += weights_table_item_column_width;
			col_section.read(weights_table_item_column_text_color);
			col_section.read(weights_table_item_header_text_color);
			col_section.read(weights_table_item_text_alignment);
			weights_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			weights_table_weight_column_start = running_w_total;
			col_section.read(weights_table_weight_column_width);
			running_w_total += weights_table_weight_column_width;
			col_section.read(weights_table_weight_column_text_color);
			col_section.read(weights_table_weight_header_text_color);
			col_section.read(weights_table_weight_text_alignment);
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
// BEGIN weights_row::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_weights_row(sys::state& state) {
	auto ptr = std::make_unique<pop_details_weights_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_emm_header_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->emm_table_destination_column_start && x < table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->emm_table_destination_sort_direction;
		table_source->emm_table_destination_sort_direction = 0;
		table_source->emm_table_weight_sort_direction = 0;
		table_source->emm_table_destination_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->emm_table_weight_column_start && x < table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->emm_table_weight_sort_direction;
		table_source->emm_table_destination_sort_direction = 0;
		table_source->emm_table_weight_sort_direction = 0;
		table_source->emm_table_weight_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result pop_details_emm_header_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_emm_header_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->emm_table_destination_column_start && x < table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width) {
	}
	if(x >= table_source->emm_table_weight_column_start && x < table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_emm_header_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->emm_table_destination_column_start && x <  table_source->emm_table_destination_column_start +  table_source->emm_table_destination_column_width) {
	}
	if(x >=  table_source->emm_table_weight_column_start && x <  table_source->emm_table_weight_column_start +  table_source->emm_table_weight_column_width) {
	}
}
void pop_details_emm_header_content_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	{
	destination_cached_text = text::produce_simple_string(state, table_source->emm_table_destination_header_text_key);
	 destination_internal_layout.contents.clear();
	 destination_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  destination_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->emm_table_destination_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->emm_table_destination_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, destination_cached_text);
	}
	{
	weight_cached_text = text::produce_simple_string(state, table_source->emm_table_weight_header_text_key);
	 weight_internal_layout.contents.clear();
	 weight_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->emm_table_weight_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->emm_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, weight_cached_text);
	}
}
void pop_details_emm_header_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_flag = rel_mouse_x >= table_source->emm_table_flag_column_start && rel_mouse_x < (table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_flag){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->emm_table_flag_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_flag){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_flag){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y), float(table_source->emm_table_flag_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y + base_data.size.y - 2), float(table_source->emm_table_flag_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_destination = rel_mouse_x >= table_source->emm_table_destination_column_start && rel_mouse_x < (table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_destination) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->emm_table_destination_column_start), float(y), float(table_source->emm_table_destination_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->emm_table_destination_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->emm_table_destination_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y), float(table_source->emm_table_destination_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y + base_data.size.y - 2), float(table_source->emm_table_destination_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_destination = state.ui_templates.colors[table_source->emm_table_destination_header_text_color]; 	if(table_source->emm_table_destination_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->emm_table_destination_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->emm_table_destination_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->emm_table_destination_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!destination_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : destination_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->emm_table_destination_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_destination.r, col_color_destination.g, col_color_destination.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->emm_table_weight_column_start && rel_mouse_x < (table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->emm_table_weight_column_start), float(y), float(table_source->emm_table_weight_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->emm_table_weight_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->emm_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y), float(table_source->emm_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->emm_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[table_source->emm_table_weight_header_text_color]; 	if(table_source->emm_table_weight_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->emm_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->emm_table_weight_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->emm_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->emm_table_weight_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void pop_details_emm_header_content_t::on_update(sys::state& state) noexcept {
	pop_details_emm_header_t& emm_header = *((pop_details_emm_header_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN emm_header::content::update
// END
}
void pop_details_emm_header_content_t::on_create(sys::state& state) noexcept {
// BEGIN emm_header::content::create
// END
}
void  pop_details_emm_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_emm_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_emm_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_emm_header_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN emm_header::update
// END
	remake_layout(state, true);
}
void pop_details_emm_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_emm_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::emm_header"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_emm_header_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == ".tabemm_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			emm_table_ascending_icon_key = main_section.read<std::string_view>();
			emm_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_flag_column_start = running_w_total;
			col_section.read(emm_table_flag_column_width);
			running_w_total += emm_table_flag_column_width;
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			emm_table_destination_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_destination_column_start = running_w_total;
			col_section.read(emm_table_destination_column_width);
			running_w_total += emm_table_destination_column_width;
			col_section.read(emm_table_destination_column_text_color);
			col_section.read(emm_table_destination_header_text_color);
			col_section.read(emm_table_destination_text_alignment);
			emm_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_weight_column_start = running_w_total;
			col_section.read(emm_table_weight_column_width);
			running_w_total += emm_table_weight_column_width;
			col_section.read(emm_table_weight_column_text_color);
			col_section.read(emm_table_weight_header_text_color);
			col_section.read(emm_table_weight_text_alignment);
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
// BEGIN emm_header::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_emm_header(sys::state& state) {
	auto ptr = std::make_unique<pop_details_emm_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_emm_row_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_emm_row_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_emm_row_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->emm_table_destination_column_start && x < table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width) {
	}
	if(x >= table_source->emm_table_weight_column_start && x < table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width) {
		ident = 1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->emm_table_weight_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->emm_table_weight_column_width);
		return;
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_emm_row_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->emm_table_destination_column_start && x <  table_source->emm_table_destination_column_start +  table_source->emm_table_destination_column_width) {
	}
	if(x >=  table_source->emm_table_weight_column_start && x <  table_source->emm_table_weight_column_start +  table_source->emm_table_weight_column_width) {
	pop_details_emm_row_t& emm_row = *((pop_details_emm_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN emm_row::content::weight::column_tooltip
	auto pt = state.world.pop_get_poptype(main.for_pop);
	auto modifier = state.world.pop_type_get_country_migration_target(pt);

	float ainterp_result = trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(emm_row.destination), trigger::to_generic(main.for_pop), 0);

	text::add_line(state, contents, "pop_emigration_attraction_1");
	text::add_line(state, contents, "pop_emigration_attraction_2", text::variable_type::x, text::fp_percentage{ state.world.nation_get_modifier_values(emm_row.destination, sys::national_mod_offsets::global_immigrant_attract) + 1.0f });
	ui::active_modifiers_description(state, contents, emm_row.destination, 15, sys::national_mod_offsets::global_immigrant_attract, false);
	text::add_line(state, contents, "pop_emigration_attraction_3", text::variable_type::x, text::fp_two_places{ ainterp_result });
	ui::multiplicative_value_modifier_description(state, contents, modifier, trigger::to_generic(emm_row.destination), trigger::to_generic(main.for_pop), 0);
// END
	}
}
void pop_details_emm_row_content_t::set_destination_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  destination_cached_text) {
		destination_cached_text = new_text;
		destination_internal_layout.contents.clear();
		destination_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ destination_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->emm_table_destination_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->emm_table_destination_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, destination_cached_text);
		}
	} else {
	}
}
void pop_details_emm_row_content_t::set_weight_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  weight_cached_text) {
		weight_cached_text = new_text;
		weight_internal_layout.contents.clear();
		weight_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->emm_table_weight_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->emm_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, weight_cached_text);
		}
	} else {
	}
}
void pop_details_emm_row_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_flag = rel_mouse_x >= table_source->emm_table_flag_column_start && rel_mouse_x < (table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_flag){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->emm_table_flag_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_flag){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start + table_source->emm_table_flag_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_flag){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y), float(table_source->emm_table_flag_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_flag_column_start), float(y + base_data.size.y - 2), float(table_source->emm_table_flag_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_destination = rel_mouse_x >= table_source->emm_table_destination_column_start && rel_mouse_x < (table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->emm_table_destination_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start + table_source->emm_table_destination_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y), float(table_source->emm_table_destination_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_destination_column_start), float(y + base_data.size.y - 2), float(table_source->emm_table_destination_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_destination = state.ui_templates.colors[destination_text_color]; 	if(!destination_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : destination_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->emm_table_destination_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_destination.r, col_color_destination.g, col_color_destination.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->emm_table_weight_column_start && rel_mouse_x < (table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->emm_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start + table_source->emm_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y), float(table_source->emm_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->emm_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->emm_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[weight_text_color]; 	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->emm_table_weight_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
}
void pop_details_emm_row_content_t::on_update(sys::state& state) noexcept {
	pop_details_emm_row_t& emm_row = *((pop_details_emm_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN emm_row::content::update
	set_destination_text(state, text::produce_simple_string(state, text::get_name(state, emm_row.destination)));
	auto pt = state.world.pop_get_poptype(main.for_pop);
	auto modifier = state.world.pop_type_get_country_migration_target(pt);

	float ainterp_result = std::max(trigger::evaluate_multiplicative_modifier(state, modifier, trigger::to_generic(emm_row.destination), trigger::to_generic(main.for_pop), 0) * (state.world.nation_get_modifier_values(emm_row.destination, sys::national_mod_offsets::global_immigrant_attract) + 1.0f), 0.0f);

	set_weight_text(state, text::format_float(ainterp_result, 2));
// END
}
void pop_details_emm_row_content_t::on_create(sys::state& state) noexcept {
// BEGIN emm_row::content::create
// END
}
ui::message_result pop_details_emm_row_dest_flag_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
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
ui::message_result pop_details_emm_row_dest_flag_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void pop_details_emm_row_dest_flag_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void pop_details_emm_row_dest_flag_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void pop_details_emm_row_dest_flag_t::on_update(sys::state& state) noexcept {
	pop_details_emm_row_t& emm_row = *((pop_details_emm_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN emm_row::dest_flag::update
	flag = emm_row.destination;
// END
}
void pop_details_emm_row_dest_flag_t::on_create(sys::state& state) noexcept {
// BEGIN emm_row::dest_flag::create
// END
}
void  pop_details_emm_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_emm_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_emm_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_emm_row_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN emm_row::update
// END
	remake_layout(state, true);
}
void pop_details_emm_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
				} else
				if(cname == "dest_flag") {
					temp.ptr = dest_flag.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_emm_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::emm_row"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_emm_row_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == "dest_flag") {
			dest_flag = std::make_unique<pop_details_emm_row_dest_flag_t>();
			dest_flag->parent = this;
			auto cptr = dest_flag.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == ".tabemm_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			emm_table_ascending_icon_key = main_section.read<std::string_view>();
			emm_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_flag_column_start = running_w_total;
			col_section.read(emm_table_flag_column_width);
			running_w_total += emm_table_flag_column_width;
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			emm_table_destination_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_destination_column_start = running_w_total;
			col_section.read(emm_table_destination_column_width);
			running_w_total += emm_table_destination_column_width;
			col_section.read(emm_table_destination_column_text_color);
			col_section.read(emm_table_destination_header_text_color);
			col_section.read(emm_table_destination_text_alignment);
			emm_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			emm_table_weight_column_start = running_w_total;
			col_section.read(emm_table_weight_column_width);
			running_w_total += emm_table_weight_column_width;
			col_section.read(emm_table_weight_column_text_color);
			col_section.read(emm_table_weight_header_text_color);
			col_section.read(emm_table_weight_text_alignment);
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
// BEGIN emm_row::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_emm_row(sys::state& state) {
	auto ptr = std::make_unique<pop_details_emm_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_mig_header_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->mig_table_destination_column_start && x < table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->mig_table_destination_sort_direction;
		table_source->mig_table_destination_sort_direction = 0;
		table_source->mig_table_weight_sort_direction = 0;
		table_source->mig_table_destination_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->mig_table_weight_column_start && x < table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->mig_table_weight_sort_direction;
		table_source->mig_table_destination_sort_direction = 0;
		table_source->mig_table_weight_sort_direction = 0;
		table_source->mig_table_weight_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result pop_details_mig_header_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_mig_header_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->mig_table_destination_column_start && x < table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width) {
	}
	if(x >= table_source->mig_table_weight_column_start && x < table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_mig_header_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->mig_table_destination_column_start && x <  table_source->mig_table_destination_column_start +  table_source->mig_table_destination_column_width) {
	}
	if(x >=  table_source->mig_table_weight_column_start && x <  table_source->mig_table_weight_column_start +  table_source->mig_table_weight_column_width) {
	}
}
void pop_details_mig_header_content_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	{
	destination_cached_text = text::produce_simple_string(state, table_source->mig_table_destination_header_text_key);
	 destination_internal_layout.contents.clear();
	 destination_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  destination_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->mig_table_destination_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->mig_table_destination_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, destination_cached_text);
	}
	{
	weight_cached_text = text::produce_simple_string(state, table_source->mig_table_weight_header_text_key);
	 weight_internal_layout.contents.clear();
	 weight_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->mig_table_weight_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->mig_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, weight_cached_text);
	}
}
void pop_details_mig_header_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_destination = rel_mouse_x >= table_source->mig_table_destination_column_start && rel_mouse_x < (table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_destination) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->mig_table_destination_column_start), float(y), float(table_source->mig_table_destination_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->mig_table_destination_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->mig_table_destination_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y), float(table_source->mig_table_destination_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y + base_data.size.y - 2), float(table_source->mig_table_destination_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_destination = state.ui_templates.colors[table_source->mig_table_destination_header_text_color]; 	if(table_source->mig_table_destination_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->mig_table_destination_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->mig_table_destination_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->mig_table_destination_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!destination_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : destination_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->mig_table_destination_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_destination.r, col_color_destination.g, col_color_destination.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->mig_table_weight_column_start && rel_mouse_x < (table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->mig_table_weight_column_start), float(y), float(table_source->mig_table_weight_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->mig_table_weight_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->mig_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y), float(table_source->mig_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->mig_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[table_source->mig_table_weight_header_text_color]; 	if(table_source->mig_table_weight_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->mig_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->mig_table_weight_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->mig_table_weight_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->mig_table_weight_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void pop_details_mig_header_content_t::on_update(sys::state& state) noexcept {
	pop_details_mig_header_t& mig_header = *((pop_details_mig_header_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN mig_header::content::update
// END
}
void pop_details_mig_header_content_t::on_create(sys::state& state) noexcept {
// BEGIN mig_header::content::create
// END
}
void  pop_details_mig_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_mig_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_mig_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_mig_header_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN mig_header::update
// END
	remake_layout(state, true);
}
void pop_details_mig_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_mig_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::mig_header"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_mig_header_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == ".tabmig_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			mig_table_ascending_icon_key = main_section.read<std::string_view>();
			mig_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			mig_table_destination_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			mig_table_destination_column_start = running_w_total;
			col_section.read(mig_table_destination_column_width);
			running_w_total += mig_table_destination_column_width;
			col_section.read(mig_table_destination_column_text_color);
			col_section.read(mig_table_destination_header_text_color);
			col_section.read(mig_table_destination_text_alignment);
			mig_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			mig_table_weight_column_start = running_w_total;
			col_section.read(mig_table_weight_column_width);
			running_w_total += mig_table_weight_column_width;
			col_section.read(mig_table_weight_column_text_color);
			col_section.read(mig_table_weight_header_text_color);
			col_section.read(mig_table_weight_text_alignment);
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
// BEGIN mig_header::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_mig_header(sys::state& state) {
	auto ptr = std::make_unique<pop_details_mig_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_mig_row_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_mig_row_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_mig_row_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >= table_source->mig_table_destination_column_start && x < table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width) {
	}
	if(x >= table_source->mig_table_weight_column_start && x < table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width) {
		ident = 1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->mig_table_weight_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->mig_table_weight_column_width);
		return;
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void pop_details_mig_row_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (pop_details_main_t*)(parent->parent);
	if(x >=  table_source->mig_table_destination_column_start && x <  table_source->mig_table_destination_column_start +  table_source->mig_table_destination_column_width) {
	}
	if(x >=  table_source->mig_table_weight_column_start && x <  table_source->mig_table_weight_column_start +  table_source->mig_table_weight_column_width) {
	pop_details_mig_row_t& mig_row = *((pop_details_mig_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN mig_row::content::weight::column_tooltip
	auto explanation = demographics::explain_province_internal_migration_weight(state, main.for_pop, mig_row.destination);
	auto pt = state.world.pop_get_poptype(main.for_pop);
	auto modifier = state.world.pop_type_get_migration_target(pt);

	text::add_line(state, contents, "pop_migration_attraction_1");
	text::add_line(state, contents, "pop_migration_attraction_2", text::variable_type::x, text::fp_percentage{ explanation.modifier });
	ui::active_modifiers_description(state, contents, mig_row.destination, 15, sys::provincial_mod_offsets::immigrant_attract, false);
	text::add_line(state, contents, "pop_migration_attraction_3", text::variable_type::x, text::fp_two_places{ explanation.base_multiplier });
	ui::multiplicative_value_modifier_description(state, contents, modifier, trigger::to_generic(mig_row.destination), trigger::to_generic(main.for_pop), 0);
	if(explanation.base_weight > 0.f) {
		text::add_line(state, contents, "pop_migration_attraction_bureaucracy", text::variable_type::x, text::fp_percentage{ explanation.base_weight });
	}
	text::add_line(state, contents, "pop_migration_attraction_wage_ratio", text::variable_type::x, text::fp_percentage{ explanation.wage_multiplier });
// END
	}
}
void pop_details_mig_row_content_t::set_destination_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  destination_cached_text) {
		destination_cached_text = new_text;
		destination_internal_layout.contents.clear();
		destination_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ destination_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->mig_table_destination_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->mig_table_destination_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, destination_cached_text);
		}
	} else {
	}
}
void pop_details_mig_row_content_t::set_weight_text(sys::state & state, std::string const& new_text) {
		auto table_source = (pop_details_main_t*)(parent->parent);
	if(new_text !=  weight_cached_text) {
		weight_cached_text = new_text;
		weight_internal_layout.contents.clear();
		weight_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ weight_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->mig_table_weight_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->mig_table_weight_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, weight_cached_text);
		}
	} else {
	}
}
void pop_details_mig_row_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (pop_details_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_destination = rel_mouse_x >= table_source->mig_table_destination_column_start && rel_mouse_x < (table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->mig_table_destination_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start + table_source->mig_table_destination_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_destination){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y), float(table_source->mig_table_destination_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_destination_column_start), float(y + base_data.size.y - 2), float(table_source->mig_table_destination_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_destination = state.ui_templates.colors[destination_text_color]; 	if(!destination_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : destination_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->mig_table_destination_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_destination.r, col_color_destination.g, col_color_destination.b }, ogl::color_modification::none);
		}
	}
	bool col_um_weight = rel_mouse_x >= table_source->mig_table_weight_column_start && rel_mouse_x < (table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->mig_table_weight_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start + table_source->mig_table_weight_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_weight){
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y), float(table_source->mig_table_weight_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->mig_table_weight_column_start), float(y + base_data.size.y - 2), float(table_source->mig_table_weight_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_weight = state.ui_templates.colors[weight_text_color]; 	if(!weight_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : weight_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->mig_table_weight_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_weight.r, col_color_weight.g, col_color_weight.b }, ogl::color_modification::none);
		}
	}
}
void pop_details_mig_row_content_t::on_update(sys::state& state) noexcept {
	pop_details_mig_row_t& mig_row = *((pop_details_mig_row_t*)(parent)); 
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN mig_row::content::update
	set_destination_text(state, text::produce_simple_string(state, state.world.province_get_name(mig_row.destination)));
	auto explanation = demographics::explain_province_internal_migration_weight(state, main.for_pop, mig_row.destination);
	set_weight_text(state, text::format_float(explanation.result, 2));
// END
}
void pop_details_mig_row_content_t::on_create(sys::state& state) noexcept {
// BEGIN mig_row::content::create
// END
}
void  pop_details_mig_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result pop_details_mig_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_mig_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_mig_row_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN mig_row::update
// END
	remake_layout(state, true);
}
void pop_details_mig_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
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
					temp.ptr = make_pop_details_main(state);
				}
				if(cname == "needs_row") {
					temp.ptr = make_pop_details_needs_row(state);
				}
				if(cname == "needs_header") {
					temp.ptr = make_pop_details_needs_header(state);
				}
				if(cname == "prom_header") {
					temp.ptr = make_pop_details_prom_header(state);
				}
				if(cname == "prom_row") {
					temp.ptr = make_pop_details_prom_row(state);
				}
				if(cname == "weights_header") {
					temp.ptr = make_pop_details_weights_header(state);
				}
				if(cname == "weights_row") {
					temp.ptr = make_pop_details_weights_row(state);
				}
				if(cname == "emm_header") {
					temp.ptr = make_pop_details_emm_header(state);
				}
				if(cname == "emm_row") {
					temp.ptr = make_pop_details_emm_row(state);
				}
				if(cname == "mig_header") {
					temp.ptr = make_pop_details_mig_header(state);
				}
				if(cname == "mig_row") {
					temp.ptr = make_pop_details_mig_row(state);
				}
				if(cname == "fancy_separator_1") {
					temp.ptr = make_pop_details_fancy_separator_1(state);
				}
				if(cname == "fancy_separator_2") {
					temp.ptr = make_pop_details_fancy_separator_2(state);
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
void pop_details_mig_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::mig_row"));
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
		if(child_data.name == "content") {
			content = std::make_unique<pop_details_mig_row_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
		if(child_data.name == ".tabmig_table") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			mig_table_ascending_icon_key = main_section.read<std::string_view>();
			mig_table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			mig_table_destination_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			mig_table_destination_column_start = running_w_total;
			col_section.read(mig_table_destination_column_width);
			running_w_total += mig_table_destination_column_width;
			col_section.read(mig_table_destination_column_text_color);
			col_section.read(mig_table_destination_header_text_color);
			col_section.read(mig_table_destination_text_alignment);
			mig_table_weight_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			mig_table_weight_column_start = running_w_total;
			col_section.read(mig_table_weight_column_width);
			running_w_total += mig_table_weight_column_width;
			col_section.read(mig_table_weight_column_text_color);
			col_section.read(mig_table_weight_header_text_color);
			col_section.read(mig_table_weight_text_alignment);
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
// BEGIN mig_row::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_mig_row(sys::state& state) {
	auto ptr = std::make_unique<pop_details_mig_row_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_fancy_separator_1_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_fancy_separator_1_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_fancy_separator_1_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN fancy_separator_1::update
// END
}
void pop_details_fancy_separator_1_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::fancy_separator_1"));
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
// BEGIN fancy_separator_1::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_fancy_separator_1(sys::state& state) {
	auto ptr = std::make_unique<pop_details_fancy_separator_1_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result pop_details_fancy_separator_2_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result pop_details_fancy_separator_2_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void pop_details_fancy_separator_2_t::on_update(sys::state& state) noexcept {
	pop_details_main_t& main = *((pop_details_main_t*)(parent->parent)); 
// BEGIN fancy_separator_2::update
// END
}
void pop_details_fancy_separator_2_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("pop_details::fancy_separator_2"));
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
// BEGIN fancy_separator_2::create
// END
}
std::unique_ptr<ui::element_base> make_pop_details_fancy_separator_2(sys::state& state) {
	auto ptr = std::make_unique<pop_details_fancy_separator_2_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
// BEGIN main::close_button::lbutton_action
//////////////////////////////////////	main.set_visible(state, false);
// END
}
