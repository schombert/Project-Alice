// BEGIN prelude
#include "alice_ui.hpp"
#include "system_state.hpp"
#include "gui_deserialize.hpp"
#include "lua_alice_api.hpp"
#include "economy_production.hpp"
#include "labour_details.hpp"

namespace alice_ui {
enum class prod_category : uint8_t {
	production, consumption, employment, details
};
enum class prod_source : uint8_t {
	combined, factory, artisan, rgo
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
}

// END

namespace alice_ui {
struct production_main_view_dropdown_t;
struct production_main_location_dropdown_t;
struct production_main_filter_dropdown_t;
struct production_main_combine_toggle_t;
struct production_main_t;
struct production_production_table_header_t;
struct production_primary_row_main_commodity_icon_t;
struct production_primary_row_name_t;
struct production_primary_row_size_t;
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
	std::vector<std::unique_ptr<ui::element_base>> production_table_header_pool;
	int32_t production_table_header_pool_used = 0;
	std::vector<std::variant<std::monostate, primary_row_option, sub_item_row_option, category_header_option, small_divider_option>> values;
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
struct production_primary_row_size_t : public alice_ui::template_label {
// BEGIN primary_row::size::variables
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
struct production_production_table_header_t : public layout_window_element {
// BEGIN production_table_header::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> Control2;
	std::unique_ptr<template_label> Control3;
	std::unique_ptr<template_label> Control4;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void set_alternate(bool alt) noexcept;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_production_production_table_header(sys::state& state);
struct production_primary_row_t : public layout_window_element {
// BEGIN primary_row::variables
// END
	dcon::commodity_id commodity_type;
	prod_category category_type;
	prod_source about;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<production_primary_row_main_commodity_icon_t> main_commodity_icon;
	std::unique_ptr<production_primary_row_name_t> name;
	std::unique_ptr<production_primary_row_size_t> size;
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
					add_sub_item_row(i.c, economy::labor::basic_education, prod_category::employment, false, prod_source::factory);
					add_sub_item_row(i.c, economy::labor::high_education, prod_category::employment, false, prod_source::factory);
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
		if(production_table_header_pool.empty()) production_table_header_pool.emplace_back(make_production_production_table_header(state));
		if(primary_row_pool.empty()) primary_row_pool.emplace_back(make_production_primary_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<primary_row_option>(values[index - 1]) && !std::holds_alternative<sub_item_row_option>(values[index - 1]) && !std::holds_alternative<category_header_option>(values[index - 1]) && !std::holds_alternative<small_divider_option>(values[index - 1]))) {
			if(destination) {
				if(production_table_header_pool.size() <= size_t(production_table_header_pool_used)) production_table_header_pool.emplace_back(make_production_production_table_header(state));
				if(primary_row_pool.size() <= size_t(primary_row_pool_used)) primary_row_pool.emplace_back(make_production_primary_row(state));
				production_table_header_pool[production_table_header_pool_used]->base_data.position.x = int16_t(x);
				production_table_header_pool[production_table_header_pool_used]->base_data.position.y = int16_t(y);
				if(!production_table_header_pool[production_table_header_pool_used]->parent) {
					production_table_header_pool[production_table_header_pool_used]->parent = destination;
					production_table_header_pool[production_table_header_pool_used]->impl_on_update(state);
					production_table_header_pool[production_table_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(production_table_header_pool[production_table_header_pool_used].get());
			((production_production_table_header_t*)(production_table_header_pool[production_table_header_pool_used].get()))->set_alternate(alternate);
				primary_row_pool[primary_row_pool_used]->base_data.position.x = int16_t(x);
				primary_row_pool[primary_row_pool_used]->base_data.position.y = int16_t(y +  production_table_header_pool[0]->base_data.size.y + 0);
				primary_row_pool[primary_row_pool_used]->parent = destination;
				destination->children.push_back(primary_row_pool[primary_row_pool_used].get());
				((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->commodity_type = std::get<primary_row_option>(values[index]).commodity_type;
				((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->category_type = std::get<primary_row_option>(values[index]).category_type;
				((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->about = std::get<primary_row_option>(values[index]).about;
			((production_primary_row_t*)(primary_row_pool[primary_row_pool_used].get()))->set_alternate(!alternate);
				primary_row_pool[primary_row_pool_used]->impl_on_update(state);
				production_table_header_pool_used++;
				primary_row_pool_used++;
			}
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<primary_row_option>(values[index + 1])|| std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<category_header_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1]));
			return measure_result{std::max(production_table_header_pool[0]->base_data.size.x, primary_row_pool[0]->base_data.size.x), production_table_header_pool[0]->base_data.size.y + primary_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
		}
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
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<primary_row_option>(values[index + 1])|| std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<category_header_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1]));
		return measure_result{ primary_row_pool[0]->base_data.size.x, primary_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<sub_item_row_option>(values[index])) {
		if(production_table_header_pool.empty()) production_table_header_pool.emplace_back(make_production_production_table_header(state));
		if(sub_item_row_pool.empty()) sub_item_row_pool.emplace_back(make_production_sub_item_row(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<primary_row_option>(values[index - 1]) && !std::holds_alternative<sub_item_row_option>(values[index - 1]) && !std::holds_alternative<category_header_option>(values[index - 1]) && !std::holds_alternative<small_divider_option>(values[index - 1]))) {
			if(destination) {
				if(production_table_header_pool.size() <= size_t(production_table_header_pool_used)) production_table_header_pool.emplace_back(make_production_production_table_header(state));
				if(sub_item_row_pool.size() <= size_t(sub_item_row_pool_used)) sub_item_row_pool.emplace_back(make_production_sub_item_row(state));
				production_table_header_pool[production_table_header_pool_used]->base_data.position.x = int16_t(x);
				production_table_header_pool[production_table_header_pool_used]->base_data.position.y = int16_t(y);
				if(!production_table_header_pool[production_table_header_pool_used]->parent) {
					production_table_header_pool[production_table_header_pool_used]->parent = destination;
					production_table_header_pool[production_table_header_pool_used]->impl_on_update(state);
					production_table_header_pool[production_table_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(production_table_header_pool[production_table_header_pool_used].get());
			((production_production_table_header_t*)(production_table_header_pool[production_table_header_pool_used].get()))->set_alternate(alternate);
				sub_item_row_pool[sub_item_row_pool_used]->base_data.position.x = int16_t(x);
				sub_item_row_pool[sub_item_row_pool_used]->base_data.position.y = int16_t(y +  production_table_header_pool[0]->base_data.size.y + 0);
				sub_item_row_pool[sub_item_row_pool_used]->parent = destination;
				destination->children.push_back(sub_item_row_pool[sub_item_row_pool_used].get());
				((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->main_commodity = std::get<sub_item_row_option>(values[index]).main_commodity;
				((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->secondary_type = std::get<sub_item_row_option>(values[index]).secondary_type;
				((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->category_type = std::get<sub_item_row_option>(values[index]).category_type;
				((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->is_first_in_subsection = std::get<sub_item_row_option>(values[index]).is_first_in_subsection;
				((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->about = std::get<sub_item_row_option>(values[index]).about;
			((production_sub_item_row_t*)(sub_item_row_pool[sub_item_row_pool_used].get()))->set_alternate(!alternate);
				sub_item_row_pool[sub_item_row_pool_used]->impl_on_update(state);
				production_table_header_pool_used++;
				sub_item_row_pool_used++;
			}
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1]));
			return measure_result{std::max(production_table_header_pool[0]->base_data.size.x, sub_item_row_pool[0]->base_data.size.x), production_table_header_pool[0]->base_data.size.y + sub_item_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
		}
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
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1]));
		return measure_result{ sub_item_row_pool[0]->base_data.size.x, sub_item_row_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<category_header_option>(values[index])) {
		if(production_table_header_pool.empty()) production_table_header_pool.emplace_back(make_production_production_table_header(state));
		if(category_header_pool.empty()) category_header_pool.emplace_back(make_production_category_header(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<primary_row_option>(values[index - 1]) && !std::holds_alternative<sub_item_row_option>(values[index - 1]) && !std::holds_alternative<category_header_option>(values[index - 1]) && !std::holds_alternative<small_divider_option>(values[index - 1]))) {
			if(destination) {
				if(production_table_header_pool.size() <= size_t(production_table_header_pool_used)) production_table_header_pool.emplace_back(make_production_production_table_header(state));
				if(category_header_pool.size() <= size_t(category_header_pool_used)) category_header_pool.emplace_back(make_production_category_header(state));
				production_table_header_pool[production_table_header_pool_used]->base_data.position.x = int16_t(x);
				production_table_header_pool[production_table_header_pool_used]->base_data.position.y = int16_t(y);
				if(!production_table_header_pool[production_table_header_pool_used]->parent) {
					production_table_header_pool[production_table_header_pool_used]->parent = destination;
					production_table_header_pool[production_table_header_pool_used]->impl_on_update(state);
					production_table_header_pool[production_table_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(production_table_header_pool[production_table_header_pool_used].get());
			((production_production_table_header_t*)(production_table_header_pool[production_table_header_pool_used].get()))->set_alternate(alternate);
				category_header_pool[category_header_pool_used]->base_data.position.x = int16_t(x);
				category_header_pool[category_header_pool_used]->base_data.position.y = int16_t(y +  production_table_header_pool[0]->base_data.size.y + 0);
				category_header_pool[category_header_pool_used]->parent = destination;
				destination->children.push_back(category_header_pool[category_header_pool_used].get());
				((production_category_header_t*)(category_header_pool[category_header_pool_used].get()))->category = std::get<category_header_option>(values[index]).category;
				category_header_pool[category_header_pool_used]->impl_on_update(state);
				production_table_header_pool_used++;
				category_header_pool_used++;
			}
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<primary_row_option>(values[index + 1])|| std::holds_alternative<category_header_option>(values[index + 1]));
			return measure_result{std::max(production_table_header_pool[0]->base_data.size.x, category_header_pool[0]->base_data.size.x), production_table_header_pool[0]->base_data.size.y + category_header_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::no_break};
		}
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
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<primary_row_option>(values[index + 1])|| std::holds_alternative<category_header_option>(values[index + 1]));
		return measure_result{ category_header_pool[0]->base_data.size.x, category_header_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::no_break};
	}
	if(std::holds_alternative<small_divider_option>(values[index])) {
		if(production_table_header_pool.empty()) production_table_header_pool.emplace_back(make_production_production_table_header(state));
		if(small_divider_pool.empty()) small_divider_pool.emplace_back(make_production_small_divider(state));
		if(index == 0 || first_in_section || (true && !std::holds_alternative<primary_row_option>(values[index - 1]) && !std::holds_alternative<sub_item_row_option>(values[index - 1]) && !std::holds_alternative<category_header_option>(values[index - 1]) && !std::holds_alternative<small_divider_option>(values[index - 1]))) {
			if(destination) {
				if(production_table_header_pool.size() <= size_t(production_table_header_pool_used)) production_table_header_pool.emplace_back(make_production_production_table_header(state));
				if(small_divider_pool.size() <= size_t(small_divider_pool_used)) small_divider_pool.emplace_back(make_production_small_divider(state));
				production_table_header_pool[production_table_header_pool_used]->base_data.position.x = int16_t(x);
				production_table_header_pool[production_table_header_pool_used]->base_data.position.y = int16_t(y);
				if(!production_table_header_pool[production_table_header_pool_used]->parent) {
					production_table_header_pool[production_table_header_pool_used]->parent = destination;
					production_table_header_pool[production_table_header_pool_used]->impl_on_update(state);
					production_table_header_pool[production_table_header_pool_used]->impl_on_reset_text(state);
				}
				destination->children.push_back(production_table_header_pool[production_table_header_pool_used].get());
			((production_production_table_header_t*)(production_table_header_pool[production_table_header_pool_used].get()))->set_alternate(alternate);
				small_divider_pool[small_divider_pool_used]->base_data.position.x = int16_t(x);
				small_divider_pool[small_divider_pool_used]->base_data.position.y = int16_t(y +  production_table_header_pool[0]->base_data.size.y + 0);
				small_divider_pool[small_divider_pool_used]->parent = destination;
				destination->children.push_back(small_divider_pool[small_divider_pool_used].get());
				small_divider_pool[small_divider_pool_used]->impl_on_update(state);
				production_table_header_pool_used++;
				small_divider_pool_used++;
			}
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1]));
			return measure_result{std::max(production_table_header_pool[0]->base_data.size.x, small_divider_pool[0]->base_data.size.x), production_table_header_pool[0]->base_data.size.y + small_divider_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
		}
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
	 	 	bool stick_to_next = (index + 1) < values.size() && (false || std::holds_alternative<sub_item_row_option>(values[index + 1])|| std::holds_alternative<small_divider_option>(values[index + 1]));
		return measure_result{ small_divider_pool[0]->base_data.size.x, small_divider_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  production_main_main_list_t::reset_pools() {
	production_table_header_pool_used = 0;
	primary_row_pool_used = 0;
	production_table_header_pool_used = 0;
	sub_item_row_pool_used = 0;
	production_table_header_pool_used = 0;
	category_header_pool_used = 0;
	production_table_header_pool_used = 0;
	small_divider_pool_used = 0;
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
	bool made_selection = false;
	for(auto os : state.world.nation_get_state_ownership(state.local_player_nation)) {
		add_item(os.get_state());
		if(os.get_state() == main.selected_location) {
			quiet_on_selection(state, total_items - 1);
			made_selection = true;
		}
	}
	if(!made_selection) {
		quiet_on_selection(state, 0);
		if(main.selected_location) {
			main.selected_location = dcon::state_instance_id{};
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
void  production_production_table_header_t::set_alternate(bool alt) noexcept {
	window_template = alt ? 3 : 4;
}
ui::message_result production_production_table_header_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result production_production_table_header_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void production_production_table_header_t::on_update(sys::state& state) noexcept {
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN production_table_header::update
// END
	remake_layout(state, true);
}
void production_production_table_header_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
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
				if(cname == "Control2") {
					temp.ptr = Control2.get();
				} else
				if(cname == "Control3") {
					temp.ptr = Control3.get();
				} else
				if(cname == "Control4") {
					temp.ptr = Control4.get();
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
void production_production_table_header_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("production::production_table_header"));
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
		if(child_data.name == "Control2") {
			Control2 = std::make_unique<template_label>();
			Control2->parent = this;
			auto cptr = Control2.get();
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
		if(child_data.name == "Control3") {
			Control3 = std::make_unique<template_label>();
			Control3->parent = this;
			auto cptr = Control3.get();
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
		if(child_data.name == "Control4") {
			Control4 = std::make_unique<template_label>();
			Control4->parent = this;
			auto cptr = Control4.get();
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
// BEGIN production_table_header::create
// END
}
std::unique_ptr<ui::element_base> make_production_production_table_header(sys::state& state) {
	auto ptr = std::make_unique<production_production_table_header_t>();
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
void production_primary_row_size_t::on_update(sys::state& state) noexcept {
	production_primary_row_t& primary_row = *((production_primary_row_t*)(parent)); 
	production_main_t& main = *((production_main_t*)(parent->parent)); 
// BEGIN primary_row::size::update
	switch(primary_row.about) {
	case prod_source::combined:
		set_text(state, "");
		break;
	case prod_source::factory:
		switch(primary_row.category_type) {
		case prod_category::production:
			[[fallthrough]];
		case prod_category::consumption:
			[[fallthrough]];
		case prod_category::employment:
		{
			float total = 0.0f;
			if(main.selected_location) {
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					for(auto fac : state.world.province_get_factory_location(p)) {
						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
							total += economy::get_factory_level(state, fac.get_factory());
						}
					}
				});
			} else {
				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
					for(auto fac : op.get_province().get_factory_location()) {
						if(fac.get_factory().get_building_type().get_output() == primary_row.commodity_type) {
							total += economy::get_factory_level(state, fac.get_factory());
						}
					}
				}
			}
			set_text(state, text::format_float(total, 1));
		} break;
		case prod_category::details:
			set_text(state, "");
			break;
		}
		break;
	case prod_source::artisan:
		switch(primary_row.category_type) {
		case prod_category::production:
			[[fallthrough]];
		case prod_category::consumption:
			[[fallthrough]];
		case prod_category::employment:
		{
			float total = 0.0f;
			if(main.selected_location) {
				auto artisan_count = state.world.state_instance_get_demographics(main.selected_location, demographics::to_key(state, state.culture_definitions.artisans));
				total = artisan_count / economy::artisans_per_employment_unit;
			} else {
				auto artisan_count = state.world.nation_get_demographics(state.local_player_nation, demographics::to_key(state, state.culture_definitions.artisans));
				total = artisan_count / economy::artisans_per_employment_unit;
			}
			set_text(state, text::format_float(total, 1));
		} break;
		case prod_category::details:
			set_text(state, "");
			break;
		} break;
	case prod_source::rgo:
		switch(primary_row.category_type) {
		case prod_category::production:
			[[fallthrough]];
		case prod_category::consumption:
			[[fallthrough]];
		case prod_category::employment:
		{
			float total = 0.0f;
			if(main.selected_location) {
				province::for_each_province_in_state_instance(state, main.selected_location, [&](dcon::province_id p) {
					total += state.world.province_get_rgo_size(p, primary_row.commodity_type) / economy::artisans_per_employment_unit;;
				});
			} else {
				for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
					total += op.get_province().get_rgo_size(primary_row.commodity_type) / economy::artisans_per_employment_unit;
				}
			}
			set_text(state, text::format_float(total, 1));
		} break;
		case prod_category::details:
			set_text(state, "");
			break;
		}
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
				if(cname == "size") {
					temp.ptr = size.get();
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
		if(child_data.name == "size") {
			size = std::make_unique<production_primary_row_size_t>();
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
				if(cname == "production_table_header") {
					temp.ptr = make_production_production_table_header(state);
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
// LOST-CODE
}
