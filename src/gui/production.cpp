// BEGIN prelude
#include "alice_ui.hpp"
#include "system_state.hpp"
#include "gui_deserialize.hpp"
#include "lua_alice_api.hpp"
#include "economy_production.hpp"
#include "labour_details.hpp"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#endif

namespace alice_ui {
enum class prod_category : uint8_t {
	production, consumption, employment, details
};
enum class prod_source : uint8_t {
	combined, factory, artisan, rgo
};
enum class max_option : uint8_t {
	max_output, max_employment
};
enum class warning_option : uint8_t{
	labor, inputs
};
struct pseudo_commodity_set {
	struct item {
		dcon::commodity_id c;
		float amount = 0;
	};
	std::vector<item> contents;
	void add(dcon::commodity_id c, float amount) {
		if(!c)
			return;
		if(amount == 0)
			return;
		for(auto& v : contents) {
			if(v.c == c) {
				v.amount += amount;
				return;
			}
		}
		contents.push_back(item{ c, amount });
	}
	void sort() {
		std::sort(contents.begin(), contents.end(), [&](item const& a, item const& b) { return b.amount < a.amount; });
	}
};

std::unique_ptr<ui::element_base> make_production_rh_state_item(sys::state& state);

class rh_production_map_items : public rh_map_items {
public:
	rh_production_map_items() {
		grid_size = 8;
		base_data.size.x = int16_t(400);
		base_data.position.x = int16_t(-400);
		base_data.flags = uint8_t(ui::orientation::upper_right);
	}
	std::unique_ptr<ui::element_base> make_item(sys::state& state) override {
		return make_production_rh_state_item(state);
	}
	bool province_filter(sys::state& state, dcon::province_id p) override {
		if(state.world.province_get_nation_from_province_ownership(p) == state.local_player_nation) {
			return state.world.province_get_state_membership(p).get_capital() == p;
		}
		return false;
	}
	void update_item(sys::state& state, ui::element_base& item, dcon::province_id p) override {
		dcon::state_instance_id* ptr = (dcon::state_instance_id*)(item.get_by_name(state, "for_state"));
		*ptr = state.world.province_get_state_membership(p);
		item.impl_on_update(state);
	}
	bool province_is_selected(sys::state& state, dcon::province_id p) override {
		return state.map_state.under_mouse_province && state.world.province_get_state_membership(state.map_state.under_mouse_province) == state.world.province_get_state_membership(p);
	}
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			rh_map_items::viewport.top_left.x = int16_t(432 + grid_size);
			rh_map_items::viewport.top_left.y = int16_t(grid_size + 138);
			rh_map_items::viewport.size.x = int16_t(parent->base_data.size.x - 432 - 400 - grid_size * 2);
			rh_map_items::viewport.size.y = int16_t(parent->base_data.size.y - grid_size * 2 - 138);
		}
		rh_map_items::on_update(state);
	}
};

std::unique_ptr<ui::element_base> make_production_rh_view(sys::state& state) {
	return std::make_unique< rh_production_map_items>();
}

}

// END

