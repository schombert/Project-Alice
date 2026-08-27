namespace alice_ui {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif
struct demographicswindow_main_show_filters_t;
struct demographicswindow_main_reset_filters_t;
struct demographicswindow_main_only_pops_toggle_t;
struct demographicswindow_main_sort_pops_toggle_t;
struct demographicswindow_main_sort_states_toggle_t;
struct demographicswindow_main_t;
struct demographicswindow_nation_row_location_t;
struct demographicswindow_nation_row_size_t;
struct demographicswindow_nation_row_size_trend_t;
struct demographicswindow_nation_row_culture_t;
struct demographicswindow_nation_row_job_t;
struct demographicswindow_nation_row_religion_t;
struct demographicswindow_nation_row_ideology_t;
struct demographicswindow_nation_row_issues_t;
struct demographicswindow_nation_row_militancy_t;
struct demographicswindow_nation_row_consciousness_t;
struct demographicswindow_nation_row_employment_t;
struct demographicswindow_nation_row_literacy_t;
struct demographicswindow_nation_row_money_t;
struct demographicswindow_nation_row_needs_t;
struct demographicswindow_nation_row_frames_t;
struct demographicswindow_nation_row_t;
struct demographicswindow_state_row_folder_icon_t;
struct demographicswindow_state_row_location_t;
struct demographicswindow_state_row_size_t;
struct demographicswindow_state_row_size_trend_t;
struct demographicswindow_state_row_culture_t;
struct demographicswindow_state_row_job_t;
struct demographicswindow_state_row_religion_t;
struct demographicswindow_state_row_ideology_t;
struct demographicswindow_state_row_issues_t;
struct demographicswindow_state_row_militancy_t;
struct demographicswindow_state_row_consciousness_t;
struct demographicswindow_state_row_employment_t;
struct demographicswindow_state_row_literacy_t;
struct demographicswindow_state_row_money_t;
struct demographicswindow_state_row_needs_t;
struct demographicswindow_state_row_frames_t;
struct demographicswindow_state_row_focus_button_t;
struct demographicswindow_state_row_t;
struct demographicswindow_province_row_folder_icon_t;
struct demographicswindow_province_row_location_t;
struct demographicswindow_province_row_size_t;
struct demographicswindow_province_row_size_trend_t;
struct demographicswindow_province_row_culture_t;
struct demographicswindow_province_row_job_t;
struct demographicswindow_province_row_religion_t;
struct demographicswindow_province_row_ideology_t;
struct demographicswindow_province_row_issues_t;
struct demographicswindow_province_row_militancy_t;
struct demographicswindow_province_row_consciousness_t;
struct demographicswindow_province_row_employment_t;
struct demographicswindow_province_row_literacy_t;
struct demographicswindow_province_row_money_t;
struct demographicswindow_province_row_needs_t;
struct demographicswindow_province_row_frames_t;
struct demographicswindow_province_row_t;
struct demographicswindow_pop_row_location_t;
struct demographicswindow_pop_row_size_t;
struct demographicswindow_pop_row_size_trend_t;
struct demographicswindow_pop_row_culture_t;
struct demographicswindow_pop_row_job_t;
struct demographicswindow_pop_row_religion_t;
struct demographicswindow_pop_row_ideology_t;
struct demographicswindow_pop_row_issues_t;
struct demographicswindow_pop_row_militancy_t;
struct demographicswindow_pop_row_consciousness_t;
struct demographicswindow_pop_row_employment_t;
struct demographicswindow_pop_row_literacy_t;
struct demographicswindow_pop_row_money_t;
struct demographicswindow_pop_row_needs_t;
struct demographicswindow_pop_row_rebel_flag_t;
struct demographicswindow_pop_row_culture_name_t;
struct demographicswindow_pop_row_job_name_t;
struct demographicswindow_pop_row_religion_name_t;
struct demographicswindow_pop_row_frames_t;
struct demographicswindow_pop_row_movement_political_t;
struct demographicswindow_pop_row_movement_social_t;
struct demographicswindow_pop_row_details_button_t;
struct demographicswindow_pop_row_t;
struct demographicswindow_filters_window_job_all_t;
struct demographicswindow_filters_window_job_none_t;
struct demographicswindow_filters_window_culture_all_t;
struct demographicswindow_filters_window_culture_none_t;
struct demographicswindow_filters_window_religion_all_t;
struct demographicswindow_filters_window_religion_none_t;
struct demographicswindow_filters_window_location_all_t;
struct demographicswindow_filters_window_location_none_t;
struct demographicswindow_filters_window_non_colonial_t;
struct demographicswindow_filters_window_colonial_t;
struct demographicswindow_filters_window_t;
struct demographicswindow_job_filter_filter_button_t;
struct demographicswindow_job_filter_icon_t;
struct demographicswindow_job_filter_filter_label_t;
struct demographicswindow_job_filter_t;
struct demographicswindow_religion_filter_filter_button_t;
struct demographicswindow_religion_filter_icon_t;
struct demographicswindow_religion_filter_filter_label_t;
struct demographicswindow_religion_filter_t;
struct demographicswindow_location_filter_filter_button_t;
struct demographicswindow_location_filter_t;
struct demographicswindow_culture_filter_filter_button_t;
struct demographicswindow_culture_filter_t;
struct demographicswindow_pops_header_content_t;
struct demographicswindow_pops_header_mil_icon_t;
struct demographicswindow_pops_header_con_icon_t;
struct demographicswindow_pops_header_emp_icon_t;
struct demographicswindow_pops_header_lit_icon_t;
struct demographicswindow_pops_header_reb_icon_t;
struct demographicswindow_pops_header_cash_icon_t;
struct demographicswindow_pops_header_t;
struct demographicswindow_main_show_filters_t : public alice_ui::template_text_button {
// BEGIN main::show_filters::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_main_reset_filters_t : public alice_ui::template_text_button {
// BEGIN main::reset_filters::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_main_only_pops_toggle_t : public alice_ui::template_toggle_button {
// BEGIN main::only_pops_toggle::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_main_sort_pops_toggle_t : public alice_ui::template_toggle_button {
// BEGIN main::sort_pops_toggle::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_main_sort_states_toggle_t : public alice_ui::template_toggle_button {
// BEGIN main::sort_states_toggle::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_main_table_t : public layout_generator {
// BEGIN main::table::variables
// 
// END
	struct nation_row_option { dcon::nation_id content; };
	std::vector<std::unique_ptr<ui::element_base>> nation_row_pool;
	int32_t nation_row_pool_used = 0;
	void add_nation_row( dcon::nation_id content);
	struct state_row_option { dcon::state_instance_id content; };
	std::vector<std::unique_ptr<ui::element_base>> state_row_pool;
	int32_t state_row_pool_used = 0;
	void add_state_row( dcon::state_instance_id content);
	struct province_row_option { dcon::province_id content; };
	std::vector<std::unique_ptr<ui::element_base>> province_row_pool;
	int32_t province_row_pool_used = 0;
	void add_province_row( dcon::province_id content);
	struct pop_row_option { dcon::pop_id value; };
	std::vector<std::unique_ptr<ui::element_base>> pop_row_pool;
	int32_t pop_row_pool_used = 0;
	void add_pop_row( dcon::pop_id value);
	std::vector<std::unique_ptr<ui::element_base>> pops_header_pool;
	int32_t pops_header_pool_used = 0;
	std::vector<std::variant<std::monostate, nation_row_option, state_row_option, province_row_option, pop_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct demographicswindow_nation_row_location_t : public alice_ui::template_label {
// BEGIN nation_row::location::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_size_t : public alice_ui::template_label {
// BEGIN nation_row::size::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_size_trend_t : public ui::element_base {
// BEGIN nation_row::size_trend::variables
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
struct demographicswindow_nation_row_culture_t : public ui::element_base {
// BEGIN nation_row::culture::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {dcon::culture_id key; ogl::color3f color; float amount; };
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
struct demographicswindow_nation_row_job_t : public ui::element_base {
// BEGIN nation_row::job::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_nation_row_religion_t : public ui::element_base {
// BEGIN nation_row::religion::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {dcon::religion_id key; ogl::color3f color; float amount; };
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
struct demographicswindow_nation_row_ideology_t : public ui::element_base {
// BEGIN nation_row::ideology::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_nation_row_issues_t : public ui::element_base {
// BEGIN nation_row::issues::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_nation_row_militancy_t : public alice_ui::template_label {
// BEGIN nation_row::militancy::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_consciousness_t : public alice_ui::template_label {
// BEGIN nation_row::consciousness::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_employment_t : public alice_ui::template_label {
// BEGIN nation_row::employment::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_literacy_t : public alice_ui::template_label {
// BEGIN nation_row::literacy::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_money_t : public alice_ui::template_label {
// BEGIN nation_row::money::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_needs_t : public ui::element_base {
// BEGIN nation_row::needs::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 99, 3 };
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
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_nation_row_frames_t : public ui::element_base {
// BEGIN nation_row::frames::variables
// END
	int32_t template_id = -1;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_create(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_folder_icon_t : public alice_ui::template_icon_button {
// BEGIN state_row::folder_icon::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_location_t : public alice_ui::template_label {
// BEGIN state_row::location::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_size_t : public alice_ui::template_label {
// BEGIN state_row::size::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_size_trend_t : public ui::element_base {
// BEGIN state_row::size_trend::variables
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
struct demographicswindow_state_row_culture_t : public ui::element_base {
// BEGIN state_row::culture::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {dcon::culture_id key; ogl::color3f color; float amount; };
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
struct demographicswindow_state_row_job_t : public ui::element_base {
// BEGIN state_row::job::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_state_row_religion_t : public ui::element_base {
// BEGIN state_row::religion::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {dcon::religion_id key; ogl::color3f color; float amount; };
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
struct demographicswindow_state_row_ideology_t : public ui::element_base {
// BEGIN state_row::ideology::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_state_row_issues_t : public ui::element_base {
// BEGIN state_row::issues::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_state_row_militancy_t : public alice_ui::template_label {
// BEGIN state_row::militancy::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_consciousness_t : public alice_ui::template_label {
// BEGIN state_row::consciousness::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_employment_t : public alice_ui::template_label {
// BEGIN state_row::employment::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_literacy_t : public alice_ui::template_label {
// BEGIN state_row::literacy::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_money_t : public alice_ui::template_label {
// BEGIN state_row::money::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_needs_t : public ui::element_base {
// BEGIN state_row::needs::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 99, 3 };
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
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_frames_t : public ui::element_base {
// BEGIN state_row::frames::variables
// END
	int32_t template_id = -1;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_create(sys::state& state) noexcept override;
};
struct demographicswindow_state_row_focus_button_t : public ui::element_base {
// BEGIN state_row::focus_button::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
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
struct demographicswindow_province_row_folder_icon_t : public alice_ui::template_icon_button {
// BEGIN province_row::folder_icon::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_location_t : public alice_ui::template_label {
// BEGIN province_row::location::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_size_t : public alice_ui::template_label {
// BEGIN province_row::size::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_size_trend_t : public ui::element_base {
// BEGIN province_row::size_trend::variables
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
struct demographicswindow_province_row_culture_t : public ui::element_base {
// BEGIN province_row::culture::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {dcon::culture_id key; ogl::color3f color; float amount; };
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
struct demographicswindow_province_row_job_t : public ui::element_base {
// BEGIN province_row::job::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_province_row_religion_t : public ui::element_base {
// BEGIN province_row::religion::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
	struct graph_entry {dcon::religion_id key; ogl::color3f color; float amount; };
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
struct demographicswindow_province_row_ideology_t : public ui::element_base {
// BEGIN province_row::ideology::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_province_row_issues_t : public ui::element_base {
// BEGIN province_row::issues::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_province_row_militancy_t : public alice_ui::template_label {
// BEGIN province_row::militancy::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_consciousness_t : public alice_ui::template_label {
// BEGIN province_row::consciousness::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_employment_t : public alice_ui::template_label {
// BEGIN province_row::employment::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_literacy_t : public alice_ui::template_label {
// BEGIN province_row::literacy::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_money_t : public alice_ui::template_label {
// BEGIN province_row::money::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_needs_t : public ui::element_base {
// BEGIN province_row::needs::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 99, 3 };
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
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_province_row_frames_t : public ui::element_base {
// BEGIN province_row::frames::variables
// END
	int32_t template_id = -1;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_create(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_location_t : public alice_ui::template_label {
// BEGIN pop_row::location::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_size_t : public alice_ui::template_label {
// BEGIN pop_row::size::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_size_trend_t : public ui::element_base {
// BEGIN pop_row::size_trend::variables
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
struct demographicswindow_pop_row_culture_t : public ui::element_base {
// BEGIN pop_row::culture::variables
// END
	ogl::color4f color{ 1.000000f, 0.996078f, 1.000000f, 1.000000f };
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
struct demographicswindow_pop_row_job_t : public ui::element_base {
// BEGIN pop_row::job::variables
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
struct demographicswindow_pop_row_religion_t : public ui::element_base {
// BEGIN pop_row::religion::variables
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
struct demographicswindow_pop_row_ideology_t : public ui::element_base {
// BEGIN pop_row::ideology::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_pop_row_issues_t : public ui::element_base {
// BEGIN pop_row::issues::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 100, 3 };
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
struct demographicswindow_pop_row_militancy_t : public alice_ui::template_label {
// BEGIN pop_row::militancy::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_consciousness_t : public alice_ui::template_label {
// BEGIN pop_row::consciousness::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_employment_t : public alice_ui::template_label {
// BEGIN pop_row::employment::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_literacy_t : public alice_ui::template_label {
// BEGIN pop_row::literacy::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_money_t : public alice_ui::template_label {
// BEGIN pop_row::money::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_needs_t : public ui::element_base {
// BEGIN pop_row::needs::variables
// END
	int32_t template_id = -1;
	ogl::data_texture data_texture{ 99, 3 };
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
			return ui::message_result::consumed;
		} else {
			return ui::message_result::unseen;
		}
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_rebel_flag_t : public ui::element_base {
// BEGIN pop_row::rebel_flag::variables
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
struct demographicswindow_pop_row_culture_name_t : public alice_ui::template_label {
// BEGIN pop_row::culture_name::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_job_name_t : public alice_ui::template_label {
// BEGIN pop_row::job_name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_religion_name_t : public alice_ui::template_label {
// BEGIN pop_row::religion_name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_frames_t : public ui::element_base {
// BEGIN pop_row::frames::variables
// END
	int32_t template_id = -1;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_create(sys::state& state) noexcept override;
};
struct demographicswindow_pop_row_movement_political_t : public ui::element_base {
// BEGIN pop_row::movement_political::variables
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
struct demographicswindow_pop_row_movement_social_t : public ui::element_base {
// BEGIN pop_row::movement_social::variables
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
struct demographicswindow_pop_row_details_button_t : public alice_ui::template_icon_button {
// BEGIN pop_row::details_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_job_all_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::job_all::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_job_none_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::job_none::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_culture_all_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::culture_all::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_culture_none_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::culture_none::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_religion_all_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::religion_all::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_religion_none_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::religion_none::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_location_all_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::location_all::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_location_none_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::location_none::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_non_colonial_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::non_colonial::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_colonial_t : public alice_ui::template_mixed_button {
// BEGIN filters_window::colonial::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_filters_window_jobs_list_t : public layout_generator {
// BEGIN filters_window::jobs_list::variables
// END
	struct job_filter_option { dcon::pop_type_id value; };
	std::vector<std::unique_ptr<ui::element_base>> job_filter_pool;
	int32_t job_filter_pool_used = 0;
	void add_job_filter( dcon::pop_type_id value);
	std::vector<std::variant<std::monostate, job_filter_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct demographicswindow_filters_window_culture_list_t : public layout_generator {
// BEGIN filters_window::culture_list::variables
// END
	struct culture_filter_option { dcon::culture_id value; };
	std::vector<std::unique_ptr<ui::element_base>> culture_filter_pool;
	int32_t culture_filter_pool_used = 0;
	void add_culture_filter( dcon::culture_id value);
	std::vector<std::variant<std::monostate, culture_filter_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct demographicswindow_filters_window_religion_list_t : public layout_generator {
// BEGIN filters_window::religion_list::variables
// END
	struct religion_filter_option { dcon::religion_id value; };
	std::vector<std::unique_ptr<ui::element_base>> religion_filter_pool;
	int32_t religion_filter_pool_used = 0;
	void add_religion_filter( dcon::religion_id value);
	std::vector<std::variant<std::monostate, religion_filter_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct demographicswindow_filters_window_location_list_t : public layout_generator {
// BEGIN filters_window::location_list::variables
// END
	struct location_filter_option { dcon::state_instance_id value; };
	std::vector<std::unique_ptr<ui::element_base>> location_filter_pool;
	int32_t location_filter_pool_used = 0;
	void add_location_filter( dcon::state_instance_id value);
	std::vector<std::variant<std::monostate, location_filter_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct demographicswindow_job_filter_filter_button_t : public alice_ui::template_mixed_button {
// BEGIN job_filter::filter_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_job_filter_icon_t : public ui::element_base {
// BEGIN job_filter::icon::variables
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
struct demographicswindow_job_filter_filter_label_t : public alice_ui::template_label {
// BEGIN job_filter::filter_label::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_religion_filter_filter_button_t : public alice_ui::template_mixed_button {
// BEGIN religion_filter::filter_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_religion_filter_icon_t : public ui::element_base {
// BEGIN religion_filter::icon::variables
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
struct demographicswindow_religion_filter_filter_label_t : public alice_ui::template_label {
// BEGIN religion_filter::filter_label::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_location_filter_filter_button_t : public alice_ui::template_mixed_button {
// BEGIN location_filter::filter_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_culture_filter_filter_button_t : public alice_ui::template_mixed_button {
// BEGIN culture_filter::filter_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct demographicswindow_pops_header_content_t : public ui::element_base {
// BEGIN pops_header::content::variables
// END
	int32_t template_id = -1;
	text::layout location_internal_layout;
	std::string location_cached_text;
	text::layout size_internal_layout;
	std::string size_cached_text;
	text::layout culture_internal_layout;
	std::string culture_cached_text;
	text::layout job_internal_layout;
	std::string job_cached_text;
	text::layout religion_internal_layout;
	std::string religion_cached_text;
	text::layout ideology_internal_layout;
	std::string ideology_cached_text;
	text::layout issues_internal_layout;
	std::string issues_cached_text;
	text::layout needs_internal_layout;
	std::string needs_cached_text;
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
struct demographicswindow_pops_header_mil_icon_t : public ui::element_base {
// BEGIN pops_header::mil_icon::variables
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
struct demographicswindow_pops_header_con_icon_t : public ui::element_base {
// BEGIN pops_header::con_icon::variables
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
struct demographicswindow_pops_header_emp_icon_t : public ui::element_base {
// BEGIN pops_header::emp_icon::variables
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
struct demographicswindow_pops_header_lit_icon_t : public ui::element_base {
// BEGIN pops_header::lit_icon::variables
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
struct demographicswindow_pops_header_reb_icon_t : public ui::element_base {
// BEGIN pops_header::reb_icon::variables
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
struct demographicswindow_pops_header_cash_icon_t : public ui::element_base {
// BEGIN pops_header::cash_icon::variables
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
struct demographicswindow_main_t : public layout_window_element {
// BEGIN main::variables
// END
	ui::element_base* filter_w;
	ui::element_base* details_w;
	ui::element_base* focus_w;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> title;
	std::unique_ptr<demographicswindow_main_show_filters_t> show_filters;
	std::unique_ptr<demographicswindow_main_reset_filters_t> reset_filters;
	std::unique_ptr<demographicswindow_main_only_pops_toggle_t> only_pops_toggle;
	std::unique_ptr<demographicswindow_main_sort_pops_toggle_t> sort_pops_toggle;
	std::unique_ptr<demographicswindow_main_sort_states_toggle_t> sort_states_toggle;
	demographicswindow_main_table_t table;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	int16_t table_spacer_column_start = 0;
	int16_t table_spacer_column_width = 0;
	std::string_view table_location_header_text_key;
	uint8_t table_location_header_text_color = 0;
	uint8_t table_location_column_text_color = 0;
	text::alignment table_location_text_alignment = text::alignment::center;
	int8_t table_location_sort_direction = 0;
	int16_t table_location_column_start = 0;
	int16_t table_location_column_width = 0;
	std::string_view table_size_header_text_key;
	uint8_t table_size_header_text_color = 0;
	uint8_t table_size_column_text_color = 0;
	text::alignment table_size_text_alignment = text::alignment::center;
	int8_t table_size_sort_direction = 0;
	int16_t table_size_column_start = 0;
	int16_t table_size_column_width = 0;
	std::string_view table_culture_header_text_key;
	uint8_t table_culture_header_text_color = 0;
	uint8_t table_culture_column_text_color = 0;
	text::alignment table_culture_text_alignment = text::alignment::center;
	int8_t table_culture_sort_direction = 0;
	int16_t table_culture_column_start = 0;
	int16_t table_culture_column_width = 0;
	std::string_view table_job_header_text_key;
	uint8_t table_job_header_text_color = 0;
	uint8_t table_job_column_text_color = 0;
	text::alignment table_job_text_alignment = text::alignment::center;
	int8_t table_job_sort_direction = 0;
	int16_t table_job_column_start = 0;
	int16_t table_job_column_width = 0;
	std::string_view table_religion_header_text_key;
	uint8_t table_religion_header_text_color = 0;
	uint8_t table_religion_column_text_color = 0;
	text::alignment table_religion_text_alignment = text::alignment::center;
	int8_t table_religion_sort_direction = 0;
	int16_t table_religion_column_start = 0;
	int16_t table_religion_column_width = 0;
	std::string_view table_ideology_header_text_key;
	uint8_t table_ideology_header_text_color = 0;
	uint8_t table_ideology_column_text_color = 0;
	text::alignment table_ideology_text_alignment = text::alignment::center;
	int16_t table_ideology_column_start = 0;
	int16_t table_ideology_column_width = 0;
	std::string_view table_issues_header_text_key;
	uint8_t table_issues_header_text_color = 0;
	uint8_t table_issues_column_text_color = 0;
	text::alignment table_issues_text_alignment = text::alignment::center;
	int16_t table_issues_column_start = 0;
	int16_t table_issues_column_width = 0;
	std::string_view table_militancy_header_tooltip_key;
	uint8_t table_militancy_header_text_color = 0;
	uint8_t table_militancy_column_text_color = 0;
	text::alignment table_militancy_text_alignment = text::alignment::center;
	int8_t table_militancy_sort_direction = 0;
	int16_t table_militancy_column_start = 0;
	int16_t table_militancy_column_width = 0;
	std::string_view table_consciousness_header_tooltip_key;
	uint8_t table_consciousness_header_text_color = 0;
	uint8_t table_consciousness_column_text_color = 0;
	text::alignment table_consciousness_text_alignment = text::alignment::center;
	int8_t table_consciousness_sort_direction = 0;
	int16_t table_consciousness_column_start = 0;
	int16_t table_consciousness_column_width = 0;
	std::string_view table_employment_header_tooltip_key;
	uint8_t table_employment_header_text_color = 0;
	uint8_t table_employment_column_text_color = 0;
	text::alignment table_employment_text_alignment = text::alignment::center;
	int8_t table_employment_sort_direction = 0;
	int16_t table_employment_column_start = 0;
	int16_t table_employment_column_width = 0;
	std::string_view table_literacy_header_tooltip_key;
	uint8_t table_literacy_header_text_color = 0;
	uint8_t table_literacy_column_text_color = 0;
	text::alignment table_literacy_text_alignment = text::alignment::center;
	int8_t table_literacy_sort_direction = 0;
	int16_t table_literacy_column_start = 0;
	int16_t table_literacy_column_width = 0;
	std::string_view table_money_header_tooltip_key;
	uint8_t table_money_header_text_color = 0;
	uint8_t table_money_column_text_color = 0;
	text::alignment table_money_text_alignment = text::alignment::center;
	int8_t table_money_sort_direction = 0;
	int16_t table_money_column_start = 0;
	int16_t table_money_column_width = 0;
	std::string_view table_needs_header_text_key;
	uint8_t table_needs_header_text_color = 0;
	uint8_t table_needs_column_text_color = 0;
	text::alignment table_needs_text_alignment = text::alignment::center;
	int8_t table_needs_sort_direction = 0;
	int16_t table_needs_column_start = 0;
	int16_t table_needs_column_width = 0;
	std::string_view table_rebels_header_tooltip_key;
	uint8_t table_rebels_header_text_color = 0;
	uint8_t table_rebels_column_text_color = 0;
	text::alignment table_rebels_text_alignment = text::alignment::center;
	int16_t table_rebels_column_start = 0;
	int16_t table_rebels_column_width = 0;
	std::string_view table_ascending_icon_key;
	dcon::texture_id table_ascending_icon;
	std::string_view table_descending_icon_key;
	dcon::texture_id table_descending_icon;
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
		if(name_parameter == "filter_w") {
			return (void*)(&filter_w);
		}
		if(name_parameter == "details_w") {
			return (void*)(&details_w);
		}
		if(name_parameter == "focus_w") {
			return (void*)(&focus_w);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_demographicswindow_main(sys::state& state);
struct demographicswindow_nation_row_t : public layout_window_element {
// BEGIN nation_row::variables
// END
	dcon::nation_id content;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_nation_row_location_t> location;
	std::unique_ptr<demographicswindow_nation_row_size_t> size;
	std::unique_ptr<demographicswindow_nation_row_size_trend_t> size_trend;
	std::unique_ptr<demographicswindow_nation_row_culture_t> culture;
	std::unique_ptr<demographicswindow_nation_row_job_t> job;
	std::unique_ptr<demographicswindow_nation_row_religion_t> religion;
	std::unique_ptr<demographicswindow_nation_row_ideology_t> ideology;
	std::unique_ptr<demographicswindow_nation_row_issues_t> issues;
	std::unique_ptr<demographicswindow_nation_row_militancy_t> militancy;
	std::unique_ptr<demographicswindow_nation_row_consciousness_t> consciousness;
	std::unique_ptr<demographicswindow_nation_row_employment_t> employment;
	std::unique_ptr<demographicswindow_nation_row_literacy_t> literacy;
	std::unique_ptr<demographicswindow_nation_row_money_t> money;
	std::unique_ptr<demographicswindow_nation_row_needs_t> needs;
	std::unique_ptr<demographicswindow_nation_row_frames_t> frames;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "content") {
			return (void*)(&content);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_demographicswindow_nation_row(sys::state& state);
struct demographicswindow_state_row_t : public layout_window_element {
// BEGIN state_row::variables
// END
	dcon::state_instance_id content;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_state_row_folder_icon_t> folder_icon;
	std::unique_ptr<demographicswindow_state_row_location_t> location;
	std::unique_ptr<demographicswindow_state_row_size_t> size;
	std::unique_ptr<demographicswindow_state_row_size_trend_t> size_trend;
	std::unique_ptr<demographicswindow_state_row_culture_t> culture;
	std::unique_ptr<demographicswindow_state_row_job_t> job;
	std::unique_ptr<demographicswindow_state_row_religion_t> religion;
	std::unique_ptr<demographicswindow_state_row_ideology_t> ideology;
	std::unique_ptr<demographicswindow_state_row_issues_t> issues;
	std::unique_ptr<demographicswindow_state_row_militancy_t> militancy;
	std::unique_ptr<demographicswindow_state_row_consciousness_t> consciousness;
	std::unique_ptr<demographicswindow_state_row_employment_t> employment;
	std::unique_ptr<demographicswindow_state_row_literacy_t> literacy;
	std::unique_ptr<demographicswindow_state_row_money_t> money;
	std::unique_ptr<demographicswindow_state_row_needs_t> needs;
	std::unique_ptr<demographicswindow_state_row_frames_t> frames;
	std::unique_ptr<demographicswindow_state_row_focus_button_t> focus_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "content") {
			return (void*)(&content);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_demographicswindow_state_row(sys::state& state);
struct demographicswindow_province_row_t : public layout_window_element {
// BEGIN province_row::variables
// END
	dcon::province_id content;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_province_row_folder_icon_t> folder_icon;
	std::unique_ptr<demographicswindow_province_row_location_t> location;
	std::unique_ptr<demographicswindow_province_row_size_t> size;
	std::unique_ptr<demographicswindow_province_row_size_trend_t> size_trend;
	std::unique_ptr<demographicswindow_province_row_culture_t> culture;
	std::unique_ptr<demographicswindow_province_row_job_t> job;
	std::unique_ptr<demographicswindow_province_row_religion_t> religion;
	std::unique_ptr<demographicswindow_province_row_ideology_t> ideology;
	std::unique_ptr<demographicswindow_province_row_issues_t> issues;
	std::unique_ptr<demographicswindow_province_row_militancy_t> militancy;
	std::unique_ptr<demographicswindow_province_row_consciousness_t> consciousness;
	std::unique_ptr<demographicswindow_province_row_employment_t> employment;
	std::unique_ptr<demographicswindow_province_row_literacy_t> literacy;
	std::unique_ptr<demographicswindow_province_row_money_t> money;
	std::unique_ptr<demographicswindow_province_row_needs_t> needs;
	std::unique_ptr<demographicswindow_province_row_frames_t> frames;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "content") {
			return (void*)(&content);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_demographicswindow_province_row(sys::state& state);
struct demographicswindow_pop_row_t : public layout_window_element {
// BEGIN pop_row::variables
// END
	dcon::pop_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_pop_row_location_t> location;
	std::unique_ptr<demographicswindow_pop_row_size_t> size;
	std::unique_ptr<demographicswindow_pop_row_size_trend_t> size_trend;
	std::unique_ptr<demographicswindow_pop_row_culture_t> culture;
	std::unique_ptr<demographicswindow_pop_row_job_t> job;
	std::unique_ptr<demographicswindow_pop_row_religion_t> religion;
	std::unique_ptr<demographicswindow_pop_row_ideology_t> ideology;
	std::unique_ptr<demographicswindow_pop_row_issues_t> issues;
	std::unique_ptr<demographicswindow_pop_row_militancy_t> militancy;
	std::unique_ptr<demographicswindow_pop_row_consciousness_t> consciousness;
	std::unique_ptr<demographicswindow_pop_row_employment_t> employment;
	std::unique_ptr<demographicswindow_pop_row_literacy_t> literacy;
	std::unique_ptr<demographicswindow_pop_row_money_t> money;
	std::unique_ptr<demographicswindow_pop_row_needs_t> needs;
	std::unique_ptr<demographicswindow_pop_row_rebel_flag_t> rebel_flag;
	std::unique_ptr<demographicswindow_pop_row_culture_name_t> culture_name;
	std::unique_ptr<demographicswindow_pop_row_job_name_t> job_name;
	std::unique_ptr<demographicswindow_pop_row_religion_name_t> religion_name;
	std::unique_ptr<demographicswindow_pop_row_frames_t> frames;
	std::unique_ptr<demographicswindow_pop_row_movement_political_t> movement_political;
	std::unique_ptr<demographicswindow_pop_row_movement_social_t> movement_social;
	std::unique_ptr<demographicswindow_pop_row_details_button_t> details_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_demographicswindow_pop_row(sys::state& state);
struct demographicswindow_filters_window_t : public layout_window_element {
// BEGIN filters_window::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_filters_window_job_all_t> job_all;
	std::unique_ptr<demographicswindow_filters_window_job_none_t> job_none;
	std::unique_ptr<demographicswindow_filters_window_culture_all_t> culture_all;
	std::unique_ptr<demographicswindow_filters_window_culture_none_t> culture_none;
	std::unique_ptr<demographicswindow_filters_window_religion_all_t> religion_all;
	std::unique_ptr<demographicswindow_filters_window_religion_none_t> religion_none;
	std::unique_ptr<demographicswindow_filters_window_location_all_t> location_all;
	std::unique_ptr<demographicswindow_filters_window_location_none_t> location_none;
	std::unique_ptr<demographicswindow_filters_window_non_colonial_t> non_colonial;
	std::unique_ptr<demographicswindow_filters_window_colonial_t> colonial;
	std::unique_ptr<template_label> job_header;
	std::unique_ptr<template_label> culture_header;
	std::unique_ptr<template_label> religion_header;
	std::unique_ptr<template_label> location_header;
	demographicswindow_filters_window_jobs_list_t jobs_list;
	demographicswindow_filters_window_culture_list_t culture_list;
	demographicswindow_filters_window_religion_list_t religion_list;
	demographicswindow_filters_window_location_list_t location_list;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
};
std::unique_ptr<ui::element_base> make_demographicswindow_filters_window(sys::state& state);
struct demographicswindow_job_filter_t : public layout_window_element {
// BEGIN job_filter::variables
// END
	dcon::pop_type_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_job_filter_filter_button_t> filter_button;
	std::unique_ptr<demographicswindow_job_filter_icon_t> icon;
	std::unique_ptr<demographicswindow_job_filter_filter_label_t> filter_label;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_demographicswindow_job_filter(sys::state& state);
struct demographicswindow_religion_filter_t : public layout_window_element {
// BEGIN religion_filter::variables
// END
	dcon::religion_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_religion_filter_filter_button_t> filter_button;
	std::unique_ptr<demographicswindow_religion_filter_icon_t> icon;
	std::unique_ptr<demographicswindow_religion_filter_filter_label_t> filter_label;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_demographicswindow_religion_filter(sys::state& state);
struct demographicswindow_location_filter_t : public layout_window_element {
// BEGIN location_filter::variables
// END
	dcon::state_instance_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_location_filter_filter_button_t> filter_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_demographicswindow_location_filter(sys::state& state);
struct demographicswindow_culture_filter_t : public layout_window_element {
// BEGIN culture_filter::variables
// END
	dcon::culture_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_culture_filter_filter_button_t> filter_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_demographicswindow_culture_filter(sys::state& state);
struct demographicswindow_pops_header_t : public layout_window_element {
// BEGIN pops_header::variables
// END
	dcon::pop_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<demographicswindow_pops_header_content_t> content;
	std::unique_ptr<demographicswindow_pops_header_mil_icon_t> mil_icon;
	std::unique_ptr<demographicswindow_pops_header_con_icon_t> con_icon;
	std::unique_ptr<demographicswindow_pops_header_emp_icon_t> emp_icon;
	std::unique_ptr<demographicswindow_pops_header_lit_icon_t> lit_icon;
	std::unique_ptr<demographicswindow_pops_header_reb_icon_t> reb_icon;
	std::unique_ptr<demographicswindow_pops_header_cash_icon_t> cash_icon;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
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
std::unique_ptr<ui::element_base> make_demographicswindow_pops_header(sys::state& state);
void demographicswindow_main_table_t::add_nation_row(dcon::nation_id content) {
	values.emplace_back(nation_row_option{content});
}
void demographicswindow_main_table_t::add_state_row(dcon::state_instance_id content) {
	values.emplace_back(state_row_option{content});
}
void demographicswindow_main_table_t::add_province_row(dcon::province_id content) {
	values.emplace_back(province_row_option{content});
}
void demographicswindow_main_table_t::add_pop_row(dcon::pop_id value) {
	values.emplace_back(pop_row_option{value});
}
void  demographicswindow_main_table_t::on_create(sys::state& state, layout_window_element* parent) {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::table::on_create
// END
}
void  demographicswindow_main_table_t::update(sys::state& state, layout_window_element* parent) {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::table::update
	values.clear();
	if(!popwindow::show_only_pops) {
		add_nation_row(state.local_player_nation);

		std::vector<dcon::state_instance_id> si;
		for(auto so : state.world.nation_get_state_ownership(state.local_player_nation)) {
			si.push_back(so.get_state().id);
		}
		if(popwindow::sort_states) {
			pop_screen_sort_state_rows(state, si, parent);
		}
		for(auto s : si) {
			bool added_header = false;
			bool state_is_open = popwindow::open_states.contains(s.index());

			std::vector<dcon::province_id> provs;
			province::for_each_province_in_state_instance(state, s, [&](dcon::province_id p) { provs.push_back(p); });

			sys::merge_sort(provs.begin(), provs.end(), [&](dcon::province_id a, dcon::province_id b) {
				return text::produce_simple_string(state, state.world.province_get_name(a)) < text::produce_simple_string(state, state.world.province_get_name(b));
			});

			for(auto p : provs) {
				bool added_pop = false;
				bool province_is_open = popwindow::open_provs.contains(p.index());

				for(auto po : state.world.province_get_pop_location(p)) {
					if(pop_passes_filter(state, po.get_pop())) {
						if(!added_pop) {
							if(!added_header) {
								added_header = true;
								add_state_row(s);
							}
							if(state_is_open) {
								add_province_row(p);
							}
							added_pop = true;
						}
						if(state_is_open && province_is_open) {
							add_pop_row(po.get_pop().id);
						}
					}
				}
			}
		}
	} else {
		add_nation_row(state.local_player_nation);
		for(auto p : state.world.nation_get_province_ownership(state.local_player_nation)) {
			for(auto po : p.get_province().get_pop_location()) {
				if(pop_passes_filter(state, po.get_pop())) {
					add_pop_row(po.get_pop().id);
				}
			}
		}
	}
	if(!popwindow::sort_pops) {
		return;
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (demographicswindow_main_t*)(parent);
	if(table_source->table_location_sort_direction != 0) work_to_do = true;
	if(table_source->table_size_sort_direction != 0) work_to_do = true;
	if(table_source->table_culture_sort_direction != 0) work_to_do = true;
	if(table_source->table_job_sort_direction != 0) work_to_do = true;
	if(table_source->table_religion_sort_direction != 0) work_to_do = true;
	if(table_source->table_militancy_sort_direction != 0) work_to_do = true;
	if(table_source->table_consciousness_sort_direction != 0) work_to_do = true;
	if(table_source->table_employment_sort_direction != 0) work_to_do = true;
	if(table_source->table_literacy_sort_direction != 0) work_to_do = true;
	if(table_source->table_money_sort_direction != 0) work_to_do = true;
	if(table_source->table_needs_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<pop_row_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<pop_row_option>(values[i])) ++i;
				if(table_source->table_location_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::location
						result = cmp3(text::produce_simple_string(state, state.world.province_get_name(state.world.pop_get_province_from_pop_location(a.value))),
							text::produce_simple_string(state, state.world.province_get_name(state.world.pop_get_province_from_pop_location(b.value))));
// END
						return -result == table_source->table_location_sort_direction;
					});
				}
				if(table_source->table_size_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::size
						result = cmp3(state.world.pop_get_size(a.value), state.world.pop_get_size(b.value));
// END
						return -result == table_source->table_size_sort_direction;
					});
				}
				if(table_source->table_culture_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::culture
						result = cmp3(text::produce_simple_string(state, state.world.culture_get_name(state.world.pop_get_culture(a.value))),
							text::produce_simple_string(state, state.world.culture_get_name(state.world.pop_get_culture(b.value))));
// END
						return -result == table_source->table_culture_sort_direction;
					});
				}
				if(table_source->table_job_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::job
						result = cmp3(text::produce_simple_string(state, state.world.pop_type_get_name(state.world.pop_get_poptype(a.value))),
							text::produce_simple_string(state, state.world.pop_type_get_name(state.world.pop_get_poptype(b.value))));
// END
						return -result == table_source->table_job_sort_direction;
					});
				}
				if(table_source->table_religion_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::religion
						result = cmp3(text::produce_simple_string(state, state.world.religion_get_name(state.world.pop_get_religion(a.value))),
							text::produce_simple_string(state, state.world.religion_get_name(state.world.pop_get_religion(b.value))));
// END
						return -result == table_source->table_religion_sort_direction;
					});
				}
				if(table_source->table_militancy_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::militancy
						result = cmp3(pop_demographics::get_militancy(state, a.value), pop_demographics::get_militancy(state, b.value));
// END
						return -result == table_source->table_militancy_sort_direction;
					});
				}
				if(table_source->table_consciousness_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::consciousness
						result = cmp3(pop_demographics::get_consciousness(state, a.value), pop_demographics::get_consciousness(state, b.value));
// END
						return -result == table_source->table_consciousness_sort_direction;
					});
				}
				if(table_source->table_employment_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::employment
						result = cmp3(pop_demographics::get_employment(state, a.value), pop_demographics::get_employment(state, b.value));
// END
						return -result == table_source->table_employment_sort_direction;
					});
				}
				if(table_source->table_literacy_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::literacy
						result = cmp3(pop_demographics::get_literacy(state, a.value), pop_demographics::get_literacy(state, b.value));
// END
						return -result == table_source->table_literacy_sort_direction;
					});
				}
				if(table_source->table_money_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::money
						result = cmp3(state.world.pop_get_savings(a.value), state.world.pop_get_savings(b.value));
// END
						return -result == table_source->table_money_sort_direction;
					});
				}
				if(table_source->table_needs_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<pop_row_option>(raw_a);
						auto const& b = std::get<pop_row_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table::table::sort::needs
						result = cmp3(pop_demographics::get_life_needs(state, a.value) + pop_demographics::get_everyday_needs(state, a.value) + pop_demographics::get_luxury_needs(state, a.value), pop_demographics::get_life_needs(state, b.value) + pop_demographics::get_everyday_needs(state, b.value) + pop_demographics::get_luxury_needs(state,b.value));
// END
						return -result == table_source->table_needs_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  demographicswindow_main_table_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<nation_row_option>(values[index])) {
		if(pops_header_pool.empty()) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
		if(nation_row_pool.empty()) nation_row_pool.emplace_back(make_demographicswindow_nation_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<nation_row_option>(values[index - 1]) && !std::holds_alternative<state_row_option>(values[index - 1]) && !std::holds_alternative<province_row_option>(values[index - 1]) && !std::holds_alternative<pop_row_option>(values[index - 1]))) {
			if(destination) {
				if(pops_header_pool.size() <= size_t(pops_header_pool_used)) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
				if(nation_row_pool.size() <= size_t(nation_row_pool_used)) nation_row_pool.emplace_back(make_demographicswindow_nation_row(state));
				pops_header_pool[pops_header_pool_used]->base_data.position.x = int16_t(x);
				pops_header_pool[pops_header_pool_used]->base_data.position.y = int16_t(y);
				if(!pops_header_pool[pops_header_pool_used]->parent) {
					pops_header_pool[pops_header_pool_used]->parent = destination;
					pops_header_pool[pops_header_pool_used]->impl_on_update(state);
					pops_header_pool[pops_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(pops_header_pool[pops_header_pool_used].get());
			((demographicswindow_pops_header_t*)(pops_header_pool[pops_header_pool_used].get()))->set_alternate(alternate);
				nation_row_pool[nation_row_pool_used]->base_data.position.x = int16_t(x);
				nation_row_pool[nation_row_pool_used]->base_data.position.y = int16_t(y +  nation_row_pool[0]->base_data.size.y + 0);
				nation_row_pool[nation_row_pool_used]->parent = destination;
				destination->children.push_back(nation_row_pool[nation_row_pool_used].get());
				((demographicswindow_nation_row_t*)(nation_row_pool[nation_row_pool_used].get()))->content = std::get<nation_row_option>(values[index]).content;
			((demographicswindow_nation_row_t*)(nation_row_pool[nation_row_pool_used].get()))->set_alternate(!alternate);
				nation_row_pool[nation_row_pool_used]->impl_on_update(state);
				pops_header_pool_used++;
				nation_row_pool_used++;
			}
			return measure_result{std::max(pops_header_pool[0]->base_data.size.x, nation_row_pool[0]->base_data.size.x), pops_header_pool[0]->base_data.size.y + nation_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(nation_row_pool.size() <= size_t(nation_row_pool_used)) nation_row_pool.emplace_back(make_demographicswindow_nation_row(state));
			nation_row_pool[nation_row_pool_used]->base_data.position.x = int16_t(x);
			nation_row_pool[nation_row_pool_used]->base_data.position.y = int16_t(y);
			nation_row_pool[nation_row_pool_used]->parent = destination;
			destination->children.push_back(nation_row_pool[nation_row_pool_used].get());
			((demographicswindow_nation_row_t*)(nation_row_pool[nation_row_pool_used].get()))->content = std::get<nation_row_option>(values[index]).content;
			((demographicswindow_nation_row_t*)(nation_row_pool[nation_row_pool_used].get()))->set_alternate(alternate);
			nation_row_pool[nation_row_pool_used]->impl_on_update(state);
			nation_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ nation_row_pool[0]->base_data.size.x, nation_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<state_row_option>(values[index])) {
		if(pops_header_pool.empty()) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
		if(state_row_pool.empty()) state_row_pool.emplace_back(make_demographicswindow_state_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<nation_row_option>(values[index - 1]) && !std::holds_alternative<state_row_option>(values[index - 1]) && !std::holds_alternative<province_row_option>(values[index - 1]) && !std::holds_alternative<pop_row_option>(values[index - 1]))) {
			if(destination) {
				if(pops_header_pool.size() <= size_t(pops_header_pool_used)) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
				if(state_row_pool.size() <= size_t(state_row_pool_used)) state_row_pool.emplace_back(make_demographicswindow_state_row(state));
				pops_header_pool[pops_header_pool_used]->base_data.position.x = int16_t(x);
				pops_header_pool[pops_header_pool_used]->base_data.position.y = int16_t(y);
				if(!pops_header_pool[pops_header_pool_used]->parent) {
					pops_header_pool[pops_header_pool_used]->parent = destination;
					pops_header_pool[pops_header_pool_used]->impl_on_update(state);
					pops_header_pool[pops_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(pops_header_pool[pops_header_pool_used].get());
			((demographicswindow_pops_header_t*)(pops_header_pool[pops_header_pool_used].get()))->set_alternate(alternate);
				state_row_pool[state_row_pool_used]->base_data.position.x = int16_t(x);
				state_row_pool[state_row_pool_used]->base_data.position.y = int16_t(y +  state_row_pool[0]->base_data.size.y + 0);
				state_row_pool[state_row_pool_used]->parent = destination;
				destination->children.push_back(state_row_pool[state_row_pool_used].get());
				((demographicswindow_state_row_t*)(state_row_pool[state_row_pool_used].get()))->content = std::get<state_row_option>(values[index]).content;
			((demographicswindow_state_row_t*)(state_row_pool[state_row_pool_used].get()))->set_alternate(!alternate);
				state_row_pool[state_row_pool_used]->impl_on_update(state);
				pops_header_pool_used++;
				state_row_pool_used++;
			}
			return measure_result{std::max(pops_header_pool[0]->base_data.size.x, state_row_pool[0]->base_data.size.x), pops_header_pool[0]->base_data.size.y + state_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(state_row_pool.size() <= size_t(state_row_pool_used)) state_row_pool.emplace_back(make_demographicswindow_state_row(state));
			state_row_pool[state_row_pool_used]->base_data.position.x = int16_t(x);
			state_row_pool[state_row_pool_used]->base_data.position.y = int16_t(y);
			state_row_pool[state_row_pool_used]->parent = destination;
			destination->children.push_back(state_row_pool[state_row_pool_used].get());
			((demographicswindow_state_row_t*)(state_row_pool[state_row_pool_used].get()))->content = std::get<state_row_option>(values[index]).content;
			((demographicswindow_state_row_t*)(state_row_pool[state_row_pool_used].get()))->set_alternate(alternate);
			state_row_pool[state_row_pool_used]->impl_on_update(state);
			state_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ state_row_pool[0]->base_data.size.x, state_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<province_row_option>(values[index])) {
		if(pops_header_pool.empty()) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
		if(province_row_pool.empty()) province_row_pool.emplace_back(make_demographicswindow_province_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<nation_row_option>(values[index - 1]) && !std::holds_alternative<state_row_option>(values[index - 1]) && !std::holds_alternative<province_row_option>(values[index - 1]) && !std::holds_alternative<pop_row_option>(values[index - 1]))) {
			if(destination) {
				if(pops_header_pool.size() <= size_t(pops_header_pool_used)) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
				if(province_row_pool.size() <= size_t(province_row_pool_used)) province_row_pool.emplace_back(make_demographicswindow_province_row(state));
				pops_header_pool[pops_header_pool_used]->base_data.position.x = int16_t(x);
				pops_header_pool[pops_header_pool_used]->base_data.position.y = int16_t(y);
				if(!pops_header_pool[pops_header_pool_used]->parent) {
					pops_header_pool[pops_header_pool_used]->parent = destination;
					pops_header_pool[pops_header_pool_used]->impl_on_update(state);
					pops_header_pool[pops_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(pops_header_pool[pops_header_pool_used].get());
			((demographicswindow_pops_header_t*)(pops_header_pool[pops_header_pool_used].get()))->set_alternate(alternate);
				province_row_pool[province_row_pool_used]->base_data.position.x = int16_t(x);
				province_row_pool[province_row_pool_used]->base_data.position.y = int16_t(y +  province_row_pool[0]->base_data.size.y + 0);
				province_row_pool[province_row_pool_used]->parent = destination;
				destination->children.push_back(province_row_pool[province_row_pool_used].get());
				((demographicswindow_province_row_t*)(province_row_pool[province_row_pool_used].get()))->content = std::get<province_row_option>(values[index]).content;
			((demographicswindow_province_row_t*)(province_row_pool[province_row_pool_used].get()))->set_alternate(!alternate);
				province_row_pool[province_row_pool_used]->impl_on_update(state);
				pops_header_pool_used++;
				province_row_pool_used++;
			}
			return measure_result{std::max(pops_header_pool[0]->base_data.size.x, province_row_pool[0]->base_data.size.x), pops_header_pool[0]->base_data.size.y + province_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(province_row_pool.size() <= size_t(province_row_pool_used)) province_row_pool.emplace_back(make_demographicswindow_province_row(state));
			province_row_pool[province_row_pool_used]->base_data.position.x = int16_t(x);
			province_row_pool[province_row_pool_used]->base_data.position.y = int16_t(y);
			province_row_pool[province_row_pool_used]->parent = destination;
			destination->children.push_back(province_row_pool[province_row_pool_used].get());
			((demographicswindow_province_row_t*)(province_row_pool[province_row_pool_used].get()))->content = std::get<province_row_option>(values[index]).content;
			((demographicswindow_province_row_t*)(province_row_pool[province_row_pool_used].get()))->set_alternate(alternate);
			province_row_pool[province_row_pool_used]->impl_on_update(state);
			province_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ province_row_pool[0]->base_data.size.x, province_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	if(std::holds_alternative<pop_row_option>(values[index])) {
		if(pops_header_pool.empty()) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
		if(pop_row_pool.empty()) pop_row_pool.emplace_back(make_demographicswindow_pop_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<nation_row_option>(values[index - 1]) && !std::holds_alternative<state_row_option>(values[index - 1]) && !std::holds_alternative<province_row_option>(values[index - 1]) && !std::holds_alternative<pop_row_option>(values[index - 1]))) {
			if(destination) {
				if(pops_header_pool.size() <= size_t(pops_header_pool_used)) pops_header_pool.emplace_back(make_demographicswindow_pops_header(state));
				if(pop_row_pool.size() <= size_t(pop_row_pool_used)) pop_row_pool.emplace_back(make_demographicswindow_pop_row(state));
				pops_header_pool[pops_header_pool_used]->base_data.position.x = int16_t(x);
				pops_header_pool[pops_header_pool_used]->base_data.position.y = int16_t(y);
				if(!pops_header_pool[pops_header_pool_used]->parent) {
					pops_header_pool[pops_header_pool_used]->parent = destination;
					pops_header_pool[pops_header_pool_used]->impl_on_update(state);
					pops_header_pool[pops_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(pops_header_pool[pops_header_pool_used].get());
			((demographicswindow_pops_header_t*)(pops_header_pool[pops_header_pool_used].get()))->set_alternate(alternate);
				pop_row_pool[pop_row_pool_used]->base_data.position.x = int16_t(x);
				pop_row_pool[pop_row_pool_used]->base_data.position.y = int16_t(y +  pop_row_pool[0]->base_data.size.y + 0);
				pop_row_pool[pop_row_pool_used]->parent = destination;
				destination->children.push_back(pop_row_pool[pop_row_pool_used].get());
				((demographicswindow_pop_row_t*)(pop_row_pool[pop_row_pool_used].get()))->value = std::get<pop_row_option>(values[index]).value;
			((demographicswindow_pop_row_t*)(pop_row_pool[pop_row_pool_used].get()))->set_alternate(!alternate);
				pop_row_pool[pop_row_pool_used]->impl_on_update(state);
				pops_header_pool_used++;
				pop_row_pool_used++;
			}
			return measure_result{std::max(pops_header_pool[0]->base_data.size.x, pop_row_pool[0]->base_data.size.x), pops_header_pool[0]->base_data.size.y + pop_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
		}
		if(destination) {
			if(pop_row_pool.size() <= size_t(pop_row_pool_used)) pop_row_pool.emplace_back(make_demographicswindow_pop_row(state));
			pop_row_pool[pop_row_pool_used]->base_data.position.x = int16_t(x);
			pop_row_pool[pop_row_pool_used]->base_data.position.y = int16_t(y);
			pop_row_pool[pop_row_pool_used]->parent = destination;
			destination->children.push_back(pop_row_pool[pop_row_pool_used].get());
			((demographicswindow_pop_row_t*)(pop_row_pool[pop_row_pool_used].get()))->value = std::get<pop_row_option>(values[index]).value;
			((demographicswindow_pop_row_t*)(pop_row_pool[pop_row_pool_used].get()))->set_alternate(alternate);
			pop_row_pool[pop_row_pool_used]->impl_on_update(state);
			pop_row_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ pop_row_pool[0]->base_data.size.x, pop_row_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  demographicswindow_main_table_t::reset_pools() {
	pops_header_pool_used = 0;
	nation_row_pool_used = 0;
	pops_header_pool_used = 0;
	state_row_pool_used = 0;
	pops_header_pool_used = 0;
	province_row_pool_used = 0;
	pops_header_pool_used = 0;
	pop_row_pool_used = 0;
}
void demographicswindow_main_show_filters_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::show_filters::update
// END
}
bool demographicswindow_main_show_filters_t::button_action(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::show_filters::lbutton_action
	main.filter_w->parent->move_child_to_front(main.filter_w);
	main.filter_w->set_visible(state, true);
// END
	return true;
}
void demographicswindow_main_reset_filters_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::reset_filters::update
// END
}
bool demographicswindow_main_reset_filters_t::button_action(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::reset_filters::lbutton_action
	popwindow::show_colonial = true;
	popwindow::show_non_colonial = true;
	popwindow::excluded_cultures.clear();
	popwindow::excluded_religions.clear();
	popwindow::excluded_states.clear();
	popwindow::excluded_types.clear();
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_main_only_pops_toggle_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::only_pops_toggle::update
	set_active(state, popwindow::show_only_pops);
// END
}
bool demographicswindow_main_only_pops_toggle_t::button_action(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::only_pops_toggle::lbutton_action
	popwindow::show_only_pops = !popwindow::show_only_pops;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_main_sort_pops_toggle_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::sort_pops_toggle::update
	set_active(state, popwindow::sort_pops);
// END
}
bool demographicswindow_main_sort_pops_toggle_t::button_action(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::sort_pops_toggle::lbutton_action
	popwindow::sort_pops = !popwindow::sort_pops;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_main_sort_states_toggle_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::sort_states_toggle::update
	set_active(state, popwindow::sort_states);
// END
}
bool demographicswindow_main_sort_states_toggle_t::button_action(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent)); 
// BEGIN main::sort_states_toggle::lbutton_action
	popwindow::sort_states = !popwindow::sort_states;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
ui::message_result demographicswindow_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	table.update(state, this);
	remake_layout(state, true);
}
void demographicswindow_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "show_filters") {
					temp.ptr = show_filters.get();
				} else
				if(cname == "reset_filters") {
					temp.ptr = reset_filters.get();
				} else
				if(cname == "only_pops_toggle") {
					temp.ptr = only_pops_toggle.get();
				} else
				if(cname == "sort_pops_toggle") {
					temp.ptr = sort_pops_toggle.get();
				} else
				if(cname == "sort_states_toggle") {
					temp.ptr = sort_states_toggle.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
				if(cname == "table") {
					temp.generator = &table;
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
void demographicswindow_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::main"));
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
		if(child_data.name == "show_filters") {
			show_filters = std::make_unique<demographicswindow_main_show_filters_t>();
			show_filters->parent = this;
			auto cptr = show_filters.get();
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
		if(child_data.name == "reset_filters") {
			reset_filters = std::make_unique<demographicswindow_main_reset_filters_t>();
			reset_filters->parent = this;
			auto cptr = reset_filters.get();
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
		if(child_data.name == "only_pops_toggle") {
			only_pops_toggle = std::make_unique<demographicswindow_main_only_pops_toggle_t>();
			only_pops_toggle->parent = this;
			auto cptr = only_pops_toggle.get();
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
		if(child_data.name == "sort_pops_toggle") {
			sort_pops_toggle = std::make_unique<demographicswindow_main_sort_pops_toggle_t>();
			sort_pops_toggle->parent = this;
			auto cptr = sort_pops_toggle.get();
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
		if(child_data.name == "sort_states_toggle") {
			sort_states_toggle = std::make_unique<demographicswindow_main_sort_states_toggle_t>();
			sort_states_toggle->parent = this;
			auto cptr = sort_states_toggle.get();
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
		if(child_data.name == ".tabtable") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			table_ascending_icon_key = main_section.read<std::string_view>();
			table_descending_icon_key = main_section.read<std::string_view>();
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_spacer_column_start = running_w_total;
			col_section.read(table_spacer_column_width);
			running_w_total += table_spacer_column_width;
			col_section.read<text::text_color>(); // discard
			col_section.read<text::text_color>(); // discard
			col_section.read<text::alignment>(); // discard
			table_location_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_location_column_start = running_w_total;
			col_section.read(table_location_column_width);
			running_w_total += table_location_column_width;
			col_section.read(table_location_column_text_color);
			col_section.read(table_location_header_text_color);
			col_section.read(table_location_text_alignment);
			table_size_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_size_column_start = running_w_total;
			col_section.read(table_size_column_width);
			running_w_total += table_size_column_width;
			col_section.read(table_size_column_text_color);
			col_section.read(table_size_header_text_color);
			col_section.read(table_size_text_alignment);
			table_culture_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_culture_column_start = running_w_total;
			col_section.read(table_culture_column_width);
			running_w_total += table_culture_column_width;
			col_section.read(table_culture_column_text_color);
			col_section.read(table_culture_header_text_color);
			col_section.read(table_culture_text_alignment);
			table_job_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_job_column_start = running_w_total;
			col_section.read(table_job_column_width);
			running_w_total += table_job_column_width;
			col_section.read(table_job_column_text_color);
			col_section.read(table_job_header_text_color);
			col_section.read(table_job_text_alignment);
			table_religion_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_religion_column_start = running_w_total;
			col_section.read(table_religion_column_width);
			running_w_total += table_religion_column_width;
			col_section.read(table_religion_column_text_color);
			col_section.read(table_religion_header_text_color);
			col_section.read(table_religion_text_alignment);
			table_ideology_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_ideology_column_start = running_w_total;
			col_section.read(table_ideology_column_width);
			running_w_total += table_ideology_column_width;
			col_section.read(table_ideology_column_text_color);
			col_section.read(table_ideology_header_text_color);
			col_section.read(table_ideology_text_alignment);
			table_issues_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_issues_column_start = running_w_total;
			col_section.read(table_issues_column_width);
			running_w_total += table_issues_column_width;
			col_section.read(table_issues_column_text_color);
			col_section.read(table_issues_header_text_color);
			col_section.read(table_issues_text_alignment);
			col_section.read<std::string_view>(); // discard
			table_militancy_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			table_militancy_column_start = running_w_total;
			col_section.read(table_militancy_column_width);
			running_w_total += table_militancy_column_width;
			col_section.read(table_militancy_column_text_color);
			col_section.read(table_militancy_header_text_color);
			col_section.read(table_militancy_text_alignment);
			col_section.read<std::string_view>(); // discard
			table_consciousness_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			table_consciousness_column_start = running_w_total;
			col_section.read(table_consciousness_column_width);
			running_w_total += table_consciousness_column_width;
			col_section.read(table_consciousness_column_text_color);
			col_section.read(table_consciousness_header_text_color);
			col_section.read(table_consciousness_text_alignment);
			col_section.read<std::string_view>(); // discard
			table_employment_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			table_employment_column_start = running_w_total;
			col_section.read(table_employment_column_width);
			running_w_total += table_employment_column_width;
			col_section.read(table_employment_column_text_color);
			col_section.read(table_employment_header_text_color);
			col_section.read(table_employment_text_alignment);
			col_section.read<std::string_view>(); // discard
			table_literacy_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			table_literacy_column_start = running_w_total;
			col_section.read(table_literacy_column_width);
			running_w_total += table_literacy_column_width;
			col_section.read(table_literacy_column_text_color);
			col_section.read(table_literacy_header_text_color);
			col_section.read(table_literacy_text_alignment);
			col_section.read<std::string_view>(); // discard
			table_money_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			table_money_column_start = running_w_total;
			col_section.read(table_money_column_width);
			running_w_total += table_money_column_width;
			col_section.read(table_money_column_text_color);
			col_section.read(table_money_header_text_color);
			col_section.read(table_money_text_alignment);
			table_needs_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			table_needs_column_start = running_w_total;
			col_section.read(table_needs_column_width);
			running_w_total += table_needs_column_width;
			col_section.read(table_needs_column_text_color);
			col_section.read(table_needs_header_text_color);
			col_section.read(table_needs_text_alignment);
			col_section.read<std::string_view>(); // discard
			table_rebels_header_tooltip_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			table_rebels_column_start = running_w_total;
			col_section.read(table_rebels_column_width);
			running_w_total += table_rebels_column_width;
			col_section.read(table_rebels_column_text_color);
			col_section.read(table_rebels_header_text_color);
			col_section.read(table_rebels_text_alignment);
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
	table.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
	{
		auto ptr = make_demographicswindow_filters_window(state);
		filter_w = ptr.get();
		//children.insert(children.begin(), filter_w.get());
		filter_w->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(ptr));
	}
	{
		auto ptr = ui::make_element_by_type<ui::national_focus_window>(state, "state_focus_window");
		focus_w = ptr.get();
		//children.insert(children.begin(), focus_w.get());
		focus_w->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(ptr));
	}
	{
		auto ptr = make_pop_details_main(state);
		details_w = ptr.get();
		//children.insert(children.begin(), details_w.get());
		details_w->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(ptr));
	}
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_main(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_main_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_nation_row_location_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::location::update
	set_text(state, text::get_name_as_string(state, nation_row.content));
// END
}
void demographicswindow_nation_row_size_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::size::update
	float total = 0.0f;
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				total += o.get_pop().get_size();
			}
		}
	}
	set_text(state, text::prettify(int64_t(total)));
// END
}
ui::message_result demographicswindow_nation_row_size_trend_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_nation_row_size_trend_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_nation_row_size_trend_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::size_trend::tooltip
	auto box = text::open_layout_box(contents);
	text::localised_format_box(state, contents, box, "pop_growth_1");
	auto result = demographics::get_monthly_pop_increase(state, nation_row.content);

	if(result >= 0) {
		text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
		text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::green);
	} else {
		text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::red);
	}
	text::close_layout_box(contents, box);
// END
}
void demographicswindow_nation_row_size_trend_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_nation_row_size_trend_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::size_trend::update
	auto result = demographics::get_monthly_pop_increase(state, nation_row.content);
	if(result > 0) {
		frame = 0;
	} else if(result < 0) {
		frame = 2;
	} else {
		frame = 1;
	}
// END
}
void demographicswindow_nation_row_size_trend_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN nation_row::size_trend::create
// END
}
void demographicswindow_nation_row_culture_t::on_create(sys::state& state) noexcept {
// BEGIN nation_row::culture::create
// END
}
void demographicswindow_nation_row_culture_t::update_chart(sys::state& state) {
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
void demographicswindow_nation_row_culture_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN nation_row::culture::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.culture_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_nation_row_culture_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_nation_row_culture_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::culture::update
	auto cultures = state.world.culture_make_vectorizable_float_buffer();
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_culture();
				if(c)
					cultures.get(c) += o.get_pop().get_size();
			}
		}
	}
	graph_content.clear();
	for(auto c : state.world.in_culture) {
		if(cultures.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), cultures.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_nation_row_job_t::on_create(sys::state& state) noexcept {
// BEGIN nation_row::job::create
// END
}
void demographicswindow_nation_row_job_t::update_chart(sys::state& state) {
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
void demographicswindow_nation_row_job_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN nation_row::job::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_nation_row_job_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_nation_row_job_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::job::update
	auto jobs = state.world.pop_type_make_vectorizable_float_buffer();
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_poptype();
				if(c)
					jobs.get(c) += o.get_pop().get_size();
			}
		}
	}
	graph_content.clear();
	for(auto c : state.world.in_pop_type) {
		if(jobs.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), jobs.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_nation_row_religion_t::on_create(sys::state& state) noexcept {
// BEGIN nation_row::religion::create
// END
}
void demographicswindow_nation_row_religion_t::update_chart(sys::state& state) {
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
void demographicswindow_nation_row_religion_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN nation_row::religion::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.religion_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_nation_row_religion_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_nation_row_religion_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::religion::update
	auto religions = state.world.religion_make_vectorizable_float_buffer();
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_religion();
				if(c)
					religions.get(c) += o.get_pop().get_size();
			}
		}
	}
	graph_content.clear();
	for(auto c : state.world.in_religion) {
		if(religions.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), religions.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_nation_row_ideology_t::on_create(sys::state& state) noexcept {
// BEGIN nation_row::ideology::create
// END
}
void demographicswindow_nation_row_ideology_t::update_chart(sys::state& state) {
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
void demographicswindow_nation_row_ideology_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN nation_row::ideology::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.ideology_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_nation_row_ideology_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_nation_row_ideology_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::ideology::update
	auto ideologies = state.world.ideology_make_vectorizable_float_buffer();
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto sz = o.get_pop().get_size();
				for(auto i : state.world.in_ideology) {
					ideologies.get(i) = ideologies.get(i) + sz * pop_demographics::from_pu16(o.get_pop().get_udemographics(pop_demographics::to_key(state, i)));
				}
			}
		}
	}
	graph_content.clear();
	for(auto c : state.world.in_ideology) {
		if(ideologies.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), ideologies.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_nation_row_issues_t::on_create(sys::state& state) noexcept {
// BEGIN nation_row::issues::create
// END
}
void demographicswindow_nation_row_issues_t::update_chart(sys::state& state) {
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
void demographicswindow_nation_row_issues_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN nation_row::issues::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.issue_get_name(state.world.issue_option_get_parent_issue(selected_key)));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, state.world.issue_option_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_nation_row_issues_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_nation_row_issues_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::issues::update
	auto issues = state.world.issue_option_make_vectorizable_float_buffer();
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto sz = o.get_pop().get_size();
				for(auto i : state.world.in_issue_option) {
					issues.get(i) = issues.get(i) + sz * pop_demographics::from_pu16(o.get_pop().get_udemographics(pop_demographics::to_key(state, i)));
				}
			}
		}
	}
	graph_content.clear();
	for(auto c : state.world.in_issue_option) {
		if(issues.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(ogl::get_ui_color(state, c.id)), issues.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_nation_row_militancy_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::militancy::update
	float total = 0.0f;
	float sz = 0.0f;
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::from_pmc(o.get_pop().get_umilitancy()) * o.get_pop().get_size();
			}
		}
	}
	set_text(state, text::format_float(sz > 0 ? total / sz : 0.0f, 1));
// END
}
void demographicswindow_nation_row_consciousness_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::consciousness::update
	float total = 0.0f;
	float sz = 0.0f;
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::from_pmc(o.get_pop().get_uconsciousness()) * o.get_pop().get_size();
			}
		}
	}
	set_text(state, text::format_float(sz > 0 ? total / sz : 0.0f, 1));
// END
}
void demographicswindow_nation_row_employment_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::employment::update
	float total = 0.0f;
	float sz = 0.0f;
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				if(o.get_pop().get_poptype().get_has_unemployment()) {
					sz += o.get_pop().get_size();
					total += pop_demographics::get_employment(state, o.get_pop());
				} else {
					sz += o.get_pop().get_size();
					total += o.get_pop().get_size();
				}
			}
		}
	}
	set_text(state, text::format_percentage(sz > 0 ? total / sz : 0.0f, 0));
// END
}
void demographicswindow_nation_row_literacy_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::literacy::update
	float total = 0.0f;
	float sz = 0.0f;
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::get_literacy(state, o.get_pop()) * o.get_pop().get_size();
			}
		}
	}
	set_text(state, text::format_percentage(sz > 0 ? total / sz : 0.0f, 0));
// END
}
void demographicswindow_nation_row_money_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::money::update
	float total = 0.0f;
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				total += o.get_pop().get_savings();
			}
		}
	}
	set_text(state, text::prettify_currency(total));
// END
}
void demographicswindow_nation_row_needs_t::on_create(sys::state& state) noexcept {
// BEGIN nation_row::needs::create
// END
}
void demographicswindow_nation_row_needs_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 99; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 99; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(99)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void demographicswindow_nation_row_needs_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN nation_row::needs::tooltip
		float total = 0.0f;
		auto box = text::open_layout_box(contents);
		if(temp_index == 0 || temp_index == 1) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("life_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[0].amount });
		} else if(temp_index == 2 || temp_index == 3) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("everyday_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[2].amount });
		} else if(temp_index == 4 || temp_index == 5) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("luxury_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[4].amount });
		}
		
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_nation_row_needs_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_nation_row_needs_t::on_update(sys::state& state) noexcept {
	demographicswindow_nation_row_t& nation_row = *((demographicswindow_nation_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::needs::update
	float lntotal = 0.0f;
	float entotal = 0.0f;
	float lxtotal = 0.0f;
	float sz = 0.0f;
	for(auto p : state.world.nation_get_province_ownership(nation_row.content)) {
		for(auto o : p.get_province().get_pop_location()) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				lntotal += pop_demographics::get_life_needs(state, o.get_pop()) * o.get_pop().get_size();
				entotal += pop_demographics::get_everyday_needs(state, o.get_pop()) * o.get_pop().get_size();
				lxtotal += pop_demographics::get_luxury_needs(state, o.get_pop()) * o.get_pop().get_size();
			}
		}
	}

	graph_content.clear();
	graph_content.emplace_back(graph_entry{ 0, ogl::color3f{ 130.0f / 255.0f, 162.0f / 255.0f, 217.0f / 255.0f }, sz > 0.0f ? lntotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 1, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - lntotal / sz : 1.0f });
	graph_content.emplace_back(graph_entry{ 2, ogl::color3f{ 210.0f / 255.0f, 140.0f / 255.0f, 37.0f / 255.0f }, sz > 0.0f ? entotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 3, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - entotal / sz : 1.0f });
	graph_content.emplace_back(graph_entry{ 4, ogl::color3f{ 157.0f / 255.0f, 66.0f / 255.0f, 62.0f / 255.0f }, sz > 0.0f ? lxtotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 5, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - lxtotal / sz : 1.0f });
	update_chart(state);
// END
}
void demographicswindow_nation_row_frames_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_spacer = rel_mouse_x >= table_source->table_spacer_column_start && rel_mouse_x < (table_source->table_spacer_column_start + table_source->table_spacer_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_spacer_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(table_source->table_spacer_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(table_source->table_spacer_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_location = rel_mouse_x >= table_source->table_location_column_start && rel_mouse_x < (table_source->table_location_column_start + table_source->table_location_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_location_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(table_source->table_location_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(table_source->table_location_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_size = rel_mouse_x >= table_source->table_size_column_start && rel_mouse_x < (table_source->table_size_column_start + table_source->table_size_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_size_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(table_source->table_size_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(table_source->table_size_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_culture = rel_mouse_x >= table_source->table_culture_column_start && rel_mouse_x < (table_source->table_culture_column_start + table_source->table_culture_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_culture_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(table_source->table_culture_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(table_source->table_culture_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_job = rel_mouse_x >= table_source->table_job_column_start && rel_mouse_x < (table_source->table_job_column_start + table_source->table_job_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_job_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(table_source->table_job_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(table_source->table_job_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_religion = rel_mouse_x >= table_source->table_religion_column_start && rel_mouse_x < (table_source->table_religion_column_start + table_source->table_religion_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_religion_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(table_source->table_religion_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(table_source->table_religion_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_ideology = rel_mouse_x >= table_source->table_ideology_column_start && rel_mouse_x < (table_source->table_ideology_column_start + table_source->table_ideology_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_ideology_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(table_source->table_ideology_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(table_source->table_ideology_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_issues = rel_mouse_x >= table_source->table_issues_column_start && rel_mouse_x < (table_source->table_issues_column_start + table_source->table_issues_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_issues_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(table_source->table_issues_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(table_source->table_issues_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_militancy = rel_mouse_x >= table_source->table_militancy_column_start && rel_mouse_x < (table_source->table_militancy_column_start + table_source->table_militancy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_militancy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(table_source->table_militancy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(table_source->table_militancy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_consciousness = rel_mouse_x >= table_source->table_consciousness_column_start && rel_mouse_x < (table_source->table_consciousness_column_start + table_source->table_consciousness_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_consciousness_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(table_source->table_consciousness_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(table_source->table_consciousness_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_employment = rel_mouse_x >= table_source->table_employment_column_start && rel_mouse_x < (table_source->table_employment_column_start + table_source->table_employment_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(table_source->table_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(table_source->table_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_literacy = rel_mouse_x >= table_source->table_literacy_column_start && rel_mouse_x < (table_source->table_literacy_column_start + table_source->table_literacy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_literacy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(table_source->table_literacy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(table_source->table_literacy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_money = rel_mouse_x >= table_source->table_money_column_start && rel_mouse_x < (table_source->table_money_column_start + table_source->table_money_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_money_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(table_source->table_money_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(table_source->table_money_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_needs = rel_mouse_x >= table_source->table_needs_column_start && rel_mouse_x < (table_source->table_needs_column_start + table_source->table_needs_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_needs_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(table_source->table_needs_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(table_source->table_needs_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_rebels = rel_mouse_x >= table_source->table_rebels_column_start && rel_mouse_x < (table_source->table_rebels_column_start + table_source->table_rebels_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_rebels_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(table_source->table_rebels_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(table_source->table_rebels_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void demographicswindow_nation_row_frames_t::on_create(sys::state& state) noexcept {
}
void  demographicswindow_nation_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result demographicswindow_nation_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_nation_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_nation_row_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN nation_row::update
// END
	remake_layout(state, true);
}
void demographicswindow_nation_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "location") {
					temp.ptr = location.get();
				} else
				if(cname == "size") {
					temp.ptr = size.get();
				} else
				if(cname == "size_trend") {
					temp.ptr = size_trend.get();
				} else
				if(cname == "culture") {
					temp.ptr = culture.get();
				} else
				if(cname == "job") {
					temp.ptr = job.get();
				} else
				if(cname == "religion") {
					temp.ptr = religion.get();
				} else
				if(cname == "ideology") {
					temp.ptr = ideology.get();
				} else
				if(cname == "issues") {
					temp.ptr = issues.get();
				} else
				if(cname == "militancy") {
					temp.ptr = militancy.get();
				} else
				if(cname == "consciousness") {
					temp.ptr = consciousness.get();
				} else
				if(cname == "employment") {
					temp.ptr = employment.get();
				} else
				if(cname == "literacy") {
					temp.ptr = literacy.get();
				} else
				if(cname == "money") {
					temp.ptr = money.get();
				} else
				if(cname == "needs") {
					temp.ptr = needs.get();
				} else
				if(cname == "frames") {
					temp.ptr = frames.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_nation_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::nation_row"));
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
		if(child_data.name == "location") {
			location = std::make_unique<demographicswindow_nation_row_location_t>();
			location->parent = this;
			auto cptr = location.get();
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
		if(child_data.name == "size") {
			size = std::make_unique<demographicswindow_nation_row_size_t>();
			size->parent = this;
			auto cptr = size.get();
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
		if(child_data.name == "size_trend") {
			size_trend = std::make_unique<demographicswindow_nation_row_size_trend_t>();
			size_trend->parent = this;
			auto cptr = size_trend.get();
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
		if(child_data.name == "culture") {
			culture = std::make_unique<demographicswindow_nation_row_culture_t>();
			culture->parent = this;
			auto cptr = culture.get();
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
		if(child_data.name == "job") {
			job = std::make_unique<demographicswindow_nation_row_job_t>();
			job->parent = this;
			auto cptr = job.get();
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
		if(child_data.name == "religion") {
			religion = std::make_unique<demographicswindow_nation_row_religion_t>();
			religion->parent = this;
			auto cptr = religion.get();
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
		if(child_data.name == "ideology") {
			ideology = std::make_unique<demographicswindow_nation_row_ideology_t>();
			ideology->parent = this;
			auto cptr = ideology.get();
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
		if(child_data.name == "issues") {
			issues = std::make_unique<demographicswindow_nation_row_issues_t>();
			issues->parent = this;
			auto cptr = issues.get();
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
		if(child_data.name == "militancy") {
			militancy = std::make_unique<demographicswindow_nation_row_militancy_t>();
			militancy->parent = this;
			auto cptr = militancy.get();
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
		if(child_data.name == "consciousness") {
			consciousness = std::make_unique<demographicswindow_nation_row_consciousness_t>();
			consciousness->parent = this;
			auto cptr = consciousness.get();
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
		if(child_data.name == "employment") {
			employment = std::make_unique<demographicswindow_nation_row_employment_t>();
			employment->parent = this;
			auto cptr = employment.get();
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
		if(child_data.name == "literacy") {
			literacy = std::make_unique<demographicswindow_nation_row_literacy_t>();
			literacy->parent = this;
			auto cptr = literacy.get();
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
		if(child_data.name == "money") {
			money = std::make_unique<demographicswindow_nation_row_money_t>();
			money->parent = this;
			auto cptr = money.get();
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
		if(child_data.name == "needs") {
			needs = std::make_unique<demographicswindow_nation_row_needs_t>();
			needs->parent = this;
			auto cptr = needs.get();
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
		if(child_data.name == "frames") {
			frames = std::make_unique<demographicswindow_nation_row_frames_t>();
			frames->parent = this;
			auto cptr = frames.get();
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
// BEGIN nation_row::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_nation_row(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_nation_row_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_state_row_folder_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::folder_icon::update
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_folder_48_filled.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_folder_open_28_filled.svg");
	bool is_active = popwindow::open_states.contains(state_row.content.index());
	icon = int16_t(is_active ? un_select_icon : select_icon);
// END
}
bool demographicswindow_state_row_folder_icon_t::button_action(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::folder_icon::lbutton_action
	if(popwindow::open_states.contains(state_row.content.index()))
		popwindow::open_states.erase(state_row.content.index());
	else
		popwindow::open_states.insert(state_row.content.index());
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_state_row_location_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::location::update
	set_text(state, text::get_short_state_name(state, state_row.content));
// END
}
void demographicswindow_state_row_size_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::size::update
	float total = 0.0f;
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				total += o.get_pop().get_size();
			}
		}
	});
	set_text(state, text::prettify(int64_t(total)));
// END
}
ui::message_result demographicswindow_state_row_size_trend_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_state_row_size_trend_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_state_row_size_trend_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::size_trend::tooltip
	auto box = text::open_layout_box(contents);
	text::localised_format_box(state, contents, box, "pop_growth_1");
	auto result = demographics::get_monthly_pop_increase(state, state_row.content);

	if(result >= 0) {
		text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
		text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::green);
	} else {
		text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::red);
	}
	text::close_layout_box(contents, box);
// END
}
void demographicswindow_state_row_size_trend_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_state_row_size_trend_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::size_trend::update
	auto result = demographics::get_monthly_pop_increase(state, state_row.content);
	if(result > 0) {
		frame = 0;
	} else if(result < 0) {
		frame = 2;
	} else {
		frame = 1;
	}
// END
}
void demographicswindow_state_row_size_trend_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN state_row::size_trend::create
// END
}
void demographicswindow_state_row_culture_t::on_create(sys::state& state) noexcept {
// BEGIN state_row::culture::create
// END
}
void demographicswindow_state_row_culture_t::update_chart(sys::state& state) {
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
void demographicswindow_state_row_culture_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN state_row::culture::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.culture_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_state_row_culture_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_state_row_culture_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::culture::update
	auto cultures = state.world.culture_make_vectorizable_float_buffer();
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_culture();
				if(c)
					cultures.get(c) += o.get_pop().get_size();
			}
		}
	});
	graph_content.clear();
	for(auto c : state.world.in_culture) {
		if(cultures.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), cultures.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_state_row_job_t::on_create(sys::state& state) noexcept {
// BEGIN state_row::job::create
// END
}
void demographicswindow_state_row_job_t::update_chart(sys::state& state) {
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
void demographicswindow_state_row_job_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN state_row::job::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_state_row_job_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_state_row_job_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::job::update
	auto jobs = state.world.pop_type_make_vectorizable_float_buffer();
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_poptype();
				if(c)
					jobs.get(c) += o.get_pop().get_size();
			}
		}
	});
	graph_content.clear();
	for(auto c : state.world.in_pop_type) {
		if(jobs.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), jobs.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_state_row_religion_t::on_create(sys::state& state) noexcept {
// BEGIN state_row::religion::create
// END
}
void demographicswindow_state_row_religion_t::update_chart(sys::state& state) {
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
void demographicswindow_state_row_religion_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN state_row::religion::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.religion_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_state_row_religion_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_state_row_religion_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::religion::update
	auto religions = state.world.religion_make_vectorizable_float_buffer();
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_religion();
				if(c)
					religions.get(c) += o.get_pop().get_size();
			}
		}
	});
	graph_content.clear();
	for(auto c : state.world.in_religion) {
		if(religions.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), religions.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_state_row_ideology_t::on_create(sys::state& state) noexcept {
// BEGIN state_row::ideology::create
// END
}
void demographicswindow_state_row_ideology_t::update_chart(sys::state& state) {
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
void demographicswindow_state_row_ideology_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN state_row::ideology::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.ideology_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_state_row_ideology_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_state_row_ideology_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::ideology::update
	auto ideologies = state.world.ideology_make_vectorizable_float_buffer();
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto sz = o.get_pop().get_size();
				for(auto i : state.world.in_ideology) {
					ideologies.get(i) = ideologies.get(i) + sz * pop_demographics::from_pu16(o.get_pop().get_udemographics(pop_demographics::to_key(state, i)));
				}
			}
		}
	});
	graph_content.clear();
	for(auto c : state.world.in_ideology) {
		if(ideologies.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), ideologies.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_state_row_issues_t::on_create(sys::state& state) noexcept {
// BEGIN state_row::issues::create
// END
}
void demographicswindow_state_row_issues_t::update_chart(sys::state& state) {
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
void demographicswindow_state_row_issues_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN state_row::issues::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.issue_get_name(state.world.issue_option_get_parent_issue(selected_key)));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, state.world.issue_option_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_state_row_issues_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_state_row_issues_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::issues::update
	auto issues = state.world.issue_option_make_vectorizable_float_buffer();
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto sz = o.get_pop().get_size();
				for(auto i : state.world.in_issue_option) {
					issues.get(i) = issues.get(i) + sz * pop_demographics::from_pu16(o.get_pop().get_udemographics(pop_demographics::to_key(state, i)));
				}
			}
		}
	});
	graph_content.clear();
	for(auto c : state.world.in_issue_option) {
		if(issues.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(ogl::get_ui_color(state, c.id)), issues.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_state_row_militancy_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::militancy::update
	float total = 0.0f;
	float sz = 0.0f;
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::from_pmc(o.get_pop().get_umilitancy()) * o.get_pop().get_size();
			}
		}
	});
	set_text(state, text::format_float(sz > 0 ? total / sz : 0.0f, 1));
// END
}
void demographicswindow_state_row_consciousness_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::consciousness::update
	float total = 0.0f;
	float sz = 0.0f;
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::from_pmc(o.get_pop().get_uconsciousness()) * o.get_pop().get_size();
			}
		}
	});
	set_text(state, text::format_float(sz > 0 ? total / sz : 0.0f, 1));
// END
}
void demographicswindow_state_row_employment_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::employment::update
	float total = 0.0f;
	float sz = 0.0f;
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				if(o.get_pop().get_poptype().get_has_unemployment()) {
					sz += o.get_pop().get_size();
					total += pop_demographics::get_employment(state, o.get_pop());
				} else {
					sz += o.get_pop().get_size();
					total += o.get_pop().get_size();
				}
			}
		}
	});
	set_text(state, text::format_percentage(sz > 0 ? total / sz : 0.0f, 0));
// END
}
void demographicswindow_state_row_literacy_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::literacy::update
	float total = 0.0f;
	float sz = 0.0f;
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::get_literacy(state, o.get_pop()) * o.get_pop().get_size();
			}
		}
	});
	set_text(state, text::format_percentage(sz > 0 ? total / sz : 0.0f, 0));
// END
}
void demographicswindow_state_row_money_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::money::update
	float total = 0.0f;
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				total += o.get_pop().get_savings();
			}
		}
	});
	set_text(state, text::prettify_currency(total));
// END
}
void demographicswindow_state_row_needs_t::on_create(sys::state& state) noexcept {
// BEGIN state_row::needs::create
// END
}
void demographicswindow_state_row_needs_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 99; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 99; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(99)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void demographicswindow_state_row_needs_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN state_row::needs::tooltip
		float total = 0.0f;
		auto box = text::open_layout_box(contents);
		if(temp_index == 0 || temp_index == 1) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("life_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[0].amount });
		} else if(temp_index == 2 || temp_index == 3) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("everyday_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[2].amount });
		} else if(temp_index == 4 || temp_index == 5) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("luxury_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[4].amount });
		}

		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_state_row_needs_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_state_row_needs_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::needs::update
	float lntotal = 0.0f;
	float entotal = 0.0f;
	float lxtotal = 0.0f;
	float sz = 0.0f;
	province::for_each_province_in_state_instance(state, state_row.content, [&](dcon::province_id p) {
		for(auto o : state.world.province_get_pop_location(p)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				lntotal += pop_demographics::get_life_needs(state, o.get_pop()) * o.get_pop().get_size();
				entotal += pop_demographics::get_everyday_needs(state, o.get_pop()) * o.get_pop().get_size();
				lxtotal += pop_demographics::get_luxury_needs(state, o.get_pop()) * o.get_pop().get_size();
			}
		}
	});

	graph_content.clear();
	graph_content.emplace_back(graph_entry{ 0, ogl::color3f{ 130.0f / 255.0f, 162.0f / 255.0f, 217.0f / 255.0f }, sz > 0.0f ? lntotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 1, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - lntotal / sz : 1.0f });
	graph_content.emplace_back(graph_entry{ 2, ogl::color3f{ 210.0f / 255.0f, 140.0f / 255.0f, 37.0f / 255.0f }, sz > 0.0f ? entotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 3, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - entotal / sz : 1.0f });
	graph_content.emplace_back(graph_entry{ 4, ogl::color3f{ 157.0f / 255.0f, 66.0f / 255.0f, 62.0f / 255.0f }, sz > 0.0f ? lxtotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 5, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - lxtotal / sz : 1.0f });
	update_chart(state);
// END
}
void demographicswindow_state_row_frames_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_spacer = rel_mouse_x >= table_source->table_spacer_column_start && rel_mouse_x < (table_source->table_spacer_column_start + table_source->table_spacer_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_spacer_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(table_source->table_spacer_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(table_source->table_spacer_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_location = rel_mouse_x >= table_source->table_location_column_start && rel_mouse_x < (table_source->table_location_column_start + table_source->table_location_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_location_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(table_source->table_location_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(table_source->table_location_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_size = rel_mouse_x >= table_source->table_size_column_start && rel_mouse_x < (table_source->table_size_column_start + table_source->table_size_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_size_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(table_source->table_size_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(table_source->table_size_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_culture = rel_mouse_x >= table_source->table_culture_column_start && rel_mouse_x < (table_source->table_culture_column_start + table_source->table_culture_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_culture_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(table_source->table_culture_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(table_source->table_culture_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_job = rel_mouse_x >= table_source->table_job_column_start && rel_mouse_x < (table_source->table_job_column_start + table_source->table_job_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_job_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(table_source->table_job_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(table_source->table_job_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_religion = rel_mouse_x >= table_source->table_religion_column_start && rel_mouse_x < (table_source->table_religion_column_start + table_source->table_religion_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_religion_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(table_source->table_religion_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(table_source->table_religion_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_ideology = rel_mouse_x >= table_source->table_ideology_column_start && rel_mouse_x < (table_source->table_ideology_column_start + table_source->table_ideology_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_ideology_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(table_source->table_ideology_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(table_source->table_ideology_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_issues = rel_mouse_x >= table_source->table_issues_column_start && rel_mouse_x < (table_source->table_issues_column_start + table_source->table_issues_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_issues_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(table_source->table_issues_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(table_source->table_issues_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_militancy = rel_mouse_x >= table_source->table_militancy_column_start && rel_mouse_x < (table_source->table_militancy_column_start + table_source->table_militancy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_militancy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(table_source->table_militancy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(table_source->table_militancy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_consciousness = rel_mouse_x >= table_source->table_consciousness_column_start && rel_mouse_x < (table_source->table_consciousness_column_start + table_source->table_consciousness_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_consciousness_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(table_source->table_consciousness_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(table_source->table_consciousness_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_employment = rel_mouse_x >= table_source->table_employment_column_start && rel_mouse_x < (table_source->table_employment_column_start + table_source->table_employment_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(table_source->table_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(table_source->table_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_literacy = rel_mouse_x >= table_source->table_literacy_column_start && rel_mouse_x < (table_source->table_literacy_column_start + table_source->table_literacy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_literacy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(table_source->table_literacy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(table_source->table_literacy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_money = rel_mouse_x >= table_source->table_money_column_start && rel_mouse_x < (table_source->table_money_column_start + table_source->table_money_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_money_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(table_source->table_money_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(table_source->table_money_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_needs = rel_mouse_x >= table_source->table_needs_column_start && rel_mouse_x < (table_source->table_needs_column_start + table_source->table_needs_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_needs_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(table_source->table_needs_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(table_source->table_needs_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_rebels = rel_mouse_x >= table_source->table_rebels_column_start && rel_mouse_x < (table_source->table_rebels_column_start + table_source->table_rebels_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_rebels_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(table_source->table_rebels_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(table_source->table_rebels_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void demographicswindow_state_row_frames_t::on_create(sys::state& state) noexcept {
}
ui::message_result demographicswindow_state_row_focus_button_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN state_row::focus_button::lbutton_action
	auto fwindow_ptr = (ui::national_focus_window*)(main.focus_w);
	assert(fwindow_ptr);
	(fwindow_ptr)->provided = state_row.content;
	if(!(fwindow_ptr)->is_visible())
		(fwindow_ptr)->set_visible(state, true);
	else
		(fwindow_ptr)->impl_on_update(state);
	(fwindow_ptr)->parent->move_child_to_front(fwindow_ptr);
	(fwindow_ptr)->base_data.position = ui::get_absolute_location(state, *this);
	(fwindow_ptr)->base_data.position.y = std::min((fwindow_ptr)->base_data.position.y, int16_t(fwindow_ptr->parent->base_data.size.y - 460));
// END
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_state_row_focus_button_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(disabled) return ui::message_result::consumed;
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN state_row::focus_button::rbutton_action
	command::set_national_focus(state, state.local_player_nation, state_row.content, dcon::national_focus_id{});
// END
	return ui::message_result::consumed;
}
void demographicswindow_state_row_focus_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::focus_button::tooltip
	auto box = text::open_layout_box(contents, 0);

	auto sid = state_row.content;
	auto fat_si = dcon::fatten(state.world, sid);
	text::add_to_layout_box(state, contents, box, sid);
	text::add_line_break_to_layout_box(state, contents, box);
	auto content = state.world.state_instance_get_owner_focus(sid);
	if(bool(content)) {
		auto fat_nf = dcon::fatten(state.world, content);
		text::add_to_layout_box(state, contents, box, state.world.national_focus_get_name(content), text::substitution_map{});
		text::add_line_break_to_layout_box(state, contents, box);
		auto color = text::text_color::white;
		if(fat_nf.get_promotion_type()) {
			//Is the NF not optimal? Recolor it
			if(fat_nf.get_promotion_type() == state.culture_definitions.clergy) {
				if((fat_si.get_demographics(demographics::to_key(state, fat_nf.get_promotion_type())) / fat_si.get_demographics(demographics::total)) > state.defines.max_clergy_for_literacy) {
					color = text::text_color::red;
				}
			}
			auto full_str = text::format_percentage(fat_si.get_demographics(demographics::to_key(state, fat_nf.get_promotion_type())) / fat_si.get_demographics(demographics::total));
			text::add_to_layout_box(state, contents, box, std::string_view(full_str), color);
		}
	}
	text::close_layout_box(contents, box);
	if(auto mid = state.world.national_focus_get_modifier(content);  mid) {
		ui::modifier_description(state, contents, mid, 15);
	}
	text::add_line(state, contents, "alice_nf_controls");
// END
}
void demographicswindow_state_row_focus_button_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(background_gid) {
		auto& gfx_def = state.ui_defs.gfx[background_gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), frame, gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else {
				ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, disabled, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			}
		}
	}
}
void demographicswindow_state_row_focus_button_t::on_update(sys::state& state) noexcept {
	demographicswindow_state_row_t& state_row = *((demographicswindow_state_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::focus_button::update
	auto content = state_row.content;
	disabled = true;
	state.world.for_each_national_focus([&](dcon::national_focus_id nfid) {
		disabled = command::can_set_national_focus(state, state.local_player_nation, content, nfid) ? false : disabled;
	});
	frame = bool(state.world.state_instance_get_owner_focus(content).id) ? state.world.state_instance_get_owner_focus(content).get_icon() - 1 : 0;
// END
}
void demographicswindow_state_row_focus_button_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN state_row::focus_button::create
// END
}
void  demographicswindow_state_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result demographicswindow_state_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_state_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_state_row_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN state_row::update
// END
	remake_layout(state, true);
}
void demographicswindow_state_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "folder_icon") {
					temp.ptr = folder_icon.get();
				} else
				if(cname == "location") {
					temp.ptr = location.get();
				} else
				if(cname == "size") {
					temp.ptr = size.get();
				} else
				if(cname == "size_trend") {
					temp.ptr = size_trend.get();
				} else
				if(cname == "culture") {
					temp.ptr = culture.get();
				} else
				if(cname == "job") {
					temp.ptr = job.get();
				} else
				if(cname == "religion") {
					temp.ptr = religion.get();
				} else
				if(cname == "ideology") {
					temp.ptr = ideology.get();
				} else
				if(cname == "issues") {
					temp.ptr = issues.get();
				} else
				if(cname == "militancy") {
					temp.ptr = militancy.get();
				} else
				if(cname == "consciousness") {
					temp.ptr = consciousness.get();
				} else
				if(cname == "employment") {
					temp.ptr = employment.get();
				} else
				if(cname == "literacy") {
					temp.ptr = literacy.get();
				} else
				if(cname == "money") {
					temp.ptr = money.get();
				} else
				if(cname == "needs") {
					temp.ptr = needs.get();
				} else
				if(cname == "frames") {
					temp.ptr = frames.get();
				} else
				if(cname == "focus_button") {
					temp.ptr = focus_button.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_state_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::state_row"));
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
		if(child_data.name == "folder_icon") {
			folder_icon = std::make_unique<demographicswindow_state_row_folder_icon_t>();
			folder_icon->parent = this;
			auto cptr = folder_icon.get();
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
		if(child_data.name == "location") {
			location = std::make_unique<demographicswindow_state_row_location_t>();
			location->parent = this;
			auto cptr = location.get();
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
		if(child_data.name == "size") {
			size = std::make_unique<demographicswindow_state_row_size_t>();
			size->parent = this;
			auto cptr = size.get();
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
		if(child_data.name == "size_trend") {
			size_trend = std::make_unique<demographicswindow_state_row_size_trend_t>();
			size_trend->parent = this;
			auto cptr = size_trend.get();
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
		if(child_data.name == "culture") {
			culture = std::make_unique<demographicswindow_state_row_culture_t>();
			culture->parent = this;
			auto cptr = culture.get();
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
		if(child_data.name == "job") {
			job = std::make_unique<demographicswindow_state_row_job_t>();
			job->parent = this;
			auto cptr = job.get();
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
		if(child_data.name == "religion") {
			religion = std::make_unique<demographicswindow_state_row_religion_t>();
			religion->parent = this;
			auto cptr = religion.get();
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
		if(child_data.name == "ideology") {
			ideology = std::make_unique<demographicswindow_state_row_ideology_t>();
			ideology->parent = this;
			auto cptr = ideology.get();
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
		if(child_data.name == "issues") {
			issues = std::make_unique<demographicswindow_state_row_issues_t>();
			issues->parent = this;
			auto cptr = issues.get();
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
		if(child_data.name == "militancy") {
			militancy = std::make_unique<demographicswindow_state_row_militancy_t>();
			militancy->parent = this;
			auto cptr = militancy.get();
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
		if(child_data.name == "consciousness") {
			consciousness = std::make_unique<demographicswindow_state_row_consciousness_t>();
			consciousness->parent = this;
			auto cptr = consciousness.get();
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
		if(child_data.name == "employment") {
			employment = std::make_unique<demographicswindow_state_row_employment_t>();
			employment->parent = this;
			auto cptr = employment.get();
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
		if(child_data.name == "literacy") {
			literacy = std::make_unique<demographicswindow_state_row_literacy_t>();
			literacy->parent = this;
			auto cptr = literacy.get();
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
		if(child_data.name == "money") {
			money = std::make_unique<demographicswindow_state_row_money_t>();
			money->parent = this;
			auto cptr = money.get();
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
		if(child_data.name == "needs") {
			needs = std::make_unique<demographicswindow_state_row_needs_t>();
			needs->parent = this;
			auto cptr = needs.get();
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
		if(child_data.name == "frames") {
			frames = std::make_unique<demographicswindow_state_row_frames_t>();
			frames->parent = this;
			auto cptr = frames.get();
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
		if(child_data.name == "focus_button") {
			focus_button = std::make_unique<demographicswindow_state_row_focus_button_t>();
			focus_button->parent = this;
			auto cptr = focus_button.get();
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
// BEGIN state_row::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_state_row(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_state_row_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_province_row_folder_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::folder_icon::update
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_folder_48_filled.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_folder_open_28_filled.svg");
	bool is_active = popwindow::open_provs.contains(province_row.content.index());
	icon = int16_t(is_active ? un_select_icon : select_icon);
// END
}
bool demographicswindow_province_row_folder_icon_t::button_action(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::folder_icon::lbutton_action
	if(popwindow::open_provs.contains(province_row.content.index()))
		popwindow::open_provs.erase(province_row.content.index());
	else
		popwindow::open_provs.insert(province_row.content.index());
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_province_row_location_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::location::update
	set_text(state, text::produce_simple_string(state, state.world.province_get_name(province_row.content)));
// END
}
void demographicswindow_province_row_size_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::size::update
	float total = 0.0f;
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				total += o.get_pop().get_size();
			}
		}
	
	set_text(state, text::prettify(int64_t(total)));
// END
}
ui::message_result demographicswindow_province_row_size_trend_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_province_row_size_trend_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_province_row_size_trend_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::size_trend::tooltip
	auto box = text::open_layout_box(contents);
	text::localised_format_box(state, contents, box, "pop_growth_1");
	auto result = demographics::get_monthly_pop_increase(state, province_row.content);

	if(result >= 0) {
		text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
		text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::green);
	} else {
		text::add_to_layout_box(state, contents, box, int64_t(result), text::text_color::red);
	}
	text::close_layout_box(contents, box);
// END
}
void demographicswindow_province_row_size_trend_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_province_row_size_trend_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::size_trend::update
	auto result = demographics::get_monthly_pop_increase(state, province_row.content);
	if(result > 0) {
		frame = 0;
	} else if(result < 0) {
		frame = 2;
	} else {
		frame = 1;
	}
// END
}
void demographicswindow_province_row_size_trend_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN province_row::size_trend::create
// END
}
void demographicswindow_province_row_culture_t::on_create(sys::state& state) noexcept {
// BEGIN province_row::culture::create
// END
}
void demographicswindow_province_row_culture_t::update_chart(sys::state& state) {
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
void demographicswindow_province_row_culture_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN province_row::culture::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.culture_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_province_row_culture_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_province_row_culture_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::culture::update
	auto cultures = state.world.culture_make_vectorizable_float_buffer();
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_culture();
				if(c)
					cultures.get(c) += o.get_pop().get_size();
			}
		}
	graph_content.clear();
	for(auto c : state.world.in_culture) {
		if(cultures.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), cultures.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_province_row_job_t::on_create(sys::state& state) noexcept {
// BEGIN province_row::job::create
// END
}
void demographicswindow_province_row_job_t::update_chart(sys::state& state) {
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
void demographicswindow_province_row_job_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN province_row::job::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.pop_type_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_province_row_job_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_province_row_job_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::job::update
	auto jobs = state.world.pop_type_make_vectorizable_float_buffer();
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_poptype();
				if(c)
					jobs.get(c) += o.get_pop().get_size();
			}
		}
	graph_content.clear();
	for(auto c : state.world.in_pop_type) {
		if(jobs.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), jobs.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_province_row_religion_t::on_create(sys::state& state) noexcept {
// BEGIN province_row::religion::create
// END
}
void demographicswindow_province_row_religion_t::update_chart(sys::state& state) {
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
void demographicswindow_province_row_religion_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN province_row::religion::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.religion_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_province_row_religion_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_province_row_religion_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::religion::update
	auto religions = state.world.religion_make_vectorizable_float_buffer();
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto c = o.get_pop().get_religion();
				if(c)
					religions.get(c) += o.get_pop().get_size();
			}
		}
	graph_content.clear();
	for(auto c : state.world.in_religion) {
		if(religions.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), religions.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_province_row_ideology_t::on_create(sys::state& state) noexcept {
// BEGIN province_row::ideology::create
// END
}
void demographicswindow_province_row_ideology_t::update_chart(sys::state& state) {
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
void demographicswindow_province_row_ideology_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN province_row::ideology::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.ideology_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_province_row_ideology_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_province_row_ideology_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::ideology::update
	auto ideologies = state.world.ideology_make_vectorizable_float_buffer();
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto sz = o.get_pop().get_size();
				for(auto i : state.world.in_ideology) {
					ideologies.get(i) = ideologies.get(i) + sz * pop_demographics::from_pu16(o.get_pop().get_udemographics(pop_demographics::to_key(state, i)));
				}
			}
		}
	
	graph_content.clear();
	for(auto c : state.world.in_ideology) {
		if(ideologies.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(c.get_color()), ideologies.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_province_row_issues_t::on_create(sys::state& state) noexcept {
// BEGIN province_row::issues::create
// END
}
void demographicswindow_province_row_issues_t::update_chart(sys::state& state) {
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
void demographicswindow_province_row_issues_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN province_row::issues::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.issue_get_name(state.world.issue_option_get_parent_issue(selected_key)));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, state.world.issue_option_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_province_row_issues_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_province_row_issues_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::issues::update
	auto issues = state.world.issue_option_make_vectorizable_float_buffer();
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				auto sz = o.get_pop().get_size();
				for(auto i : state.world.in_issue_option) {
					issues.get(i) = issues.get(i) + sz * pop_demographics::from_pu16(o.get_pop().get_udemographics(pop_demographics::to_key(state, i)));
				}
			}
		}
	graph_content.clear();
	for(auto c : state.world.in_issue_option) {
		if(issues.get(c) > 0.0f) {
			graph_content.emplace_back(graph_entry{ c.id, ogl::unpack_color(ogl::get_ui_color(state, c.id)), issues.get(c) });
		}
	}
	update_chart(state);
// END
}
void demographicswindow_province_row_militancy_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::militancy::update
	float total = 0.0f;
	float sz = 0.0f;
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::from_pmc(o.get_pop().get_umilitancy()) * o.get_pop().get_size();
			}
		}
	set_text(state, text::format_float(sz > 0 ? total / sz : 0.0f, 1));
// END
}
void demographicswindow_province_row_consciousness_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::consciousness::update
	float total = 0.0f;
	float sz = 0.0f;
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::from_pmc(o.get_pop().get_uconsciousness()) * o.get_pop().get_size();
			}
		}
	set_text(state, text::format_float(sz > 0 ? total / sz : 0.0f, 1));
// END
}
void demographicswindow_province_row_employment_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::employment::update
	float total = 0.0f;
	float sz = 0.0f;
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				if(o.get_pop().get_poptype().get_has_unemployment()) {
					sz += o.get_pop().get_size();
					total += pop_demographics::get_employment(state, o.get_pop());
				} else {
					sz += o.get_pop().get_size();
					total += o.get_pop().get_size();
				}
			}
		}
	set_text(state, text::format_percentage(sz > 0 ? total / sz : 0.0f, 0));
// END
}
void demographicswindow_province_row_literacy_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::literacy::update
	float total = 0.0f;
	float sz = 0.0f;
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				total += pop_demographics::get_literacy(state, o.get_pop()) * o.get_pop().get_size();
			}
		}
	set_text(state, text::format_percentage(sz > 0 ? total / sz : 0.0f, 0));
// END
}
void demographicswindow_province_row_money_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::money::update
	float total = 0.0f;
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				total += o.get_pop().get_savings();
			}
		}
	set_text(state, text::prettify_currency(total));
// END
}
void demographicswindow_province_row_needs_t::on_create(sys::state& state) noexcept {
// BEGIN province_row::needs::create
// END
}
void demographicswindow_province_row_needs_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 99; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 99; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(99)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void demographicswindow_province_row_needs_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN province_row::needs::tooltip
		float total = 0.0f;
		auto box = text::open_layout_box(contents);
		if(temp_index == 0 || temp_index == 1) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("life_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[0].amount });
		} else if(temp_index == 2 || temp_index == 3) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("everyday_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[2].amount });
		} else if(temp_index == 4 || temp_index == 5) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("luxury_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[4].amount });
		}

		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_province_row_needs_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_province_row_needs_t::on_update(sys::state& state) noexcept {
	demographicswindow_province_row_t& province_row = *((demographicswindow_province_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::needs::update
	float lntotal = 0.0f;
	float entotal = 0.0f;
	float lxtotal = 0.0f;
	float sz = 0.0f;
	
		for(auto o : state.world.province_get_pop_location(province_row.content)) {
			if(alice_ui::pop_passes_filter(state, o.get_pop())) {
				sz += o.get_pop().get_size();
				lntotal += pop_demographics::get_life_needs(state, o.get_pop()) * o.get_pop().get_size();
				entotal += pop_demographics::get_everyday_needs(state, o.get_pop()) * o.get_pop().get_size();
				lxtotal += pop_demographics::get_luxury_needs(state, o.get_pop()) * o.get_pop().get_size();
			}
		}

	graph_content.clear();
	graph_content.emplace_back(graph_entry{ 0, ogl::color3f{ 130.0f / 255.0f, 162.0f / 255.0f, 217.0f / 255.0f }, sz > 0.0f ? lntotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 1, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - lntotal / sz : 1.0f });
	graph_content.emplace_back(graph_entry{ 2, ogl::color3f{ 210.0f / 255.0f, 140.0f / 255.0f, 37.0f / 255.0f }, sz > 0.0f ? entotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 3, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - entotal / sz : 1.0f });
	graph_content.emplace_back(graph_entry{ 4, ogl::color3f{ 157.0f / 255.0f, 66.0f / 255.0f, 62.0f / 255.0f }, sz > 0.0f ? lxtotal / sz : 0.0f });
	graph_content.emplace_back(graph_entry{ 5, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, sz > 0.0f ? 1.0f - lxtotal / sz : 1.0f });
	update_chart(state);
// END
}
void demographicswindow_province_row_frames_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_spacer = rel_mouse_x >= table_source->table_spacer_column_start && rel_mouse_x < (table_source->table_spacer_column_start + table_source->table_spacer_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_spacer_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(table_source->table_spacer_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(table_source->table_spacer_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_location = rel_mouse_x >= table_source->table_location_column_start && rel_mouse_x < (table_source->table_location_column_start + table_source->table_location_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_location_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(table_source->table_location_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(table_source->table_location_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_size = rel_mouse_x >= table_source->table_size_column_start && rel_mouse_x < (table_source->table_size_column_start + table_source->table_size_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_size_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(table_source->table_size_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(table_source->table_size_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_culture = rel_mouse_x >= table_source->table_culture_column_start && rel_mouse_x < (table_source->table_culture_column_start + table_source->table_culture_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_culture_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(table_source->table_culture_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(table_source->table_culture_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_job = rel_mouse_x >= table_source->table_job_column_start && rel_mouse_x < (table_source->table_job_column_start + table_source->table_job_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_job_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(table_source->table_job_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(table_source->table_job_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_religion = rel_mouse_x >= table_source->table_religion_column_start && rel_mouse_x < (table_source->table_religion_column_start + table_source->table_religion_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_religion_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(table_source->table_religion_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(table_source->table_religion_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_ideology = rel_mouse_x >= table_source->table_ideology_column_start && rel_mouse_x < (table_source->table_ideology_column_start + table_source->table_ideology_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_ideology_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(table_source->table_ideology_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(table_source->table_ideology_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_issues = rel_mouse_x >= table_source->table_issues_column_start && rel_mouse_x < (table_source->table_issues_column_start + table_source->table_issues_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_issues_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(table_source->table_issues_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(table_source->table_issues_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_militancy = rel_mouse_x >= table_source->table_militancy_column_start && rel_mouse_x < (table_source->table_militancy_column_start + table_source->table_militancy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_militancy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(table_source->table_militancy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(table_source->table_militancy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_consciousness = rel_mouse_x >= table_source->table_consciousness_column_start && rel_mouse_x < (table_source->table_consciousness_column_start + table_source->table_consciousness_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_consciousness_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(table_source->table_consciousness_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(table_source->table_consciousness_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_employment = rel_mouse_x >= table_source->table_employment_column_start && rel_mouse_x < (table_source->table_employment_column_start + table_source->table_employment_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(table_source->table_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(table_source->table_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_literacy = rel_mouse_x >= table_source->table_literacy_column_start && rel_mouse_x < (table_source->table_literacy_column_start + table_source->table_literacy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_literacy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(table_source->table_literacy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(table_source->table_literacy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_money = rel_mouse_x >= table_source->table_money_column_start && rel_mouse_x < (table_source->table_money_column_start + table_source->table_money_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_money_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(table_source->table_money_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(table_source->table_money_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_needs = rel_mouse_x >= table_source->table_needs_column_start && rel_mouse_x < (table_source->table_needs_column_start + table_source->table_needs_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_needs_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(table_source->table_needs_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(table_source->table_needs_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_rebels = rel_mouse_x >= table_source->table_rebels_column_start && rel_mouse_x < (table_source->table_rebels_column_start + table_source->table_rebels_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_rebels_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(table_source->table_rebels_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(table_source->table_rebels_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void demographicswindow_province_row_frames_t::on_create(sys::state& state) noexcept {
}
void  demographicswindow_province_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result demographicswindow_province_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_province_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_province_row_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN province_row::update
// END
	remake_layout(state, true);
}
void demographicswindow_province_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "folder_icon") {
					temp.ptr = folder_icon.get();
				} else
				if(cname == "location") {
					temp.ptr = location.get();
				} else
				if(cname == "size") {
					temp.ptr = size.get();
				} else
				if(cname == "size_trend") {
					temp.ptr = size_trend.get();
				} else
				if(cname == "culture") {
					temp.ptr = culture.get();
				} else
				if(cname == "job") {
					temp.ptr = job.get();
				} else
				if(cname == "religion") {
					temp.ptr = religion.get();
				} else
				if(cname == "ideology") {
					temp.ptr = ideology.get();
				} else
				if(cname == "issues") {
					temp.ptr = issues.get();
				} else
				if(cname == "militancy") {
					temp.ptr = militancy.get();
				} else
				if(cname == "consciousness") {
					temp.ptr = consciousness.get();
				} else
				if(cname == "employment") {
					temp.ptr = employment.get();
				} else
				if(cname == "literacy") {
					temp.ptr = literacy.get();
				} else
				if(cname == "money") {
					temp.ptr = money.get();
				} else
				if(cname == "needs") {
					temp.ptr = needs.get();
				} else
				if(cname == "frames") {
					temp.ptr = frames.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_province_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::province_row"));
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
		if(child_data.name == "folder_icon") {
			folder_icon = std::make_unique<demographicswindow_province_row_folder_icon_t>();
			folder_icon->parent = this;
			auto cptr = folder_icon.get();
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
		if(child_data.name == "location") {
			location = std::make_unique<demographicswindow_province_row_location_t>();
			location->parent = this;
			auto cptr = location.get();
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
		if(child_data.name == "size") {
			size = std::make_unique<demographicswindow_province_row_size_t>();
			size->parent = this;
			auto cptr = size.get();
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
		if(child_data.name == "size_trend") {
			size_trend = std::make_unique<demographicswindow_province_row_size_trend_t>();
			size_trend->parent = this;
			auto cptr = size_trend.get();
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
		if(child_data.name == "culture") {
			culture = std::make_unique<demographicswindow_province_row_culture_t>();
			culture->parent = this;
			auto cptr = culture.get();
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
		if(child_data.name == "job") {
			job = std::make_unique<demographicswindow_province_row_job_t>();
			job->parent = this;
			auto cptr = job.get();
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
		if(child_data.name == "religion") {
			religion = std::make_unique<demographicswindow_province_row_religion_t>();
			religion->parent = this;
			auto cptr = religion.get();
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
		if(child_data.name == "ideology") {
			ideology = std::make_unique<demographicswindow_province_row_ideology_t>();
			ideology->parent = this;
			auto cptr = ideology.get();
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
		if(child_data.name == "issues") {
			issues = std::make_unique<demographicswindow_province_row_issues_t>();
			issues->parent = this;
			auto cptr = issues.get();
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
		if(child_data.name == "militancy") {
			militancy = std::make_unique<demographicswindow_province_row_militancy_t>();
			militancy->parent = this;
			auto cptr = militancy.get();
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
		if(child_data.name == "consciousness") {
			consciousness = std::make_unique<demographicswindow_province_row_consciousness_t>();
			consciousness->parent = this;
			auto cptr = consciousness.get();
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
		if(child_data.name == "employment") {
			employment = std::make_unique<demographicswindow_province_row_employment_t>();
			employment->parent = this;
			auto cptr = employment.get();
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
		if(child_data.name == "literacy") {
			literacy = std::make_unique<demographicswindow_province_row_literacy_t>();
			literacy->parent = this;
			auto cptr = literacy.get();
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
		if(child_data.name == "money") {
			money = std::make_unique<demographicswindow_province_row_money_t>();
			money->parent = this;
			auto cptr = money.get();
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
		if(child_data.name == "needs") {
			needs = std::make_unique<demographicswindow_province_row_needs_t>();
			needs->parent = this;
			auto cptr = needs.get();
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
		if(child_data.name == "frames") {
			frames = std::make_unique<demographicswindow_province_row_frames_t>();
			frames->parent = this;
			auto cptr = frames.get();
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
// BEGIN province_row::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_province_row(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_province_row_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_pop_row_location_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::location::update
	if(popwindow::show_only_pops) {
		auto loc = state.world.pop_get_province_from_pop_location(pop_row.value);
		auto txt = text::produce_simple_string(state, state.world.province_get_name(loc)) + " (" + text::get_short_state_name(state, state.world.province_get_state_membership(loc)) + ")";
		set_text(state, txt);
	} else {
		set_text(state, "");
	}
// END
}
void demographicswindow_pop_row_size_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::size::tooltip
	auto pop = pop_row.value;
	auto growth = int64_t(demographics::get_monthly_pop_increase(state, pop));
	auto promote = -int64_t(demographics::get_estimated_type_change(state, pop));
	auto assimilation = -int64_t(demographics::get_estimated_assimilation(state, pop));
	auto internal_migration = -int64_t(demographics::get_estimated_internal_migration(state, pop));
	auto colonial_migration = -int64_t(demographics::get_estimated_colonial_migration(state, pop));
	auto emigration = -int64_t(demographics::get_estimated_emigration(state, pop));
	auto total = int64_t(growth) + promote + assimilation + internal_migration + colonial_migration + emigration;

	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_size_1");
		if(total >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, total, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, total, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	text::add_line_break_to_layout(state, contents);
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_size_2");
		if(growth >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, growth, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, growth, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_size_3");
		if(promote >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, promote, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, promote, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_size_4");
		if(assimilation >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, assimilation, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, assimilation, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_size_5");
		if(internal_migration >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, internal_migration, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, internal_migration, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_size_6");
		if(colonial_migration >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, colonial_migration, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, colonial_migration, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
	{
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "pop_size_7");
		if(emigration >= 0) {
			text::add_to_layout_box(state, contents, box, std::string_view{ "+" }, text::text_color::green);
			text::add_to_layout_box(state, contents, box, emigration, text::text_color::green);
		} else {
			text::add_to_layout_box(state, contents, box, emigration, text::text_color::red);
		}
		text::close_layout_box(contents, box);
	}
// END
}
void demographicswindow_pop_row_size_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::size::update
	set_text(state, text::prettify(int64_t(state.world.pop_get_size(pop_row.value))));
// END
}
ui::message_result demographicswindow_pop_row_size_trend_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pop_row_size_trend_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pop_row_size_trend_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::size_trend::tooltip
	alice_ui::describe_growth(state, contents, pop_row.value);
// END
}
void demographicswindow_pop_row_size_trend_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_pop_row_size_trend_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::size_trend::update
	auto result = int64_t(demographics::get_monthly_pop_increase(state, pop_row.value));
	if(result > 0) {
		frame = 0;
	} else if(result < 0) {
		frame = 2;
	} else {
		frame = 1;
	}
// END
}
void demographicswindow_pop_row_size_trend_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN pop_row::size_trend::create
// END
}
ui::message_result demographicswindow_pop_row_culture_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pop_row_culture_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pop_row_culture_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::culture::tooltip
	text::add_line(state, contents, state.world.pop_get_culture(pop_row.value).get_name());
// END
}
void demographicswindow_pop_row_culture_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
		ogl::render_alpha_colored_rect(state, float(x ), float(y), float(base_data.size.x), float(base_data.size.y), color.r, color.g, color.b, color.a);
}
void demographicswindow_pop_row_culture_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::culture::update
	auto c = ogl::unpack_color(state.world.pop_get_culture(pop_row.value).get_color());
	color.r = c.r; color.b = c.b; color.g = c.g;
// END
}
void demographicswindow_pop_row_culture_t::on_create(sys::state& state) noexcept {
// BEGIN pop_row::culture::create
// END
}
ui::message_result demographicswindow_pop_row_job_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pop_row_job_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pop_row_job_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::job::tooltip
	text::add_line(state, contents, state.world.pop_get_poptype(pop_row.value).get_name());
// END
}
void demographicswindow_pop_row_job_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_pop_row_job_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::job::update
	frame = int32_t(state.world.pop_get_poptype(pop_row.value).get_sprite()) - 1;
// END
}
void demographicswindow_pop_row_job_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN pop_row::job::create
// END
}
ui::message_result demographicswindow_pop_row_religion_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pop_row_religion_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pop_row_religion_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::religion::tooltip
	text::add_line(state, contents, state.world.pop_get_religion(pop_row.value).get_name());
// END
}
void demographicswindow_pop_row_religion_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_pop_row_religion_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::religion::update
	frame = int32_t(state.world.pop_get_religion(pop_row.value).get_icon()) - 1;
// END
}
void demographicswindow_pop_row_religion_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN pop_row::religion::create
// END
}
void demographicswindow_pop_row_ideology_t::on_create(sys::state& state) noexcept {
// BEGIN pop_row::ideology::create
// END
}
void demographicswindow_pop_row_ideology_t::update_chart(sys::state& state) {
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
void demographicswindow_pop_row_ideology_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN pop_row::ideology::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.ideology_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_pop_row_ideology_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_pop_row_ideology_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::ideology::update
	graph_content.clear();
	for(auto i : state.world.in_ideology) {
		auto amount = pop_demographics::get_demo(state, pop_row.value, pop_demographics::to_key(state, i));
		if(amount > 0)
			graph_content.emplace_back(graph_entry{ i, ogl::unpack_color(i.get_color()),  amount });
	}
	update_chart(state);
// END
}
void demographicswindow_pop_row_issues_t::on_create(sys::state& state) noexcept {
// BEGIN pop_row::issues::create
// END
}
void demographicswindow_pop_row_issues_t::update_chart(sys::state& state) {
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
void demographicswindow_pop_row_issues_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN pop_row::issues::tooltip
		float total = 0.0f;
		for(auto& i : graph_content)
			total += i.amount;
		auto box = text::open_layout_box(contents);
		text::add_to_layout_box(state, contents, box, state.world.issue_get_name(state.world.issue_option_get_parent_issue(selected_key)));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, state.world.issue_option_get_name(selected_key));
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ total > 0.0f ? graph_content[temp_index].amount / total : 0.0f });
		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_pop_row_issues_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_pop_row_issues_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::issues::update
	graph_content.clear();
	for(auto i : state.world.in_issue_option) {
		auto amount = pop_demographics::get_demo(state, pop_row.value, pop_demographics::to_key(state, i));
		if(amount > 0)
			graph_content.emplace_back(graph_entry{ i, ogl::unpack_color(ogl::get_ui_color(state,i.id)),  amount });
	}
	update_chart(state);
// END
}
void demographicswindow_pop_row_militancy_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::militancy::tooltip
	describe_mil(state, contents, pop_row.value);
// END
}
void demographicswindow_pop_row_militancy_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::militancy::update
	set_text(state, text::format_float(pop_demographics::get_militancy(state, pop_row.value), 1));
// END
}
void demographicswindow_pop_row_consciousness_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::consciousness::tooltip
	alice_ui::describe_con(state, contents, pop_row.value);
// END
}
void demographicswindow_pop_row_consciousness_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::consciousness::update
	set_text(state, text::format_float(pop_demographics::get_consciousness(state, pop_row.value), 1));
// END
}
void demographicswindow_pop_row_employment_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::employment::update
	if(state.world.pop_get_poptype(pop_row.value).get_has_unemployment()) {
		auto sz = state.world.pop_get_size(pop_row.value);
		set_text(state, text::format_percentage(sz > 0 ? pop_demographics::get_employment(state, pop_row.value) / sz : 0.0f, 0));
	} else {
		set_text(state, "-");
	}
// END
}
void demographicswindow_pop_row_literacy_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::literacy::tooltip
	alice_ui::describe_lit(state, contents, pop_row.value);
// END
}
void demographicswindow_pop_row_literacy_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::literacy::update
	set_text(state, text::format_percentage(pop_demographics::get_literacy(state, pop_row.value), 0));
// END
}
void demographicswindow_pop_row_money_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::money::tooltip
	alice_ui::describe_money(state, contents, pop_row.value);
// END
}
void demographicswindow_pop_row_money_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::money::update
	set_text(state, text::prettify_currency(state.world.pop_get_savings(pop_row.value)));
// END
}
void demographicswindow_pop_row_needs_t::on_create(sys::state& state) noexcept {
// BEGIN pop_row::needs::create
// END
}
void demographicswindow_pop_row_needs_t::update_chart(sys::state& state) {
	float total = 0.0f;
	for(auto& e : graph_content) { total += e.amount; }
	if(total <= 0.0f) {
		for(int32_t k = 0; k < 99; k++) {
			data_texture.data[k * 3] = uint8_t(0);
			data_texture.data[k * 3 + 1] = uint8_t(0);
			data_texture.data[k * 3 + 2] = uint8_t(0);
		}
		data_texture.data_updated = true;
		return;
	}
	int32_t index = 0;
	float offset = 0.0f;
	for(int32_t k = 0; k < 99; k++) {
		while(graph_content[index].amount + offset < (float(k) + 0.5f) * total /  float(99)) {
			offset += graph_content[index].amount;
			++index;
		}
		data_texture.data[k * 3] = uint8_t(graph_content[index].color.r * 255.0f);
		data_texture.data[k * 3 + 1] = uint8_t(graph_content[index].color.g * 255.0f);
		data_texture.data[k * 3 + 2] = uint8_t(graph_content[index].color.b * 255.0f);
	}
	data_texture.data_updated = true;
}
void demographicswindow_pop_row_needs_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
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
// BEGIN pop_row::needs::tooltip
		float total = 0.0f;
		auto box = text::open_layout_box(contents);
		if(temp_index == 0 || temp_index == 1) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("life_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[0].amount });
		} else if(temp_index == 2 || temp_index == 3) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("everyday_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[2].amount });
		} else if(temp_index == 4 || temp_index == 5) {
			text::add_to_layout_box(state, contents, box, state.lookup_key("luxury_needs"));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage_one_place{ graph_content[4].amount });
		}

		text::close_layout_box(contents, box);
// END
	}
}
void demographicswindow_pop_row_needs_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(template_id == -1) return;
	alice_ui::layout_window_element* par = static_cast<alice_ui::layout_window_element*>(parent);
	ogl::render_stripchart(state, ogl::color_modification::none, float(x + par->grid_size * state.ui_templates.stacked_bar_t[template_id].l_margin), float(y + par->grid_size * state.ui_templates.stacked_bar_t[template_id].t_margin), float(base_data.size.x - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].l_margin +state.ui_templates.stacked_bar_t[template_id].r_margin)), float(base_data.size.y - par->grid_size *(state.ui_templates.stacked_bar_t[template_id].t_margin +state.ui_templates.stacked_bar_t[template_id].b_margin)), data_texture);
	auto bg_id = state.ui_templates.stacked_bar_t[template_id].overlay_bg;
	if(bg_id != -1)
		ogl::render_textured_rect_direct(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y), state.ui_templates.backgrounds[bg_id].renders.get_render(state, float(base_data.size.x) / float(par->grid_size), float(base_data.size.y) / float(par->grid_size), int32_t(par->grid_size), state.user_settings.ui_scale));
}
void demographicswindow_pop_row_needs_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::needs::update
	float lntotal = pop_demographics::get_life_needs(state, pop_row.value);
	float entotal = pop_demographics::get_everyday_needs(state, pop_row.value);
	float lxtotal = pop_demographics::get_luxury_needs(state, pop_row.value);

	graph_content.clear();
	graph_content.emplace_back(graph_entry{ 0, ogl::color3f{ 130.0f / 255.0f, 162.0f / 255.0f, 217.0f / 255.0f }, lntotal  });
	graph_content.emplace_back(graph_entry{ 1, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f },  1.0f - lntotal });
	graph_content.emplace_back(graph_entry{ 2, ogl::color3f{ 210.0f / 255.0f, 140.0f / 255.0f, 37.0f / 255.0f }, entotal });
	graph_content.emplace_back(graph_entry{ 3, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f },  1.0f - entotal });
	graph_content.emplace_back(graph_entry{ 4, ogl::color3f{ 157.0f / 255.0f, 66.0f / 255.0f, 62.0f / 255.0f },  lxtotal });
	graph_content.emplace_back(graph_entry{ 5, ogl::color3f{ 206.0f / 255.0f, 199.0f / 255.0f, 181.0f / 255.0f }, 1.0f - lxtotal });
	update_chart(state);
// END
}
ui::message_result demographicswindow_pop_row_rebel_flag_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
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
ui::message_result demographicswindow_pop_row_rebel_flag_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pop_row_rebel_flag_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void demographicswindow_pop_row_rebel_flag_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_pop_row_rebel_flag_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::rebel_flag::update
	if(auto rf = state.world.pop_get_rebel_faction_from_pop_rebellion_membership(pop_row.value); rf) {
		flag = rf;
		set_visible(state, true);
		return;
	}
	if(auto m = state.world.pop_get_movement_from_pop_movement_membership(pop_row.value); m) {
		if(auto id = state.world.movement_get_associated_independence(m); id) {
			flag = id;
			set_visible(state, true);
			return;
		}
	}
	set_visible(state, false);
// END
}
void demographicswindow_pop_row_rebel_flag_t::on_create(sys::state& state) noexcept {
// BEGIN pop_row::rebel_flag::create
// END
}
void demographicswindow_pop_row_culture_name_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::culture_name::tooltip
	describe_assimilation(state, contents, pop_row.value);
// END
}
void demographicswindow_pop_row_culture_name_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::culture_name::update
	set_text(state, text::produce_simple_string(state, state.world.pop_get_culture(pop_row.value).get_name()));
// END
}
void demographicswindow_pop_row_job_name_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::job_name::update
	set_text(state, text::produce_simple_string(state, state.world.pop_get_poptype(pop_row.value).get_name()));
// END
}
void demographicswindow_pop_row_religion_name_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::religion_name::update
	set_text(state, text::produce_simple_string(state, state.world.pop_get_religion(pop_row.value).get_name()));
// END
}
void demographicswindow_pop_row_frames_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_spacer = rel_mouse_x >= table_source->table_spacer_column_start && rel_mouse_x < (table_source->table_spacer_column_start + table_source->table_spacer_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_spacer_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(table_source->table_spacer_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(table_source->table_spacer_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_location = rel_mouse_x >= table_source->table_location_column_start && rel_mouse_x < (table_source->table_location_column_start + table_source->table_location_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_location_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(table_source->table_location_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(table_source->table_location_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_size = rel_mouse_x >= table_source->table_size_column_start && rel_mouse_x < (table_source->table_size_column_start + table_source->table_size_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_size_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(table_source->table_size_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(table_source->table_size_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_culture = rel_mouse_x >= table_source->table_culture_column_start && rel_mouse_x < (table_source->table_culture_column_start + table_source->table_culture_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_culture_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(table_source->table_culture_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(table_source->table_culture_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_job = rel_mouse_x >= table_source->table_job_column_start && rel_mouse_x < (table_source->table_job_column_start + table_source->table_job_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_job_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(table_source->table_job_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(table_source->table_job_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_religion = rel_mouse_x >= table_source->table_religion_column_start && rel_mouse_x < (table_source->table_religion_column_start + table_source->table_religion_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_religion_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(table_source->table_religion_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(table_source->table_religion_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_ideology = rel_mouse_x >= table_source->table_ideology_column_start && rel_mouse_x < (table_source->table_ideology_column_start + table_source->table_ideology_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_ideology_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(table_source->table_ideology_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(table_source->table_ideology_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_issues = rel_mouse_x >= table_source->table_issues_column_start && rel_mouse_x < (table_source->table_issues_column_start + table_source->table_issues_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_issues_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(table_source->table_issues_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(table_source->table_issues_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_militancy = rel_mouse_x >= table_source->table_militancy_column_start && rel_mouse_x < (table_source->table_militancy_column_start + table_source->table_militancy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_militancy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(table_source->table_militancy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(table_source->table_militancy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_consciousness = rel_mouse_x >= table_source->table_consciousness_column_start && rel_mouse_x < (table_source->table_consciousness_column_start + table_source->table_consciousness_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_consciousness_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(table_source->table_consciousness_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(table_source->table_consciousness_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_employment = rel_mouse_x >= table_source->table_employment_column_start && rel_mouse_x < (table_source->table_employment_column_start + table_source->table_employment_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(table_source->table_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(table_source->table_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_literacy = rel_mouse_x >= table_source->table_literacy_column_start && rel_mouse_x < (table_source->table_literacy_column_start + table_source->table_literacy_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_literacy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(table_source->table_literacy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(table_source->table_literacy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_money = rel_mouse_x >= table_source->table_money_column_start && rel_mouse_x < (table_source->table_money_column_start + table_source->table_money_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_money_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(table_source->table_money_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(table_source->table_money_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_needs = rel_mouse_x >= table_source->table_needs_column_start && rel_mouse_x < (table_source->table_needs_column_start + table_source->table_needs_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_needs_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(table_source->table_needs_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(table_source->table_needs_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_rebels = rel_mouse_x >= table_source->table_rebels_column_start && rel_mouse_x < (table_source->table_rebels_column_start + table_source->table_rebels_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_rebels_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(table_source->table_rebels_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(table_source->table_rebels_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void demographicswindow_pop_row_frames_t::on_create(sys::state& state) noexcept {
}
ui::message_result demographicswindow_pop_row_movement_political_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pop_row_movement_political_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pop_row_movement_political_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::movement_political::tooltip
	auto m = state.world.pop_get_movement_from_pop_movement_membership(pop_row.value);
	if(m) {
		auto movement_issue = state.world.movement_get_associated_issue_option(m);
		text::add_line(state, contents, movement_issue.get_movement_name());
	}
// END
}
void demographicswindow_pop_row_movement_political_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_pop_row_movement_political_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::movement_political::update
	if(auto m = state.world.pop_get_movement_from_pop_movement_membership(pop_row.value); m) {
		auto parent_issue = state.world.movement_get_associated_issue_option(m).get_parent_issue();
		if(parent_issue.get_issue_type() == uint8_t(culture::issue_category::political)) {
			int32_t count = 0;
			for(; count < int32_t(state.culture_definitions.political_issues.size()); ++count) {
				if(state.culture_definitions.political_issues[count] == parent_issue)
					break;
			}
			set_visible(state, true);
			frame = count;
		}
	}
	set_visible(state, false);
// END
}
void demographicswindow_pop_row_movement_political_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN pop_row::movement_political::create
// END
}
ui::message_result demographicswindow_pop_row_movement_social_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pop_row_movement_social_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pop_row_movement_social_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::movement_social::tooltip
	auto m = state.world.pop_get_movement_from_pop_movement_membership(pop_row.value);
	if(m) {
		auto movement_issue = state.world.movement_get_associated_issue_option(m);
		text::add_line(state, contents, movement_issue.get_movement_name());
	}
// END
}
void demographicswindow_pop_row_movement_social_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_pop_row_movement_social_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::movement_social::update
	if(auto m = state.world.pop_get_movement_from_pop_movement_membership(pop_row.value); m) {
		auto parent_issue = state.world.movement_get_associated_issue_option(m).get_parent_issue();
		if(parent_issue.get_issue_type() == uint8_t(culture::issue_category::social)) {
			int32_t count = 0;
			for(; count < int32_t(state.culture_definitions.social_issues.size()); ++count) {
				if(state.culture_definitions.social_issues[count] == parent_issue)
					break;
			}
			set_visible(state, true);
			frame = count;
		}
	}
	set_visible(state, false);
// END
}
void demographicswindow_pop_row_movement_social_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN pop_row::movement_social::create
// END
}
void demographicswindow_pop_row_details_button_t::on_update(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::details_button::update
// END
}
bool demographicswindow_pop_row_details_button_t::button_action(sys::state& state) noexcept {
	demographicswindow_pop_row_t& pop_row = *((demographicswindow_pop_row_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::details_button::lbutton_action
	assert(main.details_w);
	auto pop_ptr =(dcon::pop_id*)main.details_w->get_by_name(state, "for_pop");
	*pop_ptr = pop_row.value;

	if(!main.details_w->is_visible())
		main.details_w->set_visible(state, true);
	else
		main.details_w->impl_on_update(state);
	main.details_w->parent->move_child_to_front(main.details_w);
// END
	return true;
}
void  demographicswindow_pop_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result demographicswindow_pop_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_pop_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_pop_row_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pop_row::update
// END
	remake_layout(state, true);
}
void demographicswindow_pop_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "location") {
					temp.ptr = location.get();
				} else
				if(cname == "size") {
					temp.ptr = size.get();
				} else
				if(cname == "size_trend") {
					temp.ptr = size_trend.get();
				} else
				if(cname == "culture") {
					temp.ptr = culture.get();
				} else
				if(cname == "job") {
					temp.ptr = job.get();
				} else
				if(cname == "religion") {
					temp.ptr = religion.get();
				} else
				if(cname == "ideology") {
					temp.ptr = ideology.get();
				} else
				if(cname == "issues") {
					temp.ptr = issues.get();
				} else
				if(cname == "militancy") {
					temp.ptr = militancy.get();
				} else
				if(cname == "consciousness") {
					temp.ptr = consciousness.get();
				} else
				if(cname == "employment") {
					temp.ptr = employment.get();
				} else
				if(cname == "literacy") {
					temp.ptr = literacy.get();
				} else
				if(cname == "money") {
					temp.ptr = money.get();
				} else
				if(cname == "needs") {
					temp.ptr = needs.get();
				} else
				if(cname == "rebel_flag") {
					temp.ptr = rebel_flag.get();
				} else
				if(cname == "culture_name") {
					temp.ptr = culture_name.get();
				} else
				if(cname == "job_name") {
					temp.ptr = job_name.get();
				} else
				if(cname == "religion_name") {
					temp.ptr = religion_name.get();
				} else
				if(cname == "frames") {
					temp.ptr = frames.get();
				} else
				if(cname == "movement_political") {
					temp.ptr = movement_political.get();
				} else
				if(cname == "movement_social") {
					temp.ptr = movement_social.get();
				} else
				if(cname == "details_button") {
					temp.ptr = details_button.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_pop_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::pop_row"));
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
		if(child_data.name == "location") {
			location = std::make_unique<demographicswindow_pop_row_location_t>();
			location->parent = this;
			auto cptr = location.get();
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
		if(child_data.name == "size") {
			size = std::make_unique<demographicswindow_pop_row_size_t>();
			size->parent = this;
			auto cptr = size.get();
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
		if(child_data.name == "size_trend") {
			size_trend = std::make_unique<demographicswindow_pop_row_size_trend_t>();
			size_trend->parent = this;
			auto cptr = size_trend.get();
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
		if(child_data.name == "culture") {
			culture = std::make_unique<demographicswindow_pop_row_culture_t>();
			culture->parent = this;
			auto cptr = culture.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->color = child_data.table_highlight_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "job") {
			job = std::make_unique<demographicswindow_pop_row_job_t>();
			job->parent = this;
			auto cptr = job.get();
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
		if(child_data.name == "religion") {
			religion = std::make_unique<demographicswindow_pop_row_religion_t>();
			religion->parent = this;
			auto cptr = religion.get();
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
		if(child_data.name == "ideology") {
			ideology = std::make_unique<demographicswindow_pop_row_ideology_t>();
			ideology->parent = this;
			auto cptr = ideology.get();
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
		if(child_data.name == "issues") {
			issues = std::make_unique<demographicswindow_pop_row_issues_t>();
			issues->parent = this;
			auto cptr = issues.get();
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
		if(child_data.name == "militancy") {
			militancy = std::make_unique<demographicswindow_pop_row_militancy_t>();
			militancy->parent = this;
			auto cptr = militancy.get();
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
		if(child_data.name == "consciousness") {
			consciousness = std::make_unique<demographicswindow_pop_row_consciousness_t>();
			consciousness->parent = this;
			auto cptr = consciousness.get();
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
		if(child_data.name == "employment") {
			employment = std::make_unique<demographicswindow_pop_row_employment_t>();
			employment->parent = this;
			auto cptr = employment.get();
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
		if(child_data.name == "literacy") {
			literacy = std::make_unique<demographicswindow_pop_row_literacy_t>();
			literacy->parent = this;
			auto cptr = literacy.get();
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
		if(child_data.name == "money") {
			money = std::make_unique<demographicswindow_pop_row_money_t>();
			money->parent = this;
			auto cptr = money.get();
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
		if(child_data.name == "needs") {
			needs = std::make_unique<demographicswindow_pop_row_needs_t>();
			needs->parent = this;
			auto cptr = needs.get();
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
		if(child_data.name == "rebel_flag") {
			rebel_flag = std::make_unique<demographicswindow_pop_row_rebel_flag_t>();
			rebel_flag->parent = this;
			auto cptr = rebel_flag.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "culture_name") {
			culture_name = std::make_unique<demographicswindow_pop_row_culture_name_t>();
			culture_name->parent = this;
			auto cptr = culture_name.get();
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
		if(child_data.name == "job_name") {
			job_name = std::make_unique<demographicswindow_pop_row_job_name_t>();
			job_name->parent = this;
			auto cptr = job_name.get();
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
		if(child_data.name == "religion_name") {
			religion_name = std::make_unique<demographicswindow_pop_row_religion_name_t>();
			religion_name->parent = this;
			auto cptr = religion_name.get();
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
		if(child_data.name == "frames") {
			frames = std::make_unique<demographicswindow_pop_row_frames_t>();
			frames->parent = this;
			auto cptr = frames.get();
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
		if(child_data.name == "movement_political") {
			movement_political = std::make_unique<demographicswindow_pop_row_movement_political_t>();
			movement_political->parent = this;
			auto cptr = movement_political.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->gfx_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "movement_social") {
			movement_social = std::make_unique<demographicswindow_pop_row_movement_social_t>();
			movement_social->parent = this;
			auto cptr = movement_social.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->gfx_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "details_button") {
			details_button = std::make_unique<demographicswindow_pop_row_details_button_t>();
			details_button->parent = this;
			auto cptr = details_button.get();
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
// BEGIN pop_row::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_pop_row(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_pop_row_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_filters_window_jobs_list_t::add_job_filter(dcon::pop_type_id value) {
	values.emplace_back(job_filter_option{value});
}
void  demographicswindow_filters_window_jobs_list_t::on_create(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::jobs_list::on_create
	for(auto j : state.world.in_pop_type)
		add_job_filter(j.id);
	std::sort(values.begin(), values.end(), [&](auto& a, auto& b) {
		return text::produce_simple_string(state, state.world.pop_type_get_name(get<job_filter_option>(a).value)) < text::produce_simple_string(state, state.world.pop_type_get_name(get<job_filter_option>(b).value));
	});
// END
}
void  demographicswindow_filters_window_jobs_list_t::update(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::jobs_list::update
// END
}
measure_result  demographicswindow_filters_window_jobs_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<job_filter_option>(values[index])) {
		if(job_filter_pool.empty()) job_filter_pool.emplace_back(make_demographicswindow_job_filter(state));
		if(destination) {
			if(job_filter_pool.size() <= size_t(job_filter_pool_used)) job_filter_pool.emplace_back(make_demographicswindow_job_filter(state));
			job_filter_pool[job_filter_pool_used]->base_data.position.x = int16_t(x);
			job_filter_pool[job_filter_pool_used]->base_data.position.y = int16_t(y);
			job_filter_pool[job_filter_pool_used]->parent = destination;
			destination->children.push_back(job_filter_pool[job_filter_pool_used].get());
			((demographicswindow_job_filter_t*)(job_filter_pool[job_filter_pool_used].get()))->value = std::get<job_filter_option>(values[index]).value;
			((demographicswindow_job_filter_t*)(job_filter_pool[job_filter_pool_used].get()))->set_alternate(alternate);
			job_filter_pool[job_filter_pool_used]->impl_on_update(state);
			job_filter_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ job_filter_pool[0]->base_data.size.x, job_filter_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  demographicswindow_filters_window_jobs_list_t::reset_pools() {
	job_filter_pool_used = 0;
}
void demographicswindow_filters_window_culture_list_t::add_culture_filter(dcon::culture_id value) {
	values.emplace_back(culture_filter_option{value});
}
void  demographicswindow_filters_window_culture_list_t::on_create(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::culture_list::on_create
	for(auto c : state.world.in_culture) {
		add_culture_filter(c.id);
	}
	std::sort(values.begin(), values.end(), [&](auto& a, auto& b) {
		return text::produce_simple_string(state, state.world.culture_get_name(get<culture_filter_option>(a).value)) < text::produce_simple_string(state, state.world.culture_get_name(get<culture_filter_option>(b).value));
	});
// END
}
void  demographicswindow_filters_window_culture_list_t::update(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::culture_list::update
// END
}
measure_result  demographicswindow_filters_window_culture_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<culture_filter_option>(values[index])) {
		if(culture_filter_pool.empty()) culture_filter_pool.emplace_back(make_demographicswindow_culture_filter(state));
		if(destination) {
			if(culture_filter_pool.size() <= size_t(culture_filter_pool_used)) culture_filter_pool.emplace_back(make_demographicswindow_culture_filter(state));
			culture_filter_pool[culture_filter_pool_used]->base_data.position.x = int16_t(x);
			culture_filter_pool[culture_filter_pool_used]->base_data.position.y = int16_t(y);
			culture_filter_pool[culture_filter_pool_used]->parent = destination;
			destination->children.push_back(culture_filter_pool[culture_filter_pool_used].get());
			((demographicswindow_culture_filter_t*)(culture_filter_pool[culture_filter_pool_used].get()))->value = std::get<culture_filter_option>(values[index]).value;
			((demographicswindow_culture_filter_t*)(culture_filter_pool[culture_filter_pool_used].get()))->set_alternate(alternate);
			culture_filter_pool[culture_filter_pool_used]->impl_on_update(state);
			culture_filter_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ culture_filter_pool[0]->base_data.size.x, culture_filter_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  demographicswindow_filters_window_culture_list_t::reset_pools() {
	culture_filter_pool_used = 0;
}
void demographicswindow_filters_window_religion_list_t::add_religion_filter(dcon::religion_id value) {
	values.emplace_back(religion_filter_option{value});
}
void  demographicswindow_filters_window_religion_list_t::on_create(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::religion_list::on_create
	for(auto r : state.world.in_religion)
		add_religion_filter(r.id);
	std::sort(values.begin(), values.end(), [&](auto& a, auto& b) {
		return text::produce_simple_string(state, state.world.religion_get_name(get<religion_filter_option>(a).value)) < text::produce_simple_string(state, state.world.religion_get_name(get<religion_filter_option>(b).value));
	});
// END
}
void  demographicswindow_filters_window_religion_list_t::update(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::religion_list::update
// END
}
measure_result  demographicswindow_filters_window_religion_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<religion_filter_option>(values[index])) {
		if(religion_filter_pool.empty()) religion_filter_pool.emplace_back(make_demographicswindow_religion_filter(state));
		if(destination) {
			if(religion_filter_pool.size() <= size_t(religion_filter_pool_used)) religion_filter_pool.emplace_back(make_demographicswindow_religion_filter(state));
			religion_filter_pool[religion_filter_pool_used]->base_data.position.x = int16_t(x);
			religion_filter_pool[religion_filter_pool_used]->base_data.position.y = int16_t(y);
			religion_filter_pool[religion_filter_pool_used]->parent = destination;
			destination->children.push_back(religion_filter_pool[religion_filter_pool_used].get());
			((demographicswindow_religion_filter_t*)(religion_filter_pool[religion_filter_pool_used].get()))->value = std::get<religion_filter_option>(values[index]).value;
			((demographicswindow_religion_filter_t*)(religion_filter_pool[religion_filter_pool_used].get()))->set_alternate(alternate);
			religion_filter_pool[religion_filter_pool_used]->impl_on_update(state);
			religion_filter_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ religion_filter_pool[0]->base_data.size.x, religion_filter_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  demographicswindow_filters_window_religion_list_t::reset_pools() {
	religion_filter_pool_used = 0;
}
void demographicswindow_filters_window_location_list_t::add_location_filter(dcon::state_instance_id value) {
	values.emplace_back(location_filter_option{value});
}
void  demographicswindow_filters_window_location_list_t::on_create(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::location_list::on_create
// END
}
void  demographicswindow_filters_window_location_list_t::update(sys::state& state, layout_window_element* parent) {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::location_list::update
	values.clear();
	for(auto s : state.world.nation_get_state_ownership(state.local_player_nation)) {
		add_location_filter(s.get_state().id);
	}
	sys::merge_sort(values.begin(), values.end(), [&](auto& a, auto& b) {
		return text::get_short_state_name(state, get<location_filter_option>(a).value) < text::get_short_state_name(state, get<location_filter_option>(b).value);
	});
// END
}
measure_result  demographicswindow_filters_window_location_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<location_filter_option>(values[index])) {
		if(location_filter_pool.empty()) location_filter_pool.emplace_back(make_demographicswindow_location_filter(state));
		if(destination) {
			if(location_filter_pool.size() <= size_t(location_filter_pool_used)) location_filter_pool.emplace_back(make_demographicswindow_location_filter(state));
			location_filter_pool[location_filter_pool_used]->base_data.position.x = int16_t(x);
			location_filter_pool[location_filter_pool_used]->base_data.position.y = int16_t(y);
			location_filter_pool[location_filter_pool_used]->parent = destination;
			destination->children.push_back(location_filter_pool[location_filter_pool_used].get());
			((demographicswindow_location_filter_t*)(location_filter_pool[location_filter_pool_used].get()))->value = std::get<location_filter_option>(values[index]).value;
			((demographicswindow_location_filter_t*)(location_filter_pool[location_filter_pool_used].get()))->set_alternate(alternate);
			location_filter_pool[location_filter_pool_used]->impl_on_update(state);
			location_filter_pool_used++;
		}
		alternate = !alternate;
		return measure_result{ location_filter_pool[0]->base_data.size.x, location_filter_pool[0]->base_data.size.y + 0, measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  demographicswindow_filters_window_location_list_t::reset_pools() {
	location_filter_pool_used = 0;
}
void demographicswindow_filters_window_job_all_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::job_all::update
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(popwindow::excluded_types.empty() ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_job_all_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::job_all::lbutton_action
	popwindow::excluded_types.clear();
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_job_none_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::job_none::update
	bool is_active = (popwindow::excluded_types.size() == state.world.pop_type_size());
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_job_none_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::job_none::lbutton_action
	for(auto j : state.world.in_pop_type) {
		popwindow::excluded_types.insert(j.id.index());
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_culture_all_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::culture_all::update
	bool is_active = popwindow::excluded_cultures.empty();
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_culture_all_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::culture_all::lbutton_action
	popwindow::excluded_cultures.clear();
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_culture_none_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::culture_none::update
	bool is_active = popwindow::excluded_cultures.size() == state.world.culture_size();
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_culture_none_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::culture_none::lbutton_action
	for(auto c : state.world.in_culture) {
		popwindow::excluded_cultures.insert(c.id.index());
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_religion_all_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::religion_all::update
	bool is_active = popwindow::excluded_religions.empty();
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_religion_all_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::religion_all::lbutton_action
	popwindow::excluded_religions.clear();
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_religion_none_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::religion_none::update
	bool is_active = popwindow::excluded_religions.size() == state.world.religion_size();
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_religion_none_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::religion_none::lbutton_action
	for(auto r : state.world.in_religion)
		popwindow::excluded_religions.insert(r.id.index());
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_location_all_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::location_all::update
	bool is_active = popwindow::excluded_states.empty();
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_location_all_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::location_all::lbutton_action
	popwindow::excluded_states.clear();
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_location_none_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::location_none::update
	auto orange = state.world.nation_get_state_ownership(state.local_player_nation);
	bool is_active = popwindow::excluded_states.size() == size_t(orange.end() - orange.begin());
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_location_none_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::location_none::lbutton_action
	for(auto s : state.world.nation_get_state_ownership(state.local_player_nation)) {
		popwindow::excluded_states.insert(s.get_state().id.index());
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_non_colonial_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::non_colonial::update
	bool is_active = popwindow::show_non_colonial;
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_non_colonial_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::non_colonial::lbutton_action
	popwindow::show_non_colonial = !popwindow::show_non_colonial;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void demographicswindow_filters_window_colonial_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::colonial::update
	bool is_active = popwindow::show_colonial;
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_filters_window_colonial_t::button_action(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent)); 
// BEGIN filters_window::colonial::lbutton_action
	popwindow::show_colonial = !popwindow::show_colonial;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
ui::message_result demographicswindow_filters_window_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_filters_window_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_filters_window_t::on_update(sys::state& state) noexcept {
// BEGIN filters_window::update
// END
	jobs_list.update(state, this);
	culture_list.update(state, this);
	religion_list.update(state, this);
	location_list.update(state, this);
	remake_layout(state, true);
}
void demographicswindow_filters_window_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "job_all") {
					temp.ptr = job_all.get();
				} else
				if(cname == "job_none") {
					temp.ptr = job_none.get();
				} else
				if(cname == "culture_all") {
					temp.ptr = culture_all.get();
				} else
				if(cname == "culture_none") {
					temp.ptr = culture_none.get();
				} else
				if(cname == "religion_all") {
					temp.ptr = religion_all.get();
				} else
				if(cname == "religion_none") {
					temp.ptr = religion_none.get();
				} else
				if(cname == "location_all") {
					temp.ptr = location_all.get();
				} else
				if(cname == "location_none") {
					temp.ptr = location_none.get();
				} else
				if(cname == "non_colonial") {
					temp.ptr = non_colonial.get();
				} else
				if(cname == "colonial") {
					temp.ptr = colonial.get();
				} else
				if(cname == "job_header") {
					temp.ptr = job_header.get();
				} else
				if(cname == "culture_header") {
					temp.ptr = culture_header.get();
				} else
				if(cname == "religion_header") {
					temp.ptr = religion_header.get();
				} else
				if(cname == "location_header") {
					temp.ptr = location_header.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
				if(cname == "jobs_list") {
					temp.generator = &jobs_list;
				}
				if(cname == "culture_list") {
					temp.generator = &culture_list;
				}
				if(cname == "religion_list") {
					temp.generator = &religion_list;
				}
				if(cname == "location_list") {
					temp.generator = &location_list;
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
void demographicswindow_filters_window_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::filters_window"));
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
		if(child_data.name == "job_all") {
			job_all = std::make_unique<demographicswindow_filters_window_job_all_t>();
			job_all->parent = this;
			auto cptr = job_all.get();
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
		if(child_data.name == "job_none") {
			job_none = std::make_unique<demographicswindow_filters_window_job_none_t>();
			job_none->parent = this;
			auto cptr = job_none.get();
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
		if(child_data.name == "culture_all") {
			culture_all = std::make_unique<demographicswindow_filters_window_culture_all_t>();
			culture_all->parent = this;
			auto cptr = culture_all.get();
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
		if(child_data.name == "culture_none") {
			culture_none = std::make_unique<demographicswindow_filters_window_culture_none_t>();
			culture_none->parent = this;
			auto cptr = culture_none.get();
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
		if(child_data.name == "religion_all") {
			religion_all = std::make_unique<demographicswindow_filters_window_religion_all_t>();
			religion_all->parent = this;
			auto cptr = religion_all.get();
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
		if(child_data.name == "religion_none") {
			religion_none = std::make_unique<demographicswindow_filters_window_religion_none_t>();
			religion_none->parent = this;
			auto cptr = religion_none.get();
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
		if(child_data.name == "location_all") {
			location_all = std::make_unique<demographicswindow_filters_window_location_all_t>();
			location_all->parent = this;
			auto cptr = location_all.get();
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
		if(child_data.name == "location_none") {
			location_none = std::make_unique<demographicswindow_filters_window_location_none_t>();
			location_none->parent = this;
			auto cptr = location_none.get();
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
		if(child_data.name == "non_colonial") {
			non_colonial = std::make_unique<demographicswindow_filters_window_non_colonial_t>();
			non_colonial->parent = this;
			auto cptr = non_colonial.get();
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
		if(child_data.name == "colonial") {
			colonial = std::make_unique<demographicswindow_filters_window_colonial_t>();
			colonial->parent = this;
			auto cptr = colonial.get();
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
	jobs_list.on_create(state, this);
	culture_list.on_create(state, this);
	religion_list.on_create(state, this);
	location_list.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN filters_window::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_filters_window(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_filters_window_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_job_filter_filter_button_t::on_update(sys::state& state) noexcept {
	demographicswindow_job_filter_t& job_filter = *((demographicswindow_job_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN job_filter::filter_button::update
	bool is_active = !popwindow::excluded_types.contains(job_filter.value.index());
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_job_filter_filter_button_t::button_action(sys::state& state) noexcept {
	demographicswindow_job_filter_t& job_filter = *((demographicswindow_job_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN job_filter::filter_button::lbutton_action
	if(popwindow::excluded_types.contains(job_filter.value.index()))
		popwindow::excluded_types.erase(job_filter.value.index());
	else
		popwindow::excluded_types.insert(job_filter.value.index());
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
ui::message_result demographicswindow_job_filter_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_job_filter_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_job_filter_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_job_filter_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_job_filter_t& job_filter = *((demographicswindow_job_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN job_filter::icon::update
	frame = int32_t(state.world.pop_type_get_sprite(job_filter.value)) - 1;
// END
}
void demographicswindow_job_filter_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN job_filter::icon::create
// END
}
void demographicswindow_job_filter_filter_label_t::on_update(sys::state& state) noexcept {
	demographicswindow_job_filter_t& job_filter = *((demographicswindow_job_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN job_filter::filter_label::update
	set_text(state, text::produce_simple_string(state, state.world.pop_type_get_name(job_filter.value)));
// END
}
void  demographicswindow_job_filter_t::set_alternate(bool alt) noexcept {
	window_template = alt ? -1 : 2;
	filter_button->template_id = alt ? 3 : 4;
}
ui::message_result demographicswindow_job_filter_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_job_filter_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_job_filter_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN job_filter::update
// END
	remake_layout(state, true);
}
void demographicswindow_job_filter_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "filter_button") {
					temp.ptr = filter_button.get();
				} else
				if(cname == "icon") {
					temp.ptr = icon.get();
				} else
				if(cname == "filter_label") {
					temp.ptr = filter_label.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_job_filter_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::job_filter"));
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
		if(child_data.name == "filter_button") {
			filter_button = std::make_unique<demographicswindow_job_filter_filter_button_t>();
			filter_button->parent = this;
			auto cptr = filter_button.get();
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
		if(child_data.name == "icon") {
			icon = std::make_unique<demographicswindow_job_filter_icon_t>();
			icon->parent = this;
			auto cptr = icon.get();
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
		if(child_data.name == "filter_label") {
			filter_label = std::make_unique<demographicswindow_job_filter_filter_label_t>();
			filter_label->parent = this;
			auto cptr = filter_label.get();
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
// BEGIN job_filter::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_job_filter(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_job_filter_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_religion_filter_filter_button_t::on_update(sys::state& state) noexcept {
	demographicswindow_religion_filter_t& religion_filter = *((demographicswindow_religion_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN religion_filter::filter_button::update
	bool is_active = !popwindow::excluded_religions.contains(religion_filter.value.index());
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_religion_filter_filter_button_t::button_action(sys::state& state) noexcept {
	demographicswindow_religion_filter_t& religion_filter = *((demographicswindow_religion_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN religion_filter::filter_button::lbutton_action
	if(popwindow::excluded_religions.contains(religion_filter.value.index()))
		popwindow::excluded_religions.erase(religion_filter.value.index());
	else
		popwindow::excluded_religions.insert(religion_filter.value.index());
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
ui::message_result demographicswindow_religion_filter_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_religion_filter_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_religion_filter_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void demographicswindow_religion_filter_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_religion_filter_t& religion_filter = *((demographicswindow_religion_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN religion_filter::icon::update
	frame = int32_t(state.world.religion_get_icon(religion_filter.value)) - 1;
// END
}
void demographicswindow_religion_filter_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN religion_filter::icon::create
// END
}
void demographicswindow_religion_filter_filter_label_t::on_update(sys::state& state) noexcept {
	demographicswindow_religion_filter_t& religion_filter = *((demographicswindow_religion_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN religion_filter::filter_label::update
	set_text(state, text::produce_simple_string(state, state.world.religion_get_name(religion_filter.value)));
// END
}
void  demographicswindow_religion_filter_t::set_alternate(bool alt) noexcept {
	window_template = alt ? -1 : 2;
	filter_button->template_id = alt ? 3 : 4;
}
ui::message_result demographicswindow_religion_filter_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_religion_filter_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_religion_filter_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN religion_filter::update
// END
	remake_layout(state, true);
}
void demographicswindow_religion_filter_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "filter_button") {
					temp.ptr = filter_button.get();
				} else
				if(cname == "icon") {
					temp.ptr = icon.get();
				} else
				if(cname == "filter_label") {
					temp.ptr = filter_label.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_religion_filter_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::religion_filter"));
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
		if(child_data.name == "filter_button") {
			filter_button = std::make_unique<demographicswindow_religion_filter_filter_button_t>();
			filter_button->parent = this;
			auto cptr = filter_button.get();
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
		if(child_data.name == "icon") {
			icon = std::make_unique<demographicswindow_religion_filter_icon_t>();
			icon->parent = this;
			auto cptr = icon.get();
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
		if(child_data.name == "filter_label") {
			filter_label = std::make_unique<demographicswindow_religion_filter_filter_label_t>();
			filter_label->parent = this;
			auto cptr = filter_label.get();
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
// BEGIN religion_filter::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_religion_filter(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_religion_filter_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_location_filter_filter_button_t::on_update(sys::state& state) noexcept {
	demographicswindow_location_filter_t& location_filter = *((demographicswindow_location_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN location_filter::filter_button::update
	set_text(state, text::get_short_state_name(state, location_filter.value));
	bool is_active = !popwindow::excluded_states.contains(location_filter.value.index());
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(is_active ? select_icon : un_select_icon);
// END
}
bool demographicswindow_location_filter_filter_button_t::button_action(sys::state& state) noexcept {
	demographicswindow_location_filter_t& location_filter = *((demographicswindow_location_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN location_filter::filter_button::lbutton_action
	if(popwindow::excluded_states.contains(location_filter.value.index()))
		popwindow::excluded_states.erase(location_filter.value.index());
	else
		popwindow::excluded_states.insert(location_filter.value.index());
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void  demographicswindow_location_filter_t::set_alternate(bool alt) noexcept {
	window_template = alt ? -1 : 2;
	filter_button->template_id = alt ? 3 : 4;
}
ui::message_result demographicswindow_location_filter_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_location_filter_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_location_filter_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN location_filter::update
// END
	remake_layout(state, true);
}
void demographicswindow_location_filter_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "filter_button") {
					temp.ptr = filter_button.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_location_filter_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::location_filter"));
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
		if(child_data.name == "filter_button") {
			filter_button = std::make_unique<demographicswindow_location_filter_filter_button_t>();
			filter_button->parent = this;
			auto cptr = filter_button.get();
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
// BEGIN location_filter::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_location_filter(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_location_filter_t>();
	ptr->on_create(state);
	return ptr;
}
void demographicswindow_culture_filter_filter_button_t::on_update(sys::state& state) noexcept {
	demographicswindow_culture_filter_t& culture_filter = *((demographicswindow_culture_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN culture_filter::filter_button::update
	set_text(state, text::produce_simple_string(state, state.world.culture_get_name(culture_filter.value)));
	static const int32_t select_icon = template_project::icon_by_name(state.ui_templates, "filled_circle.svg");
	static const int32_t un_select_icon = template_project::icon_by_name(state.ui_templates, "empty_circle.svg");
	icon_id = int16_t(!popwindow::excluded_cultures.contains(culture_filter.value.index()) ? select_icon : un_select_icon);
// END
}
bool demographicswindow_culture_filter_filter_button_t::button_action(sys::state& state) noexcept {
	demographicswindow_culture_filter_t& culture_filter = *((demographicswindow_culture_filter_t*)(parent)); 
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN culture_filter::filter_button::lbutton_action
	if(popwindow::excluded_cultures.contains(culture_filter.value.index()))
		popwindow::excluded_cultures.erase(culture_filter.value.index());
	else
		popwindow::excluded_cultures.insert(culture_filter.value.index());
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void  demographicswindow_culture_filter_t::set_alternate(bool alt) noexcept {
	window_template = alt ? -1 : 2;
	filter_button->template_id = alt ? 3 : 4;
}
ui::message_result demographicswindow_culture_filter_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_culture_filter_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_culture_filter_t::on_update(sys::state& state) noexcept {
	demographicswindow_filters_window_t& filters_window = *((demographicswindow_filters_window_t*)(parent->parent)); 
// BEGIN culture_filter::update
// END
	remake_layout(state, true);
}
void demographicswindow_culture_filter_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "filter_button") {
					temp.ptr = filter_button.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_culture_filter_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::culture_filter"));
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
		if(child_data.name == "filter_button") {
			filter_button = std::make_unique<demographicswindow_culture_filter_filter_button_t>();
			filter_button->parent = this;
			auto cptr = filter_button.get();
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
// BEGIN culture_filter::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_culture_filter(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_culture_filter_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result demographicswindow_pops_header_content_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	if(x >= table_source->table_location_column_start && x < table_source->table_location_column_start + table_source->table_location_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_location_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_location_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_size_column_start && x < table_source->table_size_column_start + table_source->table_size_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_size_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_size_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_culture_column_start && x < table_source->table_culture_column_start + table_source->table_culture_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_culture_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_culture_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_job_column_start && x < table_source->table_job_column_start + table_source->table_job_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_job_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_job_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_religion_column_start && x < table_source->table_religion_column_start + table_source->table_religion_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_religion_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_religion_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_militancy_column_start && x < table_source->table_militancy_column_start + table_source->table_militancy_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_militancy_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_militancy_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_consciousness_column_start && x < table_source->table_consciousness_column_start + table_source->table_consciousness_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_consciousness_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_consciousness_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_employment_column_start && x < table_source->table_employment_column_start + table_source->table_employment_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_employment_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_employment_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_literacy_column_start && x < table_source->table_literacy_column_start + table_source->table_literacy_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_literacy_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_literacy_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_money_column_start && x < table_source->table_money_column_start + table_source->table_money_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_money_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_money_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->table_needs_column_start && x < table_source->table_needs_column_start + table_source->table_needs_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->table_needs_sort_direction;
		table_source->table_location_sort_direction = 0;
		table_source->table_size_sort_direction = 0;
		table_source->table_culture_sort_direction = 0;
		table_source->table_job_sort_direction = 0;
		table_source->table_religion_sort_direction = 0;
		table_source->table_militancy_sort_direction = 0;
		table_source->table_consciousness_sort_direction = 0;
		table_source->table_employment_sort_direction = 0;
		table_source->table_literacy_sort_direction = 0;
		table_source->table_money_sort_direction = 0;
		table_source->table_needs_sort_direction = 0;
		table_source->table_needs_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result demographicswindow_pops_header_content_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_pops_header_content_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	if(x >= table_source->table_location_column_start && x < table_source->table_location_column_start + table_source->table_location_column_width) {
	}
	if(x >= table_source->table_size_column_start && x < table_source->table_size_column_start + table_source->table_size_column_width) {
	}
	if(x >= table_source->table_culture_column_start && x < table_source->table_culture_column_start + table_source->table_culture_column_width) {
	}
	if(x >= table_source->table_job_column_start && x < table_source->table_job_column_start + table_source->table_job_column_width) {
	}
	if(x >= table_source->table_religion_column_start && x < table_source->table_religion_column_start + table_source->table_religion_column_width) {
	}
	if(x >= table_source->table_ideology_column_start && x < table_source->table_ideology_column_start + table_source->table_ideology_column_width) {
	}
	if(x >= table_source->table_issues_column_start && x < table_source->table_issues_column_start + table_source->table_issues_column_width) {
	}
	if(x >= table_source->table_militancy_column_start && x < table_source->table_militancy_column_start + table_source->table_militancy_column_width) {
		ident = 7;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->table_militancy_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->table_militancy_column_width);
		return;
	}
	if(x >= table_source->table_consciousness_column_start && x < table_source->table_consciousness_column_start + table_source->table_consciousness_column_width) {
		ident = 8;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->table_consciousness_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->table_consciousness_column_width);
		return;
	}
	if(x >= table_source->table_employment_column_start && x < table_source->table_employment_column_start + table_source->table_employment_column_width) {
		ident = 9;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->table_employment_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->table_employment_column_width);
		return;
	}
	if(x >= table_source->table_literacy_column_start && x < table_source->table_literacy_column_start + table_source->table_literacy_column_width) {
		ident = 10;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->table_literacy_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->table_literacy_column_width);
		return;
	}
	if(x >= table_source->table_money_column_start && x < table_source->table_money_column_start + table_source->table_money_column_width) {
		ident = 11;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->table_money_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->table_money_column_width);
		return;
	}
	if(x >= table_source->table_needs_column_start && x < table_source->table_needs_column_start + table_source->table_needs_column_width) {
	}
	if(x >= table_source->table_rebels_column_start && x < table_source->table_rebels_column_start + table_source->table_rebels_column_width) {
		ident = 13;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.top_left.x += int16_t(table_source->table_rebels_column_start);
		subrect.size = base_data.size;
		subrect.size.x = int16_t(table_source->table_rebels_column_width);
		return;
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void demographicswindow_pops_header_content_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	if(x >=  table_source->table_location_column_start && x <  table_source->table_location_column_start +  table_source->table_location_column_width) {
	}
	if(x >=  table_source->table_size_column_start && x <  table_source->table_size_column_start +  table_source->table_size_column_width) {
	}
	if(x >=  table_source->table_culture_column_start && x <  table_source->table_culture_column_start +  table_source->table_culture_column_width) {
	}
	if(x >=  table_source->table_job_column_start && x <  table_source->table_job_column_start +  table_source->table_job_column_width) {
	}
	if(x >=  table_source->table_religion_column_start && x <  table_source->table_religion_column_start +  table_source->table_religion_column_width) {
	}
	if(x >=  table_source->table_ideology_column_start && x <  table_source->table_ideology_column_start +  table_source->table_ideology_column_width) {
	}
	if(x >=  table_source->table_issues_column_start && x <  table_source->table_issues_column_start +  table_source->table_issues_column_width) {
	}
	if(x >=  table_source->table_militancy_column_start && x <  table_source->table_militancy_column_start +  table_source->table_militancy_column_width) {
	text::add_line(state, contents, table_source->table_militancy_header_tooltip_key);
	}
	if(x >=  table_source->table_consciousness_column_start && x <  table_source->table_consciousness_column_start +  table_source->table_consciousness_column_width) {
	text::add_line(state, contents, table_source->table_consciousness_header_tooltip_key);
	}
	if(x >=  table_source->table_employment_column_start && x <  table_source->table_employment_column_start +  table_source->table_employment_column_width) {
	text::add_line(state, contents, table_source->table_employment_header_tooltip_key);
	}
	if(x >=  table_source->table_literacy_column_start && x <  table_source->table_literacy_column_start +  table_source->table_literacy_column_width) {
	text::add_line(state, contents, table_source->table_literacy_header_tooltip_key);
	}
	if(x >=  table_source->table_money_column_start && x <  table_source->table_money_column_start +  table_source->table_money_column_width) {
	text::add_line(state, contents, table_source->table_money_header_tooltip_key);
	}
	if(x >=  table_source->table_needs_column_start && x <  table_source->table_needs_column_start +  table_source->table_needs_column_width) {
	}
	if(x >=  table_source->table_rebels_column_start && x <  table_source->table_rebels_column_start +  table_source->table_rebels_column_width) {
	text::add_line(state, contents, table_source->table_rebels_header_tooltip_key);
	}
}
void demographicswindow_pops_header_content_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	{
	location_cached_text = text::produce_simple_string(state, table_source->table_location_header_text_key);
	 location_internal_layout.contents.clear();
	 location_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  location_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_location_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_location_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, location_cached_text);
	}
	{
	size_cached_text = text::produce_simple_string(state, table_source->table_size_header_text_key);
	 size_internal_layout.contents.clear();
	 size_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  size_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_size_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_size_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, size_cached_text);
	}
	{
	culture_cached_text = text::produce_simple_string(state, table_source->table_culture_header_text_key);
	 culture_internal_layout.contents.clear();
	 culture_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  culture_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_culture_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_culture_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, culture_cached_text);
	}
	{
	job_cached_text = text::produce_simple_string(state, table_source->table_job_header_text_key);
	 job_internal_layout.contents.clear();
	 job_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  job_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_job_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_job_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, job_cached_text);
	}
	{
	religion_cached_text = text::produce_simple_string(state, table_source->table_religion_header_text_key);
	 religion_internal_layout.contents.clear();
	 religion_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  religion_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_religion_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_religion_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, religion_cached_text);
	}
	{
	ideology_cached_text = text::produce_simple_string(state, table_source->table_ideology_header_text_key);
	 ideology_internal_layout.contents.clear();
	 ideology_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  ideology_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_ideology_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_ideology_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, ideology_cached_text);
	}
	{
	issues_cached_text = text::produce_simple_string(state, table_source->table_issues_header_text_key);
	 issues_internal_layout.contents.clear();
	 issues_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  issues_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_issues_column_width - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_issues_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, issues_cached_text);
	}
	{
	needs_cached_text = text::produce_simple_string(state, table_source->table_needs_header_text_key);
	 needs_internal_layout.contents.clear();
	 needs_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  needs_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->table_needs_column_width - 0 - 16), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 16), 0, table_source->table_needs_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, needs_cached_text);
	}
}
void demographicswindow_pops_header_content_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 16);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (demographicswindow_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_spacer = rel_mouse_x >= table_source->table_spacer_column_start && rel_mouse_x < (table_source->table_spacer_column_start + table_source->table_spacer_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_spacer_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start + table_source->table_spacer_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_spacer){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y), float(table_source->table_spacer_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_spacer_column_start), float(y + base_data.size.y - 2), float(table_source->table_spacer_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	bool col_um_location = rel_mouse_x >= table_source->table_location_column_start && rel_mouse_x < (table_source->table_location_column_start + table_source->table_location_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_location) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_location_column_start), float(y), float(table_source->table_location_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_location_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_location_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start + table_source->table_location_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_location){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y), float(table_source->table_location_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_location_column_start), float(y + base_data.size.y - 2), float(table_source->table_location_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_location = state.ui_templates.colors[table_source->table_location_header_text_color]; 	if(table_source->table_location_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_location_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_location_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_location_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!location_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : location_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_location_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_location.r, col_color_location.g, col_color_location.b }, ogl::color_modification::none);
		}
	}
	bool col_um_size = rel_mouse_x >= table_source->table_size_column_start && rel_mouse_x < (table_source->table_size_column_start + table_source->table_size_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_size_column_start), float(y), float(table_source->table_size_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_size_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_size_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start + table_source->table_size_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y), float(table_source->table_size_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_size_column_start), float(y + base_data.size.y - 2), float(table_source->table_size_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_size = state.ui_templates.colors[table_source->table_size_header_text_color]; 	if(table_source->table_size_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_size_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_size_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_size_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!size_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : size_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_size_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_size.r, col_color_size.g, col_color_size.b }, ogl::color_modification::none);
		}
	}
	bool col_um_culture = rel_mouse_x >= table_source->table_culture_column_start && rel_mouse_x < (table_source->table_culture_column_start + table_source->table_culture_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_culture) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_culture_column_start), float(y), float(table_source->table_culture_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_culture_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_culture_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start + table_source->table_culture_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_culture){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y), float(table_source->table_culture_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_culture_column_start), float(y + base_data.size.y - 2), float(table_source->table_culture_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_culture = state.ui_templates.colors[table_source->table_culture_header_text_color]; 	if(table_source->table_culture_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_culture_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_culture_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_culture_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!culture_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : culture_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_culture_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_culture.r, col_color_culture.g, col_color_culture.b }, ogl::color_modification::none);
		}
	}
	bool col_um_job = rel_mouse_x >= table_source->table_job_column_start && rel_mouse_x < (table_source->table_job_column_start + table_source->table_job_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_job) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_job_column_start), float(y), float(table_source->table_job_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_job_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_job_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start + table_source->table_job_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_job){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y), float(table_source->table_job_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_job_column_start), float(y + base_data.size.y - 2), float(table_source->table_job_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_job = state.ui_templates.colors[table_source->table_job_header_text_color]; 	if(table_source->table_job_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_job_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_job_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_job_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!job_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : job_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_job_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_job.r, col_color_job.g, col_color_job.b }, ogl::color_modification::none);
		}
	}
	bool col_um_religion = rel_mouse_x >= table_source->table_religion_column_start && rel_mouse_x < (table_source->table_religion_column_start + table_source->table_religion_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_religion) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_religion_column_start), float(y), float(table_source->table_religion_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_religion_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_religion_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start + table_source->table_religion_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_religion){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y), float(table_source->table_religion_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_religion_column_start), float(y + base_data.size.y - 2), float(table_source->table_religion_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_religion = state.ui_templates.colors[table_source->table_religion_header_text_color]; 	if(table_source->table_religion_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_religion_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_religion_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_religion_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!religion_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : religion_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_religion_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_religion.r, col_color_religion.g, col_color_religion.b }, ogl::color_modification::none);
		}
	}
	bool col_um_ideology = rel_mouse_x >= table_source->table_ideology_column_start && rel_mouse_x < (table_source->table_ideology_column_start + table_source->table_ideology_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_ideology_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start + table_source->table_ideology_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_ideology){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y), float(table_source->table_ideology_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_ideology_column_start), float(y + base_data.size.y - 2), float(table_source->table_ideology_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_ideology = state.ui_templates.colors[table_source->table_ideology_header_text_color]; 	if(!ideology_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : ideology_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_ideology_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_ideology.r, col_color_ideology.g, col_color_ideology.b }, ogl::color_modification::none);
		}
	}
	bool col_um_issues = rel_mouse_x >= table_source->table_issues_column_start && rel_mouse_x < (table_source->table_issues_column_start + table_source->table_issues_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_issues_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start + table_source->table_issues_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_issues){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y), float(table_source->table_issues_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_issues_column_start), float(y + base_data.size.y - 2), float(table_source->table_issues_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_issues = state.ui_templates.colors[table_source->table_issues_header_text_color]; 	if(!issues_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : issues_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_issues_column_start + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_issues.r, col_color_issues.g, col_color_issues.b }, ogl::color_modification::none);
		}
	}
	bool col_um_militancy = rel_mouse_x >= table_source->table_militancy_column_start && rel_mouse_x < (table_source->table_militancy_column_start + table_source->table_militancy_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_militancy) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_militancy_column_start), float(y), float(table_source->table_militancy_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_militancy_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_militancy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start + table_source->table_militancy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_militancy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y), float(table_source->table_militancy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_militancy_column_start), float(y + base_data.size.y - 2), float(table_source->table_militancy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_militancy = state.ui_templates.colors[table_source->table_militancy_header_text_color]; 	if(table_source->table_militancy_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_militancy_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_militancy_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_militancy_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	bool col_um_consciousness = rel_mouse_x >= table_source->table_consciousness_column_start && rel_mouse_x < (table_source->table_consciousness_column_start + table_source->table_consciousness_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_consciousness) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_consciousness_column_start), float(y), float(table_source->table_consciousness_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_consciousness_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_consciousness_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start + table_source->table_consciousness_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_consciousness){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y), float(table_source->table_consciousness_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_consciousness_column_start), float(y + base_data.size.y - 2), float(table_source->table_consciousness_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_consciousness = state.ui_templates.colors[table_source->table_consciousness_header_text_color]; 	if(table_source->table_consciousness_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_consciousness_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_consciousness_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_consciousness_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	bool col_um_employment = rel_mouse_x >= table_source->table_employment_column_start && rel_mouse_x < (table_source->table_employment_column_start + table_source->table_employment_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_employment_column_start), float(y), float(table_source->table_employment_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_employment_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_employment_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start + table_source->table_employment_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_employment){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y), float(table_source->table_employment_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_employment_column_start), float(y + base_data.size.y - 2), float(table_source->table_employment_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_employment = state.ui_templates.colors[table_source->table_employment_header_text_color]; 	if(table_source->table_employment_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_employment_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_employment_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_employment_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	bool col_um_literacy = rel_mouse_x >= table_source->table_literacy_column_start && rel_mouse_x < (table_source->table_literacy_column_start + table_source->table_literacy_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_literacy) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_literacy_column_start), float(y), float(table_source->table_literacy_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_literacy_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_literacy_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start + table_source->table_literacy_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_literacy){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y), float(table_source->table_literacy_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_literacy_column_start), float(y + base_data.size.y - 2), float(table_source->table_literacy_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_literacy = state.ui_templates.colors[table_source->table_literacy_header_text_color]; 	if(table_source->table_literacy_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_literacy_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_literacy_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_literacy_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	bool col_um_money = rel_mouse_x >= table_source->table_money_column_start && rel_mouse_x < (table_source->table_money_column_start + table_source->table_money_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_money) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_money_column_start), float(y), float(table_source->table_money_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_money_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_money_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start + table_source->table_money_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_money){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y), float(table_source->table_money_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_money_column_start), float(y + base_data.size.y - 2), float(table_source->table_money_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_money = state.ui_templates.colors[table_source->table_money_header_text_color]; 	if(table_source->table_money_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_money_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_money_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_money_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	bool col_um_needs = rel_mouse_x >= table_source->table_needs_column_start && rel_mouse_x < (table_source->table_needs_column_start + table_source->table_needs_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_needs) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_needs_column_start), float(y), float(table_source->table_needs_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->table_needs_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_needs_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start + table_source->table_needs_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_needs){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y), float(table_source->table_needs_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_needs_column_start), float(y + base_data.size.y - 2), float(table_source->table_needs_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_needs = state.ui_templates.colors[table_source->table_needs_header_text_color]; 	if(table_source->table_needs_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_needs_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->table_needs_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->table_needs_column_start + 0), float(y + base_data.size.y / 2 - 8), float(8), float(16), state.ui_templates.icons[icon].renders.get_render(state, 8, 16, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!needs_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : needs_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->table_needs_column_start + 0 + 8, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_needs.r, col_color_needs.g, col_color_needs.b }, ogl::color_modification::none);
		}
	}
	bool col_um_rebels = rel_mouse_x >= table_source->table_rebels_column_start && rel_mouse_x < (table_source->table_rebels_column_start + table_source->table_rebels_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->table_rebels_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start + table_source->table_rebels_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_rebels){
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y), float(table_source->table_rebels_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->table_rebels_column_start), float(y + base_data.size.y - 2), float(table_source->table_rebels_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_rebels = state.ui_templates.colors[table_source->table_rebels_header_text_color]; 	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void demographicswindow_pops_header_content_t::on_update(sys::state& state) noexcept {
	demographicswindow_pops_header_t& pops_header = *((demographicswindow_pops_header_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::content::update
// END
}
void demographicswindow_pops_header_content_t::on_create(sys::state& state) noexcept {
// BEGIN pops_header::content::create
// END
}
ui::message_result demographicswindow_pops_header_mil_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pops_header_mil_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pops_header_mil_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void demographicswindow_pops_header_mil_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_pops_header_t& pops_header = *((demographicswindow_pops_header_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::mil_icon::update
// END
}
void demographicswindow_pops_header_mil_icon_t::on_create(sys::state& state) noexcept {
// BEGIN pops_header::mil_icon::create
// END
}
ui::message_result demographicswindow_pops_header_con_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pops_header_con_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pops_header_con_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void demographicswindow_pops_header_con_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_pops_header_t& pops_header = *((demographicswindow_pops_header_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::con_icon::update
// END
}
void demographicswindow_pops_header_con_icon_t::on_create(sys::state& state) noexcept {
// BEGIN pops_header::con_icon::create
// END
}
ui::message_result demographicswindow_pops_header_emp_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pops_header_emp_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pops_header_emp_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void demographicswindow_pops_header_emp_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_pops_header_t& pops_header = *((demographicswindow_pops_header_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::emp_icon::update
// END
}
void demographicswindow_pops_header_emp_icon_t::on_create(sys::state& state) noexcept {
// BEGIN pops_header::emp_icon::create
// END
}
ui::message_result demographicswindow_pops_header_lit_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pops_header_lit_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pops_header_lit_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void demographicswindow_pops_header_lit_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_pops_header_t& pops_header = *((demographicswindow_pops_header_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::lit_icon::update
// END
}
void demographicswindow_pops_header_lit_icon_t::on_create(sys::state& state) noexcept {
// BEGIN pops_header::lit_icon::create
// END
}
ui::message_result demographicswindow_pops_header_reb_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pops_header_reb_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pops_header_reb_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void demographicswindow_pops_header_reb_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_pops_header_t& pops_header = *((demographicswindow_pops_header_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::reb_icon::update
// END
}
void demographicswindow_pops_header_reb_icon_t::on_create(sys::state& state) noexcept {
// BEGIN pops_header::reb_icon::create
// END
}
ui::message_result demographicswindow_pops_header_cash_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result demographicswindow_pops_header_cash_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void demographicswindow_pops_header_cash_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_late_load_texture_handle(state, background_texture, texture_key), base_data.get_rotation(), false, state_is_rtl(state));
}
void demographicswindow_pops_header_cash_icon_t::on_update(sys::state& state) noexcept {
	demographicswindow_pops_header_t& pops_header = *((demographicswindow_pops_header_t*)(parent)); 
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::cash_icon::update
// END
}
void demographicswindow_pops_header_cash_icon_t::on_create(sys::state& state) noexcept {
// BEGIN pops_header::cash_icon::create
// END
}
void  demographicswindow_pops_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result demographicswindow_pops_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result demographicswindow_pops_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void demographicswindow_pops_header_t::on_update(sys::state& state) noexcept {
	demographicswindow_main_t& main = *((demographicswindow_main_t*)(parent->parent)); 
// BEGIN pops_header::update
// END
	remake_layout(state, true);
}
void demographicswindow_pops_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "content") {
					temp.ptr = content.get();
				} else
				if(cname == "mil_icon") {
					temp.ptr = mil_icon.get();
				} else
				if(cname == "con_icon") {
					temp.ptr = con_icon.get();
				} else
				if(cname == "emp_icon") {
					temp.ptr = emp_icon.get();
				} else
				if(cname == "lit_icon") {
					temp.ptr = lit_icon.get();
				} else
				if(cname == "reb_icon") {
					temp.ptr = reb_icon.get();
				} else
				if(cname == "cash_icon") {
					temp.ptr = cash_icon.get();
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
				if(cname == "main") {
					temp.ptr = make_demographicswindow_main(state);
				}
				if(cname == "nation_row") {
					temp.ptr = make_demographicswindow_nation_row(state);
				}
				if(cname == "state_row") {
					temp.ptr = make_demographicswindow_state_row(state);
				}
				if(cname == "province_row") {
					temp.ptr = make_demographicswindow_province_row(state);
				}
				if(cname == "pop_row") {
					temp.ptr = make_demographicswindow_pop_row(state);
				}
				if(cname == "filters_window") {
					temp.ptr = make_demographicswindow_filters_window(state);
				}
				if(cname == "job_filter") {
					temp.ptr = make_demographicswindow_job_filter(state);
				}
				if(cname == "religion_filter") {
					temp.ptr = make_demographicswindow_religion_filter(state);
				}
				if(cname == "location_filter") {
					temp.ptr = make_demographicswindow_location_filter(state);
				}
				if(cname == "culture_filter") {
					temp.ptr = make_demographicswindow_culture_filter(state);
				}
				if(cname == "pops_header") {
					temp.ptr = make_demographicswindow_pops_header(state);
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
void demographicswindow_pops_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("demographicswindow::pops_header"));
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
			content = std::make_unique<demographicswindow_pops_header_content_t>();
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
		if(child_data.name == "mil_icon") {
			mil_icon = std::make_unique<demographicswindow_pops_header_mil_icon_t>();
			mil_icon->parent = this;
			auto cptr = mil_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "con_icon") {
			con_icon = std::make_unique<demographicswindow_pops_header_con_icon_t>();
			con_icon->parent = this;
			auto cptr = con_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "emp_icon") {
			emp_icon = std::make_unique<demographicswindow_pops_header_emp_icon_t>();
			emp_icon->parent = this;
			auto cptr = emp_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "lit_icon") {
			lit_icon = std::make_unique<demographicswindow_pops_header_lit_icon_t>();
			lit_icon->parent = this;
			auto cptr = lit_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "reb_icon") {
			reb_icon = std::make_unique<demographicswindow_pops_header_reb_icon_t>();
			reb_icon->parent = this;
			auto cptr = reb_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "cash_icon") {
			cash_icon = std::make_unique<demographicswindow_pops_header_cash_icon_t>();
			cash_icon->parent = this;
			auto cptr = cash_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
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
// BEGIN pops_header::create
// END
}
std::unique_ptr<ui::element_base> make_demographicswindow_pops_header(sys::state& state) {
	auto ptr = std::make_unique<demographicswindow_pops_header_t>();
	ptr->on_create(state);
	return ptr;
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
// LOST-CODE
// BEGIN state_row::location::lbutton_action
//////////	auto p = state.world.state_instance_get_capital(state_row.content);
//////////	static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
//////////	state.map_state.center_map_on_province(state, p);
//////////	main.set_visible(state, false);
// END
// BEGIN location_filter::filter_label::update
//////	set_text(state, text::get_short_state_name(state, location_filter.value));
//////	text_color = !popwindow::excluded_states.contains(location_filter.value.index()) ? text::text_color::black : text::text_color::gold;
// END
// BEGIN culture_filter::filter_label::update
//////	set_text(state, text::produce_simple_string(state, state.world.culture_get_name(culture_filter.value)));
//////	text_color = !popwindow::excluded_cultures.contains(culture_filter.value.index()) ? text::text_color::black : text::text_color::gold;
// END
// BEGIN pop_row::location::lbutton_action
//////////	auto p = state.world.pop_get_province_from_pop_location(pop_row.value);
//////////	static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
//////////	state.map_state.center_map_on_province(state, p);
//////////	main.set_visible(state, false);
// END
}
