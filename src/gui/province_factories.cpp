// BEGIN prelude
#include "alice_ui.hpp"
#include "system_state.hpp"
#include "gui_deserialize.hpp"
#include "lua_alice_api.hpp"
#include "economy_production.hpp"
#include "labour_details.hpp"
#include "commands.hpp"
#include "ai_economy.hpp"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif

// END

namespace alice_ui {
struct province_factories_main_main_header_t;
struct province_factories_main_net_profit_value_t;
struct province_factories_main_total_sold_value_t;
struct province_factories_main_total_bought_value_t;
struct province_factories_main_total_wage_value_t;
struct province_factories_main_total_employment_value_t;
struct province_factories_main_total_employment_max_value_t;
struct province_factories_main_total_no_education_value_t;
struct province_factories_main_total_no_education_target_value_t;
struct province_factories_main_total_basic_education_value_t;
struct province_factories_main_total_basic_education_target_value_t;
struct province_factories_main_total_high_education_value_t;
struct province_factories_main_total_high_education_target_value_t;
struct province_factories_main_pop_project_name_t;
struct province_factories_main_pop_project_margin_value_t;
struct province_factories_main_pop_project_payback_value_t;
struct province_factories_main_build_factory_button_t;
struct province_factories_main_total_subsidy_value_t;
struct province_factories_main_t;
struct province_factories_table_body_table_body_control_t;
struct province_factories_table_body_icon_t;
struct province_factories_table_body_details_button_t;
struct province_factories_table_body_t;
struct province_factories_table_header_table_header_control_t;
struct province_factories_table_header_t;
struct province_factories_main_main_header_t : public alice_ui::template_label {
// BEGIN main::main_header::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_net_profit_value_t : public alice_ui::template_label {
// BEGIN main::net_profit_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_sold_value_t : public alice_ui::template_label {
// BEGIN main::total_sold_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_bought_value_t : public alice_ui::template_label {
// BEGIN main::total_bought_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_wage_value_t : public alice_ui::template_label {
// BEGIN main::total_wage_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_employment_value_t : public alice_ui::template_label {
// BEGIN main::total_employment_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_employment_max_value_t : public alice_ui::template_label {
// BEGIN main::total_employment_max_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_no_education_value_t : public alice_ui::template_label {
// BEGIN main::total_no_education_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_no_education_target_value_t : public alice_ui::template_label {
// BEGIN main::total_no_education_target_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_basic_education_value_t : public alice_ui::template_label {
// BEGIN main::total_basic_education_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_basic_education_target_value_t : public alice_ui::template_label {
// BEGIN main::total_basic_education_target_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_high_education_value_t : public alice_ui::template_label {
// BEGIN main::total_high_education_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_high_education_target_value_t : public alice_ui::template_label {
// BEGIN main::total_high_education_target_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_pop_project_name_t : public alice_ui::template_label {
// BEGIN main::pop_project_name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_pop_project_margin_value_t : public alice_ui::template_label {
// BEGIN main::pop_project_margin_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_pop_project_payback_value_t : public alice_ui::template_label {
// BEGIN main::pop_project_payback_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_build_factory_button_t : public alice_ui::template_icon_button {
// BEGIN main::build_factory_button::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_total_subsidy_value_t : public alice_ui::template_label {
// BEGIN main::total_subsidy_value::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_main_table_generator_t : public layout_generator {
// BEGIN main::table_generator::variables
// END
	struct table_body_option { dcon::factory_id factory; float profit_margin; float current_size; float total_hired; };
	std::vector<std::unique_ptr<ui::element_base>> table_body_pool;
	int32_t table_body_pool_used = 0;
	void add_table_body( dcon::factory_id factory,  float profit_margin,  float current_size,  float total_hired);
	std::vector<std::unique_ptr<ui::element_base>> table_header_pool;
	int32_t table_header_pool_used = 0;
	std::vector<std::variant<std::monostate, table_body_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct province_factories_table_body_table_body_control_t : public ui::element_base {
// BEGIN table_body::table_body_control::variables
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
	text::layout details_internal_layout;
	int32_t  details_text_color = 0;
	std::string details_cached_text;
	void set_details_text(sys::state & state, std::string const& new_text);
	text::layout margin_internal_layout;
	int32_t  margin_text_color = 0;
	std::string margin_cached_text;
	void set_margin_text(sys::state & state, std::string const& new_text);
	text::layout hired_internal_layout;
	int32_t  hired_text_color = 0;
	std::string hired_cached_text;
	void set_hired_text(sys::state & state, std::string const& new_text);
	text::layout size_internal_layout;
	int32_t  size_text_color = 0;
	std::string size_cached_text;
	void set_size_text(sys::state & state, std::string const& new_text);
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
struct province_factories_table_body_icon_t : public alice_ui::legacy_commodity_icon {
// BEGIN table_body::icon::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_table_body_details_button_t : public alice_ui::template_icon_button {
// BEGIN table_body::details_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct province_factories_table_header_table_header_control_t : public ui::element_base {
// BEGIN table_header::table_header_control::variables
// END
	int32_t template_id = -1;
	text::layout name_internal_layout;
	std::string name_cached_text;
	text::layout margin_internal_layout;
	std::string margin_cached_text;
	text::layout hired_internal_layout;
	std::string hired_cached_text;
	text::layout size_internal_layout;
	std::string size_cached_text;
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
struct province_factories_main_t : public layout_window_element {
// BEGIN main::variables
	dcon::factory_type_id desired_pop_project;
	ui::element_base* factory_details_window;
// END
	std::unique_ptr<province_factories_main_main_header_t> main_header;
	std::unique_ptr<template_label> net_profit_label;
	std::unique_ptr<template_label> total_sold_label;
	std::unique_ptr<template_label> total_bought_label;
	std::unique_ptr<template_label> total_wage_label;
	std::unique_ptr<province_factories_main_net_profit_value_t> net_profit_value;
	std::unique_ptr<province_factories_main_total_sold_value_t> total_sold_value;
	std::unique_ptr<province_factories_main_total_bought_value_t> total_bought_value;
	std::unique_ptr<province_factories_main_total_wage_value_t> total_wage_value;
	std::unique_ptr<template_label> total_employment_label;
	std::unique_ptr<template_label> total_no_education_label;
	std::unique_ptr<template_label> total_basic_education_label;
	std::unique_ptr<template_label> total_high_education_label;
	std::unique_ptr<province_factories_main_total_employment_value_t> total_employment_value;
	std::unique_ptr<province_factories_main_total_employment_max_value_t> total_employment_max_value;
	std::unique_ptr<province_factories_main_total_no_education_value_t> total_no_education_value;
	std::unique_ptr<province_factories_main_total_no_education_target_value_t> total_no_education_target_value;
	std::unique_ptr<province_factories_main_total_basic_education_value_t> total_basic_education_value;
	std::unique_ptr<province_factories_main_total_basic_education_target_value_t> total_basic_education_target_value;
	std::unique_ptr<province_factories_main_total_high_education_value_t> total_high_education_value;
	std::unique_ptr<province_factories_main_total_high_education_target_value_t> total_high_education_target_value;
	std::unique_ptr<template_label> pop_project_target_label;
	std::unique_ptr<province_factories_main_pop_project_name_t> pop_project_name;
	std::unique_ptr<template_label> pop_project_margin_label;
	std::unique_ptr<province_factories_main_pop_project_margin_value_t> pop_project_margin_value;
	std::unique_ptr<template_label> pop_project_payback_label;
	std::unique_ptr<province_factories_main_pop_project_payback_value_t> pop_project_payback_value;
	std::unique_ptr<province_factories_main_build_factory_button_t> build_factory_button;
	std::unique_ptr<template_label> total_subsidy_label;
	std::unique_ptr<province_factories_main_total_subsidy_value_t> total_subsidy_value;
	province_factories_main_table_generator_t table_generator;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t local_factories_list_icon_header_text_color = 0;
	uint8_t local_factories_list_icon_column_text_color = 0;
	text::alignment local_factories_list_icon_text_alignment = text::alignment::center;
	int16_t local_factories_list_icon_column_start = 0;
	int16_t local_factories_list_icon_column_width = 0;
	std::string_view local_factories_list_name_header_text_key;
	uint8_t local_factories_list_name_header_text_color = 0;
	uint8_t local_factories_list_name_column_text_color = 0;
	text::alignment local_factories_list_name_text_alignment = text::alignment::center;
	int8_t local_factories_list_name_sort_direction = 0;
	int16_t local_factories_list_name_column_start = 0;
	int16_t local_factories_list_name_column_width = 0;
	uint8_t local_factories_list_details_header_text_color = 0;
	uint8_t local_factories_list_details_column_text_color = 0;
	text::alignment local_factories_list_details_text_alignment = text::alignment::center;
	int16_t local_factories_list_details_column_start = 0;
	int16_t local_factories_list_details_column_width = 0;
	std::string_view local_factories_list_margin_header_text_key;
	uint8_t local_factories_list_margin_header_text_color = 0;
	uint8_t local_factories_list_margin_column_text_color = 0;
	text::alignment local_factories_list_margin_text_alignment = text::alignment::center;
	int8_t local_factories_list_margin_sort_direction = 0;
	int16_t local_factories_list_margin_column_start = 0;
	int16_t local_factories_list_margin_column_width = 0;
	std::string_view local_factories_list_hired_header_text_key;
	uint8_t local_factories_list_hired_header_text_color = 0;
	uint8_t local_factories_list_hired_column_text_color = 0;
	text::alignment local_factories_list_hired_text_alignment = text::alignment::center;
	int8_t local_factories_list_hired_sort_direction = 0;
	int16_t local_factories_list_hired_column_start = 0;
	int16_t local_factories_list_hired_column_width = 0;
	std::string_view local_factories_list_size_header_text_key;
	uint8_t local_factories_list_size_header_text_color = 0;
	uint8_t local_factories_list_size_column_text_color = 0;
	text::alignment local_factories_list_size_text_alignment = text::alignment::center;
	int8_t local_factories_list_size_sort_direction = 0;
	int16_t local_factories_list_size_column_start = 0;
	int16_t local_factories_list_size_column_width = 0;
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
std::unique_ptr<ui::element_base> make_province_factories_main(sys::state& state);
struct province_factories_table_body_t : public layout_window_element {
// BEGIN table_body::variables
// END
	dcon::factory_id factory;
	float profit_margin;
	float current_size;
	float total_hired;
	std::unique_ptr<province_factories_table_body_table_body_control_t> table_body_control;
	std::unique_ptr<province_factories_table_body_icon_t> icon;
	std::unique_ptr<province_factories_table_body_details_button_t> details_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t local_factories_list_icon_header_text_color = 0;
	uint8_t local_factories_list_icon_column_text_color = 0;
	text::alignment local_factories_list_icon_text_alignment = text::alignment::center;
	int16_t local_factories_list_icon_column_start = 0;
	int16_t local_factories_list_icon_column_width = 0;
	std::string_view local_factories_list_name_header_text_key;
	uint8_t local_factories_list_name_header_text_color = 0;
	uint8_t local_factories_list_name_column_text_color = 0;
	text::alignment local_factories_list_name_text_alignment = text::alignment::center;
	int8_t local_factories_list_name_sort_direction = 0;
	int16_t local_factories_list_name_column_start = 0;
	int16_t local_factories_list_name_column_width = 0;
	uint8_t local_factories_list_details_header_text_color = 0;
	uint8_t local_factories_list_details_column_text_color = 0;
	text::alignment local_factories_list_details_text_alignment = text::alignment::center;
	int16_t local_factories_list_details_column_start = 0;
	int16_t local_factories_list_details_column_width = 0;
	std::string_view local_factories_list_margin_header_text_key;
	uint8_t local_factories_list_margin_header_text_color = 0;
	uint8_t local_factories_list_margin_column_text_color = 0;
	text::alignment local_factories_list_margin_text_alignment = text::alignment::center;
	int8_t local_factories_list_margin_sort_direction = 0;
	int16_t local_factories_list_margin_column_start = 0;
	int16_t local_factories_list_margin_column_width = 0;
	std::string_view local_factories_list_hired_header_text_key;
	uint8_t local_factories_list_hired_header_text_color = 0;
	uint8_t local_factories_list_hired_column_text_color = 0;
	text::alignment local_factories_list_hired_text_alignment = text::alignment::center;
	int8_t local_factories_list_hired_sort_direction = 0;
	int16_t local_factories_list_hired_column_start = 0;
	int16_t local_factories_list_hired_column_width = 0;
	std::string_view local_factories_list_size_header_text_key;
	uint8_t local_factories_list_size_header_text_color = 0;
	uint8_t local_factories_list_size_column_text_color = 0;
	text::alignment local_factories_list_size_text_alignment = text::alignment::center;
	int8_t local_factories_list_size_sort_direction = 0;
	int16_t local_factories_list_size_column_start = 0;
	int16_t local_factories_list_size_column_width = 0;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "factory") {
			return (void*)(&factory);
		}
		if(name_parameter == "profit_margin") {
			return (void*)(&profit_margin);
		}
		if(name_parameter == "current_size") {
			return (void*)(&current_size);
		}
		if(name_parameter == "total_hired") {
			return (void*)(&total_hired);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_province_factories_table_body(sys::state& state);
struct province_factories_table_header_t : public layout_window_element {
// BEGIN table_header::variables
// END
	std::unique_ptr<province_factories_table_header_table_header_control_t> table_header_control;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	uint8_t local_factories_list_icon_header_text_color = 0;
	uint8_t local_factories_list_icon_column_text_color = 0;
	text::alignment local_factories_list_icon_text_alignment = text::alignment::center;
	int16_t local_factories_list_icon_column_start = 0;
	int16_t local_factories_list_icon_column_width = 0;
	std::string_view local_factories_list_name_header_text_key;
	uint8_t local_factories_list_name_header_text_color = 0;
	uint8_t local_factories_list_name_column_text_color = 0;
	text::alignment local_factories_list_name_text_alignment = text::alignment::center;
	int8_t local_factories_list_name_sort_direction = 0;
	int16_t local_factories_list_name_column_start = 0;
	int16_t local_factories_list_name_column_width = 0;
	uint8_t local_factories_list_details_header_text_color = 0;
	uint8_t local_factories_list_details_column_text_color = 0;
	text::alignment local_factories_list_details_text_alignment = text::alignment::center;
	int16_t local_factories_list_details_column_start = 0;
	int16_t local_factories_list_details_column_width = 0;
	std::string_view local_factories_list_margin_header_text_key;
	uint8_t local_factories_list_margin_header_text_color = 0;
	uint8_t local_factories_list_margin_column_text_color = 0;
	text::alignment local_factories_list_margin_text_alignment = text::alignment::center;
	int8_t local_factories_list_margin_sort_direction = 0;
	int16_t local_factories_list_margin_column_start = 0;
	int16_t local_factories_list_margin_column_width = 0;
	std::string_view local_factories_list_hired_header_text_key;
	uint8_t local_factories_list_hired_header_text_color = 0;
	uint8_t local_factories_list_hired_column_text_color = 0;
	text::alignment local_factories_list_hired_text_alignment = text::alignment::center;
	int8_t local_factories_list_hired_sort_direction = 0;
	int16_t local_factories_list_hired_column_start = 0;
	int16_t local_factories_list_hired_column_width = 0;
	std::string_view local_factories_list_size_header_text_key;
	uint8_t local_factories_list_size_header_text_color = 0;
	uint8_t local_factories_list_size_column_text_color = 0;
	text::alignment local_factories_list_size_text_alignment = text::alignment::center;
	int8_t local_factories_list_size_sort_direction = 0;
	int16_t local_factories_list_size_column_start = 0;
	int16_t local_factories_list_size_column_width = 0;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_province_factories_table_header(sys::state& state);
void province_factories_main_table_generator_t::add_table_body(dcon::factory_id factory, float profit_margin, float current_size, float total_hired) {
	values.emplace_back(table_body_option{factory, profit_margin, current_size, total_hired});
}
void  province_factories_main_table_generator_t::on_create(sys::state& state, layout_window_element* parent) {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::table_generator::on_create
// END
}
void  province_factories_main_table_generator_t::update(sys::state& state, layout_window_element* parent) {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::table_generator::update
	values.clear();
	auto province = state.map_state.selected_province;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto total = economy::factory_total_employment(state, factory);
		auto size = state.world.factory_get_size(factory);
		auto profits = economy::explain_last_factory_profit(state, factory);
		add_table_body(factory, (profits.profit) / (profits.inputs + profits.wages), size, total);
	}
// END
	{
	bool work_to_do = false;
	auto table_source = (province_factories_main_t*)(parent);
	if(table_source->local_factories_list_name_sort_direction != 0) work_to_do = true;
	if(table_source->local_factories_list_margin_sort_direction != 0) work_to_do = true;
	if(table_source->local_factories_list_hired_sort_direction != 0) work_to_do = true;
	if(table_source->local_factories_list_size_sort_direction != 0) work_to_do = true;
	if(work_to_do) {
		for(size_t i = 0; i < values.size(); ) {
			if(std::holds_alternative<table_body_option>(values[i])) {
				auto start_i = i;
				while(i < values.size() && std::holds_alternative<table_body_option>(values[i])) ++i;
				if(table_source->local_factories_list_name_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<table_body_option>(raw_a);
						auto const& b = std::get<table_body_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table_generator::local_factories_list::sort::name
						result = cmp3(
							text::produce_simple_string(state, state.world.factory_type_get_name(state.world.factory_get_building_type(a.factory))),
							text::produce_simple_string(state, state.world.factory_type_get_name(state.world.factory_get_building_type(b.factory)))
						);
// END
						return -result == table_source->local_factories_list_name_sort_direction;
					});
				}
				if(table_source->local_factories_list_margin_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<table_body_option>(raw_a);
						auto const& b = std::get<table_body_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table_generator::local_factories_list::sort::margin
						result = cmp3(a.profit_margin, b.profit_margin);
// END
						return -result == table_source->local_factories_list_margin_sort_direction;
					});
				}
				if(table_source->local_factories_list_hired_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<table_body_option>(raw_a);
						auto const& b = std::get<table_body_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table_generator::local_factories_list::sort::hired
						result = cmp3(a.total_hired, b.total_hired);
// END
						return -result == table_source->local_factories_list_hired_sort_direction;
					});
				}
				if(table_source->local_factories_list_size_sort_direction != 0) {
					sys::merge_sort(values.begin() + start_i, values.begin() + i, [&](auto const& raw_a, auto const& raw_b){
						auto const& a = std::get<table_body_option>(raw_a);
						auto const& b = std::get<table_body_option>(raw_b);
						int8_t result = 0;
// BEGIN main::table_generator::local_factories_list::sort::size
						result = cmp3(a.current_size, b.current_size);
// END
						return -result == table_source->local_factories_list_size_sort_direction;
					});
				}
			} else {
				++i;
			}
		}
	}
	}
}
measure_result  province_factories_main_table_generator_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<table_body_option>(values[index])) {
		if(table_header_pool.empty()) table_header_pool.emplace_back(make_province_factories_table_header(state));
		if(table_body_pool.empty()) table_body_pool.emplace_back(make_province_factories_table_body(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<table_body_option>(values[index - 1]))) {
			if(destination) {
				if(table_header_pool.size() <= size_t(table_header_pool_used)) table_header_pool.emplace_back(make_province_factories_table_header(state));
				if(table_body_pool.size() <= size_t(table_body_pool_used)) table_body_pool.emplace_back(make_province_factories_table_body(state));
				table_header_pool[table_header_pool_used]->base_data.position.x = int16_t(x);
				table_header_pool[table_header_pool_used]->base_data.position.y = int16_t(y);
				if(!table_header_pool[table_header_pool_used]->parent) {
					table_header_pool[table_header_pool_used]->parent = destination;
					table_header_pool[table_header_pool_used]->impl_on_update(state);
					table_header_pool[table_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(table_header_pool[table_header_pool_used].get());
				table_body_pool[table_body_pool_used]->base_data.position.x = int16_t(x);
				table_body_pool[table_body_pool_used]->base_data.position.y = int16_t(y +  table_header_pool[0]->base_data.size.y + 0);
				table_body_pool[table_body_pool_used]->parent = destination;
				destination->children.push_back(table_body_pool[table_body_pool_used].get());
				((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->factory = std::get<table_body_option>(values[index]).factory;
				((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->profit_margin = std::get<table_body_option>(values[index]).profit_margin;
				((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->current_size = std::get<table_body_option>(values[index]).current_size;
				((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->total_hired = std::get<table_body_option>(values[index]).total_hired;
			((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->set_alternate(!alternate);
				table_body_pool[table_body_pool_used]->impl_on_update(state);
				table_header_pool_used++;
				table_body_pool_used++;
			}
	 	 	bool stick_to_next = false;
			return measure_result{std::max(table_header_pool[0]->base_data.size.x, table_body_pool[0]->base_data.size.x), table_header_pool[0]->base_data.size.y + table_body_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
		}
		if(destination) {
			if(table_body_pool.size() <= size_t(table_body_pool_used)) table_body_pool.emplace_back(make_province_factories_table_body(state));
			table_body_pool[table_body_pool_used]->base_data.position.x = int16_t(x);
			table_body_pool[table_body_pool_used]->base_data.position.y = int16_t(y);
			table_body_pool[table_body_pool_used]->parent = destination;
			destination->children.push_back(table_body_pool[table_body_pool_used].get());
			((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->factory = std::get<table_body_option>(values[index]).factory;
			((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->profit_margin = std::get<table_body_option>(values[index]).profit_margin;
			((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->current_size = std::get<table_body_option>(values[index]).current_size;
			((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->total_hired = std::get<table_body_option>(values[index]).total_hired;
			((province_factories_table_body_t*)(table_body_pool[table_body_pool_used].get()))->set_alternate(alternate);
			table_body_pool[table_body_pool_used]->impl_on_update(state);
			table_body_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = false;
		return measure_result{ table_body_pool[0]->base_data.size.x, table_body_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  province_factories_main_table_generator_t::reset_pools() {
	table_header_pool_used = 0;
	table_body_pool_used = 0;
}
void province_factories_main_main_header_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::main_header::update
	text::substitution_map m;
	text::add_to_substitution_map(m, text::variable_type::province, state.map_state.selected_province);
	auto result = text::resolve_string_substitution(state, "province_factories_report", m);
	set_text(state, result);
// END
}
void province_factories_main_net_profit_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::net_profit_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto explanation = economy::explain_last_factory_profit(state, factory);
		total = total + explanation.profit;
	}
	set_text(state, text::format_money(total));
// END
}
void province_factories_main_total_sold_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_sold_value::update
	auto province = state.map_state.selected_province;
	auto area = state.world.province_get_state_membership(province);
	auto market = state.world.state_instance_get_market_from_local_market(area);
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto explanation = economy::explain_last_factory_profit(state, factory);
		total = total + explanation.output * state.world.market_get_actual_probability_to_sell(market, state.world.factory_type_get_output(state.world.factory_get_building_type(factory)));
	}
	set_text(state, text::format_money(total));
// END
}
void province_factories_main_total_bought_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_bought_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto explanation = economy::explain_last_factory_profit(state, factory);
		total = total + explanation.inputs;
	}
	set_text(state, text::format_money(total));
// END
}
void province_factories_main_total_wage_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_wage_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto explanation = economy::explain_last_factory_profit(state, factory);
		total = total + explanation.wages;
	}
	set_text(state, text::format_money(total));
// END
}
void province_factories_main_total_employment_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_employment_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = economy::factory_total_employment(state, factory);
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_total_employment_max_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_employment_max_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = factory.get_size();
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_total_no_education_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_no_education_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = economy::factory_unqualified_employment(state, factory);
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_total_no_education_target_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_no_education_target_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = state.world.factory_get_unqualified_employment(factory);
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_total_basic_education_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_basic_education_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = economy::factory_primary_employment(state, factory);
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_total_basic_education_target_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_basic_education_target_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = state.world.factory_get_primary_employment(factory);
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_total_high_education_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_high_education_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = economy::factory_secondary_employment(state, factory);
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_total_high_education_target_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_high_education_target_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto value = state.world.factory_get_secondary_employment(factory);
		total = total + value;
	}
	set_text(state, text::prettify((int)total));
// END
}
void province_factories_main_pop_project_name_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::pop_project_name::update
	auto province = state.map_state.selected_province;
	auto nation = state.world.province_get_nation_from_province_ownership(province);
	auto area = state.world.province_get_state_membership(province);
	auto market = state.world.state_instance_get_market_from_local_market(area);
	std::vector<dcon::factory_type_id> craved {};
	if(economy::can_build_factory_in_colony(state, province)) {
		ai::get_craved_factory_types(
			state,
			state.world.province_get_nation_from_province_ownership(province),
			market,
			province,
			craved,
			true
		);
	}

	if(craved.size() > 0) {
		main.desired_pop_project = craved[0];
	}

	if(craved.size() == 0 && economy::can_build_factory_in_colony(state, province)) {
		ai::get_desired_factory_types(
			state,
			state.world.province_get_nation_from_province_ownership(province),
			market,
			province,
			craved,
			true
		);
	}


	if (main.desired_pop_project) {
		auto name = state.world.factory_type_get_name(main.desired_pop_project);
		set_text(state, text::produce_simple_string(state, name));
	} else {
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::province, state.map_state.selected_province);
		auto result = text::resolve_string_substitution(state, "no_factory_type", m);
		set_text(state, text::produce_simple_string(state, result));
	}
// END
}
void province_factories_main_pop_project_margin_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::pop_project_margin_value::update
	// todo: split into separate function
	if(!main.desired_pop_project) {
		set_text(state, text::format_percentage(0, 1));
		return;
	}
	auto province = state.map_state.selected_province;
	auto area = state.world.province_get_state_membership(province);
	auto market = state.world.state_instance_get_market_from_local_market(area);
	auto nation = state.world.province_get_nation_from_province_ownership(province);
	auto wage = state.world.province_get_labor_price(province, economy::labor::basic_education) * 2.f;
	auto base_workforce = state.world.factory_type_get_base_workforce(main.desired_pop_project);
	float cost = economy::factory_type_build_cost(state, nation, province, main.desired_pop_project, true);
	auto const tax_eff = economy::tax_collection_rate(state, nation, province);
	auto const rich_effect = (1.0f - tax_eff * float(state.world.nation_get_rich_tax(nation)) / 100.0f);
	float output = economy::factory_type_output_cost(state, nation, market, main.desired_pop_project) * rich_effect * (1.f + std::remainder(rng::get_random(state, nation.value * province.value * main.desired_pop_project.value) / 100.f, 0.5f) - 0.25f);
	float input = economy::factory_type_input_cost(state, nation, market, main.desired_pop_project);
	float profitability = (output - input - wage * base_workforce) / (input + wage * base_workforce);
	float payback_time = cost / std::max(0.00001f, (output - input - wage * base_workforce));
	set_text(state, text::format_percentage(profitability, 1));
// END
}
void province_factories_main_pop_project_payback_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::pop_project_payback_value::update
	// todo: split into separate function
	if(!main.desired_pop_project) {
		set_text(state, text::format_percentage(0, 1));
		return;
	}
	auto province = state.map_state.selected_province;
	auto area = state.world.province_get_state_membership(province);
	auto market = state.world.state_instance_get_market_from_local_market(area);
	auto nation = state.world.province_get_nation_from_province_ownership(province);
	auto wage = state.world.province_get_labor_price(province, economy::labor::basic_education) * 2.f;
	auto base_workforce = state.world.factory_type_get_base_workforce(main.desired_pop_project);
	float cost = economy::factory_type_build_cost(state, nation, province, main.desired_pop_project, true);
	auto const tax_eff = economy::tax_collection_rate(state, nation, province);
	auto const rich_effect = (1.0f - tax_eff * float(state.world.nation_get_rich_tax(nation)) / 100.0f);
	float output = economy::factory_type_output_cost(state, nation, market, main.desired_pop_project) * rich_effect * (1.f + std::remainder(rng::get_random(state, nation.value * province.value * main.desired_pop_project.value) / 100.f, 0.5f) - 0.25f);
	float input = economy::factory_type_input_cost(state, nation, market, main.desired_pop_project);
	float profitability = (output - input - wage * base_workforce) / input;
	float payback_time = cost / std::max(0.00001f, (output - input - wage * base_workforce));
	set_text(state, text::format_wholenum((int)payback_time));
// END
}
void province_factories_main_build_factory_button_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::build_factory_button::update
// END
}
void province_factories_main_total_subsidy_value_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent)); 
// BEGIN main::total_subsidy_value::update
	auto province = state.map_state.selected_province;
	auto total = 0.f;
	for(auto f : state.world.province_get_factory_location(province)) {
		auto factory = f.get_factory();
		auto explanation = economy::explain_last_factory_profit(state, factory);
		total = total + explanation.subsidy;
	}
	set_text(state, text::format_money(total));
// END
}
ui::message_result province_factories_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result province_factories_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_factories_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	table_generator.update(state, this);
	remake_layout(state, true);
}
void province_factories_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "main_header") {
					temp.ptr = main_header.get();
				} else
				if(cname == "net_profit_label") {
					temp.ptr = net_profit_label.get();
				} else
				if(cname == "total_sold_label") {
					temp.ptr = total_sold_label.get();
				} else
				if(cname == "total_bought_label") {
					temp.ptr = total_bought_label.get();
				} else
				if(cname == "total_wage_label") {
					temp.ptr = total_wage_label.get();
				} else
				if(cname == "net_profit_value") {
					temp.ptr = net_profit_value.get();
				} else
				if(cname == "total_sold_value") {
					temp.ptr = total_sold_value.get();
				} else
				if(cname == "total_bought_value") {
					temp.ptr = total_bought_value.get();
				} else
				if(cname == "total_wage_value") {
					temp.ptr = total_wage_value.get();
				} else
				if(cname == "total_employment_label") {
					temp.ptr = total_employment_label.get();
				} else
				if(cname == "total_no_education_label") {
					temp.ptr = total_no_education_label.get();
				} else
				if(cname == "total_basic_education_label") {
					temp.ptr = total_basic_education_label.get();
				} else
				if(cname == "total_high_education_label") {
					temp.ptr = total_high_education_label.get();
				} else
				if(cname == "total_employment_value") {
					temp.ptr = total_employment_value.get();
				} else
				if(cname == "total_employment_max_value") {
					temp.ptr = total_employment_max_value.get();
				} else
				if(cname == "total_no_education_value") {
					temp.ptr = total_no_education_value.get();
				} else
				if(cname == "total_no_education_target_value") {
					temp.ptr = total_no_education_target_value.get();
				} else
				if(cname == "total_basic_education_value") {
					temp.ptr = total_basic_education_value.get();
				} else
				if(cname == "total_basic_education_target_value") {
					temp.ptr = total_basic_education_target_value.get();
				} else
				if(cname == "total_high_education_value") {
					temp.ptr = total_high_education_value.get();
				} else
				if(cname == "total_high_education_target_value") {
					temp.ptr = total_high_education_target_value.get();
				} else
				if(cname == "pop_project_target_label") {
					temp.ptr = pop_project_target_label.get();
				} else
				if(cname == "pop_project_name") {
					temp.ptr = pop_project_name.get();
				} else
				if(cname == "pop_project_margin_label") {
					temp.ptr = pop_project_margin_label.get();
				} else
				if(cname == "pop_project_margin_value") {
					temp.ptr = pop_project_margin_value.get();
				} else
				if(cname == "pop_project_payback_label") {
					temp.ptr = pop_project_payback_label.get();
				} else
				if(cname == "pop_project_payback_value") {
					temp.ptr = pop_project_payback_value.get();
				} else
				if(cname == "build_factory_button") {
					temp.ptr = build_factory_button.get();
				} else
				if(cname == "total_subsidy_label") {
					temp.ptr = total_subsidy_label.get();
				} else
				if(cname == "total_subsidy_value") {
					temp.ptr = total_subsidy_value.get();
				} else
				{
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
					temp.ptr = make_province_factories_main(state);
				}
				if(cname == "table_body") {
					temp.ptr = make_province_factories_table_body(state);
				}
				if(cname == "table_header") {
					temp.ptr = make_province_factories_table_header(state);
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
				if(cname == "table_generator") {
					temp.generator = &table_generator;
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
void province_factories_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_factories::main"));
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
		if(child_data.name == "main_header") {
			main_header = std::make_unique<province_factories_main_main_header_t>();
			main_header->parent = this;
			auto cptr = main_header.get();
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
		if(child_data.name == "net_profit_label") {
			net_profit_label = std::make_unique<template_label>();
			net_profit_label->parent = this;
			auto cptr = net_profit_label.get();
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
		if(child_data.name == "total_sold_label") {
			total_sold_label = std::make_unique<template_label>();
			total_sold_label->parent = this;
			auto cptr = total_sold_label.get();
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
		if(child_data.name == "total_bought_label") {
			total_bought_label = std::make_unique<template_label>();
			total_bought_label->parent = this;
			auto cptr = total_bought_label.get();
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
		if(child_data.name == "total_wage_label") {
			total_wage_label = std::make_unique<template_label>();
			total_wage_label->parent = this;
			auto cptr = total_wage_label.get();
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
		if(child_data.name == "net_profit_value") {
			net_profit_value = std::make_unique<province_factories_main_net_profit_value_t>();
			net_profit_value->parent = this;
			auto cptr = net_profit_value.get();
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
		if(child_data.name == "total_sold_value") {
			total_sold_value = std::make_unique<province_factories_main_total_sold_value_t>();
			total_sold_value->parent = this;
			auto cptr = total_sold_value.get();
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
		if(child_data.name == "total_bought_value") {
			total_bought_value = std::make_unique<province_factories_main_total_bought_value_t>();
			total_bought_value->parent = this;
			auto cptr = total_bought_value.get();
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
		if(child_data.name == "total_wage_value") {
			total_wage_value = std::make_unique<province_factories_main_total_wage_value_t>();
			total_wage_value->parent = this;
			auto cptr = total_wage_value.get();
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
		if(child_data.name == "total_employment_label") {
			total_employment_label = std::make_unique<template_label>();
			total_employment_label->parent = this;
			auto cptr = total_employment_label.get();
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
		if(child_data.name == "total_no_education_label") {
			total_no_education_label = std::make_unique<template_label>();
			total_no_education_label->parent = this;
			auto cptr = total_no_education_label.get();
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
		if(child_data.name == "total_basic_education_label") {
			total_basic_education_label = std::make_unique<template_label>();
			total_basic_education_label->parent = this;
			auto cptr = total_basic_education_label.get();
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
		if(child_data.name == "total_high_education_label") {
			total_high_education_label = std::make_unique<template_label>();
			total_high_education_label->parent = this;
			auto cptr = total_high_education_label.get();
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
		if(child_data.name == "total_employment_value") {
			total_employment_value = std::make_unique<province_factories_main_total_employment_value_t>();
			total_employment_value->parent = this;
			auto cptr = total_employment_value.get();
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
		if(child_data.name == "total_employment_max_value") {
			total_employment_max_value = std::make_unique<province_factories_main_total_employment_max_value_t>();
			total_employment_max_value->parent = this;
			auto cptr = total_employment_max_value.get();
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
		if(child_data.name == "total_no_education_value") {
			total_no_education_value = std::make_unique<province_factories_main_total_no_education_value_t>();
			total_no_education_value->parent = this;
			auto cptr = total_no_education_value.get();
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
		if(child_data.name == "total_no_education_target_value") {
			total_no_education_target_value = std::make_unique<province_factories_main_total_no_education_target_value_t>();
			total_no_education_target_value->parent = this;
			auto cptr = total_no_education_target_value.get();
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
		if(child_data.name == "total_basic_education_value") {
			total_basic_education_value = std::make_unique<province_factories_main_total_basic_education_value_t>();
			total_basic_education_value->parent = this;
			auto cptr = total_basic_education_value.get();
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
		if(child_data.name == "total_basic_education_target_value") {
			total_basic_education_target_value = std::make_unique<province_factories_main_total_basic_education_target_value_t>();
			total_basic_education_target_value->parent = this;
			auto cptr = total_basic_education_target_value.get();
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
		if(child_data.name == "total_high_education_value") {
			total_high_education_value = std::make_unique<province_factories_main_total_high_education_value_t>();
			total_high_education_value->parent = this;
			auto cptr = total_high_education_value.get();
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
		if(child_data.name == "total_high_education_target_value") {
			total_high_education_target_value = std::make_unique<province_factories_main_total_high_education_target_value_t>();
			total_high_education_target_value->parent = this;
			auto cptr = total_high_education_target_value.get();
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
		if(child_data.name == "pop_project_target_label") {
			pop_project_target_label = std::make_unique<template_label>();
			pop_project_target_label->parent = this;
			auto cptr = pop_project_target_label.get();
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
		if(child_data.name == "pop_project_name") {
			pop_project_name = std::make_unique<province_factories_main_pop_project_name_t>();
			pop_project_name->parent = this;
			auto cptr = pop_project_name.get();
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
		if(child_data.name == "pop_project_margin_label") {
			pop_project_margin_label = std::make_unique<template_label>();
			pop_project_margin_label->parent = this;
			auto cptr = pop_project_margin_label.get();
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
		if(child_data.name == "pop_project_margin_value") {
			pop_project_margin_value = std::make_unique<province_factories_main_pop_project_margin_value_t>();
			pop_project_margin_value->parent = this;
			auto cptr = pop_project_margin_value.get();
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
		if(child_data.name == "pop_project_payback_label") {
			pop_project_payback_label = std::make_unique<template_label>();
			pop_project_payback_label->parent = this;
			auto cptr = pop_project_payback_label.get();
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
		if(child_data.name == "pop_project_payback_value") {
			pop_project_payback_value = std::make_unique<province_factories_main_pop_project_payback_value_t>();
			pop_project_payback_value->parent = this;
			auto cptr = pop_project_payback_value.get();
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
		if(child_data.name == "build_factory_button") {
			build_factory_button = std::make_unique<province_factories_main_build_factory_button_t>();
			build_factory_button->parent = this;
			auto cptr = build_factory_button.get();
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
		if(child_data.name == "total_subsidy_label") {
			total_subsidy_label = std::make_unique<template_label>();
			total_subsidy_label->parent = this;
			auto cptr = total_subsidy_label.get();
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
		if(child_data.name == "total_subsidy_value") {
			total_subsidy_value = std::make_unique<province_factories_main_total_subsidy_value_t>();
			total_subsidy_value->parent = this;
			auto cptr = total_subsidy_value.get();
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
		if(child_data.name == ".tablocal_factories_list") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			main_section.read<std::string_view>(); // discard
			main_section.read<std::string_view>(); // discard
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_icon_column_start = running_w_total;
			col_section.read(local_factories_list_icon_column_width);
			running_w_total += local_factories_list_icon_column_width;
			col_section.read(local_factories_list_icon_column_text_color);
			col_section.read(local_factories_list_icon_header_text_color);
			col_section.read(local_factories_list_icon_text_alignment);
			local_factories_list_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_name_column_start = running_w_total;
			col_section.read(local_factories_list_name_column_width);
			running_w_total += local_factories_list_name_column_width;
			col_section.read(local_factories_list_name_column_text_color);
			col_section.read(local_factories_list_name_header_text_color);
			col_section.read(local_factories_list_name_text_alignment);
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_details_column_start = running_w_total;
			col_section.read(local_factories_list_details_column_width);
			running_w_total += local_factories_list_details_column_width;
			col_section.read(local_factories_list_details_column_text_color);
			col_section.read(local_factories_list_details_header_text_color);
			col_section.read(local_factories_list_details_text_alignment);
			local_factories_list_margin_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_margin_column_start = running_w_total;
			col_section.read(local_factories_list_margin_column_width);
			running_w_total += local_factories_list_margin_column_width;
			col_section.read(local_factories_list_margin_column_text_color);
			col_section.read(local_factories_list_margin_header_text_color);
			col_section.read(local_factories_list_margin_text_alignment);
			local_factories_list_hired_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_hired_column_start = running_w_total;
			col_section.read(local_factories_list_hired_column_width);
			running_w_total += local_factories_list_hired_column_width;
			col_section.read(local_factories_list_hired_column_text_color);
			col_section.read(local_factories_list_hired_header_text_color);
			col_section.read(local_factories_list_hired_text_alignment);
			local_factories_list_size_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_size_column_start = running_w_total;
			col_section.read(local_factories_list_size_column_width);
			running_w_total += local_factories_list_size_column_width;
			col_section.read(local_factories_list_size_column_text_color);
			col_section.read(local_factories_list_size_header_text_color);
			col_section.read(local_factories_list_size_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		{ } 
		pending_children.pop_back();
	}
	table_generator.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
	{
		auto u_ptr = make_factory_details_main(state);
		factory_details_window = u_ptr.get();
		factory_details_window->set_visible(state, false);
		state.ui_state.root->add_child_to_back(std::move(u_ptr));
	}
// END
}
std::unique_ptr<ui::element_base> make_province_factories_main(sys::state& state) {
	auto ptr = std::make_unique<province_factories_main_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result province_factories_table_body_table_body_control_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_factories_table_body_table_body_control_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_factories_table_body_table_body_control_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (province_factories_main_t*)(parent->parent);
	if(x >= table_source->local_factories_list_icon_column_start && x < table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width) {
	}
	if(x >= table_source->local_factories_list_name_column_start && x < table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width) {
	}
	if(x >= table_source->local_factories_list_details_column_start && x < table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width) {
	}
	if(x >= table_source->local_factories_list_margin_column_start && x < table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width) {
	}
	if(x >= table_source->local_factories_list_hired_column_start && x < table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width) {
	}
	if(x >= table_source->local_factories_list_size_column_start && x < table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void province_factories_table_body_table_body_control_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (province_factories_main_t*)(parent->parent);
	if(x >=  table_source->local_factories_list_icon_column_start && x <  table_source->local_factories_list_icon_column_start +  table_source->local_factories_list_icon_column_width) {
	}
	if(x >=  table_source->local_factories_list_name_column_start && x <  table_source->local_factories_list_name_column_start +  table_source->local_factories_list_name_column_width) {
	}
	if(x >=  table_source->local_factories_list_details_column_start && x <  table_source->local_factories_list_details_column_start +  table_source->local_factories_list_details_column_width) {
	}
	if(x >=  table_source->local_factories_list_margin_column_start && x <  table_source->local_factories_list_margin_column_start +  table_source->local_factories_list_margin_column_width) {
	}
	if(x >=  table_source->local_factories_list_hired_column_start && x <  table_source->local_factories_list_hired_column_start +  table_source->local_factories_list_hired_column_width) {
	}
	if(x >=  table_source->local_factories_list_size_column_start && x <  table_source->local_factories_list_size_column_start +  table_source->local_factories_list_size_column_width) {
	}
}
void province_factories_table_body_table_body_control_t::set_icon_text(sys::state & state, std::string const& new_text) {
		auto table_source = (province_factories_main_t*)(parent->parent);
	if(new_text !=  icon_cached_text) {
		icon_cached_text = new_text;
		icon_internal_layout.contents.clear();
		icon_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ icon_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_icon_column_width - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_icon_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, icon_cached_text);
		}
	} else {
	}
}
void province_factories_table_body_table_body_control_t::set_name_text(sys::state & state, std::string const& new_text) {
		auto table_source = (province_factories_main_t*)(parent->parent);
	if(new_text !=  name_cached_text) {
		name_cached_text = new_text;
		name_internal_layout.contents.clear();
		name_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_name_column_width - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, name_cached_text);
		}
	} else {
	}
}
void province_factories_table_body_table_body_control_t::set_details_text(sys::state & state, std::string const& new_text) {
		auto table_source = (province_factories_main_t*)(parent->parent);
	if(new_text !=  details_cached_text) {
		details_cached_text = new_text;
		details_internal_layout.contents.clear();
		details_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ details_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_details_column_width - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_details_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, details_cached_text);
		}
	} else {
	}
}
void province_factories_table_body_table_body_control_t::set_margin_text(sys::state & state, std::string const& new_text) {
		auto table_source = (province_factories_main_t*)(parent->parent);
	if(new_text !=  margin_cached_text) {
		margin_cached_text = new_text;
		margin_internal_layout.contents.clear();
		margin_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ margin_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_margin_column_width - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_margin_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, margin_cached_text);
		}
	} else {
	}
}
void province_factories_table_body_table_body_control_t::set_hired_text(sys::state & state, std::string const& new_text) {
		auto table_source = (province_factories_main_t*)(parent->parent);
	if(new_text !=  hired_cached_text) {
		hired_cached_text = new_text;
		hired_internal_layout.contents.clear();
		hired_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ hired_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_hired_column_width - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_hired_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, hired_cached_text);
		}
	} else {
	}
}
void province_factories_table_body_table_body_control_t::set_size_text(sys::state & state, std::string const& new_text) {
		auto table_source = (province_factories_main_t*)(parent->parent);
	if(new_text !=  size_cached_text) {
		size_cached_text = new_text;
		size_internal_layout.contents.clear();
		size_internal_layout.number_of_lines = 0;
		{
		text::single_line_layout sl{ size_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_size_column_width - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_size_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr }; 
		sl.add_text(state, size_cached_text);
		}
	} else {
	}
}
void province_factories_table_body_table_body_control_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (province_factories_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_icon = rel_mouse_x >= table_source->local_factories_list_icon_column_start && rel_mouse_x < (table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_icon_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y), float(table_source->local_factories_list_icon_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_icon_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_icon = state.ui_templates.colors[icon_text_color]; 	if(!icon_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : icon_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_icon_column_start + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_icon.r, col_color_icon.g, col_color_icon.b }, ogl::color_modification::none);
		}
	}
	bool col_um_name = rel_mouse_x >= table_source->local_factories_list_name_column_start && rel_mouse_x < (table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y), float(table_source->local_factories_list_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_name = state.ui_templates.colors[name_text_color]; 	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_name_column_start + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_name.r, col_color_name.g, col_color_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_details = rel_mouse_x >= table_source->local_factories_list_details_column_start && rel_mouse_x < (table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_details){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_details_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_details){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_details){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y), float(table_source->local_factories_list_details_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_details_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_details = state.ui_templates.colors[details_text_color]; 	if(!details_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : details_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_details_column_start + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_details.r, col_color_details.g, col_color_details.b }, ogl::color_modification::none);
		}
	}
	bool col_um_margin = rel_mouse_x >= table_source->local_factories_list_margin_column_start && rel_mouse_x < (table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_margin){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_margin_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_margin){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_margin){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y), float(table_source->local_factories_list_margin_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_margin_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_margin = state.ui_templates.colors[margin_text_color]; 	if(!margin_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : margin_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_margin_column_start + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_margin.r, col_color_margin.g, col_color_margin.b }, ogl::color_modification::none);
		}
	}
	bool col_um_hired = rel_mouse_x >= table_source->local_factories_list_hired_column_start && rel_mouse_x < (table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_hired){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_hired_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_hired){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_hired){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y), float(table_source->local_factories_list_hired_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_hired_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_hired = state.ui_templates.colors[hired_text_color]; 	if(!hired_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : hired_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_hired_column_start + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_hired.r, col_color_hired.g, col_color_hired.b }, ogl::color_modification::none);
		}
	}
	bool col_um_size = rel_mouse_x >= table_source->local_factories_list_size_column_start && rel_mouse_x < (table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_size_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y), float(table_source->local_factories_list_size_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_size_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_size = state.ui_templates.colors[size_text_color]; 	if(!size_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : size_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_size_column_start + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_size.r, col_color_size.g, col_color_size.b }, ogl::color_modification::none);
		}
	}
}
void province_factories_table_body_table_body_control_t::on_update(sys::state& state) noexcept {
	province_factories_table_body_t& table_body = *((province_factories_table_body_t*)(parent)); 
	province_factories_main_t& main = *((province_factories_main_t*)(parent->parent)); 
// BEGIN table_body::table_body_control::update
	auto factory = table_body.factory;
	auto factory_type = state.world.factory_get_building_type(factory);
	auto name = state.world.factory_type_get_name(factory_type);

	set_name_text(state, text::produce_simple_string(state, name));
	set_hired_text(state, text::prettify((int)table_body.total_hired));
	set_size_text(state, text::prettify((int)table_body.current_size));
	set_margin_text(state, text::format_percentage(table_body.profit_margin, 0));
// END
}
void province_factories_table_body_table_body_control_t::on_create(sys::state& state) noexcept {
// BEGIN table_body::table_body_control::create
// END
}
void province_factories_table_body_icon_t::on_update(sys::state& state) noexcept {
	province_factories_table_body_t& table_body = *((province_factories_table_body_t*)(parent)); 
	province_factories_main_t& main = *((province_factories_main_t*)(parent->parent)); 
// BEGIN table_body::icon::update
	content = state.world.factory_type_get_output(state.world.factory_get_building_type(table_body.factory));
// END
}
void province_factories_table_body_details_button_t::on_update(sys::state& state) noexcept {
	province_factories_table_body_t& table_body = *((province_factories_table_body_t*)(parent)); 
	province_factories_main_t& main = *((province_factories_main_t*)(parent->parent)); 
// BEGIN table_body::details_button::update
// END
}
bool province_factories_table_body_details_button_t::button_action(sys::state& state) noexcept {
	province_factories_table_body_t& table_body = *((province_factories_table_body_t*)(parent)); 
	province_factories_main_t& main = *((province_factories_main_t*)(parent->parent)); 
// BEGIN table_body::details_button::lbutton_action
	state.selected_factory = table_body.factory;
	auto target_is_visible = main.factory_details_window->is_visible();
	main.factory_details_window->set_visible(state, true);
	main.factory_details_window->impl_on_update(state);
	main.factory_details_window->parent->move_child_to_front(main.factory_details_window);
// END
	return true;
}
void  province_factories_table_body_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 2 : 4;
}
ui::message_result province_factories_table_body_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_factories_table_body_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_factories_table_body_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent->parent)); 
// BEGIN table_body::update
// END
	remake_layout(state, true);
}
void province_factories_table_body_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "table_body_control") {
					temp.ptr = table_body_control.get();
				} else
				if(cname == "icon") {
					temp.ptr = icon.get();
				} else
				if(cname == "details_button") {
					temp.ptr = details_button.get();
				} else
				{
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
					temp.ptr = make_province_factories_main(state);
				}
				if(cname == "table_body") {
					temp.ptr = make_province_factories_table_body(state);
				}
				if(cname == "table_header") {
					temp.ptr = make_province_factories_table_header(state);
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
void province_factories_table_body_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_factories::table_body"));
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
		if(child_data.name == "table_body_control") {
			table_body_control = std::make_unique<province_factories_table_body_table_body_control_t>();
			table_body_control->parent = this;
			auto cptr = table_body_control.get();
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
		if(child_data.name == "icon") {
			icon = std::make_unique<province_factories_table_body_icon_t>();
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
		} else 
		if(child_data.name == "details_button") {
			details_button = std::make_unique<province_factories_table_body_details_button_t>();
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
		if(child_data.name == ".tablocal_factories_list") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			main_section.read<std::string_view>(); // discard
			main_section.read<std::string_view>(); // discard
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_icon_column_start = running_w_total;
			col_section.read(local_factories_list_icon_column_width);
			running_w_total += local_factories_list_icon_column_width;
			col_section.read(local_factories_list_icon_column_text_color);
			col_section.read(local_factories_list_icon_header_text_color);
			col_section.read(local_factories_list_icon_text_alignment);
			local_factories_list_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_name_column_start = running_w_total;
			col_section.read(local_factories_list_name_column_width);
			running_w_total += local_factories_list_name_column_width;
			col_section.read(local_factories_list_name_column_text_color);
			col_section.read(local_factories_list_name_header_text_color);
			col_section.read(local_factories_list_name_text_alignment);
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_details_column_start = running_w_total;
			col_section.read(local_factories_list_details_column_width);
			running_w_total += local_factories_list_details_column_width;
			col_section.read(local_factories_list_details_column_text_color);
			col_section.read(local_factories_list_details_header_text_color);
			col_section.read(local_factories_list_details_text_alignment);
			local_factories_list_margin_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_margin_column_start = running_w_total;
			col_section.read(local_factories_list_margin_column_width);
			running_w_total += local_factories_list_margin_column_width;
			col_section.read(local_factories_list_margin_column_text_color);
			col_section.read(local_factories_list_margin_header_text_color);
			col_section.read(local_factories_list_margin_text_alignment);
			local_factories_list_hired_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_hired_column_start = running_w_total;
			col_section.read(local_factories_list_hired_column_width);
			running_w_total += local_factories_list_hired_column_width;
			col_section.read(local_factories_list_hired_column_text_color);
			col_section.read(local_factories_list_hired_header_text_color);
			col_section.read(local_factories_list_hired_text_alignment);
			local_factories_list_size_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_size_column_start = running_w_total;
			col_section.read(local_factories_list_size_column_width);
			running_w_total += local_factories_list_size_column_width;
			col_section.read(local_factories_list_size_column_text_color);
			col_section.read(local_factories_list_size_header_text_color);
			col_section.read(local_factories_list_size_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		{ } 
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN table_body::create
// END
}
std::unique_ptr<ui::element_base> make_province_factories_table_body(sys::state& state) {
	auto ptr = std::make_unique<province_factories_table_body_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result province_factories_table_header_table_header_control_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto table_source = (province_factories_main_t*)(parent->parent);
	if(x >= table_source->local_factories_list_name_column_start && x < table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->local_factories_list_name_sort_direction;
		table_source->local_factories_list_name_sort_direction = 0;
		table_source->local_factories_list_margin_sort_direction = 0;
		table_source->local_factories_list_hired_sort_direction = 0;
		table_source->local_factories_list_size_sort_direction = 0;
		table_source->local_factories_list_name_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->local_factories_list_margin_column_start && x < table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->local_factories_list_margin_sort_direction;
		table_source->local_factories_list_name_sort_direction = 0;
		table_source->local_factories_list_margin_sort_direction = 0;
		table_source->local_factories_list_hired_sort_direction = 0;
		table_source->local_factories_list_size_sort_direction = 0;
		table_source->local_factories_list_margin_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->local_factories_list_hired_column_start && x < table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->local_factories_list_hired_sort_direction;
		table_source->local_factories_list_name_sort_direction = 0;
		table_source->local_factories_list_margin_sort_direction = 0;
		table_source->local_factories_list_hired_sort_direction = 0;
		table_source->local_factories_list_size_sort_direction = 0;
		table_source->local_factories_list_hired_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	if(x >= table_source->local_factories_list_size_column_start && x < table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
		auto old_direction = table_source->local_factories_list_size_sort_direction;
		table_source->local_factories_list_name_sort_direction = 0;
		table_source->local_factories_list_margin_sort_direction = 0;
		table_source->local_factories_list_hired_sort_direction = 0;
		table_source->local_factories_list_size_sort_direction = 0;
		table_source->local_factories_list_size_sort_direction = int8_t(old_direction <= 0 ? 1 : -1);
		parent->parent->impl_on_update(state);
	}
	return ui::message_result::consumed;}
ui::message_result province_factories_table_header_table_header_control_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_factories_table_header_table_header_control_t::tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept {
	auto table_source = (province_factories_main_t*)(parent->parent);
	if(x >= table_source->local_factories_list_icon_column_start && x < table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width) {
	}
	if(x >= table_source->local_factories_list_name_column_start && x < table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width) {
	}
	if(x >= table_source->local_factories_list_details_column_start && x < table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width) {
	}
	if(x >= table_source->local_factories_list_margin_column_start && x < table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width) {
	}
	if(x >= table_source->local_factories_list_hired_column_start && x < table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width) {
	}
	if(x >= table_source->local_factories_list_size_column_start && x < table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width) {
	}
		ident = -1;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
}
void province_factories_table_header_table_header_control_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	auto table_source = (province_factories_main_t*)(parent->parent);
	if(x >=  table_source->local_factories_list_icon_column_start && x <  table_source->local_factories_list_icon_column_start +  table_source->local_factories_list_icon_column_width) {
	}
	if(x >=  table_source->local_factories_list_name_column_start && x <  table_source->local_factories_list_name_column_start +  table_source->local_factories_list_name_column_width) {
	}
	if(x >=  table_source->local_factories_list_details_column_start && x <  table_source->local_factories_list_details_column_start +  table_source->local_factories_list_details_column_width) {
	}
	if(x >=  table_source->local_factories_list_margin_column_start && x <  table_source->local_factories_list_margin_column_start +  table_source->local_factories_list_margin_column_width) {
	}
	if(x >=  table_source->local_factories_list_hired_column_start && x <  table_source->local_factories_list_hired_column_start +  table_source->local_factories_list_hired_column_width) {
	}
	if(x >=  table_source->local_factories_list_size_column_start && x <  table_source->local_factories_list_size_column_start +  table_source->local_factories_list_size_column_width) {
	}
}
void province_factories_table_header_table_header_control_t::on_reset_text(sys::state& state) noexcept {
	auto table_source = (province_factories_main_t*)(parent->parent);
	{
	name_cached_text = text::produce_simple_string(state, table_source->local_factories_list_name_header_text_key);
	 name_internal_layout.contents.clear();
	 name_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  name_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_name_column_width - 0 - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_name_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, name_cached_text);
	}
	{
	margin_cached_text = text::produce_simple_string(state, table_source->local_factories_list_margin_header_text_key);
	 margin_internal_layout.contents.clear();
	 margin_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  margin_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_margin_column_width - 0 - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_margin_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, margin_cached_text);
	}
	{
	hired_cached_text = text::produce_simple_string(state, table_source->local_factories_list_hired_header_text_key);
	 hired_internal_layout.contents.clear();
	 hired_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  hired_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_hired_column_width - 0 - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_hired_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, hired_cached_text);
	}
	{
	size_cached_text = text::produce_simple_string(state, table_source->local_factories_list_size_header_text_key);
	 size_internal_layout.contents.clear();
	 size_internal_layout.number_of_lines = 0;
	text::single_line_layout sl{  size_internal_layout, text::layout_parameters{ 0, 0, int16_t(table_source->local_factories_list_size_column_width - 0 - 20), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, false, 1.0f * 20), 0, table_source->local_factories_list_size_text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
	sl.add_text(state, size_cached_text);
	}
}
void province_factories_table_header_table_header_control_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	auto fh = text::make_font_id(state, false, 1.0f * 20);
	auto linesz = state.font_collection.line_height(state, fh); 
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto table_source = (province_factories_main_t*)(parent->parent);
	auto abs_location = ui::get_absolute_location(state, *this);
	int32_t rel_mouse_x = int32_t(state.mouse_x_position / state.user_settings.ui_scale) - abs_location.x;
	int32_t rel_mouse_y = int32_t(state.mouse_y_position / state.user_settings.ui_scale) - abs_location.y;
	auto ink_color =template_id != -1 ? ogl::color3f(state.ui_templates.colors[state.ui_templates.table_t[template_id].table_color]) : ogl::color3f{}; 	bool col_um_icon = rel_mouse_x >= table_source->local_factories_list_icon_column_start && rel_mouse_x < (table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_icon_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start + table_source->local_factories_list_icon_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_icon){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y), float(table_source->local_factories_list_icon_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_icon_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_icon_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_icon = state.ui_templates.colors[table_source->local_factories_list_icon_header_text_color]; 	bool col_um_name = rel_mouse_x >= table_source->local_factories_list_name_column_start && rel_mouse_x < (table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_name_column_start), float(y), float(table_source->local_factories_list_name_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->local_factories_list_name_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_name_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start + table_source->local_factories_list_name_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_name){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y), float(table_source->local_factories_list_name_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_name_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_name_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_name = state.ui_templates.colors[table_source->local_factories_list_name_header_text_color]; 	if(table_source->local_factories_list_name_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_name_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->local_factories_list_name_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_name_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!name_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : name_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_name_column_start + 0 + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_name.r, col_color_name.g, col_color_name.b }, ogl::color_modification::none);
		}
	}
	bool col_um_details = rel_mouse_x >= table_source->local_factories_list_details_column_start && rel_mouse_x < (table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width);
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_details){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_details_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_details){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start + table_source->local_factories_list_details_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_details){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y), float(table_source->local_factories_list_details_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_details_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_details_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_details = state.ui_templates.colors[table_source->local_factories_list_details_header_text_color]; 	bool col_um_margin = rel_mouse_x >= table_source->local_factories_list_margin_column_start && rel_mouse_x < (table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_margin) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_margin_column_start), float(y), float(table_source->local_factories_list_margin_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->local_factories_list_margin_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_margin){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_margin_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_margin){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start + table_source->local_factories_list_margin_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_margin){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y), float(table_source->local_factories_list_margin_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_margin_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_margin_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_margin = state.ui_templates.colors[table_source->local_factories_list_margin_header_text_color]; 	if(table_source->local_factories_list_margin_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_margin_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->local_factories_list_margin_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_margin_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!margin_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : margin_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_margin_column_start + 0 + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_margin.r, col_color_margin.g, col_color_margin.b }, ogl::color_modification::none);
		}
	}
	bool col_um_hired = rel_mouse_x >= table_source->local_factories_list_hired_column_start && rel_mouse_x < (table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_hired) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_hired_column_start), float(y), float(table_source->local_factories_list_hired_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->local_factories_list_hired_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_hired){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_hired_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_hired){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start + table_source->local_factories_list_hired_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_hired){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y), float(table_source->local_factories_list_hired_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_hired_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_hired_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_hired = state.ui_templates.colors[table_source->local_factories_list_hired_header_text_color]; 	if(table_source->local_factories_list_hired_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_hired_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->local_factories_list_hired_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_hired_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!hired_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : hired_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_hired_column_start + 0 + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_hired.r, col_color_hired.g, col_color_hired.b }, ogl::color_modification::none);
		}
	}
	bool col_um_size = rel_mouse_x >= table_source->local_factories_list_size_column_start && rel_mouse_x < (table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width);
		{
		auto bg = template_id != -1 ? ((0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size) ? state.ui_templates.table_t[template_id].active_header_bg : state.ui_templates.table_t[template_id].interactable_header_bg) : -1;
		if(bg != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_size_column_start), float(y), float(table_source->local_factories_list_size_column_width), float(base_data.size.y), state.ui_templates.backgrounds[bg].renders.get_render(state, float(table_source->local_factories_list_size_column_width) / float(table_source->grid_size), float(base_data.size.y) / float(table_source->grid_size), int32_t(table_source->grid_size), state.user_settings.ui_scale)); 
		}
	if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width - 2), float(y + base_data.size.y - 2), float(2), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y), float(1), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width - 2), float(y), float(2), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y + base_data.size.y - 2), float(1), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width * 0.25f), float(y + base_data.size.y - 1), float(table_source->local_factories_list_size_column_width * 0.5f), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y) && col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y), float(1), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start + table_source->local_factories_list_size_column_width - 2), float(y), float(2), float(base_data.size.y), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	} else if(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y && !col_um_size){
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y), float(table_source->local_factories_list_size_column_width), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
		ogl::render_alpha_colored_rect(state, float(x + table_source->local_factories_list_size_column_start), float(y + base_data.size.y - 2), float(table_source->local_factories_list_size_column_width), float(2), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
	auto col_color_size = state.ui_templates.colors[table_source->local_factories_list_size_header_text_color]; 	if(table_source->local_factories_list_size_sort_direction > 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_increasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_size_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(table_source->local_factories_list_size_sort_direction < 0) {
		auto icon = template_id != -1 ? state.ui_templates.table_t[template_id].arrow_decreasing : -1;
		if(icon != -1)
		ogl::render_textured_rect_direct(state, float(x + table_source->local_factories_list_size_column_start + 0), float(y + base_data.size.y / 2 - 10), float(10), float(20), state.ui_templates.icons[icon].renders.get_render(state, 10, 20, state.user_settings.ui_scale, ink_color.r, ink_color.g, ink_color.b)); 
	}
	if(!size_internal_layout.contents.empty() && linesz > 0.0f) {
		for(auto& t : size_internal_layout.contents) {
			ui::render_text_chunk(state, t, float(x) + t.x + table_source->local_factories_list_size_column_start + 0 + 10, float(y + int32_t(ycentered)),  fh, ogl::color3f{ col_color_size.r, col_color_size.g, col_color_size.b }, ogl::color_modification::none);
		}
	}
	if(!(0 <= rel_mouse_y && rel_mouse_y < base_data.size.y)){
	ogl::render_alpha_colored_rect(state, float(x), float(y + base_data.size.y - 1), float(base_data.size.x), float(1), ink_color.r, ink_color.g, ink_color.b, 1.0f);
	}
}
void province_factories_table_header_table_header_control_t::on_update(sys::state& state) noexcept {
	province_factories_table_header_t& table_header = *((province_factories_table_header_t*)(parent)); 
	province_factories_main_t& main = *((province_factories_main_t*)(parent->parent)); 
// BEGIN table_header::table_header_control::update
// END
}
void province_factories_table_header_table_header_control_t::on_create(sys::state& state) noexcept {
// BEGIN table_header::table_header_control::create
// END
}
ui::message_result province_factories_table_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result province_factories_table_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void province_factories_table_header_t::on_update(sys::state& state) noexcept {
	province_factories_main_t& main = *((province_factories_main_t*)(parent->parent)); 
// BEGIN table_header::update
// END
	remake_layout(state, true);
}
void province_factories_table_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "table_header_control") {
					temp.ptr = table_header_control.get();
				} else
				{
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
					temp.ptr = make_province_factories_main(state);
				}
				if(cname == "table_body") {
					temp.ptr = make_province_factories_table_body(state);
				}
				if(cname == "table_header") {
					temp.ptr = make_province_factories_table_header(state);
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
void province_factories_table_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("province_factories::table_header"));
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
		if(child_data.name == "table_header_control") {
			table_header_control = std::make_unique<province_factories_table_header_table_header_control_t>();
			table_header_control->parent = this;
			auto cptr = table_header_control.get();
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
		if(child_data.name == ".tablocal_factories_list") {
			int16_t running_w_total = 0;
			auto tbuffer = serialization::in_buffer(pending_children.back().data, pending_children.back().size);
			auto main_section = tbuffer.read_section();
			main_section.read<std::string_view>(); // discard name 
			main_section.read<std::string_view>(); // discard
			main_section.read<std::string_view>(); // discard
			main_section.read<ogl::color3f>();
			auto col_section = tbuffer.read_section();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_icon_column_start = running_w_total;
			col_section.read(local_factories_list_icon_column_width);
			running_w_total += local_factories_list_icon_column_width;
			col_section.read(local_factories_list_icon_column_text_color);
			col_section.read(local_factories_list_icon_header_text_color);
			col_section.read(local_factories_list_icon_text_alignment);
			local_factories_list_name_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_name_column_start = running_w_total;
			col_section.read(local_factories_list_name_column_width);
			running_w_total += local_factories_list_name_column_width;
			col_section.read(local_factories_list_name_column_text_color);
			col_section.read(local_factories_list_name_header_text_color);
			col_section.read(local_factories_list_name_text_alignment);
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_details_column_start = running_w_total;
			col_section.read(local_factories_list_details_column_width);
			running_w_total += local_factories_list_details_column_width;
			col_section.read(local_factories_list_details_column_text_color);
			col_section.read(local_factories_list_details_header_text_color);
			col_section.read(local_factories_list_details_text_alignment);
			local_factories_list_margin_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_margin_column_start = running_w_total;
			col_section.read(local_factories_list_margin_column_width);
			running_w_total += local_factories_list_margin_column_width;
			col_section.read(local_factories_list_margin_column_text_color);
			col_section.read(local_factories_list_margin_header_text_color);
			col_section.read(local_factories_list_margin_text_alignment);
			local_factories_list_hired_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_hired_column_start = running_w_total;
			col_section.read(local_factories_list_hired_column_width);
			running_w_total += local_factories_list_hired_column_width;
			col_section.read(local_factories_list_hired_column_text_color);
			col_section.read(local_factories_list_hired_header_text_color);
			col_section.read(local_factories_list_hired_text_alignment);
			local_factories_list_size_header_text_key = col_section.read<std::string_view>();
			col_section.read<std::string_view>(); // discard
			col_section.read<std::string_view>(); // discard
			local_factories_list_size_column_start = running_w_total;
			col_section.read(local_factories_list_size_column_width);
			running_w_total += local_factories_list_size_column_width;
			col_section.read(local_factories_list_size_column_text_color);
			col_section.read(local_factories_list_size_header_text_color);
			col_section.read(local_factories_list_size_text_alignment);
			pending_children.pop_back(); continue;
		} else 
		{ } 
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN table_header::create
// END
}
std::unique_ptr<ui::element_base> make_province_factories_table_header(sys::state& state) {
	auto ptr = std::make_unique<province_factories_table_header_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
}