namespace alice_ui {
struct production_main_view_dropdown_t;
struct production_main_location_dropdown_t;
struct production_main_filter_dropdown_t;
struct production_main_combine_toggle_t;
struct production_main_t;
struct production_primary_row_main_commodity_icon_t;
struct production_primary_row_name_t;
struct production_primary_row_amount_t;
struct production_primary_row_t;
struct production_sub_item_row_commodity_icon_t;
struct production_sub_item_row_general_icon_t;
struct production_sub_item_row_amount_t;
struct production_sub_item_row_sub_items_label_t;
struct production_sub_item_row_t;
struct production_category_header_header_text_t;
struct production_category_header_t;
struct production_small_divider_t;
struct production_category_option_contents_t;
struct production_category_option_t;
struct production_location_option_content_t;
struct production_location_option_t;
struct production_filter_option_commodity_icon_t;
struct production_filter_option_filter_name_t;
struct production_filter_option_t;
struct production_max_item_description_t;
struct production_max_item_amount_label_t;
struct production_max_item_t;
struct production_shortage_item_description_t;
struct production_shortage_item_amount_label_t;
struct production_shortage_item_warning_icon_t;
struct production_shortage_item_t;
struct production_current_specialization_row_amount_label_t;
struct production_current_specialization_row_rankings_button_t;
struct production_current_specialization_row_t;
struct production_specialization_priority_row_left_t;
struct production_specialization_priority_row_right_t;
struct production_specialization_priority_row_amount_t;
struct production_specialization_priority_row_t;
struct production_spec_rankings_popup_t;
struct production_spec_rankings_item_amount_t;
struct production_spec_rankings_item_flag_control_t;
struct production_spec_rankings_item_t;
struct production_rh_state_item_state_name_t;
struct production_rh_state_item_population_amount_t;
struct production_rh_state_item_directives_drop_target_t;
struct production_rh_state_item_t;
struct production_dad_source_special_dad_button_t;
struct production_dad_source_special_t;
struct production_dad_source_commodity_dad_button_t;
struct production_dad_source_commodity_t;
struct production_directive_special_remove_button_t;
struct production_directive_special_t;
struct production_directive_commodity_remove_button_t;
struct production_directive_commodity_t;
struct production_directives_window_directives_drop_target_t;
struct production_directives_window_t;
struct production_main_view_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN main::view_dropdown::variables
// END
	struct category_option_option { prod_category category; };
	std::vector<category_option_option> list_contents;
	std::vector<std::unique_ptr<production_category_option_t >> list_pool;
	std::unique_ptr<production_category_option_t> label_window_internal;
	void add_item( prod_category category);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_main_location_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN main::location_dropdown::variables
// END
	struct location_option_option { dcon::state_instance_id location; };
	std::vector<location_option_option> list_contents;
	std::vector<std::unique_ptr<production_location_option_t >> list_pool;
	std::unique_ptr<production_location_option_t> label_window_internal;
	void add_item( dcon::state_instance_id location);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_main_filter_dropdown_t : public alice_ui::template_drop_down_control {
// BEGIN main::filter_dropdown::variables
// END
	struct filter_option_option { dcon::commodity_id commodity; };
	std::vector<filter_option_option> list_contents;
	std::vector<std::unique_ptr<production_filter_option_t >> list_pool;
	std::unique_ptr<production_filter_option_t> label_window_internal;
	void add_item( dcon::commodity_id commodity);
	ui::element_base* get_nth_item(sys::state& state, int32_t id, int32_t pool_id) override;
	void quiet_on_selection(sys::state& state, int32_t id);
	void on_selection(sys::state& state, int32_t id) override;
	void clear_list();
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_main_combine_toggle_t : public alice_ui::template_toggle_button {
// BEGIN main::combine_toggle::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_main_main_list_t : public layout_generator {
// BEGIN main::main_list::variables
// END
	struct primary_row_option { dcon::commodity_id commodity_type; prod_category category_type; prod_source about; };
	std::vector<std::unique_ptr<ui::element_base>> primary_row_pool;
	int32_t primary_row_pool_used = 0;
	void add_primary_row( dcon::commodity_id commodity_type,  prod_category category_type,  prod_source about);
	struct sub_item_row_option { dcon::commodity_id main_commodity; int32_t secondary_type; prod_category category_type; bool is_first_in_subsection; prod_source about; };
	std::vector<std::unique_ptr<ui::element_base>> sub_item_row_pool;
	int32_t sub_item_row_pool_used = 0;
	void add_sub_item_row( dcon::commodity_id main_commodity,  int32_t secondary_type,  prod_category category_type,  bool is_first_in_subsection,  prod_source about);
	struct category_header_option { prod_source category; };
	std::vector<std::unique_ptr<ui::element_base>> category_header_pool;
	int32_t category_header_pool_used = 0;
	void add_category_header( prod_source category);
	struct small_divider_option { };
	std::vector<std::unique_ptr<ui::element_base>> small_divider_pool;
	int32_t small_divider_pool_used = 0;
	void add_small_divider();
	struct max_item_option { float amount; max_option kind; };
	std::vector<std::unique_ptr<ui::element_base>> max_item_pool;
	int32_t max_item_pool_used = 0;
	void add_max_item( float amount,  max_option kind);
	struct shortage_item_option { float amount; warning_option kind; };
	std::vector<std::unique_ptr<ui::element_base>> shortage_item_pool;
	int32_t shortage_item_pool_used = 0;
	void add_shortage_item( float amount,  warning_option kind);
	struct current_specialization_row_option { dcon::factory_type_id for_type; };
	std::vector<std::unique_ptr<ui::element_base>> current_specialization_row_pool;
	int32_t current_specialization_row_pool_used = 0;
	void add_current_specialization_row( dcon::factory_type_id for_type);
	struct specialization_priority_row_option { dcon::factory_type_id for_type; };
	std::vector<std::unique_ptr<ui::element_base>> specialization_priority_row_pool;
	int32_t specialization_priority_row_pool_used = 0;
	void add_specialization_priority_row( dcon::factory_type_id for_type);
	std::vector<std::variant<std::monostate, primary_row_option, sub_item_row_option, category_header_option, small_divider_option, max_item_option, shortage_item_option, current_specialization_row_option, specialization_priority_row_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct production_primary_row_main_commodity_icon_t : public alice_ui::legacy_commodity_icon {
// BEGIN primary_row::main_commodity_icon::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_primary_row_name_t : public alice_ui::template_label {
// BEGIN primary_row::name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_primary_row_amount_t : public alice_ui::template_label {
// BEGIN primary_row::amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_sub_item_row_commodity_icon_t : public alice_ui::legacy_commodity_icon {
// BEGIN sub_item_row::commodity_icon::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_sub_item_row_general_icon_t : public alice_ui::template_icon_graphic {
// BEGIN sub_item_row::general_icon::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_sub_item_row_amount_t : public alice_ui::template_label {
// BEGIN sub_item_row::amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_sub_item_row_sub_items_label_t : public alice_ui::template_label {
// BEGIN sub_item_row::sub_items_label::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_category_header_header_text_t : public alice_ui::template_label {
// BEGIN category_header::header_text::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_category_option_contents_t : public alice_ui::template_label {
// BEGIN category_option::contents::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_location_option_content_t : public alice_ui::template_label {
// BEGIN location_option::content::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_filter_option_commodity_icon_t : public alice_ui::legacy_commodity_icon {
// BEGIN filter_option::commodity_icon::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_filter_option_filter_name_t : public alice_ui::template_label {
// BEGIN filter_option::filter_name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_max_item_description_t : public alice_ui::template_label {
// BEGIN max_item::description::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_max_item_amount_label_t : public alice_ui::template_label {
// BEGIN max_item::amount_label::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_shortage_item_description_t : public alice_ui::template_label {
// BEGIN shortage_item::description::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_shortage_item_amount_label_t : public alice_ui::template_label {
// BEGIN shortage_item::amount_label::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_shortage_item_warning_icon_t : public alice_ui::template_icon_graphic {
// BEGIN shortage_item::warning_icon::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_current_specialization_row_amount_label_t : public alice_ui::template_label {
// BEGIN current_specialization_row::amount_label::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_current_specialization_row_rankings_button_t : public alice_ui::template_icon_button {
// BEGIN current_specialization_row::rankings_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_specialization_priority_row_left_t : public alice_ui::template_icon_button {
// BEGIN specialization_priority_row::left::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_specialization_priority_row_right_t : public alice_ui::template_icon_button {
// BEGIN specialization_priority_row::right::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_specialization_priority_row_amount_t : public alice_ui::template_label {
// BEGIN specialization_priority_row::amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_spec_rankings_popup_top_rankings_t : public layout_generator {
// BEGIN spec_rankings_popup::top_rankings::variables
// END
	struct spec_rankings_item_option { dcon::nation_id nation_for; };
	std::vector<std::unique_ptr<ui::element_base>> spec_rankings_item_pool;
	int32_t spec_rankings_item_pool_used = 0;
	void add_spec_rankings_item( dcon::nation_id nation_for);
	std::vector<std::variant<std::monostate, spec_rankings_item_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct production_spec_rankings_item_amount_t : public alice_ui::template_label {
// BEGIN spec_rankings_item::amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_spec_rankings_item_flag_control_t : public ui::element_base {
// BEGIN spec_rankings_item::flag_control::variables
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
struct production_rh_state_item_state_name_t : public alice_ui::template_label {
// BEGIN rh_state_item::state_name::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_rh_state_item_population_amount_t : public alice_ui::template_label {
// BEGIN rh_state_item::population_amount::variables
// END
	void on_update(sys::state& state) noexcept override;
};
struct production_rh_state_item_directives_drop_target_t : public alice_ui::drag_and_drop_target_control {
// BEGIN rh_state_item::directives_drop_target::variables
// END
	bool recieve_drag_and_drop(sys::state& state, std::any& data, ui::drag_and_drop_data data_type, ui::drag_and_drop_target sub_target, bool shift_held_down) noexcept override;
};
struct production_rh_state_item_active_directives_t : public layout_generator {
// BEGIN rh_state_item::active_directives::variables
// END
	struct directive_special_option { dcon::production_directive_id content; dcon::state_instance_id for_state; };
	std::vector<std::unique_ptr<ui::element_base>> directive_special_pool;
	int32_t directive_special_pool_used = 0;
	void add_directive_special( dcon::production_directive_id content,  dcon::state_instance_id for_state);
	struct directive_commodity_option { dcon::commodity_id content; dcon::state_instance_id for_state; };
	std::vector<std::unique_ptr<ui::element_base>> directive_commodity_pool;
	int32_t directive_commodity_pool_used = 0;
	void add_directive_commodity( dcon::commodity_id content,  dcon::state_instance_id for_state);
	std::vector<std::variant<std::monostate, directive_special_option, directive_commodity_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct production_dad_source_special_dad_button_t : public alice_ui::template_icon_button {
// BEGIN dad_source_special::dad_button::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_dad_source_commodity_dad_button_t : public alice_ui::template_icon_button {
// BEGIN dad_source_commodity::dad_button::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_directive_special_remove_button_t : public alice_ui::template_icon_button {
// BEGIN directive_special::remove_button::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_directive_commodity_remove_button_t : public alice_ui::template_icon_button {
// BEGIN directive_commodity::remove_button::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct production_directives_window_directives_drop_target_t : public alice_ui::drag_and_drop_target_control {
// BEGIN directives_window::directives_drop_target::variables
// END
	bool recieve_drag_and_drop(sys::state& state, std::any& data, ui::drag_and_drop_data data_type, ui::drag_and_drop_target sub_target, bool shift_held_down) noexcept override;
};
struct production_directives_window_all_directives_t : public layout_generator {
// BEGIN directives_window::all_directives::variables
// END
	struct dad_source_commodity_option { dcon::commodity_id content; };
	std::vector<std::unique_ptr<ui::element_base>> dad_source_commodity_pool;
	int32_t dad_source_commodity_pool_used = 0;
	void add_dad_source_commodity( dcon::commodity_id content);
	struct dad_source_special_option { dcon::production_directive_id content; };
	std::vector<std::unique_ptr<ui::element_base>> dad_source_special_pool;
	int32_t dad_source_special_pool_used = 0;
	void add_dad_source_special( dcon::production_directive_id content);
	std::vector<std::variant<std::monostate, dad_source_commodity_option, dad_source_special_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct production_directives_window_active_directives_t : public layout_generator {
// BEGIN directives_window::active_directives::variables
// END
	struct directive_special_option { dcon::production_directive_id content; dcon::state_instance_id for_state; };
	std::vector<std::unique_ptr<ui::element_base>> directive_special_pool;
	int32_t directive_special_pool_used = 0;
	void add_directive_special( dcon::production_directive_id content,  dcon::state_instance_id for_state);
	struct directive_commodity_option { dcon::commodity_id content; dcon::state_instance_id for_state; };
	std::vector<std::unique_ptr<ui::element_base>> directive_commodity_pool;
	int32_t directive_commodity_pool_used = 0;
	void add_directive_commodity( dcon::commodity_id content,  dcon::state_instance_id for_state);
	std::vector<std::variant<std::monostate, directive_special_option, directive_commodity_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct production_main_t : public layout_window_element {
// BEGIN main::variables
	bool combine_types = false;
	prod_category selected_categry = prod_category::production;
	dcon::commodity_id filter;
// END
	dcon::state_instance_id selected_location;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_main_view_dropdown_t> view_dropdown;
	std::unique_ptr<production_main_location_dropdown_t> location_dropdown;
	std::unique_ptr<production_main_filter_dropdown_t> filter_dropdown;
	std::unique_ptr<production_main_combine_toggle_t> combine_toggle;
	std::unique_ptr<template_label> title;
	std::unique_ptr<template_icon_graphic> view_tt_icon;
	std::unique_ptr<template_icon_graphic> location_tt_icon;
	std::unique_ptr<template_icon_graphic> combine_tt_icon;
	std::unique_ptr<template_icon_graphic> filter_tt_icon;
	production_main_main_list_t main_list;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::consumed;
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "selected_location") {
			return (void*)(&selected_location);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_main(sys::state& state);
struct production_primary_row_t : public layout_window_element {
// BEGIN primary_row::variables
// END
	dcon::commodity_id commodity_type;
	prod_category category_type;
	prod_source about;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_primary_row_main_commodity_icon_t> main_commodity_icon;
	std::unique_ptr<production_primary_row_name_t> name;
	std::unique_ptr<production_primary_row_amount_t> amount;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "commodity_type") {
			return (void*)(&commodity_type);
		}
		if(name_parameter == "category_type") {
			return (void*)(&category_type);
		}
		if(name_parameter == "about") {
			return (void*)(&about);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_primary_row(sys::state& state);
struct production_sub_item_row_t : public layout_window_element {
// BEGIN sub_item_row::variables
// END
	dcon::commodity_id main_commodity;
	int32_t secondary_type;
	prod_category category_type;
	bool is_first_in_subsection;
	prod_source about;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_sub_item_row_commodity_icon_t> commodity_icon;
	std::unique_ptr<production_sub_item_row_general_icon_t> general_icon;
	std::unique_ptr<production_sub_item_row_amount_t> amount;
	std::unique_ptr<production_sub_item_row_sub_items_label_t> sub_items_label;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "main_commodity") {
			return (void*)(&main_commodity);
		}
		if(name_parameter == "secondary_type") {
			return (void*)(&secondary_type);
		}
		if(name_parameter == "category_type") {
			return (void*)(&category_type);
		}
		if(name_parameter == "is_first_in_subsection") {
			return (void*)(&is_first_in_subsection);
		}
		if(name_parameter == "about") {
			return (void*)(&about);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_sub_item_row(sys::state& state);
struct production_category_header_t : public layout_window_element {
// BEGIN category_header::variables
// END
	prod_source category;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_category_header_header_text_t> header_text;
	std::unique_ptr<template_bg_graphic> dash;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "category") {
			return (void*)(&category);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_category_header(sys::state& state);
struct production_small_divider_t : public layout_window_element {
// BEGIN small_divider::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_bg_graphic> Control1;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_production_small_divider(sys::state& state);
struct production_category_option_t : public layout_window_element {
// BEGIN category_option::variables
// END
	prod_category category;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_category_option_contents_t> contents;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "category") {
			return (void*)(&category);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_category_option(sys::state& state);
struct production_location_option_t : public layout_window_element {
// BEGIN location_option::variables
// END
	dcon::state_instance_id location;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_location_option_content_t> content;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "location") {
			return (void*)(&location);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_location_option(sys::state& state);
struct production_filter_option_t : public layout_window_element {
// BEGIN filter_option::variables
// END
	dcon::commodity_id commodity;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_filter_option_commodity_icon_t> commodity_icon;
	std::unique_ptr<production_filter_option_filter_name_t> filter_name;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "commodity") {
			return (void*)(&commodity);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_filter_option(sys::state& state);
struct production_max_item_t : public layout_window_element {
// BEGIN max_item::variables
// END
	float amount;
	max_option kind;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_max_item_description_t> description;
	std::unique_ptr<production_max_item_amount_label_t> amount_label;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "amount") {
			return (void*)(&amount);
		}
		if(name_parameter == "kind") {
			return (void*)(&kind);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_max_item(sys::state& state);
struct production_shortage_item_t : public layout_window_element {
// BEGIN shortage_item::variables
// END
	float amount;
	warning_option kind;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_shortage_item_description_t> description;
	std::unique_ptr<production_shortage_item_amount_label_t> amount_label;
	std::unique_ptr<production_shortage_item_warning_icon_t> warning_icon;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "amount") {
			return (void*)(&amount);
		}
		if(name_parameter == "kind") {
			return (void*)(&kind);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_shortage_item(sys::state& state);
struct production_current_specialization_row_t : public layout_window_element {
// BEGIN current_specialization_row::variables
// END
	dcon::factory_type_id for_type;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> label;
	std::unique_ptr<production_current_specialization_row_amount_label_t> amount_label;
	std::unique_ptr<production_current_specialization_row_rankings_button_t> rankings_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "for_type") {
			return (void*)(&for_type);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_current_specialization_row(sys::state& state);
struct production_specialization_priority_row_t : public layout_window_element {
// BEGIN specialization_priority_row::variables
// END
	dcon::factory_type_id for_type;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> label;
	std::unique_ptr<production_specialization_priority_row_left_t> left;
	std::unique_ptr<production_specialization_priority_row_right_t> right;
	std::unique_ptr<production_specialization_priority_row_amount_t> amount;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "for_type") {
			return (void*)(&for_type);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_specialization_priority_row(sys::state& state);
struct production_spec_rankings_popup_t : public layout_window_element {
// BEGIN spec_rankings_popup::variables
// END
	dcon::factory_type_id for_type;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> header_label;
	std::unique_ptr<template_icon_graphic> globe_icon;
	production_spec_rankings_popup_top_rankings_t top_rankings;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "for_type") {
			return (void*)(&for_type);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_spec_rankings_popup(sys::state& state);
struct production_spec_rankings_item_t : public layout_window_element {
// BEGIN spec_rankings_item::variables
// END
	dcon::nation_id nation_for;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_spec_rankings_item_amount_t> amount;
	std::unique_ptr<production_spec_rankings_item_flag_control_t> flag_control;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "nation_for") {
			return (void*)(&nation_for);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_spec_rankings_item(sys::state& state);
struct production_rh_state_item_t : public layout_window_element {
// BEGIN rh_state_item::variables
	void on_hover(sys::state& state) noexcept override {
		state.map_state.under_mouse_province = state.world.state_instance_get_capital(for_state);
	}
// END
	dcon::state_instance_id for_state;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_rh_state_item_state_name_t> state_name;
	std::unique_ptr<template_icon_graphic> population_icon;
	std::unique_ptr<production_rh_state_item_population_amount_t> population_amount;
	std::unique_ptr<template_icon_graphic> consumption_icon;
	std::unique_ptr<alice_ui::legacy_commodity_icon> consumption1;
	std::unique_ptr<alice_ui::legacy_commodity_icon> consumption2;
	std::unique_ptr<alice_ui::legacy_commodity_icon> consumption3;
	std::unique_ptr<alice_ui::legacy_commodity_icon> consumption4;
	std::unique_ptr<template_icon_graphic> production_icon;
	std::unique_ptr<alice_ui::legacy_commodity_icon> production1;
	std::unique_ptr<alice_ui::legacy_commodity_icon> production2;
	std::unique_ptr<alice_ui::legacy_commodity_icon> production3;
	std::unique_ptr<alice_ui::legacy_commodity_icon> production4;
	std::unique_ptr<production_rh_state_item_directives_drop_target_t> directives_drop_target;
	production_rh_state_item_active_directives_t active_directives;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "for_state") {
			return (void*)(&for_state);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_rh_state_item(sys::state& state);
struct production_dad_source_special_t : public layout_window_element {
// BEGIN dad_source_special::variables
// END
	dcon::production_directive_id content;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_dad_source_special_dad_button_t> dad_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
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
std::unique_ptr<ui::element_base> make_production_dad_source_special(sys::state& state);
struct production_dad_source_commodity_t : public layout_window_element {
// BEGIN dad_source_commodity::variables
// END
	dcon::commodity_id content;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_dad_source_commodity_dad_button_t> dad_button;
	std::unique_ptr<alice_ui::legacy_commodity_icon> commodity_overlay;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
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
std::unique_ptr<ui::element_base> make_production_dad_source_commodity(sys::state& state);
struct production_directive_special_t : public layout_window_element {
// BEGIN directive_special::variables
// END
	dcon::production_directive_id content;
	dcon::state_instance_id for_state;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_directive_special_remove_button_t> remove_button;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "content") {
			return (void*)(&content);
		}
		if(name_parameter == "for_state") {
			return (void*)(&for_state);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_directive_special(sys::state& state);
struct production_directive_commodity_t : public layout_window_element {
// BEGIN directive_commodity::variables
// END
	dcon::commodity_id content;
	dcon::state_instance_id for_state;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_directive_commodity_remove_button_t> remove_button;
	std::unique_ptr<alice_ui::legacy_commodity_icon> commodity_overlay;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "content") {
			return (void*)(&content);
		}
		if(name_parameter == "for_state") {
			return (void*)(&for_state);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_production_directive_commodity(sys::state& state);
struct production_directives_window_t : public layout_window_element {
// BEGIN directives_window::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> header;
	std::unique_ptr<template_icon_graphic> general_explain_tt;
	std::unique_ptr<template_label> national_directives_header;
	std::unique_ptr<template_icon_graphic> national_directives_tt;
	std::unique_ptr<production_directives_window_directives_drop_target_t> directives_drop_target;
	production_directives_window_all_directives_t all_directives;
	production_directives_window_active_directives_t active_directives;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::consumed;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_production_directives_window(sys::state& state);
void production_main_main_list_t::add_primary_row(dcon::commodity_id commodity_type, prod_category category_type, prod_source about) {
	values.emplace_back(primary_row_option{commodity_type, category_type, about});
}
void production_main_main_list_t::add_sub_item_row(dcon::commodity_id main_commodity, int32_t secondary_type, prod_category category_type, bool is_first_in_subsection, prod_source about) {
	values.emplace_back(sub_item_row_option{main_commodity, secondary_type, category_type, is_first_in_subsection, about});
}
void production_main_main_list_t::add_category_header(prod_source category) {
	values.emplace_back(category_header_option{category});
}
void production_main_main_list_t::add_small_divider() {
	values.emplace_back(small_divider_option{});
}
void production_main_main_list_t::add_max_item(float amount, max_option kind) {
	values.emplace_back(max_item_option{amount, kind});
}
void production_main_main_list_t::add_shortage_item(float amount, warning_option kind) {
	values.emplace_back(shortage_item_option{amount, kind});
}
void production_main_main_list_t::add_current_specialization_row(dcon::factory_type_id for_type) {
	values.emplace_back(current_specialization_row_option{for_type});
}
void production_main_main_list_t::add_specialization_priority_row(dcon::factory_type_id for_type) {
	values.emplace_back(specialization_priority_row_option{for_type});
}
void  production_main_main_list_t::on_create(sys::state& state, layout_window_element* parent) {
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::main_list::on_create
// END
}
void  production_main_main_list_t::update(sys::state& state, layout_window_element* parent) {
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::main_list::update
	values.clear();
	switch(main.selected_categry) {
	case prod_category::production:
		if(main.combine_types) {
			pseudo_commodity_set seen_commodities;
			for(auto c : state.world.in_commodity) {
				if(main.filter && main.filter != c)
					continue;

				float total = 0;
				if(main.selected_location) {
					total = economy::factory_output(state, c, main.selected_location) + economy::artisan_output(state, c, main.selected_location) + economy::rgo_output(state, c, main.selected_location);
				} else {
					total = economy::factory_output(state, c, state.local_player_nation) + economy::artisan_output(state, c, state.local_player_nation) + economy::rgo_output(state, c, state.local_player_nation);
				}
				seen_commodities.add(c, total);
			}
			seen_commodities.sort();
			for(auto& i : seen_commodities.contents) {
				add_primary_row(i.c, prod_category::production, prod_source::combined);
				bool first = true;

				pseudo_commodity_set usages_f;
				for(auto c : state.world.in_commodity) {
					if(main.selected_location)
						usages_f.add(c, economy::estimate_factory_consumption_in_production(state, c, main.selected_location, i.c));
					else
						usages_f.add(c, economy::estimate_factory_consumption_in_production(state, c, state.local_player_nation, i.c));
				}
				usages_f.sort();
				for(auto& j : usages_f.contents) {
					add_sub_item_row(j.c, i.c.index(), prod_category::production, first, prod_source::factory);
					first = false;
				}

				pseudo_commodity_set usages_a;
				for(auto c : state.world.in_commodity) {
					if(main.selected_location)
						usages_a.add(c, economy::estimate_artisan_consumption_in_production(state, c, main.selected_location, i.c));
					else
						usages_a.add(c, economy::estimate_artisan_consumption_in_production(state, c, state.local_player_nation, i.c));
				}
				usages_a.sort();
				for(auto& j : usages_a.contents) {
					add_sub_item_row(j.c, i.c.index(), prod_category::production, first, prod_source::artisan);
					first = false;
				}

				add_small_divider();
			}
		} else {
			{
				pseudo_commodity_set seen_commodities;
				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;

					float total = 0;
					if(main.selected_location) {
						total = economy::factory_output(state, c, main.selected_location);
					} else {
						total = economy::factory_output(state, c, state.local_player_nation);
					}
					seen_commodities.add(c, total);
				}
				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::factory);

				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::production, prod_source::factory);
					bool first = true;

					pseudo_commodity_set usages_f;
					for(auto c : state.world.in_commodity) {
						if(main.selected_location)
							usages_f.add(c, economy::estimate_factory_consumption_in_production(state, c, main.selected_location, i.c));
						else
							usages_f.add(c, economy::estimate_factory_consumption_in_production(state, c, state.local_player_nation, i.c));
					}
					usages_f.sort();
					for(auto& j : usages_f.contents) {
						add_sub_item_row(j.c, i.c.index(), prod_category::production, first, prod_source::factory);
						first = false;
					}

					float total = 0;
					if(main.selected_location) {
						province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
							total += economy::factory_potential_output(state, i.c, p);
						});
					} else {
						for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
							total += economy::factory_potential_output(state, i.c, op.get_province());
						}
					}

					add_max_item(total, max_option::max_output);
					add_small_divider();
				}
			}
			{
				pseudo_commodity_set seen_commodities;
				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;

					float total = 0;
					if(main.selected_location) {
						total = economy::artisan_output(state, c, main.selected_location);
					} else {
						total = economy::artisan_output(state, c, state.local_player_nation);
					}
					seen_commodities.add(c, total);
				}
				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::artisan);

				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::production, prod_source::artisan);
					bool first = true;

					pseudo_commodity_set usages_f;
					for(auto c : state.world.in_commodity) {
						if(main.selected_location)
							usages_f.add(c, economy::estimate_artisan_consumption_in_production(state, c, main.selected_location, i.c));
						else
							usages_f.add(c, economy::estimate_artisan_consumption_in_production(state, c, state.local_player_nation, i.c));
					}
					usages_f.sort();
					for(auto& j : usages_f.contents) {
						add_sub_item_row(j.c, i.c.index(), prod_category::production, first, prod_source::artisan);
						first = false;
					}


					float total = 0;
					if(main.selected_location) {
						province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
							total += economy::artisan_potential_output(state, i.c, p);
						});
					} else {
						for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
							total += economy::artisan_potential_output(state, i.c, op.get_province());
						}
					}
					add_max_item(total, max_option::max_output);

					add_small_divider();
				}
			}
			{
				pseudo_commodity_set seen_commodities;
				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;

					float total = 0;
					if(main.selected_location) {
						total = economy::rgo_output(state, c, main.selected_location);
					} else {
						total = economy::rgo_output(state, c, state.local_player_nation);
					}
					seen_commodities.add(c, total);
				}
				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::rgo);

				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::production, prod_source::rgo);
					float total = 0;
					if(main.selected_location) {
						province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
							total += economy::rgo_potential_output(state, i.c, p);
						});
					} else {
						for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
							total += economy::rgo_potential_output(state, i.c, op.get_province());
						}
					}
					add_max_item(total, max_option::max_output);
					add_small_divider();
				}
			}
		}
		break;
	case prod_category::consumption:
		if(main.combine_types) {
			pseudo_commodity_set seen_commodities;
			for(auto c : state.world.in_commodity) {
				if(main.filter && main.filter != c)
					continue;

				float total = 0;
				if(main.selected_location) {
					total = economy::estimate_factory_consumption(state, c, main.selected_location) + economy::estimate_artisan_consumption(state, c, main.selected_location);
				} else {
					total = economy::estimate_factory_consumption(state, c, state.local_player_nation) + economy::estimate_artisan_consumption(state, c, state.local_player_nation);
				}
				seen_commodities.add(c, total);
			}
			seen_commodities.sort();
			for(auto& i : seen_commodities.contents) {
				add_primary_row(i.c, prod_category::consumption, prod_source::combined);
				bool first = true;

				pseudo_commodity_set usages_f;
				for(auto c : state.world.in_commodity) {
					if(main.selected_location)
						usages_f.add(c, economy::estimate_factory_consumption_in_production(state, i.c, main.selected_location, c));
					else
						usages_f.add(c, economy::estimate_factory_consumption_in_production(state, i.c, state.local_player_nation, c));
				}
				usages_f.sort();
				for(auto& j : usages_f.contents) {
					add_sub_item_row(j.c, i.c.index(), prod_category::consumption, first, prod_source::factory);
					first = false;
				}

				pseudo_commodity_set usages_a;
				for(auto c : state.world.in_commodity) {
					if(main.selected_location)
						usages_a.add(c, economy::estimate_artisan_consumption_in_production(state, i.c, main.selected_location, c));
					else
						usages_a.add(c, economy::estimate_artisan_consumption_in_production(state, i.c, state.local_player_nation, c));
				}
				usages_a.sort();
				for(auto& j : usages_a.contents) {
					add_sub_item_row(j.c, i.c.index(), prod_category::consumption, first, prod_source::artisan);
					first = false;
				}

				if(main.selected_location) {
					auto m = state.world.state_instance_get_market_from_local_market(main.selected_location);
					if(m) {
						auto sat = economy::demand_satisfaction(state, m, i.c);
						if(sat < 0.99f) {
							add_shortage_item(1.0f - sat, warning_option::inputs);
						}
					}
				} else {
					auto sat = economy::demand_satisfaction(state, i.c);
					if(sat < 0.99f) {
						add_shortage_item(1.0f - sat, warning_option::inputs);
					}
				}

				add_small_divider();
			}
		} else {
			{
				pseudo_commodity_set seen_commodities;
				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;

					float total = 0;
					if(main.selected_location) {
						total = economy::estimate_factory_consumption(state, c, main.selected_location);
					} else {
						total = economy::estimate_factory_consumption(state, c, state.local_player_nation);
					}
					seen_commodities.add(c, total);
				}
				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::factory);

				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::consumption, prod_source::factory);
					bool first = true;

					pseudo_commodity_set usages_f;
					for(auto c : state.world.in_commodity) {
						if(main.selected_location)
							usages_f.add(c, economy::estimate_factory_consumption_in_production(state, i.c, main.selected_location, c));
						else
							usages_f.add(c, economy::estimate_factory_consumption_in_production(state, i.c, state.local_player_nation, c));
					}
					usages_f.sort();
					for(auto& j : usages_f.contents) {
						add_sub_item_row(j.c, i.c.index(), prod_category::consumption, first, prod_source::factory);
						first = false;
					}

					if(main.selected_location) {
						auto m = state.world.state_instance_get_market_from_local_market(main.selected_location);
						if(m) {
							auto sat = economy::demand_satisfaction(state, m, i.c);
							if(sat < 0.99f) {
								add_shortage_item(1.0f - sat, warning_option::inputs);
							}
						}
					} else {
						auto sat = economy::demand_satisfaction(state, i.c);
						if(sat < 0.99f) {
							add_shortage_item(1.0f - sat, warning_option::inputs);
						}
					}

					add_small_divider();
				}
			}
			{
				pseudo_commodity_set seen_commodities;
				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;

					float total = 0;
					if(main.selected_location) {
						total = economy::estimate_artisan_consumption(state, c, main.selected_location);
					} else {
						total = economy::estimate_artisan_consumption(state, c, state.local_player_nation);
					}
					seen_commodities.add(c, total);
				}
				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::artisan);

				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::consumption, prod_source::artisan);
					bool first = true;

					pseudo_commodity_set usages_f;
					for(auto c : state.world.in_commodity) {
						if(main.selected_location)
							usages_f.add(c, economy::estimate_artisan_consumption_in_production(state, i.c, main.selected_location, c));
						else
							usages_f.add(c, economy::estimate_artisan_consumption_in_production(state, i.c, state.local_player_nation, c));
					}
					usages_f.sort();
					for(auto& j : usages_f.contents) {
						add_sub_item_row(j.c, i.c.index(), prod_category::consumption, first, prod_source::artisan);
						first = false;
					}

					if(main.selected_location) {
						auto m = state.world.state_instance_get_market_from_local_market(main.selected_location);
						if(m) {
							auto sat = economy::demand_satisfaction(state, m, i.c);
							if(sat < 0.99f) {
								add_shortage_item(1.0f - sat, warning_option::inputs);
							}
						}
					} else {
						auto sat = economy::demand_satisfaction(state, i.c);
						if(sat < 0.99f) {
							add_shortage_item(1.0f - sat, warning_option::inputs);
						}
					}

					add_small_divider();
				}
			}
		}
		break;
	case prod_category::employment:
		if(main.combine_types) {
			pseudo_commodity_set seen_commodities;
			for(auto c : state.world.in_commodity) {
				if(main.filter && main.filter != c)
					continue;

				float total = 0;
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						for(auto fac : state.world.province_get_factory_location(p)) {
							if(fac.get_factory().get_building_type().get_output() == c) {
								total += economy::factory_primary_employment(state, fac.get_factory());
								total += economy::factory_secondary_employment(state, fac.get_factory());
								total += economy::factory_unqualified_employment(state, fac.get_factory());
							}
						}
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						for(auto fac : op.get_province().get_factory_location()) {
							if(fac.get_factory().get_building_type().get_output() == c) {
								total += economy::factory_primary_employment(state, fac.get_factory());
								total += economy::factory_secondary_employment(state, fac.get_factory());
								total += economy::factory_unqualified_employment(state, fac.get_factory());
							}
						}
					}
				}
				if(economy::valid_artisan_good(state, state.local_player_nation, c)) {
					if(main.selected_location) {
						province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
							total += state.world.province_get_artisan_score(p, c) * state.world.province_get_labor_demand_satisfaction(p, economy::labor::guild_education);
						});
					} else {
						for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
							total += state.world.province_get_artisan_score(op.get_province(), c) * state.world.province_get_labor_demand_satisfaction(op.get_province(), economy::labor::guild_education);
						}
					}
				}
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						total += economy::rgo_employment(state, c, p);
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						total += economy::rgo_employment(state, c, op.get_province());
					}
				}
				seen_commodities.add(c, total);
			}
			seen_commodities.sort();
			for(auto& i : seen_commodities.contents) {
				add_primary_row(i.c, prod_category::employment, prod_source::combined);
				bool first = true;
				for(int32_t j = 0; j < economy::labor::high_education_and_accepted; ++j) {
					add_sub_item_row(i.c, j, prod_category::employment, first, prod_source::combined);
					first = false;
				}
				add_small_divider();
			}
		} else {
			{
				pseudo_commodity_set seen_commodities;

				float avg_worker_sat = 0.0f;
				float bavg_worker_sat = 0.0f;
				float cavg_worker_sat = 0.0f;
				float count = 0;
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					avg_worker_sat += state.world.province_get_labor_demand_satisfaction(p, economy::labor::basic_education);
					bavg_worker_sat += state.world.province_get_labor_demand_satisfaction(p, economy::labor::no_education);
					cavg_worker_sat += state.world.province_get_labor_demand_satisfaction(p, economy::labor::high_education);
					++count;
				});
				if(count > 0) {
					avg_worker_sat /= count;
					bavg_worker_sat /= count;
					cavg_worker_sat /= count;
				} else {
					avg_worker_sat = 1.0f;
					bavg_worker_sat = 1.0f;
					cavg_worker_sat = 1.0f;
				}

				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;

					float total = 0;
					if(main.selected_location) {
						province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
							for(auto fac : state.world.province_get_factory_location(p)) {
								if(fac.get_factory().get_building_type().get_output() == c) {
									total += economy::factory_primary_employment(state, fac.get_factory());
									total += economy::factory_secondary_employment(state, fac.get_factory());
									total += economy::factory_unqualified_employment(state, fac.get_factory());
								}
							}
						});
					} else {
						for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
							for(auto fac : op.get_province().get_factory_location()) {
								if(fac.get_factory().get_building_type().get_output() == c) {
									total += economy::factory_primary_employment(state, fac.get_factory());
									total += economy::factory_secondary_employment(state, fac.get_factory());
									total += economy::factory_unqualified_employment(state, fac.get_factory());
								}
							}
						}
					}
					seen_commodities.add(c, total);
				}

				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::factory);
				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::employment, prod_source::factory);
					add_sub_item_row(i.c, economy::labor::no_education, prod_category::employment, true, prod_source::factory);
					if(bavg_worker_sat < 0.99f) {
						add_shortage_item(1.0f - bavg_worker_sat, warning_option::labor);
					}
					add_sub_item_row(i.c, economy::labor::basic_education, prod_category::employment, false, prod_source::factory);
					if(avg_worker_sat < 0.99f) {
						add_shortage_item(1.0f - avg_worker_sat, warning_option::labor);
					}
					add_sub_item_row(i.c, economy::labor::high_education, prod_category::employment, false, prod_source::factory);
					if(cavg_worker_sat < 0.99f) {
						add_shortage_item(1.0f - cavg_worker_sat, warning_option::labor);
					}
					add_small_divider();
				}
			}
			{
				pseudo_commodity_set seen_commodities;

				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;
					float total = 0;
					if(economy::valid_artisan_good(state, state.local_player_nation, c)) {
						if(main.selected_location) {
							province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
								total += state.world.province_get_artisan_score(p, c) * state.world.province_get_labor_demand_satisfaction(p, economy::labor::guild_education);
							});
						} else {
							for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
								total += state.world.province_get_artisan_score(op.get_province(), c) * state.world.province_get_labor_demand_satisfaction(op.get_province(), economy::labor::guild_education);
							}
						}
						seen_commodities.add(c, total);
					}
				}

				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::artisan);
				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::employment, prod_source::artisan);
					add_small_divider();
				}
			}
			{
				pseudo_commodity_set seen_commodities;

				float avg_worker_sat = 0.0f;
				float count = 0;
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					avg_worker_sat += state.world.province_get_labor_demand_satisfaction(p, economy::labor::no_education);
					++count;
				});
				if(count > 0)
					avg_worker_sat /= count;
				else
					avg_worker_sat = 1.0f;

				for(auto c : state.world.in_commodity) {
					if(main.filter && main.filter != c)
						continue;

					float total = 0;
					if(main.selected_location) {
						province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
							total += economy::rgo_employment(state, c, p);
						});
					} else {
						for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
							total += economy::rgo_employment(state, c, op.get_province());
						}
					}
					seen_commodities.add(c, total);
				}

				seen_commodities.sort();
				if(!seen_commodities.contents.empty())
					add_category_header(prod_source::rgo);
				for(auto& i : seen_commodities.contents) {
					add_primary_row(i.c, prod_category::employment, prod_source::rgo);
					if(avg_worker_sat < 0.99f) {
						add_shortage_item(1.0f - avg_worker_sat, warning_option::labor);
					}
					add_small_divider();
				}
			}
		}
		break;
	case prod_category::details:
		add_category_header(prod_source::factory);
		for(auto c : state.world.in_commodity) {
			if(main.filter && main.filter != c)
				continue;

			if(!c.get_money_rgo()) {
				for(auto ft : state.world.in_factory_type) {
					if(ft.get_output() == c) {
						add_primary_row(c, prod_category::details, prod_source::factory);

						pseudo_commodity_set seen_commodities;

						auto& cset = ft.get_inputs();
						for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
							seen_commodities.add(cset.commodity_type[i], cset.commodity_amounts[i]);
						}
						auto& csetb = ft.get_efficiency_inputs();
						for(uint32_t i = 0; i < economy::small_commodity_set::set_size; ++i) {
							seen_commodities.add(csetb.commodity_type[i], csetb.commodity_amounts[i]);
						}
						seen_commodities.sort();
						bool first = true;
						for(auto& i : seen_commodities.contents) {
							add_sub_item_row(c, i.c.index(), prod_category::details, first, prod_source::factory);
							first = false;
						}
						add_current_specialization_row(ft);
						add_specialization_priority_row(ft);
						add_small_divider();
						break;
					}
				}
			}
		}
		add_category_header(prod_source::artisan);
		for(auto c : state.world.in_commodity) {
			if(main.filter && main.filter != c)
				continue;
			if(!c.get_money_rgo() && c.get_artisan_output_amount() > 0) {
				add_primary_row(c, prod_category::details, prod_source::artisan);

				pseudo_commodity_set seen_commodities;

				auto& cset = c.get_artisan_inputs();
				for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
					seen_commodities.add(cset.commodity_type[i], cset.commodity_amounts[i]);
				}

				seen_commodities.sort();

				bool first = true;
				for(auto& i : seen_commodities.contents) {
					add_sub_item_row(c, i.c.index(), prod_category::details, first, prod_source::artisan);
					first = false;
				}
				add_small_divider();
			}
		}
		break;
	}
// END
}
measure_result  production_main_main_list_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<primary_row_option>(values[index])) {
		if(primary_row_pool.empty()) primary_row_pool.emplace_back(make_production_primary_row(state));
		if(destination) {
			if(primary_row_pool.size() <= size_t(primary_row_pool_used)) primary_row_pool.emplace_back(make_production_primary_row(state));
			primary_row_pool[primary_row_pool_used]->base_data.position.x = int16_t(x);
			primary_row_pool[primary_row_pool_used]->base_data.position.y = int16_t(y);
			primary_row_pool[primary_row_pool_used]->parent = destination;
			destination->children.push_back(primary_row_pool[primary_row_pool_used].get());
			((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->commodity_type = std::get<primary_row_option>(values[index]).commodity_type;
			((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->category_type = std::get<primary_row_option>(values[index]).category_type;
			((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->about = std::get<primary_row_option>(values[index]).about;
			((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->set_alternate(alternate);
			primary_row_pool[primary_row_pool_used]->impl_on_update(state);
			primary_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1])|| std::holds_alternative<max_item_option>(values[index + 1])|| std::holds_alternative<shortage_item_option>(values[index + 1])|| std::holds_alternative<current_specialization_row_option>(values[index + 1])|| std::holds_alternative<specialization_priority_row_option>(values[index + 1]));
		return measure_result{ primary_row_pool[0]->base_data.size.x, primary_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<sub_item_row_option>(values[index])) {
		if(sub_item_row_pool.empty()) sub_item_row_pool.emplace_back(make_production_sub_item_row(state));
		if(destination) {
			if(sub_item_row_pool.size() <= size_t(sub_item_row_pool_used)) sub_item_row_pool.emplace_back(make_production_sub_item_row(state));
			sub_item_row_pool[sub_item_row_pool_used]->base_data.position.x = int16_t(x);
			sub_item_row_pool[sub_item_row_pool_used]->base_data.position.y = int16_t(y);
			sub_item_row_pool[sub_item_row_pool_used]->parent = destination;
			destination->children.push_back(sub_item_row_pool[sub_item_row_pool_used].get());
			((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->main_commodity = std::get<sub_item_row_option>(values[index]).main_commodity;
			((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->secondary_type = std::get<sub_item_row_option>(values[index]).secondary_type;
			((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->category_type = std::get<sub_item_row_option>(values[index]).category_type;
			((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->is_first_in_subsection = std::get<sub_item_row_option>(values[index]).is_first_in_subsection;
			((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->about = std::get<sub_item_row_option>(values[index]).about;
			((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->set_alternate(alternate);
			sub_item_row_pool[sub_item_row_pool_used]->impl_on_update(state);
			sub_item_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1])|| std::holds_alternative<max_item_option>(values[index + 1])|| std::holds_alternative<shortage_item_option>(values[index + 1])|| std::holds_alternative<current_specialization_row_option>(values[index + 1])|| std::holds_alternative<specialization_priority_row_option>(values[index + 1]));
		return measure_result{ sub_item_row_pool[0]->base_data.size.x, sub_item_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<category_header_option>(values[index])) {
		if(category_header_pool.empty()) category_header_pool.emplace_back(make_production_category_header(state));
		if(destination) {
			if(category_header_pool.size() <= size_t(category_header_pool_used)) category_header_pool.emplace_back(make_production_category_header(state));
			category_header_pool[category_header_pool_used]->base_data.position.x = int16_t(x);
			category_header_pool[category_header_pool_used]->base_data.position.y = int16_t(y);
			category_header_pool[category_header_pool_used]->parent = destination;
			destination->children.push_back(category_header_pool[category_header_pool_used].get());
			((production_category_header_t*)(category_header_pool[category_header_pool_used].get()))->category = std::get<category_header_option>(values[index]).category;
			category_header_pool[category_header_pool_used]->impl_on_update(state);
			category_header_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ category_header_pool[0]->base_data.size.x, category_header_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::no_break};
	}
	if(std::holds_alternative<small_divider_option>(values[index])) {
		if(small_divider_pool.empty()) small_divider_pool.emplace_back(make_production_small_divider(state));
		if(destination) {
			if(small_divider_pool.size() <= size_t(small_divider_pool_used)) small_divider_pool.emplace_back(make_production_small_divider(state));
			small_divider_pool[small_divider_pool_used]->base_data.position.x = int16_t(x);
			small_divider_pool[small_divider_pool_used]->base_data.position.y = int16_t(y);
			small_divider_pool[small_divider_pool_used]->parent = destination;
			destination->children.push_back(small_divider_pool[small_divider_pool_used].get());
			small_divider_pool[small_divider_pool_used]->impl_on_update(state);
			small_divider_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1])|| std::holds_alternative<max_item_option>(values[index + 1])|| std::holds_alternative<shortage_item_option>(values[index + 1])|| std::holds_alternative<current_specialization_row_option>(values[index + 1])|| std::holds_alternative<specialization_priority_row_option>(values[index + 1]));
		return measure_result{ small_divider_pool[0]->base_data.size.x, small_divider_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<max_item_option>(values[index])) {
		if(max_item_pool.empty()) max_item_pool.emplace_back(make_production_max_item(state));
		if(destination) {
			if(max_item_pool.size() <= size_t(max_item_pool_used)) max_item_pool.emplace_back(make_production_max_item(state));
			max_item_pool[max_item_pool_used]->base_data.position.x = int16_t(x);
			max_item_pool[max_item_pool_used]->base_data.position.y = int16_t(y);
			max_item_pool[max_item_pool_used]->parent = destination;
			destination->children.push_back(max_item_pool[max_item_pool_used].get());
			((production_max_item_t*)(max_item_pool[max_item_pool_used].get()))->amount = std::get<max_item_option>(values[index]).amount;
			((production_max_item_t*)(max_item_pool[max_item_pool_used].get()))->kind = std::get<max_item_option>(values[index]).kind;
			((production_max_item_t*)(max_item_pool[max_item_pool_used].get()))->set_alternate(alternate);
			max_item_pool[max_item_pool_used]->impl_on_update(state);
			max_item_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1])|| std::holds_alternative<max_item_option>(values[index + 1])|| std::holds_alternative<shortage_item_option>(values[index + 1])|| std::holds_alternative<current_specialization_row_option>(values[index + 1])|| std::holds_alternative<specialization_priority_row_option>(values[index + 1]));
		return measure_result{ max_item_pool[0]->base_data.size.x, max_item_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<shortage_item_option>(values[index])) {
		if(shortage_item_pool.empty()) shortage_item_pool.emplace_back(make_production_shortage_item(state));
		if(destination) {
			if(shortage_item_pool.size() <= size_t(shortage_item_pool_used)) shortage_item_pool.emplace_back(make_production_shortage_item(state));
			shortage_item_pool[shortage_item_pool_used]->base_data.position.x = int16_t(x);
			shortage_item_pool[shortage_item_pool_used]->base_data.position.y = int16_t(y);
			shortage_item_pool[shortage_item_pool_used]->parent = destination;
			destination->children.push_back(shortage_item_pool[shortage_item_pool_used].get());
			((production_shortage_item_t*)(shortage_item_pool[shortage_item_pool_used].get()))->amount = std::get<shortage_item_option>(values[index]).amount;
			((production_shortage_item_t*)(shortage_item_pool[shortage_item_pool_used].get()))->kind = std::get<shortage_item_option>(values[index]).kind;
			((production_shortage_item_t*)(shortage_item_pool[shortage_item_pool_used].get()))->set_alternate(alternate);
			shortage_item_pool[shortage_item_pool_used]->impl_on_update(state);
			shortage_item_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1])|| std::holds_alternative<max_item_option>(values[index + 1])|| std::holds_alternative<shortage_item_option>(values[index + 1])|| std::holds_alternative<current_specialization_row_option>(values[index + 1])|| std::holds_alternative<specialization_priority_row_option>(values[index + 1]));
		return measure_result{ shortage_item_pool[0]->base_data.size.x, shortage_item_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<current_specialization_row_option>(values[index])) {
		if(current_specialization_row_pool.empty()) current_specialization_row_pool.emplace_back(make_production_current_specialization_row(state));
		if(destination) {
			if(current_specialization_row_pool.size() <= size_t(current_specialization_row_pool_used)) current_specialization_row_pool.emplace_back(make_production_current_specialization_row(state));
			current_specialization_row_pool[current_specialization_row_pool_used]->base_data.position.x = int16_t(x);
			current_specialization_row_pool[current_specialization_row_pool_used]->base_data.position.y = int16_t(y);
			current_specialization_row_pool[current_specialization_row_pool_used]->parent = destination;
			destination->children.push_back(current_specialization_row_pool[current_specialization_row_pool_used].get());
			((production_current_specialization_row_t*)(current_specialization_row_pool[current_specialization_row_pool_used].get()))->for_type = std::get<current_specialization_row_option>(values[index]).for_type;
			((production_current_specialization_row_t*)(current_specialization_row_pool[current_specialization_row_pool_used].get()))->set_alternate(alternate);
			current_specialization_row_pool[current_specialization_row_pool_used]->impl_on_update(state);
			current_specialization_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1])|| std::holds_alternative<max_item_option>(values[index + 1])|| std::holds_alternative<shortage_item_option>(values[index + 1])|| std::holds_alternative<current_specialization_row_option>(values[index + 1])|| std::holds_alternative<specialization_priority_row_option>(values[index + 1]));
		return measure_result{ current_specialization_row_pool[0]->base_data.size.x, current_specialization_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<specialization_priority_row_option>(values[index])) {
		if(specialization_priority_row_pool.empty()) specialization_priority_row_pool.emplace_back(make_production_specialization_priority_row(state));
		if(destination) {
			if(specialization_priority_row_pool.size() <= size_t(specialization_priority_row_pool_used)) specialization_priority_row_pool.emplace_back(make_production_specialization_priority_row(state));
			specialization_priority_row_pool[specialization_priority_row_pool_used]->base_data.position.x = int16_t(x);
			specialization_priority_row_pool[specialization_priority_row_pool_used]->base_data.position.y = int16_t(y);
			specialization_priority_row_pool[specialization_priority_row_pool_used]->parent = destination;
			destination->children.push_back(specialization_priority_row_pool[specialization_priority_row_pool_used].get());
			((production_specialization_priority_row_t*)(specialization_priority_row_pool[specialization_priority_row_pool_used].get()))->for_type = std::get<specialization_priority_row_option>(values[index]).for_type;
			((production_specialization_priority_row_t*)(specialization_priority_row_pool[specialization_priority_row_pool_used].get()))->set_alternate(alternate);
			specialization_priority_row_pool[specialization_priority_row_pool_used]->impl_on_update(state);
			specialization_priority_row_pool_used++;
		}
		alternate = !alternate;
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1])|| std::holds_alternative<max_item_option>(values[index + 1])|| std::holds_alternative<shortage_item_option>(values[index + 1])|| std::holds_alternative<current_specialization_row_option>(values[index + 1])|| std::holds_alternative<specialization_priority_row_option>(values[index + 1]));
		return measure_result{ specialization_priority_row_pool[0]->base_data.size.x, specialization_priority_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  production_main_main_list_t::reset_pools() {
	primary_row_pool_used = 0;
	sub_item_row_pool_used = 0;
	category_header_pool_used = 0;
	small_divider_pool_used = 0;
	max_item_pool_used = 0;
	shortage_item_pool_used = 0;
	current_specialization_row_pool_used = 0;
	specialization_priority_row_pool_used = 0;
}
void production_main_view_dropdown_t::add_item( prod_category category) {
	list_contents.emplace_back(category_option_option{category});
	++total_items;
}
ui::element_base* production_main_view_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<production_category_option_t*>(make_production_category_option(state).release()));
	}
	list_pool[pool_id]->category = list_contents[id].category; 
	return list_pool[pool_id].get();
}
void production_main_view_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->category = list_contents[id].category; 
	label_window_internal->impl_on_update(state); 
}
void production_main_view_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::view_dropdown::on_selection
	main.selected_categry = list_contents[id].category;
	main.impl_on_update(state);
// END
}
void production_main_view_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void production_main_view_dropdown_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::view_dropdown::update
// END
}
void production_main_view_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN main::view_dropdown::create
	add_item(prod_category::production);
	add_item(prod_category::consumption);
	add_item(prod_category::employment);
	add_item(prod_category::details);
// END
}
void production_main_location_dropdown_t::add_item( dcon::state_instance_id location) {
	list_contents.emplace_back(location_option_option{location});
	++total_items;
}
ui::element_base* production_main_location_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<production_location_option_t*>(make_production_location_option(state).release()));
	}
	list_pool[pool_id]->location = list_contents[id].location; 
	return list_pool[pool_id].get();
}
void production_main_location_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->location = list_contents[id].location; 
	label_window_internal->impl_on_update(state); 
}
void production_main_location_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::location_dropdown::on_selection
	main.selected_location = list_contents[id].location;
	main.impl_on_update(state);
// END
}
void production_main_location_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void production_main_location_dropdown_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::location_dropdown::update
	clear_list();
	add_item(dcon::state_instance_id{});
	for(auto os : state.world.nation_get_state_ownership(state.local_player_nation)) {
		add_item(os.get_state());
	}
	std::sort(list_contents.begin() + 1, list_contents.end(), [&](location_option_option const& a, location_option_option const& b) {
		return text::get_dynamic_state_name(state, a.location) < text::get_dynamic_state_name(state, b.location);
	});

	for(auto i = list_contents.size(); i-- > 0;) {
		if(list_contents[i].location == main.selected_location) {
			quiet_on_selection(state, int32_t(i));
			return;
		}
	}
// END
}
void production_main_location_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN main::location_dropdown::create
// END
}
void production_main_filter_dropdown_t::add_item( dcon::commodity_id commodity) {
	list_contents.emplace_back(filter_option_option{commodity});
	++total_items;
}
ui::element_base* production_main_filter_dropdown_t::get_nth_item(sys::state& state, int32_t id, int32_t pool_id) {
	while(pool_id >= int32_t(list_pool.size())) {
		list_pool.emplace_back(static_cast<production_filter_option_t*>(make_production_filter_option(state).release()));
	}
	list_pool[pool_id]->commodity = list_contents[id].commodity; 
	return list_pool[pool_id].get();
}
void production_main_filter_dropdown_t::quiet_on_selection(sys::state& state, int32_t id) {
	if(id < 0 || id >= int32_t(list_contents.size())) return;
	selected_item = id;
	label_window_internal->commodity = list_contents[id].commodity; 
	label_window_internal->impl_on_update(state); 
}
void production_main_filter_dropdown_t::on_selection(sys::state& state, int32_t id) {
	quiet_on_selection(state, id);
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::filter_dropdown::on_selection
	main.filter = list_contents[id].commodity;
	main.impl_on_update(state);
// END
}
void production_main_filter_dropdown_t::clear_list() {	list_contents.clear();
	total_items = 0;
}
void production_main_filter_dropdown_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::filter_dropdown::update
// END
}
void production_main_filter_dropdown_t::on_create(sys::state& state) noexcept {
	template_drop_down_control::on_create(state);
// BEGIN main::filter_dropdown::create
	add_item(dcon::commodity_id{});
	for(auto c : state.world.in_commodity) {
		if(!c.get_money_rgo()) {
			add_item(c);
		}
	}
	std::sort(list_contents.begin() + 1, list_contents.end(), [&](filter_option_option const& a, filter_option_option const& b) {
		return text::produce_simple_string(state, state.world.commodity_get_name(a.commodity)) < text::produce_simple_string(state, state.world.commodity_get_name(b.commodity));
	});
// END
}
void production_main_combine_toggle_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::combine_toggle::update
	set_active(state, main.combine_types);
// END
}
bool production_main_combine_toggle_t::button_action(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent)); 
// BEGIN main::combine_toggle::lbutton_action
	main.combine_types = !main.combine_types;
	main.impl_on_update(state);
// END
	return true;
}
ui::message_result production_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	main_list.update(state, this);
	remake_layout(state, true);
}
void production_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "view_dropdown") {
					temp.ptr = view_dropdown.get();
				} else
				if(cname == "location_dropdown") {
					temp.ptr = location_dropdown.get();
				} else
				if(cname == "filter_dropdown") {
					temp.ptr = filter_dropdown.get();
				} else
				if(cname == "combine_toggle") {
					temp.ptr = combine_toggle.get();
				} else
				if(cname == "title") {
					temp.ptr = title.get();
				} else
				if(cname == "view_tt_icon") {
					temp.ptr = view_tt_icon.get();
				} else
				if(cname == "location_tt_icon") {
					temp.ptr = location_tt_icon.get();
				} else
				if(cname == "combine_tt_icon") {
					temp.ptr = combine_tt_icon.get();
				} else
				if(cname == "filter_tt_icon") {
					temp.ptr = filter_tt_icon.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
				if(cname == "main_list") {
					temp.generator = &main_list;
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
void production_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::main"));
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
		if(child_data.name == "view_dropdown") {
			view_dropdown = std::make_unique<production_main_view_dropdown_t>();
			view_dropdown->parent = this;
			auto cptr = view_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<production_category_option_t>(static_cast<production_category_option_t*>(make_production_category_option(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "location_dropdown") {
			location_dropdown = std::make_unique<production_main_location_dropdown_t>();
			location_dropdown->parent = this;
			auto cptr = location_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<production_location_option_t>(static_cast<production_location_option_t*>(make_production_location_option(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "filter_dropdown") {
			filter_dropdown = std::make_unique<production_main_filter_dropdown_t>();
			filter_dropdown->parent = this;
			auto cptr = filter_dropdown.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->two_columns = (child_data.text_type != aui_text_type::body);
			cptr->target_page_height = child_data.border_size;
			cptr->label_window_internal = std::unique_ptr<production_filter_option_t>(static_cast<production_filter_option_t*>(make_production_filter_option(state).release()));
			cptr->element_x_size = cptr->label_window_internal->base_data.size.x;
			cptr->element_y_size = cptr->label_window_internal->base_data.size.y;
			cptr->label_window = cptr->label_window_internal.get();
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "combine_toggle") {
			combine_toggle = std::make_unique<production_main_combine_toggle_t>();
			combine_toggle->parent = this;
			auto cptr = combine_toggle.get();
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
		if(child_data.name == "view_tt_icon") {
			view_tt_icon = std::make_unique<template_icon_graphic>();
			view_tt_icon->parent = this;
			auto cptr = view_tt_icon.get();
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
		if(child_data.name == "location_tt_icon") {
			location_tt_icon = std::make_unique<template_icon_graphic>();
			location_tt_icon->parent = this;
			auto cptr = location_tt_icon.get();
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
		if(child_data.name == "combine_tt_icon") {
			combine_tt_icon = std::make_unique<template_icon_graphic>();
			combine_tt_icon->parent = this;
			auto cptr = combine_tt_icon.get();
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
		if(child_data.name == "filter_tt_icon") {
			filter_tt_icon = std::make_unique<template_icon_graphic>();
			filter_tt_icon->parent = this;
			auto cptr = filter_tt_icon.get();
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
	main_list.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_production_main(sys::state& state) {
	auto ptr = std::make_unique<production_main_t>();
	ptr->on_create(state);
	return ptr;
}
void production_primary_row_main_commodity_icon_t::on_update(sys::state& state) noexcept {
	production_primary_row_t& primary_row = *((production_primary_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN primary_row::main_commodity_icon::update
	content = primary_row.commodity_type;
// END
}
void production_primary_row_name_t::on_update(sys::state& state) noexcept {
	production_primary_row_t& primary_row = *((production_primary_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN primary_row::name::update
	switch(primary_row.about) {
	case prod_source::combined:
		set_text(state, text::produce_simple_string(state, state.world.commodity_get_name(primary_row.commodity_type)));
		break;
	case prod_source::factory:
		set_text(state, text::produce_simple_string(state, state.world.commodity_get_name(primary_row.commodity_type)));
		break;
	case prod_source::artisan:
		set_text(state, text::produce_simple_string(state, state.world.commodity_get_name(primary_row.commodity_type)));
		break;
	case prod_source::rgo:
		set_text(state, text::produce_simple_string(state, state.world.commodity_get_name(primary_row.commodity_type)));
		break;
	default:
		set_text(state, "");
		break;
	}
// END
}
void production_primary_row_amount_t::on_update(sys::state& state) noexcept {
	production_primary_row_t& primary_row = *((production_primary_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN primary_row::amount::update
	switch(primary_row.about) {
	case prod_source::combined:
		switch(primary_row.category_type) {
		case prod_category::production:
		{
			float total = 0;
			if(main.selected_location) {
				total = economy::factory_output(state, primary_row.commodity_type, main.selected_location) + economy::artisan_output(state, primary_row.commodity_type, main.selected_location) + economy::rgo_output(state, primary_row.commodity_type, main.selected_location);
			} else {
				total = economy::factory_output(state, primary_row.commodity_type, state.local_player_nation) + economy::artisan_output(state, primary_row.commodity_type, state.local_player_nation) + economy::rgo_output(state, primary_row.commodity_type, state.local_player_nation);
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::consumption:
		{
			float total = 0;
			if(main.selected_location) {
				total = economy::estimate_factory_consumption(state, primary_row.commodity_type, main.selected_location) + economy::estimate_artisan_consumption(state, primary_row.commodity_type, main.selected_location);
			} else {
				total = economy::estimate_factory_consumption(state, primary_row.commodity_type, state.local_player_nation) + economy::estimate_artisan_consumption(state, primary_row.commodity_type, state.local_player_nation);
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::employment:
		{
			float total = 0;

			if(main.selected_location) {
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					for(auto fac : state.world.province_get_factory_location(p)) {
						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
							total += economy::factory_primary_employment(state, fac.get_factory());
							total += economy::factory_secondary_employment(state, fac.get_factory());
							total += economy::factory_unqualified_employment(state, fac.get_factory());
						}
					}
				});
			} else {
				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
					for(auto fac : op.get_province().get_factory_location()) {
						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
							total += economy::factory_primary_employment(state, fac.get_factory());
							total += economy::factory_secondary_employment(state, fac.get_factory());
							total += economy::factory_unqualified_employment(state, fac.get_factory());
						}
					}
				}
			}
			if(economy::valid_artisan_good(state, state.local_player_nation, primary_row.commodity_type)) {
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						total += state.world.province_get_artisan_score(p, primary_row.commodity_type) * state.world.province_get_labor_demand_satisfaction(p, economy::labor::guild_education);
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						total += state.world.province_get_artisan_score(op.get_province(), primary_row.commodity_type) * state.world.province_get_labor_demand_satisfaction(op.get_province(), economy::labor::guild_education);
					}
				}
			}
			if(main.selected_location) {
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					total += economy::rgo_employment(state, primary_row.commodity_type, p);
				});
			} else {
				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
					total += economy::rgo_employment(state, primary_row.commodity_type, op.get_province());
				}
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::details:
			set_text(state, "");
			break;
		}
		break;
	case prod_source::factory:
		switch(primary_row.category_type) {
		case prod_category::production:
		{
			float total = 0;
			if(main.selected_location) {
				total = economy::factory_output(state, primary_row.commodity_type, main.selected_location);
			} else {
				total = economy::factory_output(state, primary_row.commodity_type, state.local_player_nation);
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::consumption:
		{
			float total = 0;
			if(main.selected_location) {
				total = economy::estimate_factory_consumption(state, primary_row.commodity_type, main.selected_location);
			} else {
				total = economy::estimate_factory_consumption(state, primary_row.commodity_type, state.local_player_nation);
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::employment:
		{
			float total = 0;
			if(main.selected_location) {
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					for(auto fac : state.world.province_get_factory_location(p)) {
						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
							total += economy::factory_primary_employment(state, fac.get_factory());
							total += economy::factory_secondary_employment(state, fac.get_factory());
						}
					}
				});
			} else {
				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
					for(auto fac : op.get_province().get_factory_location()) {
						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
							total += economy::factory_primary_employment(state, fac.get_factory());
							total += economy::factory_secondary_employment(state, fac.get_factory());
						}
					}
				}
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::details:
		{
			float total = 0.0f;
			for(auto ft : state.world.in_factory_type) {
				if(ft.get_output() == primary_row.commodity_type) {
					total = ft.get_output_amount();
				}
			}
			set_text(state, text::format_float(total, 2));
		} break;
		}
		break;
	case prod_source::artisan:
		switch(primary_row.category_type) {
		case prod_category::production:
		{
			float total = 0;
			if(main.selected_location) {
				total = economy::artisan_output(state, primary_row.commodity_type, main.selected_location);
			} else {
				total = economy::artisan_output(state, primary_row.commodity_type, state.local_player_nation);
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::consumption:
		{
			float total = 0;
			if(main.selected_location) {
				total = economy::estimate_artisan_consumption(state, primary_row.commodity_type, main.selected_location);
			} else {
				total = economy::estimate_artisan_consumption(state, primary_row.commodity_type, state.local_player_nation);
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::employment:
		{
			float total = 0;
			if(economy::valid_artisan_good(state, state.local_player_nation, primary_row.commodity_type)) {
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						total += state.world.province_get_artisan_score(p, primary_row.commodity_type) * state.world.province_get_labor_demand_satisfaction(p, economy::labor::guild_education);
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						total += state.world.province_get_artisan_score(op.get_province(), primary_row.commodity_type) * state.world.province_get_labor_demand_satisfaction(op.get_province(), economy::labor::guild_education);
					}
				}
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::details:
			set_text(state, text::format_float(state.world.commodity_get_artisan_output_amount(primary_row.commodity_type), 2));
			break;
		} break;
	case prod_source::rgo:
		switch(primary_row.category_type) {
		case prod_category::production:
		{
			float total = 0;
			if(main.selected_location) {
				total = economy::rgo_output(state, primary_row.commodity_type, main.selected_location);
			} else {
				total = economy::rgo_output(state, primary_row.commodity_type, state.local_player_nation);
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::consumption:
			set_text(state, text::prettify_float(0.0f));
			break;
		case prod_category::employment:
		{
			float total = 0;
			if(main.selected_location) {
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					total += economy::rgo_employment(state, primary_row.commodity_type, p);
				});
			} else {
				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
					total += economy::rgo_employment(state, primary_row.commodity_type, op.get_province());
				}
			}
			set_text(state, text::prettify_float(total));
		} break;
		case prod_category::details:
			set_text(state, text::format_float(state.world.commodity_get_rgo_amount(primary_row.commodity_type), 2));
			break;
		}
		break;
	default:
		set_text(state, "");
		break;
	}
// END
}
void  production_primary_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result production_primary_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_primary_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_primary_row_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN primary_row::update
// END
	remake_layout(state, true);
}
void production_primary_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "main_commodity_icon") {
					temp.ptr = main_commodity_icon.get();
				} else
				if(cname == "name") {
					temp.ptr = name.get();
				} else
				if(cname == "amount") {
					temp.ptr = amount.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_primary_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::primary_row"));
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
		if(child_data.name == "main_commodity_icon") {
			main_commodity_icon = std::make_unique<production_primary_row_main_commodity_icon_t>();
			main_commodity_icon->parent = this;
			auto cptr = main_commodity_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "name") {
			name = std::make_unique<production_primary_row_name_t>();
			name->parent = this;
			auto cptr = name.get();
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
		if(child_data.name == "amount") {
			amount = std::make_unique<production_primary_row_amount_t>();
			amount->parent = this;
			auto cptr = amount.get();
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
// BEGIN primary_row::create
// END
}
std::unique_ptr<ui::element_base> make_production_primary_row(sys::state& state) {
	auto ptr = std::make_unique<production_primary_row_t>();
	ptr->on_create(state);
	return ptr;
}
void production_sub_item_row_commodity_icon_t::on_update(sys::state& state) noexcept {
	production_sub_item_row_t& sub_item_row = *((production_sub_item_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN sub_item_row::commodity_icon::update
	if(sub_item_row.category_type == prod_category::production || sub_item_row.category_type == prod_category::consumption)
		content = sub_item_row.main_commodity;
	else if(sub_item_row.category_type == prod_category::details)
		content = dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) };
	else
		content = dcon::commodity_id{};
// END
}
void production_sub_item_row_general_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	production_sub_item_row_t& sub_item_row = *((production_sub_item_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN sub_item_row::general_icon::tooltip
	switch(sub_item_row.category_type) {
	case prod_category::production:
		switch(sub_item_row.about) {
		case prod_source::factory:
			text::add_line(state, contents, "production_about_factory_one");
			break;
		case prod_source::artisan:
			text::add_line(state, contents, "production_about_artisan_one");
			break;
		case prod_source::rgo:
			text::add_line(state, contents, "production_about_rgo_one");
			break;
		}
		break;
	case prod_category::consumption:
		switch(sub_item_row.about) {
		case prod_source::factory:
			text::add_line(state, contents, "production_about_factory_one");
			break;
		case prod_source::artisan:
			text::add_line(state, contents, "production_about_artisan_one");
			break;
		case prod_source::rgo:
			text::add_line(state, contents, "production_about_rgo_one");
			break;
		}
		break;
	case prod_category::employment:
		text::add_line(state, contents, ui::labour_type_to_employment_name_text_key(sub_item_row.secondary_type));
		break;
	case prod_category::details:
		// nothing
		break;
	}
// END
}
void production_sub_item_row_general_icon_t::on_update(sys::state& state) noexcept {
	production_sub_item_row_t& sub_item_row = *((production_sub_item_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN sub_item_row::general_icon::update
	static const int32_t factory_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_building_factory_48_filled.svg");
	static const int32_t artisan_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_building_shop_24_filled.svg");
	static const int32_t rgo_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_food_grains_24_filled.svg");

	static const int32_t no_education_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_person_guest_24_filled.svg");
	static const int32_t basic_education_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_person_edit_48_filled.svg");
	static const int32_t high_education_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_person_lightbulb_24_filled.svg");
	static const int32_t guild_education_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_person_wrench_20_filled.svg");
	static const int32_t high_education_and_accepted_icon = template_project::icon_by_name(state.ui_templates, "ic_fluent_person_star_48_filled.svg");

	switch(sub_item_row.category_type) {
	case prod_category::production:
		switch(sub_item_row.about) {
		case prod_source::factory:
			template_id = factory_icon;
			break;
		case prod_source::artisan:
			template_id = artisan_icon;
			break;
		case prod_source::rgo:
			template_id = rgo_icon;
			break;
		case prod_source::combined:
			template_id = -1;
			break;
		}
		break;
	case prod_category::consumption:
		switch(sub_item_row.about) {
		case prod_source::factory:
			template_id = factory_icon;
			break;
		case prod_source::artisan:
			template_id = artisan_icon;
			break;
		case prod_source::rgo:
			template_id = rgo_icon;
			break;
		case prod_source::combined:
			template_id = -1;
			break;
		}
		break;
	case prod_category::employment:
		switch(sub_item_row.secondary_type) {
		case economy::labor::no_education:
			template_id = no_education_icon;
			break;
		case economy::labor::basic_education:
			template_id = basic_education_icon;
			break;
		case economy::labor::high_education:
			template_id = high_education_icon;
			break;
		case economy::labor::guild_education:
			template_id = guild_education_icon;
			break;
		case economy::labor::high_education_and_accepted:
			template_id = high_education_and_accepted_icon;
			break;
		default:
			template_id = -1;
			break;
		}
		break;
	case prod_category::details:
		template_id = -1;
		break;
	}
// END
}
void production_sub_item_row_amount_t::on_update(sys::state& state) noexcept {
	production_sub_item_row_t& sub_item_row = *((production_sub_item_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN sub_item_row::amount::update
	switch(sub_item_row.category_type) {
	case prod_category::production:
		switch(sub_item_row.about) {
		case prod_source::factory:
			if(main.selected_location)
				set_text(state, text::prettify_float(economy::estimate_factory_consumption_in_production(state, sub_item_row.main_commodity, main.selected_location, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) })));
			else
				set_text(state, text::prettify_float(economy::estimate_factory_consumption_in_production(state, sub_item_row.main_commodity, state.local_player_nation, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) })));
			break;
		case prod_source::artisan:
			if(main.selected_location)
				set_text(state, text::prettify_float(economy::estimate_artisan_consumption_in_production(state, sub_item_row.main_commodity, main.selected_location, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) })));
			else
				set_text(state, text::prettify_float(economy::estimate_artisan_consumption_in_production(state, sub_item_row.main_commodity, state.local_player_nation, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) })));
			break;
		case prod_source::rgo:
			set_text(state, "");
			break;
		case prod_source::combined:
			if(main.selected_location)
				set_text(state, text::prettify_float(economy::estimate_factory_consumption_in_production(state, sub_item_row.main_commodity, main.selected_location, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }) + economy::estimate_artisan_consumption_in_production(state, sub_item_row.main_commodity, main.selected_location, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) })));
			else
				set_text(state, text::prettify_float(economy::estimate_factory_consumption_in_production(state, sub_item_row.main_commodity, state.local_player_nation, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }) + economy::estimate_artisan_consumption_in_production(state, sub_item_row.main_commodity, state.local_player_nation, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) })));
			break;
		}
		break;
	case prod_category::consumption:
		switch(sub_item_row.about) {
		case prod_source::factory:
			if(main.selected_location)
				set_text(state, text::prettify_float(economy::estimate_factory_consumption_in_production(state, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }, main.selected_location, sub_item_row.main_commodity )));
			else
				set_text(state, text::prettify_float(economy::estimate_factory_consumption_in_production(state, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }, state.local_player_nation, sub_item_row.main_commodity)));
			break;
		case prod_source::artisan:
			if(main.selected_location)
				set_text(state, text::prettify_float(economy::estimate_artisan_consumption_in_production(state, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }, main.selected_location, sub_item_row.main_commodity)));
			else
				set_text(state, text::prettify_float(economy::estimate_artisan_consumption_in_production(state, dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }, state.local_player_nation, sub_item_row.main_commodity)));
			break;
		case prod_source::rgo:
			set_text(state, "");
			break;
		}
		break;
	case prod_category::employment:
		switch(sub_item_row.secondary_type) {
		case economy::labor::no_education:
			if(sub_item_row.about == prod_source::factory) {
				float total = 0.0f;
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						for(auto fac : state.world.province_get_factory_location(p)) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_unqualified_employment(state, fac.get_factory());
							}
						}
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						for(auto fac : op.get_province().get_factory_location()) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_unqualified_employment(state, fac.get_factory());
							}
						}
					}
				}
				set_text(state, text::prettify_float(total));
			} else if(sub_item_row.about == prod_source::rgo) {
				float total = 0;
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						total += economy::rgo_employment(state, sub_item_row.main_commodity, p);
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						total += economy::rgo_employment(state, sub_item_row.main_commodity, op.get_province());
					}
				}
				set_text(state, text::prettify_float(total));
			} else if(sub_item_row.about == prod_source::combined) {
				float total = 0;
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						for(auto fac : state.world.province_get_factory_location(p)) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_unqualified_employment(state, fac.get_factory());
							}
						}
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						for(auto fac : op.get_province().get_factory_location()) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_unqualified_employment(state, fac.get_factory());
							}
						}
					}
				}
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						total += economy::rgo_employment(state, sub_item_row.main_commodity, p);
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						total += economy::rgo_employment(state, sub_item_row.main_commodity, op.get_province());
					}
				}
				set_text(state, text::prettify_float(total));
			}
			break;
		case economy::labor::basic_education:
			if(sub_item_row.about == prod_source::factory || sub_item_row.about == prod_source::combined) {
				float total = 0.0f;
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						for(auto fac : state.world.province_get_factory_location(p)) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_primary_employment(state, fac.get_factory());
							}
						}
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						for(auto fac : op.get_province().get_factory_location()) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_primary_employment(state, fac.get_factory());
							}
						}
					}
				}
				set_text(state, text::prettify_float(total));
			} else {
				set_text(state, "");
			}
			break;
		case economy::labor::high_education:
			if(sub_item_row.about == prod_source::factory || sub_item_row.about == prod_source::combined) {
				float total = 0.0f;
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						for(auto fac : state.world.province_get_factory_location(p)) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_secondary_employment(state, fac.get_factory());
							}
						}
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						for(auto fac : op.get_province().get_factory_location()) {
							if(fac.get_factory().get_building_type().get_output() == sub_item_row.main_commodity) {
								total += economy::factory_secondary_employment(state, fac.get_factory());
							}
						}
					}
				}
				set_text(state, text::prettify_float(total));
			} else {
				set_text(state, "");
			}
			break;
		case economy::labor::guild_education:
		{
			float total = 0;
			if(economy::valid_artisan_good(state, state.local_player_nation, sub_item_row.main_commodity)) {
				if(main.selected_location) {
					province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
						total += state.world.province_get_artisan_score(p, sub_item_row.main_commodity) * state.world.province_get_labor_demand_satisfaction(p, economy::labor::guild_education);
					});
				} else {
					for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
						total += state.world.province_get_artisan_score(op.get_province(), sub_item_row.main_commodity) * state.world.province_get_labor_demand_satisfaction(op.get_province(), economy::labor::guild_education);
					}
				}
			}
			set_text(state, text::prettify_float(total));
		} break;
		case economy::labor::high_education_and_accepted:
			set_text(state, "");
			break;
		default:
			break;
		}
		break;
	case prod_category::details:
		switch(sub_item_row.about) {
		case prod_source::factory:
		{
			float total = 0.0f;
			for(auto ft : state.world.in_factory_type) {
				if(ft.get_output() == sub_item_row.main_commodity) {
					auto& cset = ft.get_inputs();
					for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
						if(cset.commodity_type[i] == dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }) {
							total += cset.commodity_amounts[i];
						}
					}
					auto& csetb = ft.get_efficiency_inputs();
					for(uint32_t i = 0; i < economy::small_commodity_set::set_size; ++i) {
						if(csetb.commodity_type[i] == dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }) {
							total += csetb.commodity_amounts[i];
						}
					}
				}
			}
			set_text(state, text::format_float(total, 2));
		} break;
		case prod_source::artisan:
		{
			float amount = 0;
			auto& cset = state.world.commodity_get_artisan_inputs(sub_item_row.main_commodity);
			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
				if(cset.commodity_type[i] == dcon::commodity_id{ dcon::commodity_id::value_base_t(sub_item_row.secondary_type) }) {
					amount = cset.commodity_amounts[i];
				}
			}
			set_text(state, text::format_float(amount, 2));
		} break;
		case prod_source::rgo:
			set_text(state, "");
			break;
		}
		break;
	}
// END
}
void production_sub_item_row_sub_items_label_t::on_update(sys::state& state) noexcept {
	production_sub_item_row_t& sub_item_row = *((production_sub_item_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN sub_item_row::sub_items_label::update
	if(sub_item_row.is_first_in_subsection) {
		switch(sub_item_row.category_type) {
		case prod_category::production:
			set_text(state, text::produce_simple_string(state, "production_sub_item_consumption"));
			break;
		case prod_category::consumption:
			set_text(state, text::produce_simple_string(state, "production_sub_item_uses"));
			break;
		case prod_category::employment:
			set_text(state, text::produce_simple_string(state, "production_sub_item_labor"));
			break;
		case prod_category::details:
			set_text(state, text::produce_simple_string(state, "production_sub_item_inputs"));
			break;
		}
	} else {
		set_text(state, "");
	}
// END
}
void  production_sub_item_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result production_sub_item_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_sub_item_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_sub_item_row_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN sub_item_row::update
// END
	remake_layout(state, true);
}
void production_sub_item_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "general_icon") {
					temp.ptr = general_icon.get();
				} else
				if(cname == "amount") {
					temp.ptr = amount.get();
				} else
				if(cname == "sub_items_label") {
					temp.ptr = sub_items_label.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_sub_item_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::sub_item_row"));
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
		if(child_data.name == "commodity_icon") {
			commodity_icon = std::make_unique<production_sub_item_row_commodity_icon_t>();
			commodity_icon->parent = this;
			auto cptr = commodity_icon.get();
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
		if(child_data.name == "general_icon") {
			general_icon = std::make_unique<production_sub_item_row_general_icon_t>();
			general_icon->parent = this;
			auto cptr = general_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->flags |= ui::element_base::wants_update_when_hidden_mask;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "amount") {
			amount = std::make_unique<production_sub_item_row_amount_t>();
			amount->parent = this;
			auto cptr = amount.get();
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
		if(child_data.name == "sub_items_label") {
			sub_items_label = std::make_unique<production_sub_item_row_sub_items_label_t>();
			sub_items_label->parent = this;
			auto cptr = sub_items_label.get();
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
// BEGIN sub_item_row::create
// END
}
std::unique_ptr<ui::element_base> make_production_sub_item_row(sys::state& state) {
	auto ptr = std::make_unique<production_sub_item_row_t>();
	ptr->on_create(state);
	return ptr;
}
void production_category_header_header_text_t::on_update(sys::state& state) noexcept {
	production_category_header_t& category_header = *((production_category_header_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN category_header::header_text::update
	switch(category_header.category) {
	case prod_source::combined:
		set_text(state, text::produce_simple_string(state, "production_about_combined"));
		break;
	case prod_source::factory:
		set_text(state, text::produce_simple_string(state, "production_about_factory"));
		break;
	case prod_source::artisan:
		set_text(state, text::produce_simple_string(state, "production_about_artisan"));
		break;
	case prod_source::rgo:
		set_text(state, text::produce_simple_string(state, "production_about_rgo"));
		break;
	default:
		set_text(state,  "");
		break;
	}
// END
}
ui::message_result production_category_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_category_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_category_header_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN category_header::update
// END
	remake_layout(state, true);
}
void production_category_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "header_text") {
					temp.ptr = header_text.get();
				} else
				if(cname == "dash") {
					temp.ptr = dash.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_category_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::category_header"));
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
		if(child_data.name == "header_text") {
			header_text = std::make_unique<production_category_header_header_text_t>();
			header_text->parent = this;
			auto cptr = header_text.get();
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
		if(child_data.name == "dash") {
			dash = std::make_unique<template_bg_graphic>();
			dash->parent = this;
			auto cptr = dash.get();
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
// BEGIN category_header::create
// END
}
std::unique_ptr<ui::element_base> make_production_category_header(sys::state& state) {
	auto ptr = std::make_unique<production_category_header_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result production_small_divider_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_small_divider_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_small_divider_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN small_divider::update
// END
	remake_layout(state, true);
}
void production_small_divider_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "Control1") {
					temp.ptr = Control1.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_small_divider_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::small_divider"));
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
		if(child_data.name == "Control1") {
			Control1 = std::make_unique<template_bg_graphic>();
			Control1->parent = this;
			auto cptr = Control1.get();
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
// BEGIN small_divider::create
// END
}
std::unique_ptr<ui::element_base> make_production_small_divider(sys::state& state) {
	auto ptr = std::make_unique<production_small_divider_t>();
	ptr->on_create(state);
	return ptr;
}
void production_category_option_contents_t::on_update(sys::state& state) noexcept {
	production_category_option_t& category_option = *((production_category_option_t*)(parent)); 
// BEGIN category_option::contents::update
	switch(category_option.category) {
	case prod_category::production:
		set_text(state, text::produce_simple_string(state, "production_category_production"));
		break;
	case prod_category::consumption:
		set_text(state, text::produce_simple_string(state, "production_category_consumption"));
		break;
	case prod_category::employment:
		set_text(state, text::produce_simple_string(state, "production_category_employment"));
		break;
	case prod_category::details:
		set_text(state, text::produce_simple_string(state, "production_category_details"));
		break;
	default:
		set_text(state, "");
		break;
	}
// END
}
ui::message_result production_category_option_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_category_option_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_category_option_t::on_update(sys::state& state) noexcept {
// BEGIN category_option::update
// END
	remake_layout(state, true);
}
void production_category_option_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_category_option_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::category_option"));
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
			contents = std::make_unique<production_category_option_contents_t>();
			contents->parent = this;
			auto cptr = contents.get();
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
// BEGIN category_option::create
// END
}
std::unique_ptr<ui::element_base> make_production_category_option(sys::state& state) {
	auto ptr = std::make_unique<production_category_option_t>();
	ptr->on_create(state);
	return ptr;
}
void production_location_option_content_t::on_update(sys::state& state) noexcept {
	production_location_option_t& location_option = *((production_location_option_t*)(parent)); 
// BEGIN location_option::content::update
	if(location_option.location) {
		set_text(state, text::get_dynamic_state_name(state, location_option.location));
	} else {
		set_text(state, text::produce_simple_string(state, "production_location_national"));
	}
// END
}
void production_location_option_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void production_location_option_t::on_update(sys::state& state) noexcept {
// BEGIN location_option::update
// END
	remake_layout(state, true);
}
void production_location_option_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_location_option_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::location_option"));
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
		if(child_data.name == "content") {
			content = std::make_unique<production_location_option_content_t>();
			content->parent = this;
			auto cptr = content.get();
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
// BEGIN location_option::create
// END
}
std::unique_ptr<ui::element_base> make_production_location_option(sys::state& state) {
	auto ptr = std::make_unique<production_location_option_t>();
	ptr->on_create(state);
	return ptr;
}
void production_filter_option_commodity_icon_t::on_update(sys::state& state) noexcept {
	production_filter_option_t& filter_option = *((production_filter_option_t*)(parent)); 
// BEGIN filter_option::commodity_icon::update
	show_tooltip = false;
	content = filter_option.commodity;
	if(!filter_option.commodity) {
		set_visible(state, false);
	} else {
		set_visible(state, true);
	}
// END
}
void production_filter_option_filter_name_t::on_update(sys::state& state) noexcept {
	production_filter_option_t& filter_option = *((production_filter_option_t*)(parent)); 
// BEGIN filter_option::filter_name::update
	if(filter_option.commodity)
		set_text(state, text::produce_simple_string(state, state.world.commodity_get_name(filter_option.commodity)));
	else
		set_text(state, text::produce_simple_string(state, "production_filter_all"));
// END
}
ui::message_result production_filter_option_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_filter_option_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_filter_option_t::on_update(sys::state& state) noexcept {
// BEGIN filter_option::update
// END
	remake_layout(state, true);
}
void production_filter_option_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "filter_name") {
					temp.ptr = filter_name.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_filter_option_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::filter_option"));
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
		if(child_data.name == "commodity_icon") {
			commodity_icon = std::make_unique<production_filter_option_commodity_icon_t>();
			commodity_icon->parent = this;
			auto cptr = commodity_icon.get();
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
		if(child_data.name == "filter_name") {
			filter_name = std::make_unique<production_filter_option_filter_name_t>();
			filter_name->parent = this;
			auto cptr = filter_name.get();
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
// BEGIN filter_option::create
// END
}
std::unique_ptr<ui::element_base> make_production_filter_option(sys::state& state) {
	auto ptr = std::make_unique<production_filter_option_t>();
	ptr->on_create(state);
	return ptr;
}
void production_max_item_description_t::on_update(sys::state& state) noexcept {
	production_max_item_t& max_item = *((production_max_item_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN max_item::description::update
	switch(max_item.kind) {
	case max_option::max_output:
		set_text(state, text::produce_simple_string(state, "production_max_output"));
		break;
	case max_option::max_employment:
		set_text(state, text::produce_simple_string(state, "production_max_employment"));
		break;
	}
// END
}
void production_max_item_amount_label_t::on_update(sys::state& state) noexcept {
	production_max_item_t& max_item = *((production_max_item_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN max_item::amount_label::update
	set_text(state, text::prettify_float(max_item.amount));
// END
}
void  production_max_item_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result production_max_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_max_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_max_item_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN max_item::update
// END
	remake_layout(state, true);
}
void production_max_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "description") {
					temp.ptr = description.get();
				} else
				if(cname == "amount_label") {
					temp.ptr = amount_label.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_max_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::max_item"));
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
		if(child_data.name == "description") {
			description = std::make_unique<production_max_item_description_t>();
			description->parent = this;
			auto cptr = description.get();
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
		if(child_data.name == "amount_label") {
			amount_label = std::make_unique<production_max_item_amount_label_t>();
			amount_label->parent = this;
			auto cptr = amount_label.get();
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
// BEGIN max_item::create
// END
}
std::unique_ptr<ui::element_base> make_production_max_item(sys::state& state) {
	auto ptr = std::make_unique<production_max_item_t>();
	ptr->on_create(state);
	return ptr;
}
void production_shortage_item_description_t::on_update(sys::state& state) noexcept {
	production_shortage_item_t& shortage_item = *((production_shortage_item_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN shortage_item::description::update
	switch(shortage_item.kind) {
	case warning_option::inputs:
		set_text(state, text::produce_simple_string(state, "production_shortage_warn_inputs"));
		break;
	case warning_option::labor:
		set_text(state, text::produce_simple_string(state, "production_shortage_warn_labor"));
		break;
	}
// END
}
void production_shortage_item_amount_label_t::on_update(sys::state& state) noexcept {
	production_shortage_item_t& shortage_item = *((production_shortage_item_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN shortage_item::amount_label::update
	set_text(state, text::format_percentage(shortage_item.amount, 0));
// END
}
void production_shortage_item_warning_icon_t::on_update(sys::state& state) noexcept {
	production_shortage_item_t& shortage_item = *((production_shortage_item_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN shortage_item::warning_icon::update
	static auto ocolor = template_project::color_by_name(state.ui_templates, "light orange");
	static auto rcolor = template_project::color_by_name(state.ui_templates, "med red");

	if(shortage_item.amount <= .1f)
		color = state.ui_templates.colors[ocolor];
	else
		color = state.ui_templates.colors[rcolor];
// END
}
void  production_shortage_item_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result production_shortage_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_shortage_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_shortage_item_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN shortage_item::update
// END
	remake_layout(state, true);
}
void production_shortage_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "description") {
					temp.ptr = description.get();
				} else
				if(cname == "amount_label") {
					temp.ptr = amount_label.get();
				} else
				if(cname == "warning_icon") {
					temp.ptr = warning_icon.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_shortage_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::shortage_item"));
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
		if(child_data.name == "description") {
			description = std::make_unique<production_shortage_item_description_t>();
			description->parent = this;
			auto cptr = description.get();
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
		if(child_data.name == "amount_label") {
			amount_label = std::make_unique<production_shortage_item_amount_label_t>();
			amount_label->parent = this;
			auto cptr = amount_label.get();
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
		if(child_data.name == "warning_icon") {
			warning_icon = std::make_unique<production_shortage_item_warning_icon_t>();
			warning_icon->parent = this;
			auto cptr = warning_icon.get();
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
// BEGIN shortage_item::create
// END
}
std::unique_ptr<ui::element_base> make_production_shortage_item(sys::state& state) {
	auto ptr = std::make_unique<production_shortage_item_t>();
	ptr->on_create(state);
	return ptr;
}
void production_current_specialization_row_amount_label_t::on_update(sys::state& state) noexcept {
	production_current_specialization_row_t& current_specialization_row = *((production_current_specialization_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN current_specialization_row::amount_label::update
	set_text(state, text::format_percentage(1.0f - economy::priority_multiplier(state, current_specialization_row.for_type, state.local_player_nation), 1));
// END
}
void production_current_specialization_row_rankings_button_t::on_update(sys::state& state) noexcept {
	production_current_specialization_row_t& current_specialization_row = *((production_current_specialization_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN current_specialization_row::rankings_button::update
// END
}
bool production_current_specialization_row_rankings_button_t::button_action(sys::state& state) noexcept {
	production_current_specialization_row_t& current_specialization_row = *((production_current_specialization_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN current_specialization_row::rankings_button::lbutton_action
	production_spec_rankings_popup_t* ptr = (production_spec_rankings_popup_t*)display_at_front< make_production_spec_rankings_popup>(state, display_closure_command::return_pointer);
	ptr->for_type = current_specialization_row.for_type;
	ptr->set_visible(state, false);

	auto self_pos = ui::get_absolute_location(state, *this);

	ptr->base_data.position.x = int16_t(self_pos.x + base_data.size.x);
	ptr->base_data.position.y = int16_t(self_pos.y + base_data.size.y / 2 - ptr->base_data.size.y / 2);
	if(ptr->base_data.position.x < 0) {
		ptr->base_data.position.x = 0;
	}
	if(ptr->base_data.position.x + ptr->base_data.size.x > state.ui_state.root->base_data.size.x) {
		ptr->base_data.position.x = int16_t(state.ui_state.root->base_data.size.x - ptr->base_data.size.x);
	}
	if(ptr->base_data.position.y < 0) {
		ptr->base_data.position.y = 0;
	}
	if(ptr->base_data.position.y + ptr->base_data.size.y > state.ui_state.root->base_data.size.y) {
		ptr->base_data.position.y = int16_t(state.ui_state.root->base_data.size.y - ptr->base_data.size.y);
	}

	display_at_front< make_production_spec_rankings_popup>(state);
	state.ui_state.set_mouse_sensitive_target(state, ptr);
// END
	return true;
}
void  production_current_specialization_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result production_current_specialization_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_current_specialization_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_current_specialization_row_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN current_specialization_row::update
// END
	remake_layout(state, true);
}
void production_current_specialization_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "amount_label") {
					temp.ptr = amount_label.get();
				} else
				if(cname == "rankings_button") {
					temp.ptr = rankings_button.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_current_specialization_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::current_specialization_row"));
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
			label = std::make_unique<template_label>();
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
		if(child_data.name == "amount_label") {
			amount_label = std::make_unique<production_current_specialization_row_amount_label_t>();
			amount_label->parent = this;
			auto cptr = amount_label.get();
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
		if(child_data.name == "rankings_button") {
			rankings_button = std::make_unique<production_current_specialization_row_rankings_button_t>();
			rankings_button->parent = this;
			auto cptr = rankings_button.get();
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
// BEGIN current_specialization_row::create
// END
}
std::unique_ptr<ui::element_base> make_production_current_specialization_row(sys::state& state) {
	auto ptr = std::make_unique<production_current_specialization_row_t>();
	ptr->on_create(state);
	return ptr;
}
void production_specialization_priority_row_left_t::on_update(sys::state& state) noexcept {
	production_specialization_priority_row_t& specialization_priority_row = *((production_specialization_priority_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN specialization_priority_row::left::update
	disabled = state.world.nation_get_factory_type_experience_priority_national(state.local_player_nation, specialization_priority_row.for_type) < 100.0f;
// END
}
bool production_specialization_priority_row_left_t::button_action(sys::state& state) noexcept {
	production_specialization_priority_row_t& specialization_priority_row = *((production_specialization_priority_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN specialization_priority_row::left::lbutton_action
	command::set_factory_type_priority(state, state.local_player_nation, specialization_priority_row.for_type, state.world.nation_get_factory_type_experience_priority_national(state.local_player_nation, specialization_priority_row.for_type) - 100.0f);
// END
	return true;
}
void production_specialization_priority_row_right_t::on_update(sys::state& state) noexcept {
	production_specialization_priority_row_t& specialization_priority_row = *((production_specialization_priority_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN specialization_priority_row::right::update
	disabled = state.world.nation_get_factory_type_experience_priority_national(state.local_player_nation, specialization_priority_row.for_type) > 900.0f;
// END
}
bool production_specialization_priority_row_right_t::button_action(sys::state& state) noexcept {
	production_specialization_priority_row_t& specialization_priority_row = *((production_specialization_priority_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN specialization_priority_row::right::lbutton_action
	command::set_factory_type_priority(state, state.local_player_nation, specialization_priority_row.for_type, state.world.nation_get_factory_type_experience_priority_national(state.local_player_nation, specialization_priority_row.for_type) + 100.0f);
// END
	return true;
}
void production_specialization_priority_row_amount_t::on_update(sys::state& state) noexcept {
	production_specialization_priority_row_t& specialization_priority_row = *((production_specialization_priority_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN specialization_priority_row::amount::update
	auto amount = int32_t(std::round(state.world.nation_get_factory_type_experience_priority_national(state.local_player_nation, specialization_priority_row.for_type) / 100.0f));
	set_text(state, std::to_string(amount));
// END
}
void  production_specialization_priority_row_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result production_specialization_priority_row_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_specialization_priority_row_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_specialization_priority_row_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN specialization_priority_row::update
// END
	remake_layout(state, true);
}
void production_specialization_priority_row_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "left") {
					temp.ptr = left.get();
				} else
				if(cname == "right") {
					temp.ptr = right.get();
				} else
				if(cname == "amount") {
					temp.ptr = amount.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_specialization_priority_row_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::specialization_priority_row"));
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
			label = std::make_unique<template_label>();
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
		if(child_data.name == "left") {
			left = std::make_unique<production_specialization_priority_row_left_t>();
			left->parent = this;
			auto cptr = left.get();
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
		if(child_data.name == "right") {
			right = std::make_unique<production_specialization_priority_row_right_t>();
			right->parent = this;
			auto cptr = right.get();
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
		if(child_data.name == "amount") {
			amount = std::make_unique<production_specialization_priority_row_amount_t>();
			amount->parent = this;
			auto cptr = amount.get();
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
// BEGIN specialization_priority_row::create
// END
}
std::unique_ptr<ui::element_base> make_production_specialization_priority_row(sys::state& state) {
	auto ptr = std::make_unique<production_specialization_priority_row_t>();
	ptr->on_create(state);
	return ptr;
}
void production_spec_rankings_popup_top_rankings_t::add_spec_rankings_item(dcon::nation_id nation_for) {
	values.emplace_back(spec_rankings_item_option{nation_for});
}
void  production_spec_rankings_popup_top_rankings_t::on_create(sys::state& state, layout_window_element* parent) {
	production_spec_rankings_popup_t& spec_rankings_popup = *((production_spec_rankings_popup_t*)(parent)); 
// BEGIN spec_rankings_popup::top_rankings::on_create
// END
}
void  production_spec_rankings_popup_top_rankings_t::update(sys::state& state, layout_window_element* parent) {
	production_spec_rankings_popup_t& spec_rankings_popup = *((production_spec_rankings_popup_t*)(parent)); 
// BEGIN spec_rankings_popup::top_rankings::update
	struct ranking_item {
		dcon::nation_id n;
		float value = 0.0f;
	};
	std::vector<ranking_item> items;
	for(auto n : state.world.in_nation) {
		auto v = economy::priority_multiplier(state, spec_rankings_popup.for_type, n);
		if(v <= 0.99f)
			items.push_back(ranking_item{ n, v });
	}
	std::sort(items.begin(), items.end(), [&](ranking_item const& a, ranking_item const& b) {
		return a.value < b.value;
	});
	values.clear();
	for(size_t i = 0; i < items.size() && i < 9; ++i) {
		add_spec_rankings_item(items[i].n);
	}
// END
}
measure_result  production_spec_rankings_popup_top_rankings_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<spec_rankings_item_option>(values[index])) {
		if(spec_rankings_item_pool.empty()) spec_rankings_item_pool.emplace_back(make_production_spec_rankings_item(state));
		if(destination) {
			if(spec_rankings_item_pool.size() <= size_t(spec_rankings_item_pool_used)) spec_rankings_item_pool.emplace_back(make_production_spec_rankings_item(state));
			spec_rankings_item_pool[spec_rankings_item_pool_used]->base_data.position.x = int16_t(x);
			spec_rankings_item_pool[spec_rankings_item_pool_used]->base_data.position.y = int16_t(y);
			spec_rankings_item_pool[spec_rankings_item_pool_used]->parent = destination;
			destination->children.push_back(spec_rankings_item_pool[spec_rankings_item_pool_used].get());
			((production_spec_rankings_item_t*)(spec_rankings_item_pool[spec_rankings_item_pool_used].get()))->nation_for = std::get<spec_rankings_item_option>(values[index]).nation_for;
			spec_rankings_item_pool[spec_rankings_item_pool_used]->impl_on_update(state);
			spec_rankings_item_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ spec_rankings_item_pool[0]->base_data.size.x, spec_rankings_item_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  production_spec_rankings_popup_top_rankings_t::reset_pools() {
	spec_rankings_item_pool_used = 0;
}
ui::message_result production_spec_rankings_popup_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_spec_rankings_popup_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_spec_rankings_popup_t::on_update(sys::state& state) noexcept {
// BEGIN spec_rankings_popup::update
// END
	top_rankings.update(state, this);
	remake_layout(state, true);
}
void production_spec_rankings_popup_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "header_label") {
					temp.ptr = header_label.get();
				} else
				if(cname == "globe_icon") {
					temp.ptr = globe_icon.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
				if(cname == "top_rankings") {
					temp.generator = &top_rankings;
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
void production_spec_rankings_popup_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::spec_rankings_popup"));
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
		if(child_data.name == "header_label") {
			header_label = std::make_unique<template_label>();
			header_label->parent = this;
			auto cptr = header_label.get();
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
		if(child_data.name == "globe_icon") {
			globe_icon = std::make_unique<template_icon_graphic>();
			globe_icon->parent = this;
			auto cptr = globe_icon.get();
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
	top_rankings.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN spec_rankings_popup::create
// END
}
std::unique_ptr<ui::element_base> make_production_spec_rankings_popup(sys::state& state) {
	auto ptr = std::make_unique<production_spec_rankings_popup_t>();
	ptr->on_create(state);
	return ptr;
}
void production_spec_rankings_item_amount_t::on_update(sys::state& state) noexcept {
	production_spec_rankings_item_t& spec_rankings_item = *((production_spec_rankings_item_t*)(parent)); 
	production_spec_rankings_popup_t& spec_rankings_popup = *((production_spec_rankings_popup_t*)(parent->parent)); 
// BEGIN spec_rankings_item::amount::update
	set_text(state, text::format_percentage(1.0f - economy::priority_multiplier(state, spec_rankings_popup.for_type, spec_rankings_item.nation_for), 1));
// END
}
ui::message_result production_spec_rankings_item_flag_control_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
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
ui::message_result production_spec_rankings_item_flag_control_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void production_spec_rankings_item_flag_control_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
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
void production_spec_rankings_item_flag_control_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
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
void production_spec_rankings_item_flag_control_t::on_update(sys::state& state) noexcept {
	production_spec_rankings_item_t& spec_rankings_item = *((production_spec_rankings_item_t*)(parent)); 
	production_spec_rankings_popup_t& spec_rankings_popup = *((production_spec_rankings_popup_t*)(parent->parent)); 
// BEGIN spec_rankings_item::flag_control::update
	flag = spec_rankings_item.nation_for;
// END
}
void production_spec_rankings_item_flag_control_t::on_create(sys::state& state) noexcept {
// BEGIN spec_rankings_item::flag_control::create
// END
}
ui::message_result production_spec_rankings_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_spec_rankings_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_spec_rankings_item_t::on_update(sys::state& state) noexcept {
	production_spec_rankings_popup_t& spec_rankings_popup = *((production_spec_rankings_popup_t*)(parent->parent)); 
// BEGIN spec_rankings_item::update
// END
	remake_layout(state, true);
}
void production_spec_rankings_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "amount") {
					temp.ptr = amount.get();
				} else
				if(cname == "flag_control") {
					temp.ptr = flag_control.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_spec_rankings_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::spec_rankings_item"));
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
		if(child_data.name == "amount") {
			amount = std::make_unique<production_spec_rankings_item_amount_t>();
			amount->parent = this;
			auto cptr = amount.get();
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
		if(child_data.name == "flag_control") {
			flag_control = std::make_unique<production_spec_rankings_item_flag_control_t>();
			flag_control->parent = this;
			auto cptr = flag_control.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
// BEGIN spec_rankings_item::create
// END
}
std::unique_ptr<ui::element_base> make_production_spec_rankings_item(sys::state& state) {
	auto ptr = std::make_unique<production_spec_rankings_item_t>();
	ptr->on_create(state);
	return ptr;
}
void production_rh_state_item_active_directives_t::add_directive_special(dcon::production_directive_id content, dcon::state_instance_id for_state) {
	values.emplace_back(directive_special_option{content, for_state});
}
void production_rh_state_item_active_directives_t::add_directive_commodity(dcon::commodity_id content, dcon::state_instance_id for_state) {
	values.emplace_back(directive_commodity_option{content, for_state});
}
void  production_rh_state_item_active_directives_t::on_create(sys::state& state, layout_window_element* parent) {
	production_rh_state_item_t& rh_state_item = *((production_rh_state_item_t*)(parent)); 
// BEGIN rh_state_item::active_directives::on_create
// END
}
void  production_rh_state_item_active_directives_t::update(sys::state& state, layout_window_element* parent) {
	production_rh_state_item_t& rh_state_item = *((production_rh_state_item_t*)(parent)); 
// BEGIN rh_state_item::active_directives::update
	values.clear();

	for(uint32_t i = 0; i < production_directives::count_special_keys; ++i) {
		if(state.world.state_instance_get_production_directive(rh_state_item.for_state, dcon::production_directive_id{ dcon::production_directive_id::value_base_t(i) })) {
			add_directive_special(dcon::production_directive_id{ dcon::production_directive_id::value_base_t(i) }, rh_state_item.for_state);
		}
	}
	std::vector<dcon::commodity_id> ctemp;
	for(auto c : state.world.in_commodity) {
		if(!c.get_money_rgo() && state.world.state_instance_get_production_directive(rh_state_item.for_state, production_directives::to_key(state, c))) {
			for(auto t : state.world.in_factory_type) {
				if(t.get_output() == c) {
					ctemp.push_back(c.id);
					break;
				}
			}
		}
	}
	std::sort(ctemp.begin(), ctemp.end(), [&](auto a, auto b) {
		return text::produce_simple_string(state, state.world.commodity_get_name(a)) < text::produce_simple_string(state, state.world.commodity_get_name(b));
	});
	for(auto c : ctemp) {
		add_directive_commodity(c, rh_state_item.for_state);
	}
// END
}
measure_result  production_rh_state_item_active_directives_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<directive_special_option>(values[index])) {
		if(directive_special_pool.empty()) directive_special_pool.emplace_back(make_production_directive_special(state));
		if(destination) {
			if(directive_special_pool.size() <= size_t(directive_special_pool_used)) directive_special_pool.emplace_back(make_production_directive_special(state));
			directive_special_pool[directive_special_pool_used]->base_data.position.x = int16_t(x);
			directive_special_pool[directive_special_pool_used]->base_data.position.y = int16_t(y);
			directive_special_pool[directive_special_pool_used]->parent = destination;
			destination->children.push_back(directive_special_pool[directive_special_pool_used].get());
			((production_directive_special_t*)(directive_special_pool[directive_special_pool_used].get()))->content = std::get<directive_special_option>(values[index]).content;
			((production_directive_special_t*)(directive_special_pool[directive_special_pool_used].get()))->for_state = std::get<directive_special_option>(values[index]).for_state;
			directive_special_pool[directive_special_pool_used]->impl_on_update(state);
			directive_special_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ directive_special_pool[0]->base_data.size.x, directive_special_pool[0]->base_data.size.y + 2, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<directive_commodity_option>(values[index])) {
		if(directive_commodity_pool.empty()) directive_commodity_pool.emplace_back(make_production_directive_commodity(state));
		if(destination) {
			if(directive_commodity_pool.size() <= size_t(directive_commodity_pool_used)) directive_commodity_pool.emplace_back(make_production_directive_commodity(state));
			directive_commodity_pool[directive_commodity_pool_used]->base_data.position.x = int16_t(x);
			directive_commodity_pool[directive_commodity_pool_used]->base_data.position.y = int16_t(y);
			directive_commodity_pool[directive_commodity_pool_used]->parent = destination;
			destination->children.push_back(directive_commodity_pool[directive_commodity_pool_used].get());
			((production_directive_commodity_t*)(directive_commodity_pool[directive_commodity_pool_used].get()))->content = std::get<directive_commodity_option>(values[index]).content;
			((production_directive_commodity_t*)(directive_commodity_pool[directive_commodity_pool_used].get()))->for_state = std::get<directive_commodity_option>(values[index]).for_state;
			directive_commodity_pool[directive_commodity_pool_used]->impl_on_update(state);
			directive_commodity_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ directive_commodity_pool[0]->base_data.size.x, directive_commodity_pool[0]->base_data.size.y + 2, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  production_rh_state_item_active_directives_t::reset_pools() {
	directive_special_pool_used = 0;
	directive_commodity_pool_used = 0;
}
void production_rh_state_item_state_name_t::on_update(sys::state& state) noexcept {
	production_rh_state_item_t& rh_state_item = *((production_rh_state_item_t*)(parent)); 
// BEGIN rh_state_item::state_name::update
	set_text(state, text::get_dynamic_state_name(state, rh_state_item.for_state));
// END
}
void production_rh_state_item_population_amount_t::on_update(sys::state& state) noexcept {
	production_rh_state_item_t& rh_state_item = *((production_rh_state_item_t*)(parent)); 
// BEGIN rh_state_item::population_amount::update
	set_text(state, text::prettify_float(state.world.state_instance_get_demographics(rh_state_item.for_state, demographics::total)));
// END
}
bool production_rh_state_item_directives_drop_target_t::recieve_drag_and_drop(sys::state& state, std::any& data, ui::drag_and_drop_data data_type, ui::drag_and_drop_target sub_target, bool shift_held_down) noexcept  {
	production_rh_state_item_t& rh_state_item = *((production_rh_state_item_t*)(parent)); 
// BEGIN rh_state_item::directives_drop_target::recieve
	auto dir = std::any_cast<dcon::production_directive_id>(data);
	if(!state.world.state_instance_get_production_directive(rh_state_item.for_state, dir)) {
		command::toggle_production_directive(state, state.local_player_nation, rh_state_item.for_state, dir);
	}
	if(shift_held_down)
		return false;
// END
	return true;
}
ui::message_result production_rh_state_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_rh_state_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_rh_state_item_t::on_update(sys::state& state) noexcept {
// BEGIN rh_state_item::update
	struct camount {
		dcon::commodity_id c;
		float amount;
	};
	std::vector< camount> consumption_amounts;
	std::vector< camount> production_amounts;

	for(auto c : state.world.in_commodity) {
		float prod_amount = 0.0f;
		float consume_amount = 0.0f;
		province::for_each_province_in_state_instance(state, for_state, [&](dcon::province_id p) {
			prod_amount += economy::estimate_production(state, c, p);
			consume_amount += economy::estimate_intermediate_consumption(state, c, p);
		});

		consumption_amounts.push_back(camount{ c.id, consume_amount});
		production_amounts.push_back(camount{ c.id, prod_amount });
	}

	std::sort(consumption_amounts.begin(), consumption_amounts.end(), [](camount const& a, camount const& b) { return a.amount > b.amount; });
	std::sort(production_amounts.begin(), production_amounts.end(), [](camount const& a, camount const& b) { return a.amount > b.amount; });

	if(consumption_amounts.size() >= 1 && consumption_amounts[0].amount > 0.0f)
		consumption1->content = consumption_amounts[0].c;
	else
		consumption1->content = dcon::commodity_id{};
	if(consumption_amounts.size() >= 2 && consumption_amounts[1].amount > 0.0f)
		consumption2->content = consumption_amounts[1].c;
	else
		consumption2->content = dcon::commodity_id{};
	if(consumption_amounts.size() >= 3 && consumption_amounts[2].amount > 0.0f)
		consumption3->content = consumption_amounts[2].c;
	else
		consumption3->content = dcon::commodity_id{};
	if(consumption_amounts.size() >= 4 && consumption_amounts[3].amount > 0.0f)
		consumption4->content = consumption_amounts[3].c;
	else
		consumption4->content = dcon::commodity_id{};

	if(production_amounts.size() >= 1 && production_amounts[0].amount > 0.0f)
		production1->content = production_amounts[0].c;
	else
		production1->content = dcon::commodity_id{};
	if(production_amounts.size() >= 2 && production_amounts[1].amount > 0.0f)
		production2->content = production_amounts[1].c;
	else
		production2->content = dcon::commodity_id{};
	if(production_amounts.size() >= 3 && production_amounts[2].amount > 0.0f)
		production3->content = production_amounts[2].c;
	else
		production3->content = dcon::commodity_id{};
	if(production_amounts.size() >= 4 && production_amounts[3].amount > 0.0f)
		production4->content = production_amounts[3].c;
	else
		production4->content = dcon::commodity_id{};
// END
	active_directives.update(state, this);
	remake_layout(state, true);
}
void production_rh_state_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "state_name") {
					temp.ptr = state_name.get();
				} else
				if(cname == "population_icon") {
					temp.ptr = population_icon.get();
				} else
				if(cname == "population_amount") {
					temp.ptr = population_amount.get();
				} else
				if(cname == "consumption_icon") {
					temp.ptr = consumption_icon.get();
				} else
				if(cname == "consumption1") {
					temp.ptr = consumption1.get();
				} else
				if(cname == "consumption2") {
					temp.ptr = consumption2.get();
				} else
				if(cname == "consumption3") {
					temp.ptr = consumption3.get();
				} else
				if(cname == "consumption4") {
					temp.ptr = consumption4.get();
				} else
				if(cname == "production_icon") {
					temp.ptr = production_icon.get();
				} else
				if(cname == "production1") {
					temp.ptr = production1.get();
				} else
				if(cname == "production2") {
					temp.ptr = production2.get();
				} else
				if(cname == "production3") {
					temp.ptr = production3.get();
				} else
				if(cname == "production4") {
					temp.ptr = production4.get();
				} else
				if(cname == "directives_drop_target") {
					temp.ptr = directives_drop_target.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
				if(cname == "active_directives") {
					temp.generator = &active_directives;
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
void production_rh_state_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::rh_state_item"));
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
		if(child_data.name == "state_name") {
			state_name = std::make_unique<production_rh_state_item_state_name_t>();
			state_name->parent = this;
			auto cptr = state_name.get();
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
		if(child_data.name == "population_icon") {
			population_icon = std::make_unique<template_icon_graphic>();
			population_icon->parent = this;
			auto cptr = population_icon.get();
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
		if(child_data.name == "population_amount") {
			population_amount = std::make_unique<production_rh_state_item_population_amount_t>();
			population_amount->parent = this;
			auto cptr = population_amount.get();
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
		if(child_data.name == "consumption_icon") {
			consumption_icon = std::make_unique<template_icon_graphic>();
			consumption_icon->parent = this;
			auto cptr = consumption_icon.get();
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
		if(child_data.name == "consumption1") {
			consumption1 = std::make_unique<alice_ui::legacy_commodity_icon>();
			consumption1->parent = this;
			auto cptr = consumption1.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "consumption2") {
			consumption2 = std::make_unique<alice_ui::legacy_commodity_icon>();
			consumption2->parent = this;
			auto cptr = consumption2.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "consumption3") {
			consumption3 = std::make_unique<alice_ui::legacy_commodity_icon>();
			consumption3->parent = this;
			auto cptr = consumption3.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "consumption4") {
			consumption4 = std::make_unique<alice_ui::legacy_commodity_icon>();
			consumption4->parent = this;
			auto cptr = consumption4.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "production_icon") {
			production_icon = std::make_unique<template_icon_graphic>();
			production_icon->parent = this;
			auto cptr = production_icon.get();
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
		if(child_data.name == "production1") {
			production1 = std::make_unique<alice_ui::legacy_commodity_icon>();
			production1->parent = this;
			auto cptr = production1.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "production2") {
			production2 = std::make_unique<alice_ui::legacy_commodity_icon>();
			production2->parent = this;
			auto cptr = production2.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "production3") {
			production3 = std::make_unique<alice_ui::legacy_commodity_icon>();
			production3->parent = this;
			auto cptr = production3.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "production4") {
			production4 = std::make_unique<alice_ui::legacy_commodity_icon>();
			production4->parent = this;
			auto cptr = production4.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "directives_drop_target") {
			directives_drop_target = std::make_unique<production_rh_state_item_directives_drop_target_t>();
			directives_drop_target->parent = this;
			auto cptr = directives_drop_target.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->supported_data_type = ui::drag_and_drop_data::production_directive;
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
	active_directives.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN rh_state_item::create
// END
}
std::unique_ptr<ui::element_base> make_production_rh_state_item(sys::state& state) {
	auto ptr = std::make_unique<production_rh_state_item_t>();
	ptr->on_create(state);
	return ptr;
}
void production_dad_source_special_dad_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	production_dad_source_special_t& dad_source_special = *((production_dad_source_special_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_special::dad_button::tooltip
	if(dad_source_special.content == production_directives::factory_profit) {
		text::add_line(state, contents, "directive_description_fmoney");
	} else if(dad_source_special.content == production_directives::factory_employment) {
		text::add_line(state, contents, "directive_description_fpeople");
	} else if(dad_source_special.content == production_directives::factory_specialization) {
		text::add_line(state, contents, "directive_description_fspecial");
	} else if(dad_source_special.content == production_directives::factory_upgrades) {
		text::add_line(state, contents, "directive_description_fupgrade");
	} else if(dad_source_special.content == production_directives::no_factories) {
		text::add_line(state, contents, "directive_description_fnone");
	} else if(dad_source_special.content == production_directives::railroads) {
		text::add_line(state, contents, "directive_description_rail");
	} else if(dad_source_special.content == production_directives::naval_bases) {
		text::add_line(state, contents, "directive_description_naval");
	} else if(dad_source_special.content == production_directives::forts) {
		text::add_line(state, contents, "directive_description_forts");
	}
// END
}
void production_dad_source_special_dad_button_t::on_update(sys::state& state) noexcept {
	production_dad_source_special_t& dad_source_special = *((production_dad_source_special_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_special::dad_button::update
	static auto icon0 = template_project::icon_by_name(state.ui_templates, "factory_money.svg");
	static auto icon1 = template_project::icon_by_name(state.ui_templates, "factory_people.svg");
	static auto icon2 = template_project::icon_by_name(state.ui_templates, "factory_special.svg");
	static auto icon3 = template_project::icon_by_name(state.ui_templates, "factory_upgrade.svg");
	static auto icon4 = template_project::icon_by_name(state.ui_templates, "factory_none.svg");
	static auto icon5 = template_project::icon_by_name(state.ui_templates, "train_upgrade.svg");
	static auto icon6 = template_project::icon_by_name(state.ui_templates, "naval_base_upgrade.svg");
	static auto icon7 = template_project::icon_by_name(state.ui_templates, "fort_upgrade.svg");
	if(dad_source_special.content == production_directives::factory_profit) {
		icon = icon0;
	} else if(dad_source_special.content == production_directives::factory_employment) {
		icon = icon1;
	} else if(dad_source_special.content == production_directives::factory_specialization) {
		icon = icon2;
	} else if(dad_source_special.content == production_directives::factory_upgrades) {
		icon = icon3;
	} else if(dad_source_special.content == production_directives::no_factories) {
		icon = icon4;
	} else if(dad_source_special.content == production_directives::railroads) {
		icon = icon5;
	} else if(dad_source_special.content == production_directives::naval_bases) {
		icon = icon6;
	} else if(dad_source_special.content == production_directives::forts) {
		icon = icon7;
	}
// END
}
bool production_dad_source_special_dad_button_t::button_action(sys::state& state) noexcept {
	production_dad_source_special_t& dad_source_special = *((production_dad_source_special_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_special::dad_button::lbutton_action
	ui::place_in_drag_and_drop(state, *this, dad_source_special.content, ui::drag_and_drop_data::production_directive);
// END
	return true;
}
ui::message_result production_dad_source_special_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_dad_source_special_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_dad_source_special_t::on_update(sys::state& state) noexcept {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_special::update
// END
	remake_layout(state, true);
}
void production_dad_source_special_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "dad_button") {
					temp.ptr = dad_button.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_dad_source_special_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::dad_source_special"));
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
		if(child_data.name == "dad_button") {
			dad_button = std::make_unique<production_dad_source_special_dad_button_t>();
			dad_button->parent = this;
			auto cptr = dad_button.get();
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
// BEGIN dad_source_special::create
// END
}
std::unique_ptr<ui::element_base> make_production_dad_source_special(sys::state& state) {
	auto ptr = std::make_unique<production_dad_source_special_t>();
	ptr->on_create(state);
	return ptr;
}
void production_dad_source_commodity_dad_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	production_dad_source_commodity_t& dad_source_commodity = *((production_dad_source_commodity_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_commodity::dad_button::tooltip
	text::add_line(state, contents, "directive_description_commodity", text::variable_type::x, state.world.commodity_get_name(dad_source_commodity.content));
// END
}
void production_dad_source_commodity_dad_button_t::on_update(sys::state& state) noexcept {
	production_dad_source_commodity_t& dad_source_commodity = *((production_dad_source_commodity_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_commodity::dad_button::update
// END
}
bool production_dad_source_commodity_dad_button_t::button_action(sys::state& state) noexcept {
	production_dad_source_commodity_t& dad_source_commodity = *((production_dad_source_commodity_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_commodity::dad_button::lbutton_action
	ui::place_in_drag_and_drop(state, *this, production_directives::to_key(state, dad_source_commodity.content), ui::drag_and_drop_data::production_directive);
// END
	return true;
}
ui::message_result production_dad_source_commodity_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_dad_source_commodity_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_dad_source_commodity_t::on_update(sys::state& state) noexcept {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN dad_source_commodity::update
	commodity_overlay->show_tooltip = false;
	commodity_overlay->content = content;
// END
	remake_layout(state, true);
}
void production_dad_source_commodity_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "dad_button") {
					temp.ptr = dad_button.get();
				} else
				if(cname == "commodity_overlay") {
					temp.ptr = commodity_overlay.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_dad_source_commodity_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::dad_source_commodity"));
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
		if(child_data.name == "dad_button") {
			dad_button = std::make_unique<production_dad_source_commodity_dad_button_t>();
			dad_button->parent = this;
			auto cptr = dad_button.get();
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
		if(child_data.name == "commodity_overlay") {
			commodity_overlay = std::make_unique<alice_ui::legacy_commodity_icon>();
			commodity_overlay->parent = this;
			auto cptr = commodity_overlay.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
// BEGIN dad_source_commodity::create
// END
}
std::unique_ptr<ui::element_base> make_production_dad_source_commodity(sys::state& state) {
	auto ptr = std::make_unique<production_dad_source_commodity_t>();
	ptr->on_create(state);
	return ptr;
}
void production_directive_special_remove_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	production_directive_special_t& directive_special = *((production_directive_special_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_special::remove_button::tooltip
	if(directive_special.content == production_directives::factory_profit) {
		text::add_line(state, contents, "directive_description_fmoney");
	} else if(directive_special.content == production_directives::factory_employment) {
		text::add_line(state, contents, "directive_description_fpeople");
	} else if(directive_special.content == production_directives::factory_specialization) {
		text::add_line(state, contents, "directive_description_fspecial");
	} else if(directive_special.content == production_directives::factory_upgrades) {
		text::add_line(state, contents, "directive_description_fupgrade");
	} else if(directive_special.content == production_directives::no_factories) {
		text::add_line(state, contents, "directive_description_fnone");
	} else if(directive_special.content == production_directives::railroads) {
		text::add_line(state, contents, "directive_description_rail");
	} else if(directive_special.content == production_directives::naval_bases) {
		text::add_line(state, contents, "directive_description_naval");
	} else if(directive_special.content == production_directives::forts) {
		text::add_line(state, contents, "directive_description_forts");
	}
// END
}
void production_directive_special_remove_button_t::on_update(sys::state& state) noexcept {
	production_directive_special_t& directive_special = *((production_directive_special_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_special::remove_button::update
	static auto icon0 = template_project::icon_by_name(state.ui_templates, "factory_money.svg");
	static auto icon1 = template_project::icon_by_name(state.ui_templates, "factory_people.svg");
	static auto icon2 = template_project::icon_by_name(state.ui_templates, "factory_special.svg");
	static auto icon3 = template_project::icon_by_name(state.ui_templates, "factory_upgrade.svg");
	static auto icon4 = template_project::icon_by_name(state.ui_templates, "factory_none.svg");
	static auto icon5 = template_project::icon_by_name(state.ui_templates, "train_upgrade.svg");
	static auto icon6 = template_project::icon_by_name(state.ui_templates, "naval_base_upgrade.svg");
	static auto icon7 = template_project::icon_by_name(state.ui_templates, "fort_upgrade.svg");
	if(directive_special.content == production_directives::factory_profit) {
		icon = icon0;
	} else if(directive_special.content == production_directives::factory_employment) {
		icon = icon1;
	} else if(directive_special.content == production_directives::factory_specialization) {
		icon = icon2;
	} else if(directive_special.content == production_directives::factory_upgrades) {
		icon = icon3;
	} else if(directive_special.content == production_directives::no_factories) {
		icon = icon4;
	} else if(directive_special.content == production_directives::railroads) {
		icon = icon5;
	} else if(directive_special.content == production_directives::naval_bases) {
		icon = icon6;
	} else if(directive_special.content == production_directives::forts) {
		icon = icon7;
	}
// END
}
bool production_directive_special_remove_button_t::button_action(sys::state& state) noexcept {
	production_directive_special_t& directive_special = *((production_directive_special_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_special::remove_button::lbutton_action
	command::toggle_production_directive(state, state.local_player_nation, directive_special.for_state, directive_special.content);
// END
	return true;
}
ui::message_result production_directive_special_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_directive_special_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_directive_special_t::on_update(sys::state& state) noexcept {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_special::update
// END
	remake_layout(state, true);
}
void production_directive_special_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "remove_button") {
					temp.ptr = remove_button.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_directive_special_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::directive_special"));
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
		if(child_data.name == "remove_button") {
			remove_button = std::make_unique<production_directive_special_remove_button_t>();
			remove_button->parent = this;
			auto cptr = remove_button.get();
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
// BEGIN directive_special::create
// END
}
std::unique_ptr<ui::element_base> make_production_directive_special(sys::state& state) {
	auto ptr = std::make_unique<production_directive_special_t>();
	ptr->on_create(state);
	return ptr;
}
void production_directive_commodity_remove_button_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	production_directive_commodity_t& directive_commodity = *((production_directive_commodity_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_commodity::remove_button::tooltip
	text::add_line(state, contents, "directive_description_commodity", text::variable_type::x, state.world.commodity_get_name(directive_commodity.content));
// END
}
void production_directive_commodity_remove_button_t::on_update(sys::state& state) noexcept {
	production_directive_commodity_t& directive_commodity = *((production_directive_commodity_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_commodity::remove_button::update
// END
}
bool production_directive_commodity_remove_button_t::button_action(sys::state& state) noexcept {
	production_directive_commodity_t& directive_commodity = *((production_directive_commodity_t*)(parent)); 
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_commodity::remove_button::lbutton_action
	command::toggle_production_directive(state, state.local_player_nation, directive_commodity.for_state, production_directives::to_key(state, directive_commodity.content));
// END
	return true;
}
ui::message_result production_directive_commodity_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_directive_commodity_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_directive_commodity_t::on_update(sys::state& state) noexcept {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent->parent)); 
// BEGIN directive_commodity::update
	commodity_overlay->content = content;
	commodity_overlay->show_tooltip = false;
// END
	remake_layout(state, true);
}
void production_directive_commodity_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "remove_button") {
					temp.ptr = remove_button.get();
				} else
				if(cname == "commodity_overlay") {
					temp.ptr = commodity_overlay.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
void production_directive_commodity_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::directive_commodity"));
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
		if(child_data.name == "remove_button") {
			remove_button = std::make_unique<production_directive_commodity_remove_button_t>();
			remove_button->parent = this;
			auto cptr = remove_button.get();
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
		if(child_data.name == "commodity_overlay") {
			commodity_overlay = std::make_unique<alice_ui::legacy_commodity_icon>();
			commodity_overlay->parent = this;
			auto cptr = commodity_overlay.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
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
// BEGIN directive_commodity::create
// END
}
std::unique_ptr<ui::element_base> make_production_directive_commodity(sys::state& state) {
	auto ptr = std::make_unique<production_directive_commodity_t>();
	ptr->on_create(state);
	return ptr;
}
void production_directives_window_all_directives_t::add_dad_source_commodity(dcon::commodity_id content) {
	values.emplace_back(dad_source_commodity_option{content});
}
void production_directives_window_all_directives_t::add_dad_source_special(dcon::production_directive_id content) {
	values.emplace_back(dad_source_special_option{content});
}
void  production_directives_window_all_directives_t::on_create(sys::state& state, layout_window_element* parent) {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent)); 
// BEGIN directives_window::all_directives::on_create
	for(uint32_t i = 0; i < production_directives::count_special_keys; ++i) {
		add_dad_source_special(dcon::production_directive_id{ dcon::production_directive_id::value_base_t(i) });
	}
	std::vector<dcon::commodity_id> ctemp;
	for(auto c : state.world.in_commodity) {
		if(!c.get_money_rgo() ) {
			for(auto t : state.world.in_factory_type) {
				if(t.get_output() == c) {
					ctemp.push_back(c.id);
					break;
				}
			}
		}
	}
	std::sort(ctemp.begin(), ctemp.end(), [&](auto a, auto b) {
		return text::produce_simple_string(state, state.world.commodity_get_name(a)) < text::produce_simple_string(state, state.world.commodity_get_name(b));
	});
	for(auto c : ctemp) {
		add_dad_source_commodity(c);
	}
// END
}
void  production_directives_window_all_directives_t::update(sys::state& state, layout_window_element* parent) {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent)); 
// BEGIN directives_window::all_directives::update
// END
}
measure_result  production_directives_window_all_directives_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<dad_source_commodity_option>(values[index])) {
		if(dad_source_commodity_pool.empty()) dad_source_commodity_pool.emplace_back(make_production_dad_source_commodity(state));
		if(destination) {
			if(dad_source_commodity_pool.size() <= size_t(dad_source_commodity_pool_used)) dad_source_commodity_pool.emplace_back(make_production_dad_source_commodity(state));
			dad_source_commodity_pool[dad_source_commodity_pool_used]->base_data.position.x = int16_t(x);
			dad_source_commodity_pool[dad_source_commodity_pool_used]->base_data.position.y = int16_t(y);
			dad_source_commodity_pool[dad_source_commodity_pool_used]->parent = destination;
			destination->children.push_back(dad_source_commodity_pool[dad_source_commodity_pool_used].get());
			((production_dad_source_commodity_t*)(dad_source_commodity_pool[dad_source_commodity_pool_used].get()))->content = std::get<dad_source_commodity_option>(values[index]).content;
			dad_source_commodity_pool[dad_source_commodity_pool_used]->impl_on_update(state);
			dad_source_commodity_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ dad_source_commodity_pool[0]->base_data.size.x, dad_source_commodity_pool[0]->base_data.size.y + 2, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<dad_source_special_option>(values[index])) {
		if(dad_source_special_pool.empty()) dad_source_special_pool.emplace_back(make_production_dad_source_special(state));
		if(destination) {
			if(dad_source_special_pool.size() <= size_t(dad_source_special_pool_used)) dad_source_special_pool.emplace_back(make_production_dad_source_special(state));
			dad_source_special_pool[dad_source_special_pool_used]->base_data.position.x = int16_t(x);
			dad_source_special_pool[dad_source_special_pool_used]->base_data.position.y = int16_t(y);
			dad_source_special_pool[dad_source_special_pool_used]->parent = destination;
			destination->children.push_back(dad_source_special_pool[dad_source_special_pool_used].get());
			((production_dad_source_special_t*)(dad_source_special_pool[dad_source_special_pool_used].get()))->content = std::get<dad_source_special_option>(values[index]).content;
			dad_source_special_pool[dad_source_special_pool_used]->impl_on_update(state);
			dad_source_special_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ dad_source_special_pool[0]->base_data.size.x, dad_source_special_pool[0]->base_data.size.y + 2, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  production_directives_window_all_directives_t::reset_pools() {
	dad_source_commodity_pool_used = 0;
	dad_source_special_pool_used = 0;
}
void production_directives_window_active_directives_t::add_directive_special(dcon::production_directive_id content, dcon::state_instance_id for_state) {
	values.emplace_back(directive_special_option{content, for_state});
}
void production_directives_window_active_directives_t::add_directive_commodity(dcon::commodity_id content, dcon::state_instance_id for_state) {
	values.emplace_back(directive_commodity_option{content, for_state});
}
void  production_directives_window_active_directives_t::on_create(sys::state& state, layout_window_element* parent) {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent)); 
// BEGIN directives_window::active_directives::on_create
// END
}
void  production_directives_window_active_directives_t::update(sys::state& state, layout_window_element* parent) {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent)); 
// BEGIN directives_window::active_directives::update
	values.clear();

	for(uint32_t i = 0; i < production_directives::count_special_keys; ++i) {
		if(state.world.nation_get_production_directive(state.local_player_nation, dcon::production_directive_id{ dcon::production_directive_id::value_base_t(i) })) {
			add_directive_special(dcon::production_directive_id{ dcon::production_directive_id::value_base_t(i) }, dcon::state_instance_id{});
		}
	}
	std::vector<dcon::commodity_id> ctemp;
	for(auto c : state.world.in_commodity) {
		if(!c.get_money_rgo() && state.world.nation_get_production_directive(state.local_player_nation, production_directives::to_key(state, c))) {
			for(auto t : state.world.in_factory_type) {
				if(t.get_output() == c) {
					ctemp.push_back(c.id);
					break;
				}
			}
		}
	}
	std::sort(ctemp.begin(), ctemp.end(), [&](auto a, auto b) {
		return text::produce_simple_string(state, state.world.commodity_get_name(a)) < text::produce_simple_string(state, state.world.commodity_get_name(b));
	});
	for(auto c : ctemp) {
		add_directive_commodity(c, dcon::state_instance_id{});
	}
// END
}
measure_result  production_directives_window_active_directives_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<directive_special_option>(values[index])) {
		if(directive_special_pool.empty()) directive_special_pool.emplace_back(make_production_directive_special(state));
		if(destination) {
			if(directive_special_pool.size() <= size_t(directive_special_pool_used)) directive_special_pool.emplace_back(make_production_directive_special(state));
			directive_special_pool[directive_special_pool_used]->base_data.position.x = int16_t(x);
			directive_special_pool[directive_special_pool_used]->base_data.position.y = int16_t(y);
			directive_special_pool[directive_special_pool_used]->parent = destination;
			destination->children.push_back(directive_special_pool[directive_special_pool_used].get());
			((production_directive_special_t*)(directive_special_pool[directive_special_pool_used].get()))->content = std::get<directive_special_option>(values[index]).content;
			((production_directive_special_t*)(directive_special_pool[directive_special_pool_used].get()))->for_state = std::get<directive_special_option>(values[index]).for_state;
			directive_special_pool[directive_special_pool_used]->impl_on_update(state);
			directive_special_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ directive_special_pool[0]->base_data.size.x, directive_special_pool[0]->base_data.size.y + 2, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<directive_commodity_option>(values[index])) {
		if(directive_commodity_pool.empty()) directive_commodity_pool.emplace_back(make_production_directive_commodity(state));
		if(destination) {
			if(directive_commodity_pool.size() <= size_t(directive_commodity_pool_used)) directive_commodity_pool.emplace_back(make_production_directive_commodity(state));
			directive_commodity_pool[directive_commodity_pool_used]->base_data.position.x = int16_t(x);
			directive_commodity_pool[directive_commodity_pool_used]->base_data.position.y = int16_t(y);
			directive_commodity_pool[directive_commodity_pool_used]->parent = destination;
			destination->children.push_back(directive_commodity_pool[directive_commodity_pool_used].get());
			((production_directive_commodity_t*)(directive_commodity_pool[directive_commodity_pool_used].get()))->content = std::get<directive_commodity_option>(values[index]).content;
			((production_directive_commodity_t*)(directive_commodity_pool[directive_commodity_pool_used].get()))->for_state = std::get<directive_commodity_option>(values[index]).for_state;
			directive_commodity_pool[directive_commodity_pool_used]->impl_on_update(state);
			directive_commodity_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ directive_commodity_pool[0]->base_data.size.x, directive_commodity_pool[0]->base_data.size.y + 2, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  production_directives_window_active_directives_t::reset_pools() {
	directive_special_pool_used = 0;
	directive_commodity_pool_used = 0;
}
bool production_directives_window_directives_drop_target_t::recieve_drag_and_drop(sys::state& state, std::any& data, ui::drag_and_drop_data data_type, ui::drag_and_drop_target sub_target, bool shift_held_down) noexcept  {
	production_directives_window_t& directives_window = *((production_directives_window_t*)(parent)); 
// BEGIN directives_window::directives_drop_target::recieve
	auto dir = std::any_cast<dcon::production_directive_id>(data);
	if(!state.world.nation_get_production_directive(state.local_player_nation, dir)) {
		command::toggle_production_directive(state, state.local_player_nation, dcon::state_instance_id{}, dir);
	}
	if(shift_held_down)
		return false;
// END
	return true;
}
ui::message_result production_directives_window_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_directives_window_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_directives_window_t::on_update(sys::state& state) noexcept {
// BEGIN directives_window::update
// END
	all_directives.update(state, this);
	active_directives.update(state, this);
	remake_layout(state, true);
}
void production_directives_window_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "header") {
					temp.ptr = header.get();
				} else
				if(cname == "general_explain_tt") {
					temp.ptr = general_explain_tt.get();
				} else
				if(cname == "national_directives_header") {
					temp.ptr = national_directives_header.get();
				} else
				if(cname == "national_directives_tt") {
					temp.ptr = national_directives_tt.get();
				} else
				if(cname == "directives_drop_target") {
					temp.ptr = directives_drop_target.get();
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
					temp.ptr = make_production_main(state);
				}
				if(cname == "primary_row") {
					temp.ptr = make_production_primary_row(state);
				}
				if(cname == "sub_item_row") {
					temp.ptr = make_production_sub_item_row(state);
				}
				if(cname == "category_header") {
					temp.ptr = make_production_category_header(state);
				}
				if(cname == "small_divider") {
					temp.ptr = make_production_small_divider(state);
				}
				if(cname == "category_option") {
					temp.ptr = make_production_category_option(state);
				}
				if(cname == "location_option") {
					temp.ptr = make_production_location_option(state);
				}
				if(cname == "filter_option") {
					temp.ptr = make_production_filter_option(state);
				}
				if(cname == "max_item") {
					temp.ptr = make_production_max_item(state);
				}
				if(cname == "shortage_item") {
					temp.ptr = make_production_shortage_item(state);
				}
				if(cname == "current_specialization_row") {
					temp.ptr = make_production_current_specialization_row(state);
				}
				if(cname == "specialization_priority_row") {
					temp.ptr = make_production_specialization_priority_row(state);
				}
				if(cname == "spec_rankings_popup") {
					temp.ptr = make_production_spec_rankings_popup(state);
				}
				if(cname == "spec_rankings_item") {
					temp.ptr = make_production_spec_rankings_item(state);
				}
				if(cname == "rh_state_item") {
					temp.ptr = make_production_rh_state_item(state);
				}
				if(cname == "dad_source_special") {
					temp.ptr = make_production_dad_source_special(state);
				}
				if(cname == "dad_source_commodity") {
					temp.ptr = make_production_dad_source_commodity(state);
				}
				if(cname == "directive_special") {
					temp.ptr = make_production_directive_special(state);
				}
				if(cname == "directive_commodity") {
					temp.ptr = make_production_directive_commodity(state);
				}
				if(cname == "directives_window") {
					temp.ptr = make_production_directives_window(state);
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
				if(cname == "all_directives") {
					temp.generator = &all_directives;
				}
				if(cname == "active_directives") {
					temp.generator = &active_directives;
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
void production_directives_window_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::directives_window"));
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
		if(child_data.name == "header") {
			header = std::make_unique<template_label>();
			header->parent = this;
			auto cptr = header.get();
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
		if(child_data.name == "general_explain_tt") {
			general_explain_tt = std::make_unique<template_icon_graphic>();
			general_explain_tt->parent = this;
			auto cptr = general_explain_tt.get();
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
		if(child_data.name == "national_directives_header") {
			national_directives_header = std::make_unique<template_label>();
			national_directives_header->parent = this;
			auto cptr = national_directives_header.get();
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
		if(child_data.name == "national_directives_tt") {
			national_directives_tt = std::make_unique<template_icon_graphic>();
			national_directives_tt->parent = this;
			auto cptr = national_directives_tt.get();
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
		if(child_data.name == "directives_drop_target") {
			directives_drop_target = std::make_unique<production_directives_window_directives_drop_target_t>();
			directives_drop_target->parent = this;
			auto cptr = directives_drop_target.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->supported_data_type = ui::drag_and_drop_data::production_directive;
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
	all_directives.on_create(state, this);
	active_directives.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN directives_window::create
// END
}
std::unique_ptr<ui::element_base> make_production_directives_window(sys::state& state) {
	auto ptr = std::make_unique<production_directives_window_t>();
	ptr->on_create(state);
	return ptr;
}
// LOST-CODE
// BEGIN shortage_item::amount::update
//	set_text(state, text::format_percentage(shortage_item.amount, 0));
// END
// BEGIN primary_row::size::update
//	switch(primary_row.about) {
//	case prod_source::combined:
//		set_text(state, "");
//		break;
//	case prod_source::factory:
//		switch(primary_row.category_type) {
//		case prod_category::production:
//			[[fallthrough]];
//		case prod_category::consumption:
//			[[fallthrough]];
//		case prod_category::employment:
//		{
//			float total = 0.0f;
//			if(main.selected_location) {
//				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
//					for(auto fac : state.world.province_get_factory_location(p)) {
//						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
//							total += economy::get_factory_level(state, fac.get_factory());
//						}
//					}
//				});
//			} else {
//				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
//					for(auto fac : op.get_province().get_factory_location()) {
//						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
//							total += economy::get_factory_level(state, fac.get_factory());
//						}
//					}
//				}
//			}
//			set_text(state, text::format_float(total, 1));
//		} break;
//		case prod_category::details:
//			set_text(state, "");
//			break;
//		}
//		break;
//	case prod_source::artisan:
//		switch(primary_row.category_type) {
//		case prod_category::production:
//			[[fallthrough]];
//		case prod_category::consumption:
//			[[fallthrough]];
//		case prod_category::employment:
//		{
//			float total = 0.0f;
//			if(main.selected_location) {
//				auto artisan_count = state.world.state_instance_get_demographics(main.selected_location, demographics::to_key(state, state.culture_definitions.artisans));
//				total = artisan_count / economy::artisans_per_employment_unit;
//			} else {
//				auto artisan_count = state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, state.culture_definitions.artisans));
//				total = artisan_count / economy::artisans_per_employment_unit;
//			}
//			set_text(state, text::format_float(total, 1));
//		} break;
//		case prod_category::details:
//			set_text(state, "");
//			break;
//		} break;
//	case prod_source::rgo:
//		switch(primary_row.category_type) {
//		case prod_category::production:
//			[[fallthrough]];
//		case prod_category::consumption:
//			[[fallthrough]];
//		case prod_category::employment:
//		{
//			float total = 0.0f;
//			if(main.selected_location) {
//				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
//					total += state.world.province_get_rgo_size(p, primary_row.commodity_type) / economy::artisans_per_employment_unit;;
//				});
//			} else {
//				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
//					total += op.get_province().get_rgo_size(primary_row.commodity_type) / economy::artisans_per_employment_unit;
//				}
//			}
//			set_text(state, text::format_float(total, 1));
//		} break;
//		case prod_category::details:
//			set_text(state, "");
//			break;
//		}
//		break;
//	default:
//		set_text(state, "");
//		break;
//	}
// END
}
